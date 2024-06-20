/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    openamp_thread.c
  * @author  MCD Application Team
  * @brief   Thread openAMP file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "openamp_thread.h"
#include "rsc_table.h"
#include "metal/sys.h"
#include "metal/device.h"
#include "openamp.h"
#include "openamp_log.h"

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

#define SHM_DEVICE_NAME         "STM32_SHM"

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

static osSemaphoreId_t tx_EventSemaphore;
static osThreadId_t ThreadOpenampHandle;
static const osThreadAttr_t Thread_Openamp_attributes = {
  .name = "OpenAMP",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 2048
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

static void Thread_Openamp_Entry(void *argument);

/**
  * @brief  Application Thread Initialization.
  * @param memory_ptr: memory pointer
  * @retval int
  */
int Thread_Openamp_Init(osSemaphoreId_t EventSemaphore)
{
  tx_EventSemaphore = EventSemaphore;

  /* Create Thread OpenAMP. */
  ThreadOpenampHandle = osThreadNew(Thread_Openamp_Entry, NULL, &Thread_Openamp_attributes);
  if (NULL == ThreadOpenampHandle)
  {
	return osError;
  }

  return osOK;
}

/**
  * @brief  Application Thread Initialization.
  * @param memory_ptr: memory pointer
  * @retval int
  */
int Thread_Openamp_DeInit()
{
  /* USER CODE BEGIN PRE_OPENAMP_DEINIT */

  /* USER CODE END PRE_OPENAMP_DEINIT */

  osThreadTerminate(ThreadOpenampHandle);

  OPENAMP_DeInit();

  /* USER CODE BEGIN POST_OPENAMP_DEINIT */

  /* USER CODE END POST_OPENAMP_DEINIT */

  return 0;
}

/**
  * @brief  Function implementing the ThreadOpenamp thread.
  * @param  thread_input: Not used
  * @retval None
  */
static void Thread_Openamp_Entry(void *argument)
{
  (void) argument;

  /* Libmetal Initilalization */
  if(MX_OPENAMP_Init(RPMSG_REMOTE, NULL))
  {
    Error_Handler();
  }

  if (osSemaphoreRelease(tx_EventSemaphore) != osOK)
  {
    Error_Handler();
  }

  /* OpenAMP Initialized */
  while (1)
  {
    OPENAMP_check_for_message();
  }
}

/* USER CODE END 1 */
