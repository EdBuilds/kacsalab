/*
 * shared_task_resources.h
 *
 *  Created on: Aug 2, 2021
 *      Author: tamas
 */

#ifndef INC_SHARED_TASK_RESOURCES_H_
#define INC_SHARED_TASK_RESOURCES_H_

#include "cmsis_os2.h"
#include "bmmcp/bmmcp_packet.h"
#include "stdbool.h"

typedef struct {
	bool receive;
	BMMCP_universal_packet_t packet;
}BMMCP_master_task_msg_t;
uint32_t SHARED_TASK_init();
osMessageQueueId_t SHARED_TASK_get_bmmcp_master_queue();
#endif /* INC_SHARED_TASK_RESOURCES_H_ */
