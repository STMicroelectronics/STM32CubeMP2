/**
  ******************************************************************************
  * @file    scmi_mgr_task.c
  * @author  MCD Application Team
  * @brief   SCMI Manager task implementation file.
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

/** @addtogroup ScmiMgrTask
  * @{
  */

/* Includes ------------------------------------------------------------------*/
#include "scmi_mgr_task.h"
#include "remoteproc_task.h"
#if ENABLE_LOW_POWER_MGR_TASK
#include "low_power_mgr_task.h"
#endif
#include "tfm_ns_notif_api.h"
#include "psa_manifest/sid.h"
#include <dt-bindings/scmi/stm32mp2-agents.h>
#include "ns_evt.h"

/* Private define ------------------------------------------------------------*/

#define SCMI_MGR_TASK_MAX_LISTENERS 4

#define _PROT(A)  (((A) >> 10) & 0xff)
#define _TOKEN(A) (((A) >> 18) & 0x3ff)
#define _MSG_ID(A)((A) & 0xff)
#define SYS_POWER 0x12
#define _MSG(PROT, ID)  ((PROT) << 10 | (ID))
#define SYS_POWER 0x12
#define SYS_POWER_STATE_NOTIFIER (0x0)
#define SYS_POWER_STATE_NOTIFY (0x5)
#define ENABLE_NOTIFY (0x1)
#define GRACEFUL (0x1)
#define SCMI_MGR_NS_NOTIF_MASK (TFM_SP_IPCC_RSE_NS_EVT | TFM_SP_IPCC_SCMI_CA35_NS_EVT | TFM_SP_IPCC_SCMI_CA35_BL31_NS_EVT)
#define SCMI_MGR_NS_NOTIF_UNMASK 0U

/* Private types -------------------------------------------------------------*/
/**
  * @brief  SCMI Manager event listener entry.
  */
typedef struct {
  ScmiMgrTask_EventListener cb;
  void *context;
} ScmiMgrTask_ListenerEntry_t;

/* Private variables ---------------------------------------------------------*/

/**
  * @brief  List of registered SCMI Manager event listeners.
  */
static ScmiMgrTask_ListenerEntry_t listener_list[SCMI_MGR_TASK_MAX_LISTENERS] = {0};

static const char *agent_name[] = {
  [STM32MP25_AGENT_ID_M33_NS] = "M33_NS",
  [STM32MP25_AGENT_ID_CA35] = "CA35_NS",
  [STM32MP25_AGENT_ID_CA35_BL31] = "CA35_BL31"
};

/**
  * @brief Semaphore for SCMI notifications.
  */
static osSemaphoreId_t ScmiMgrSemaphoreHandle;
static const osSemaphoreAttr_t ScmiMgrSemaphoreAttr = {
  .name = "ScmiMgrSemaphore"
};

/**
  * @brief SCMI Manager task handle.
  */
static osThreadId_t ScmiMgrTaskHandle;
static const osThreadAttr_t ScmiMgrTaskAttr = {
  .name = "ScmiMgrTask",
  .priority = (osPriority_t)SCMI_MGR_TASK_PRIORITY,
  .stack_size = SCMI_MGR_TASK_STACK_SIZE
};

/* Private function prototypes -----------------------------------------------*/
static void ScmiMgrTask(void *argument);
static void ScmiMgrHandleEvent(uint32_t event);
static void ScmiMgrHandleScmiPowerNotif(void);
static void ScmiMgrTask_NotifyListeners(ScmiPowerEvent_t event);

/* Private function definitions ----------------------------------------------*/

/**
  * @brief  Notify all registered listeners of a SCMI power event.
  * @param  event: The SCMI power event.
  * @retval None
  */
static void ScmiMgrTask_NotifyListeners(ScmiPowerEvent_t event)
{
  for (int i = 0; i < SCMI_MGR_TASK_MAX_LISTENERS; ++i) {
    if (listener_list[i].cb) {
      listener_list[i].cb(event, listener_list[i].context);
    }
  }
}

/**
  * @brief  Callback for remoteproc state changes, used to sequence STOP/START.
  *         Posts a START event when the remoteproc reaches OFFLINE, then unregisters itself.
  * @param  state: The new state of the remote processor.
  * @param  context: Unused.
  * @retval None
  */
static void SCMIMgr_RemoteProcOfflineCallback(RemoteProcState state, void *context)
{
  if (state == REMOTEPROC_STATE_OFFLINE) {
    RemoteProcTask_PostEvent(REMOTEPROC_EVENT_START, 0);
    /* Unregister this callback */
    RemoteProcTask_UnregisterCallback(SCMIMgr_RemoteProcOfflineCallback);
  }
}

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Register a listener for SCMI power events.
  * @param  cb: Callback function to be called on event.
  * @param  context: User context pointer.
  * @retval HAL_OK if registered, HAL_ERROR if no slot available.
  */
