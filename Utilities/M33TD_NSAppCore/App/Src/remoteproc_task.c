/**
  ******************************************************************************
  * @file    remoteproc_task.c
  * @author  MCD Application Team
  * @brief   Remote processor task implementation file.
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
  * @addtogroup RemoteProcTask
  * @{
  */

#include "remoteproc_task.h"
#include "remoteproc_driver.h"
#include "scmi_mgr_task.h"
#include "tfm_ioctl_cpu_api.h"
#include <string.h>
#include <stdio.h>

/**
  * @brief Maximum number of remote processor state callbacks.
  */
#define REMOTEPROC_MAX_CALLBACKS 4

/**
  * @brief Timeout and interval for asynchronous copro commands.
  */
#define REMOTEPROC_CMD_TIMEOUT_MS         1000U
#define REMOTEPROC_CMD_CHECK_INTERVAL_MS  10U

/**
  * @brief Structure for remote processor event message.
  */
typedef struct {
    RemoteProcEvent event; /**< Event type. */
    uint32_t param;        /**< Event parameter. */
} RemoteProcEventMsg;

/**
  * @brief Structure for remote processor state callback entry.
  */
typedef struct {
    RemoteProcStateCallback cb; /**< Callback function pointer. */
    void *context;              /**< User context. */
} RemoteProcCallbackEntry;

/**
  * @brief Function pointer type for CPU commands.
  */
typedef enum tfm_platform_err_t (*RemoteProcCpuCmdFn_t)(uint32_t cpu_id, int32_t *status);

/**
  * @brief Recovery reason for a stop/start restart sequence.
  */
typedef enum {
    REMOTEPROC_RECOVERY_NONE = 0,
    REMOTEPROC_RECOVERY_CRASH,
    REMOTEPROC_RECOVERY_RESUME,
} RemoteProcRecoveryType_t;

/**
  * @brief List of registered remote processor state callbacks.
  */
static RemoteProcCallbackEntry remoteProcCallbackList[REMOTEPROC_MAX_CALLBACKS];

/**
  * @brief Handle for the event message queue.
  */
static osMessageQueueId_t remoteProcEventQueueHandle;

/**
  * @brief Handle for the remote processor task thread.
  */
static osThreadId_t remoteProcTaskHandle;

/**
  * @brief Structure holding coprocessor information.
  */
static struct cpu_info_res remoteProcCoproInfo;

/**
  * @brief Current state of the remote processor.
  */
static RemoteProcState remoteProcCurrentState = REMOTEPROC_STATE_UNKNOWN;

/**
  * @brief String representations of coprocessor CPU status.
  */
static const char *remoteProcCpuStatusStr[] = {
    "offline", "suspended", "started", "running", "crashed", "unknown"
};

/**
  * @brief Tick count for the currently active async command.
  */
static uint32_t remoteProcCmdTick = 0U;

/**
  * @brief CPU ID to restart after stop/start recovery.
  */
static uint32_t remoteProcRecoveryCpuId = 0;

/**
  * @brief Recovery reason for a stop/start restart sequence.
  */
static RemoteProcRecoveryType_t remoteProcRecoveryType = REMOTEPROC_RECOVERY_NONE;

/**
  * @brief Track whether SCMI notifications are currently masked for a stop sequence.
  */
static bool remoteProcScmiNotifMasked = false;

/**
  * @brief Attributes for the event message queue.
  */
static const osMessageQueueAttr_t remoteProcEventQueueAttr = {
    .name = "RemoteProcEventQueue"
};

/**
  * @brief Attributes for the remote processor task thread.
  */
static const osThreadAttr_t remoteProcTaskAttr = {
    .name = "RemoteProcTask",
    .priority = (osPriority_t) REMOTEPROC_TASK_PRIORITY,
    .stack_size = REMOTEPROC_TASK_STACK_SIZE
};

