/*
 * motor_object.c
 *
 *  Created on: Aug 5, 2021
 *      Author: tamas
 */

#include "motor_object.h"
#include "errors/errors.h"
#include "stddef.h"
#include "cmsis_os2.h"
#include "motor_interface.h"
#include "cmsis_cb_sizes.h"
#include "logging.h"
#include "motor_interface.h"

static MOTOR_handle_t s_motor_handle[MOTOR_NUM];

static osThreadId_t s_motor_owner_thread_id[MOTOR_NUM];
static uint8_t s_timer_callback_args[MOTOR_NUM] = {0U, 1U};

static osMutexId_t s_motor_mutex[MOTOR_NUM];
static uint8_t s_motor_mutex_cb_container[MOTOR_NUM][80] = {0U};
static const osMutexAttr_t s_motor_mutex_attributes[MOTOR_NUM] = {
		{.name = "Motor object mutex 1",
		.attr_bits = 0,
		.cb_mem = (void*)s_motor_mutex_cb_container[0],
		.cb_size = 80},
		{.name = "Motor object mutex 2",
		.attr_bits = 0,
		.cb_mem = (void*)s_motor_mutex_cb_container[1],
		.cb_size = 80},
};

static osTimerId_t s_motor_telemetry_timeout_timer[MOTOR_NUM];
static uint8_t s_motor_timer_cb_container[MOTOR_NUM][OS_TIMER_CB_SIZE] = {0U};
static const osTimerAttr_t s_motor_telemetry_timeout_timer_attributes[MOTOR_NUM] = {
		{
				.name = "motor telem timeout 1",
				.attr_bits = 0,
				.cb_mem = (void*)s_motor_timer_cb_container[0],
				.cb_size = OS_TIMER_CB_SIZE,
		},
		{
				.name = "motor telem timeout 2",
				.attr_bits = 0,
				.cb_mem = (void*)s_motor_timer_cb_container[1],
				.cb_size = OS_TIMER_CB_SIZE,
		},
};

static osEventFlagsId_t s_motor_event_flags[MOTOR_NUM];
static uint8_t s_motor_event_flag_cb_container[MOTOR_NUM][OS_EVENT_FLAG_CB_SIZE] = {0U};
static const osEventFlagsAttr_t s_motor_event_flags_attributes[MOTOR_NUM] = {
		{
				.name = "motor 1 event flags",
				.attr_bits = 0,
				.cb_mem = (void*)s_motor_event_flag_cb_container[0],
				.cb_size = OS_EVENT_FLAG_CB_SIZE,
		},
		{
				.name = "motor 2 event flags",
				.attr_bits = 0,
				.cb_mem = (void*)s_motor_event_flag_cb_container[1],
				.cb_size = OS_EVENT_FLAG_CB_SIZE,
		},
};

static ERRORS_return_t MOTOR_OBJ_wait_for_flag(uint8_t id, uint32_t flag, uint32_t timeout);
static ERRORS_return_t MOTOR_OBJ_send_user_cmd(uint8_t id, MOTOR_user_command_t command, uint32_t timeout);

static ERRORS_return_t MOTOR_OBJ_send_user_cmd(uint8_t id, MOTOR_user_command_t command, uint32_t timeout)
{
	ERRORS_return_t result = ERRORS_ok;

	if (osMutexAcquire(s_motor_mutex[id], timeout) != osOK) {
		result = ERRORS_resource_busy;
	} else {
		result = MOTOR_command(&(s_motor_handle[id]), command);

		if (osMutexRelease(s_motor_mutex[id]) != osOK) {
			result = ERRORS_os_error;
		}
	}
	return result;
}

