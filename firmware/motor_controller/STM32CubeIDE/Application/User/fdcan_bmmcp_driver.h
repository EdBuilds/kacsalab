/*
 * bmmcp_driver.h
 *
 *  Created on: Jul 30, 2021
 *      Author: tamas
 */

#ifndef BOARD_SUPPORT_CAN_BMMCP_DRIVER_H_
#define BOARD_SUPPORT_CAN_BMMCP_DRIVER_H_

#include <stdint.h>
#include <stdbool.h>
#include "stm32g4xx.h"
/* To be removed no protocol awarness at this level */


bool FDCAN_BMMCP_send_packet (void *pHandle, void *data, uint16_t length);
void FDCAN_BMMCP_receive_buffer (void *pHandle, void* buffer, uint16_t length);
void FDCAN_BMMCP_init (void *pHandle);
void FDCAN_BMMCP_idle_enable (void *pHWHandle);




#endif /* BOARD_SUPPORT_CAN_BMMCP_DRIVER_H_ */
