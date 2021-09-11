/*
 * i2c_task.c
 *
 *  Created on: 5 May 2021
 *      Author: tamas
 */
#include "LSM6DS3_ACC_GYRO_Driver.h"
#include "cmsis_os.h"
#include "FreeRTOS.h"
#include "MadgwickAHRS.h"
#include "queue.h"
#include "i2c.h"
#include "math.h"
#include "stdbool.h"
#include "shared_data.h"
#include "frame_state_object.h"
#include "logging.h"

#define IMU_TASK_QUEUE_LENGTH (30U)
#define IMU_MEAS_AX_NUM (3U)

typedef enum {
	IMU_TASK_new_acc_meas,
	IMU_TASK_new_gyro_meas,
	IMU_TASK_invalid
} IMU_TASK_command_t;

static uint8_t IMU_TASK_queue_container[IMU_TASK_QUEUE_LENGTH * sizeof(IMU_TASK_command_t)] = {0};
static StaticQueue_t IMU_TASK_xStaticQueue;
I2C_HandleTypeDef hi2c1;
static QueueHandle_t IMU_task_queue = NULL;
static bool IMU_TASK_acc_data_received = false;
static bool IMU_TASK_gyro_data_received = false;
static bool IMU_TASK_message_dropped = false;
void dik() {
	volatile uint8_t ouch = 1;
	++ouch;
    LogError("IMU message failed to transmit/receive");
}

void IMU_int1_handler(BaseType_t * higher_prio_task_woken){
	IMU_TASK_command_t temp_command = IMU_TASK_new_acc_meas;
	if (IMU_task_queue != NULL) {
		if (xQueueSendFromISR(IMU_task_queue, &temp_command, higher_prio_task_woken) != pdPASS) {
		  IMU_TASK_message_dropped = true;
		}
	}
}

void IMU_int2_handler(BaseType_t * higher_prio_task_woken){
	IMU_TASK_command_t temp_command = IMU_TASK_new_gyro_meas;
	if (IMU_task_queue != NULL) {
		if (xQueueSendFromISR(IMU_task_queue, &temp_command, higher_prio_task_woken) != pdPASS) {
		    IMU_TASK_message_dropped = true;
		}
	}
}
static float raw_gyro_to_radps(int16_t raw_gyro);
static float raw_gyro_to_radps(int16_t raw_gyro)
{
	return (float)raw_gyro / (float)INT16_MAX * 245.0 / 180.0 * M_PI;
}

static float raw_acc_to_g(int16_t raw_acc);
static float raw_acc_to_g(int16_t raw_acc)
{
	return (float)raw_acc / (float)INT16_MAX * 2.0f;
}

static uint8_t axis_to_idx(FRAME_axis_t axis);
static uint8_t axis_to_idx(FRAME_axis_t axis) {
	uint8_t idx = 0;
	switch (axis) {
	case FRAME_roll:
		idx = 0;
	break;

	case FRAME_pitch:
		idx = 1;
	break;

	case FRAME_yaw:
		idx = 2;
	break;
	default:
	break;
	}
	return idx;
}
static const FRAME_axis_t idx_to_axis[FRAME_axis_num] = {FRAME_roll, FRAME_pitch, FRAME_yaw};