/* Forward declarations */
static void RemoteProcTask(void *argument);
static void RemoteProcTask_NotifyCallbacks(RemoteProcState new_state);
static void RemoteProcTask_HandleEvent(RemoteProcEvent event, uint32_t param);
static void RemoteProcTask_HandleCoproCmd(RemoteProcCpuCmdFn_t cmd_fn, uint32_t cpu_id, int32_t expected_status);
static bool RemoteProcTask_CheckCoproCmdStatus(uint32_t cpu_id, int32_t expected_status);
static void RemoteProcTask_StartRecovery(uint32_t cpu_id, RemoteProcRecoveryType_t recovery_type);
static void RemoteProcTask_CrashHandler(uint32_t cpu_id);
static void RemoteProcTask_BeginStopSequence(void);
static void RemoteProcTask_EndStopSequence(void);

/**
  * @brief Return a printable string for a CPU status.
  * @param  status: CPU status value.
  * @retval Status string.
  */
static const char *RemoteProcTask_GetCpuStatusStr(int32_t status)
{
  if ((status >= 0) && (status < CPU_LAST))
  {
    return remoteProcCpuStatusStr[status];
  }

  return remoteProcCpuStatusStr[CPU_LAST];
}

/**
  * @brief Return a printable action string for an in-progress state.
  * @param  state: Current remote processor state.
  * @retval Action string.
  */
static const char *RemoteProcTask_GetStateAction(RemoteProcState state)
{
  switch (state)
  {
  case REMOTEPROC_STATE_STARTING:
    return "starting";
  case REMOTEPROC_STATE_STOPPING:
    return "stopping";
  case REMOTEPROC_STATE_SUSPENDING:
    return "suspending";
  case REMOTEPROC_STATE_RESUMING:
    return "resuming";
  default:
    return "processing";
  }
}

/**
  * @brief Mask SCMI notifications before launching a generic stop sequence.
  * @retval None
  */
static void RemoteProcTask_BeginStopSequence(void)
{
  if (!remoteProcScmiNotifMasked)
  {
    ScmiMgrTask_DisableNotifications();
    remoteProcScmiNotifMasked = true;
  }
}

/**
  * @brief Re-enable SCMI notifications once a generic stop sequence has completed.
  * @retval None
  */
static void RemoteProcTask_EndStopSequence(void)
{
  if (remoteProcScmiNotifMasked)
  {
    ScmiMgrTask_ResetAndEnableNotifications();
    remoteProcScmiNotifMasked = false;
  }
}

/**
  * @brief Launch a generic copro command and schedule async polling.
  * @param  cmd_fn: CPU command function.
  * @param  cpu_id: CPU ID.
  * @param  expected_status: Expected CPU status after completion.
  */
static void RemoteProcTask_HandleCoproCmd(RemoteProcCpuCmdFn_t cmd_fn, uint32_t cpu_id, int32_t expected_status)
{
  int err;

  if (cmd_fn == NULL)
  {
    (void)RemoteProcTask_PostEvent(REMOTEPROC_EVENT_ERROR, cpu_id);
    return;
  }

  err = tfm_platform_cpu_info(cpu_id, &remoteProcCoproInfo);
  if (err != TFM_PLATFORM_ERR_SUCCESS)
  {
    APP_LOG_ERR("RemoteProc", "Failed to get copro info for cpu%lu (err: %d)", cpu_id, err);
    (void)RemoteProcTask_PostEvent(REMOTEPROC_EVENT_ERROR, cpu_id);
    return;
  }

  if ((remoteProcCoproInfo.status < 0) || (remoteProcCoproInfo.status >= CPU_LAST))
  {
    APP_LOG_ERR("RemoteProc", "copro %s error %ld", remoteProcCoproInfo.name, remoteProcCoproInfo.status);
    (void)RemoteProcTask_PostEvent(REMOTEPROC_EVENT_ERROR, cpu_id);
    return;
  }

  if (remoteProcCoproInfo.status == expected_status)
  {
    APP_LOG_INF("RemoteProc", "copro %s already %s.",
      remoteProcCoproInfo.name, RemoteProcTask_GetCpuStatusStr(remoteProcCoproInfo.status));
    remoteProcCmdTick = osKernelGetTickCount();
    (void)RemoteProcTask_PostEvent(REMOTEPROC_EVENT_CHECK_STATUS, cpu_id);
    return;
  }

  err = cmd_fn(cpu_id, &remoteProcCoproInfo.status);
  if (err != TFM_PLATFORM_ERR_SUCCESS)
  {
    APP_LOG_ERR("RemoteProc", "Failed while %s copro %s (err: %d)",
      RemoteProcTask_GetStateAction(remoteProcCurrentState), remoteProcCoproInfo.name, err);
    (void)RemoteProcTask_PostEvent(REMOTEPROC_EVENT_ERROR, cpu_id);
    return;
  }

  APP_LOG_INF("RemoteProc", "%s copro %s... (pending)",
    RemoteProcTask_GetStateAction(remoteProcCurrentState), remoteProcCoproInfo.name);

  remoteProcCmdTick = osKernelGetTickCount();
  (void)RemoteProcTask_PostEvent(REMOTEPROC_EVENT_CHECK_STATUS, cpu_id);
}

