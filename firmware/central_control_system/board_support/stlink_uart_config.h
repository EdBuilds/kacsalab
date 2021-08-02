/*
 * stlink_uart_config.h
 *
 *  Created on: Jul 30, 2021
 *      Author: tamas
 */

#ifndef BOARD_SUPPORT_STLINK_UART_CONFIG_H_
#define BOARD_SUPPORT_STLINK_UART_CONFIG_H_

#include <stdint.h>
#include <stdbool.h>
#include "stm32f3xx.h"
#include "stm32f3xx_hal_dma.h"

#define USARTA_IRQHandler USART2_IRQHandler
#define MCP_RX_IRQHandler_A DMA2_Channel2_IRQHandler

typedef struct {
    UART_HandleTypeDef husart;
    DMA_HandleTypeDef hrxdma;
    DMA_HandleTypeDef htxdma;
} STLINK_USART_Handle_t;

extern STLINK_USART_Handle_t STLINK_USART_handle;

#endif /* BOARD_SUPPORT_STLINK_UART_CONFIG_H_ */
