/**
  ******************************************************************************
  * @file    low_power_display_task.h
  * @author  MCD Application Team
  * @brief   Header file for the low-power display task.
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

#ifndef LOW_POWER_DISPLAY_TASK_H
#define LOW_POWER_DISPLAY_TASK_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "cmsis_os2.h"
#include "main.h"

/** @addtogroup LowPowerDisplayTask
  * @{
  */

/** @defgroup LowPowerDisplayTask_Types Exported Types
  * @{
  */

/**
  * @brief  Enumeration for display message types.
  */
typedef enum {
  DISPLAY_MSG_BOOT_SCREEN,   /*!< Welcome message for the display */
  DISPLAY_MSG_RTC_UPDATE,    /*!< RTC date/time update message */
  DISPLAY_MSG_COPRO_UPDATE   /*!< CPU state change message */
} DisplayMessageType;

/**
  * @brief  Structure for display messages.
  */
typedef struct {
  DisplayMessageType type; /*!< Type of the display message */
  uint32_t data;           /*!< Additional data */
} DisplayMessage;

/**
  * @}
  */

/** @defgroup LowPowerDisplayTask_Functions Exported Functions
  * @{
  */

/**
  * @brief  Initialize the low-power display task and its resources.
  * @retval None
  */
void LowPowerDisplayTask_Init(void);

/**
  * @brief  De-initialize the low-power display task and release its resources.
  * @retval None
  */
void LowPowerDisplayTask_DeInit(void);

/**
  * @brief  Enqueue a message to the Low Power Display Task's message queue.
  * @param  msg: Pointer to the `DisplayMessage` structure to enqueue.
  * @retval HAL_OK if the message was successfully enqueued, HAL_ERROR otherwise.
  */
HAL_StatusTypeDef LowPowerDisplay_PostMsg(const DisplayMessage *msg);

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* LOW_POWER_DISPLAY_TASK_H */

/************************ (C) COPYRIGHT MCD Application Team *****END OF FILE****/
