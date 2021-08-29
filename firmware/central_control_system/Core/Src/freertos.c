/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "imu_task.h"
#include "bmmcp_master_task.h"
#include "command_console_task.h"
#include "shared_task_resources.h"
#include "object_manager.h"
#include "logging.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

const bool debug = true;


#define IMU_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE + 100)
#define BMMCP_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE + 100)
osThreadId_t imu_task_handle;
uint8_t imu_task_cb[sizeof(StaticTask_t)] = {0};
uint8_t imu_task_stack[IMU_TASK_STACK_SIZE * 4] = {0};
const osThreadAttr_t imu_task_attributes = {
  .name = "imuTask",
  .priority = (osPriority_t) osPriorityRealtime1,
  .cb_mem = imu_task_cb,
  .cb_size = sizeof(StaticTask_t),
  .stack_mem = imu_task_stack,
  .stack_size = IMU_TASK_STACK_SIZE * 4,
};

osThreadId_t bmmcp_master_task_handle;
uint8_t bmmcp_master_task_cb[sizeof(StaticTask_t)] = {0};
uint8_t bmmcp_master_task_stack[BMMCP_TASK_STACK_SIZE * 4] = {0};
const osThreadAttr_t bmmcp_master_task_attributes = {
  .name = "bmmcp_task",
  .priority = (osPriority_t) osPriorityRealtime,
  .cb_mem = bmmcp_master_task_cb,
  .cb_size = sizeof(StaticTask_t),
  .stack_mem = bmmcp_master_task_stack,
  .stack_size = BMMCP_TASK_STACK_SIZE * 4,
};
BMMCP_MASTER_task_arg_t BMMCP_MASTER_task_arg = {.debugging = debug};

#define COMMAND_CONSOLE_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE + 100)
osThreadId_t command_console_task_handle;
uint8_t command_console_task_cb[sizeof(StaticTask_t)] = {0};
uint8_t command_console_task_stack[COMMAND_CONSOLE_TASK_STACK_SIZE * 4] = {0};
const osThreadAttr_t command_console_task_attributes = {
  .name = "commandCon",
  .priority = (osPriority_t) osPriorityNormal1,
  .cb_mem = command_console_task_cb,
  .cb_size = sizeof(StaticTask_t),
  .stack_mem = command_console_task_stack,
  .stack_size = COMMAND_CONSOLE_TASK_STACK_SIZE * 4,
};

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */

    LogInfo("Initializing shared task resources..");
	if (SHARED_TASK_init() != 0) {
		LogError("Failed to initialize shared task resources");
	}
    LogInfo("Initializing objects..");
    const ERRORS_return_t result = OBJECT_MANAGER_init();
	if (result != ERRORS_ok) {
		LogError("Failed to initialize objects: %s", ERRORS_error_to_string(result));
	}
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  //imu_task_handle = osThreadNew(StartImuTask, NULL, &imu_task_attributes);
  bmmcp_master_task_handle = osThreadNew(start_bmmcp_master_task, (void *)&BMMCP_MASTER_task_arg, &bmmcp_master_task_attributes);
  command_console_task_handle = osThreadNew(start_command_console_task, NULL, &command_console_task_attributes);
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
