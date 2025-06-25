/**
  ******************************************************************************
  * @file    low_power_display_task.c
  * @author  MCD Application
  * @brief   Task to drive the low-power I2C display.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 MCD Application Team.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "low_power_display_task.h"
#include "cmsis_os2.h"
#include "ssd1306.h"
#include "app_freertos.h"
#include "remoteproc_task.h"
#include "app_tasks_config.h"

/** @addtogroup Display_Task_Module
  * @{
  */

/* Private variables ---------------------------------------------------------*/
/** @brief Handle for the display thread. */
static osThreadId_t DisplayThreadHandle;

/** @brief Attributes for the display thread. */
static const osThreadAttr_t DisplayThread_attributes = {
    .name = "LowPowerDisplayTask",
    .priority = (osPriority_t)LOW_POWER_DISPLAY_TASK_PRIORITY,
    .stack_size = LOW_POWER_DISPLAY_TASK_STACK_SIZE
};

/** @brief Message queue for display updates. */
osMessageQueueId_t display_update_msg_queue = NULL;

/** @brief Attributes for the display update message queue. */
static const osMessageQueueAttr_t display_update_msg_queue_attributes = {
    .name = "DisplayUpdateQueue"
};

/** @brief Buffer used for displaying the date. */
static char aShowDate[20] = "Day dd/mm/yy";

/** @brief Buffer used for displaying the time. */
static char aShowTime[20] = "hh:ms:ss";

/** @brief CPU status strings for the coprocessor. */
static const char *CoproCpuStatusStr[] = {
    "offline",
    "suspended",
    "started",
    "running",
    "crashed",
    "unknown",
};

/* Private function prototypes -----------------------------------------------*/
/**
  * @brief  Task to update the low-power I2C display.
  * @param  argument: Not used.
  * @retval None
  */
static void LowPowerDisplayTask(void *argument);

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Initialize the display task and its resources.
  * @retval None
  */
void LowPowerDisplayTask_Init(void)
{
    /* Create the message queue */
    display_update_msg_queue = osMessageQueueNew(5, sizeof(DisplayMessage), &display_update_msg_queue_attributes);
    if (display_update_msg_queue == NULL)
    {
        Error_Handler();
    }

    DisplayMessage msg = { .type = DISPLAY_MSG_BOOT_SCREEN, .data = 0 };
    LowPowerDisplay_PostMsg(&msg);

    /* Create the display thread */
    DisplayThreadHandle = osThreadNew(LowPowerDisplayTask, NULL, &DisplayThread_attributes);
    if (DisplayThreadHandle == NULL)
    {
        Error_Handler();
    }
}

/**
  * @brief  De-initialize the display task and release its resources.
  * @retval None
  */
void LowPowerDisplayTask_DeInit(void)
{

    MX_EXTI2_DeInit();

    if (DisplayThreadHandle != NULL)
    {
        osThreadTerminate(DisplayThreadHandle);
        DisplayThreadHandle = NULL;
    }

    if (display_update_msg_queue != NULL)
    {
        osMessageQueueDelete(display_update_msg_queue);
        display_update_msg_queue = NULL;
    }
}

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Task to update the low-power I2C display.
  * @param  argument: Not used.
  * @retval None
  */
static void LowPowerDisplayTask(void *argument)
{
    (void)argument;
    DisplayMessage rMsg;

    for (;;)
    {
        if (osMessageQueueGet(display_update_msg_queue, &rMsg, 0, osWaitForever) == osOK)
        {
            switch (rMsg.type)
            {
            case DISPLAY_MSG_BOOT_SCREEN:
            {
                // Prepare the display
                ssd1306_Fill(Black);
                ssd1306_SetCursor(8, 2);
                ssd1306_WriteString("M33TD Display Demo", Font_6x8, White);
                RTC_TimeShow(aShowDate, aShowTime);
                ssd1306_SetCursor(25, 14);
                ssd1306_WriteString(aShowDate, Font_6x8, White);
                ssd1306_SetCursor(30, 24);
                ssd1306_WriteString(aShowTime, Font_6x8, White);
                ssd1306_SetCursor(15, 38);
                ssd1306_WriteString("M33 : running", Font_6x8, White);

                // Fetch the copro info
                struct cpu_info_res coproInfo = RemoteProc_GetCoproInfo();

                // Use the string directly in sprintf
                char displayStr[32];
                sprintf(displayStr, "A35 : %s", CoproCpuStatusStr[coproInfo.status]);
                ssd1306_SetCursor(15, 48);
                ssd1306_WriteString(displayStr, Font_6x8, White);
                break;
            }

            case DISPLAY_MSG_RTC_UPDATE:
            {
                RTC_TimeShow(aShowDate, aShowTime);
                ssd1306_FillRectangle(0, 14, 128, 32, Black);
                ssd1306_SetCursor(25, 14);
                ssd1306_WriteString(aShowDate, Font_6x8, White);
                ssd1306_SetCursor(30, 24);
                ssd1306_WriteString(aShowTime, Font_6x8, White);
                break;
            }

            case DISPLAY_MSG_COPRO_UPDATE:
            {
                // Fetch the copro info and use the string directly
                struct cpu_info_res coproInfo = RemoteProc_GetCoproInfo();

                // Use the string directly in sprintf
                char displayStr[32];
                sprintf(displayStr, "A35 : %s", CoproCpuStatusStr[coproInfo.status]);
                ssd1306_FillRectangle(0, 48, 128, 56, Black);
                ssd1306_SetCursor(15, 48);
                ssd1306_WriteString(displayStr, Font_6x8, White);
                break;
            }

            default:
                break;
            }

            ssd1306_UpdateScreen();
        }
    }
}

/**
  * @brief  Enqueue a message to the Low Power Display Task's message queue.
  * @param  msg: Pointer to the `DisplayMessage` structure to enqueue.
  * @retval HAL_OK if the message was successfully enqueued, HAL_ERROR otherwise.
  */
HAL_StatusTypeDef LowPowerDisplay_PostMsg(const DisplayMessage *msg)
{
    if (display_update_msg_queue == NULL || msg == NULL) {
        return HAL_ERROR;
    }
    if (osMessageQueuePut(display_update_msg_queue, msg, 0, 0) == osOK) {
        return HAL_OK;
    }
    return HAL_ERROR;
}

/**
  * @}
  */

/************************ (C) COPYRIGHT MCD Application Team *****END OF FILE****/
