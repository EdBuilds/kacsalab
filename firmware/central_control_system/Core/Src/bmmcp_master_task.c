/*
 * bmmcp_master_task.c
 *
 *  Created on: Jul 31, 2021
 *      Author: tamas
 */
#include "bmmcp_master_task.h"
#include "bmmcp_config.h"
#include "motor_object.h"
#include "cmsis_os2.h"
#include "shared_task_resources.h"
#include "logging.h"
extern BMMCP_handle_t BMMCP_handle;
static void bmmcp_received_packet_callback(void);
static void bmmcp_telemetry_timeout_callback(void * arg);
void start_bmmcp_master_task(void *argument)
{
	BMMCP_MASTER_task_arg_t * args = (BMMCP_MASTER_task_arg_t *)argument;
	if (BMMCP_init(&BMMCP_handle, &bmmcp_received_packet_callback)!= BMMCP_ok) {
		LogError("Could not init BMMCP interface");
	} else {

		if (!args->debugging) {
		    if (MOTOR_OBJ_create_telemetry_timer(&bmmcp_telemetry_timeout_callback) != ERRORS_ok) {
		    	LogError("Could not create timers for BMMCP telemetry timeout");
		    	// TODO: deinit bmmcp here
		    }
		}

		BMMCP_master_task_msg_t msg;
		osStatus_t status;

		while (true) {
			status = osMessageQueueGet(SHARED_TASK_get_bmmcp_master_queue(), &msg, NULL, osWaitForever);
			if (status == osOK) {
				ERRORS_return_t new_msg_result = ERRORS_ok;
				switch (msg.variant) {
				case BMMCP_MASTER_TASK_packet_receive:
					// I don't feel like allocating even more on the stack, so I'm repurposing the msg packet for this
					if (BMMCP_read_msg(&BMMCP_handle, &(msg.payload.packet)) != BMMCP_ok) {
						LogWarn("Could not parse new msg");
					} else {
						new_msg_result = MOTOR_OBJ_new_message(&(msg.payload.packet));
						if (new_msg_result != ERRORS_ok) {
							LogWarn("Could not process new msg: %s", ERRORS_error_to_string(new_msg_result));
						}

					}
				break;
				case BMMCP_MASTER_TASK_packet_send:
					BMMCP_write_msg(&BMMCP_handle, &(msg.payload.packet));
					//TODO dropped error
				break;
				case BMMCP_MASTER_TASK_telem_timeout:
					if (MOTOR_OBJ_lost_connection(msg.payload.timeout_motor_id) != ERRORS_ok) {
						LogError("Could not report lost connection to motor");
					}
				break;
				}
			} else {
				LogError("Could not read from BMMCP master task queue");
			}
		}

	}
}

static void bmmcp_received_packet_callback(void) {
	BMMCP_master_task_msg_t msg;
	msg.variant = BMMCP_MASTER_TASK_packet_receive;
	osMessageQueuePut(SHARED_TASK_get_bmmcp_master_queue(), &msg, 0, 0U);
}

static void bmmcp_telemetry_timeout_callback(void * arg)
{
	BMMCP_master_task_msg_t msg;
	msg.variant = BMMCP_MASTER_TASK_telem_timeout;
	msg.payload.timeout_motor_id = *((uint8_t*)arg);
	osMessageQueuePut(SHARED_TASK_get_bmmcp_master_queue(), &msg, 0, 0U);
}
