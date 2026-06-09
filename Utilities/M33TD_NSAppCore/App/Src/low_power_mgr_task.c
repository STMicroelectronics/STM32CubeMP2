/**
  ******************************************************************************
  * @file    low_power_mgr_task.c
  * @author  MCD Application Team
  * @brief   Low Power Manager Task implementation file.
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
  * @addtogroup LowPowerMgrTask
  * @{
  */

/* Includes ------------------------------------------------------------------*/
#include "low_power_mgr_task.h"
#include "low_power_mgr_driver.h"
#include "remoteproc_task.h"

#include <string.h>
#include <uapi/tfm_pm_api.h>

/* Private defines -----------------------------------------------------------*/
#define LOW_POWER_MGR_QUEUE_SIZE               4U
#ifndef LOW_POWER_MGR_MAX_LISTENERS
#define LOW_POWER_MGR_MAX_LISTENERS            8U
#endif
#ifndef LOW_POWER_MGR_MAX_AGENTS
#define LOW_POWER_MGR_MAX_AGENTS               1U
#endif
#define LOW_POWER_MGR_TIMEOUT_MS               1000U
#define LOW_POWER_MGR_CHECK_INTERVAL_MS        10U
#define LOW_POWER_MGR_RUN2_SYNC_INTERVAL_MS    100U
#define LOW_POWER_MGR_MSEC_PER_SEC             1000U
#define LOW_POWER_MGR_A35_CPU_ID               0U
#define LOW_POWER_MGR_EXTI1_C2IMR1_GPIO        0x0000FFFFU
#define LOW_POWER_MGR_EXTI1_C2IMR1_PVD         (1UL << 16)
#define LOW_POWER_MGR_EXTI1_C2IMR1_PVM         (1UL << 17)
#define LOW_POWER_MGR_EXTI1_C2IMR2_WKUP_MASK   0x03F00000U
#define LOW_POWER_MGR_EXTI1_C2IMR3_C1SEV       (1UL << 1)
#define LOW_POWER_MGR_SYSTICK_ENABLE_MASK      SysTick_CTRL_ENABLE_Msk
#define LOW_POWER_MGR_MODE_INVALID            ((LowPowerMgrMode_t)-1)

/* Private message kinds -----------------------------------------------------*/
typedef enum {
    LOW_POWER_MGR_MSG_EVENT = 0,
    LOW_POWER_MGR_MSG_AGENT_SET,
    LOW_POWER_MGR_MSG_AGENT_CLEAR,
    LOW_POWER_MGR_MSG_AGENT_UNREGISTER,
    LOW_POWER_MGR_MSG_RUN2_SYNC_CHECK,
} LowPowerMgrMsgType_t;

/* Private typedef -----------------------------------------------------------*/
/**
  * @brief  Low Power Manager state machine states.
  */
typedef enum {
    LOW_POWER_MGR_STATE_IDLE = 0,
    LOW_POWER_MGR_STATE_PREPARING,
    LOW_POWER_MGR_STATE_ENTERING_SLEEP,
    LOW_POWER_MGR_STATE_RESUMING,
} LowPowerMgrState_t;

/**
  * @brief  Low Power Manager queue message.
  */
typedef struct {
    LowPowerMgrMsgType_t type;
    LowPowerMgrEvent_t event;
    uint32_t param;
    LowPowerMgrAgentHandle_t agent;
    LowPowerMgrMode_t mode;
} LowPowerMgrEventMsg_t;

/**
  * @brief  Saved non-secure context for STANDBY1 entry and exit.
  */
typedef struct {
    uint32_t VTOR_NS;
    uint32_t MSPLIM_NS;
    uint32_t PSPLIM_NS;
    uint32_t CONTROL_NS;
    uint32_t MSP_NS;
    uint32_t PSP_NS;
    uint32_t primask_ns;
} LowPowerMgrSavedContext_t;

/**
  * @brief  Saved tick state for manual suspend/resume around deep low power entry.
  */
typedef struct {
  bool systick_was_enabled;
} LowPowerMgrTickContext_t;

/**
  * @brief  Low Power Manager runtime state.
  */
typedef struct {
    LowPowerMgrState_t state;
    LowPowerMgrMode_t allowed_mode;
    LowPowerMgrMode_t active_mode;
    volatile bool copro_is_suspended;
    volatile bool copro_is_running;
    bool run2_sync_pending;
    bool shutdown_requested;
    bool standby1_exit;
    uint32_t last_error;
} LowPowerMgrTaskState_t;

/**
  * @brief  Low Power Manager listener entry.
  */
typedef struct {
  LowPowerMgrNotifListener cb;
    void *context;
} LowPowerMgrListenerEntry_t;

/**
  * @brief  Low Power Manager agent entry.
  */
typedef struct {
  bool active;
  LowPowerMgrMode_t mode;
  const char *label;
} LowPowerMgrAgent_t;

/* Private variables ---------------------------------------------------------*/
static osMessageQueueId_t lowPowerMgrQueueHandle;
static osThreadId_t lowPowerMgrTaskHandle;

static LowPowerMgrListenerEntry_t lowPowerMgrListenerList[LOW_POWER_MGR_MAX_LISTENERS];
static LowPowerMgrAgent_t lowPowerMgrAgents[LOW_POWER_MGR_MAX_AGENTS];
static LowPowerMgrSavedContext_t lowPowerMgrSavedContext;
static LowPowerMgrTickContext_t lowPowerMgrTickContext;

static LowPowerMgrTaskState_t lowPowerMgrState = {
    .state = LOW_POWER_MGR_STATE_IDLE,
    .allowed_mode = LOW_POWER_MGR_MODE_NONE,
    .active_mode = LOW_POWER_MGR_MODE_NONE,
    .copro_is_suspended = false,
    .copro_is_running = false,
    .run2_sync_pending = false,
    .shutdown_requested = false,
    .standby1_exit = false,
    .last_error = 0U,
};

static const osMessageQueueAttr_t lowPowerMgrQueueAttr = {
    .name = "LowPowerMgrQueue"
};

