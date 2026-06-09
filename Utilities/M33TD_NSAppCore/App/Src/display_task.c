/**
  ******************************************************************************
  * @file    display_task.c
  * @author  MCD Application Team
  * @brief   Display Task implementation (splash/overlay/reboot state machine).
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

/**
  * @addtogroup DisplayTask
  * @{
  */

/* Includes ------------------------------------------------------------------*/
#include "display_task.h"
#include "display_driver.h"
#if ENABLE_OPENAMP_TASK
#include "openamp_task.h"
#endif

#include <string.h>

/* Private defines -----------------------------------------------------------*/
#define DISPLAY_QUEUE_SIZE  (4U)

/* Private typedef -----------------------------------------------------------*/
/**
  * @brief Display Task state tracking.
  */
typedef struct
{
  DisplayTaskState_t current_state;
  DisplayTaskState_t previous_state;
} DisplayTaskStateInfo_t;

/* Private function prototypes ----------------------------------------------*/
static void DisplayTask(void *argument);
static void DisplayTask_TransitionState(DisplayTaskState_t next_state);

/* Private variables ---------------------------------------------------------*/
static osMessageQueueId_t DisplayTaskQueueHandle;
static osThreadId_t DisplayTaskHandle;

static DisplayTaskStateInfo_t DisplayTaskState =
{
  .current_state = DISPLAY_TASK_STATE_IDLE,
  .previous_state = DISPLAY_TASK_STATE_IDLE,
};

static const osThreadAttr_t DisplayTask_Attributes =
{
  .name       = "DisplayTask",
  .priority   = (osPriority_t)DISPLAY_TASK_PRIORITY,
  .stack_size = DISPLAY_TASK_STACK_SIZE
};

static const osMessageQueueAttr_t DisplayQueue_Attributes =
{
  .name = "DisplayQueue"
};

/* Private functions ---------------------------------------------------------*/
static void DisplayTask_TransitionState(DisplayTaskState_t next_state)
{
  DisplayTaskState.previous_state = DisplayTaskState.current_state;
  DisplayTaskState.current_state = next_state;
}

/**
  * @brief  RPMsg reception callback for the Display endpoint.
  *
  * Expected messages:
  * - DISPLAY_CMD_HIDE_SPLASH_MSG
  * - DISPLAY_CMD_START_REBOOT_MSG
  */
int DisplayTask_RpmsgCallback(struct rpmsg_endpoint *ept, void *data,
                             size_t len, uint32_t src, void *priv)
{
  (void)ept;
  (void)len;
  (void)src;
  (void)priv;

  if (data == NULL)
  {
    return -1;
  }

  if (strncmp((const char *)data, DISPLAY_CMD_HIDE_SPLASH_MSG,
              sizeof(DISPLAY_CMD_HIDE_SPLASH_MSG) - 1U) == 0)
  {
    const DisplayTaskCommand_t cmd = { .type = DISPLAY_CMD_HIDE_SPLASH, .param = 0U };
    (void)DisplayTask_PostCommand(&cmd);
    return 0;
  }

  if (strncmp((const char *)data, DISPLAY_CMD_START_REBOOT_MSG,
              sizeof(DISPLAY_CMD_START_REBOOT_MSG) - 1U) == 0)
  {
    const DisplayTaskCommand_t cmd = { .type = DISPLAY_CMD_REQUEST_REBOOT, .param = 0U };
    (void)DisplayTask_PostCommand(&cmd);
    return 0;
  }

  APP_LOG_WARN("DisplayTask", "Unknown RPMsg command");
  return 0;
}

/**
  * @brief  Initialize the Display Task and its resources.
  * @retval None
  */
void DisplayTask_Init(void)
{
  DisplayTaskQueueHandle = osMessageQueueNew(DISPLAY_QUEUE_SIZE, sizeof(DisplayTaskCommand_t), &DisplayQueue_Attributes);
  if (DisplayTaskQueueHandle == NULL)
  {
    NSAppCore_ErrorHandler();
  }

  DisplayTaskHandle = osThreadNew(DisplayTask, NULL, &DisplayTask_Attributes);
  if (DisplayTaskHandle == NULL)
  {
    NSAppCore_ErrorHandler();
  }

  (void)display_driver.init();
  printf("\033[32m[NS] [INF] [DisplayTask] Display is initialized and ready...\033[0m\r\n");
  /* Default behavior: start in splash mode (driver may ignore). */
  {
    const DisplayTaskCommand_t show_splash_cmd = { .type = DISPLAY_CMD_SHOW_SPLASH, .param = 0U };
    (void)DisplayTask_PostCommand(&show_splash_cmd);
  }
}

/**
  * @brief  Deinitialize the Display Task and release resources.
  * @retval None
  */
void DisplayTask_DeInit(void)
{
  if (DisplayTaskHandle != NULL)
  {
    (void)osThreadTerminate(DisplayTaskHandle);
    DisplayTaskHandle = NULL;
  }

  if (DisplayTaskQueueHandle != NULL)
  {
    (void)osMessageQueueDelete(DisplayTaskQueueHandle);
    DisplayTaskQueueHandle = NULL;
  }

  display_driver.deinit();
}

/**
  * @brief  Post a command to the Display Task queue.
  * @param  cmd: Command to post
  * @retval true if posted successfully, false otherwise
  */
bool DisplayTask_PostCommand(const DisplayTaskCommand_t *cmd)
{
  if ((DisplayTaskQueueHandle == NULL) || (cmd == NULL))
  {
    return false;
  }

  return (osMessageQueuePut(DisplayTaskQueueHandle, cmd, 0U, 0U) == osOK);
}

