/*
 * console_command_helpers.c
 *
 *  Created on: Aug 7, 2021
 *      Author: tamas
 */

#include "console_command_helpers.h"
#include "string.h"
#include "object_manager.h"
#include "stdlib.h"

ERRORS_return_t CC_HELPER_build_getobj_command_help(char * getobj_command_help_buf, unsigned buffer_size)
{
	memset(getobj_command_help_buf, 0x00, buffer_size);
	strcat(getobj_command_help_buf, "getobj [IDX]: \n Prints the state of an object.\n");
	strcat(getobj_command_help_buf, " Available objects:\n");
	for (uint8_t obj_idx = 0; obj_idx < OBJECT_NUM; ++obj_idx) {
		char idx_string[4] = {0};
		snprintf(idx_string, 4, "%u", obj_idx+1);
		strcat(getobj_command_help_buf, "  ");
		strcat(getobj_command_help_buf, idx_string);
		strcat(getobj_command_help_buf, ": ");
		strcat(getobj_command_help_buf, OBJECT_MANAGER_get_name(obj_idx));
		strcat(getobj_command_help_buf, "\r\n");
	}
	strcat(getobj_command_help_buf, "\r\n");
	// TODO: this buffer can overflow
	return ERRORS_ok;
}

const char * CC_HELPER_os_thread_state_to_string(osThreadState_t thread_state)
{
	switch (thread_state) {
	case osThreadInactive:
		return "Inactive";
	break;
	case osThreadReady:
		return "Ready";
	break;
	case osThreadRunning:
		return "Running";
	break;
	case osThreadBlocked:
		return "Blocked";
	break;
	case osThreadTerminated:
		return "Terminated";
	break;
	case osThreadError:
		return "Error";
	break;
	default:
		return "Unknown os state";
	break;
	}
	return "";
}