static const osThreadAttr_t lowPowerMgrTaskAttr = {
    .name = "LowPowerMgrTask",
    .priority = (osPriority_t)LOW_POWER_MGR_TASK_PRIORITY,
    .stack_size = LOW_POWER_MGR_TASK_STACK_SIZE,
};

/* Forward declarations ------------------------------------------------------*/
static void LowPowerMgrTask(void *argument);
static void LowPowerMgrTask_HandlePrepareSleep(void);
static void LowPowerMgrTask_HandleEnterSleep(void);
static void LowPowerMgrTask_HandleResume(void);
static void LowPowerMgrTask_NotifyListeners(LowPowerMgrNotif_t notif, LowPowerMgrMode_t mode);
static void LowPowerMgrTask_RemoteProcCallback(RemoteProcState new_state, void *context);
static void LowPowerMgrTask_RecomputeAllowedMode(void);
static void LowPowerMgrTask_ResetSuspendPolicy(void);
static void LowPowerMgrTask_HandleRun2SyncCheck(void);
static const char *LowPowerMgrTask_GetModeStr(LowPowerMgrMode_t mode);
static const char *LowPowerMgrTask_GetAgentLabel(uint32_t agent_index);
static LowPowerMgrMode_t LowPowerMgrTask_GetSuspendMode(LowPowerMgrMode_t allowed_mode);
static int32_t LowPowerMgrTask_WaitD1State(uint32_t timeout_ms, uint32_t state);
static bool LowPowerMgrTask_WaitForCoproSuspend(void);
static bool LowPowerMgrTask_WaitForCoproResume(void);
static void LowPowerMgrTask_SaveContext(void);
static void LowPowerMgrTask_RestoreContext(void);
static void LowPowerMgrTask_SuspendRtosTick(void);
static void LowPowerMgrTask_ResumeRtosTick(void);
static HAL_StatusTypeDef LowPowerMgrTask_PostAgentMessage(LowPowerMgrMsgType_t type,
                                                          LowPowerMgrAgentHandle_t agent,
                                                          LowPowerMgrMode_t mode);
static HAL_StatusTypeDef LowPowerMgrTask_PostRun2SyncCheck(void);

extern void vPortSetupTimerInterrupt(void);

/* Compile-time default policy ------------------------------------------------*/
#if LOW_POWER_DEFAULT_POLICY_ENABLE
#define LOW_POWER_MGR_DEFAULT_ALLOWED_MODE LOW_POWER_MGR_MODE_LPLV_STOP2
#else
#define LOW_POWER_MGR_DEFAULT_ALLOWED_MODE LOW_POWER_MGR_MODE_NONE
#endif

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Reset suspend policy back to the configured default state.
  * @retval None
  */
static void LowPowerMgrTask_ResetSuspendPolicy(void)
{
  (void)RemoteProcTask_UnregisterCallback(LowPowerMgrTask_RemoteProcCallback);
  LowPowerMgrTask_RecomputeAllowedMode();
  lowPowerMgrState.active_mode = LOW_POWER_MGR_MODE_NONE;
  lowPowerMgrState.copro_is_suspended = false;
  lowPowerMgrState.copro_is_running = false;
  lowPowerMgrState.run2_sync_pending = false;
  lowPowerMgrState.shutdown_requested = false;
}

/**
  * @brief  Monitor A35 D1 standby exit while RUN2 remains valid on the M33 side.
  * @retval None
  */
static void LowPowerMgrTask_HandleRun2SyncCheck(void)
{
  RemoteProcState remoteproc_state;

  if (!lowPowerMgrState.run2_sync_pending)
  {
    return;
  }

  if (HAL_PWR_D1State() == PWR_D1_DSTANDBY)
  {
    osDelay(LOW_POWER_MGR_RUN2_SYNC_INTERVAL_MS);
    (void)LowPowerMgrTask_PostRun2SyncCheck();
    return;
  }

  remoteproc_state = RemoteProcTask_GetState();
  if (remoteproc_state == REMOTEPROC_STATE_SUSPENDED)
  {
    APP_LOG_INF("LowPowerMgr", "A35 exited D1 standby, resynchronizing copro state");
    (void)RemoteProcTask_PostEvent(REMOTEPROC_EVENT_RESUME, LOW_POWER_MGR_A35_CPU_ID);
    lowPowerMgrState.run2_sync_pending = false;
  }
  else if ((remoteproc_state == REMOTEPROC_STATE_RUNNING) ||
           (remoteproc_state == REMOTEPROC_STATE_RESUMING) ||
           (remoteproc_state == REMOTEPROC_STATE_OFFLINE))
  {
    lowPowerMgrState.run2_sync_pending = false;
  }
  else
  {
    osDelay(LOW_POWER_MGR_RUN2_SYNC_INTERVAL_MS);
    (void)LowPowerMgrTask_PostRun2SyncCheck();
  }
}

/**
  * @brief  Return a printable string for a low power mode.
  * @param  mode: Low power mode.
  * @retval Mode string.
  */
static const char *LowPowerMgrTask_GetModeStr(LowPowerMgrMode_t mode)
{
  switch (mode)
  {
  case LOW_POWER_MGR_MODE_STOP2:
    return "STOP2";
  case LOW_POWER_MGR_MODE_LP_STOP2:
    return "LP_STOP2";
  case LOW_POWER_MGR_MODE_LPLV_STOP2:
    return "LPLV_STOP2";
  case LOW_POWER_MGR_MODE_STANDBY1:
    return "STANDBY1";
  case LOW_POWER_MGR_MODE_STANDBY2:
    return "STANDBY2";
  case LOW_POWER_MGR_MODE_NONE:
  default:
    return "NONE";
  }
}

/**
  * @brief  Return a printable label for a registered agent.
  * @param  agent_index: Zero-based agent slot index.
  * @retval Agent label if available, otherwise a fallback string.
  */
static const char *LowPowerMgrTask_GetAgentLabel(uint32_t agent_index)
{
  const char *label = lowPowerMgrAgents[agent_index].label;

  return ((label != NULL) && (label[0] != '\0')) ? label : "anonymous";
}

