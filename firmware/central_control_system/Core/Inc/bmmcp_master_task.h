/*
 * bmmcp_master_task.h
 *
 *  Created on: Jul 31, 2021
 *      Author: tamas
 */

#ifndef INC_BMMCP_MASTER_TASK_H_
#define INC_BMMCP_MASTER_TASK_H_
#include "stdbool.h"
typedef struct {
	bool debugging;
}BMMCP_MASTER_task_arg_t;
void start_bmmcp_master_task(void *argument);



#endif /* INC_BMMCP_MASTER_TASK_H_ */
