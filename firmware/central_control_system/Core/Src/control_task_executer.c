/*
 * control_task_executer.c
 *
 *  Created on: Aug 5, 2021
 *      Author: tamas
 */
#include "stdbool.h"
#include "control_task_executer.h"
#include "frame_state_object.h"
#include "motor_object.h"
#include "motor_interface.h"
#include "logging.h"
#include "errors/errors.h"
#include "controller.h"

static const uint32_t c_short_timeout = 10U;
static float s_state_setpoint[CONTROL_state_num] = {0.0f};

ERRORS_return_t CONTROL_TASK_init(CONTROL_TASK_handle_t *handle)
{

	ERRORS_return_t result = ERRORS_ok;

    for (uint8_t state_idx = 0U; state_idx < CONTROL_state_num; ++state_idx) {
    	s_state_setpoint[state_idx] = 0.0f;
    }

    for (uint8_t idx = 0U; idx < MOTOR_NUM; ++idx) {
	    result = MOTOR_OBJ_take(idx);
        if (result == ERRORS_ok) {
        	result = MOTOR_OBJ_start(idx, osWaitForever);
        	if (result == ERRORS_ok) {

        	} else {
        		LogError("Could not start the motor idx: %i, %s", idx, ERRORS_error_to_string(result));
        	}
        } else {
        	LogError("Could not take the motor resource idx: %i, %s", idx, ERRORS_error_to_string(result));
        }
    }
    return result;
}

ERRORS_return_t CONTROL_TASK_execute(CONTROL_TASK_handle_t *handle)
{
	ERRORS_return_t result = ERRORS_initial_error;
	bool abort_loop = false;
	float state_vector[CONTROL_state_num] = {0};
	float output_vector[c_CONTROL_output_num] = {0};
	MOTOR_telem_si_t telemetries[MOTOR_NUM] = {0};
	FRAME_state_si_t frame_state[FRAME_axis_num] = {0};

	//collect motor telemetry data
    for (uint8_t idx = 0U; idx < MOTOR_NUM; ++idx) {
    	MOTOR_state_t state;
	    result = MOTOR_OBJ_get_state(idx, &state, c_short_timeout);
	    if (result != ERRORS_ok) {
        	LogError("Could not get motor state, %s", ERRORS_error_to_string(result));
	    } else {
	    	if (state != MOTOR_state_running) {
	    		LogError("Motor idx:%i is in fault state", idx);
	    	} else {
	    		result = MOTOR_OBJ_get_telemetry(idx, &(telemetries[idx]), c_short_timeout);
	    		if (result == ERRORS_ok) {
    	    		if (telemetries[idx].state == OBJ_data_not_available) {
    	    			LogError("Motor idx:%i has no telemetry yet, skipping loop", idx);
    	    			abort_loop = true;
    	    		} else if (telemetries[idx].state == OBJ_no_new_data) {
    	    			LogWarn("Motor idx:%i has no new telemetry", idx);
    	    		}
	    		} else {

	    		}
	    	}
	    }
    }

    if ((result != ERRORS_ok) | abort_loop) {
    	return result;
    }

    for (uint8_t idx = 0U; idx < FRAME_axis_num; ++idx) {
    	result = FRAME_get_state(idx, &(frame_state[idx]), c_short_timeout);

    	if (result != ERRORS_ok) {
        	LogError("Could not get frame state, %s", ERRORS_error_to_string(result));
    	} else {    if ((result != ERRORS_ok) | abort_loop) {
        	return result;
        }


        	if (frame_state[idx].angle.state == OBJ_data_not_available) {
        	    LogError("Frame axis idx:%i has no angle yet, skipping loop", idx);
        	    abort_loop = true;
    	    } else if (frame_state[idx].angle.state == OBJ_no_new_data) {
    	    	LogWarn("Frame axis idx:%i has no new angle", idx);
    	    }
        	if (frame_state[idx].rate.state == OBJ_data_not_available) {
        	    LogError("Frame axis idx:%i has no rate yet, skipping loop", idx);
        	    abort_loop = true;
    	    } else if (frame_state[idx].rate.state == OBJ_no_new_data) {
    	    	LogWarn("Frame axis idx:%i has no new rate", idx);
    	    }
    	}

    }

    if ((result != ERRORS_ok) | abort_loop) {
    	return result;
    }

	state_vector[CONTROL_motor_0_vel] = telemetries[0].velocity;
	state_vector[CONTROL_motor_1_vel] = telemetries[1].velocity;
	state_vector[CONTROL_pitch_angle] = frame_state[FRAME_pitch].angle.data;
	state_vector[CONTROL_roll_angle] = frame_state[FRAME_roll].angle.data;
	state_vector[CONTROL_pitch_rate] = frame_state[FRAME_pitch].rate.data;
	state_vector[CONTROL_roll_rate] = frame_state[FRAME_roll].rate.data;


	//pass measurement data to controller
	//get command from controller
	//feed command to motors

    return result;
}


void CONTROL_TASK_deinit(CONTROL_TASK_handle_t *handle)
{

	ERRORS_return_t result = ERRORS_initial_error;
    LogInfo("Deinitializing control task");
    for (uint8_t idx = 0; idx < MOTOR_NUM; ++idx) {
    	result = MOTOR_OBJ_stop(idx, c_short_timeout);
    	if (result != ERRORS_ok) {
    		LogError("Could not stop motor idx:%i, %s", idx, ERRORS_error_to_string(result));
    	}

    	result = MOTOR_OBJ_release(idx);
    	if (result != ERRORS_ok) {
    		LogError("Could not release motor resource idx:%i, %s", idx, ERRORS_error_to_string(result));
    	}
    }

}