/**
  * @brief Poll the current CPU status and reschedule async checking if needed.
  * @param  cpu_id: CPU ID to poll.
  * @param  expected_status: Expected CPU status after completion.
  * @retval true if the expected status is reached, false otherwise.
  */
static bool RemoteProcTask_CheckCoproCmdStatus(uint32_t cpu_id, int32_t expected_status)
{
  int err;

  err = tfm_platform_cpu_info(cpu_id, &remoteProcCoproInfo);
  if (err != TFM_PLATFORM_ERR_SUCCESS)
  {
    APP_LOG_ERR("RemoteProc", "Poll: failed to get copro info for cpu%lu (err: %d)", cpu_id, err);
    (void)RemoteProcTask_PostEvent(REMOTEPROC_EVENT_ERROR, cpu_id);
    return false;
  }

  if (remoteProcCoproInfo.status == expected_status)
  {
    return true;
  }

  if ((osKernelGetTickCount() - remoteProcCmdTick) > REMOTEPROC_CMD_TIMEOUT_MS)
  {
    APP_LOG_ERR("RemoteProc", "Timeout while %s copro %s. Last status: %s",
      RemoteProcTask_GetStateAction(remoteProcCurrentState),
      remoteProcCoproInfo.name,
      RemoteProcTask_GetCpuStatusStr(remoteProcCoproInfo.status));
    (void)RemoteProcTask_PostEvent(REMOTEPROC_EVENT_ERROR, cpu_id);
    return false;
  }

  osDelay(REMOTEPROC_CMD_CHECK_INTERVAL_MS);
  (void)RemoteProcTask_PostEvent(REMOTEPROC_EVENT_CHECK_STATUS, cpu_id);
  return false;
}

/**
  * @brief Start a stop/start recovery sequence.
  * @param  cpu_id: CPU ID to recover.
  * @param  recovery_type: Recovery reason.
  * @retval None
  */
static void RemoteProcTask_StartRecovery(uint32_t cpu_id, RemoteProcRecoveryType_t recovery_type)
{
  remoteProcRecoveryType = recovery_type;
  remoteProcRecoveryCpuId = cpu_id;
  RemoteProcTask_BeginStopSequence();
  remoteProcCurrentState = REMOTEPROC_STATE_STOPPING;
  RemoteProcTask_NotifyCallbacks(remoteProcCurrentState);
  RemoteProcTask_HandleCoproCmd(tfm_platform_cpu_stop, cpu_id, CPU_OFFLINE);
}

/**
  * @brief Register a callback for remote processor state changes.
  * @param  cb: Callback function pointer.
  * @param  context: User context pointer.
  * @retval HAL_OK if registered, HAL_ERROR otherwise.
  */
HAL_StatusTypeDef RemoteProcTask_RegisterCallback(RemoteProcStateCallback cb, void *context)
{
  for (int i = 0; i < REMOTEPROC_MAX_CALLBACKS; ++i)
  {
    if (remoteProcCallbackList[i].cb == NULL)
    {
      remoteProcCallbackList[i].cb = cb;
      remoteProcCallbackList[i].context = context;
      return HAL_OK;
    }
  }
  return HAL_ERROR;
}

