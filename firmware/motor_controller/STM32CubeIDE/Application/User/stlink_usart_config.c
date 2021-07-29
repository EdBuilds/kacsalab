/*
 * stlink_usart_config.c
 *
 *  Created on: Jul 25, 2021
 *      Author: tamas
 */

#include "stlink_usart_config.h"

UASPEP_Handle_t STLINK_USART_uaspep =
{
 .USARTx = USARTA,
 .rxDMA = DMA_RX_A,
 .txDMA = DMA_TX_A,
 .rxChannel = DMACH_RX_A,
 .txChannel = DMACH_TX_A,
};
