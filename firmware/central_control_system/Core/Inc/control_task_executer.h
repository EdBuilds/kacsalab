/*
 * control_task_executer.h
 *
 *  Created on: Aug 5, 2021
 *      Author: tamas
 */

#ifndef INC_CONTROL_TASK_EXECUTER_H_
#define INC_CONTROL_TASK_EXECUTER_H_

#include "errors/errors.h"
#include "controller_types.h"
#include "stdint.h"

typedef struct {
    float setpoint_vector[CONTROL_state_num];
    uint32_t tick;
}CONTROL_TASK_handle_t;

ERRORS_return_t CONTROL_TASK_init(CONTROL_TASK_handle_t *handle);
ERRORS_return_t CONTROL_TASK_execute(CONTROL_TASK_handle_t *handle);
void CONTROL_TASK_deinit(CONTROL_TASK_handle_t *handle);


#endif /* INC_CONTROL_TASK_EXECUTER_H_ */