/**
  * @brief Unregister a previously registered callback.
  * @param  cb: Callback function pointer.
  * @retval HAL_OK if unregistered, HAL_ERROR otherwise.
  */
HAL_StatusTypeDef RemoteProcTask_UnregisterCallback(RemoteProcStateCallback cb)
{
  for (int i = 0; i < REMOTEPROC_MAX_CALLBACKS; ++i)
  {
    if (remoteProcCallbackList[i].cb == cb)
    {
      remoteProcCallbackList[i].cb = NULL;
      remoteProcCallbackList[i].context = NULL;
      return HAL_OK;
    }
  }
  return HAL_ERROR;
}

/**
  * @brief Notify all registered callbacks of a state change.
  * @param  new_state: The new state to notify.
  */
static void RemoteProcTask_NotifyCallbacks(RemoteProcState new_state)
{
  for (int i = 0; i < REMOTEPROC_MAX_CALLBACKS; ++i)
  {
    if (remoteProcCallbackList[i].cb)
    {
      remoteProcCallbackList[i].cb(new_state, remoteProcCallbackList[i].context);
    }
  }
}

/**
  * @brief Post an event to the remote processor event queue.
  * @param  event: Event type.
  * @param  param: Event parameter.
  * @retval HAL_OK if the event was posted, HAL_ERROR otherwise.
  */
HAL_StatusTypeDef RemoteProcTask_PostEvent(RemoteProcEvent event, uint32_t param)
{
  if (!remoteProcEventQueueHandle)
    return HAL_ERROR;
  RemoteProcEventMsg msg = { .event = event, .param = param };
  if (osMessageQueuePut(remoteProcEventQueueHandle, &msg, 0, 0) == osOK)
    return HAL_OK;
  return HAL_ERROR;
}

/**
  * @brief Get the current state of the remote processor.
  * @retval The current remote processor state.
  */
RemoteProcState RemoteProcTask_GetState(void)
{
  return remoteProcCurrentState;
}

/**
  * @brief Crash handler callback for the remote processor task.
  * @param  cpu_id: CPU ID that crashed.
  */
static void RemoteProcTask_CrashHandler(uint32_t cpu_id)
{
  RemoteProcTask_PostEvent(REMOTEPROC_EVENT_CRASH, cpu_id);
}

/**
  * @brief Initialize the remote processor task and its resources.
  *        Creates the event queue and the task thread, and initializes the driver.
  * @retval None
  */
void RemoteProcTask_Init(void)
{
  memset(remoteProcCallbackList, 0, sizeof(remoteProcCallbackList));
  remoteProcScmiNotifMasked = false;
  remoteProcEventQueueHandle = osMessageQueueNew(8, sizeof(RemoteProcEventMsg), &remoteProcEventQueueAttr);
  if (!remoteProcEventQueueHandle)
    NSAppCore_ErrorHandler();

  remoteProcTaskHandle = osThreadNew(RemoteProcTask, NULL, &remoteProcTaskAttr);
  if (!remoteProcTaskHandle)
    NSAppCore_ErrorHandler();

  remoteproc_driver.init();
  remoteproc_driver.register_crash_callback(RemoteProcTask_CrashHandler);

  remoteProcCurrentState = REMOTEPROC_STATE_OFFLINE;
  RemoteProcTask_NotifyCallbacks(remoteProcCurrentState);

  /* Auto-start the Cortex-A35. */
#if REMOTE_PROC_AUTO_START
  RemoteProcTask_PostEvent(REMOTEPROC_EVENT_START, 0);
#endif /* REMOTE_PROC_AUTO_START */
}

/**
  * @brief De-initialize the remote processor task and its resources.
  *        Deletes the event queue and terminates the task thread.
  * @retval None
  */
