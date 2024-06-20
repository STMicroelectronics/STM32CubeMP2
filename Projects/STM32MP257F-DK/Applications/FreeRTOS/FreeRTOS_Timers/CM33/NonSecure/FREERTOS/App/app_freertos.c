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


static osThreadId_t LEDThreadHandle;
static const osThreadAttr_t LEDThread_attributes = {
  .name = "LEDThread",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 512
};

static osTimerId_t LEDTimerHandle;

static __IO uint32_t TimeCounter = 0;
static IPCC_HandleTypeDef const *hipcc_handle;
static uint32_t ipcc_ch_id;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void shutdown_thread_entry(void *argument);

/* USER CODE BEGIN PFP */
static void ToggleLEDsThread(void *argument);
static void osTimerCallback(void *argument);
static void MX_FREERTOS_DeInit(void);
static IPCC_HandleTypeDef const *hipcc_handle;
static uint32_t ipcc_ch_id;

/* USER CODE END PFP */

void MX_FREERTOS_DeInit(void)
{
  HAL_DeInitTick();
  (void) osSemaphoreDelete(ShutDownSemaphore);
  (void) osThreadTerminate(LEDThreadHandle);
  (void) osTimerDelete(LEDTimerHandle);
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
  /* Create the timer(s) */
  LEDTimerHandle = osTimerNew(osTimerCallback, osTimerPeriodic, NULL, NULL);
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* USER CODE BEGIN RTOS_TIMERS */
  osTimerStart(LEDTimerHandle, 200);
  /* USER CODE END RTOS_TIMERS */

  /* creation of Task */
  ShutDownThreadHandle = osThreadNew(shutdown_thread_entry, NULL, &ShutDown_attributes);
   if (NULL == ShutDownThreadHandle)
   {
     Error_Handler();
   }

  LEDThreadHandle = osThreadNew(ToggleLEDsThread, NULL, &LEDThread_attributes);
  if (NULL == LEDThreadHandle)
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
  * @brief  Function implementing the LEDThread thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_ToggleLEDsThread */
static void ToggleLEDsThread(void *argument)
{

  /* USER CODE BEGIN 5 */
  (void) argument;
  /* Infinite loop */
  for(;;)
  {
    osDelay(400);
  }
  /* USER CODE END 5 */
}

/* osTimerCallback function */
static void osTimerCallback(void *argument)
{
  /* USER CODE BEGIN osTimerCallback */
  (void) argument;

  /* Toggle LED1*/
  BSP_LED_Toggle(LED3);
  if (TimeCounter == 25)
  {
      TimeCounter = 0;
  }
  /* USER CODE END osTimerCallback */
}
