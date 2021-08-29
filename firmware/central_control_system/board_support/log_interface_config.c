/*
 * log_interface_config.c
 *
 *  Created on: Aug 7, 2021
 *      Author: tamas
 */
#include "SEGGER_RTT.h"
#include "stdarg.h"
#define LOG_RTT_CHANNEL (1U)
#define LOG_RTT_BUF_SIZE (200U)
static uint8_t LOG_RTT_buf[LOG_RTT_BUF_SIZE] = {0};
void LOG_IF_init(void)
{
	SEGGER_RTT_ConfigUpBuffer(LOG_RTT_CHANNEL, "Logging", LOG_RTT_buf, LOG_RTT_BUF_SIZE, SEGGER_RTT_MODE_NO_BLOCK_TRIM);
}
void LOG_IF_write( const char *format, ...)
{
	va_list args;
	va_start(args, format);
	SEGGER_RTT_vprintf(LOG_RTT_CHANNEL, format, &args);
    va_end(args);

}
