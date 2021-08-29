/*
 * motor_interface.c
 *
 *  Created on: Aug 2, 2021
 *      Author: tamas
 */
#include "motor_interface.h"
#include "stddef.h"
#include "bmmcp/bmmcp_common.h"
#include "logging.h"
#include "shared_task_resources.h"

// entry functions
static ERRORS_return_t MOTOR_STATE_unknown_entry(MOTOR_handle_t * handle);
static ERRORS_return_t MOTOR_STATE_alignment_command_sent_entry(MOTOR_handle_t * handle);
static ERRORS_return_t MOTOR_STATE_idle_aligned_entry(MOTOR_handle_t * handle);
static ERRORS_return_t MOTOR_STATE_torque_command_sent_entry(MOTOR_handle_t * handle);
static ERRORS_return_t MOTOR_STATE_start_command_sent_entry(MOTOR_handle_t * handle);
static ERRORS_return_t MOTOR_STATE_running_entry(MOTOR_handle_t * handle);
static ERRORS_return_t MOTOR_STATE_stop_command_sent_entry(MOTOR_handle_t * handle);
static ERRORS_return_t MOTOR_STATE_faults_entry(MOTOR_handle_t * handle);
static ERRORS_return_t MOTOR_STATE_reset_fault_sent_entry(MOTOR_handle_t * handle);

// exit functions
static ERRORS_return_t MOTOR_STATE_unknown_exit(MOTOR_handle_t * handle);
static ERRORS_return_t MOTOR_STATE_idle_aligned_exit(MOTOR_handle_t * handle);
static ERRORS_return_t MOTOR_STATE_running_exit(MOTOR_handle_t * handle);

// user command functions
static void * MOTOR_STATE_idle_not_aligned_user_command(MOTOR_user_command_t user_command);
static void * MOTOR_STATE_running_user_command(MOTOR_user_command_t user_command);
static void * MOTOR_STATE_idle_aligned_user_command(MOTOR_user_command_t user_command);
static void * MOTOR_STATE_passive_fault_user_command(MOTOR_user_command_t user_command);

// command response received
static void * MOTOR_STATE_alignment_command_sent_command_response(BMMCP_response_t response);
static void * MOTOR_STATE_torque_command_sent_command_response(BMMCP_response_t response);

// Slave state received function
static void * MOTOR_STATE_unknown_slave_state(BMMCP_slave_state_t slave_state);
static void * MOTOR_STATE_alignment_in_progress_slave_state(BMMCP_slave_state_t slave_state);
static void * MOTOR_STATE_start_command_sent_slave_state(BMMCP_slave_state_t slave_state);
static void * MOTOR_STATE_stop_command_sent_slave_state(BMMCP_slave_state_t slave_state);
static void * MOTOR_STATE_reset_fault_command_sent_slave_state(BMMCP_slave_state_t slave_state);
static void * MOTOR_STATE_stopping_slave_state(BMMCP_slave_state_t slave_state);