/**
  * @brief  Compute the deepest low power mode allowed by platform wakeup constraints.
  * @param  allowed_mode: Policy limit requested by the system.
  * @retval Effective low power mode to enter.
  */
static LowPowerMgrMode_t LowPowerMgrTask_GetSuspendMode(LowPowerMgrMode_t allowed_mode)
{
  uint32_t cpu1cr = PWR->CPU1CR;
  uint32_t c2imr1 = EXTI1->C2IMR1;
  uint32_t c2imr2 = EXTI1->C2IMR2;
  uint32_t c2imr3 = EXTI1->C2IMR3;
  LowPowerMgrMode_t suspend_mode = LOW_POWER_MGR_MODE_STANDBY1;

  if ((c2imr1 & (LOW_POWER_MGR_EXTI1_C2IMR1_GPIO |
                 LOW_POWER_MGR_EXTI1_C2IMR1_PVD |
                 LOW_POWER_MGR_EXTI1_C2IMR1_PVM)) != 0U)
  {
    suspend_mode = LOW_POWER_MGR_MODE_LPLV_STOP2;
  }

  if (((c2imr1 & ~(LOW_POWER_MGR_EXTI1_C2IMR1_GPIO |
                   LOW_POWER_MGR_EXTI1_C2IMR1_PVD |
                   LOW_POWER_MGR_EXTI1_C2IMR1_PVM)) != 0U) ||
      ((c2imr2 & ~LOW_POWER_MGR_EXTI1_C2IMR2_WKUP_MASK) != 0U) ||
      ((c2imr3 & ~LOW_POWER_MGR_EXTI1_C2IMR3_C1SEV) != 0U))
  {
    suspend_mode = LOW_POWER_MGR_MODE_LP_STOP2;
  }

  if ((cpu1cr & PWR_CPU1CR_PDDS_D2) == 0U)
  {
    suspend_mode = LOW_POWER_MGR_MODE_LP_STOP2;
  }

  if (suspend_mode > allowed_mode)
  {
    suspend_mode = allowed_mode;
  }

  return suspend_mode;
}

/**
  * @brief  Recompute the aggregate allowed low power mode from baseline and agent constraints.
  * @retval None
  */
static void LowPowerMgrTask_RecomputeAllowedMode(void)
{
  LowPowerMgrMode_t allowed_mode = LOW_POWER_MGR_DEFAULT_ALLOWED_MODE;
  uint32_t index;
  bool has_sleep_constraint = false;

  for (index = 0U; index < LOW_POWER_MGR_MAX_AGENTS; ++index)
  {
    if ((!lowPowerMgrAgents[index].active) ||
        (lowPowerMgrAgents[index].mode == LOW_POWER_MGR_MODE_INVALID))
    {
      continue;
    }

    if (lowPowerMgrAgents[index].mode == LOW_POWER_MGR_MODE_NONE)
    {
      allowed_mode = LOW_POWER_MGR_MODE_NONE;
      break;
    }

    if ((!has_sleep_constraint) ||
        (lowPowerMgrAgents[index].mode < allowed_mode))
    {
      allowed_mode = lowPowerMgrAgents[index].mode;
      has_sleep_constraint = true;
    }
  }

  lowPowerMgrState.allowed_mode = allowed_mode;
}

/**
  * @brief  Wait until D1 reaches the expected baseline state.
  * @param  timeout_ms: Timeout in milliseconds.
  * @param  state: Expected D1 state.
  * @retval 0 on success, negative value on timeout.
  */
static int32_t LowPowerMgrTask_WaitD1State(uint32_t timeout_ms, uint32_t state)
{
  uint32_t tick_timeout;
  uint32_t tick_start;
  uint32_t tick_curr = 0U;

  tick_timeout = (osKernelGetTickFreq() * timeout_ms) / LOW_POWER_MGR_MSEC_PER_SEC;
  tick_start = osKernelGetTickCount();

  do
  {
    osDelay(1U);

    if (HAL_PWR_D1State() == state)
    {
      return 0;
    }

    tick_curr = osKernelGetTickCount();
  } while ((tick_curr - tick_start) < tick_timeout);

  return -1;
}

/**
  * @brief  Wait for the remote processor suspend completion callback.
  * @retval true if the remote processor reported suspend, false on timeout.
  */
static bool LowPowerMgrTask_WaitForCoproSuspend(void)
{
  uint32_t start_tick = osKernelGetTickCount();

  while (!lowPowerMgrState.copro_is_suspended)
  {
    if ((osKernelGetTickCount() - start_tick) > LOW_POWER_MGR_TIMEOUT_MS)
    {
      return false;
    }

    osDelay(LOW_POWER_MGR_CHECK_INTERVAL_MS);
  }

  return true;
}

/**
  * @brief  Wait for the remote processor to return to RUNNING state.
  * @retval true if the remote processor resumed, false otherwise.
  */
static bool LowPowerMgrTask_WaitForCoproResume(void)
{
  uint32_t start_tick = osKernelGetTickCount();

  while (!lowPowerMgrState.copro_is_running)
  {
    if ((osKernelGetTickCount() - start_tick) > LOW_POWER_MGR_TIMEOUT_MS)
    {
      return false;
    }

    if (RemoteProcTask_GetState() == REMOTEPROC_STATE_OFFLINE)
    {
      return false;
    }

    osDelay(LOW_POWER_MGR_CHECK_INTERVAL_MS);
  }

  return true;
}

/**
  * @brief  Save non-secure CPU context for STANDBY1 mode.
  * @retval None
  */
static void LowPowerMgrTask_SaveContext(void)
{
  lowPowerMgrSavedContext.VTOR_NS = SCB->VTOR;
  lowPowerMgrSavedContext.primask_ns = __get_PRIMASK();
  lowPowerMgrSavedContext.MSPLIM_NS = __get_MSPLIM();
  lowPowerMgrSavedContext.PSPLIM_NS = __get_PSPLIM();
  lowPowerMgrSavedContext.CONTROL_NS = __get_CONTROL();
  lowPowerMgrSavedContext.MSP_NS = __get_MSP();
  lowPowerMgrSavedContext.PSP_NS = __get_PSP();
}

