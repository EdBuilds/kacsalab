#include "bmmcp/bmmcp_packet.h"
#include "cmsis_os2.h"


typedef enum {

 BMMCP_MASTER_TASK_packet_receive = 0,

 BMMCP_MASTER_TASK_packet_send,

 BMMCP_MASTER_TASK_telem_timeout,

}BMMCP_master_task_msg_variant_t;



typedef union{

 uint8_t timeout_motor_id;

 BMMCP_universal_packet_t packet;

}BMMCP_master_task_msg_payload_t;



typedef struct {

 BMMCP_master_task_msg_variant_t variant;

 BMMCP_master_task_msg_payload_t payload;

}BMMCP_master_task_msg_t;

uint32_t SHARED_TASK_init();

osMessageQueueId_t SHARED_TASK_get_bmmcp_master_queue();
