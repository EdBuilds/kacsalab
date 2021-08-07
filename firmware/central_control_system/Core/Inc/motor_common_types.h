/*
 * motor_common_types.h
 *
 *  Created on: Aug 5, 2021
 *      Author: tamas
 */

#ifndef INC_MOTOR_COMMON_TYPES_H_
#define INC_MOTOR_COMMON_TYPES_H_


typedef enum{
	MOTOR_state_unknown = 0,
	MOTOR_state_running = 1,
	MOTOR_state_fault = 2,
}MOTOR_state_t;


#endif /* INC_MOTOR_COMMON_TYPES_H_ */
