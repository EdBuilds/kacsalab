/*
 * logging_config.h
 *
 *  Created on: Aug 7, 2021
 *      Author: tamas
 */

#ifndef INC_LOGGING_CONFIG_H_
#define INC_LOGGING_CONFIG_H_
/**************************************************/
/******* DO NOT CHANGE the following order ********/
/**************************************************/

#include "logging_levels.h"
#include "log_interface_config.h"
#include <string.h>
#include "cmsis_os2.h"


#ifndef __FILENAME__
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

/* Logging configurations for the application. */

#ifndef LIBRARY_LOG_NAME
#define LIBRARY_LOG_NAME    "Kacsalab CCS"
#endif
#ifndef LIBRARY_LOG_LEVEL
#define LIBRARY_LOG_LEVEL    LOG_DEBUG
#endif

/* Define the metadata information to add in each log.
 * The example here sets the metadata to [:]. */
#if !defined( LOG_METADATA_FORMAT ) && !defined( LOG_METADATA_ARGS )
   #define LOG_METADATA_FORMAT "[%s:%d] [%d]"
   #define LOG_METADATA_ARGS __FILENAME__, __LINE__, osKernelGetTickCount()

#endif

/* Define the platform-specific logging function to call from
 * enabled logging macros. */
#ifndef SdkLog
#define SdkLog(f_, ...) LOG_IF_write((f_), ##__VA_ARGS__)
    //#define SdkLog( message )   LOG_IF_write message
#endif

/************ End of logging configuration ****************/
void log_init(void);


#endif /* INC_LOGGING_CONFIG_H_ */
