/*
 * frame_state_object.h
 *
 *  Created on: Aug 3, 2021
 *      Author: tamas
 */

#ifndef INC_FRAME_STATE_OBJECT_H_
#define INC_FRAME_STATE_OBJECT_H_

#include "stddef.h"
#include "object_types.h"
#include "errors/errors.h"

typedef enum {
	FRAME_roll= 0,
	FRAME_pitch,
	FRAME_yaw,
	FRAME_axis_num,
}FRAME_axis_t;

typedef struct {
	OBJ_data_state_t state;
	float data;
}FRAME_tracked_data_t;
typedef struct {
	FRAME_tracked_data_t angle;
	FRAME_tracked_data_t rate;
}FRAME_state_si_t;

ERRORS_return_t FRAME_init(void);
ERRORS_return_t FRAME_deinit(void);
ERRORS_return_t FRAME_get_state(FRAME_axis_t axis, FRAME_state_si_t *state);
ERRORS_return_t FRAME_set_angle(FRAME_axis_t axis, float angle_rad);
ERRORS_return_t FRAME_set_rate(FRAME_axis_t axis, float rate_radps);
ERRORS_return_t FRAME_print_state(char *buffer, size_t buffer_len, size_t *used_buffer_space);


#endif /* INC_FRAME_STATE_OBJECT_H_ */
