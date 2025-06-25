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
/**
  * @brief Includes required for FreeRTOS application functionality.
  */
#include "app_freertos.h"
#include "remoteproc_task.h"
#include "userapp_task.h"
#include "testapp_task.h"

/* Private includes ----------------------------------------------------------*/
/**
  * @brief Private includes for additional headers required by this file.
  */
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/**
  * @brief Private typedefs used in this file.
  */
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/**
  * @brief Private defines used in this file.
  */
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/**
  * @brief Private macros used in this file.
  */
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/**
  * @brief Private variables used in this file.
  */
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes ------------------------------------------------*/
/**
  * @brief Private function prototypes used in this file.
  */
/* USER CODE BEGIN PFP */
void MX_FREERTOS_DeInit(void);
extern void HAL_DeInitTick(void);
/* USER CODE END PFP */

/* Exported functions --------------------------------------------------------*/
/**
  * @brief  De-initialize the FreeRTOS system.
  *         Disables the system tick and releases RTOS resources if needed.
  * @retval None
  */
void MX_FREERTOS_DeInit(void)
{
  HAL_DeInitTick();
  RemoteProcTask_DeInit();
  UserAppTask_DeInit();
  TestAppTask_DeInit();
}

/**
  * @brief  Initialize the FreeRTOS system and application tasks.
  *         Sets up RTOS objects and starts the Remote Processor Task.
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
     RemoteProcTask_Init();
     UserAppTask_Init();
     TestAppTask_Init();
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* Add mutexes, if required */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* Add semaphores, if required */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* Start timers, add new ones, if required */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* Create the queue(s) */
  /* USER CODE END RTOS_QUEUES */

  /* USER CODE BEGIN RTOS_THREADS */
  /* Add threads, if required */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* Add events, timers, or semaphores here, if required */
  /* USER CODE END RTOS_EVENTS */
}

/**
  * @brief  Handles application-level errors.
  *         Calls the generic error handler for cleanup and logging.
  * @retval None
  */
void App_ErrorHandler(void)
{
    // You can add logging or RTOS-specific cleanup here if needed
    Error_Handler();
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