HAL_StatusTypeDef ScmiMgrTask_RegisterListener(ScmiMgrTask_EventListener cb, void *context)
{
  for (int i = 0; i < SCMI_MGR_TASK_MAX_LISTENERS; ++i) {
    if (listener_list[i].cb == NULL) {
      listener_list[i].cb = cb;
      listener_list[i].context = context;
      return HAL_OK;
    }
  }
  return HAL_ERROR;
}

/**
  * @brief  Unregister a listener for SCMI power events.
  * @param  cb: Callback function to remove.
  * @retval HAL_OK if unregistered, HAL_ERROR if not found.
  */
HAL_StatusTypeDef ScmiMgrTask_UnregisterListener(ScmiMgrTask_EventListener cb)
{
  for (int i = 0; i < SCMI_MGR_TASK_MAX_LISTENERS; ++i) {
    if (listener_list[i].cb == cb) {
      listener_list[i].cb = NULL;
      listener_list[i].context = NULL;
      return HAL_OK;
    }
  }
  return HAL_ERROR;
}

/**
  * @brief  Initialize the SCMI Manager Task and its resources.
  *         Creates the semaphore and the task thread.
  * @retval None
  */
void ScmiMgrTask_Init(void)
{
    /* Create semaphore */
    ScmiMgrSemaphoreHandle = osSemaphoreNew(1, 0, &ScmiMgrSemaphoreAttr);
    if (ScmiMgrSemaphoreHandle == NULL) {
        NSAppCore_ErrorHandler();
    }
    /* Create the task */
    ScmiMgrTaskHandle = osThreadNew(ScmiMgrTask, NULL, &ScmiMgrTaskAttr);
    if (ScmiMgrTaskHandle == NULL) {
        NSAppCore_ErrorHandler();
    }

    /* Initialize interrupt and notification area */
    HAL_NVIC_SetPriority(RESERVED_9, NSAPPCORE_IRQ_PRIO_BASE + DEFAULT_IRQ_PRIO, 0);
    HAL_NVIC_EnableIRQ(RESERVED_9);

    /* Event area for notifications */
    static uint32_t ScmiMgrEventArea[16 + sizeof(struct ns_event_fifo)];
    int notif_err = tfm_ns_notif_init(ScmiMgrEventArea, sizeof(ScmiMgrEventArea));
    if (notif_err != 0) {
        NSAppCore_ErrorHandler();
    }

    /* Enable SCMI system power notifications for non-secure power events. */
    uint32_t in_buf[2];
    uint32_t out_buf[2];
    size_t out_sz, in_sz;
    int ret;

    in_buf[0] = _MSG(SYS_POWER, SYS_POWER_STATE_NOTIFY);
    in_buf[1] = ENABLE_NOTIFY;
    in_sz = sizeof(in_buf);
    out_sz = sizeof(out_buf);
    ret = tfm_scmi_req(in_buf, in_sz, out_buf, out_sz);
    if ((ret != PSA_SCMI_ERR_SUCCESS) || (out_buf[1])) {
        printf("\033[31m[NS] [ERR] [ScmiMgr] SCMI power notification not activated\033[0m\n");
        NSAppCore_ErrorHandler();
    }
}

/**
 * @brief  Handle SCMI system power state notifications.
 * @param  agent_id: SCMI agent identifier.
 * @param  graceful: Graceful flag.
 * @param  event: SCMI system power event.
 * @retval None
 */
static void ScmiMgr_HandleSysPowerStateNotif(uint32_t agent_id, bool graceful, enum scmi_sys_power event)
{
    const char *graceful_str = graceful ? "GRACEFUL" : "";
    const char *name = (agent_id < (sizeof(agent_name)/sizeof(agent_name[0])) && agent_name[agent_id]) ? agent_name[agent_id] : "UNKNOWN";
    ScmiPowerEvent_t notify_event = SCMI_POWER_EVENT_UNKNOWN;

    switch (event) {
    case SYS_POWER_SHUTDOWN:
        APP_LOG_INF("ScmiMgr", "agent %s %s SYS_POWER_SHUTDOWN", name, graceful_str);
#if ENABLE_LOW_POWER_MGR_TASK
      LowPowerMgrTask_PostEvent(LOW_POWER_MGR_EVENT_SHUTDOWN, 0U);
#endif
        notify_event = SCMI_POWER_EVENT_SHUTDOWN;
        break;
    case SYS_POWER_COLD_RESET:
        APP_LOG_INF("ScmiMgr", "agent %s %s SYS_POWER_COLD_RESET", name, graceful_str);
        tfm_platform_system_reset();
        notify_event = SCMI_POWER_EVENT_COLD_RESET;
        break;
    case SYS_POWER_WARM_RESET:
        APP_LOG_INF("ScmiMgr", "agent %s %s SYS_POWER_WARM_RESET", name, graceful_str);
        /* Register callback to be notified when remoteproc is OFFLINE */
        RemoteProcTask_RegisterCallback(SCMIMgr_RemoteProcOfflineCallback, NULL);
        RemoteProcTask_PostEvent(REMOTEPROC_EVENT_STOP, 0);
        notify_event = SCMI_POWER_EVENT_WARM_RESET;
        break;
    case SYS_POWER_SUSPEND:
        APP_LOG_INF("ScmiMgr", "agent %s %s SYS_POWER_SUSPEND", name, graceful_str);
  #if ENABLE_LOW_POWER_MGR_TASK
        LowPowerMgrTask_PostEvent(LOW_POWER_MGR_EVENT_SUSPEND, 0U);
  #endif
        notify_event = SCMI_POWER_EVENT_SUSPEND;
        break;
    default:
        APP_LOG_WARN("ScmiMgr", "Unsupported sys power notification %x", event);
        notify_event = SCMI_POWER_EVENT_UNKNOWN;
        break;
    }

    /* Notify all registered listeners */
    ScmiMgrTask_NotifyListeners(notify_event);
}

