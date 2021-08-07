/*
 * object_manager.c
 *
 *  Created on: Aug 6, 2021
 *      Author: tamas
 */

#include "object_manager.h"
#include "motor_object.h"
#include "frame_state_object.h"
#include "errors/errors.h"
#include "stddef.h"

typedef ERRORS_return_t (* OBJ_init_func_t ) (void);
typedef ERRORS_return_t (* OBJ_deinit_func_t ) (void);
typedef ERRORS_return_t (* OBJ_print_state_func_t ) (char *, size_t, size_t *);

typedef struct {
	const char *name;
	OBJ_init_func_t init;
	OBJ_deinit_func_t deinit;
	OBJ_print_state_func_t print_state;
}OBJ_handle_t;

static OBJ_handle_t s_OBJ_handles[OBJECT_NUM] = {
		{.name = "Frame",
		 .init = &FRAME_init,
		 .deinit = &FRAME_deinit,
		 .print_state = &FRAME_print_state,
		},

		{.name = "Motor",
		 .init = &MOTOR_OBJ_init,
		 .deinit = &MOTOR_OBJ_deinit,
		 .print_state = &MOTOR_OBJ_print_state,
		},
};

ERRORS_return_t OBJECT_MANAGER_init(void)
{
	ERRORS_return_t result = ERRORS_ok;
	for (uint8_t obj_idx = 0U; obj_idx < OBJECT_NUM; ++obj_idx) {
		result = s_OBJ_handles[obj_idx].init();
		if (result != ERRORS_ok) {
			break;
		}
	}
	return result;
}

const char * OBJECT_MANAGER_get_name(uint8_t obj_idx)
{
    return s_OBJ_handles[obj_idx].name;
}

ERRORS_return_t  OBJECT_MANAGER_print_state(uint8_t obj_idx, char *buffer, size_t buffer_len, size_t *used_buffer_space)
{
    return s_OBJ_handles[obj_idx].print_state(buffer, buffer_len, used_buffer_space);
}
