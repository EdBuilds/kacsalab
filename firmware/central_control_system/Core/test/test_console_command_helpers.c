/*
 * test_console_command_helpers.c
 *
 *  Created on: Aug 7, 2021
 *      Author: tamas
 */

// Unit test framework
#include "unity.h"
#include "mock_object_manager.h"
#include "stdlib.h"
#include "string.h"
#include "console_command_helpers.h"

#define TEST_BUFFER_LENGTH (512)
void test_getobj_helper_string_build(void)
{
	char test_getobj_command_help_buf[TEST_BUFFER_LENGTH] = {0};
	OBJECT_MANAGER_get_name_ExpectAndReturn(0,"Motor");
	OBJECT_MANAGER_get_name_ExpectAndReturn(1,"Frame");
	CC_HELPER_build_getobj_command_help(test_getobj_command_help_buf, TEST_BUFFER_LENGTH);
	TEST_ASSERT_EQUAL_STRING("getobj [IDX]: \nPrints the state of an object.\nAvailable objects:\n  0: Motor\n  1: Frame\n",test_getobj_command_help_buf);
}
