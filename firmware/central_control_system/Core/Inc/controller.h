/*
 * controller.h
 *
 *  Created on: Sep 12, 2021
 *      Author: tamas
 */

#ifndef INC_CONTROLLER_H_
#define INC_CONTROLLER_H_

#define CONTROL_OUTPUT_NUM (2)
void CONTROLLER_state_space_run(float * setpoint_vector, float * state_vector, float * output_vector);

#endif /* INC_CONTROLLER_H_ */
