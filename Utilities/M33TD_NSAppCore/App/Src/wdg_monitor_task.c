/**
  ******************************************************************************
  * @file    wdg_monitor_task.c
  * @author  MCD Application Team
  * @brief   Watchdog Monitor Task implementation file.
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
    * @addtogroup WdgMonitorTask
    * @{
    */

/* Includes ------------------------------------------------------------------*/
#include "wdg_monitor_task.h"

#if ENABLE_LOW_POWER_MGR_TASK
#include "low_power_mgr_task.h"
#endif
#include "tfm_ioctl_wdt_api.h"

#define WDG_MONITOR_WDG_TIMEOUT_MS     (120000U)
#define WDG_MONITOR_WDG_PING_MS        (80000U)

static osThreadId_t WdgMonitorTaskHandle;
static const osThreadAttr_t WdgMonitorTaskAttr = {
    .name = "WdgMonitorTask",
    .priority = (osPriority_t)WDG_MONITOR_TASK_PRIORITY,
    .stack_size = WDG_MONITOR_TASK_STACK_SIZE
};

/**
    * @brief  Low-power manager listener for watchdog handling.
    * @param  notif: Low-power manager notif.
    * @param  mode: Low-power mode associated with the event.
    * @param  context: User context pointer.
    * @retval None
    */
#if ENABLE_LOW_POWER_MGR_TASK
static void WdgMonitorTask_LowPowerListener(LowPowerMgrNotif_t notif, LowPowerMgrMode_t mode, void *context);

/**
    * @brief  Handle low-power notifications for the watchdog task.
    * @param  notif: Low-power manager notif.
    * @param  mode: Low-power mode associated with the event.
    * @param  context: User context pointer.
    * @retval None
    */
static void WdgMonitorTask_LowPowerListener(LowPowerMgrNotif_t notif, LowPowerMgrMode_t mode, void *context)
{
    int err;

    (void)mode;
    (void)context;

    if ((notif == LOW_POWER_MGR_NOTIF_PREPARE_STOP) ||
        (notif == LOW_POWER_MGR_NOTIF_PREPARE_STANDBY))
    {
        err = tfm_platform_wdt_ping();
        if (err != TFM_PLATFORM_ERR_SUCCESS)
        {
            APP_LOG_ERR("WdgMonitor", "watchdog ping fail on %s:%d",
                        (notif == LOW_POWER_MGR_NOTIF_PREPARE_STOP) ? "prepare_stop" : "prepare_standby",
                        err);
        }
    }
}
#endif

/**
  * @brief  Main function for the Watchdog Monitor Task.
  *         - Configures and starts the watchdog via TFM.
  *         - Periodically pings the watchdog to prevent system reset.
  *         - Logs errors if watchdog operations fail.
  * @param  argument: Not used.
  * @retval None
  */
static void WdgMonitorTask(void *argument)
{
    (void)argument;
    struct wdt_info wdt_info;
    int err = tfm_platform_wdt_info(&wdt_info);
    if (err != TFM_PLATFORM_ERR_SUCCESS)
    {
        APP_LOG_ERR("WdgMonitor", "watchdog get info fail:%d", err);
        osThreadExit();
    }

    if (wdt_info.status == WDT_DISABLED)
    {
        err = tfm_platform_wdt_start();
        if (err != TFM_PLATFORM_ERR_SUCCESS)
        {
            APP_LOG_ERR("WdgMonitor", "watchdog start fail:%d", err);
            osThreadExit();
        }
    }

    APP_LOG_INF("WdgMonitor", "watchdog timeout: %lums", (unsigned long)WDG_MONITOR_WDG_TIMEOUT_MS);

    for (;;)
    {
        err = tfm_platform_wdt_ping();
        if (err != TFM_PLATFORM_ERR_SUCCESS)
        {
            APP_LOG_ERR("WdgMonitor", "watchdog ping fail:%d", err);
        }
        osDelay(WDG_MONITOR_WDG_PING_MS);
    }
}

/**
  * @brief  Initialize the Watchdog Monitor Task and its resources.
  * @retval None
  */
void WdgMonitorTask_Init(void)
{
    struct wdt_timeout_cfg wdt_cfg = { .timeout = WDG_MONITOR_WDG_TIMEOUT_MS };
    int err = tfm_platform_wdt_set(&wdt_cfg);
    if (err != TFM_PLATFORM_ERR_SUCCESS)
    {
        NSAppCore_ErrorHandler();
    }

    WdgMonitorTaskHandle = osThreadNew(WdgMonitorTask, NULL, &WdgMonitorTaskAttr);
    if (WdgMonitorTaskHandle == NULL)
    {
        NSAppCore_ErrorHandler();
    }

#if ENABLE_LOW_POWER_MGR_TASK
    if (LowPowerMgrTask_RegisterListener(WdgMonitorTask_LowPowerListener, NULL) != HAL_OK)
    {
        NSAppCore_ErrorHandler();
    }
#endif
}

/**
  * @brief  De-initialize the Watchdog Monitor Task and release its resources.
  * @retval None
  */
void WdgMonitorTask_DeInit(void)
{
#if ENABLE_LOW_POWER_MGR_TASK
    (void)LowPowerMgrTask_UnregisterListener(WdgMonitorTask_LowPowerListener);
#endif

    if (WdgMonitorTaskHandle)
    {
        osThreadTerminate(WdgMonitorTaskHandle);
        WdgMonitorTaskHandle = NULL;
    }
}

/**
    * @}
    */

