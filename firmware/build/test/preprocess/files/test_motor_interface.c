#include "build/temp/_test_motor_interface.c"
#include "motor_interface.h"
#include "mock_motor_common_types.h"
#include "mock_shared_task_resources.h"
#include "mock_log_interface_config.h"
#include "mock_logging.h"
#include "mock_logging_config.h"
#include "mock_bmmcp_common.h"
#include "mock_cmsis_os2.h"
#include "unity.h"




static MOTOR_handle_t test_handle;

static const uint8_t test_id = 1;

static const osEventFlagsId_t test_event_flags_id = (void *)0xDEADBEEF;

static const osTimerId_t test_timer_id = (void *)0x69420102;

static const osMessageQueueId_t test_bmmcp_queue = (void *)0x69696969;



static void helper_get_motor_to_running();

static void helper_receive_generic_telemetry();

void setUp(void)

{



 osEventFlagsClear_CMockExpectAndReturn(23, test_event_flags_id, ((1U << 4U) | (1U << 3U) | (1U << 2U) | (1U << 1U) | (1U << 0U) | (1U << 5U)), 0U);

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



 BMMCP_velocity_to_si_CMockExpectAndReturn(48, 0, 0.0f);

 osTimerStop_CMockExpectAndReturn(49, test_timer_id, osOK);

 osTimerStart_CMockExpectAndReturn(50, test_timer_id, (10), osOK);

 osEventFlagsSet_CMockExpectAndReturn(51, test_event_flags_id, (1U << 3U), 0U);

 result = MOTOR_new_message(&test_handle, &test_packet);

 UnityAssertEqualNumber((UNITY_INT)((result)), (UNITY_INT)((ERRORS_ok)), (

((void *)0)

), (UNITY_UINT)(53), UNITY_DISPLAY_STYLE_INT);





 BMMCP_velocity_to_si_CMockExpectAndReturn(56, 0, 0.0f);

 osTimerStop_CMockExpectAndReturn(57, test_timer_id, osOK);

 osTimerStart_CMockExpectAndReturn(58, test_timer_id, (10), osOK);

 result = MOTOR_new_message(&test_handle, &test_packet);

 UnityAssertEqualNumber((UNITY_INT)((result)), (UNITY_INT)((ERRORS_ok)), (

((void *)0)

), (UNITY_UINT)(60), UNITY_DISPLAY_STYLE_INT);





 SHARED_TASK_get_bmmcp_master_queue_CMockExpectAndReturn(63, test_bmmcp_queue);

 osMessageQueuePut_CMockExpectAndReturn(64, test_bmmcp_queue, 

((void *)0)

, 0, 0, osOK);

 osMessageQueuePut_CMockIgnoreArg_msg_ptr(65);

 result = MOTOR_command(&test_handle, MOTOR_user_command_align);

 UnityAssertEqualNumber((UNITY_INT)((result)), (UNITY_INT)((ERRORS_ok)), (

((void *)0)

), (UNITY_UINT)(67), UNITY_DISPLAY_STYLE_INT);





 BMMCP_velocity_to_si_CMockExpectAndReturn(70, 0, 0.0f);

 osTimerStop_CMockExpectAndReturn(71, test_timer_id, osOK);

 osTimerStart_CMockExpectAndReturn(72, test_timer_id, (10), osOK);

 result = MOTOR_new_message(&test_handle, &test_packet);

 UnityAssertEqualNumber((UNITY_INT)((result)), (UNITY_INT)((ERRORS_ok)), (

((void *)0)

), (UNITY_UINT)(74), UNITY_DISPLAY_STYLE_INT);





 test_packet.command = BMMCP_response;

 test_packet.data.command_response = BMMCP_ack;

 result = MOTOR_new_message(&test_handle, &test_packet);

 UnityAssertEqualNumber((UNITY_INT)((result)), (UNITY_INT)((ERRORS_ok)), (

((void *)0)

), (UNITY_UINT)(80), UNITY_DISPLAY_STYLE_INT);





 helper_receive_generic_telemetry(IDLE_ALIGNMENT);

 helper_receive_generic_telemetry(ALIGN_CHARGE_BOOT_CAP);

 helper_receive_generic_telemetry(ALIGN_OFFSET_CALIB);

 helper_receive_generic_telemetry(ALIGNMENT);

 helper_receive_generic_telemetry(ANY_STOP);

 helper_receive_generic_telemetry(STOP);

 helper_receive_generic_telemetry(STOP_IDLE);

 helper_receive_generic_telemetry(ICLWAIT);





 test_packet.command = BMMCP_telemetry;

 test_packet.data.telemetry.velocity = 0U;

 test_packet.data.telemetry.current = 0U;

 test_packet.data.telemetry.stm_state = IDLE;

 BMMCP_velocity_to_si_CMockExpectAndReturn(97, 0, 0.0f);

 osTimerStop_CMockExpectAndReturn(98, test_timer_id, osOK);

 osTimerStart_CMockExpectAndReturn(99, test_timer_id, (10), osOK);

 osEventFlagsSet_CMockExpectAndReturn(100, test_event_flags_id, (1U << 4U) | (1U << 1U), 0U);

 result = MOTOR_new_message(&test_handle, &test_packet);

 UnityAssertEqualNumber((UNITY_INT)((result)), (UNITY_INT)((ERRORS_ok)), (

((void *)0)

), (UNITY_UINT)(102), UNITY_DISPLAY_STYLE_INT);





 osEventFlagsClear_CMockExpectAndReturn(105, test_event_flags_id, (1U << 1U), 0U);

 SHARED_TASK_get_bmmcp_master_queue_CMockExpectAndReturn(106, test_bmmcp_queue);

 osMessageQueuePut_CMockExpectAndReturn(107, test_bmmcp_queue, 

((void *)0)

, 0, 0, osOK);

 osMessageQueuePut_CMockIgnoreArg_msg_ptr(108);

 result = MOTOR_command(&test_handle, MOTOR_user_command_start);

 UnityAssertEqualNumber((UNITY_INT)((result)), (UNITY_INT)((ERRORS_ok)), (

((void *)0)

), (UNITY_UINT)(110), UNITY_DISPLAY_STYLE_INT);





 helper_receive_generic_telemetry(IDLE);





 test_packet.command = BMMCP_response;

 test_packet.data.command_response = BMMCP_ack;

 SHARED_TASK_get_bmmcp_master_queue_CMockExpectAndReturn(118, test_bmmcp_queue);

 osMessageQueuePut_CMockExpectAndReturn(119, test_bmmcp_queue, 

((void *)0)

, 0, 0, osOK);

 osMessageQueuePut_CMockIgnoreArg_msg_ptr(120);

 result = MOTOR_new_message(&test_handle, &test_packet);

 UnityAssertEqualNumber((UNITY_INT)((result)), (UNITY_INT)((ERRORS_ok)), (

((void *)0)

), (UNITY_UINT)(122), UNITY_DISPLAY_STYLE_INT);





 helper_receive_generic_telemetry(IDLE);





 test_packet.command = BMMCP_response;

 test_packet.data.command_response = BMMCP_ack;

 result = MOTOR_new_message(&test_handle, &test_packet);

 UnityAssertEqualNumber((UNITY_INT)((result)), (UNITY_INT)((ERRORS_ok)), (

((void *)0)

), (UNITY_UINT)(131), UNITY_DISPLAY_STYLE_INT);





 helper_receive_generic_telemetry(IDLE_START);

 helper_receive_generic_telemetry(CHARGE_BOOT_CAP);

 helper_receive_generic_telemetry(OFFSET_CALIB);

 helper_receive_generic_telemetry(START);

 helper_receive_generic_telemetry(START_RUN);





 test_packet.command = BMMCP_telemetry;

 test_packet.data.telemetry.velocity = 0U;

 test_packet.data.telemetry.current = 0U;

 test_packet.data.telemetry.stm_state = RUN;

 BMMCP_velocity_to_si_CMockExpectAndReturn(145, 0, 0.0f);

 osTimerStop_CMockExpectAndReturn(146, test_timer_id, osOK);

 osTimerStart_CMockExpectAndReturn(147, test_timer_id, (10), osOK);

 osEventFlagsSet_CMockExpectAndReturn(148, test_event_flags_id, (1U << 0U), 0U);

 result = MOTOR_new_message(&test_handle, &test_packet);

 UnityAssertEqualNumber((UNITY_INT)((result)), (UNITY_INT)((ERRORS_ok)), (

((void *)0)

), (UNITY_UINT)(150), UNITY_DISPLAY_STYLE_INT);





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



 BMMCP_velocity_to_si_CMockExpectAndReturn(171, 0, 0.0f);

 osTimerStop_CMockExpectAndReturn(172, test_timer_id, osOK);

 osTimerStart_CMockExpectAndReturn(173, test_timer_id, (10), osOK);

 result = MOTOR_new_message(&test_handle, &test_packet);

 UnityAssertEqualNumber((UNITY_INT)((result)), (UNITY_INT)((ERRORS_ok)), (

((void *)0)

), (UNITY_UINT)(175), UNITY_DISPLAY_STYLE_INT);

}

