/*
 * console_commands.c
 *
 *  Created on: Aug 5, 2021
 *      Author: tamas
 */

#include "console_commands.h"
#include "errors/errors.h"
#include "FreeRTOS.h"
#include "FreeRTOS_CLI.h"
#include "stm32f3xx_hal.h"
#include "string.h"
#include "object_manager.h"
#include "console_command_helpers.h"
#include "stdlib.h"
#include "cmsis_os2.h"

#define COMMAND_NUM (3U)
#define GETOBJ_COMMAND_HELP_STRING_BUF_LEN (512U)
#define MAX_THREAD_NUM (10U)

static BaseType_t CONSOLE_COMMAND_reboot(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t CONSOLE_COMMAND_get_object(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t CONSOLE_COMMAND_os_thread_stats(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static char getobj_command_help_buf[GETOBJ_COMMAND_HELP_STRING_BUF_LEN] = {0};
CLI_Command_Definition_t commands[COMMAND_NUM] =
{
		{
				.pcCommand = "reboot",
				.pcHelpString = "reboot:\n reboots the system\r\n\r\n",
				.cExpectedNumberOfParameters = 0,
				.pxCommandInterpreter = &CONSOLE_COMMAND_reboot
		},
		{
				.pcCommand = "getobj",
				.pcHelpString = getobj_command_help_buf,
				.cExpectedNumberOfParameters = 1,
				.pxCommandInterpreter = &CONSOLE_COMMAND_get_object
		},
		{
				.pcCommand = "threads",
				.pcHelpString = "threads:\n Prints thread statistics: name, stack size and availabe stack space\r\n\r\n",
				.cExpectedNumberOfParameters = 0,
				.pxCommandInterpreter = &CONSOLE_COMMAND_os_thread_stats
		},
};
ERRORS_return_t register_console_commands(void)
{
	CC_HELPER_build_getobj_command_help(getobj_command_help_buf, GETOBJ_COMMAND_HELP_STRING_BUF_LEN);
	for (uint8_t command_idx = 0U; command_idx < COMMAND_NUM; ++command_idx) {
		if (FreeRTOS_CLIRegisterCommand(&(commands[command_idx])) != pdPASS) {
			return ERRORS_not_enough_memory;
		}
	}
	return ERRORS_ok;
}

static BaseType_t CONSOLE_COMMAND_reboot(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	HAL_NVIC_SystemReset();
	return pdFALSE;
}

static BaseType_t CONSOLE_COMMAND_get_object(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	char *pcParameter1;
	BaseType_t xParameter1StringLength;
	size_t used_buffer_space = 0;

	    /* Obtain the name of the source file, and the length of its name, from
	    the command string. The name of the source file is the first parameter. */
	    pcParameter1 = FreeRTOS_CLIGetParameter
	                        (
	                          /* The command string itself. */
	                          pcCommandString,
	                          /* Return the first parameter. */
	                          1,
	                          /* Store the parameter string length. */
	                          &xParameter1StringLength
	                        );

	    /* Terminate both file names. */
	    pcParameter1[ xParameter1StringLength ] = 0x00;

	    int32_t readable_idx = strtol(pcParameter1, (char **)NULL, 10);

	    if( readable_idx <= OBJECT_NUM && readable_idx > 0)
	    {
	        const uint8_t obj_idx = readable_idx-1;
	        ERRORS_return_t result = OBJECT_MANAGER_print_state(obj_idx, pcWriteBuffer, xWriteBufferLen, &used_buffer_space);
	        if (result != ERRORS_ok) {
	        	used_buffer_space += snprintf( pcWriteBuffer, xWriteBufferLen, "Failed to get " );
	        	used_buffer_space += snprintf( pcWriteBuffer+used_buffer_space, xWriteBufferLen-used_buffer_space, OBJECT_MANAGER_get_name(obj_idx));
	        	used_buffer_space += snprintf( pcWriteBuffer+used_buffer_space, xWriteBufferLen-used_buffer_space, " object state: ");
	        	used_buffer_space += snprintf( pcWriteBuffer+used_buffer_space, xWriteBufferLen-used_buffer_space, ERRORS_error_to_string(result));

	        }
	    }
	    else
	    {
	    	used_buffer_space += snprintf( pcWriteBuffer+used_buffer_space, xWriteBufferLen-used_buffer_space, "Invalid input index");
	    }

	    used_buffer_space += snprintf( pcWriteBuffer+used_buffer_space, xWriteBufferLen-used_buffer_space, "\r\n");
	    /* There is only a single line of output produced in all cases.  pdFALSE is
	    returned because there is no more output to be generated. */
	    return pdFALSE;
}

static BaseType_t CONSOLE_COMMAND_os_thread_stats(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{

	size_t used_buffer_space = 0;
	osThreadId_t thread_array[MAX_THREAD_NUM] ={0};
	uint32_t thread_num = 0;
	thread_num = osThreadEnumerate(thread_array, MAX_THREAD_NUM);
	for (uint32_t thread_idx = 0; thread_idx < thread_num; ++thread_idx) {
		const osThreadId_t thread_id = thread_array[thread_idx];
		used_buffer_space += snprintf( pcWriteBuffer+used_buffer_space, xWriteBufferLen-used_buffer_space, osThreadGetName(thread_id));
		used_buffer_space += snprintf( pcWriteBuffer+used_buffer_space, xWriteBufferLen-used_buffer_space, ":\n");
		used_buffer_space += snprintf( pcWriteBuffer+used_buffer_space, xWriteBufferLen-used_buffer_space, "  State: ");
		used_buffer_space += snprintf( pcWriteBuffer+used_buffer_space, xWriteBufferLen-used_buffer_space, CC_HELPER_os_thread_state_to_string(osThreadGetState(thread_id)));
		used_buffer_space += snprintf( pcWriteBuffer+used_buffer_space, xWriteBufferLen-used_buffer_space, "  Stack size: ");
		used_buffer_space += snprintf( pcWriteBuffer+used_buffer_space, xWriteBufferLen-used_buffer_space, "%d", osThreadGetStackSize(thread_id));
		used_buffer_space += snprintf( pcWriteBuffer+used_buffer_space, xWriteBufferLen-used_buffer_space, "  Stack space: ");
		used_buffer_space += snprintf( pcWriteBuffer+used_buffer_space, xWriteBufferLen-used_buffer_space, "%d", osThreadGetStackSpace(thread_id));
		used_buffer_space += snprintf( pcWriteBuffer+used_buffer_space, xWriteBufferLen-used_buffer_space, "\n");
	}
	return pdFALSE;
}
