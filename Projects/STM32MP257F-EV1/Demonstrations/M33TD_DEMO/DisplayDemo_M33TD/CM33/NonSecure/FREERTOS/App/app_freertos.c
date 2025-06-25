/**
  ******************************************************************************
  * @file    app_freertos.c
  * @author  MCD Application Team
  * @brief   FreeRTOS application implementation file.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "app_freertos.h"
#include "remoteproc_task.h"
#include "low_power_display_task.h"


/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
void MX_FREERTOS_DeInit(void);
extern void HAL_DeInitTick(void);

/* USER CODE END PFP */

/**
  * @brief  De-initialize the FreeRTOS system.
  *         Disables the system tick and releases RTOS resources if needed.
  * @retval None
  */
void MX_FREERTOS_DeInit(void)
{
  HAL_DeInitTick();
  RemoteProcTask_DeInit();
  LowPowerDisplayTask_DeInit();
}

/**
  * @brief  Initialize the FreeRTOS system and application tasks.
  *         Sets up RTOS objects and starts the Remote Processor Task.
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
     RemoteProcTask_Init();
     LowPowerDisplayTask_Init();
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
  /* create the queue(s) */
  /* USER CODE END RTOS_QUEUES */

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, timers, or semaphores here, ... */
  /* USER CODE END RTOS_EVENTS */

}

/**
  * @brief  Handles application-level errors.
  *         Calls the generic error handler for cleanup and logging.
  * @retval None
  */
 void App_ErrorHandler(void)
 {
     /* You can add logging or RTOS-specific cleanup here if needed */
     Error_Handler();
 }
 
 /************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

