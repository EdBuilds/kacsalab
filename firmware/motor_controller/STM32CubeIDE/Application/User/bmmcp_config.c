/*
 * bmmcp_config.c
 *
 *  Created on: Jul 25, 2021
 *      Author: tamas
 */
#include "bmmcp_config.h"
#include "usart_aspep_driver.h"
#include "stlink_usart_config.h"

UASPEP_Handle_t STLINK_USART_uaspep;

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

