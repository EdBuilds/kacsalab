/*
 * logging_config.c
 *
 *  Created on: Aug 7, 2021
 *      Author: tamas
 */
#include "logging_config.h"
#include "log_interface_config.h"

void log_init(void)
{
	LOG_IF_init();
}