static const MOTOR_STATE_handle_t c_MOTOR_STATE_unknown = {
	.MOTOR_STATE_entry_func = &MOTOR_STATE_unknown_entry,
	.MOTOR_STATE_exit_func = &MOTOR_STATE_unknown_exit,
    .MOTOR_STATE_user_command = NULL,
	.MOTOR_STATE_new_slave_state = &MOTOR_STATE_unknown_slave_state,
	.MOTOR_STATE_slave_command_response = NULL,
};
static const MOTOR_STATE_handle_t c_MOTOR_STATE_idle_not_aligned = {
	.MOTOR_STATE_entry_func = NULL,
	.MOTOR_STATE_exit_func = NULL,
    .MOTOR_STATE_user_command = &MOTOR_STATE_idle_not_aligned_user_command,
	.MOTOR_STATE_new_slave_state = NULL,
	.MOTOR_STATE_slave_command_response = NULL,
};
static const MOTOR_STATE_handle_t c_MOTOR_STATE_Alignment_command_sent = {
	.MOTOR_STATE_entry_func = &MOTOR_STATE_alignment_command_sent_entry,
	.MOTOR_STATE_exit_func = NULL,
    .MOTOR_STATE_user_command = NULL,
	.MOTOR_STATE_new_slave_state = NULL,
	.MOTOR_STATE_slave_command_response = &MOTOR_STATE_alignment_command_sent_command_response,
};
static const MOTOR_STATE_handle_t c_MOTOR_STATE_alignment_in_progress = {
	.MOTOR_STATE_entry_func = NULL,
	.MOTOR_STATE_exit_func = NULL,
    .MOTOR_STATE_user_command = NULL,
	.MOTOR_STATE_new_slave_state = &MOTOR_STATE_alignment_in_progress_slave_state,
	.MOTOR_STATE_slave_command_response = NULL,
};
static const MOTOR_STATE_handle_t c_MOTOR_STATE_idle_aligned = {
	.MOTOR_STATE_entry_func = &MOTOR_STATE_idle_aligned_entry,
	.MOTOR_STATE_exit_func = &MOTOR_STATE_idle_aligned_exit,
    .MOTOR_STATE_user_command = MOTOR_STATE_idle_aligned_user_command,
	.MOTOR_STATE_new_slave_state = NULL,
	.MOTOR_STATE_slave_command_response = NULL,
};
static const MOTOR_STATE_handle_t c_MOTOR_STATE_torque_command_sent = {
	.MOTOR_STATE_entry_func = &MOTOR_STATE_torque_command_sent_entry,
	.MOTOR_STATE_exit_func = NULL,
    .MOTOR_STATE_user_command = NULL,
	.MOTOR_STATE_new_slave_state = NULL,
	.MOTOR_STATE_slave_command_response = &MOTOR_STATE_torque_command_sent_command_response,
};
static const MOTOR_STATE_handle_t c_MOTOR_STATE_Start_command_sent = {
	.MOTOR_STATE_entry_func = &MOTOR_STATE_start_command_sent_entry,
	.MOTOR_STATE_exit_func = NULL,
    .MOTOR_STATE_user_command = NULL,
	.MOTOR_STATE_new_slave_state = &MOTOR_STATE_start_command_sent_slave_state,
	.MOTOR_STATE_slave_command_response = NULL,
};
static const MOTOR_STATE_handle_t c_MOTOR_STATE_running = {
	.MOTOR_STATE_entry_func = &MOTOR_STATE_running_entry,
	.MOTOR_STATE_exit_func = &MOTOR_STATE_running_exit,
    .MOTOR_STATE_user_command = &MOTOR_STATE_running_user_command,
	.MOTOR_STATE_new_slave_state = NULL,
	.MOTOR_STATE_slave_command_response = NULL,
};
static const MOTOR_STATE_handle_t c_MOTOR_STATE_stop_command_sent = {
	.MOTOR_STATE_entry_func = &MOTOR_STATE_stop_command_sent_entry,
	.MOTOR_STATE_exit_func = NULL,
    .MOTOR_STATE_user_command = NULL,
	.MOTOR_STATE_new_slave_state = MOTOR_STATE_stop_command_sent_slave_state,
	.MOTOR_STATE_slave_command_response = NULL,
};
static const MOTOR_STATE_handle_t c_MOTOR_STATE_stopping = {
	.MOTOR_STATE_entry_func = NULL,
	.MOTOR_STATE_exit_func = NULL,
    .MOTOR_STATE_user_command = NULL,
	.MOTOR_STATE_new_slave_state = &MOTOR_STATE_stopping_slave_state,
	.MOTOR_STATE_slave_command_response = NULL,
};
static const MOTOR_STATE_handle_t c_MOTOR_STATE_active_fault = {
	.MOTOR_STATE_entry_func = &MOTOR_STATE_faults_entry,
	.MOTOR_STATE_exit_func = NULL,
    .MOTOR_STATE_user_command = NULL,
	.MOTOR_STATE_new_slave_state = NULL,
	.MOTOR_STATE_slave_command_response = NULL,
};
static const MOTOR_STATE_handle_t c_MOTOR_STATE_passive_fault = {
	.MOTOR_STATE_entry_func = &MOTOR_STATE_faults_entry,
	.MOTOR_STATE_exit_func = NULL,
    .MOTOR_STATE_user_command = &MOTOR_STATE_passive_fault_user_command,
	.MOTOR_STATE_new_slave_state = NULL,
	.MOTOR_STATE_slave_command_response = NULL,
};
static const MOTOR_STATE_handle_t c_MOTOR_STATE_reset_fault_command_sent = {
	.MOTOR_STATE_entry_func = &MOTOR_STATE_reset_fault_sent_entry,
	.MOTOR_STATE_exit_func = NULL,
    .MOTOR_STATE_user_command = NULL,
	.MOTOR_STATE_new_slave_state = &MOTOR_STATE_reset_fault_command_sent_slave_state,
	.MOTOR_STATE_slave_command_response = NULL,
};
static ERRORS_return_t set_next_state(MOTOR_handle_t *handle, const MOTOR_STATE_handle_t * next_state);
static const void* common_state_transition(MOTOR_handle_t *handle, BMMCP_slave_state_t slave_state);

