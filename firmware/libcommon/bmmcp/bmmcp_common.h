/*
 * bmmcp.h
 *
 *  Created on: Jul 24, 2021
 *      Author: tamas
 *  Bare Minimum Motor Control Protocol
 */

#ifndef BMMCP_BMMCP_COMMON_H_
#define BMMCP_BMMCP_COMMON_H_
#include "stdint.h"
#include "stdbool.h"

#define BMMCP_PACKET_LENGTH (3U)

typedef bool (* BMMCP_send_packet_func_t ) (void * HW_handle, void *txBuffer, uint16_t txDataLength);
typedef void (* BMMCP_receive_packet_func_t ) (void * HW_handle, void *rxBuffer, uint16_t rxDataLength);
typedef void (* BMMCP_init_hw_func_t ) (void * HW_handle);

typedef enum {
	BMMCP_ok = 0,
	BMMCP_no_packet_received,
	BMMCP_resource_busy,
	BMMCP_buffer_short,
	BMMCP_unexpected_value,
}BMMCP_return_t;

typedef enum {
	BMMCP_no_command = 0,
	BMMCP_start_motor = 1,
	BMMCP_stop_motor = 2,
	BMMCP_get_state = 3,
	BMMCP_get_faults = 4,
	BMMCP_set_current = 5,
	BMMCP_send_velocity = 6,
	BMMCP_response = 7,
}BMMCP_command_t;

typedef struct {
	BMMCP_ack = 0,
	BMMCP_nack,
}BMMCP_response_t;

typedef struct {
	BMMCP_command_t command;
	union {
		uint16_t velocity;
		uint16_t current;
		uint16_t stm_state;
		BMMCP_response_t command_response;
	}data;
}BMMCP_universal_packet_t;

typedef enum {
	BMMCP_empty = 0,
	BMMCP_write_lock,
	BMMCP_full,
	BMMCP_read_lock,
}BMMCP_lock_state_t;

typedef struct {
	void * HW_if;
	BMMCP_send_packet_func_t send_packet_func;
	BMMCP_receive_packet_func_t receive_packet_func;
	BMMCP_init_hw_func_t init_hw_func;
	uint8_t receive_buffer[BMMCP_PACKET_LENGTH];
	BMMCP_lock_state_t receive_buffer_lock;
	uint8_t transmit_buffer[BMMCP_PACKET_LENGTH];
	BMMCP_lock_state_t transmit_buffer_lock;
}BMMCP_handle_t;

BMMCP_return_t BMMCP_read_msg(BMMCP_handle_t * bmmcp_handle, BMMCP_universal_packet_t * received_packet);
BMMCP_return_t BMMCP_write_msg(BMMCP_handle_t * bmmcp_handle, BMMCP_universal_packet_t * packet_to_send);
BMMCP_return_t BMMCP_init(BMMCP_handle_t * bmmcp_handle);

// Interrupt handlers
void BMMCP_HWDataReceivedIT(BMMCP_handle_t * bmmcp_handle);
void BMMCP_HWDataTransmittedIT(BMMCP_handle_t * bmmcp_handle);
#endif /* BMMCP_BMMCP_COMMON_H_ */