/**
  * @brief  Restore non-secure CPU context after STANDBY1 exit.
  * @retval None
  */
static void LowPowerMgrTask_RestoreContext(void)
{
  SCB->VTOR = lowPowerMgrSavedContext.VTOR_NS;
  __DSB();
  __ISB();
  __set_PRIMASK(lowPowerMgrSavedContext.primask_ns);
  __set_MSPLIM(lowPowerMgrSavedContext.MSPLIM_NS);
  __set_PSPLIM(lowPowerMgrSavedContext.PSPLIM_NS);
  __set_MSP(lowPowerMgrSavedContext.MSP_NS);
  __set_PSP(lowPowerMgrSavedContext.PSP_NS);
  __set_CONTROL(lowPowerMgrSavedContext.CONTROL_NS);
}

/**
   * @brief  Suspend RTOS SysTick before entering deep low power.
  * @retval None
  */
  static void LowPowerMgrTask_SuspendRtosTick(void)
{
  lowPowerMgrTickContext.systick_was_enabled =
    ((SysTick->CTRL & LOW_POWER_MGR_SYSTICK_ENABLE_MASK) != 0U);

  SysTick->CTRL = 0U;
}

/**
   * @brief  Restore RTOS SysTick after exiting deep low power.
  * @retval None
  */
  static void LowPowerMgrTask_ResumeRtosTick(void)
{
  if (lowPowerMgrTickContext.systick_was_enabled)
  {
    vPortSetupTimerInterrupt();
  }
}

/**
  * @brief  Notify all registered listeners of a low power event.
  * @param  notif: Notif event.
  * @param  mode: Associated low power mode.
  * @retval None
  */
static void LowPowerMgrTask_NotifyListeners(LowPowerMgrNotif_t notif, LowPowerMgrMode_t mode)
{
  for (int i = 0; i < LOW_POWER_MGR_MAX_LISTENERS; ++i)
  {
    if (lowPowerMgrListenerList[i].cb != NULL)
    {
      lowPowerMgrListenerList[i].cb(notif, mode, lowPowerMgrListenerList[i].context);
    }
  }
}

/**
  * @brief  RemoteProc callback used to track suspend and resume completion.
  * @param  new_state: New remote processor state.
  * @param  context: User context pointer.
  * @retval None
  */
static void LowPowerMgrTask_RemoteProcCallback(RemoteProcState new_state, void *context)
{
  (void)context;

  if (new_state == REMOTEPROC_STATE_SUSPENDED)
  {
    lowPowerMgrState.copro_is_suspended = true;
  }
  else if (new_state == REMOTEPROC_STATE_RUNNING)
  {
    lowPowerMgrState.copro_is_running = true;
  }
}

/**
  * @brief  Handle PREPARING state actions.
  * @retval None
  */
static void LowPowerMgrTask_HandlePrepareSleep(void)
{
  LowPowerMgrMode_t allowed_mode;

  lowPowerMgrState.copro_is_suspended = false;
  lowPowerMgrState.copro_is_running = false;
  lowPowerMgrState.last_error = 0U;
  lowPowerMgrState.active_mode = LOW_POWER_MGR_MODE_NONE;
  lowPowerMgrState.standby1_exit = false;
  RemoteProcState remoteproc_state;

  if (LowPowerMgrTask_WaitD1State(LOW_POWER_MGR_TIMEOUT_MS, PWR_D1_DSTANDBY) != 0)
  {
    APP_LOG_ERR("LowPowerMgr", "D1 DStandby timeout before suspend sequence");
    lowPowerMgrState.last_error = 1U;
    LowPowerMgrTask_ResetSuspendPolicy();
    lowPowerMgrState.state = LOW_POWER_MGR_STATE_IDLE;
    return;
  }

  (void)RemoteProcTask_RegisterCallback(LowPowerMgrTask_RemoteProcCallback, NULL);
  remoteproc_state = RemoteProcTask_GetState();
  if (remoteproc_state == REMOTEPROC_STATE_SUSPENDED)
  {
    APP_LOG_INF("LowPowerMgr", "Copro is already suspended");
  }
  else
  {
    (void)RemoteProcTask_PostEvent(REMOTEPROC_EVENT_SUSPEND, LOW_POWER_MGR_A35_CPU_ID);
    if (!LowPowerMgrTask_WaitForCoproSuspend())
    {
      APP_LOG_ERR("LowPowerMgr", "Timeout waiting for copro suspend");
      lowPowerMgrState.last_error = 2U;
      LowPowerMgrTask_ResetSuspendPolicy();
      lowPowerMgrState.state = LOW_POWER_MGR_STATE_IDLE;
      return;
    }
  }

  allowed_mode = lowPowerMgrState.allowed_mode;

  if (lowPowerMgrState.shutdown_requested)
  {
    lowPowerMgrState.active_mode = LOW_POWER_MGR_MODE_STANDBY2;
  }
  else
  {
    lowPowerMgrState.active_mode = LowPowerMgrTask_GetSuspendMode(allowed_mode);
  }

  if ((lowPowerMgrState.active_mode == LOW_POWER_MGR_MODE_STANDBY1) ||
      (lowPowerMgrState.active_mode == LOW_POWER_MGR_MODE_STANDBY2))
  {
    LowPowerMgrTask_NotifyListeners(LOW_POWER_MGR_NOTIF_PREPARE_STANDBY, lowPowerMgrState.active_mode);
  }
  else
  {
    LowPowerMgrTask_NotifyListeners(LOW_POWER_MGR_NOTIF_PREPARE_STOP, lowPowerMgrState.active_mode);
  }

  APP_LOG_INF("LowPowerMgr", "Prepared low power sequence with mode %s",
    LowPowerMgrTask_GetModeStr(lowPowerMgrState.active_mode));
  lowPowerMgrState.state = LOW_POWER_MGR_STATE_ENTERING_SLEEP;
  (void)LowPowerMgrTask_PostEvent(LOW_POWER_MGR_EVENT_ENTER_SLEEP, 0U);
}

/**
  * @brief  Handle ENTERING_SLEEP state actions.
  * @retval None
  */