static ERRORS_return_t set_next_state(MOTOR_handle_t *handle, const MOTOR_STATE_handle_t * next_state)
{
	ERRORS_return_t result = ERRORS_ok;
	if (next_state != NULL) {
		const MOTOR_STATE_handle_t * current_state = (const MOTOR_STATE_handle_t *)handle->current_state;
		if (next_state != current_state) {
		    if (current_state->MOTOR_STATE_exit_func != NULL) {
		    	result = current_state->MOTOR_STATE_exit_func(handle);
		    }
		    if (result == ERRORS_ok) {
		    	if (next_state->MOTOR_STATE_entry_func != NULL) {
		    		result = next_state->MOTOR_STATE_entry_func(handle);
		    	}
		    }
		    handle->current_state = (const void *) next_state;
		}
	}
	return result;
}

static const void* common_state_transition(MOTOR_handle_t *handle, BMMCP_slave_state_t slave_state) {
	void * next_state = NULL;
	switch(slave_state) {

	case ICLWAIT:
	case STOP_IDLE:
	case STOP:
	case ANY_STOP:
		next_state = (void *)&c_MOTOR_STATE_stopping;
	break;

	case FAULT_NOW:
		next_state = (void *)&c_MOTOR_STATE_active_fault;
	break;

	case FAULT_OVER:
		//Exceptional case: if the acknowledge command has already been sent, ignore the fault telemetry
		if (handle->current_state != (void *)&c_MOTOR_STATE_reset_fault_command_sent) {
			next_state = (void *)&c_MOTOR_STATE_passive_fault;
		}
	break;

	case IDLE_START:
	case CHARGE_BOOT_CAP:
	case OFFSET_CALIB:
	case START:
	case START_RUN:
	case SWITCH_OVER:
		next_state = (void *)&c_MOTOR_STATE_Start_command_sent;
	break;

	case IDLE_ALIGNMENT:
	case ALIGN_CHARGE_BOOT_CAP:
	case ALIGN_OFFSET_CALIB:
	case ALIGNMENT:
		next_state = (void *)&c_MOTOR_STATE_alignment_in_progress;
	break;

	default:
	break;
	}
	return next_state;
}
void MOTOR_init(MOTOR_handle_t *handle, uint8_t motor_id, osEventFlagsId_t event_flag)
{
	handle->event_flag = event_flag;
	handle->motor_id = motor_id;
	handle->current_state = (void *)&c_MOTOR_STATE_unknown;
	handle->timeout_timer = NULL;
	handle->velocity_radps = 0.0f;
	((const MOTOR_STATE_handle_t *)handle->current_state)->MOTOR_STATE_entry_func(handle);
}
void MOTOR_deinit(MOTOR_handle_t *handle)
{
	handle->event_flag = NULL;
	handle->motor_id = 0;
	handle->current_state = NULL;
	handle->timeout_timer = NULL;
	handle->velocity_radps = 0.0f;
}

void MOTOR_register_timer(MOTOR_handle_t *handle, osTimerId_t timeout_timer)
{
	handle->timeout_timer = timeout_timer;
}

ERRORS_return_t MOTOR_command(MOTOR_handle_t *handle, MOTOR_user_command_t command)
{
	ERRORS_return_t result = ERRORS_ok;
	const MOTOR_STATE_handle_t * current_state = (const MOTOR_STATE_handle_t *)handle->current_state;
	if (current_state->MOTOR_STATE_user_command != NULL) {
		result = set_next_state(handle, current_state->MOTOR_STATE_user_command(command));
	}
	return result;
}

