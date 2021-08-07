/*
 * control_task.c
 *
 *  Created on: Aug 5, 2021
 *      Author: tamas
 */
#include "control_task.h"
#include "control_task_executer.h"
#include "stdbool.h"
// TODO: init when it has members
static CONTROL_TASK_handle_t s_control_task_handle;
void start_control_task(void *argument) {

	if (CONTROL_TASK_init(&s_control_task_handle) == ERRORS_ok) {
		while (true) {
			if (CONTROL_TASK_execute(&s_control_task_handle) != ERRORS_ok) {
				break;
			}
		}
	}
	CONTROL_TASK_deinit(&s_control_task_handle);
}
