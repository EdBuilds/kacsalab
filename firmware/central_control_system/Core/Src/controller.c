/*
 * controller.c
 *
 *  Created on: Sep 12, 2021
 *      Author: tamas
 */

#include "controller.h"
#include "controller_types.h"
#include "stm32f303x8.h"
#include "arm_math.h"
void CONTROLLER_state_space_run(float * setpoint_vector, float * state_vector, float * output_vector)
{
	arm_scale_f32(output_vector, 0.0f, output_vector, CONTROL_OUTPUT_NUM);
}