ERRORS_return_t MOTOR_new_message(MOTOR_handle_t *handle, BMMCP_universal_packet_t * packet)
{
	ERRORS_return_t result = ERRORS_ok;
	if (packet == NULL){
		return ERRORS_argument_error;
	}
	const MOTOR_STATE_handle_t * current_state = (const MOTOR_STATE_handle_t *)handle->current_state;
	const void* new_state = NULL;
	BMMCP_slave_state_t slave_state = 0;
	switch (packet->command) {
		case BMMCP_telemetry:
			handle->velocity_radps = BMMCP_velocity_to_si(packet->data.telemetry.velocity);

			if (handle->timeout_timer != NULL) {
    			if (osTimerStop(handle->timeout_timer) != osOK) {
    				result = ERRORS_os_error;
    				break;
    			}
    			if (osTimerStart(handle->timeout_timer, MOTOR_TELEMERY_TIMEOUT_TICKS) != osOK) {
    				result = ERRORS_os_error;
    				break;
    			}
			}

			slave_state = packet->data.telemetry.stm_state;
			new_state = common_state_transition(handle, slave_state);

			if (new_state == NULL) {
				if (current_state->MOTOR_STATE_new_slave_state != NULL) {
					new_state = current_state->MOTOR_STATE_new_slave_state(slave_state);
				}
			}
			result = set_next_state(handle, (const MOTOR_STATE_handle_t *)new_state);
		break;
		case BMMCP_response:
			if (packet->data.command_response == BMMCP_nack) {
				new_state = &c_MOTOR_STATE_unknown;
			} else if (current_state->MOTOR_STATE_slave_command_response != NULL) {
				new_state = current_state->MOTOR_STATE_slave_command_response(packet->data.command_response);
				result = set_next_state(handle, (const MOTOR_STATE_handle_t *)new_state);
			}
		break;
		default:
		break;
	}
	return result;
}

ERRORS_return_t MOTOR_get_state(MOTOR_handle_t *handle, MOTOR_state_t *state)
{
	return ERRORS_not_implemented;
}

ERRORS_return_t MOTOR_get_velocity(MOTOR_handle_t *handle, float *velocity_radps)
{
	return ERRORS_not_implemented;
}

ERRORS_return_t MOTOR_get_torque(MOTOR_handle_t *handle, float *torque_nm)
{
	return ERRORS_not_implemented;
}

ERRORS_return_t MOTOR_set_torque(MOTOR_handle_t *handle, float torque_nm)
{
	ERRORS_return_t result = ERRORS_ok;
	BMMCP_master_task_msg_t set_torque_msg = {
			.variant = BMMCP_MASTER_TASK_packet_send,
			.payload = {
					.packet = {
							.id = handle->motor_id,
							.command = BMMCP_set_current,
							.data.current = torque_nm,

					},
			},
	};
	if (osMessageQueuePut(SHARED_TASK_get_bmmcp_master_queue(), &set_torque_msg, 0, 0) != osOK) {
		result = ERRORS_os_error;
	}
	return result;
}
ERRORS_return_t MOTOR_timeout(MOTOR_handle_t *handle)
{
	ERRORS_return_t result = ERRORS_ok;
	LogError("Lost connection to motor %i", handle->motor_id);
	result = set_next_state(handle, &c_MOTOR_STATE_unknown);
	return result;
}