void RemoteProcTask_DeInit(void)
{
  remoteproc_driver.unregister_crash_callback(RemoteProcTask_CrashHandler);
  remoteproc_driver.deinit();
  RemoteProcTask_EndStopSequence();

  if (remoteProcEventQueueHandle)
  {
    osMessageQueueDelete(remoteProcEventQueueHandle);
    remoteProcEventQueueHandle = NULL;
  }
  if (remoteProcTaskHandle)
  {
    osThreadTerminate(remoteProcTaskHandle);
    remoteProcTaskHandle = NULL;
  }
  remoteProcScmiNotifMasked = false;
  remoteProcCurrentState = REMOTEPROC_STATE_UNKNOWN;
}

/**
  * @brief Task function to handle remote processor events.
  * @param  argument: Not used.
  * @retval None
  */
static void RemoteProcTask(void *argument)
{
  (void)argument;
  RemoteProcEventMsg msg;
  while (1)
  {
    if (osMessageQueueGet(remoteProcEventQueueHandle, &msg, NULL, osWaitForever) == osOK)
    {
      RemoteProcTask_HandleEvent(msg.event, msg.param);
    }
  }
}

/**
  * @brief Handle a remote processor event based on the current state.
  * @param  event: Event type.
  * @param  param: Event parameter.
  */
