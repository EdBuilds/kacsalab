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

#define IMU_TASK_QUEUE_LENGTH (3U)

typedef enum {
	IMU_TASK_new_acc_meas,
	IMU_TASK_new_gyro_meas,
	IMU_TASK_invalid
} IMU_TASK_command_t;

SD_euler_angles_t SD_angles;
static uint8_t IMU_TASK_queue_container[IMU_TASK_QUEUE_LENGTH * sizeof(IMU_TASK_command_t)] = {0};
static StaticQueue_t IMU_TASK_xStaticQueue;
I2C_HandleTypeDef hi2c1;
static QueueHandle_t IMU_task_queue = NULL;
static bool IMU_TASK_acc_data_received = false;
static bool IMU_TASK_gyro_data_received = false;

void dik() {
	volatile uint8_t ouch = 1;
	++ouch;
}

void IMU_int1_handler(BaseType_t * higher_prio_task_woken){
	IMU_TASK_command_t temp_command = IMU_TASK_new_acc_meas;
	if (IMU_task_queue != NULL) {
		xQueueSendFromISR(IMU_task_queue, &temp_command, higher_prio_task_woken);
	}
}

void IMU_int2_handler(BaseType_t * higher_prio_task_woken){
	IMU_TASK_command_t temp_command = IMU_TASK_new_gyro_meas;
	if (IMU_task_queue != NULL) {
		xQueueSendFromISR(IMU_task_queue, &temp_command, higher_prio_task_woken);
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

	  if (LSM6DS3_ACC_GYRO_W_ODR_G( (void *)&hi2c1, LSM6DS3_ACC_GYRO_ODR_G_833Hz) == MEMS_ERROR)
	  {
		  dik();
	  }
	  if ( LSM6DS3_ACC_GYRO_W_ODR_XL( (void *)&hi2c1, LSM6DS3_ACC_GYRO_ODR_XL_833Hz ) == MEMS_ERROR )
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
  for(;;)
  {
      if( xQueueReceive( IMU_task_queue,
                         &received_command,
                         portMAX_DELAY ) == pdPASS )
      {
    	  int16_t acc_data[3] = {0, 0, 0};
    	  int16_t gyro_data[3] = {0, 0, 0};
    	  float gyro_ang_rate[3] = {0.0f, 0.0f, 0.0f};
    	  float acc_g[3] = {0.0f, 0.0f, 0.0f};
    	  uint8_t regValue[6] = {0, 0, 0, 0, 0, 0};

    	  switch (received_command) {
    	  	  case IMU_TASK_new_acc_meas:
    		      if ( LSM6DS3_ACC_GYRO_GetRawAccData( (void *)&hi2c1, regValue ) == MEMS_ERROR )
    		      {
    		    	  dik();
    		      }
    		      /* Format the data. */
    		      acc_data[0] = ( ( ( ( int16_t )regValue[1] ) << 8 ) + ( int16_t )regValue[0] );
    		      acc_data[1] = ( ( ( ( int16_t )regValue[3] ) << 8 ) + ( int16_t )regValue[2] );
    		      acc_data[2] = ( ( ( ( int16_t )regValue[5] ) << 8 ) + ( int16_t )regValue[4] );
    		      acc_g[0] = raw_acc_to_g(acc_data[0]);
    		      acc_g[1] = raw_acc_to_g(acc_data[1]);
    		      acc_g[2] = raw_acc_to_g(acc_data[2]);
    		      IMU_TASK_acc_data_received = true;
    		  break;

    	  	  case IMU_TASK_new_gyro_meas:
    	  		  if ( LSM6DS3_ACC_GYRO_GetRawGyroData( (void *)&hi2c1, regValue ) == MEMS_ERROR )
    	  		  {
    	  			  dik();
    	  		  }
    		      /* Format the data. */
    		      gyro_ang_rate[0] = raw_gyro_to_radps( ( ( ( int16_t )regValue[1] ) << 8 ) + ( int16_t )regValue[0] );
    		      gyro_ang_rate[1] = raw_gyro_to_radps( ( ( ( int16_t )regValue[3] ) << 8 ) + ( int16_t )regValue[2] );
    		      gyro_ang_rate[2] = raw_gyro_to_radps( ( ( ( int16_t )regValue[5] ) << 8 ) + ( int16_t )regValue[4] );

    		      IMU_TASK_gyro_data_received = true;
    		  break;

    	  	  default:
    		  break;

    	  }
    	  MADGWICK_euler_angles_t angles = {0};
    	  if (IMU_TASK_acc_data_received && IMU_TASK_gyro_data_received) {
    		  IMU_TASK_acc_data_received = true;
			  IMU_TASK_gyro_data_received = true;
			  MadgwickAHRSupdateIMU(gyro_ang_rate[0], gyro_ang_rate[1], gyro_ang_rate[2], acc_g[0], acc_g[1], acc_g[2]);
			  MadgwickAHRSgetAngles(&angles);

			  SD_angles.roll = angles.roll;
			  SD_angles.pitch = angles.pitch;
			  SD_angles.yaw = angles.yaw;
    	  }

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
