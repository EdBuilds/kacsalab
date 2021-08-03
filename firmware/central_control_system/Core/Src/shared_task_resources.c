/*
 * shared_task_resources.c
 *
 *  Created on: Aug 2, 2021
 *      Author: tamas
 */
#include "shared_task_resources.h"

#define BMMCP_MSG_QUEUE_SIZE (5U)

static osMessageQueueId_t s_bmmcp_queue_id = 0U;
static uint8_t s_cb_mem[80] = {0U};
static uint8_t s_bmmcp_mq_mem[BMMCP_MSG_QUEUE_SIZE * sizeof(BMMCP_master_task_msg_t)] = {0U};

uint32_t SHARED_TASK_init()
{
	osMessageQueueAttr_t message_queue_attrs = {
			.name = "bmmcp incoming message buffer",
			.attr_bits = 0U,
			.cb_mem = s_cb_mem,
			.cb_size = 80,
			.mq_mem = s_bmmcp_mq_mem,
			.mq_size = BMMCP_MSG_QUEUE_SIZE * sizeof(BMMCP_master_task_msg_t),
	};
	s_bmmcp_queue_id = osMessageQueueNew(BMMCP_MSG_QUEUE_SIZE, sizeof(BMMCP_master_task_msg_t), &message_queue_attrs);

	if (s_bmmcp_queue_id == NULL) {
		return 1U;
	}
	return 0U;
}

osMessageQueueId_t SHARED_TASK_get_bmmcp_master_queue()
{
	return s_bmmcp_queue_id;
}