static void LowPowerMgrTask_HandleEnterSleep(void)
{
  int32_t kernel_lock;
  uint32_t pwr_cpu2cr;
  uint32_t rcc_c1bootrsts;
  uint32_t rcc_c2bootrsts;
  LowPowerMgrSleepInfo_t sleep_info;
  psa_status_t err = PSA_SUCCESS;

  sleep_info.mode = lowPowerMgrState.active_mode;

  APP_LOG_INF("LowPowerMgr", "Entering low power mode %s", LowPowerMgrTask_GetModeStr(lowPowerMgrState.active_mode));

  kernel_lock = osKernelLock();
  if (kernel_lock == 0)
  {
    LowPowerMgrTask_SuspendRtosTick();

    if (lowPowerMgrDriver.prepareSleep != NULL)
    {
      (void)lowPowerMgrDriver.prepareSleep(&sleep_info);
    }

    if (lowPowerMgrState.active_mode == LOW_POWER_MGR_MODE_STANDBY1)
    {
      LowPowerMgrTask_SaveContext();
    }

    if (lowPowerMgrState.active_mode == LOW_POWER_MGR_MODE_STANDBY2)
    {
      err = psa_pm_power_off();
      APP_LOG_ERR("LowPowerMgr", "STANDBY2 power-off call returned unexpectedly (%ld)", (long)err);
      lowPowerMgrState.last_error = (err == PSA_SUCCESS) ? 1U : (uint32_t)err;
    }
    else
    {
      err = psa_pm_suspend((enum pm_suspend_mode_t)lowPowerMgrState.active_mode);

      if (err != PSA_SUCCESS)
      {
        lowPowerMgrState.last_error = (uint32_t)err;
        APP_LOG_ERR("LowPowerMgr", "PM suspend failed (%ld)", (long)err);
      }
    }

    if (lowPowerMgrState.active_mode == LOW_POWER_MGR_MODE_STANDBY1)
    {
      LowPowerMgrTask_RestoreContext();
    }

    LowPowerMgrTask_ResumeRtosTick();

    if (lowPowerMgrDriver.resumeFromSleep != NULL)
    {
      (void)lowPowerMgrDriver.resumeFromSleep(&sleep_info);
    }

  }

  (void)osKernelRestoreLock(kernel_lock);


  pwr_cpu2cr = READ_REG(PWR->CPU2CR);
  rcc_c1bootrsts = READ_REG(RCC->C1BOOTRSTSCLRR);
  rcc_c2bootrsts = READ_REG(RCC->C2BOOTRSTSCLRR);

  lowPowerMgrState.standby1_exit = false;

  if ((rcc_c2bootrsts & RCC_C2BOOTRSTSCLRR_D2STBYRSTF) != 0U)
  {
    lowPowerMgrState.standby1_exit = true;
    APP_LOG_INF("LowPowerMgr", "Standby exit");
  }
  else if ((pwr_cpu2cr & PWR_CPU2CR_STOPF) != 0U)
  {
    APP_LOG_INF("LowPowerMgr", "Stop exit");
  }
  else if ((rcc_c1bootrsts & RCC_C1BOOTRSTSCLRR_D1STBYRSTF) != 0U)
  {
    APP_LOG_INF("LowPowerMgr", "Run2/D1 DStandby exit");
  }
  else
  {
    if(lowPowerMgrState.shutdown_requested)
    {
      APP_LOG_INF("LowPowerMgr", "Shutdown aborted");
    }
    else
    {
      APP_LOG_INF("LowPowerMgr", "Suspend aborted");
    }
  }

  lowPowerMgrState.state = LOW_POWER_MGR_STATE_RESUMING;
  (void)LowPowerMgrTask_PostEvent(LOW_POWER_MGR_EVENT_RESUME, 0U);
}

/**
  * @brief  Handle RESUMING state actions.
  * @retval None
  */
static void LowPowerMgrTask_HandleResume(void)
{
  LowPowerMgrMode_t completed_mode = lowPowerMgrState.active_mode;

  lowPowerMgrState.copro_is_running = false;
  (void)RemoteProcTask_PostEvent(REMOTEPROC_EVENT_RESUME, LOW_POWER_MGR_A35_CPU_ID);
  if (!LowPowerMgrTask_WaitForCoproResume())
  {
    APP_LOG_ERR("LowPowerMgr", "Timeout waiting for copro resume");
    lowPowerMgrState.last_error = 1U;
  }

  if (completed_mode == LOW_POWER_MGR_MODE_STANDBY1 || completed_mode == LOW_POWER_MGR_MODE_STANDBY2)
  {
    LowPowerMgrTask_NotifyListeners(LOW_POWER_MGR_NOTIF_RESUME_STANDBY, completed_mode);
  }
  else
  {
    LowPowerMgrTask_NotifyListeners(LOW_POWER_MGR_NOTIF_RESUME_STOP, completed_mode);
  }

  LowPowerMgrTask_ResetSuspendPolicy();
  lowPowerMgrState.state = LOW_POWER_MGR_STATE_IDLE;
}

/* Exported functions --------------------------------------------------------*/
/**
  * @brief  Register a listener for low power notifications.
  * @param  cb: Listener callback.
  * @param  context: User context pointer.
  * @retval HAL_OK if registered, HAL_ERROR otherwise.
  */
HAL_StatusTypeDef LowPowerMgrTask_RegisterListener(LowPowerMgrNotifListener cb, void *context)
{
  for (int i = 0; i < LOW_POWER_MGR_MAX_LISTENERS; ++i)
  {
    if (lowPowerMgrListenerList[i].cb == NULL)
    {
      lowPowerMgrListenerList[i].cb = cb;
      lowPowerMgrListenerList[i].context = context;
      return HAL_OK;
    }
  }

  return HAL_ERROR;
}

/**
  * @brief  Unregister a low power listener.
  * @param  cb: Listener callback to remove.
  * @retval HAL_OK if unregistered, HAL_ERROR otherwise.
  */
