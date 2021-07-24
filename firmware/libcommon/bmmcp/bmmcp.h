/*
 * bmmcp.h
 *
 *  Created on: Jul 24, 2021
 *      Author: tamas
 */

#ifndef BMMCP_BMMCP_H_
#define BMMCP_BMMCP_H_
#include "stdint.h"
typedef enum {
	BMMCP_no_command = 0,
	BMMCP_start_motor,
	BMMCP_stop_motor,
	BMMCP_get_state,
	BMMCP_get_faults,
	BMMCP_set_current,
	BMMCP_send_velocity,
}bmmcp_command_t;

typedef struct {
	bmmcp_command_t command;
	union data{
		uint16_t velocity;
		uint16_t current;
	};
}bmmcp_universal_packet_t;

#endif /* BMMCP_BMMCP_H_ */
