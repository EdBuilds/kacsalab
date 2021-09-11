/*
 * controller.h
 *
 *  Created on: Sep 12, 2021
 *      Author: tamas
 */

#ifndef INC_CONTROLLER_H_
#define INC_CONTROLLER_H_

typedef enum {
	CONTROL_motor_0_vel = 0,
	CONTROL_motor_1_vel,
	CONTROL_pitch_angle,
	CONTROL_roll_angle,
	CONTROL_pitch_rate,
	CONTROL_roll_rate,
	CONTROL_state_num,
}CONTROL_state_vector_elem_t;

static const float c_CONTROL_output_num = 2;
void run_state_space_controller(float * setpoint_vector, float * state_vector, float * output_vector);

#endif /* INC_CONTROLLER_H_ */
