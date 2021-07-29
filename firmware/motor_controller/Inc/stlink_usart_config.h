/*
 * stlink_usart_defs.h
 *
 *  Created on: Jul 25, 2021
 *      Author: tamas
 */

#ifndef STLINK_USART_DEFS_H_
#define STLINK_USART_DEFS_H_

#include <stdint.h>
#include <stdbool.h>
#include "stm32g431xx.h"
#include "stm32g4xx_ll_dma.h"

#define USARTA USART2
#define DMA_RX_A DMA2
#define DMA_TX_A DMA2
#define DMACH_RX_A LL_DMA_CHANNEL_2
#define DMACH_TX_A LL_DMA_CHANNEL_3
#define USARTA_IRQHandler USART2_IRQHandler
#define MCP_RX_IRQHandler_A DMA2_Channel2_IRQHandler

typedef struct {
  USART_TypeDef *USARTx;
  DMA_TypeDef * rxDMA;
  DMA_TypeDef * txDMA;
  uint32_t rxChannel;
  uint32_t txChannel;
} UASPEP_Handle_t;

extern UASPEP_Handle_t STLINK_USART_uaspep;
#endif /* STLINK_USART_DEFS_H_ */
