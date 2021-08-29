#include "build/temp/_test_motor_obj.c"
#include "motor_object.h"
#include "mock_motor_interface.h"
#include "mock_motor_common_types.h"
#include "mock_shared_task_resources.h"
#include "mock_log_interface_config.h"
#include "mock_logging.h"
#include "mock_logging_config.h"
#include "mock_bmmcp_common.h"
#include "mock_cmsis_os2.h"
#include "unity.h"




static const osMutexId_t test_mutexes[(2U)] = {(void *)0x01020304, (void *)0x10203040};

static const osMutexId_t test_event_flags[(2U)] = {(void *)0x11121314, (void *)0x11213141};

static const osThreadId_t test_thread_id = (void *)0xDEADBEEF;

static const uint8_t test_motor_id = 1;



static void helper_take_motor(uint8_t motor_id, osThreadId_t thread_id);

static void helper_take_motor(uint8_t motor_id, osThreadId_t thread_id)

{

 ERRORS_return_t result = ERRORS_ok;

 osThreadGetId_CMockExpectAndReturn(30, thread_id);

 osMutexAcquire_CMockExpectAndReturn(31, test_mutexes[test_motor_id], 0U, osOK);

 osMutexRelease_CMockExpectAndReturn(32, test_mutexes[test_motor_id], osOK);

 result = MOTOR_OBJ_take(motor_id);

 UnityAssertEqualNumber((UNITY_INT)((result)), (UNITY_INT)((ERRORS_ok)), (

((void *)0)

), (UNITY_UINT)(34), UNITY_DISPLAY_STYLE_INT);



}



void setUp(void)

{

 for (uint8_t motor_idx; motor_idx<(2U); ++motor_idx) {

  osMutexNew_CMockExpectAndReturn(41, 

 ((void *)0)

 , test_mutexes[motor_idx]);

  osMutexNew_CMockIgnoreArg_attr(42);

  osEventFlagsNew_CMockExpectAndReturn(43, 

 ((void *)0)

 , test_event_flags[motor_idx]);

  osEventFlagsNew_CMockIgnoreArg_attr(44);

  MOTOR_init_CMockExpect(45, 

 ((void *)0)

 , motor_idx, test_event_flags[motor_idx]);

  MOTOR_init_CMockIgnoreArg_handle(46);

 }

 MOTOR_OBJ_init();

}

void test_take_motor_sucsess(void)

{

 ERRORS_return_t result = ERRORS_ok;



 helper_take_motor(test_motor_id, test_thread_id);



 osThreadGetId_CMockExpectAndReturn(56, test_thread_id + 1);

 osMutexAcquire_CMockExpectAndReturn(57, test_mutexes[test_motor_id], 0U, osOK);

 osMutexRelease_CMockExpectAndReturn(58, test_mutexes[test_motor_id], osOK);

 result = MOTOR_OBJ_take(test_motor_id);

 UnityAssertEqualNumber((UNITY_INT)((result)), (UNITY_INT)((ERRORS_resource_busy)), (

((void *)0)

), (UNITY_UINT)(60), UNITY_DISPLAY_STYLE_INT);

}

void test_take_motor_fail(void)

{

 ERRORS_return_t result = ERRORS_ok;

 osThreadGetId_CMockExpectAndReturn(65, 

((void *)0)

);

 osMutexAcquire_CMockExpectAndReturn(66, test_mutexes[test_motor_id], 0U, osOK);

 osMutexRelease_CMockExpectAndReturn(67, test_mutexes[test_motor_id], osOK);

 result = MOTOR_OBJ_take(test_motor_id);

 UnityAssertEqualNumber((UNITY_INT)((result)), (UNITY_INT)((ERRORS_os_error)), (

((void *)0)

), (UNITY_UINT)(69), UNITY_DISPLAY_STYLE_INT);

}



void test_release_motor_sucsess(void)

{

 ERRORS_return_t result = ERRORS_ok;

 helper_take_motor(test_motor_id, test_thread_id);



 osThreadGetId_CMockExpectAndReturn(77, test_thread_id);

 osMutexAcquire_CMockExpectAndReturn(78, test_mutexes[test_motor_id], 0U, osOK);

 osMutexRelease_CMockExpectAndReturn(79, test_mutexes[test_motor_id], osOK);

 result = MOTOR_OBJ_release(test_motor_id);

 UnityAssertEqualNumber((UNITY_INT)((result)), (UNITY_INT)((ERRORS_ok)), (

((void *)0)

), (UNITY_UINT)(81), UNITY_DISPLAY_STYLE_INT);



 osThreadGetId_CMockExpectAndReturn(83, test_thread_id);

 osMutexAcquire_CMockExpectAndReturn(84, test_mutexes[test_motor_id], 0U, osOK);

 osMutexRelease_CMockExpectAndReturn(85, test_mutexes[test_motor_id], osOK);

 result = MOTOR_OBJ_release(test_motor_id);

 UnityAssertEqualNumber((UNITY_INT)((result)), (UNITY_INT)((ERRORS_resource_busy)), (

((void *)0)

), (UNITY_UINT)(87), UNITY_DISPLAY_STYLE_INT);

}

void test_release_motor_fail(void)

