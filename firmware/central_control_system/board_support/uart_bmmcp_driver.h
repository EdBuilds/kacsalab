/*
 * bmmcp_driver.h
 *
 *  Created on: Jul 30, 2021
 *      Author: tamas
 */

#ifndef BOARD_SUPPORT_BMMCP_DRIVER_H_
#define BOARD_SUPPORT_BMMCP_DRIVER_H_

#include <stdint.h>
#include <stdbool.h>
#include "stm32f3xx.h"
/* To be removed no protocol awarness at this level */


bool UART_BMMCP_send_packet (void *pHandle, void *data, uint16_t length);
void UART_BMMCP_receive_buffer (void *pHandle, void* buffer, uint16_t length);
void UART_BMMCP_init (void *pHandle);
void UART_BMMCP_idle_enable (void *pHWHandle);




#endif /* BOARD_SUPPORT_BMMCP_DRIVER_H_ */
