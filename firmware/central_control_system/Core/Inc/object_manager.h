/*
 * object_manager.h
 *
 *  Created on: Aug 6, 2021
 *      Author: tamas
 */

#ifndef INC_OBJECT_MANAGER_H_
#define INC_OBJECT_MANAGER_H_

#include "errors/errors.h"
#include "stdint.h"
#include "stddef.h"

#define OBJECT_NUM (2U)
ERRORS_return_t OBJECT_MANAGER_init(void);
const char * OBJECT_MANAGER_get_name(uint8_t obj_idx);
ERRORS_return_t OBJECT_MANAGER_print_state(uint8_t obj_idx, char *buffer, size_t buffer_len, size_t *used_buffer_space);

#endif /* INC_OBJECT_MANAGER_H_ */
