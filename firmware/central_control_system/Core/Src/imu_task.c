/*
 * i2c_task.c
 *
 *  Created on: 5 May 2021
 *      Author: tamas
 */
#include "LSM6DS3_ACC_GYRO_driver.h"
#include "cmsis_os.h"
#include "i2c.h"

I2C_HandleTypeDef hi2c1;

void dik() {
	volatile uint8_t ouch = 1;
	++ouch;
}
void StartImuTask(void *argument)
{
	uint8_t value = 0;
	  /* Enable register address automatically incremented during a multiple byte
	     access with a serial interface. */
	if ( LSM6DS3_ACC_GYRO_W_IF_Addr_Incr( (void *)&hi2c1, LSM6DS3_ACC_GYRO_IF_INC_ENABLED ) == MEMS_ERROR )
	  {
		dik();
	  }

	  /* Enable BDU */
	  if ( LSM6DS3_ACC_GYRO_W_BDU( (void *)&hi2c1, LSM6DS3_ACC_GYRO_BDU_BLOCK_UPDATE ) == MEMS_ERROR )
	  {dik();
	  }

	  /* FIFO mode selection */
	  if ( LSM6DS3_ACC_GYRO_W_FIFO_MODE( (void *)&hi2c1, LSM6DS3_ACC_GYRO_FIFO_MODE_BYPASS ) == MEMS_ERROR )
	  {dik();
	  }

	  /* Output data rate selection - power down. */
	  if ( LSM6DS3_ACC_GYRO_W_ODR_XL( (void *)&hi2c1, LSM6DS3_ACC_GYRO_ODR_XL_POWER_DOWN ) == MEMS_ERROR )
	  {dik();
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


	  if ( LSM6DS3_ACC_GYRO_W_XEN_G( (void *)&hi2c1, LSM6DS3_ACC_GYRO_XEN_G_ENABLED ) == MEMS_ERROR )
	  {dik();
	  }

	  if ( LSM6DS3_ACC_GYRO_W_YEN_G( (void *)&hi2c1, LSM6DS3_ACC_GYRO_YEN_G_ENABLED ) == MEMS_ERROR )
	  {dik();
	  }

	  if ( LSM6DS3_ACC_GYRO_W_ZEN_G( (void *)&hi2c1, LSM6DS3_ACC_GYRO_ZEN_G_ENABLED ) == MEMS_ERROR )
	  {dik();
	  }

	  if (LSM6DS3_ACC_GYRO_W_ODR_G( (void *)&hi2c1, LSM6DS3_ACC_GYRO_ODR_G_104Hz))
	  {
		  dik();
	  }
	  uint8_t i_guess = 0;
	  LSM6DS3_ACC_GYRO_R_ODR_XL( (void *)&hi2c1, &i_guess );
	  if ( LSM6DS3_ACC_GYRO_W_ODR_XL( (void *)&hi2c1, LSM6DS3_ACC_GYRO_ODR_XL_104Hz ) == MEMS_ERROR )
	    {
		  dik();
	    }
	  LSM6DS3_ACC_GYRO_R_ODR_XL( (void *)&hi2c1, &i_guess );
	  int16_t pData[3] = {0, 0, 0};
	  uint8_t regValue[6] = {0, 0, 0, 0, 0, 0};
  for(;;)
  {


	  /* Read output registers from LSM6DS3_ACC_GYRO_OUTX_L_XL to LSM6DS3_ACC_GYRO_OUTZ_H_XL. */
	  if ( LSM6DS3_ACC_GYRO_GetRawAccData( (void *)&hi2c1, regValue ) == MEMS_ERROR )
	  {
		  dik();
	  }

	  /* Format the data. */
	  pData[0] = ( ( ( ( int16_t )regValue[1] ) << 8 ) + ( int16_t )regValue[0] );
	  pData[1] = ( ( ( ( int16_t )regValue[3] ) << 8 ) + ( int16_t )regValue[2] );
	  pData[2] = ( ( ( ( int16_t )regValue[5] ) << 8 ) + ( int16_t )regValue[4] );
	  osDelay(100);
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
