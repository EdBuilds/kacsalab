/*
 * bmmcp_master_task.c
 *
 *  Created on: Jul 31, 2021
 *      Author: tamas
 */
#include "bmmcp_master_task.h"
#include "bmmcp_config.h"
#include "cmsis_os2.h"
#include "shared_task_resources.h"

extern BMMCP_handle_t BMMCP_handle;
static void bmmcp_received_packet_callback(void);

void start_bmmcp_master_task(void *argument)
{
	BMMCP_init(&BMMCP_handle, &bmmcp_received_packet_callback);
	BMMCP_universal_packet_t packet;
	osStatus_t status;
	while (true) {
	    status = osMessageQueueGet(SHARED_TASK_get_bmmcp_master_queue(), &packet, NULL, osWaitForever);
	    if (status == osOK) {
	    	BMMCP_write_msg(&BMMCP_handle, &packet);
	    }


	}
}

static void bmmcp_received_packet_callback(void) {
	BMMCP_universal_packet_t packet;
	BMMCP_read_msg(&BMMCP_handle, &packet);
	osMessageQueuePut(SHARED_TASK_get_bmmcp_master_queue(), &packet, 0, 0U);
}
