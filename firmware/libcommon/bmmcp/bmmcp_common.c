/*
 * bmmcp.c
 *
 *  Created on: Jul 24, 2021
 *      Author: tamas
 */
#include "bmmcp_common.h"
#include "string.h"

#define BMMCP_HEADER_SIZE (1U)
#define BMMCP_PAYLOAD_SIZE (BMMCP_PACKET_LENGTH - BMMCP_HEADER_SIZE)
static BMMCP_return_t BMMCP_unpack_packet(const uint8_t * rxbuffer, uint8_t buffer_length, BMMCP_universal_packet_t * packet);
static BMMCP_return_t BMMCP_pack_packet(uint8_t * txbuffer, uint8_t buffer_length, const BMMCP_universal_packet_t * packet);

static BMMCP_return_t BMMCP_unpack_packet(const uint8_t * rxbuffer, uint8_t buffer_length, BMMCP_universal_packet_t * packet) {
	BMMCP_return_t result = BMMCP_ok;

	if (buffer_length < BMMCP_PACKET_LENGTH) {
		result = BMMCP_buffer_short;
	} else {
		const uint8_t * command_buffer = rxbuffer;
		const uint8_t * payload_buffer = rxbuffer + BMMCP_HEADER_SIZE;
		(void) memcpy(&(packet->command), command_buffer, BMMCP_HEADER_SIZE);
		(void) memcpy(&(packet->data), payload_buffer, BMMCP_PAYLOAD_SIZE);
	}
	return result;
}

static BMMCP_return_t BMMCP_pack_packet(uint8_t * txbuffer, uint8_t buffer_length, const BMMCP_universal_packet_t * packet) {
	BMMCP_return_t result = BMMCP_ok;

	if (buffer_length < BMMCP_PACKET_LENGTH) {
		result = BMMCP_buffer_short;
	} else {
		uint8_t * command_buffer = txbuffer;
		uint8_t * payload_buffer = txbuffer + BMMCP_HEADER_SIZE;
		(void) memcpy(command_buffer, &(packet->command), BMMCP_HEADER_SIZE);
		(void) memcpy(payload_buffer, &(packet->data), BMMCP_PAYLOAD_SIZE);
	}
	return result;
}

BMMCP_return_t BMMCP_read_msg(BMMCP_handle_t * bmmcp_handle, BMMCP_universal_packet_t * received_packet) {
	BMMCP_return_t result = BMMCP_ok;

	switch (bmmcp_handle->receive_buffer_lock) {
		case BMMCP_full:
			bmmcp_handle->receive_buffer_lock = BMMCP_read_lock;
			result = BMMCP_unpack_packet(bmmcp_handle->receive_buffer, BMMCP_PACKET_LENGTH, received_packet);
			bmmcp_handle->receive_buffer_lock = BMMCP_write_lock;
			bmmcp_handle->receive_packet_func(bmmcp_handle->HW_if, bmmcp_handle->receive_buffer, BMMCP_PACKET_LENGTH);
		break;

		case BMMCP_write_lock:
			result = BMMCP_no_packet_received;
		break;

		default:
			result = BMMCP_unexpected_value;
		break;
	}
	return result;
}

BMMCP_return_t BMMCP_write_msg(BMMCP_handle_t * bmmcp_handle, BMMCP_universal_packet_t * packet_to_send) {
	BMMCP_return_t result = BMMCP_ok;

	switch (bmmcp_handle->transmit_buffer_lock) {
		case BMMCP_empty:
			bmmcp_handle->transmit_buffer_lock = BMMCP_write_lock;
			result = BMMCP_pack_packet(bmmcp_handle->transmit_buffer, BMMCP_PACKET_LENGTH, packet_to_send);
			bmmcp_handle->transmit_buffer_lock = BMMCP_read_lock;
			bmmcp_handle->send_packet_func(bmmcp_handle->HW_if, bmmcp_handle->transmit_buffer, BMMCP_PACKET_LENGTH);
		break;

		case BMMCP_read_lock:
			result = BMMCP_resource_busy;
		break;

		default:
			result = BMMCP_unexpected_value;
		break;
	}
	return result;
}

void BMMCP_HWDataReceivedIT(BMMCP_handle_t * bmmcp_handle) {

	switch (bmmcp_handle->receive_buffer_lock) {
		case BMMCP_write_lock:
			bmmcp_handle->receive_buffer_lock = BMMCP_full;
			bmmcp_handle->receive_callback_func();
		break;
		default:
			// Unexpected value, something's fishy
		break;
	}
}

void BMMCP_HWDataTransmittedIT(BMMCP_handle_t * bmmcp_handle) {

	switch (bmmcp_handle->transmit_buffer_lock) {
		case BMMCP_read_lock:
			bmmcp_handle->transmit_buffer_lock = BMMCP_empty;
		break;
		default:
			// Unexpected value, something's fishy
		break;
	}
}

BMMCP_return_t BMMCP_init(BMMCP_handle_t * bmmcp_handle, BMMCP_receive_callback_func_t receive_callback_func) {

    bmmcp_handle->init_hw_func(bmmcp_handle->HW_if);
	bmmcp_handle->receive_buffer_lock = BMMCP_write_lock;
	bmmcp_handle->receive_packet_func(bmmcp_handle->HW_if, bmmcp_handle->receive_buffer, BMMCP_PACKET_LENGTH);
	bmmcp_handle->receive_callback_func = receive_callback_func;
	return BMMCP_ok;
}