// State callback functions
static ERRORS_return_t MOTOR_STATE_unknown_entry(MOTOR_handle_t * handle)
{
	ERRORS_return_t result = ERRORS_ok;
	if ((osEventFlagsClear(handle->event_flag, MOTOR_EVENT_FLAG_ALL) & MOTOR_EVENT_FLAG_ALL) != 0) {
		result = ERRORS_os_error;
	}
	return result;
}
static ERRORS_return_t MOTOR_STATE_alignment_command_sent_entry(MOTOR_handle_t * handle)
{
	ERRORS_return_t result = ERRORS_ok;
	BMMCP_master_task_msg_t start_alignment_msg = {
			.variant = BMMCP_MASTER_TASK_packet_send,
			.payload = {
					.packet = {
							.id = handle->motor_id,
							.command = BMMCP_align_motor
					},
			},
	};
	if (osMessageQueuePut(SHARED_TASK_get_bmmcp_master_queue(), &start_alignment_msg, 0, 0) != osOK) {
		result = ERRORS_os_error;
	}
	return result;
}
static ERRORS_return_t MOTOR_STATE_idle_aligned_entry(MOTOR_handle_t * handle)
{
	ERRORS_return_t result = ERRORS_ok;
	if ((osEventFlagsSet(handle->event_flag, MOTOR_EVENT_FLAG_READY | MOTOR_EVENT_FLAG_ALIGNED) & (MOTOR_EVENT_FLAG_READY | MOTOR_EVENT_FLAG_ALIGNED)) == 0) {
		result = ERRORS_os_error;
	}
	return result;
}
static ERRORS_return_t MOTOR_STATE_torque_command_sent_entry(MOTOR_handle_t * handle)
{
	ERRORS_return_t result = ERRORS_ok;
	BMMCP_master_task_msg_t reset_torque_msg = {
			.variant = BMMCP_MASTER_TASK_packet_send,
			.payload = {
					.packet = {
							.id = handle->motor_id,
							.command = BMMCP_reset_torque
					},
			},
	};
	if (osMessageQueuePut(SHARED_TASK_get_bmmcp_master_queue(), &reset_torque_msg, 0, 0) != osOK) {
		result = ERRORS_os_error;
	}
	return result;
}
static ERRORS_return_t MOTOR_STATE_start_command_sent_entry(MOTOR_handle_t * handle)
{
	ERRORS_return_t result = ERRORS_ok;
	BMMCP_master_task_msg_t start_motor_msg = {
			.variant = BMMCP_MASTER_TASK_packet_send,
			.payload = {
					.packet = {
							.id = handle->motor_id,
							.command = BMMCP_start_motor,
					},
			},
	};
	if (osMessageQueuePut(SHARED_TASK_get_bmmcp_master_queue(), &start_motor_msg, 0, 0) != osOK) {
		result = ERRORS_os_error;
	}
	return result;
}
static ERRORS_return_t MOTOR_STATE_running_entry(MOTOR_handle_t * handle)
{
	ERRORS_return_t result = ERRORS_ok;
	if ((osEventFlagsSet(handle->event_flag, MOTOR_EVENT_FLAG_STARTED) & MOTOR_EVENT_FLAG_STARTED) == 0) {
		result = ERRORS_os_error;
	}
	return result;
}
static ERRORS_return_t MOTOR_STATE_stop_command_sent_entry(MOTOR_handle_t * handle)
{
	ERRORS_return_t result = ERRORS_ok;
	BMMCP_master_task_msg_t stop_motor_msg = {
			.variant = BMMCP_MASTER_TASK_packet_send,
			.payload = {
					.packet = {
							.id = handle->motor_id,
							.command = BMMCP_stop_motor,
					},
			},
	};
	if (osMessageQueuePut(SHARED_TASK_get_bmmcp_master_queue(), &stop_motor_msg, 0, 0) != osOK) {
		result = ERRORS_os_error;
	}
	return result;
}
static ERRORS_return_t MOTOR_STATE_faults_entry(MOTOR_handle_t * handle)
{
	ERRORS_return_t result = ERRORS_ok;
	if ((osEventFlagsSet(handle->event_flag, MOTOR_EVENT_FLAG_FAULT) & MOTOR_EVENT_FLAG_FAULT) == 0) {
		result = ERRORS_os_error;
	}
	return result;
}
static ERRORS_return_t MOTOR_STATE_reset_fault_sent_entry(MOTOR_handle_t * handle)
{
	ERRORS_return_t result = ERRORS_ok;
	BMMCP_master_task_msg_t acknowledge_fault_msg = {
			.variant = BMMCP_MASTER_TASK_packet_send,
			.payload = {
					.packet = {
							.id = handle->motor_id,
							.command = BMMCP_acknowledge_fault,
					},
			},
	};
	if (osMessageQueuePut(SHARED_TASK_get_bmmcp_master_queue(), &acknowledge_fault_msg, 0, 0) != osOK) {
		result = ERRORS_os_error;
	}
	return result;
}

//Exit functions
static ERRORS_return_t MOTOR_STATE_unknown_exit(MOTOR_handle_t * handle)
{
	ERRORS_return_t result = ERRORS_ok;
	if ((osEventFlagsSet(handle->event_flag, MOTOR_EVENT_FLAG_ALIVE) & MOTOR_EVENT_FLAG_ALIVE) == 0) {
		result = ERRORS_os_error;
	}
	return result;
}
static ERRORS_return_t MOTOR_STATE_idle_aligned_exit(MOTOR_handle_t * handle)
{
	ERRORS_return_t result = ERRORS_ok;
	volatile uint32_t flags = osEventFlagsClear(handle->event_flag, MOTOR_EVENT_FLAG_READY);
	flags = 0;
	if ((flags & MOTOR_EVENT_FLAG_READY) != 0) {
		result = ERRORS_os_error;
	}
	return result;
}
static ERRORS_return_t MOTOR_STATE_running_exit(MOTOR_handle_t * handle)
{
	ERRORS_return_t result = ERRORS_ok;
	if ((osEventFlagsClear(handle->event_flag, MOTOR_EVENT_FLAG_STARTED) & MOTOR_EVENT_FLAG_STARTED) != 0) {
		result = ERRORS_os_error;
	}
	return result;
}