/**
  * @brief  Signal the SCMI Manager Task from IRQ context.
  * @retval None
  */
void ScmiMgrTask_Signal(void)
{
  (void)osSemaphoreRelease(ScmiMgrSemaphoreHandle);
}

/**
  * @brief  Mask SCMI-related non-secure notifications during critical stop sequences.
  * @retval None
  */
void ScmiMgrTask_DisableNotifications(void)
{
  tfm_ns_notif_set_mask(SCMI_MGR_NS_NOTIF_MASK);
}

/**
  * @brief  Reset SCMI shared state and re-enable SCMI-related notifications.
  * @retval None
  */
void ScmiMgrTask_ResetAndEnableNotifications(void)
{
  uint32_t trashed_event;

  tfm_secure_scmi_reset();
  tfm_ns_notif_set_mask(SCMI_MGR_NS_NOTIF_UNMASK);
  (void)tfm_ns_notif_get(&trashed_event);
  (void)tfm_ns_notif_get_pending(SCMI_MGR_NS_NOTIF_MASK);
}

/* Private function definitions ----------------------------------------------*/
/**
  * @brief  SCMI Manager task main loop.
  * @param  argument: Not used.
  * @retval None
  */
static void ScmiMgrTask(void *argument)
{
    (void)argument;
    uint32_t event;
    for (;;)
    {
        /* Wait for notification from IRQ handler */
        if (osSemaphoreAcquire(ScmiMgrSemaphoreHandle, osWaitForever) == osOK)
        {
            /* Check for pending events */
            while (!tfm_ns_notif_get(&event))
            {
                ScmiMgrHandleEvent(event);
            }
            /* Handle SCMI system power notification */
            ScmiMgrHandleScmiPowerNotif();
        }
    }
}

/**
 * @brief Handles SCMI system power notifications.
 * @note This function runs in a loop until the notification buffer is empty.
 */
static void ScmiMgrHandleScmiPowerNotif(void)
{
    uint32_t tmp[32];
    psa_status_t ret;
    while ((ret = tfm_secure_scmi_get_notif(tmp, sizeof(tmp))) != PSA_SCMI_ERR_BUFFER_EMPTY) {
        if (((tmp[0] >> 10) & 0xff) == SYS_POWER && (tmp[0] & 0xff) == SYS_POWER_STATE_NOTIFIER) {
            ScmiMgr_HandleSysPowerStateNotif(tmp[1], tmp[2] & GRACEFUL, tmp[3]);
        } else {
            APP_LOG_WARN("ScmiMgr", "Unexpected Notif %lx %lx", tmp[0], tmp[3]);
        }
        if (ret == PSA_SCMI_ERR_BUFFER_OVERFLOW)
            APP_LOG_WARN("ScmiMgr", "SCMI Notif overflow");
    }
}

/**
  * @brief  Handle SCMI events and dispatch secure service requests.
  * @param  event: SCMI event identifier.
  * @retval None
  */
static void ScmiMgrHandleEvent(uint32_t event)
{
  if (tfm_ns_notif_get_pending(TFM_SP_IPCC_RSE_NS_EVT) == TFM_SP_IPCC_RSE_NS_EVT)
  {
    psa_call(TFM_MBOX_SERVICE_HANDLE, TFM_MBOX_SERVICE_SID, NULL, 0, NULL, 0);
  }
  else if (tfm_ns_notif_get_pending(TFM_SP_IPCC_SCMI_CA35_NS_EVT) == TFM_SP_IPCC_SCMI_CA35_NS_EVT)
  {
    tfm_secure_scmi_req(STM32MP25_AGENT_ID_CA35);
  }
  else if (tfm_ns_notif_get_pending(TFM_SP_IPCC_SCMI_CA35_BL31_NS_EVT) == TFM_SP_IPCC_SCMI_CA35_BL31_NS_EVT)
  {
    tfm_secure_scmi_req(STM32MP25_AGENT_ID_CA35_BL31);
  }
}

/**
    * @brief  RESERVED_9 interrupt handler for SCMI Manager notifications.
    *
    * This IRQ handler is reserved for SCMI Manager events.
    * It signals the SCMI Manager task to process pending notifications.
    */
void RESERVED_9_IRQHandler(void)
{
  HAL_NVIC_ClearPendingIRQ(RESERVED_9);
  ScmiMgrTask_Signal();
}

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE*************************************************/

