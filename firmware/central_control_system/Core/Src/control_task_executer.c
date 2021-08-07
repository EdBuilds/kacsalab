/*
 * control_task_executer.c
 *
 *  Created on: Aug 5, 2021
 *      Author: tamas
 */

#include "control_task_executer.h"
#include "frame_state_object.h"
#include "motor_object.h"
#include "motor_interface.h"

ERRORS_return_t CONTROL_TASK_init(CONTROL_TASK_handle_t *handle)
{


}
ERRORS_return_t CONTROL_TASK_execute(CONTROL_TASK_handle_t *handle)
{
	float state_vector[4];
	//collect measurement data

	//pass measurement data to controller
	//get command from controller
	//feed command to motors

}


void CONTROL_TASK_deinit(CONTROL_TASK_handle_t *handle)
{


}
