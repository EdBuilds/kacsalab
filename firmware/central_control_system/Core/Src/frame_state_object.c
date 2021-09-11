/*
 * frame_state_object.c
 *
 *  Created on: Aug 3, 2021
 *      Author: tamas
 */
#include "frame_state_object.h"
#include "cmsis_os2.h"
#include "string.h"

typedef struct {
}FRAME_state_tracker_t;

static const char * FRAME_axis_to_string(FRAME_axis_t axis);

static FRAME_state_si_t s_frame_state[FRAME_axis_num] = {0};
volatile static int32_t s_frame_scope_yaw_angle = 0;
volatile static int32_t s_frame_scope_pitch_angle = 0;
volatile static int32_t s_frame_scope_roll_angle = 0;
volatile static int32_t s_frame_scope_yaw_rate = 0;
volatile static int32_t s_frame_scope_pitch_rate = 0;
volatile static int32_t s_frame_scope_roll_rate = 0;
static osMutexId_t s_frame_mutex = NULL;
static uint8_t s_frame_mutex_cb_container[80] = {0U};

static osMutexAttr_t s_frame_mutex_attributes = {
		.name = "Frame state object mutex",
		.attr_bits = 0,
		.cb_mem = (void*)s_frame_mutex_cb_container,
		.cb_size = 80
};

static const char * FRAME_axis_to_string(FRAME_axis_t axis)
{
	switch (axis) {
	case FRAME_pitch:
		return "pitch";
	break;
	case FRAME_roll:
		return "roll";
	break;
	case FRAME_yaw:
		return "yaw";
	break;
	default:
	break;
	}
	return "";
}
ERRORS_return_t FRAME_init(void)
{
	ERRORS_return_t result = ERRORS_ok;

	for (uint8_t axis_idx = 0U; axis_idx < FRAME_axis_num; ++axis_idx){
		s_frame_state[axis_idx].angle.state = OBJ_data_not_available;
		s_frame_state[axis_idx].angle.data = 0.0f;
		s_frame_state[axis_idx].rate.state = OBJ_data_not_available;
		s_frame_state[axis_idx].rate.data = 0.0f;
	}

	s_frame_mutex = osMutexNew (&s_frame_mutex_attributes);
	if (s_frame_mutex == NULL) {
		result = ERRORS_os_error;
	}
	return result;
}
ERRORS_return_t FRAME_deinit(void)
{
	ERRORS_return_t result = ERRORS_ok;
	if (osMutexDelete(s_frame_mutex) != osOK) {
		result = ERRORS_os_error;
	}
	s_frame_mutex = NULL;
	return result;

}
ERRORS_return_t FRAME_get_state(FRAME_axis_t axis, FRAME_state_si_t *state, uint32_t timeout)
{
	ERRORS_return_t result = ERRORS_ok;
	if (axis >= FRAME_axis_num && state != NULL) {
		result = ERRORS_argument_error;
	} else {
		if (osMutexAcquire(s_frame_mutex, timeout) == osOK) {
			memcpy(state, &(s_frame_state[axis]), sizeof(FRAME_state_si_t));
			s_frame_state[axis].angle.state = OBJ_no_new_data;
			s_frame_state[axis].rate.state = OBJ_no_new_data;
			if (osMutexRelease(s_frame_mutex) != osOK) {
				result = ERRORS_resource_busy;
			}
		} else {
			result = ERRORS_os_error;
		}
	}
	return result;

}
ERRORS_return_t FRAME_set_angle(FRAME_axis_t axis, float angle_rad)
{
	ERRORS_return_t result = ERRORS_ok;
	if (axis >= FRAME_axis_num) {
		result = ERRORS_argument_error;
	} else {
		if (osMutexAcquire(s_frame_mutex, 0U) == osOK) {
			switch(axis) {
			case FRAME_roll:
				s_frame_scope_roll_angle = angle_rad * 1000.0;
			break;
			case FRAME_pitch:
				s_frame_scope_pitch_angle = angle_rad * 1000.0;
			break;
			case FRAME_yaw:
				s_frame_scope_yaw_angle = angle_rad * 1000.0;
			break;
			default:
			break;
			}
			s_frame_state[axis].angle.data = angle_rad;
			s_frame_state[axis].angle.state = OBJ_new_data;
			if (osMutexRelease(s_frame_mutex) != osOK) {
				result = ERRORS_resource_busy;
			}
		} else {
			result = ERRORS_os_error;
		}
	}
	return result;

}
ERRORS_return_t FRAME_set_rate(FRAME_axis_t axis, float rate_radps)
{
	ERRORS_return_t result = ERRORS_ok;
	if (axis >= FRAME_axis_num) {
		result = ERRORS_argument_error;
	} else {
		if (osMutexAcquire(s_frame_mutex, 0U) == osOK) {
			switch(axis) {
			case FRAME_roll:
				s_frame_scope_roll_rate = rate_radps * 1000.0;
			break;
			case FRAME_pitch:
				s_frame_scope_pitch_rate = rate_radps * 1000.0;
			break;
			case FRAME_yaw:
				s_frame_scope_yaw_rate = rate_radps * 1000.0;
			break;
			default:
			break;
			}
			s_frame_state[axis].rate.data = rate_radps;
			s_frame_state[axis].rate.state = OBJ_new_data;
			if (osMutexRelease(s_frame_mutex) != osOK) {
				result = ERRORS_os_error;
			}
		} else {
			result = ERRORS_resource_busy;
		}
	}
	return result;

}
ERRORS_return_t FRAME_print_state(char *buffer, size_t buffer_len, size_t *used_buffer_space)
{
	ERRORS_return_t result = ERRORS_ok;

	if (osMutexAcquire(s_frame_mutex, 0U) == osOK) {

		for (uint8_t axis_idx = 0U; axis_idx < FRAME_axis_num; ++axis_idx){
			*used_buffer_space += snprintf( buffer+*used_buffer_space, buffer_len-*used_buffer_space, FRAME_axis_to_string(axis_idx));
			*used_buffer_space += snprintf( buffer+*used_buffer_space, buffer_len-*used_buffer_space, ":\n");
			*used_buffer_space += snprintf( buffer+*used_buffer_space, buffer_len-*used_buffer_space, "  angle: %d", (int)(s_frame_state[axis_idx].angle.data*100.0));
			*used_buffer_space += snprintf( buffer+*used_buffer_space, buffer_len-*used_buffer_space, "  rate: %d", (int)(s_frame_state[axis_idx].rate.data*100.0));
			*used_buffer_space += snprintf( buffer+*used_buffer_space, buffer_len-*used_buffer_space, "\n");
		}

		if (osMutexRelease(s_frame_mutex) != osOK) {
			result = ERRORS_os_error;
		}
	} else {
		result = ERRORS_resource_busy;
	}
	return result;
}