static void RemoteProcTask_HandleEvent(RemoteProcEvent event, uint32_t param)
{
  switch (remoteProcCurrentState)
  {
  case REMOTEPROC_STATE_OFFLINE:
    if (event == REMOTEPROC_EVENT_START)
    {
      remoteProcCurrentState = REMOTEPROC_STATE_STARTING;
      RemoteProcTask_HandleCoproCmd(tfm_platform_cpu_start, param, CPU_RUNNING);
    }
    break;
  case REMOTEPROC_STATE_STARTING:
    if (event == REMOTEPROC_EVENT_CHECK_STATUS)
    {
      if (RemoteProcTask_CheckCoproCmdStatus(param, CPU_RUNNING))
      {
        APP_LOG_INF("RemoteProc", "copro %s started.", remoteProcCoproInfo.name);
        remoteProcCurrentState = REMOTEPROC_STATE_RUNNING;
        RemoteProcTask_NotifyCallbacks(remoteProcCurrentState);
      }
    }
    else if (event == REMOTEPROC_EVENT_ERROR)
    {
      remoteProcCurrentState = REMOTEPROC_STATE_OFFLINE;
      RemoteProcTask_NotifyCallbacks(remoteProcCurrentState);
    }
    break;
  case REMOTEPROC_STATE_RUNNING:
    if (event == REMOTEPROC_EVENT_STOP)
    {
      RemoteProcTask_BeginStopSequence();
      remoteProcCurrentState = REMOTEPROC_STATE_STOPPING;
      RemoteProcTask_HandleCoproCmd(tfm_platform_cpu_stop, param, CPU_OFFLINE);
    }
    else if (event == REMOTEPROC_EVENT_CRASH)
    {
      APP_LOG_INF("RemoteProc", "Crash detected on cpu%lu. Attempting recovery...", param);
      RemoteProcTask_StartRecovery(param, REMOTEPROC_RECOVERY_CRASH);
    }
    else if (event == REMOTEPROC_EVENT_SUSPEND)
    {
      remoteProcCurrentState = REMOTEPROC_STATE_SUSPENDING;
      RemoteProcTask_HandleCoproCmd(tfm_platform_cpu_suspend, param, CPU_SUSPENDED);
    }
    break;
  case REMOTEPROC_STATE_SUSPENDING:
    if (event == REMOTEPROC_EVENT_CHECK_STATUS)
    {
      if (RemoteProcTask_CheckCoproCmdStatus(param, CPU_SUSPENDED))
      {
        APP_LOG_INF("RemoteProc", "copro %s suspended.", remoteProcCoproInfo.name);
        remoteProcCurrentState = REMOTEPROC_STATE_SUSPENDED;
        RemoteProcTask_NotifyCallbacks(remoteProcCurrentState);
      }
    }
    else if (event == REMOTEPROC_EVENT_ERROR)
    {
      remoteProcCurrentState = REMOTEPROC_STATE_RUNNING;
      RemoteProcTask_NotifyCallbacks(remoteProcCurrentState);
    }
    break;
  case REMOTEPROC_STATE_SUSPENDED:
    if (event == REMOTEPROC_EVENT_RESUME)
    {
      remoteProcCurrentState = REMOTEPROC_STATE_RESUMING;
      RemoteProcTask_HandleCoproCmd(tfm_platform_cpu_resume, param, CPU_RUNNING);
    }
    else if (event == REMOTEPROC_EVENT_STOP)
    {
      RemoteProcTask_BeginStopSequence();
      remoteProcCurrentState = REMOTEPROC_STATE_STOPPING;
      RemoteProcTask_HandleCoproCmd(tfm_platform_cpu_stop, param, CPU_OFFLINE);
    }
    break;
  case REMOTEPROC_STATE_RESUMING:
    if (event == REMOTEPROC_EVENT_CHECK_STATUS)
    {
      if (RemoteProcTask_CheckCoproCmdStatus(param, CPU_RUNNING))
      {
        APP_LOG_INF("RemoteProc", "copro %s resumed.", remoteProcCoproInfo.name);
        remoteProcCurrentState = REMOTEPROC_STATE_RUNNING;
        RemoteProcTask_NotifyCallbacks(remoteProcCurrentState);
      }
    }
    else if (event == REMOTEPROC_EVENT_ERROR)
    {
      APP_LOG_ERR("RemoteProc", "Resume failed for cpu%lu. Attempting stop/start recovery...", param);
      RemoteProcTask_StartRecovery(param, REMOTEPROC_RECOVERY_RESUME);
    }
    break;
  case REMOTEPROC_STATE_STOPPING:
    if (event == REMOTEPROC_EVENT_CHECK_STATUS)
    {
      if (RemoteProcTask_CheckCoproCmdStatus(param, CPU_OFFLINE))
      {
        uint32_t recovery_cpu_id;
        RemoteProcRecoveryType_t recovery_type;

        APP_LOG_INF("RemoteProc", "copro %s stopped.", remoteProcCoproInfo.name);
        RemoteProcTask_EndStopSequence();

        if (remoteProcRecoveryType == REMOTEPROC_RECOVERY_NONE)
        {
          remoteProcCurrentState = REMOTEPROC_STATE_OFFLINE;
          RemoteProcTask_NotifyCallbacks(remoteProcCurrentState);
        }
        else
        {
          recovery_type = remoteProcRecoveryType;
          recovery_cpu_id = remoteProcRecoveryCpuId;
          remoteProcRecoveryType = REMOTEPROC_RECOVERY_NONE;

          if (recovery_type == REMOTEPROC_RECOVERY_CRASH)
          {
            APP_LOG_INF("RemoteProc", "Crash recovery: restarting cpu%lu", recovery_cpu_id);
          }
          else if (recovery_type == REMOTEPROC_RECOVERY_RESUME)
          {
            APP_LOG_INF("RemoteProc", "Resume recovery: restarting cpu%lu", recovery_cpu_id);
          }

          remoteProcCurrentState = REMOTEPROC_STATE_STARTING;
          RemoteProcTask_HandleCoproCmd(tfm_platform_cpu_start, recovery_cpu_id, CPU_RUNNING);
        }
      }
    }
    else if (event == REMOTEPROC_EVENT_ERROR)
    {
      remoteProcCurrentState = REMOTEPROC_STATE_OFFLINE;
      remoteProcRecoveryType = REMOTEPROC_RECOVERY_NONE;
      RemoteProcTask_EndStopSequence();

      APP_LOG_ERR("RemoteProc", "Stop failed for cpu%lu. Forcing offline state. Last status: %s", param,
        RemoteProcTask_GetCpuStatusStr(remoteProcCoproInfo.status));
      RemoteProcTask_NotifyCallbacks(remoteProcCurrentState);
    }
    break;
  default:
    break;
  }
}

/**
  * @brief Get a copy of the remote processor information structure.
  * @retval A copy of the cpu_info_res structure.
  */
struct cpu_info_res RemoteProcTask_GetCoproInfo(void)
{
  return remoteProcCoproInfo;
}

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

