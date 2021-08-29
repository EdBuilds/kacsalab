/*
 * can_bmmcp_handle.h
 *
 *  Created on: Aug 17, 2021
 *      Author: tamas
 */

#ifndef CAN_BMMCP_HANDLE_H_
#define CAN_BMMCP_HANDLE_H_

#include <stdint.h>
#include <stdbool.h>
#include "stm32f3xx.h"
#include "stm32f3xx_hal_dma.h"

typedef struct {
	CAN_HandleTypeDef hcan;
	CAN_RxHeaderTypeDef receive_header;
	uint8_t receive_data[8];
	bool receive_success;
} CAN_BUS_handle_t;

extern CAN_BUS_handle_t CAN_BUS_handle;


#endif /* CAN_BMMCP_HANDLE_H_ */
