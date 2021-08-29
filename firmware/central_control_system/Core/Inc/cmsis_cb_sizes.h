/*
 * cmsis_cb_sizes.h
 *
 *  Created on: Aug 29, 2021
 *      Author: tamas
 */

#ifndef INC_CMSIS_CB_SIZES_H_
#define INC_CMSIS_CB_SIZES_H_
#include "FreeRTOS.h"
#define OS_EVENT_FLAG_CB_SIZE (sizeof(StaticEventGroup_t))
#define OS_TIMER_CB_SIZE (sizeof(StaticTimer_t))
#define OS_TASK_CB_SIZE (sizeof(StaticTask_t))

#endif /* INC_CMSIS_CB_SIZES_H_ */
