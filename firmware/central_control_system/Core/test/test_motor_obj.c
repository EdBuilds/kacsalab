/*
 * test_console_command_helpers.c
 *
 *  Created on: Aug 7, 2021
 *      Author: tamas
 */

// Unit test framework
#include "unity.h"

#include "mock_cmsis_os2.h"
#include "mock_bmmcp_common.h"
#include "mock_logging_config.h"
#include "mock_logging.h"
#include "mock_log_interface_config.h"
#include "mock_shared_task_resources.h"
#include "mock_motor_common_types.h"
#include "mock_motor_interface.h"

#include "motor_object.h"
static const osMutexId_t test_mutexes[MOTOR_NUM] = {(void *)0x01020304, (void *)0x10203040};
static const osMutexId_t test_event_flags[MOTOR_NUM] = {(void *)0x11121314, (void *)0x11213141};
static const osThreadId_t test_thread_id = (void *)0xDEADBEEF;
static const uint8_t test_motor_id = 1;

static void helper_take_motor(uint8_t motor_id, osThreadId_t thread_id);
static void helper_take_motor(uint8_t motor_id, osThreadId_t thread_id)
{
	ERRORS_return_t result = ERRORS_ok;
	osThreadGetId_ExpectAndReturn(thread_id);
	osMutexAcquire_ExpectAndReturn(test_mutexes[test_motor_id], 0U, osOK);
	osMutexRelease_ExpectAndReturn(test_mutexes[test_motor_id], osOK);
	result = MOTOR_OBJ_take(motor_id);
	TEST_ASSERT_EQUAL(result, ERRORS_ok);

}

void setUp(void)
{
	for (uint8_t motor_idx; motor_idx<MOTOR_NUM; ++motor_idx) {
		osMutexNew_ExpectAndReturn(NULL, test_mutexes[motor_idx]);
		osMutexNew_IgnoreArg_attr();
		osEventFlagsNew_ExpectAndReturn(NULL, test_event_flags[motor_idx]);
		osEventFlagsNew_IgnoreArg_attr();
		MOTOR_init_Expect(NULL, motor_idx, test_event_flags[motor_idx]);
		MOTOR_init_IgnoreArg_handle();
	}
	MOTOR_OBJ_init();
}
void test_take_motor_sucsess(void)
{
	ERRORS_return_t result = ERRORS_ok;

	helper_take_motor(test_motor_id, test_thread_id);

	osThreadGetId_ExpectAndReturn(test_thread_id + 1);
	osMutexAcquire_ExpectAndReturn(test_mutexes[test_motor_id], 0U, osOK);
	osMutexRelease_ExpectAndReturn(test_mutexes[test_motor_id], osOK);
	result = MOTOR_OBJ_take(test_motor_id);
	TEST_ASSERT_EQUAL(result, ERRORS_resource_busy);
}
void test_take_motor_fail(void)
{
	ERRORS_return_t result = ERRORS_ok;
	osThreadGetId_ExpectAndReturn(NULL);
	osMutexAcquire_ExpectAndReturn(test_mutexes[test_motor_id], 0U, osOK);
	osMutexRelease_ExpectAndReturn(test_mutexes[test_motor_id], osOK);
	result = MOTOR_OBJ_take(test_motor_id);
	TEST_ASSERT_EQUAL(result, ERRORS_os_error);
}

void test_release_motor_sucsess(void)
{
	ERRORS_return_t result = ERRORS_ok;
	helper_take_motor(test_motor_id, test_thread_id);

	osThreadGetId_ExpectAndReturn(test_thread_id);
	osMutexAcquire_ExpectAndReturn(test_mutexes[test_motor_id], 0U, osOK);
	osMutexRelease_ExpectAndReturn(test_mutexes[test_motor_id], osOK);
	result = MOTOR_OBJ_release(test_motor_id);
	TEST_ASSERT_EQUAL(result, ERRORS_ok);

	osThreadGetId_ExpectAndReturn(test_thread_id);
	osMutexAcquire_ExpectAndReturn(test_mutexes[test_motor_id], 0U, osOK);
	osMutexRelease_ExpectAndReturn(test_mutexes[test_motor_id], osOK);
	result = MOTOR_OBJ_release(test_motor_id);
	TEST_ASSERT_EQUAL(result, ERRORS_resource_busy);
}
void test_release_motor_fail(void)
{
	ERRORS_return_t result = ERRORS_ok;
	osThreadGetId_ExpectAndReturn(NULL);
	osMutexAcquire_ExpectAndReturn(test_mutexes[test_motor_id], 0U, osOK);
	osMutexRelease_ExpectAndReturn(test_mutexes[test_motor_id], osOK);
	result = MOTOR_OBJ_release(test_motor_id);
	TEST_ASSERT_EQUAL(result, ERRORS_os_error);
}

