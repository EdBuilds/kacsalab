/**
  ******************************************************************************
  * @file    state_machine.h
  * @author  Motor Control SDK Team, ST Microelectronics
  * @brief   This file contains all definitions and functions prototypes for the
  *          Motor Control State Machine component of the Motor Control SDK.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  * @ingroup STATE_MACHINE
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STATEMACHINE_H
#define __STATEMACHINE_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "mc_type.h"

#include "bmmcp/bmmcp_slave_state.h"

typedef BMMCP_slave_state_t State_t ;
/** @addtogroup MCSDK
  * @{
  */

/** @addtogroup STATE_MACHINE
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/**
  * @brief  State_t enum type definition, it lists all the possible state machine states
  */
/**
  * @brief  StateMachine class members definition
  */
typedef struct
{
  State_t   bState;          /*!< Variable containing state machine current
                                    state */
  uint16_t  hFaultNow;       /*!< Bit fields variable containing faults
                                    currently present */
  uint16_t  hFaultOccurred;  /*!< Bit fields variable containing faults
                                    historically occurred since the state
                                    machine has been moved to FAULT_NOW state */
} STM_Handle_t;

/* Exported constants --------------------------------------------------------*/


/* Exported functions ------------------------------------------------------- */

/*  Initializes all the component variables. */
void STM_Init( STM_Handle_t * pHandle );

/* It submits the request for moving the state machine into the state bState */
bool STM_NextState( STM_Handle_t * pHandle, State_t bState );

/* It clocks both HW and SW faults processing and update the state machine accordingly */
State_t STM_FaultProcessing( STM_Handle_t * pHandle, uint16_t hSetErrors, uint16_t
                             hResetErrors );

/* Returns the current state machine state */
State_t STM_GetState( STM_Handle_t * pHandle );

/**
  * It reports to the state machine that the fault state has been
  *        acknowledged by the user.
  */
bool STM_FaultAcknowledged( STM_Handle_t * pHandle );

/**
  * It returns two 16 bit fields containing information about both faults
  * currently present and faults historically occurred since the state
  * machine has been moved into state
  */
uint32_t STM_GetFaultState( STM_Handle_t * pHandle );

/**
  * @}
  */

/**
  * @}
  */


#ifdef __cplusplus
}
#endif /* __cpluplus */

#endif /* __STATEMACHINE_H */

/******************* (C) COPYRIGHT 2019 STMicroelectronics *****END OF FILE****/