HAL_StatusTypeDef LowPowerMgrTask_UnregisterListener(LowPowerMgrNotifListener cb)
{
  for (int i = 0; i < LOW_POWER_MGR_MAX_LISTENERS; ++i)
  {
    if (lowPowerMgrListenerList[i].cb == cb)
    {
      lowPowerMgrListenerList[i].cb = NULL;
      lowPowerMgrListenerList[i].context = NULL;
      return HAL_OK;
    }
  }

  return HAL_ERROR;
}

/**
  * @brief  Initialize the Low Power Manager Task and its resources.
  * @retval None
  */
void LowPowerMgrTask_Init(void)
{
  if ((lowPowerMgrDriver.init != NULL) && (lowPowerMgrDriver.init() != 0))
  {
    NSAppCore_ErrorHandler();
  }

  (void)memset(lowPowerMgrAgents, 0, sizeof(lowPowerMgrAgents));
  LowPowerMgrTask_ResetSuspendPolicy();
  lowPowerMgrState.state = LOW_POWER_MGR_STATE_IDLE;

  lowPowerMgrQueueHandle = osMessageQueueNew(LOW_POWER_MGR_QUEUE_SIZE,
    sizeof(LowPowerMgrEventMsg_t), &lowPowerMgrQueueAttr);
  if (lowPowerMgrQueueHandle == NULL)
  {
    NSAppCore_ErrorHandler();
  }

  lowPowerMgrTaskHandle = osThreadNew(LowPowerMgrTask, NULL, &lowPowerMgrTaskAttr);
  if (lowPowerMgrTaskHandle == NULL)
  {
    NSAppCore_ErrorHandler();
  }
}

/**
  * @brief  De-initialize the Low Power Manager Task and release its resources.
  * @retval None
  */
void LowPowerMgrTask_DeInit(void)
{
  if (lowPowerMgrTaskHandle != NULL)
  {
    (void)osThreadTerminate(lowPowerMgrTaskHandle);
    lowPowerMgrTaskHandle = NULL;
  }

  if (lowPowerMgrQueueHandle != NULL)
  {
    (void)osMessageQueueDelete(lowPowerMgrQueueHandle);
    lowPowerMgrQueueHandle = NULL;
  }

  (void)memset(lowPowerMgrAgents, 0, sizeof(lowPowerMgrAgents));
  LowPowerMgrTask_ResetSuspendPolicy();
  lowPowerMgrState.state = LOW_POWER_MGR_STATE_IDLE;

  if (lowPowerMgrDriver.deinit != NULL)
  {
    (void)lowPowerMgrDriver.deinit();
  }
}

/**
  * @brief  Register a runtime low power agent.
  * @param  label: Optional static label used for logs, may be NULL.
  * @retval Valid agent handle on success, LOW_POWER_MGR_AGENT_HANDLE_INVALID otherwise.
  */
LowPowerMgrAgentHandle_t LowPowerMgrTask_RegisterAgent(const char *label)
{
  uint32_t index;
  LowPowerMgrAgentHandle_t agent = LOW_POWER_MGR_AGENT_HANDLE_INVALID;

  for (index = 0U; index < LOW_POWER_MGR_MAX_AGENTS; ++index)
  {
    if (!lowPowerMgrAgents[index].active)
    {
      lowPowerMgrAgents[index].active = true;
      lowPowerMgrAgents[index].mode = LOW_POWER_MGR_MODE_INVALID;
      lowPowerMgrAgents[index].label = label;
      agent = (LowPowerMgrAgentHandle_t)(index + 1U);
      break;
    }
  }

  if (agent == LOW_POWER_MGR_AGENT_HANDLE_INVALID)
  {
    APP_LOG_ERR("LowPowerMgr", "No free low power agent slots available");
  }

  return agent;
}

/**
  * @brief  Unregister a runtime low power agent.
  * @param  agent: Agent handle returned by LowPowerMgrTask_RegisterAgent().
  * @retval HAL_OK if unregistered, HAL_ERROR otherwise.
  */
HAL_StatusTypeDef LowPowerMgrTask_UnregisterAgent(LowPowerMgrAgentHandle_t agent)
{
  uint32_t index;

  if ((agent == LOW_POWER_MGR_AGENT_HANDLE_INVALID) ||
      (agent > LOW_POWER_MGR_MAX_AGENTS))
  {
    return HAL_ERROR;
  }

  index = (uint32_t)agent - 1U;
  if (!lowPowerMgrAgents[index].active)
  {
    return HAL_ERROR;
  }

  lowPowerMgrAgents[index].active = false;
  lowPowerMgrAgents[index].mode = LOW_POWER_MGR_MODE_INVALID;
  lowPowerMgrAgents[index].label = NULL;

  return LowPowerMgrTask_PostAgentMessage(LOW_POWER_MGR_MSG_AGENT_UNREGISTER,
    LOW_POWER_MGR_AGENT_HANDLE_INVALID, LOW_POWER_MGR_MODE_INVALID);
}

/**
  * @brief  Create or update a low power constraint for an agent.
  * @param  agent: Agent handle returned by LowPowerMgrTask_RegisterAgent().
  * @param  mode: Requested constraint mode, including LOW_POWER_MGR_MODE_NONE.
  * @retval HAL_OK if updated, HAL_ERROR otherwise.
  */
HAL_StatusTypeDef LowPowerMgrTask_SetAgentConstraint(LowPowerMgrAgentHandle_t agent, LowPowerMgrMode_t mode)
{
  if ((mode > LOW_POWER_MGR_MODE_NONE) ||
      (mode == LOW_POWER_MGR_MODE_STANDBY2) ||
      (agent == LOW_POWER_MGR_AGENT_HANDLE_INVALID) ||
      (agent > LOW_POWER_MGR_MAX_AGENTS))
  {
    return HAL_ERROR;
  }

  return LowPowerMgrTask_PostAgentMessage(LOW_POWER_MGR_MSG_AGENT_SET, agent, mode);
}

/**
  * @brief  Clear the active low power constraint for an agent.
  * @param  agent: Agent handle returned by LowPowerMgrTask_RegisterAgent().
  * @retval HAL_OK if cleared, HAL_ERROR otherwise.
  */
