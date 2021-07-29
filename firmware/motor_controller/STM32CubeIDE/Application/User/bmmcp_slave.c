/*
 * bmmcp_slave.c
 *
 *  Created on: Jul 25, 2021
 *      Author: tamas
 */
#include "stdbool.h"

#include "bmmcp_slave.h"
#include "bmmcp_config.h"
#include "mc_api.h"

void BMMCP_slave_process(void) {
	BMMCP_universal_packet_t received_packet = {0};
	BMMCP_return_t read_result = BMMCP_read_msg(&BMMCP_handle, &received_packet);
	switch (read_result) {

		case BMMCP_ok:
			//message received
			switch (received_packet.command) {
				case BMMCP_start_motor:
					if(!MC_StartMotor1()) {
						// TODO: handle start failure
					}
				break;

				default:
					// Error
				break;
			}
		break;

		case BMMCP_no_packet_received:
			//no message received
		break;

		default:
			// Error
		break;
	}
}
