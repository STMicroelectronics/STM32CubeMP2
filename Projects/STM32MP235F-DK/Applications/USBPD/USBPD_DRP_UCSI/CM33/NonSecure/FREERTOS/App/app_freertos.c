/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_freertos.c
  * @author  MCD Application Team
  * @brief   app_freertos application implementation file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "cmsis_os2.h"
#include "app_freertos.h"
#include "usbpd_ucsi_if.h"
#include "led_thread.h"
#include "openamp_thread.h"
#include "usbpd.h"
#include "stm32mp235f_disco_usbpd_pwr.h"

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
static osSemaphoreId_t EventSemaphore;
static const osSemaphoreAttr_t semAttr_EventSemaphore = {
    .name = "EventSemaphore",
};
static osSemaphoreId_t ShutDownSemaphore;
static const osSemaphoreAttr_t semAttr_ShutDownSemaphore = {
    .name = "ShutDownSemaphore",
};

static osThreadId_t ThreadAppHandle;
static const osThreadAttr_t ThreadApp_attributes = {
  .name = "APP",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 4048
};
static osThreadId_t ThreadShutdownHandle;
static const osThreadAttr_t ThreadShutdown_attributes = {
  .name = "SHUTDOWN",
  .priority = (osPriority_t) osPriorityAboveNormal,
  .stack_size = 2048
};
static IPCC_HandleTypeDef const *hipcc_handle;
static uint32_t ipcc_ch_id;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
void MainThread_Entry(void);
static void Thread_App_Deinit(void);
/* USER CODE END PFP */

void CoproSync_ShutdownCb(IPCC_HandleTypeDef * hipcc, uint32_t ChannelIndex, IPCC_CHANNELDirTypeDef ChannelDir)
{
  hipcc_handle = hipcc;
  ipcc_ch_id = ChannelIndex;

  (void)osSemaphoreRelease(ShutDownSemaphore);                                                          \
}

void MX_FREERTOS_DeInit(void)
{
  UCSI_I2C_DeInit();
  MX_USBPD_DeInit();
  MX_UCPD1_Deinit();
  MX_HPDMA3_Deinit();
  Thread_Led_Deinit();
  Thread_App_Deinit();

  if(!IS_DEVELOPER_BOOT_MODE())
  {
    Thread_Openamp_DeInit();
  }

  /* Disable TCPP_EN */
  HAL_GPIO_WritePin(TCPP0203_PORT0_EN_GPIO_PORT, TCPP0203_PORT0_EN_GPIO_PIN, GPIO_PIN_RESET);  /* Disable TCPP_EN */
}

static void Thread_App_Deinit(void)
{
  osThreadTerminate(ThreadAppHandle);
}

void Thread_App_Entry(void *argument)
{
  /* Copro Sync Initialization */
  CoproSync_Init();

  if(!IS_DEVELOPER_BOOT_MODE())
  {
     if (osSemaphoreAcquire(EventSemaphore, portMAX_DELAY) != osOK)
     {
        Error_Handler();
     }
  }

  /* OpenAMP is now Synchronized */

  /* Initialize USBPD stack, start related threads */
  MX_USBPD_Init(NULL);

  while(1)
  {
  	osDelay(1000);
  }
}

void Thread_Shutdown_Entry(void *argument)
{
  /* Copro Sync Initialization */
  CoproSync_Init();

  osSemaphoreAcquire(ShutDownSemaphore, osWaitForever);

  MX_FREERTOS_DeInit();
  /* When ready, notify the remote processor that we can be shut down */
  HAL_IPCC_NotifyCPU(hipcc_handle, ipcc_ch_id, IPCC_CHANNEL_DIR_RX);

  while(1);
}

int Thread_App_Init(void)
{
  ThreadAppHandle = osThreadNew(Thread_App_Entry, NULL, &ThreadApp_attributes);
  if (NULL == ThreadAppHandle)
  {
  	return osError;
  }

  return osOK;
}

int Thread_Shutdown_Init(void)
{
  ShutDownSemaphore = osSemaphoreNew(1, 0, &semAttr_ShutDownSemaphore);
  if (NULL == ShutDownSemaphore)
  {
    return osError;
  }
  ThreadShutdownHandle = osThreadNew(Thread_Shutdown_Entry, NULL, &ThreadShutdown_attributes);
  if (NULL == ThreadShutdownHandle)
  {
  	return osError;
  }

  return osOK;
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
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
  /* creation of Task */
  /* Create the event flags group.  */
  EventSemaphore = osSemaphoreNew(1, 0, &semAttr_EventSemaphore);
  if (NULL == EventSemaphore)
  {
    Error_Handler();
  }

  Thread_Led_Init();
  if(!IS_DEVELOPER_BOOT_MODE())
  {
  	Thread_Openamp_Init(EventSemaphore);
  }

  if(Thread_App_Init() != osOK)
  {
    Error_Handler();
  }
  if(Thread_Shutdown_Init() != osOK)
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

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    // This will get called if a task overflows its stack.
    // xTask: Handle of the task that has overflowed its stack.
    // pcTaskName: Name of the task that has overflowed its stack.

    // User action to handle the stack overflow.
    // For example, you might log the overflow, halt the system, or safely reset the device.

    // Example action: Halt the system
    taskDISABLE_INTERRUPTS();
    for (;;)
    {
        // Infinite loop to halt the system
        // Add code to flash an LED, send a message, etc.
    }
}
