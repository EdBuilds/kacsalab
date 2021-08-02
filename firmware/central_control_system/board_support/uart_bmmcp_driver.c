/*
 * bmmcp_driver.c
 *
 *  Created on: Jul 30, 2021
 *      Author: tamas
 */
#include "uart_bmmcp_driver.h"
#include "stlink_uart_config.h"
#include "stm32f3xx_hal.h"

#define VCP_RX_Pin GPIO_PIN_15
#define VCP_RX_GPIO_Port GPIOA
#define VCP_TX_Pin GPIO_PIN_2
#define VCP_TX_GPIO_Port GPIOA

void UART_BMMCP_init (void *pHandle) {

  STLINK_USART_Handle_t * hstuart = (STLINK_USART_Handle_t *)pHandle;
  UART_HandleTypeDef * huart2 = &(hstuart->husart);
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* DMA controller clock enable */

  /* USER CODE BEGIN USART2_MspInit 0 */

  /* USER CODE END USART2_MspInit 0 */
    /* USART2 clock enable */
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA15     ------> USART2_RX
    */

    GPIO_InitStruct.Pin = VCP_TX_Pin|VCP_RX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    __HAL_RCC_DMA1_CLK_ENABLE();
    /* USART2 DMA Init */
    /* USART2_RX Init */
    hstuart->hrxdma.Instance = DMA1_Channel6;
    hstuart->hrxdma.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hstuart->hrxdma.Init.PeriphInc = DMA_PINC_DISABLE;
    hstuart->hrxdma.Init.MemInc = DMA_MINC_ENABLE;
    hstuart->hrxdma.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hstuart->hrxdma.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hstuart->hrxdma.Init.Mode = DMA_NORMAL;
    hstuart->hrxdma.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hstuart->hrxdma) != HAL_OK)
    {
    	return;
    }
    __HAL_LINKDMA(huart2,hdmarx,hstuart->hrxdma);

  /* DMA interrupt init */
  /* DMA1_Channel6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel6_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel6_IRQn);

    /* USART2_TX Init */
    hstuart->htxdma.Instance = DMA1_Channel7;
    hstuart->htxdma.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hstuart->htxdma.Init.PeriphInc = DMA_PINC_DISABLE;
    hstuart->htxdma.Init.MemInc = DMA_MINC_ENABLE;
    hstuart->htxdma.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hstuart->htxdma.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hstuart->htxdma.Init.Mode = DMA_NORMAL;
    hstuart->htxdma.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hstuart->htxdma) != HAL_OK)
    {
    	return;
    }

    __HAL_LINKDMA(huart2,hdmatx,hstuart->htxdma);

    /* DMA1_Channel7_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA1_Channel7_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel7_IRQn);

    __HAL_RCC_USART2_CLK_ENABLE();

    huart2->Instance = USART2;
    huart2->Init.BaudRate = 115200;
    huart2->Init.WordLength = UART_WORDLENGTH_8B;
    huart2->Init.StopBits = UART_STOPBITS_1;
    huart2->Init.Parity = UART_PARITY_NONE;
    huart2->Init.Mode = UART_MODE_TX_RX;
    huart2->Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2->Init.OverSampling = UART_OVERSAMPLING_16;
    huart2->Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    huart2->AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    if (HAL_UART_Init(huart2) != HAL_OK)
    {
      //Error_Handler();
    }

    HAL_NVIC_SetPriority(USART2_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(USART2_IRQn);


  /* USER CODE BEGIN USART2_MspInit 1 */

  /* USER CODE END USART2_MspInit 1 */

}

bool UART_BMMCP_send_packet (void *pHandle, void *data, uint16_t length) {
	STLINK_USART_Handle_t * hstuart = (STLINK_USART_Handle_t *)pHandle;
	if (HAL_UART_Transmit_DMA(&(hstuart->husart), data, length) != HAL_OK) {
		return false;
	} else {
		return true;
	}
}

void UART_BMMCP_receive_buffer (void *pHandle, void* buffer, uint16_t length) {
	STLINK_USART_Handle_t * hstuart = (STLINK_USART_Handle_t *)pHandle;
	HAL_UART_Receive_DMA(&(hstuart->husart), buffer, length);
}

void UART_BMMCP_idle_enable (void *pHWHandle) {

}
