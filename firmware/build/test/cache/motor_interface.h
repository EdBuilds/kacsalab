#include "bmmcp/bmmcp_slave_state.h"
#include "bmmcp/bmmcp_packet.h"
#include "cmsis_os2.h"
#include "errors/errors.h"
#include "motor_common_types.h"
typedef enum{

 MOTOR_user_command_start = 0,

 MOTOR_user_command_align,

 MOTOR_user_command_stop,

 MOTOR_user_command_acknowledge_faults,

}MOTOR_user_command_t;



typedef struct {

 uint8_t motor_id;

 float velocity_radps;

 const void *current_state;

 osTimerId_t timeout_timer;

 osEventFlagsId_t event_flag;



}MOTOR_handle_t;



typedef struct MOTOR_STATE_handle_t{

    ERRORS_return_t (* MOTOR_STATE_entry_func ) (MOTOR_handle_t *);

    ERRORS_return_t (* MOTOR_STATE_exit_func ) (MOTOR_handle_t *);

    void* (* MOTOR_STATE_user_command ) (MOTOR_user_command_t);

    void* (* MOTOR_STATE_new_slave_state ) (BMMCP_slave_state_t);

    void* (* MOTOR_STATE_slave_command_response ) (BMMCP_response_t);

}MOTOR_STATE_handle_t;



void MOTOR_init(MOTOR_handle_t *handle, uint8_t motor_id, osEventFlagsId_t event_flag);

void MOTOR_deinit(MOTOR_handle_t *handle);

void MOTOR_register_timer(MOTOR_handle_t *handle, osTimerId_t timeout_timer);



ERRORS_return_t MOTOR_command(MOTOR_handle_t *handle, MOTOR_user_command_t command);

ERRORS_return_t MOTOR_new_message(MOTOR_handle_t *handle, BMMCP_universal_packet_t * packet);

ERRORS_return_t MOTOR_get_state(MOTOR_handle_t *handle, MOTOR_state_t *state);

ERRORS_return_t MOTOR_get_velocity(MOTOR_handle_t *handle, float *velocity_radps);

ERRORS_return_t MOTOR_get_torque(MOTOR_handle_t *handle, float *torque_nm);

ERRORS_return_t MOTOR_set_torque(MOTOR_handle_t *handle, float torque_nm);

ERRORS_return_t MOTOR_timeout(MOTOR_handle_t *handle);
