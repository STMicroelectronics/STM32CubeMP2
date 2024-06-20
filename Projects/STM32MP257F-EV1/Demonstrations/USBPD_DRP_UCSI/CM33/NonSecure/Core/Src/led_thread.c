/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    led_thread.c
  * @author  MCD Application Team
  * @brief   Thread LED file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "cmsis_os2.h"
#include "main.h"

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
static osThreadId_t ThreadLedHandle;
static const osThreadAttr_t ThreadLed_attributes = {
  .name = "Thread Led",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 1024
};
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void Thread_Led_Entry(void *argument);

/* USER CODE BEGIN PFP */
/* USER CODE END PFP */

/**
  * @brief  Application Thread Initialization.
  * @param memory_ptr: memory pointer
  * @retval int
  */

int Thread_Led_Init(void)
{
  ThreadLedHandle = osThreadNew(Thread_Led_Entry, NULL, &ThreadLed_attributes);
  if (NULL == ThreadLedHandle)
  {
	return osError;
  }

  return osOK;
}

/**
  * @brief  Application Thread Deinitialization.
  * @param memory_ptr: memory pointer
  * @retval int
  */
void Thread_Led_Deinit()
{
  osThreadTerminate(ThreadLedHandle);
  /* DeInit LED */
  BSP_LED_Off(LED_ORANGE);
  BSP_LED_DeInit(LED_ORANGE);
  BSP_COM_DeInit(COM_VCP_CM33);
}

/**
  * @brief  Function implementing the Led blinking thread.
  *
  * @param  thread_input: Not used
  * @retval None
  */
static void Thread_Led_Entry(void *argument)
{
  /* Infinite loop */
  while(1)
  {
    BSP_LED_On(LED_ORANGE);
    osDelay(300);
    BSP_LED_Off(LED_ORANGE);
    osDelay(300);
  }
}

/* USER CODE END 1 */
