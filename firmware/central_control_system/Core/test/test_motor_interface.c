// Unit test framework
#include "unity.h"
#include "mock_cmsis_os2.h"
#include "mock_bmmcp_common.h"
#include "mock_logging_config.h"
#include "mock_logging.h"
#include "mock_log_interface_config.h"
#include "mock_shared_task_resources.h"
#include "mock_motor_common_types.h"
#include "motor_interface.h"

static MOTOR_handle_t test_handle;
static const uint8_t test_id = 1;
static const osEventFlagsId_t test_event_flags_id = (void *)0xDEADBEEF;
static const osTimerId_t test_timer_id = (void *)0x69420102;
static const osMessageQueueId_t test_bmmcp_queue = (void *)0x69696969;

static void helper_get_motor_to_running();
static void helper_receive_generic_telemetry();
void setUp(void)
{

	osEventFlagsClear_ExpectAndReturn(test_event_flags_id, MOTOR_EVENT_FLAG_ALL, 0U);
	MOTOR_init(&test_handle, test_id, test_event_flags_id);
	MOTOR_register_timer(&test_handle, test_timer_id);
}
void tearDown(void)
{
	MOTOR_deinit(&test_handle);
}

static void helper_get_motor_to_running()
{
	ERRORS_return_t result = ERRORS_ok;
	// Receive first telemetry in idle state
	BMMCP_universal_packet_t test_packet = {
			.id = test_id,
			.command = BMMCP_telemetry,
			.data = {
					.telemetry = {
							.velocity = 0,
							.current = 0,
							.stm_state = IDLE,
					},
			},
	};

	BMMCP_velocity_to_si_ExpectAndReturn(0, 0.0f);
	osTimerStop_ExpectAndReturn(test_timer_id, osOK);
	osTimerStart_ExpectAndReturn(test_timer_id, MOTOR_TELEMERY_TIMEOUT_TICKS, osOK);
	osEventFlagsSet_ExpectAndReturn(test_event_flags_id, MOTOR_EVENT_FLAG_ALIVE, MOTOR_EVENT_FLAG_ALIVE);
	result = MOTOR_new_message(&test_handle, &test_packet);
	TEST_ASSERT_EQUAL(result, ERRORS_ok);

	// Second telemetry before starting the motor
	BMMCP_velocity_to_si_ExpectAndReturn(0, 0.0f);
	osTimerStop_ExpectAndReturn(test_timer_id, osOK);
	osTimerStart_ExpectAndReturn(test_timer_id, MOTOR_TELEMERY_TIMEOUT_TICKS, osOK);
	result = MOTOR_new_message(&test_handle, &test_packet);
	TEST_ASSERT_EQUAL(result, ERRORS_ok);

	// Send user command
	SHARED_TASK_get_bmmcp_master_queue_ExpectAndReturn(test_bmmcp_queue);
	osMessageQueuePut_ExpectAndReturn(test_bmmcp_queue, NULL, 0, 0, osOK);
	osMessageQueuePut_IgnoreArg_msg_ptr();
	result = MOTOR_command(&test_handle, MOTOR_user_command_align);
	TEST_ASSERT_EQUAL(result, ERRORS_ok);

	// Intermitten telemetry between ctrl msgs
	BMMCP_velocity_to_si_ExpectAndReturn(0, 0.0f);
	osTimerStop_ExpectAndReturn(test_timer_id, osOK);
	osTimerStart_ExpectAndReturn(test_timer_id, MOTOR_TELEMERY_TIMEOUT_TICKS, osOK);
	result = MOTOR_new_message(&test_handle, &test_packet);
	TEST_ASSERT_EQUAL(result, ERRORS_ok);

	// Receive response to alignment request
	test_packet.command = BMMCP_response;
	test_packet.data.command_response = BMMCP_ack;
	result = MOTOR_new_message(&test_handle, &test_packet);
	TEST_ASSERT_EQUAL(result, ERRORS_ok);

	// telemetries walking through the alignment process
	helper_receive_generic_telemetry(IDLE_ALIGNMENT);
	helper_receive_generic_telemetry(ALIGN_CHARGE_BOOT_CAP);
	helper_receive_generic_telemetry(ALIGN_OFFSET_CALIB);
	helper_receive_generic_telemetry(ALIGNMENT);
	helper_receive_generic_telemetry(ANY_STOP);
	helper_receive_generic_telemetry(STOP);
	helper_receive_generic_telemetry(STOP_IDLE);
	helper_receive_generic_telemetry(ICLWAIT);

	// telemetry returning to idle state
	test_packet.command = BMMCP_telemetry;
	test_packet.data.telemetry.velocity = 0U;
	test_packet.data.telemetry.current = 0U;
	test_packet.data.telemetry.stm_state = IDLE;
	BMMCP_velocity_to_si_ExpectAndReturn(0, 0.0f);
	osTimerStop_ExpectAndReturn(test_timer_id, osOK);
	osTimerStart_ExpectAndReturn(test_timer_id, MOTOR_TELEMERY_TIMEOUT_TICKS, osOK);
	volatile uint32_t flags_to_set = MOTOR_EVENT_FLAG_ALIGNED | MOTOR_EVENT_FLAG_READY;
	osEventFlagsSet_ExpectAndReturn(test_event_flags_id, flags_to_set, flags_to_set);
	result = MOTOR_new_message(&test_handle, &test_packet);
	TEST_ASSERT_EQUAL(result, ERRORS_ok);

	// Send start command
	osEventFlagsClear_ExpectAndReturn(test_event_flags_id, MOTOR_EVENT_FLAG_READY, 0U);
	SHARED_TASK_get_bmmcp_master_queue_ExpectAndReturn(test_bmmcp_queue);
	osMessageQueuePut_ExpectAndReturn(test_bmmcp_queue, NULL, 0, 0, osOK);
	osMessageQueuePut_IgnoreArg_msg_ptr();
	result = MOTOR_command(&test_handle, MOTOR_user_command_start);
	TEST_ASSERT_EQUAL(result, ERRORS_ok);

	// Receive intermitten telemetry
	helper_receive_generic_telemetry(IDLE);

	// Receive response to torque reset request
	test_packet.command = BMMCP_response;
	test_packet.data.command_response = BMMCP_ack;
	SHARED_TASK_get_bmmcp_master_queue_ExpectAndReturn(test_bmmcp_queue);
	osMessageQueuePut_ExpectAndReturn(test_bmmcp_queue, NULL, 0, 0, osOK);
	osMessageQueuePut_IgnoreArg_msg_ptr();
	result = MOTOR_new_message(&test_handle, &test_packet);
	TEST_ASSERT_EQUAL(result, ERRORS_ok);

	// Receive intermitten telemetry
	helper_receive_generic_telemetry(IDLE);

	// Receive response to start request
	test_packet.command = BMMCP_response;
	test_packet.data.command_response = BMMCP_ack;
	result = MOTOR_new_message(&test_handle, &test_packet);
	TEST_ASSERT_EQUAL(result, ERRORS_ok);

	// telemetries walking through the starting process
	helper_receive_generic_telemetry(IDLE_START);
	helper_receive_generic_telemetry(CHARGE_BOOT_CAP);
	helper_receive_generic_telemetry(OFFSET_CALIB);
	helper_receive_generic_telemetry(START);
	helper_receive_generic_telemetry(START_RUN);

	// telemetry to run state
	test_packet.command = BMMCP_telemetry;
	test_packet.data.telemetry.velocity = 0U;
	test_packet.data.telemetry.current = 0U;
	test_packet.data.telemetry.stm_state = RUN;
	BMMCP_velocity_to_si_ExpectAndReturn(0, 0.0f);
	osTimerStop_ExpectAndReturn(test_timer_id, osOK);
	osTimerStart_ExpectAndReturn(test_timer_id, MOTOR_TELEMERY_TIMEOUT_TICKS, osOK);
    flags_to_set = MOTOR_EVENT_FLAG_STARTED | MOTOR_EVENT_FLAG_ALIGNED;
	osEventFlagsSet_ExpectAndReturn(test_event_flags_id, flags_to_set, flags_to_set);
	result = MOTOR_new_message(&test_handle, &test_packet);
	TEST_ASSERT_EQUAL(result, ERRORS_ok);

	// Receive intermitten telemetry
	helper_receive_generic_telemetry(RUN);

}
static void helper_receive_generic_telemetry(BMMCP_slave_state_t state)
{
	ERRORS_return_t result = ERRORS_ok;
	BMMCP_universal_packet_t test_packet = {
			.id = test_id,
			.command = BMMCP_telemetry,
			.data = {
					.telemetry = {
							.velocity = 0,
							.current = 0,
							.stm_state = state,
					},
			},
	};

	BMMCP_velocity_to_si_ExpectAndReturn(0, 0.0f);
	osTimerStop_ExpectAndReturn(test_timer_id, osOK);
	osTimerStart_ExpectAndReturn(test_timer_id, MOTOR_TELEMERY_TIMEOUT_TICKS, osOK);
	result = MOTOR_new_message(&test_handle, &test_packet);
	TEST_ASSERT_EQUAL(result, ERRORS_ok);
}
void test_startup_process(void)
{
	ERRORS_return_t result = ERRORS_ok;
	// Receive first telemetry in idle state
	BMMCP_universal_packet_t test_packet = {
			.id = test_id,
			.command = BMMCP_telemetry,
			.data = {
					.telemetry = {
							.velocity = 0,
							.current = 0,
							.stm_state = IDLE,
					},
			},
	};
	helper_get_motor_to_running();
	// Send stop command
	osEventFlagsClear_ExpectAndReturn(test_event_flags_id, MOTOR_EVENT_FLAG_STARTED, 0U);
	SHARED_TASK_get_bmmcp_master_queue_ExpectAndReturn(test_bmmcp_queue);
	osMessageQueuePut_ExpectAndReturn(test_bmmcp_queue, NULL, 0, 0, osOK);
	osMessageQueuePut_IgnoreArg_msg_ptr();
	result = MOTOR_command(&test_handle, MOTOR_user_command_stop);
	TEST_ASSERT_EQUAL(result, ERRORS_ok);

	// Receive intermitten telemetry

	test_packet.command = BMMCP_telemetry;
	test_packet.data.telemetry.stm_state = RUN;
	BMMCP_velocity_to_si_ExpectAndReturn(0, 0.0f);
	osTimerStop_ExpectAndReturn(test_timer_id, osOK);
	osTimerStart_ExpectAndReturn(test_timer_id, MOTOR_TELEMERY_TIMEOUT_TICKS, osOK);
    uint32_t flags_to_set = MOTOR_EVENT_FLAG_STARTED | MOTOR_EVENT_FLAG_ALIGNED;
	osEventFlagsSet_ExpectAndReturn(test_event_flags_id, flags_to_set, flags_to_set);
	result = MOTOR_new_message(&test_handle, &test_packet);
	TEST_ASSERT_EQUAL(result, ERRORS_ok);

	// Receive response to stop request
	test_packet.command = BMMCP_response;
	test_packet.data.command_response = BMMCP_ack;
	result = MOTOR_new_message(&test_handle, &test_packet);
	TEST_ASSERT_EQUAL(result, ERRORS_ok);

	// telemetries walking through the stopping process
	test_packet.command = BMMCP_telemetry;
	test_packet.data.telemetry.stm_state = ANY_STOP;
	BMMCP_velocity_to_si_ExpectAndReturn(0, 0.0f);
	osTimerStop_ExpectAndReturn(test_timer_id, osOK);
	osTimerStart_ExpectAndReturn(test_timer_id, MOTOR_TELEMERY_TIMEOUT_TICKS, osOK);
    uint32_t flags_to_clear = MOTOR_EVENT_FLAG_STARTED ;
	osEventFlagsClear_ExpectAndReturn(test_event_flags_id, flags_to_clear, flags_to_clear);
	result = MOTOR_new_message(&test_handle, &test_packet);
	TEST_ASSERT_EQUAL(result, ERRORS_ok);
	helper_receive_generic_telemetry(STOP);
	helper_receive_generic_telemetry(STOP_IDLE);
	helper_receive_generic_telemetry(ICLWAIT);

	// telemetry returning to idle state
	test_packet.command = BMMCP_telemetry;
	test_packet.data.telemetry.velocity = 0U;
	test_packet.data.telemetry.current = 0U;
	test_packet.data.telemetry.stm_state = IDLE;
	BMMCP_velocity_to_si_ExpectAndReturn(0, 0.0f);
	osTimerStop_ExpectAndReturn(test_timer_id, osOK);
	osTimerStart_ExpectAndReturn(test_timer_id, MOTOR_TELEMERY_TIMEOUT_TICKS, osOK);
	osEventFlagsSet_ExpectAndReturn(test_event_flags_id, MOTOR_EVENT_FLAG_ALIGNED | MOTOR_EVENT_FLAG_READY, MOTOR_EVENT_FLAG_ALIGNED | MOTOR_EVENT_FLAG_READY);
	result = MOTOR_new_message(&test_handle, &test_packet);
	TEST_ASSERT_EQUAL(result, ERRORS_ok);
}

