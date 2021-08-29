/*
 * motor_object.h
 *
 *  Created on: Aug 3, 2021
 *      Author: tamas
 */

#ifndef SRC_MOTOR_OBJECT_H_
#define SRC_MOTOR_OBJECT_H_

#include "bmmcp/bmmcp_packet.h"
#include "object_types.h"
#include "errors/errors.h"
#include "stdint.h"
#include "motor_common_types.h"
#include "stddef.h"

#define MOTOR_NUM (2U)

typedef struct {
	float velocity;
	float torque;
	OBJ_data_state_t state;
} MOTOR_telem_si_t;

typedef void (*MOTOR_OBJ_timer_callback_func_t) (void *argument);
ERRORS_return_t MOTOR_OBJ_init(void);
ERRORS_return_t MOTOR_OBJ_deinit(void);
ERRORS_return_t MOTOR_OBJ_start(uint8_t id, uint32_t timeout);
ERRORS_return_t MOTOR_OBJ_stop(uint8_t id, uint32_t timeout);
ERRORS_return_t MOTOR_OBJ_take(uint8_t id);
ERRORS_return_t MOTOR_OBJ_release(uint8_t id);
ERRORS_return_t MOTOR_OBJ_get_state(uint8_t id, MOTOR_state_t * state);
ERRORS_return_t MOTOR_OBJ_get_telemetry(uint8_t id, MOTOR_telem_si_t * telemetry);
ERRORS_return_t MOTOR_OBJ_new_message(BMMCP_universal_packet_t * packet);
ERRORS_return_t MOTOR_OBJ_create_telemetry_timer(MOTOR_OBJ_timer_callback_func_t callback);
ERRORS_return_t MOTOR_OBJ_print_state(char *buffer, size_t buffer_len, size_t *used_buffer_space);
ERRORS_return_t MOTOR_OBJ_start_timer(uint8_t id);
ERRORS_return_t MOTOR_OBJ_lost_connection(uint8_t id);


#endif /* SRC_MOTOR_OBJECT_H_ */
