/*
 * can_bmmcp_driver.c
 *
 *  Created on: Aug 17, 2021
 *      Author: tamas
 */

#include "can_bmmcp_driver.h"
#include "can_bmmcp_handle.h"
#include "stm32f3xx_hal_can.h"
#include "string.h"

#define BMMCP_MASTER_ID (1 << 4)
#define BMMCP_ID_MASK (0x0F)

bool CAN_BMMCP_send_packet (void *pHandle, void *data, uint16_t length)
{
	bool result = false;
	CAN_BUS_handle_t * can_handle = (CAN_BUS_handle_t *)pHandle;
	uint32_t TxMailbox = 0;
	CAN_TxHeaderTypeDef header = {.DLC = length - 1,
									.IDE = CAN_ID_STD,
									.RTR = CAN_RTR_DATA,
									.StdId = ((uint8_t *)data)[0] | BMMCP_MASTER_ID,
									.TransmitGlobalTime = DISABLE,
	};

	if (HAL_CAN_AddTxMessage(&can_handle->hcan, &header, &(((uint8_t *)data)[1]), &TxMailbox) == HAL_OK) {
		result = true;
	}
	return result;
}
void CAN_BMMCP_receive_buffer (void *pHandle, void* buffer, uint16_t length)
{
	CAN_BUS_handle_t * can_handle = (CAN_BUS_handle_t *)pHandle;
	if (can_handle->receive_success) {
		((uint8_t *)buffer)[0] = ((uint8_t)can_handle->receive_header.StdId) & BMMCP_ID_MASK;
		memcpy(&(((uint8_t *)buffer)[1]), can_handle->receive_data, length-1);
	}

}
void CAN_BMMCP_init (void *pHandle)
{

	CAN_BUS_handle_t * can_handle = (CAN_BUS_handle_t *)pHandle;

  can_handle->receive_success = false;

  can_handle->hcan.Instance = CAN;
  can_handle->hcan.Init.Prescaler = 1;
  can_handle->hcan.Init.Mode = CAN_MODE_NORMAL;
  can_handle->hcan.Init.SyncJumpWidth = CAN_SJW_1TQ;
  can_handle->hcan.Init.TimeSeg1 = CAN_BS1_15TQ;
  can_handle->hcan.Init.TimeSeg2 = CAN_BS2_2TQ;
  can_handle->hcan.Init.TimeTriggeredMode = DISABLE;
  can_handle->hcan.Init.AutoBusOff = DISABLE;
  can_handle->hcan.Init.AutoWakeUp = DISABLE;
  can_handle->hcan.Init.AutoRetransmission = DISABLE;
  can_handle->hcan.Init.ReceiveFifoLocked = DISABLE;
  can_handle->hcan.Init.TransmitFifoPriority = DISABLE;

  if (HAL_CAN_Init(&(can_handle->hcan)) != HAL_OK) {
	 //ouch
  }

    CAN_FilterTypeDef canfil; //CAN Bus Filter

    canfil.FilterBank = 10;
    canfil.FilterMode = CAN_FILTERMODE_IDMASK;
    canfil.FilterFIFOAssignment = CAN_RX_FIFO0;
    canfil.FilterIdHigh = 0x020<<5;
    canfil.FilterIdLow = 0;
    canfil.FilterMaskIdHigh = 0x7F0<<5;
    canfil.FilterMaskIdLow = 0x0;
    canfil.FilterScale = CAN_FILTERSCALE_32BIT;
    canfil.FilterActivation = ENABLE;
    canfil.SlaveStartFilterBank = 0;
    HAL_CAN_ConfigFilter(&(can_handle->hcan),&canfil); //Initialize CAN Filter
    HAL_CAN_Start(&(can_handle->hcan)); //Initialize CAN Bus
    HAL_CAN_ActivateNotification(&(can_handle->hcan), CAN_IT_RX_FIFO0_MSG_PENDING);
    HAL_CAN_ActivateNotification(&(can_handle->hcan), CAN_IT_TX_MAILBOX_EMPTY);
}

void CAN_BMMCP_idle_enable (void *pHWHandle)
{

}


void HAL_CAN_MspInit(CAN_HandleTypeDef* canHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(canHandle->Instance==CAN)
  {
  /* USER CODE BEGIN CAN_MspInit 0 */

  /* USER CODE END CAN_MspInit 0 */
    /* CAN clock enable */
    __HAL_RCC_CAN1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**CAN GPIO Configuration
    PA11     ------> CAN_RX
    PA12     ------> CAN_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_CAN;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* CAN interrupt Init */
    HAL_NVIC_SetPriority(CAN_TX_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(CAN_TX_IRQn);
    HAL_NVIC_SetPriority(CAN_RX0_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(CAN_RX0_IRQn);
    HAL_NVIC_SetPriority(CAN_RX1_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(CAN_RX1_IRQn);
  /* USER CODE BEGIN CAN_MspInit 1 */

  /* USER CODE END CAN_MspInit 1 */
  }
}

void HAL_CAN_MspDeInit(CAN_HandleTypeDef* canHandle)
{

  if(canHandle->Instance==CAN)
  {
  /* USER CODE BEGIN CAN_MspDeInit 0 */

  /* USER CODE END CAN_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_CAN1_CLK_DISABLE();

    /**CAN GPIO Configuration
    PA11     ------> CAN_RX
    PA12     ------> CAN_TX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_11|GPIO_PIN_12);

    /* CAN interrupt Deinit */
    HAL_NVIC_DisableIRQ(CAN_TX_IRQn);
    HAL_NVIC_DisableIRQ(CAN_RX0_IRQn);
    HAL_NVIC_DisableIRQ(CAN_RX1_IRQn);
  /* USER CODE BEGIN CAN_MspDeInit 1 */

  /* USER CODE END CAN_MspDeInit 1 */
  }
}
