#ifndef usart_aspep_driver_h
#define usart_aspep_driver_h

#include <stdint.h>
#include <stdbool.h>
#include "stm32g431xx.h"
#include "stlink_usart_config.h"
/* To be removed no protocol awarness at this level */


bool UASPEP_SEND_PACKET (void *pHandle, void *data, uint16_t length); 
void UASPEP_RECEIVE_BUFFER (void *pHandle, void* buffer, uint16_t length);
void UASPEP_INIT (void *pHandle);
void UASPEP_IDLE_ENABLE (void *pHWHandle);

#endif
