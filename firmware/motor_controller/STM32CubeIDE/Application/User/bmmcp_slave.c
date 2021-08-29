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
	BMMCP_universal_packet_t packet_to_send = {0};
	BMMCP_return_t read_result = BMMCP_read_msg(&BMMCP_handle, &received_packet);
	bool send_msg = false;
	switch (read_result) {

		case BMMCP_ok:
			//message received
			switch (received_packet.command) {
				case BMMCP_start_motor:
						packet_to_send.command = BMMCP_response;
					if(!MC_StartMotor1()) {
						packet_to_send.data.command_response = BMMCP_nack;
					} else {
						packet_to_send.data.command_response = BMMCP_ack;
					}
					send_msg = true;
				break;

				case BMMCP_align_motor:
						packet_to_send.command = BMMCP_response;
					if(!MC_AlignEncoderMotor1()) {
						packet_to_send.data.command_response = BMMCP_nack;
					} else {
						packet_to_send.data.command_response = BMMCP_ack;
					}
					send_msg = true;
				break;

				case BMMCP_stop_motor:
						packet_to_send.command = BMMCP_response;
					if(!MC_StopMotor1()) {
						packet_to_send.data.command_response = BMMCP_nack;
					} else {
						packet_to_send.data.command_response = BMMCP_ack;
					}
					send_msg = true;
				break;

				case BMMCP_reset_torque:
					packet_to_send.command = BMMCP_response;
					qd_t Iqdref = {.d =0,.q=0};
					MC_SetCurrentReferenceMotor1(Iqdref);
					packet_to_send.data.command_response = BMMCP_ack;
					send_msg = true;
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

	if (send_msg) {
		BMMCP_write_msg(&BMMCP_handle, &packet_to_send);
	}
}