void test_other_states_from_unknown_state(void)
{
	// TODO: test this
}

void test_motor_fault(void)
{
	ERRORS_return_t result = ERRORS_ok;
	BMMCP_universal_packet_t test_packet = {
			.id = test_id,
			.command = BMMCP_telemetry,
			.data = {
					.telemetry = {
							.velocity = 0,
							.current = 0,
							.stm_state = FAULT_NOW,
					},
			},
	};

	helper_get_motor_to_running();

	BMMCP_velocity_to_si_ExpectAndReturn(0, 0.0f);
	osTimerStop_ExpectAndReturn(test_timer_id, osOK);
	osTimerStart_ExpectAndReturn(test_timer_id, MOTOR_TELEMERY_TIMEOUT_TICKS, osOK);
	osEventFlagsClear_ExpectAndReturn(test_event_flags_id, MOTOR_EVENT_FLAG_STARTED, 0U);
	osEventFlagsSet_ExpectAndReturn(test_event_flags_id, MOTOR_EVENT_FLAG_FAULT, MOTOR_EVENT_FLAG_FAULT);

	// Send fault query command
	SHARED_TASK_get_bmmcp_master_queue_ExpectAndReturn(test_bmmcp_queue);
	osMessageQueuePut_ExpectAndReturn(test_bmmcp_queue, NULL, 0, 0, osOK);
	osMessageQueuePut_IgnoreArg_msg_ptr();

	result = MOTOR_new_message(&test_handle, &test_packet);
	TEST_ASSERT_EQUAL(result, ERRORS_ok);

	// Send early reset command
	//SHARED_TASK_get_bmmcp_master_queue_ExpectAndReturn(test_bmmcp_queue);
	//osMessageQueuePut_ExpectAndReturn(test_bmmcp_queue, NULL, 0, 0, osOK);
	//osMessageQueuePut_IgnoreArg_msg_ptr();
	osKernelGetTickCount_IgnoreAndReturn(0);
	LOG_IF_write_Ignore();
	result = MOTOR_command(&test_handle, MOTOR_user_command_acknowledge_faults);
	TEST_ASSERT_EQUAL(result, ERRORS_ok);

	test_packet.data.telemetry.stm_state = FAULT_OVER;
	BMMCP_velocity_to_si_ExpectAndReturn(0, 0.0f);
	osTimerStop_ExpectAndReturn(test_timer_id, osOK);
	osTimerStart_ExpectAndReturn(test_timer_id, MOTOR_TELEMERY_TIMEOUT_TICKS, osOK);
	osEventFlagsSet_ExpectAndReturn(test_event_flags_id, MOTOR_EVENT_FLAG_FAULT, MOTOR_EVENT_FLAG_FAULT);

	// Send fault query command
	SHARED_TASK_get_bmmcp_master_queue_ExpectAndReturn(test_bmmcp_queue);
	osMessageQueuePut_ExpectAndReturn(test_bmmcp_queue, NULL, 0, 0, osOK);
	osMessageQueuePut_IgnoreArg_msg_ptr();

	result = MOTOR_new_message(&test_handle, &test_packet);
	TEST_ASSERT_EQUAL(result, ERRORS_ok);

	// Send appropriate reset command
	SHARED_TASK_get_bmmcp_master_queue_ExpectAndReturn(test_bmmcp_queue);
	osMessageQueuePut_ExpectAndReturn(test_bmmcp_queue, NULL, 0, 0, osOK);
	osMessageQueuePut_IgnoreArg_msg_ptr();
	result = MOTOR_command(&test_handle, MOTOR_user_command_acknowledge_faults);
	TEST_ASSERT_EQUAL(result, ERRORS_ok);

}
