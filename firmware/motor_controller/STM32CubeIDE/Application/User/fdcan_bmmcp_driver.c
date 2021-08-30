/*
 * can_bmmcp_driver.c
 *
 *  Created on: Aug 17, 2021
 *      Author: tamas
 */

#include <fdcan_bmmcp_driver.h>
#include <fdcan_bmmcp_handle.h>

#include "stm32g4xx_hal_fdcan.h"
#include "string.h"

#define BMMCP_MASTER_ID (1 << 4)
#define BMMCP_SLAVE_ID (2 << 4)
#define BMMCP_MOTOR_ID 0
#define BMMCP_ID_MASK (0x0F)

bool FDCAN_BMMCP_send_packet (void *pHandle, void *data, uint16_t length)
{
	bool result = false;
	CAN_BUS_handle_t * can_handle = (CAN_BUS_handle_t *)pHandle;

	FDCAN_TxHeaderTypeDef header;
	header.BitRateSwitch = FDCAN_BRS_OFF;
	header.DataLength = length - 1;
	header.ErrorStateIndicator = FDCAN_ESI_PASSIVE;
	header.FDFormat = FDCAN_CLASSIC_CAN;
	header.IdType = FDCAN_STANDARD_ID;
	header.Identifier = ((uint8_t *)data)[0] | BMMCP_SLAVE_ID;
	header.MessageMarker = 0;
	header.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
	header.TxFrameType = FDCAN_DATA_FRAME;

	if (HAL_FDCAN_AddMessageToTxFifoQ(&can_handle->hcan, &header, &(((uint8_t *)data)[1])) == HAL_OK) {
		result = true;
	}
	return result;
}
void FDCAN_BMMCP_receive_buffer (void *pHandle, void* buffer, uint16_t length)
{
	CAN_BUS_handle_t * can_handle = (CAN_BUS_handle_t *)pHandle;
	if (can_handle->receive_success) {
		((uint8_t *)buffer)[0] = ((uint8_t)can_handle->receive_header.Identifier) & BMMCP_ID_MASK;
		memcpy(&(((uint8_t *)buffer)[1]), can_handle->receive_data, length-1);
	}

}
void FDCAN_BMMCP_init (void *pHandle)
{

	CAN_BUS_handle_t * can_handle = (CAN_BUS_handle_t *)pHandle;

  can_handle->receive_success = false;

  can_handle->hcan.Instance = FDCAN1;
  can_handle->hcan.Init.ClockDivider = FDCAN_CLOCK_DIV10;
  can_handle->hcan.Init.FrameFormat = FDCAN_FRAME_CLASSIC;
  can_handle->hcan.Init.Mode = FDCAN_MODE_INTERNAL_LOOPBACK;
  can_handle->hcan.Init.AutoRetransmission = DISABLE;
  can_handle->hcan.Init.TransmitPause = DISABLE;
  can_handle->hcan.Init.ProtocolException = DISABLE;
  can_handle->hcan.Init.NominalPrescaler = 1;
  can_handle->hcan.Init.NominalSyncJumpWidth = 1;
  can_handle->hcan.Init.NominalTimeSeg1 = 14;
  can_handle->hcan.Init.NominalTimeSeg2 = 2;
  can_handle->hcan.Init.DataPrescaler = 1;
  can_handle->hcan.Init.DataSyncJumpWidth = 1;
  can_handle->hcan.Init.DataTimeSeg1 = 14;
  can_handle->hcan.Init.DataTimeSeg2 = 2;
  can_handle->hcan.Init.StdFiltersNbr = 1;
  can_handle->hcan.Init.ExtFiltersNbr = 0;
  can_handle->hcan.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
  if (HAL_FDCAN_Init(&(can_handle->hcan)) != HAL_OK)
  {
  }
    FDCAN_FilterTypeDef canfil; //CAN Bus Filter
    canfil.IdType = FDCAN_STANDARD_ID;
    canfil.FilterIndex = 0;
    canfil.FilterType = FDCAN_FILTER_MASK;
    canfil.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
    canfil.FilterID1 = BMMCP_MOTOR_ID | BMMCP_MASTER_ID;
    canfil.FilterID2 = 0x7FF;

    HAL_FDCAN_ConfigFilter(&(can_handle->hcan),&canfil); //Initialize CAN Filter
    HAL_FDCAN_Start(&(can_handle->hcan)); //Initialize CAN Bus
    HAL_FDCAN_ActivateNotification(&(can_handle->hcan), FDCAN_IT_LIST_RX_FIFO0, 0);
    HAL_FDCAN_ActivateNotification(&(can_handle->hcan), FDCAN_IT_LIST_RX_FIFO1, 0);
    HAL_FDCAN_ActivateNotification(&(can_handle->hcan), FDCAN_IT_LIST_SMSG, 0);
    HAL_FDCAN_ActivateNotification(&(can_handle->hcan), FDCAN_IT_LIST_TX_FIFO_ERROR, 0);
    HAL_FDCAN_ActivateNotification(&(can_handle->hcan), FDCAN_IT_LIST_MISC, 0);
    HAL_FDCAN_ActivateNotification(&(can_handle->hcan), FDCAN_IT_LIST_BIT_LINE_ERROR, 0);
    HAL_FDCAN_ActivateNotification(&(can_handle->hcan), FDCAN_IT_LIST_PROTOCOL_ERROR, 0);
}

void FDCAN_BMMCP_idle_enable (void *pHWHandle)
{

}