static void DisplayTask(void *argument)
{
  (void)argument;

  DisplayTaskCommand_t cmd;

  for (;;)
  {
    if (osMessageQueueGet(DisplayTaskQueueHandle, &cmd, NULL, osWaitForever) == osOK)
    {
      switch (DisplayTaskState.current_state)
      {
        case DISPLAY_TASK_STATE_IDLE:
          if (cmd.type == DISPLAY_CMD_SHOW_SPLASH)
          {
            DisplayTask_TransitionState(DISPLAY_TASK_STATE_SPLASH);
            display_driver.show_splash(NULL);
          }
#if defined(OVERLAY_FEATURE_ENABLED)
          else if (cmd.type == DISPLAY_CMD_SHOW_OVERLAY)
          {
            DisplayTask_TransitionState(DISPLAY_TASK_STATE_OVERLAY);
            display_driver.show_overlay(NULL);
          }
#endif
          else if (cmd.type == DISPLAY_CMD_REQUEST_REBOOT)
          {
            DisplayTask_TransitionState(DISPLAY_CMD_STATE_REBOOT);

            {
              const DisplayTaskCommand_t reboot_cmd = { .type = DISPLAY_CMD_START_REBOOT, .param = 0U };
              (void)DisplayTask_PostCommand(&reboot_cmd);
            }
          }
          break;

        case DISPLAY_TASK_STATE_SPLASH:
          if (cmd.type == DISPLAY_CMD_HIDE_SPLASH)
          {
            display_driver.hide_splash(NULL);

            /* Default: after splash, go to overlay if enabled. */
#if defined(OVERLAY_FEATURE_ENABLED)
            DisplayTask_TransitionState(DISPLAY_TASK_STATE_OVERLAY);
            display_driver.show_overlay(NULL);
#else
            DisplayTask_TransitionState(DISPLAY_TASK_STATE_IDLE);
#endif
          }
          else if (cmd.type == DISPLAY_CMD_REQUEST_REBOOT)
          {
            display_driver.hide_splash(NULL);
            DisplayTask_TransitionState(DISPLAY_CMD_STATE_REBOOT);

            {
              const DisplayTaskCommand_t reboot_cmd = { .type = DISPLAY_CMD_START_REBOOT, .param = 0U };
              (void)DisplayTask_PostCommand(&reboot_cmd);
            }
          }
#if defined(SPLASH_ANIMATION_ENABLED)
          else if (cmd.type == DISPLAY_CMD_DECODE_ANIMATION_FRAME)
          {
            display_driver.handle_animation_frame();

            DisplayStatus_t status = display_driver.get_status();
            if (status.panel_state == DISPLAY_PANEL_OFFLINE)
            {
              DisplayTask_TransitionState(DISPLAY_TASK_STATE_IDLE);
              display_driver.hide_splash(NULL);
            }
          }
#endif
          break;

#if defined(OVERLAY_FEATURE_ENABLED)
        case DISPLAY_TASK_STATE_OVERLAY:
          if (cmd.type == DISPLAY_CMD_UPDATE_OVERLAY)
          {
            display_driver.update_overlay((const void *)(uintptr_t)((DisplayOverlayUpdateType_t)cmd.param));
          }
          else if (cmd.type == DISPLAY_CMD_HIDE_OVERLAY)
          {
            display_driver.hide_overlay(NULL);
            DisplayTask_TransitionState(DISPLAY_TASK_STATE_IDLE);
          }
          else if (cmd.type == DISPLAY_CMD_SHOW_OVERLAY)
          {
            display_driver.show_overlay(NULL);
          }
          else if (cmd.type == DISPLAY_CMD_HIDE_SPLASH)
          {
            display_driver.update_overlay((const void *)(uintptr_t)DISPLAY_OVERLAY_UPDATE_STATE_SYNC_EVENT);
          }
          else if (cmd.type == DISPLAY_CMD_REQUEST_REBOOT)
          {
            display_driver.update_overlay((const void *)(uintptr_t)DISPLAY_OVERLAY_UPDATE_REBOOT_EVENT);
            DisplayTask_TransitionState(DISPLAY_CMD_STATE_REBOOT);

            {
              const DisplayTaskCommand_t reboot_cmd = { .type = DISPLAY_CMD_START_REBOOT, .param = 0U };
              (void)DisplayTask_PostCommand(&reboot_cmd);
            }
          }
          break;
#endif

        case DISPLAY_CMD_STATE_REBOOT:
          if (cmd.type == DISPLAY_CMD_START_REBOOT)
          {
            /* Reboot is an intermediate state; project decides overlay behavior.
             * Default implementation asks OpenAMP to reboot the remote processor.
             */
#if ENABLE_OPENAMP_TASK
            OpenampTaskCommand_t openamp_cmd = { .type = OPENAMP_CMD_REBOOT, .payload.param = 0U };
            (void)OpenampTask_PostCommand(&openamp_cmd);
#endif
#if defined(OVERLAY_FEATURE_ENABLED)
            if (DisplayTaskState.previous_state == DISPLAY_TASK_STATE_OVERLAY)
            {
              DisplayTask_TransitionState(DISPLAY_TASK_STATE_OVERLAY);
            }
            else
            {
              DisplayTask_TransitionState(DISPLAY_TASK_STATE_IDLE);
            }
#else
            DisplayTask_TransitionState(DISPLAY_TASK_STATE_IDLE);
#endif
          }
          break;

        default:
          break;
      }
    }
  }
}

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

