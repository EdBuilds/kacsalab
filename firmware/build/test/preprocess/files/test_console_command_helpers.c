#include "build/temp/_test_console_command_helpers.c"
#include "console_command_helpers.h"
#include "mock_object_manager.h"
#include "unity.h"




void test_getobj_helper_string_build(void)

{

 char test_getobj_command_help_buf[(512)] = {0};

 OBJECT_MANAGER_get_name_CMockExpectAndReturn(19, 0, "Motor");

 OBJECT_MANAGER_get_name_CMockExpectAndReturn(20, 1, "Frame");

 CC_HELPER_build_getobj_command_help(test_getobj_command_help_buf, (512));

 UnityAssertEqualString((const char*)(("getobj [IDX]: \nPrints the state of an object.\nAvailable objects:\n  0: Motor\n  1: Frame\n")), (const char*)((test_getobj_command_help_buf)), (

((void *)0)

), (UNITY_UINT)(22));

}
