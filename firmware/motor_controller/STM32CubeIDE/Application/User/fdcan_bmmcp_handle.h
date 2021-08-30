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
#include "stm32g4xx.h"
#include "stm32g4xx_hal_dma.h"

typedef struct {
	FDCAN_HandleTypeDef hcan;
	FDCAN_RxHeaderTypeDef receive_header;
	uint8_t receive_data[8];
	bool receive_success;
} CAN_BUS_handle_t;

extern CAN_BUS_handle_t CAN_BUS_handle;


#endif /* CAN_BMMCP_HANDLE_H_ */
