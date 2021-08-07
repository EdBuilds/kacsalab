/*********************************************************************
*               SEGGER MICROCONTROLLER GmbH & Co. KG                 *
*       Solutions for real time microcontroller applications         *
**********************************************************************
*                                                                    *
*       (c) 2014 - 2015  SEGGER Microcontroller GmbH & Co. KG        *
*                                                                    *
*       www.segger.com     Support: support@segger.com               *
*                                                                    *
**********************************************************************
----------------------------------------------------------------------
File    : SEGGER_RTT_Conf.h
Purpose : Implementation of SEGGER real-time transfer (RTT) which
          allows real-time communication on targets which support
          debugger memory accesses while the CPU is running.
---------------------------END-OF-HEADER------------------------------
*/

#ifndef SEGGER_RTT_CONF_H
#define SEGGER_RTT_CONF_H
#include "FreeRTOS.h"
#ifdef __ICCARM__
  #include <intrinsics.h>
#endif

/*********************************************************************
*
*       Defines, configurable
*
**********************************************************************
*/
#define SEGGER_RTT_MAX_NUM_UP_BUFFERS             (2)     // Max. number of up-buffers (T->H) available on this target    (Default: 2)
#define SEGGER_RTT_MAX_NUM_DOWN_BUFFERS           (2)     // Max. number of down-buffers (H->T) available on this target  (Default: 2)

//Up Buffer size is set based on RTT configuration and nrf_log configuration. Larger buffer setting is used.
#define BUFFER_SIZE_UP                            (1023)  // Size of the buffer for terminal output of target, up to host (Default: 1k)

#define BUFFER_SIZE_DOWN                          (64)    // Size of the buffer for terminal input to target from host (Usually keyboard input) (Default: 16)

#define SEGGER_RTT_PRINTF_BUFFER_SIZE             (64u)    // Size of buffer for RTT printf to bulk-send chars via RTT     (Default: 64)

//
// Target is not allowed to perform other RTT operations while string still has not been stored completely.
// Otherwise we would probably end up with a mixed string in the buffer.
// If using  RTT from within interrupts, multiple tasks or multi processors, define the SEGGER_RTT_LOCK() and SEGGER_RTT_UNLOCK() function here.
//
/*********************************************************************
*
*       RTT lock configuration for SEGGER Embedded Studio,
*       Rowley CrossStudio and GCC
*/

#define SEGGER_RTT_LOCK(SavedState)       \
                   SavedState = 0;        \
                   portENTER_CRITICAL()

#define SEGGER_RTT_UNLOCK(SavedState)     \
                   (void)SavedState;      \
                   portEXIT_CRITICAL()

/*************************** End of file ****************************/
#endif // SEGGER_RTT_CONF_H
