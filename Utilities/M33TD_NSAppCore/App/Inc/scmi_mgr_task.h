 /**
  ******************************************************************************
  * @file    scmi_mgr_task.h
  * @author  MCD Application Team
  * @brief   Header file for SCMI Manager task implementation.
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

#ifndef SCMI_MGR_TASK_H
#define SCMI_MGR_TASK_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "nsappcore_config.h"

/**
  * @brief  SCMI Power event types.
  */
typedef enum {
    SCMI_POWER_EVENT_SHUTDOWN,    /*!< System shutdown event */
    SCMI_POWER_EVENT_COLD_RESET,  /*!< System cold reset event */
    SCMI_POWER_EVENT_WARM_RESET,  /*!< System warm reset event */
    SCMI_POWER_EVENT_SUSPEND,     /*!< System suspend event */
    SCMI_POWER_EVENT_UNKNOWN      /*!< Unknown/unsupported event */
} ScmiPowerEvent_t;

/**
  * @brief  SCMI Manager event listener callback type.
  * @param  event: The SCMI power event.
  * @param  context: User context pointer.
  * @retval None
  */
typedef void (*ScmiMgrTask_EventListener)(ScmiPowerEvent_t event, void *context);

/**
  * @brief  Initialize the SCMI Manager Task and its resources.
  *         Creates the semaphore and the task thread.
  * @retval None
  */
void ScmiMgrTask_Init(void);

/**
  * @brief  De-initialize the SCMI Manager Task and release its resources.
  * @retval None
  */
void ScmiMgrTask_DeInit(void);

/**
  * @brief  Signal the SCMI Manager Task from IRQ context.
  * @retval None
  */
void ScmiMgrTask_Signal(void);

/**
  * @brief  Mask SCMI-related non-secure notifications during critical stop sequences.
  * @retval None
  */
void ScmiMgrTask_DisableNotifications(void);

/**
  * @brief  Reset SCMI shared state and re-enable SCMI-related notifications.
  * @retval None
  */
void ScmiMgrTask_ResetAndEnableNotifications(void);

/**
  * @brief  Register a listener for SCMI power events.
  * @param  cb: Callback function to be called on event.
  * @param  context: User context pointer.
  * @retval HAL_OK if registered, HAL_ERROR if no slot available.
  */
HAL_StatusTypeDef ScmiMgrTask_RegisterListener(ScmiMgrTask_EventListener cb, void *context);

/**
  * @brief  Unregister a listener for SCMI power events.
  * @param  cb: Callback function to remove.
  * @retval HAL_OK if unregistered, HAL_ERROR if not found.
  */
HAL_StatusTypeDef ScmiMgrTask_UnregisterListener(ScmiMgrTask_EventListener cb);

#ifdef __cplusplus
}
#endif

#endif /* SCMI_MGR_TASK_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

