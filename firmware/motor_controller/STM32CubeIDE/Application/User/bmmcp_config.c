/*
 * bmmcp_config.c
 *
 *  Created on: Jul 25, 2021
 *      Author: tamas
 */
#include "bmmcp_config.h"
#include "usart_aspep_driver.h"
#include "fdcan_bmmcp_driver.h"
#include "stlink_usart_config.h"
#include "fdcan_bmmcp_handle.h"

UASPEP_Handle_t STLINK_USART_uaspep;
extern CAN_BUS_handle_t CAN_BUS_handle;
#define CAN_BMMCP

#ifdef SERIAL_BMMCP
BMMCP_handle_t BMMCP_handle = {
	.HW_if = &STLINK_USART_uaspep,
	.init_hw_func = &UASPEP_INIT,
	.send_packet_func = &UASPEP_SEND_PACKET,
	.receive_packet_func = &UASPEP_RECEIVE_BUFFER,
	.receive_buffer = {0},
	.receive_buffer_lock = BMMCP_empty,
	.transmit_buffer = {0},
	.transmit_buffer_lock = BMMCP_empty
};
#endif

#ifdef CAN_BMMCP
BMMCP_handle_t BMMCP_handle = {
	.HW_if = &CAN_BUS_handle,
	.init_hw_func = &FDCAN_BMMCP_init,
	.send_packet_func = &FDCAN_BMMCP_send_packet,
	.receive_packet_func = &FDCAN_BMMCP_receive_buffer,
	.receive_buffer = {0},
	.receive_buffer_lock = BMMCP_empty,
	.transmit_buffer = {0},
	.transmit_buffer_lock = BMMCP_empty
};
#endif
