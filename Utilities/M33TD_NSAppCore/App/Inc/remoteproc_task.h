/**
  ******************************************************************************
  * @file    remoteproc_task.h
  * @author  MCD Application Team
  * @brief   Header file for remote processor task implementation.
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

#ifndef REMOTEPROC_TASK_H
#define REMOTEPROC_TASK_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "nsappcore_config.h"

/**
  * @brief  Enumeration for remote processor states.
  */
typedef enum {
    REMOTEPROC_STATE_OFFLINE,    /*!< Remote processor is powered off */
    REMOTEPROC_STATE_STARTING,   /*!< Remote processor is starting */
    REMOTEPROC_STATE_RUNNING,    /*!< Remote processor is running */
    REMOTEPROC_STATE_SUSPENDING, /*!< Remote processor is suspending */
    REMOTEPROC_STATE_SUSPENDED,  /*!< Remote processor is suspended */
    REMOTEPROC_STATE_RESUMING,   /*!< Remote processor is resuming */
    REMOTEPROC_STATE_STOPPING,   /*!< Remote processor is stopping */
    REMOTEPROC_STATE_UNKNOWN     /*!< Unknown state */
} RemoteProcState;

/**
  * @brief  Enumeration for remote processor events.
  */
typedef enum {
    REMOTEPROC_EVENT_START,                 /*!< Request to start the remote processor */
    REMOTEPROC_EVENT_STOP,                  /*!< Request to stop the remote processor */
    REMOTEPROC_EVENT_SUSPEND,               /*!< Request to suspend the remote processor */
    REMOTEPROC_EVENT_RESUME,                /*!< Request to resume the remote processor */
    REMOTEPROC_EVENT_CRASH,                 /*!< Crash detected */
    REMOTEPROC_EVENT_ERROR,                 /*!< Error occurred */
    REMOTEPROC_EVENT_CHECK_STATUS           /*!< Internal: poll CPU state for async command completion */
} RemoteProcEvent;

/**
  * @brief  Callback type for remote processor state changes.
  * @param  new_state: The new state of the remote processor.
  * @param  context: User context pointer.
  */
typedef void (*RemoteProcStateCallback)(RemoteProcState new_state, void *context);

/**
  * @brief  Register a callback for remote processor state changes.
  * @param  cb: Callback function pointer.
  * @param  context: User context pointer.
  * @retval HAL_OK if registered, HAL_ERROR otherwise.
  */
HAL_StatusTypeDef RemoteProcTask_RegisterCallback(RemoteProcStateCallback cb, void *context);

/**
  * @brief  Unregister a callback for remote processor state changes.
  * @param  cb: Callback function pointer.
  * @retval HAL_OK if unregistered, HAL_ERROR otherwise.
  */
HAL_StatusTypeDef RemoteProcTask_UnregisterCallback(RemoteProcStateCallback cb);

/**
  * @brief  Post an event to the remote processor task.
  * @param  event: Event type.
  * @param  param: Event parameter.
  * @retval HAL_OK if posted, HAL_ERROR otherwise.
  */
HAL_StatusTypeDef RemoteProcTask_PostEvent(RemoteProcEvent event, uint32_t param);

/**
  * @brief  Get the current state of the remote processor.
  * @retval Current RemoteProcState.
  */
RemoteProcState RemoteProcTask_GetState(void);

/**
  * @brief  Initialize the remote processor task and its resources.
  * @retval None
  */
void RemoteProcTask_Init(void);

/**
  * @brief  De-initialize the remote processor task and release its resources.
  * @retval None
  */
void RemoteProcTask_DeInit(void);

/**
  * @brief  Get a copy of the remote processor information structure.
  * @retval A copy of the cpu_info_res structure.
  */
struct cpu_info_res RemoteProcTask_GetCoproInfo(void);

#ifdef __cplusplus
}
#endif

#endif /* REMOTEPROC_TASK_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

