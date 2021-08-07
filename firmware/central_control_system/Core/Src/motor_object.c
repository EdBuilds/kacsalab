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

static MOTOR_handle_t s_motor_handle[MOTOR_NUM];
static osThreadId_t s_motor_owner_thread_id[MOTOR_NUM];
static osMutexId_t s_motor_mutex[MOTOR_NUM];
static uint8_t s_motor_mutex_cb_container[MOTOR_NUM][80] = {0U};
static osMutexAttr_t s_motor_mutex_attributes[MOTOR_NUM] = {
		{.name = "Motor object mutex 1",
		.attr_bits = 0,
		.cb_mem = (void*)s_motor_mutex_cb_container[0],
		.cb_size = 80},
		{.name = "Motor object mutex 2",
		.attr_bits = 0,
		.cb_mem = (void*)s_motor_mutex_cb_container[1],
		.cb_size = 80},
};

ERRORS_return_t MOTOR_OBJ_init(void)
{
	for (uint8_t motor_idx = 0U; motor_idx < MOTOR_NUM; ++motor_idx){

		s_motor_handle[motor_idx].motor_id = motor_idx;
		s_motor_handle[motor_idx].state = MOTOR_state_unknown;
		s_motor_handle[motor_idx].velocity_radps = 0.0f;

		s_motor_mutex[motor_idx] = osMutexNew(&s_motor_mutex_attributes[motor_idx]);
		s_motor_owner_thread_id[motor_idx] = NULL;
	}

	for (uint8_t motor_idx = 0U; motor_idx < MOTOR_NUM; ++motor_idx) {
		if (s_motor_mutex[motor_idx] == NULL) {
			return ERRORS_os_error;
		}
	}
	return ERRORS_ok;
}
ERRORS_return_t MOTOR_OBJ_deinit(void) {return ERRORS_not_implemented;}
ERRORS_return_t MOTOR_OBJ_start(uint8_t id) {return ERRORS_not_implemented;}
ERRORS_return_t MOTOR_OBJ_stop(uint8_t id) {return ERRORS_not_implemented;}
ERRORS_return_t MOTOR_OBJ_take(uint8_t id)
{
	if (id >= MOTOR_NUM) {
		return ERRORS_argument_error;
	}

	if (osMutexAcquire(s_motor_mutex[id], 0U) != osOK) {
		return ERRORS_resource_busy;
	}

		if (s_motor_owner_thread_id[id] == NULL) {
			s_motor_owner_thread_id[id] = osThreadGetId();
		} else {
			return ERRORS_resource_busy;
		}

	if (osMutexRelease(s_motor_mutex[id]) != osOK) {
		return ERRORS_os_error;
	}

	return ERRORS_ok;
}

ERRORS_return_t MOTOR_OBJ_release(uint8_t id)
{
	if (id >= MOTOR_NUM) {
		return ERRORS_argument_error;
	}

	if (osMutexAcquire(s_motor_mutex[id], 0U) != osOK) {
		return ERRORS_resource_busy;
	}

		if (s_motor_owner_thread_id[id] == osThreadGetId()) {
			s_motor_owner_thread_id[id] = NULL;
		} else {
			return ERRORS_resource_busy;
		}

	if (osMutexRelease(s_motor_mutex[id]) != osOK) {
		return ERRORS_os_error;
	}

	return ERRORS_ok;
}

ERRORS_return_t MOTOR_OBJ_get_state(uint8_t id, MOTOR_state_t * state)
{
	return ERRORS_not_implemented;
}
ERRORS_return_t MOTOR_OBJ_get_telemetry(uint8_t id, MOTOR_telem_si_t * telemetry) {return ERRORS_not_implemented;}
ERRORS_return_t MOTOR_OBJ_new_message(BMMCP_universal_packet_t * packet) {return ERRORS_not_implemented;}

ERRORS_return_t MOTOR_OBJ_print_state(char *buffer, size_t buffer_len, size_t *used_buffer_space)
{
	return ERRORS_not_implemented;
}
