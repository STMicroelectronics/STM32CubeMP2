/**
  ******************************************************************************
  * @file    userapp_task.c
  * @author  MCD Application Team
  * @brief   User application task implementation file.
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
#include "userapp_task.h"
#include "cmsis_os2.h"
#include "app_tasks_config.h"
#include <stdio.h>
#include "app_freertos.h"

/* Private variables ---------------------------------------------------------*/
/**
  * @brief  RTOS thread handle for the User Application Task.
  */
static osThreadId_t UserAppTaskHandle;

/**
  * @brief  RTOS thread attributes for the User Application Task.
  */
static const osThreadAttr_t UserAppTaskAttr = {
    .name = "UserAppTask",
    .priority = (osPriority_t)USERAPP_TASK_PRIORITY,
    .stack_size = USERAPP_TASK_STACK_SIZE
};

/* Private function prototypes -----------------------------------------------*/
/**
  * @brief  Main function for the User Application Task.
  * @param  argument: Not used.
  * @retval None
  */
static void UserAppTask(void *argument);

/* Exported functions --------------------------------------------------------*/
/**
  * @brief  Initialize the User Application Task and its resources.
  * @retval None
  */
void UserAppTask_Init(void)
{
    UserAppTaskHandle = osThreadNew(UserAppTask, NULL, &UserAppTaskAttr);
    if (UserAppTaskHandle == NULL) {
    	App_ErrorHandler();
    }
}

/**
  * @brief  De-initialize the User Application Task and release its resources.
  * @retval None
  */
void UserAppTask_DeInit(void)
{
    if (UserAppTaskHandle != NULL) {
        osThreadTerminate(UserAppTaskHandle);
        UserAppTaskHandle = NULL;
    }
}

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Main function for the User Application Task.
  *         Toggles LED3 every 500ms.
  * @param  argument: Not used.
  * @retval None
  */
static void UserAppTask(void *argument)
{
    (void)argument;
    // Starter: Blink LED
    for (;;) {
        BSP_LED_Toggle(LED3);
        osDelay(1000);
    }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
