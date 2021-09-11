/*
 * bmmcp_packet.h
 *
 *  Created on: Aug 2, 2021
 *      Author: tamas
 */

#ifndef BMMCP_BMMCP_PACKET_H_
#define BMMCP_BMMCP_PACKET_H_
#include "stdint.h"
#include "bmmcp/bmmcp_slave_state.h"
typedef enum {
	BMMCP_no_command = 0,
	BMMCP_align_motor = 1,
	BMMCP_start_motor = 2,
	BMMCP_stop_motor = 3,
	BMMCP_get_state = 4,
	BMMCP_get_faults = 5,
	BMMCP_set_current = 6,
	BMMCP_telemetry = 7,
	BMMCP_reset_torque = 8,
	BMMCP_acknowledge_fault = 9,
	BMMCP_response = 10,
	BMMCP_send_faults = 12,
}BMMCP_command_t;

typedef enum {
	BMMCP_ack = 0,
	BMMCP_nack,
}BMMCP_response_t;

typedef struct {
    uint16_t velocity;
    uint16_t current;
    BMMCP_slave_state_t stm_state;
}BMMCP_telemetry_payload_t;

typedef struct {
    uint16_t active;
    uint16_t occured;
}BMMCP_faults_payload_t;

typedef struct {
	uint8_t id;
	BMMCP_command_t command;
	union {
		BMMCP_telemetry_payload_t telemetry;
		uint16_t current;
		BMMCP_response_t command_response;
		BMMCP_faults_payload_t faults;
	}data;
}BMMCP_universal_packet_t;



#endif /* BMMCP_BMMCP_PACKET_H_ */
