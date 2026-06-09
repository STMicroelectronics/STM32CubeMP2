/**
  ******************************************************************************
  * @file    display_task.h
  * @author  MCD Application Team
  * @brief   Header file for Display Task (splash/overlay/reboot state machine).
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

#ifndef DISPLAY_TASK_H
#define DISPLAY_TASK_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "nsappcore_config.h"

struct rpmsg_endpoint;

/**
  * @brief  Message string to trigger splashscreen hide action via RPMsg/OpenAMP.
  */
#define DISPLAY_CMD_HIDE_SPLASH_MSG   "STOP_SPLASH_SCREEN"

/**
  * @brief  Message string to trigger reboot sequence via RPMsg/OpenAMP.
  */
#define DISPLAY_CMD_START_REBOOT_MSG  "START_REBOOT"

/**
  * @brief  Enumeration for display task commands.
  *
  * Note: Enum nomenclature intentionally matches the demo implementation.
  */
typedef enum
{
  DISPLAY_CMD_SHOW_SPLASH = 0,
  DISPLAY_CMD_HIDE_SPLASH,
  DISPLAY_CMD_DECODE_ANIMATION_FRAME,
#if OVERLAY_FEATURE_ENABLED
  DISPLAY_CMD_SHOW_OVERLAY,
  DISPLAY_CMD_UPDATE_OVERLAY,
  DISPLAY_CMD_HIDE_OVERLAY,
#endif
  DISPLAY_CMD_REQUEST_REBOOT,
  DISPLAY_CMD_START_REBOOT,
} DisplayCommand_t;

#if OVERLAY_FEATURE_ENABLED
/**
  * @brief Generic overlay update events routed through DISPLAY_CMD_UPDATE_OVERLAY.
  */
typedef enum
{
  DISPLAY_OVERLAY_UPDATE_FRAME_EVENT = 0,
  DISPLAY_OVERLAY_UPDATE_REBOOT_EVENT,
  DISPLAY_OVERLAY_UPDATE_STATE_SYNC_EVENT,
} DisplayOverlayUpdateType_t;
#endif

/**
  * @brief Display Task state machine states.
  *
  * Note: Enum nomenclature intentionally matches the demo implementation.
  */
typedef enum
{
  DISPLAY_TASK_STATE_IDLE = 0,
  DISPLAY_TASK_STATE_SPLASH,
#if OVERLAY_FEATURE_ENABLED
  DISPLAY_TASK_STATE_OVERLAY,
#endif
  DISPLAY_CMD_STATE_REBOOT,
} DisplayTaskState_t;

/**
  * @brief Display task command.
  * `param` is command-specific. For DISPLAY_CMD_UPDATE_OVERLAY it carries
  * a DisplayOverlayUpdateType_t value.
  */
typedef struct
{
  DisplayCommand_t type;
  uint32_t param;
} DisplayTaskCommand_t;

/**
  * @brief  Initialize the Display Task and its resources.
  */
void DisplayTask_Init(void);

/**
  * @brief  Deinitialize the Display Task and release resources.
  */
void DisplayTask_DeInit(void);

/**
  * @brief  Post a command to the Display Task queue.
  * @param  cmd: Command to post
  * @retval true if posted successfully, false otherwise
  */
bool DisplayTask_PostCommand(const DisplayTaskCommand_t *cmd);

/**
  * @brief  RPMsg reception callback for the Display endpoint.
  *
  * Expected messages:
  * - DISPLAY_CMD_HIDE_SPLASH_MSG
  * - DISPLAY_CMD_START_REBOOT_MSG
  */
int DisplayTask_RpmsgCallback(struct rpmsg_endpoint *ept, void *data,
                             size_t len, uint32_t src, void *priv);

#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_TASK_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

