/*
 * log_interface_config.h
 *
 *  Created on: Aug 7, 2021
 *      Author: tamas
 */

#ifndef LOG_INTERFACE_CONFIG_H_
#define LOG_INTERFACE_CONFIG_H_

void LOG_IF_init(void);
void LOG_IF_write( const char *format, ...);
#endif /* LOG_INTERFACE_CONFIG_H_ */