void test_startup_process(void)

{

 ERRORS_return_t result = ERRORS_ok;



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



 osEventFlagsClear_CMockExpectAndReturn(194, test_event_flags_id, (1U << 0U), 0U);

 SHARED_TASK_get_bmmcp_master_queue_CMockExpectAndReturn(195, test_bmmcp_queue);

 osMessageQueuePut_CMockExpectAndReturn(196, test_bmmcp_queue, 

((void *)0)

, 0, 0, osOK);

 osMessageQueuePut_CMockIgnoreArg_msg_ptr(197);

 result = MOTOR_command(&test_handle, MOTOR_user_command_stop);

 UnityAssertEqualNumber((UNITY_INT)((result)), (UNITY_INT)((ERRORS_ok)), (

((void *)0)

), (UNITY_UINT)(199), UNITY_DISPLAY_STYLE_INT);





 helper_receive_generic_telemetry(RUN);





 test_packet.command = BMMCP_response;

 test_packet.data.command_response = BMMCP_ack;

 result = MOTOR_new_message(&test_handle, &test_packet);

 UnityAssertEqualNumber((UNITY_INT)((result)), (UNITY_INT)((ERRORS_ok)), (

((void *)0)

), (UNITY_UINT)(208), UNITY_DISPLAY_STYLE_INT);





 helper_receive_generic_telemetry(ANY_STOP);

 helper_receive_generic_telemetry(STOP);

 helper_receive_generic_telemetry(STOP_IDLE);

 helper_receive_generic_telemetry(ICLWAIT);





 test_packet.command = BMMCP_telemetry;

 test_packet.data.telemetry.velocity = 0U;

 test_packet.data.telemetry.current = 0U;

 test_packet.data.telemetry.stm_state = IDLE;

 BMMCP_velocity_to_si_CMockExpectAndReturn(221, 0, 0.0f);

 osTimerStop_CMockExpectAndReturn(222, test_timer_id, osOK);

 osTimerStart_CMockExpectAndReturn(223, test_timer_id, (10), osOK);

 osEventFlagsSet_CMockExpectAndReturn(224, test_event_flags_id, (1U << 4U) | (1U << 1U), 0U);

 result = MOTOR_new_message(&test_handle, &test_packet);

 UnityAssertEqualNumber((UNITY_INT)((result)), (UNITY_INT)((ERRORS_ok)), (

((void *)0)

), (UNITY_UINT)(226), UNITY_DISPLAY_STYLE_INT);

}



