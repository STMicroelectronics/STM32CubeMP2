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
#include "main.h"
#include "portmacrocommon.h"
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
#define TX_APP_SINGLE_MSG_SIZE                                     sizeof(uint32_t)
#define TX_APP_MSG_QUEUE_NB_MSG                                    10
#define TOGGLE_LED                                                 1
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

static osThreadId_t MsgSenderThreadOneHandle;
static const osThreadAttr_t MsgSenderThreadOne_attributes = {
  .name = "Message Queue Sender Thread One",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 512
};

static osThreadId_t MsgSenderThreadTwoHandle;
static const osThreadAttr_t MsgSenderThreadTwo_attributes = {
  .name = "Message Queue Sender Thread Two",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 512
};

static osThreadId_t MsgReceiverThreadHandle;
static const osThreadAttr_t MsgReceiverThread_attributes = {
  .name = "Message Queue Receiver Thread",
  .priority = (osPriority_t) osPriorityBelowNormal,
  .stack_size = 512
};

static osMessageQueueId_t tx_app_msg_queue = NULL;
static osMessageQueueAttr_t tx_app_msg_queue_attributes = {
  .name = "Message Queue One",
};

static osMessageQueueId_t MsgQueueTwo = NULL;
static osMessageQueueAttr_t MsgQueueTwo_attributes = {
  .name = "Message Queue Two",
};

static IPCC_HandleTypeDef const *hipcc_handle;
static uint32_t ipcc_ch_id;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
static void MsgSenderThreadOne_Entry(void *argument);
static void MsgSenderThreadTwo_Entry(void *argument);
static void MsgReceiverThread_Entry(void *argument);
static void MX_FREERTOS_DeInit(void);
static void shutdown_thread_entry(void *argument);
/* USER CODE END PFP */

static void MX_FREERTOS_DeInit(void)
{
  HAL_DeInitTick();
  (void) osSemaphoreDelete(ShutDownSemaphore);
  (void) osMessageQueueDelete(tx_app_msg_queue);
  (void) osMessageQueueDelete(MsgQueueTwo);
  (void) osThreadTerminate(MsgSenderThreadOneHandle);
  (void) osThreadTerminate(MsgSenderThreadTwoHandle);
  (void) osThreadTerminate(MsgReceiverThreadHandle);
}

static void shutdown_thread_entry(void *argument)
{
  osSemaphoreAcquire(ShutDownSemaphore, osWaitForever);

  MX_FREERTOS_DeInit();

  /* Deinitialize the LED3 */
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

  MsgSenderThreadOneHandle = osThreadNew(MsgSenderThreadOne_Entry, NULL, &MsgSenderThreadOne_attributes);
  if (NULL == MsgSenderThreadOneHandle)
  {
    Error_Handler();
  }

  MsgSenderThreadTwoHandle = osThreadNew(MsgSenderThreadTwo_Entry, NULL, &MsgSenderThreadTwo_attributes);
  if (NULL == MsgSenderThreadTwoHandle)
  {
    Error_Handler();
  }

  MsgReceiverThreadHandle = osThreadNew(MsgReceiverThread_Entry, NULL, &MsgReceiverThread_attributes);
  if (NULL == MsgReceiverThreadHandle)
  {
    Error_Handler();
  }

  tx_app_msg_queue = osMessageQueueNew(TX_APP_MSG_QUEUE_NB_MSG, TX_APP_SINGLE_MSG_SIZE, &tx_app_msg_queue_attributes);
  if (NULL == tx_app_msg_queue)
  {
    Error_Handler();
  }

  MsgQueueTwo = osMessageQueueNew(TX_APP_MSG_QUEUE_NB_MSG, TX_APP_SINGLE_MSG_SIZE, &MsgQueueTwo_attributes);
  if (NULL == MsgQueueTwo)
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

static void MsgSenderThreadOne_Entry(void *argument)
{
  /* USER CODE BEGIN defaultTask */
  uint32_t msg = TOGGLE_LED;
  /* Infinite loop */
  for(;;)
  {
	if (osMessageQueuePut(tx_app_msg_queue, &msg, 0, portMAX_DELAY) != osOK)
	{
	  Error_Handler();
	}

	osDelay(500);

	/* Resume Thread 2  */
	osThreadResume(MsgSenderThreadTwoHandle);

	/* Suspend Thread 1  */
	osThreadSuspend(MsgSenderThreadOneHandle);
  }
  /* USER CODE END defaultTask */
}

static void MsgSenderThreadTwo_Entry(void *argument)
{
  /* USER CODE BEGIN defaultTask */
  /* USER CODE BEGIN defaultTask */
  uint32_t msg = TOGGLE_LED;
  /* Infinite loop */
  for(;;)
  {
	if (osMessageQueuePut(MsgQueueTwo, &msg, 0, portMAX_DELAY) != osOK)
	{
	  Error_Handler();
	}

	osDelay(500);

	/* Resume Thread 1  */
	osThreadResume(MsgSenderThreadOneHandle);

	/* Suspend Thread 2  */
	osThreadSuspend(MsgSenderThreadTwoHandle);
  }
  /* USER CODE END defaultTask */
}

static void MsgReceiverThread_Entry(void *argument)
{
  /* USER CODE BEGIN defaultTask */
  uint32_t RMsg = 0;

  /* Infinite loop */
  for(;;)
  {
	if (osMessageQueueGet(tx_app_msg_queue, &RMsg, 0, 0) == osOK)
	{
      if (RMsg != TOGGLE_LED)
      {
    	Error_Handler();
      }
      else
      {
    	BSP_LED_On(LED3);
      }
	}
	else
	{
	  if (osMessageQueueGet(MsgQueueTwo, &RMsg, 0, 0) == osOK)
	  {
        if (RMsg != TOGGLE_LED)
	    {
		  Error_Handler();
	    }
	    else
	    {
	      BSP_LED_Off(LED3);
	    }
	  }
	}
  }
  /* USER CODE END defaultTask */
}
