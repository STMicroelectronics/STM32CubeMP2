/**
  ******************************************************************************
  * @file    openamp_task.h
  * @author  MCD Application Team
  * @brief   Header file for OpenAMP task implementation.
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

#ifndef OPENAMP_TASK_H
#define OPENAMP_TASK_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "nsappcore_config.h"

#if ENABLE_FWU_MGR_TASK
#include "fwu_mgr_task.h"
#endif

/**
  * @brief  Enumeration for OpenAMP task commands.
  */
typedef enum
{
  OPENAMP_CMD_INIT = 0,
  OPENAMP_CMD_RX_EVENT,
  OPENAMP_CMD_REINIT,
  OPENAMP_CMD_SHUTDOWN,
  OPENAMP_CMD_REBOOT,
#if ENABLE_FWU_MGR_TASK
  OPENAMP_CMD_FWU_TX,
#endif
} OpenampCommand_t;

/**
  * @brief  OpenAMP task payload.
  * @note   Scalar commands use `param`; FWU transmit commands use `fwuMessage`.
  */
typedef union
{
  uint32_t param;
#if ENABLE_FWU_MGR_TASK
  FwuMgrMessage_t fwuMessage;
#endif
} OpenampTaskPayload_t;

/**
  * @brief  OpenAMP task command.
  */
typedef struct
{
  OpenampCommand_t type;
  OpenampTaskPayload_t payload;
} OpenampTaskCommand_t;

/**
  * @brief  Initialize the OpenAMP Task and its resources.
  *         Creates the OpenAMP thread with attributes defined in the module.
  * @retval None
  */
void OpenampTask_Init(void);

/**
  * @brief  De-initialize the OpenAMP Task and release resources.
  *         Terminates the OpenAMP thread and deinitializes OpenAMP.
  * @retval None
  */
void OpenampTask_DeInit(void);

/**
  * @brief  Post a command to the OpenAMP task queue.
  * @param  cmd: Command to post.
  * @retval true if posted successfully, false otherwise.
  */
bool OpenampTask_PostCommand(const OpenampTaskCommand_t *cmd);

#ifdef __cplusplus
}
#endif

#endif /* OPENAMP_TASK_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