void test_motor_start(void)
{
	ERRORS_return_t result = ERRORS_ok;
	uint32_t test_timeout = 2000U;
	osEventFlagsWait_ExpectAndReturn(test_event_flags[test_motor_id], MOTOR_EVENT_FLAG_ALIVE | MOTOR_EVENT_FLAG_FAULT, osFlagsWaitAny | osFlagsNoClear, test_timeout, MOTOR_EVENT_FLAG_ALIVE);
	osEventFlagsGet_ExpectAndReturn(test_event_flags[test_motor_id], MOTOR_EVENT_FLAG_ALIVE);
	osMutexAcquire_ExpectAndReturn(test_mutexes[test_motor_id], test_timeout, osOK);
	MOTOR_command_ExpectAndReturn(NULL, MOTOR_user_command_align, ERRORS_ok);
	MOTOR_command_IgnoreArg_handle();
	osMutexRelease_ExpectAndReturn(test_mutexes[test_motor_id], osOK);
	osEventFlagsWait_ExpectAndReturn(test_event_flags[test_motor_id], MOTOR_EVENT_FLAG_ALIGNED | MOTOR_EVENT_FLAG_FAULT, osFlagsWaitAny | osFlagsNoClear, test_timeout, MOTOR_EVENT_FLAG_ALIVE | MOTOR_EVENT_FLAG_ALIGNED);
	osEventFlagsGet_ExpectAndReturn(test_event_flags[test_motor_id], MOTOR_EVENT_FLAG_ALIGNED);
	osMutexAcquire_ExpectAndReturn(test_mutexes[test_motor_id], test_timeout, osOK);
	MOTOR_command_ExpectAndReturn(NULL, MOTOR_user_command_start, ERRORS_ok);
	MOTOR_command_IgnoreArg_handle();
	osMutexRelease_ExpectAndReturn(test_mutexes[test_motor_id], osOK);
	osEventFlagsWait_ExpectAndReturn(test_event_flags[test_motor_id], MOTOR_EVENT_FLAG_STARTED | MOTOR_EVENT_FLAG_FAULT, osFlagsWaitAny | osFlagsNoClear, test_timeout, MOTOR_EVENT_FLAG_ALIVE | MOTOR_EVENT_FLAG_ALIGNED | MOTOR_EVENT_FLAG_STARTED);
	result = MOTOR_OBJ_start(test_motor_id, test_timeout);

	TEST_ASSERT_EQUAL(result, ERRORS_ok);
}

void test_motor_start_already_started(void)
{
	ERRORS_return_t result = ERRORS_ok;
	uint32_t test_timeout = 2000U;
	uint32_t test_flags = MOTOR_EVENT_FLAG_ALIVE | MOTOR_EVENT_FLAG_ALIGNED | MOTOR_EVENT_FLAG_STARTED;
	osEventFlagsWait_ExpectAndReturn(test_event_flags[test_motor_id], MOTOR_EVENT_FLAG_ALIVE | MOTOR_EVENT_FLAG_FAULT, osFlagsWaitAny | osFlagsNoClear, test_timeout, test_flags);
	osEventFlagsGet_ExpectAndReturn(test_event_flags[test_motor_id], test_flags);
	osEventFlagsGet_ExpectAndReturn(test_event_flags[test_motor_id], test_flags);
	result = MOTOR_OBJ_start(test_motor_id, test_timeout);

	TEST_ASSERT_EQUAL(result, ERRORS_ok);
}

void test_motor_start_fault_during_alignment(void)
{
	ERRORS_return_t result = ERRORS_ok;
	uint32_t test_timeout = 2000U;
	uint32_t fault_flags = MOTOR_EVENT_FLAG_ALIVE | MOTOR_EVENT_FLAG_FAULT;
	osEventFlagsWait_ExpectAndReturn(test_event_flags[test_motor_id], MOTOR_EVENT_FLAG_ALIVE | MOTOR_EVENT_FLAG_FAULT, osFlagsWaitAny | osFlagsNoClear, test_timeout, MOTOR_EVENT_FLAG_ALIVE);
	osEventFlagsGet_ExpectAndReturn(test_event_flags[test_motor_id], MOTOR_EVENT_FLAG_ALIVE);
	osMutexAcquire_ExpectAndReturn(test_mutexes[test_motor_id], test_timeout, osOK);
	MOTOR_command_ExpectAndReturn(NULL, MOTOR_user_command_align, ERRORS_ok);
	MOTOR_command_IgnoreArg_handle();
	osMutexRelease_ExpectAndReturn(test_mutexes[test_motor_id], osOK);
	osEventFlagsWait_ExpectAndReturn(test_event_flags[test_motor_id], MOTOR_EVENT_FLAG_ALIGNED | MOTOR_EVENT_FLAG_FAULT, osFlagsWaitAny | osFlagsNoClear, test_timeout, fault_flags);
	result = MOTOR_OBJ_start(test_motor_id, test_timeout);

	TEST_ASSERT_EQUAL(result, ERRORS_fault);
}