static ERRORS_return_t MOTOR_OBJ_wait_for_flag(uint8_t id, uint32_t flag, uint32_t timeout)
{
	ERRORS_return_t result = ERRORS_ok;
	uint32_t active_flags = osEventFlagsWait(s_motor_event_flags[id], flag | MOTOR_EVENT_FLAG_FAULT, osFlagsWaitAny | osFlagsNoClear, timeout);
	if ((active_flags & flag) == 0) {
		if ((active_flags & MOTOR_EVENT_FLAG_FAULT) != 0) {
			result = ERRORS_fault;
		} else {
			result = ERRORS_timeout;
		}
	}
	return result;
}
ERRORS_return_t MOTOR_OBJ_init(void)
{
	for (uint8_t motor_idx = 0U; motor_idx < MOTOR_NUM; ++motor_idx){

		s_motor_mutex[motor_idx] = osMutexNew(&s_motor_mutex_attributes[motor_idx]);

		if (s_motor_mutex[motor_idx] == NULL) {
			return ERRORS_os_error;
		}
		s_motor_event_flags[motor_idx] = osEventFlagsNew(&s_motor_event_flags_attributes[motor_idx]);
		s_motor_owner_thread_id[motor_idx] = NULL;
		s_motor_telemetry_timeout_timer[motor_idx] = NULL;
		MOTOR_init(&(s_motor_handle[motor_idx]), motor_idx, s_motor_event_flags[motor_idx]);
	}
	return ERRORS_ok;
}
ERRORS_return_t MOTOR_OBJ_deinit(void) {

	for (uint8_t motor_idx = 0U; motor_idx < MOTOR_NUM; ++motor_idx){
		MOTOR_deinit(&(s_motor_handle[motor_idx]));
		osMutexDelete(s_motor_mutex[motor_idx]);
		osTimerDelete(s_motor_telemetry_timeout_timer[motor_idx]);
		s_motor_owner_thread_id[motor_idx] = NULL;
	}
	return ERRORS_ok;
}
ERRORS_return_t MOTOR_OBJ_start(uint8_t id, uint32_t timeout)
{
	ERRORS_return_t result = ERRORS_ok;
	result = MOTOR_OBJ_wait_for_flag(id, MOTOR_EVENT_FLAG_ALIVE, timeout);

	if (result == ERRORS_ok) {

		if (!(osEventFlagsGet(s_motor_event_flags[id]) & MOTOR_EVENT_FLAG_ALIGNED)) {

			result = MOTOR_OBJ_send_user_cmd(id, MOTOR_user_command_align, timeout);

			if (result == ERRORS_ok) {
				result = MOTOR_OBJ_wait_for_flag(id, MOTOR_EVENT_FLAG_ALIGNED, timeout);
			}
		}

		if (result == ERRORS_ok) {

			if (!(osEventFlagsGet(s_motor_event_flags[id]) & MOTOR_EVENT_FLAG_STARTED)) {
				result = MOTOR_OBJ_send_user_cmd(id, MOTOR_user_command_start, timeout);

				if (result == ERRORS_ok) {
					result = MOTOR_OBJ_wait_for_flag(id, MOTOR_EVENT_FLAG_STARTED, timeout);
				}
			}

		}
	}
	return result;
}
ERRORS_return_t MOTOR_OBJ_stop(uint8_t id, uint32_t timeout)
{
	ERRORS_return_t result = ERRORS_ok;

	if ((osEventFlagsGet(s_motor_event_flags[id]) & MOTOR_EVENT_FLAG_STARTED)) {
		result = MOTOR_OBJ_send_user_cmd(id, MOTOR_user_command_stop, timeout);

		if (result == ERRORS_ok) {
			result = MOTOR_OBJ_wait_for_flag(id, MOTOR_EVENT_FLAG_READY, timeout);
		}
	}
	return result;
}
ERRORS_return_t MOTOR_OBJ_take(uint8_t id)
{
	ERRORS_return_t result = ERRORS_ok;
	const osThreadId_t current_thread = osThreadGetId();
	if (id >= MOTOR_NUM) {
		result = ERRORS_argument_error;
	} else if (osMutexAcquire(s_motor_mutex[id], 0U) != osOK) {
		result = ERRORS_resource_busy;
	} else {
		if (s_motor_owner_thread_id[id] != NULL) {
			result = ERRORS_resource_busy;
		} else if (current_thread == NULL){
			result = ERRORS_os_error;
		} else {
			s_motor_owner_thread_id[id] = current_thread;
		}
		if (osMutexRelease(s_motor_mutex[id]) != osOK) {
			result = ERRORS_os_error;
		}
	}
	return result;
}

