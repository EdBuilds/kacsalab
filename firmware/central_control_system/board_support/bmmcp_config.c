/*
 * bmmcp_config.c
 *
 *  Created on: Jul 25, 2021
 *      Author: tamas
 */
#include "bmmcp_config.h"
#include "uart_bmmcp_driver.h"
#include "can_bmmcp_driver.h"
#include "stlink_uart_config.h"
#include "can_bmmcp_handle.h"

extern STLINK_USART_Handle_t STLINK_USART_handle;
extern CAN_BUS_handle_t CAN_BUS_handle;
#define CAN_BMMCP

#ifdef SERIAL_BMMCP
BMMCP_handle_t BMMCP_handle = {
	.HW_if = &STLINK_USART_handle,
	.init_hw_func = &UART_BMMCP_init,
	.send_packet_func = &UART_BMMCP_send_packet,
	.receive_packet_func = &UART_BMMCP_receive_buffer,
	.receive_buffer = {0},
	.receive_buffer_lock = BMMCP_empty,
	.transmit_buffer = {0},
	.transmit_buffer_lock = BMMCP_empty
};
#endif

#ifdef CAN_BMMCP
BMMCP_handle_t BMMCP_handle = {
	.HW_if = &CAN_BUS_handle,
	.init_hw_func = &CAN_BMMCP_init,
	.send_packet_func = &CAN_BMMCP_send_packet,
	.receive_packet_func = &CAN_BMMCP_receive_buffer,
	.receive_buffer = {0},
	.receive_buffer_lock = BMMCP_empty,
	.transmit_buffer = {0},
	.transmit_buffer_lock = BMMCP_empty
};
#endif