static void * MOTOR_STATE_idle_not_aligned_user_command(MOTOR_user_command_t user_command)
{
	void * next_state = NULL;
	if (user_command == MOTOR_user_command_align) {
		next_state = (void *)&c_MOTOR_STATE_Alignment_command_sent;
	} else {
		LogWarn("Invalid user command received");
	}
	return next_state;
}
static void * MOTOR_STATE_running_user_command(MOTOR_user_command_t user_command)
{
	void * next_state = NULL;
	if (user_command == MOTOR_user_command_stop) {
		next_state = (void *)&c_MOTOR_STATE_stop_command_sent;
	} else {
		LogWarn("Invalid user command received");
	}
	return next_state;
}
static void * MOTOR_STATE_idle_aligned_user_command(MOTOR_user_command_t user_command)
{
	void * next_state = NULL;
	if (user_command == MOTOR_user_command_start) {
		next_state = (void *)&c_MOTOR_STATE_torque_command_sent;
	} else {
		LogWarn("Invalid user command received");
	}
	return next_state;
}
static void * MOTOR_STATE_passive_fault_user_command(MOTOR_user_command_t user_command)
{
	void * next_state = NULL;
	if (user_command == MOTOR_user_command_acknowledge_faults) {
		next_state = (void *)&c_MOTOR_STATE_reset_fault_command_sent;
	} else {
		LogWarn("Invalid user command received");
	}
	return next_state;
}

// command response received
static void * MOTOR_STATE_alignment_command_sent_command_response(BMMCP_response_t response)
{
	return (void *)&c_MOTOR_STATE_alignment_in_progress;
}
static void * MOTOR_STATE_torque_command_sent_command_response(BMMCP_response_t response)
{
	return (void *)&c_MOTOR_STATE_Start_command_sent;
}

// Slave state received function
static void * MOTOR_STATE_unknown_slave_state(BMMCP_slave_state_t slave_state)
{
	void * next_state = NULL;
	if (slave_state == IDLE) {
		next_state = (void *)&c_MOTOR_STATE_idle_not_aligned;
	} else {
		// Unknown state transition
	}
	return next_state;
}
static void * MOTOR_STATE_alignment_in_progress_slave_state(BMMCP_slave_state_t slave_state)
{
	void * next_state = NULL;
	if (slave_state == IDLE) {
		next_state = (void *)&c_MOTOR_STATE_idle_aligned;
	} else {
		// Unknown state transition
	}
	return next_state;
}
static void * MOTOR_STATE_start_command_sent_slave_state(BMMCP_slave_state_t slave_state)
{
	void * next_state = NULL;
	if (slave_state == RUN) {
		next_state = (void *)&c_MOTOR_STATE_running;
	} else {
		// Unknown state transition
	}
	return next_state;
}
static void * MOTOR_STATE_stop_command_sent_slave_state(BMMCP_slave_state_t slave_state)
{
	void * next_state = NULL;
	if (slave_state == IDLE) {
		next_state = (void *)&c_MOTOR_STATE_idle_aligned;
	} else {
		// Unknown state transition
	}
	return next_state;
}
static void * MOTOR_STATE_reset_fault_command_sent_slave_state(BMMCP_slave_state_t slave_state)
{
	void * next_state = NULL;
	if (slave_state == IDLE) {
		next_state = (void *)&c_MOTOR_STATE_idle_aligned;
	} else {
		// Unknown state transition
	}
	return next_state;
}

static void * MOTOR_STATE_stopping_slave_state(BMMCP_slave_state_t slave_state)
{
	void * next_state = NULL;
	if (slave_state == IDLE) {
		next_state = (void *)&c_MOTOR_STATE_idle_aligned;
	} else {
		// Unknown state transition
	}
	return next_state;
}
