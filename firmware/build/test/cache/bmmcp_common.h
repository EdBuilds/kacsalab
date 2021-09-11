#include "bmmcp/bmmcp_packet.h"






typedef 

       _Bool 

            (* BMMCP_send_packet_func_t ) (void * HW_handle, void *txBuffer, uint16_t txDataLength);

typedef void (* BMMCP_receive_packet_func_t ) (void * HW_handle, void *rxBuffer, uint16_t rxDataLength);

typedef void (* BMMCP_init_hw_func_t ) (void * HW_handle);

typedef void (* BMMCP_receive_callback_func_t ) (void);



typedef enum {

 BMMCP_ok = 0,

 BMMCP_no_packet_received,

 BMMCP_resource_busy,

 BMMCP_buffer_short,

 BMMCP_unexpected_value,

}BMMCP_return_t;



typedef enum {

 BMMCP_empty = 0,

 BMMCP_write_lock,

 BMMCP_full,

 BMMCP_read_lock,

}BMMCP_lock_state_t;



typedef struct {

 void * HW_if;

 BMMCP_send_packet_func_t send_packet_func;

 BMMCP_receive_packet_func_t receive_packet_func;

 BMMCP_init_hw_func_t init_hw_func;

 BMMCP_receive_callback_func_t receive_callback_func;

 uint8_t receive_buffer[(7U)];

 BMMCP_lock_state_t receive_buffer_lock;

 uint8_t transmit_buffer[(7U)];

 BMMCP_lock_state_t transmit_buffer_lock;

}BMMCP_handle_t;



BMMCP_return_t BMMCP_read_msg(BMMCP_handle_t * bmmcp_handle, BMMCP_universal_packet_t * received_packet);

BMMCP_return_t BMMCP_write_msg(BMMCP_handle_t * bmmcp_handle, BMMCP_universal_packet_t * packet_to_send);

BMMCP_return_t BMMCP_init(BMMCP_handle_t * bmmcp_handle, BMMCP_receive_callback_func_t receive_callback_func);

float BMMCP_velocity_to_si(int16_t vel);





void BMMCP_HWDataReceivedIT(BMMCP_handle_t * bmmcp_handle);

void BMMCP_HWDataTransmittedIT(BMMCP_handle_t * bmmcp_handle);