{

 ERRORS_return_t result = ERRORS_ok;

 osThreadGetId_CMockExpectAndReturn(92, 

((void *)0)

);

 osMutexAcquire_CMockExpectAndReturn(93, test_mutexes[test_motor_id], 0U, osOK);

 osMutexRelease_CMockExpectAndReturn(94, test_mutexes[test_motor_id], osOK);

 result = MOTOR_OBJ_release(test_motor_id);

 UnityAssertEqualNumber((UNITY_INT)((result)), (UNITY_INT)((ERRORS_os_error)), (

((void *)0)

), (UNITY_UINT)(96), UNITY_DISPLAY_STYLE_INT);

}



void test_motor_start(void)

{

 ERRORS_return_t result = ERRORS_ok;

 uint32_t test_timeout = 2000U;

 osEventFlagsWait_CMockExpectAndReturn(103, test_event_flags[test_motor_id], (1U << 3U) | (1U << 5U), 0x00000000U | 0x00000002U, test_timeout, (1U << 3U));

 osEventFlagsGet_CMockExpectAndReturn(104, test_event_flags[test_motor_id], (1U << 3U));

 osMutexAcquire_CMockExpectAndReturn(105, test_mutexes[test_motor_id], test_timeout, osOK);

 MOTOR_command_CMockExpectAndReturn(106, 

((void *)0)

, MOTOR_user_command_align, ERRORS_ok);

 MOTOR_command_CMockIgnoreArg_handle(107);

 osMutexRelease_CMockExpectAndReturn(108, test_mutexes[test_motor_id], osOK);

 osEventFlagsWait_CMockExpectAndReturn(109, test_event_flags[test_motor_id], (1U << 4U) | (1U << 5U), 0x00000000U | 0x00000002U, test_timeout, (1U << 3U) | (1U << 4U));

 osEventFlagsGet_CMockExpectAndReturn(110, test_event_flags[test_motor_id], (1U << 4U));

 osMutexAcquire_CMockExpectAndReturn(111, test_mutexes[test_motor_id], test_timeout, osOK);

 MOTOR_command_CMockExpectAndReturn(112, 

((void *)0)

, MOTOR_user_command_start, ERRORS_ok);

 MOTOR_command_CMockIgnoreArg_handle(113);

 osMutexRelease_CMockExpectAndReturn(114, test_mutexes[test_motor_id], osOK);

 osEventFlagsWait_CMockExpectAndReturn(115, test_event_flags[test_motor_id], (1U << 0U) | (1U << 5U), 0x00000000U | 0x00000002U, test_timeout, (1U << 3U) | (1U << 4U) | (1U << 0U));

 result = MOTOR_OBJ_start(test_motor_id, test_timeout);



 UnityAssertEqualNumber((UNITY_INT)((result)), (UNITY_INT)((ERRORS_ok)), (

((void *)0)

), (UNITY_UINT)(118), UNITY_DISPLAY_STYLE_INT);

}



void test_motor_start_already_started(void)

{

 ERRORS_return_t result = ERRORS_ok;

 uint32_t test_timeout = 2000U;

 uint32_t test_flags = (1U << 3U) | (1U << 4U) | (1U << 0U);

 osEventFlagsWait_CMockExpectAndReturn(126, test_event_flags[test_motor_id], (1U << 3U) | (1U << 5U), 0x00000000U | 0x00000002U, test_timeout, test_flags);

 osEventFlagsGet_CMockExpectAndReturn(127, test_event_flags[test_motor_id], test_flags);

 osEventFlagsGet_CMockExpectAndReturn(128, test_event_flags[test_motor_id], test_flags);

 result = MOTOR_OBJ_start(test_motor_id, test_timeout);



 UnityAssertEqualNumber((UNITY_INT)((result)), (UNITY_INT)((ERRORS_ok)), (

((void *)0)

), (UNITY_UINT)(131), UNITY_DISPLAY_STYLE_INT);

}



void test_motor_start_fault_during_alignment(void)

{

 ERRORS_return_t result = ERRORS_ok;

 uint32_t test_timeout = 2000U;

 uint32_t fault_flags = (1U << 3U) | (1U << 5U);

 osEventFlagsWait_CMockExpectAndReturn(139, test_event_flags[test_motor_id], (1U << 3U) | (1U << 5U), 0x00000000U | 0x00000002U, test_timeout, (1U << 3U));

 osEventFlagsGet_CMockExpectAndReturn(140, test_event_flags[test_motor_id], (1U << 3U));

 osMutexAcquire_CMockExpectAndReturn(141, test_mutexes[test_motor_id], test_timeout, osOK);

 MOTOR_command_CMockExpectAndReturn(142, 

((void *)0)

, MOTOR_user_command_align, ERRORS_ok);

 MOTOR_command_CMockIgnoreArg_handle(143);

 osMutexRelease_CMockExpectAndReturn(144, test_mutexes[test_motor_id], osOK);

 osEventFlagsWait_CMockExpectAndReturn(145, test_event_flags[test_motor_id], (1U << 4U) | (1U << 5U), 0x00000000U | 0x00000002U, test_timeout, fault_flags);

 result = MOTOR_OBJ_start(test_motor_id, test_timeout);



 UnityAssertEqualNumber((UNITY_INT)((result)), (UNITY_INT)((ERRORS_fault)), (

((void *)0)

), (UNITY_UINT)(148), UNITY_DISPLAY_STYLE_INT);

}