ERRORS_return_t MOTOR_OBJ_release(uint8_t id)
{
	ERRORS_return_t result = ERRORS_ok;
	const osThreadId_t current_thread = osThreadGetId();
	if (id >= MOTOR_NUM) {
		result = ERRORS_argument_error;
	} else if (osMutexAcquire(s_motor_mutex[id], 0U) != osOK) {
		result = ERRORS_resource_busy;
	} else {
		if (current_thread == NULL) {
			result = ERRORS_os_error;
		} else if (s_motor_owner_thread_id[id] != current_thread){
			result = ERRORS_resource_busy;
		} else {
			s_motor_owner_thread_id[id] = NULL;
		}
		if (osMutexRelease(s_motor_mutex[id]) != osOK) {
			result = ERRORS_os_error;
		}
	}
	return result;
}

ERRORS_return_t MOTOR_OBJ_get_state(uint8_t id, MOTOR_state_t * state)
{
	return ERRORS_not_implemented;
}
ERRORS_return_t MOTOR_OBJ_get_telemetry(uint8_t id, MOTOR_telem_si_t * telemetry) {return ERRORS_not_implemented;}
ERRORS_return_t MOTOR_OBJ_new_message(BMMCP_universal_packet_t * packet)
{
	ERRORS_return_t result = ERRORS_ok;
	if (packet == NULL) {
		result = ERRORS_argument_error;
	} else if (packet->id >= MOTOR_NUM) {
		result = ERRORS_argument_error;
	} else if (osMutexAcquire(s_motor_mutex[packet->id], 0U) != osOK) {
		result = ERRORS_resource_busy;
	}else {

		result = MOTOR_new_message(&(s_motor_handle[packet->id]), packet);

		if (osMutexRelease(s_motor_mutex[packet->id]) != osOK) {
			result = ERRORS_os_error;
		}
	}
	return result;
}

ERRORS_return_t MOTOR_OBJ_print_state(char *buffer, size_t buffer_len, size_t *used_buffer_space)
{
	return ERRORS_not_implemented;
}

ERRORS_return_t MOTOR_OBJ_create_telemetry_timer(MOTOR_OBJ_timer_callback_func_t callback)
{
	ERRORS_return_t result = ERRORS_ok;

	for (uint8_t motor_idx = 0U; motor_idx < MOTOR_NUM; ++motor_idx) {
		s_motor_telemetry_timeout_timer[motor_idx] = osTimerNew((osTimerFunc_t)callback, osTimerOnce ,
				(void*)&(s_timer_callback_args[motor_idx]), &(s_motor_telemetry_timeout_timer_attributes[motor_idx]));
		if (s_motor_telemetry_timeout_timer[motor_idx] == NULL) {
			result = ERRORS_os_error;
			break;
		}
		MOTOR_register_timer(&(s_motor_handle[motor_idx]), s_motor_telemetry_timeout_timer[motor_idx]);
	}
	return result;
}
ERRORS_return_t MOTOR_OBJ_start_timer(uint8_t id)
{
	if (osTimerStart(s_motor_telemetry_timeout_timer[id], 1000) != osOK) {
		return ERRORS_os_error;
	}
	return ERRORS_ok;
}

ERRORS_return_t MOTOR_OBJ_lost_connection(uint8_t id)
{
	ERRORS_return_t result = ERRORS_ok;

	if (osMutexAcquire(s_motor_mutex[id], osWaitForever) != osOK) {
		result = ERRORS_os_error;
	} else {
		result = MOTOR_timeout(&(s_motor_handle[id]));

		if (osMutexRelease(s_motor_mutex[id]) != osOK) {
			result = ERRORS_os_error;
		}
	}
		//TODO handle state transitions
	return result;
}
