/*
 * bmmcp_packet.h
 *
 *  Created on: Aug 2, 2021
 *      Author: tamas
 */

#ifndef BMMCP_BMMCP_PACKET_H_
#define BMMCP_BMMCP_PACKET_H_

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

typedef enum {
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



#endif /* BMMCP_BMMCP_PACKET_H_ */
