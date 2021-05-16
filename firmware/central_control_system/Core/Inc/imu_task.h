/*
 * i2c_task.h
 *
 *  Created on: 5 May 2021
 *      Author: tamas
 */

#ifndef INC_IMU_TASK_H_
#define INC_IMU_TASK_H_
#include "FreeRTOS.h"
void StartImuTask(void *argument);
void IMU_int1_handler(BaseType_t * higher_prio_task_woken);
void IMU_int2_handler(BaseType_t * higher_prio_task_woken);
#endif /* INC_IMU_TASK_H_ */
