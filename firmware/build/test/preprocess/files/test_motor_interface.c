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

 osEventFlagsSet_CMockExpectAndReturn(51, test_event_flags_id, (1U << 3U), (1U << 3U));

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

 volatile uint32_t flags_to_set = (1U << 4U) | (1U << 1U);

 osEventFlagsSet_CMockExpectAndReturn(101, test_event_flags_id, flags_to_set, flags_to_set);

 result = MOTOR_new_message(&test_handle, &test_packet);

 UnityAssertEqualNumber((UNITY_INT)((result)), (UNITY_INT)((ERRORS_ok)), (

((void *)0)

), (UNITY_UINT)(103), UNITY_DISPLAY_STYLE_INT);





 osEventFlagsClear_CMockExpectAndReturn(106, test_event_flags_id, (1U << 1U), 0U);

 SHARED_TASK_get_bmmcp_master_queue_CMockExpectAndReturn(107, test_bmmcp_queue);

 osMessageQueuePut_CMockExpectAndReturn(108, test_bmmcp_queue, 

((void *)0)

, 0, 0, osOK);

 osMessageQueuePut_CMockIgnoreArg_msg_ptr(109);

 result = MOTOR_command(&test_handle, MOTOR_user_command_start);

 UnityAssertEqualNumber((UNITY_INT)((result)), (UNITY_INT)((ERRORS_ok)), (

((void *)0)

), (UNITY_UINT)(111), UNITY_DISPLAY_STYLE_INT);





 helper_receive_generic_telemetry(IDLE);





 test_packet.command = BMMCP_response;

 test_packet.data.command_response = BMMCP_ack;

 SHARED_TASK_get_bmmcp_master_queue_CMockExpectAndReturn(119, test_bmmcp_queue);

 osMessageQueuePut_CMockExpectAndReturn(120, test_bmmcp_queue, 

((void *)0)

, 0, 0, osOK);

 osMessageQueuePut_CMockIgnoreArg_msg_ptr(121);

 result = MOTOR_new_message(&test_handle, &test_packet);

 UnityAssertEqualNumber((UNITY_INT)((result)), (UNITY_INT)((ERRORS_ok)), (

((void *)0)

), (UNITY_UINT)(123), UNITY_DISPLAY_STYLE_INT);





 helper_receive_generic_telemetry(IDLE);





 test_packet.command = BMMCP_response;

 test_packet.data.command_response = BMMCP_ack;

 result = MOTOR_new_message(&test_handle, &test_packet);

 UnityAssertEqualNumber((UNITY_INT)((result)), (UNITY_INT)((ERRORS_ok)), (

((void *)0)

), (UNITY_UINT)(132), UNITY_DISPLAY_STYLE_INT);





 helper_receive_generic_telemetry(IDLE_START);

 helper_receive_generic_telemetry(CHARGE_BOOT_CAP);

 helper_receive_generic_telemetry(OFFSET_CALIB);

 helper_receive_generic_telemetry(START);

 helper_receive_generic_telemetry(START_RUN);





 test_packet.command = BMMCP_telemetry;

 test_packet.data.telemetry.velocity = 0U;

 test_packet.data.telemetry.current = 0U;

 test_packet.data.telemetry.stm_state = RUN;

 BMMCP_velocity_to_si_CMockExpectAndReturn(146, 0, 0.0f);

 osTimerStop_CMockExpectAndReturn(147, test_timer_id, osOK);

 osTimerStart_CMockExpectAndReturn(148, test_timer_id, (10), osOK);

    flags_to_set = (1U << 0U) | (1U << 4U);

 osEventFlagsSet_CMockExpectAndReturn(150, test_event_flags_id, flags_to_set, flags_to_set);

 result = MOTOR_new_message(&test_handle, &test_packet);

 UnityAssertEqualNumber((UNITY_INT)((result)), (UNITY_INT)((ERRORS_ok)), (

((void *)0)

), (UNITY_UINT)(152), UNITY_DISPLAY_STYLE_INT);





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



 BMMCP_velocity_to_si_CMockExpectAndReturn(173, 0, 0.0f);

 osTimerStop_CMockExpectAndReturn(174, test_timer_id, osOK);

 osTimerStart_CMockExpectAndReturn(175, test_timer_id, (10), osOK);

 result = MOTOR_new_message(&test_handle, &test_packet);

 UnityAssertEqualNumber((UNITY_INT)((result)), (UNITY_INT)((ERRORS_ok)), (

((void *)0)

), (UNITY_UINT)(177), UNITY_DISPLAY_STYLE_INT);

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



 osEventFlagsClear_CMockExpectAndReturn(196, test_event_flags_id, (1U << 0U), 0U);

 SHARED_TASK_get_bmmcp_master_queue_CMockExpectAndReturn(197, test_bmmcp_queue);

 osMessageQueuePut_CMockExpectAndReturn(198, test_bmmcp_queue, 

((void *)0)

, 0, 0, osOK);

 osMessageQueuePut_CMockIgnoreArg_msg_ptr(199);

 result = MOTOR_command(&test_handle, MOTOR_user_command_stop);

 UnityAssertEqualNumber((UNITY_INT)((result)), (UNITY_INT)((ERRORS_ok)), (

((void *)0)

), (UNITY_UINT)(201), UNITY_DISPLAY_STYLE_INT);







 test_packet.command = BMMCP_telemetry;

 test_packet.data.telemetry.stm_state = RUN;

 BMMCP_velocity_to_si_CMockExpectAndReturn(207, 0, 0.0f);

 osTimerStop_CMockExpectAndReturn(208, test_timer_id, osOK);

 osTimerStart_CMockExpectAndReturn(209, test_timer_id, (10), osOK);

    uint32_t flags_to_set = (1U << 0U) | (1U << 4U);

 osEventFlagsSet_CMockExpectAndReturn(211, test_event_flags_id, flags_to_set, flags_to_set);

 result = MOTOR_new_message(&test_handle, &test_packet);

 UnityAssertEqualNumber((UNITY_INT)((result)), (UNITY_INT)((ERRORS_ok)), (

((void *)0)

), (UNITY_UINT)(213), UNITY_DISPLAY_STYLE_INT);





 test_packet.command = BMMCP_response;

 test_packet.data.command_response = BMMCP_ack;

 result = MOTOR_new_message(&test_handle, &test_packet);

 UnityAssertEqualNumber((UNITY_INT)((result)), (UNITY_INT)((ERRORS_ok)), (

((void *)0)

), (UNITY_UINT)(219), UNITY_DISPLAY_STYLE_INT);





 test_packet.command = BMMCP_telemetry;

 test_packet.data.telemetry.stm_state = ANY_STOP;

 BMMCP_velocity_to_si_CMockExpectAndReturn(224, 0, 0.0f);

 osTimerStop_CMockExpectAndReturn(225, test_timer_id, osOK);

 osTimerStart_CMockExpectAndReturn(226, test_timer_id, (10), osOK);

    uint32_t flags_to_clear = (1U << 0U) ;

 osEventFlagsClear_CMockExpectAndReturn(228, test_event_flags_id, flags_to_clear, flags_to_clear);

 result = MOTOR_new_message(&test_handle, &test_packet);

 UnityAssertEqualNumber((UNITY_INT)((result)), (UNITY_INT)((ERRORS_ok)), (

((void *)0)

), (UNITY_UINT)(230), UNITY_DISPLAY_STYLE_INT);

 helper_receive_generic_telemetry(STOP);

 helper_receive_generic_telemetry(STOP_IDLE);

 helper_receive_generic_telemetry(ICLWAIT);





 test_packet.command = BMMCP_telemetry;

 test_packet.data.telemetry.velocity = 0U;

 test_packet.data.telemetry.current = 0U;

 test_packet.data.telemetry.stm_state = IDLE;

 BMMCP_velocity_to_si_CMockExpectAndReturn(240, 0, 0.0f);

 osTimerStop_CMockExpectAndReturn(241, test_timer_id, osOK);

 osTimerStart_CMockExpectAndReturn(242, test_timer_id, (10), osOK);

 osEventFlagsSet_CMockExpectAndReturn(243, test_event_flags_id, (1U << 4U) | (1U << 1U), (1U << 4U) | (1U << 1U));

 result = MOTOR_new_message(&test_handle, &test_packet);

 UnityAssertEqualNumber((UNITY_INT)((result)), (UNITY_INT)((ERRORS_ok)), (

((void *)0)

), (UNITY_UINT)(245), UNITY_DISPLAY_STYLE_INT);

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



 BMMCP_velocity_to_si_CMockExpectAndReturn(270, 0, 0.0f);

 osTimerStop_CMockExpectAndReturn(271, test_timer_id, osOK);

 osTimerStart_CMockExpectAndReturn(272, test_timer_id, (10), osOK);

 osEventFlagsClear_CMockExpectAndReturn(273, test_event_flags_id, (1U << 0U), 0U);

 osEventFlagsSet_CMockExpectAndReturn(274, test_event_flags_id, (1U << 5U), (1U << 5U));





 SHARED_TASK_get_bmmcp_master_queue_CMockExpectAndReturn(277, test_bmmcp_queue);

 osMessageQueuePut_CMockExpectAndReturn(278, test_bmmcp_queue, 

((void *)0)

, 0, 0, osOK);

 osMessageQueuePut_CMockIgnoreArg_msg_ptr(279);



 result = MOTOR_new_message(&test_handle, &test_packet);

 UnityAssertEqualNumber((UNITY_INT)((result)), (UNITY_INT)((ERRORS_ok)), (

((void *)0)

), (UNITY_UINT)(282), UNITY_DISPLAY_STYLE_INT);











 osKernelGetTickCount_CMockIgnoreAndReturn(288, 0);

 LOG_IF_write_CMockIgnore();

 result = MOTOR_command(&test_handle, MOTOR_user_command_acknowledge_faults);

 UnityAssertEqualNumber((UNITY_INT)((result)), (UNITY_INT)((ERRORS_ok)), (

((void *)0)

), (UNITY_UINT)(291), UNITY_DISPLAY_STYLE_INT);



 test_packet.data.telemetry.stm_state = FAULT_OVER;

 BMMCP_velocity_to_si_CMockExpectAndReturn(294, 0, 0.0f);

 osTimerStop_CMockExpectAndReturn(295, test_timer_id, osOK);

 osTimerStart_CMockExpectAndReturn(296, test_timer_id, (10), osOK);

 osEventFlagsSet_CMockExpectAndReturn(297, test_event_flags_id, (1U << 5U), (1U << 5U));





 SHARED_TASK_get_bmmcp_master_queue_CMockExpectAndReturn(300, test_bmmcp_queue);

 osMessageQueuePut_CMockExpectAndReturn(301, test_bmmcp_queue, 

((void *)0)

, 0, 0, osOK);

 osMessageQueuePut_CMockIgnoreArg_msg_ptr(302);



 result = MOTOR_new_message(&test_handle, &test_packet);

 UnityAssertEqualNumber((UNITY_INT)((result)), (UNITY_INT)((ERRORS_ok)), (

((void *)0)

), (UNITY_UINT)(305), UNITY_DISPLAY_STYLE_INT);





 SHARED_TASK_get_bmmcp_master_queue_CMockExpectAndReturn(308, test_bmmcp_queue);

 osMessageQueuePut_CMockExpectAndReturn(309, test_bmmcp_queue, 

((void *)0)

, 0, 0, osOK);

 osMessageQueuePut_CMockIgnoreArg_msg_ptr(310);

 result = MOTOR_command(&test_handle, MOTOR_user_command_acknowledge_faults);

 UnityAssertEqualNumber((UNITY_INT)((result)), (UNITY_INT)((ERRORS_ok)), (

((void *)0)

), (UNITY_UINT)(312), UNITY_DISPLAY_STYLE_INT);



}
