/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_azure_rtos.c
  * @author  MCD Application Team
  * @brief   app_azure_rtos application implementation file
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
#include "app_freertos.h"
#include "cmsis_os2.h"

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

static osSemaphoreId_t ShutDownSemaphore;
static const osSemaphoreAttr_t semAttr_ShutDownSemaphore = {
    .name = "ShutDownSemaphore",
};
static osThreadId_t ShutDownThreadHandle;
static const osThreadAttr_t ShutDown_attributes = {
  .name = "ShutDown Thread",
  .priority = (osPriority_t) osPriorityAboveNormal,
  .stack_size = 2 * 1024
};

static osThreadId_t ThreadOneHandle;
static const osThreadAttr_t ThreadOne_attributes = {
  .name = "Thread One",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 512
};

static osThreadId_t ThreadTwoHandle;
static const osThreadAttr_t ThreadTwo_attributes = {
  .name = "Thread Two",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 512
};

static IPCC_HandleTypeDef const *hipcc_handle;
static uint32_t ipcc_ch_id;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
static void ThreadOne_Entry(void *argument);
static void ThreadTwo_Entry(void *argument);
static void MX_FREERTOS_DeInit(void);
static void shutdown_thread_entry(void *argument);
/* USER CODE END PFP */

static void MX_FREERTOS_DeInit(void)
{
  HAL_DeInitTick();
  (void) osSemaphoreDelete(ShutDownSemaphore);
  (void) osThreadTerminate(ThreadOneHandle);
  (void) osThreadTerminate(ThreadTwoHandle);
}

static void shutdown_thread_entry(void *argument)
{
  osSemaphoreAcquire(ShutDownSemaphore, osWaitForever);

  MX_FREERTOS_DeInit();

  /* DeInitialize the LED3 */
  BSP_LED_DeInit(LED3);

  /* When ready, notify the remote processor that we can be shut down */
  HAL_IPCC_NotifyCPU(hipcc_handle, ipcc_ch_id, IPCC_CHANNEL_DIR_RX);

  /* Wait for complete shutdown */
  while(1);
}

/**
  * @brief  Callback from IPCC Interrupt Handler: Remote Processor asks local processor to shutdown
  * @param  hipcc IPCC handle
  * @param  ChannelIndex Channel number
  * @param  ChannelDir Channel direction
  * @retval None
  */
void CoproSync_ShutdownCb(IPCC_HandleTypeDef * hipcc, uint32_t ChannelIndex, IPCC_CHANNELDirTypeDef ChannelDir)
{
  hipcc_handle = hipcc;
  ipcc_ch_id = ChannelIndex;
  (void) osSemaphoreRelease(ShutDownSemaphore);
}

  /**
  * @brief  Define the initial system.
  * @param  first_unused_memory : Pointer to the first unused memory
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
  ShutDownSemaphore = osSemaphoreNew(1, 0, &semAttr_ShutDownSemaphore);
  if (NULL == ShutDownSemaphore)
  {
	Error_Handler();
  }
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
  /* creation of Task */
  ShutDownThreadHandle = osThreadNew(shutdown_thread_entry, NULL, &ShutDown_attributes);
  if (NULL == ShutDownThreadHandle)
  {
    Error_Handler();
  }

  ThreadOneHandle = osThreadNew(ThreadOne_Entry, NULL, &ThreadOne_attributes);
  if (NULL == ThreadOneHandle)
  {
    Error_Handler();
  }

  ThreadTwoHandle = osThreadNew(ThreadTwo_Entry, NULL, &ThreadTwo_attributes);
  if (NULL == ThreadTwoHandle)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/**
  * @brief  Toggle LED3 thread
  * @param  thread not used
  * @retval None
  */
static void ThreadOne_Entry(void *argument)
{
  uint32_t count = 0;
  (void) argument;

  for (;;)
  {
    count = osKernelGetTickCount() + 5000;
    /* Toggle LED3 every 200 ms for 5 s */
    while (count > osKernelGetTickCount())
    {
      BSP_LED_Toggle(LED3);
      osDelay(200);
    }

    /* Turn off LED3 */
    BSP_LED_Off(LED3);

    /* Suspend Thread 1 */
    osThreadSuspend(ThreadOneHandle);

    count = osKernelGetTickCount() + 5000;
    /* Toggle LED3 every 500 ms for 5 s */
    while (count > osKernelGetTickCount())
    {
      BSP_LED_Toggle(LED3);
      osDelay(500);
    }

    /* Resume Thread 2*/
    osThreadResume(ThreadTwoHandle);
  }
}

/**
  * @brief  Toggle LED3 thread
  * @param  argument not used
  * @retval None
  */
static void ThreadTwo_Entry(void *argument)
{
  uint32_t count;
  (void) argument;

  for (;;)
  {
    count = osKernelGetTickCount() + 10000;
    /* Toggle LED3 every 500 ms for 10 s */
    while (count > osKernelGetTickCount())
    {
     BSP_LED_Toggle(LED3);
     osDelay(500);
    }

    /* Turn off LED3 */
    BSP_LED_Off(LED3);

    /* Resume Thread 1 */
    osThreadResume(ThreadOneHandle);

    /* Suspend Thread 2 */
    osThreadSuspend(ThreadTwoHandle);
  }
}