HAL_StatusTypeDef LowPowerMgrTask_ClearAgentConstraint(LowPowerMgrAgentHandle_t agent)
{
  if ((agent == LOW_POWER_MGR_AGENT_HANDLE_INVALID) ||
      (agent > LOW_POWER_MGR_MAX_AGENTS))
  {
    return HAL_ERROR;
  }

  return LowPowerMgrTask_PostAgentMessage(LOW_POWER_MGR_MSG_AGENT_CLEAR,
    agent, LOW_POWER_MGR_MODE_INVALID);
}

/**
  * @brief  Post an event to the Low Power Manager Task queue.
  * @param  event: Event type.
  * @param  param: Event parameter.
  * @retval HAL_OK if posted, HAL_ERROR otherwise.
  */
HAL_StatusTypeDef LowPowerMgrTask_PostEvent(LowPowerMgrEvent_t event, uint32_t param)
{
  LowPowerMgrEventMsg_t msg = {
    .type = LOW_POWER_MGR_MSG_EVENT,
    .event = event,
    .param = param,
    .agent = LOW_POWER_MGR_AGENT_HANDLE_INVALID,
    .mode = LOW_POWER_MGR_MODE_INVALID,
  };

  if (lowPowerMgrQueueHandle == NULL)
  {
    return HAL_ERROR;
  }

  return (osMessageQueuePut(lowPowerMgrQueueHandle, &msg, 0U, 0U) == osOK) ? HAL_OK : HAL_ERROR;
}

/**
  * @brief  Post a private agent update message to the Low Power Manager Task queue.
  * @param  type: Agent message kind.
  * @param  agent: Target agent handle.
  * @param  mode: Constraint mode or LOW_POWER_MGR_MODE_INVALID for clear.
  * @retval HAL_OK if posted, HAL_ERROR otherwise.
  */
static HAL_StatusTypeDef LowPowerMgrTask_PostAgentMessage(LowPowerMgrMsgType_t type,
                                                          LowPowerMgrAgentHandle_t agent,
                                                          LowPowerMgrMode_t mode)
{
  LowPowerMgrEventMsg_t msg = {
    .type = type,
    .event = LOW_POWER_MGR_EVENT_SUSPEND,
    .param = 0U,
    .agent = agent,
    .mode = mode,
  };

  if (lowPowerMgrQueueHandle == NULL)
  {
    return HAL_ERROR;
  }

  return (osMessageQueuePut(lowPowerMgrQueueHandle, &msg, 0U, 0U) == osOK) ? HAL_OK : HAL_ERROR;
}

/**
  * @brief  Post a private RUN2 synchronization check message.
  * @retval HAL_OK if posted, HAL_ERROR otherwise.
  */
static HAL_StatusTypeDef LowPowerMgrTask_PostRun2SyncCheck(void)
{
  LowPowerMgrEventMsg_t msg = {
    .type = LOW_POWER_MGR_MSG_RUN2_SYNC_CHECK,
    .event = LOW_POWER_MGR_EVENT_SUSPEND,
    .param = 0U,
    .agent = LOW_POWER_MGR_AGENT_HANDLE_INVALID,
    .mode = LOW_POWER_MGR_MODE_INVALID,
  };

  if (lowPowerMgrQueueHandle == NULL)
  {
    return HAL_ERROR;
  }

  return (osMessageQueuePut(lowPowerMgrQueueHandle, &msg, 0U, 0U) == osOK) ? HAL_OK : HAL_ERROR;
}

/**
  * @brief  RPMsg reception callback for the Low Power Manager endpoint.
  * @param  ept: Local RPMsg endpoint.
  * @param  data: Pointer to received message buffer.
  * @param  len: Length of received message.
  * @param  src: Remote endpoint address.
  * @param  priv: User private context (unused).
  * @retval 0 on success, -1 on invalid command or queue error.
  */
int LowPowerMgrTask_RpmsgCallback(struct rpmsg_endpoint *ept, void *data,
                                  size_t len, uint32_t src, void *priv)
{
  LowPowerMgrMode_t mode;
  LowPowerMgrAgentHandle_t agent;
  const char *rpmsg_cmd = NULL;

  (void)ept;
  (void)len;
  (void)src;

  if ((data == NULL) || (priv == NULL))
  {
    return -1;
  }

  agent = *((LowPowerMgrAgentHandle_t *)priv);
  if (agent == LOW_POWER_MGR_AGENT_HANDLE_INVALID)
  {
    APP_LOG_ERR("LowPowerMgr", "Low power RPMsg agent is not registered");
    return -1;
  }

  if (strncmp((const char *)data, LOW_POWER_MGR_CMD_LIMIT_PM_DISABLED,
              sizeof(LOW_POWER_MGR_CMD_LIMIT_PM_DISABLED) - 1U) == 0)
  {
    mode = LOW_POWER_MGR_MODE_NONE;
    rpmsg_cmd = LOW_POWER_MGR_CMD_LIMIT_PM_DISABLED;
  }
  else if (strncmp((const char *)data, LOW_POWER_MGR_CMD_LIMIT_PM_STOP2,
              sizeof(LOW_POWER_MGR_CMD_LIMIT_PM_STOP2) - 1U) == 0)
  {
    mode = LOW_POWER_MGR_MODE_STOP2;
    rpmsg_cmd = LOW_POWER_MGR_CMD_LIMIT_PM_STOP2;
  }
  else if (strncmp((const char *)data, LOW_POWER_MGR_CMD_LIMIT_PM_LP_STOP2,
                   sizeof(LOW_POWER_MGR_CMD_LIMIT_PM_LP_STOP2) - 1U) == 0)
  {
    mode = LOW_POWER_MGR_MODE_LP_STOP2;
    rpmsg_cmd = LOW_POWER_MGR_CMD_LIMIT_PM_LP_STOP2;
  }
  else if (strncmp((const char *)data, LOW_POWER_MGR_CMD_LIMIT_PM_LPLV_STOP2,
                   sizeof(LOW_POWER_MGR_CMD_LIMIT_PM_LPLV_STOP2) - 1U) == 0)
  {
    mode = LOW_POWER_MGR_MODE_LPLV_STOP2;
    rpmsg_cmd = LOW_POWER_MGR_CMD_LIMIT_PM_LPLV_STOP2;
  }
  else if (strncmp((const char *)data, LOW_POWER_MGR_CMD_LIMIT_PM_STANDBY,
                   sizeof(LOW_POWER_MGR_CMD_LIMIT_PM_STANDBY) - 1U) == 0)
  {
    APP_LOG_WARN("LowPowerMgr", "RPMsg command LIMIT_PM_STANDBY ignored: STANDBY1 is not supported");
    return 0;
  }
  else
  {
    APP_LOG_WARN("LowPowerMgr", "Unknown RPMsg command");
    return 0;
  }

  if (LowPowerMgrTask_SetAgentConstraint(agent, mode) != HAL_OK)
  {
    APP_LOG_ERR("LowPowerMgr", "Failed to post RPMsg command: %s", rpmsg_cmd);
    return -1;
  }

  APP_LOG_INF("LowPowerMgr", "Low power limit command received: %s", rpmsg_cmd);
  return 0;
}