void test_other_states_from_unknown_state(void)

{



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



 BMMCP_velocity_to_si_CMockExpectAndReturn(251, 0, 0.0f);

 osTimerStop_CMockExpectAndReturn(252, test_timer_id, osOK);

 osTimerStart_CMockExpectAndReturn(253, test_timer_id, (10), osOK);

 osEventFlagsClear_CMockExpectAndReturn(254, test_event_flags_id, (1U << 0U), 0U);

 osEventFlagsSet_CMockExpectAndReturn(255, test_event_flags_id, (1U << 5U), 0U);

 result = MOTOR_new_message(&test_handle, &test_packet);

 UnityAssertEqualNumber((UNITY_INT)((result)), (UNITY_INT)((ERRORS_ok)), (

((void *)0)

), (UNITY_UINT)(257), UNITY_DISPLAY_STYLE_INT);











 osKernelGetTickCount_CMockIgnoreAndReturn(263, 0);

 LOG_IF_write_CMockIgnore();

 result = MOTOR_command(&test_handle, MOTOR_user_command_acknowledge_faults);

 UnityAssertEqualNumber((UNITY_INT)((result)), (UNITY_INT)((ERRORS_ok)), (

((void *)0)

), (UNITY_UINT)(266), UNITY_DISPLAY_STYLE_INT);



 test_packet.data.telemetry.stm_state = FAULT_OVER;

 BMMCP_velocity_to_si_CMockExpectAndReturn(269, 0, 0.0f);

 osTimerStop_CMockExpectAndReturn(270, test_timer_id, osOK);

 osTimerStart_CMockExpectAndReturn(271, test_timer_id, (10), osOK);

 osEventFlagsSet_CMockExpectAndReturn(272, test_event_flags_id, (1U << 5U), 0U);

 result = MOTOR_new_message(&test_handle, &test_packet);

 UnityAssertEqualNumber((UNITY_INT)((result)), (UNITY_INT)((ERRORS_ok)), (

((void *)0)

), (UNITY_UINT)(274), UNITY_DISPLAY_STYLE_INT);



 SHARED_TASK_get_bmmcp_master_queue_CMockExpectAndReturn(276, test_bmmcp_queue);

 osMessageQueuePut_CMockExpectAndReturn(277, test_bmmcp_queue, 

((void *)0)

, 0, 0, osOK);

 osMessageQueuePut_CMockIgnoreArg_msg_ptr(278);

 result = MOTOR_command(&test_handle, MOTOR_user_command_acknowledge_faults);

 UnityAssertEqualNumber((UNITY_INT)((result)), (UNITY_INT)((ERRORS_ok)), (

((void *)0)

), (UNITY_UINT)(280), UNITY_DISPLAY_STYLE_INT);



}
