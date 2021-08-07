/*
 * console_command_helpers.h
 *
 *  Created on: Aug 7, 2021
 *      Author: tamas
 */

#ifndef INC_CONSOLE_COMMAND_HELPERS_H_
#define INC_CONSOLE_COMMAND_HELPERS_H_

#include "errors/errors.h"
#include "cmsis_os2.h"

ERRORS_return_t CC_HELPER_build_getobj_command_help(char * getobj_command_help_buf, unsigned buffer_size);
const char * CC_HELPER_os_thread_state_to_string(osThreadState_t thread_state);


#endif /* INC_CONSOLE_COMMAND_HELPERS_H_ */