/**
  * @brief  Low Power Manager Task main loop.
  * @param  argument: Not used.
  * @retval None
  */
static void LowPowerMgrTask(void *argument)
{
  LowPowerMgrEventMsg_t cmd;
  uint32_t agent_index;

  (void)argument;

  for (;;)
  {
    if (osMessageQueueGet(lowPowerMgrQueueHandle, &cmd, NULL, osWaitForever) == osOK)
    {
      switch (lowPowerMgrState.state)
      {
      case LOW_POWER_MGR_STATE_IDLE:
        if (cmd.type == LOW_POWER_MGR_MSG_AGENT_SET)
        {
          agent_index = (uint32_t)cmd.agent - 1U;
          if (!lowPowerMgrAgents[agent_index].active)
          {
            APP_LOG_WARN("LowPowerMgr", "Dropping agent constraint update for inactive agent");
            break;
          }

          lowPowerMgrAgents[agent_index].mode = cmd.mode;
          LowPowerMgrTask_RecomputeAllowedMode();
          APP_LOG_INF("LowPowerMgr", "Constraint set to %s by agent %s",
            LowPowerMgrTask_GetModeStr(cmd.mode),
            LowPowerMgrTask_GetAgentLabel(agent_index));
        }
        else if (cmd.type == LOW_POWER_MGR_MSG_AGENT_CLEAR)
        {
          agent_index = (uint32_t)cmd.agent - 1U;
          if (!lowPowerMgrAgents[agent_index].active)
          {
            APP_LOG_WARN("LowPowerMgr", "Dropping agent constraint clear for inactive agent");
            break;
          }

          lowPowerMgrAgents[agent_index].mode = LOW_POWER_MGR_MODE_INVALID;
          LowPowerMgrTask_RecomputeAllowedMode();
          APP_LOG_INF("LowPowerMgr", "Constraint cleared by agent %s",
            LowPowerMgrTask_GetAgentLabel(agent_index));
        }
        else if (cmd.type == LOW_POWER_MGR_MSG_AGENT_UNREGISTER)
        {
          LowPowerMgrTask_RecomputeAllowedMode();
        }
        else if (cmd.type == LOW_POWER_MGR_MSG_RUN2_SYNC_CHECK)
        {
          LowPowerMgrTask_HandleRun2SyncCheck();
        }
        else if (cmd.event == LOW_POWER_MGR_EVENT_SHUTDOWN)
        {
          lowPowerMgrState.shutdown_requested = true;
          lowPowerMgrState.state = LOW_POWER_MGR_STATE_PREPARING;
          (void)LowPowerMgrTask_PostEvent(LOW_POWER_MGR_EVENT_PREPARE_SLEEP, 0U);
        }
        else if (cmd.event == LOW_POWER_MGR_EVENT_SUSPEND)
        {
          if (lowPowerMgrState.allowed_mode == LOW_POWER_MGR_MODE_NONE)
          {
            /**
              * @brief Handle the case where Linux already entered its suspend path
              *        while the M33-side low power policy rejects the request.
              * @note In this situation the M33 rejects the local low power request,
              *       but Linux may already have progressed into the D1 standby path.
              *       When D1 standby is observed within the expected timeout, a
              *       suspend event is forwarded to the remote processor.
              */
            APP_LOG_WARN("LowPowerMgr", "Suspend request rejected: low power disabled");

            if (LowPowerMgrTask_WaitD1State(LOW_POWER_MGR_TIMEOUT_MS, PWR_D1_DSTANDBY) == 0)
            {
              (void)RemoteProcTask_PostEvent(REMOTEPROC_EVENT_SUSPEND, LOW_POWER_MGR_A35_CPU_ID);
              if (!lowPowerMgrState.run2_sync_pending)
              {
                lowPowerMgrState.run2_sync_pending = true;
                (void)LowPowerMgrTask_PostRun2SyncCheck();
              }
              APP_LOG_INF("LowPowerMgr", "System entered into RUN2/D1_Standby1 mode");
            }

            osDelay(2);
          }
          else
          {
            lowPowerMgrState.state = LOW_POWER_MGR_STATE_PREPARING;
            (void)LowPowerMgrTask_PostEvent(LOW_POWER_MGR_EVENT_PREPARE_SLEEP, 0U);
          }
        }
        break;

      case LOW_POWER_MGR_STATE_PREPARING:
        if (cmd.event == LOW_POWER_MGR_EVENT_PREPARE_SLEEP)
        {
          LowPowerMgrTask_HandlePrepareSleep();
        }
        break;

      case LOW_POWER_MGR_STATE_ENTERING_SLEEP:
        if (cmd.event == LOW_POWER_MGR_EVENT_ENTER_SLEEP)
        {
          LowPowerMgrTask_HandleEnterSleep();
        }
        break;

      case LOW_POWER_MGR_STATE_RESUMING:
        if (cmd.event == LOW_POWER_MGR_EVENT_RESUME)
        {
          LowPowerMgrTask_HandleResume();
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