void StartImuTask(void *argument)
{
	IMU_task_queue = xQueueCreateStatic(IMU_TASK_QUEUE_LENGTH, sizeof(IMU_TASK_command_t), IMU_TASK_queue_container, &IMU_TASK_xStaticQueue);
	  /* Enable register address automatically incremented during a multiple byte
	     access with a serial interface. */

	if ( LSM6DS3_ACC_GYRO_W_BOOT( (void *)&hi2c1, LSM6DS3_ACC_GYRO_BOOT_REBOOT_MODE ) == MEMS_ERROR )
	  {
		dik();
	  }
	osDelay(10);
	if ( LSM6DS3_ACC_GYRO_W_IF_Addr_Incr( (void *)&hi2c1, LSM6DS3_ACC_GYRO_IF_INC_ENABLED ) == MEMS_ERROR )
	  {
		dik();
	  }

	  /* Enable BDU */
	  if ( LSM6DS3_ACC_GYRO_W_BDU( (void *)&hi2c1, LSM6DS3_ACC_GYRO_BDU_BLOCK_UPDATE ) == MEMS_ERROR )
	  {dik();
	  }

	  if ( LSM6DS3_ACC_GYRO_W_INT_ACT_LEVEL( (void *)&hi2c1, LSM6DS3_ACC_GYRO_INT_ACT_LEVEL_ACTIVE_LO ) == MEMS_ERROR )
	  {
		  dik();
	  }
	  /* FIFO mode selection */
	  if ( LSM6DS3_ACC_GYRO_W_FIFO_MODE( (void *)&hi2c1, LSM6DS3_ACC_GYRO_FIFO_MODE_BYPASS ) == MEMS_ERROR )
	  {dik();
	  }

	  /* Output data rate selection - power down. */
	  if ( LSM6DS3_ACC_GYRO_W_ODR_XL( (void *)&hi2c1, LSM6DS3_ACC_GYRO_ODR_XL_POWER_DOWN ) == MEMS_ERROR )
	  {dik();
	  }

	  if (LSM6DS3_ACC_GYRO_W_FS_XL( (void *)&hi2c1, LSM6DS3_ACC_GYRO_FS_XL_2g) == MEMS_ERROR)
	  {
		  dik();
	  }


	  /* Enable axes. */
	  if ( LSM6DS3_ACC_GYRO_W_XEN_XL( (void *)&hi2c1, LSM6DS3_ACC_GYRO_XEN_XL_ENABLED ) == MEMS_ERROR )
	  {
		  dik();
	  }

	  if ( LSM6DS3_ACC_GYRO_W_YEN_XL( (void *)&hi2c1, LSM6DS3_ACC_GYRO_YEN_XL_ENABLED ) == MEMS_ERROR )
	  {dik();
	  }

	  if ( LSM6DS3_ACC_GYRO_W_ZEN_XL( (void *)&hi2c1, LSM6DS3_ACC_GYRO_ZEN_XL_ENABLED ) == MEMS_ERROR )
	  {dik();
	  }

	  /* Output data rate selection - power down */
	  if ( LSM6DS3_ACC_GYRO_W_ODR_G( (void *)&hi2c1, LSM6DS3_ACC_GYRO_ODR_G_POWER_DOWN ) == MEMS_ERROR )
	  {dik();
	  }

	  if (LSM6DS3_ACC_GYRO_W_FS_G( (void *)&hi2c1, LSM6DS3_ACC_GYRO_FS_G_245dps) == MEMS_ERROR)
	  {
		  dik();
	  }

	  if ( LSM6DS3_ACC_GYRO_W_XEN_G( (void *)&hi2c1, LSM6DS3_ACC_GYRO_XEN_G_ENABLED ) == MEMS_ERROR )
	  {dik();
	  }

	  if ( LSM6DS3_ACC_GYRO_W_YEN_G( (void *)&hi2c1, LSM6DS3_ACC_GYRO_YEN_G_ENABLED ) == MEMS_ERROR )
	  {dik();
	  }

	  if ( LSM6DS3_ACC_GYRO_W_ZEN_G( (void *)&hi2c1, LSM6DS3_ACC_GYRO_ZEN_G_ENABLED ) == MEMS_ERROR )
	  {dik();
	  }

	  if (LSM6DS3_ACC_GYRO_W_ODR_G( (void *)&hi2c1, LSM6DS3_ACC_GYRO_ODR_G_104Hz) == MEMS_ERROR)
	  {
		  dik();
	  }
	  if ( LSM6DS3_ACC_GYRO_W_ODR_XL( (void *)&hi2c1, LSM6DS3_ACC_GYRO_ODR_XL_104Hz ) == MEMS_ERROR )
	  {
		  dik();
	  }
	  if ( LSM6DS3_ACC_GYRO_W_INT_ACT_LEVEL( (void *)&hi2c1, LSM6DS3_ACC_GYRO_INT_ACT_LEVEL_ACTIVE_HI ) == MEMS_ERROR )
	  {
		  dik();
	  }
	  if ( LSM6DS3_ACC_GYRO_W_DRDY_XL_on_INT1( (void *)&hi2c1, LSM6DS3_ACC_GYRO_INT1_DRDY_XL_ENABLED ) == MEMS_ERROR )
	  {
		  dik();
	  }
	  if ( LSM6DS3_ACC_GYRO_W_DRDY_G_on_INT2( (void *)&hi2c1, LSM6DS3_ACC_GYRO_INT2_DRDY_G_ENABLED ) == MEMS_ERROR )
	  {
		  dik();
	  }

  IMU_TASK_command_t received_command = IMU_TASK_invalid;
  float gyro_ang_rate[IMU_MEAS_AX_NUM] = {0.0f, 0.0f, 0.0f};
  float acc_g[IMU_MEAS_AX_NUM] = {0.0f, 0.0f, 0.0f};

  for(;;)
  {
	  if (IMU_TASK_message_dropped) {
		  IMU_TASK_message_dropped = false;
		  LogWarn("IMU message dropped!");
	  }
      if( xQueueReceive( IMU_task_queue,
                         &received_command,
                         portMAX_DELAY ) == pdPASS )
      {
    	  uint8_t regValue[6] = {0, 0, 0, 0, 0, 0};
    	  switch (received_command) {
    	  	  case IMU_TASK_new_acc_meas:
    		      if ( LSM6DS3_ACC_GYRO_GetRawAccData( (void *)&hi2c1, regValue ) == MEMS_ERROR )
    		      {
    		    	  dik();
    		      }
    		      /* Format the data. */

    	  		  for (uint8_t idx = 0; idx < IMU_MEAS_AX_NUM; ++idx) {
    	  			  acc_g[idx] = raw_acc_to_g((((int16_t)regValue[(idx * 2) + 1] ) << 8) + (int16_t)regValue[idx * 2]);
    	  		  }
    		      IMU_TASK_acc_data_received = true;
    		  break;

    	  	  case IMU_TASK_new_gyro_meas:
    	  		  if ( LSM6DS3_ACC_GYRO_GetRawGyroData( (void *)&hi2c1, regValue ) == MEMS_ERROR )
    	  		  {
    	  			  dik();
    	  		  }
    		      /* Format the data. */
    	  		  for (uint8_t idx = 0; idx < IMU_MEAS_AX_NUM; ++idx) {
    	  			  gyro_ang_rate[idx] = raw_gyro_to_radps((((int16_t)regValue[(idx * 2) + 1]) << 8) + (int16_t)regValue[idx * 2]);
    	  			  FRAME_set_rate(idx_to_axis[idx], gyro_ang_rate[idx]);
    	  		  }
    		      IMU_TASK_gyro_data_received = true;
    		  break;

    	  	  default:
    		  break;

    	  }
    	  MADGWICK_euler_angles_t angles = {0};

    	  if (IMU_TASK_acc_data_received && IMU_TASK_gyro_data_received) {
    		  IMU_TASK_acc_data_received = false;
			  IMU_TASK_gyro_data_received = false;
			  MadgwickAHRSupdateIMU(gyro_ang_rate[0], gyro_ang_rate[1], gyro_ang_rate[2], acc_g[0], acc_g[1], acc_g[2]);
			  MadgwickAHRSgetAngles(&angles);
			  float angles_arr[IMU_MEAS_AX_NUM] = {angles.roll, angles.pitch, angles.yaw};
    	  	  for (uint8_t idx = 0; idx < IMU_MEAS_AX_NUM; ++idx) {
    	  		FRAME_set_angle(idx_to_axis[idx], angles_arr[idx]);
    	  	  }

    	  }

      } else {
		  LogWarn("Failed to get data from IMU queue");
      }
	  /* Read output registers from LSM6DS3_ACC_GYRO_OUTX_L_XL to LSM6DS3_ACC_GYRO_OUTZ_H_XL. */
  }

}

uint8_t LSM6DS3_IO_Write( void *handle, uint8_t WriteAddr, uint8_t *pBuffer, uint16_t nBytesToWrite )
{
	return HAL_I2C_Mem_Write((I2C_HandleTypeDef*)handle, 0x6A<<1, WriteAddr, 1, pBuffer, nBytesToWrite, HAL_MAX_DELAY);
}

uint8_t LSM6DS3_IO_Read( void *handle, uint8_t ReadAddr, uint8_t *pBuffer, uint16_t nBytesToRead )
{
	return HAL_I2C_Mem_Read((I2C_HandleTypeDef*)handle, 0x6A<<1, ReadAddr, 1, pBuffer, nBytesToRead, HAL_MAX_DELAY);
}
