/*
 * motor_interface.h
 *
 *  Created on: Aug 2, 2021
 *      Author: tamas
 */

#ifndef INC_MOTOR_INTERFACE_H_
#define INC_MOTOR_INTERFACE_H_

#include "stdint.h"
#include "motor_common_types.h"

typedef enum {
	MOTOR_ok = 0,
	MOTOR_busy = 1,
}MOTOR_return_t;

typedef struct {
	uint8_t motor_id;
	float velocity_radps;
	MOTOR_state_t state;

}MOTOR_handle_t;

MOTOR_return_t MOTOR_init(MOTOR_handle_t *handle, uint8_t motor_id);
MOTOR_return_t MOTOR_start(MOTOR_handle_t *handle);
MOTOR_return_t MOTOR_get_state(MOTOR_handle_t *handle, MOTOR_state_t *state);
MOTOR_return_t MOTOR_get_velocity(MOTOR_handle_t *handle, float *velocity_radps);
MOTOR_return_t MOTOR_get_torque(MOTOR_handle_t *handle, float *torque_nm);
MOTOR_return_t MOTOR_set_torque(MOTOR_handle_t *handle, float torque_nm);

#endif /* INC_MOTOR_INTERFACE_H_ */
