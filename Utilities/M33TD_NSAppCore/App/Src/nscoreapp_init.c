/**
    ******************************************************************************
    * @file    nscoreapp_init.c
    * @author  MCD Application Team
    * @brief   Non-secure core application initialization and de-initialization.
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
    * @addtogroup NSCoreApp
    * @{
    */

/* Includes ------------------------------------------------------------------*/
#include "nscoreapp_init.h"
#include "logger_task.h"
#if ENABLE_LOW_POWER_MGR_TASK
#include "low_power_mgr_task.h"
#endif
#include "scmi_mgr_task.h"
#include "remoteproc_task.h"
#include "wdg_monitor_task.h"
#if ENABLE_USERAPP_TASK
#include "userapp_task.h"
#endif
#if ENABLE_BTN_MONITOR_TASK
#include "btn_monitor_task.h"
#endif
#if ENABLE_OPENAMP_TASK
#include "openamp_task.h"
#endif
#if ENABLE_DISPLAY_TASK
#include "display_task.h"
#endif
#if ENABLE_FWU_MGR_TASK
#include "fwu_mgr_task.h"
#endif

/**
  * @brief  Initialize all non-secure core application tasks and modules.
  *         This includes display, logger, OpenAMP, SCMI manager, remoteproc, and watchdog monitor tasks.
  */
void NSCoreApp_Init(void)
{
    /* Initialize the Logger task first to ensure UART/log output is ready. */
    LoggerTask_Init();

    /* Print standard startup logs (system, FW version, etc.). */
    printf("[NS] [INF] Non-Secure system starting...\r\n");
    printf("\033[1;34m[NS] [INF] STM32Cube FW version: v%ld.%ld.%ld-rc%ld \033[0m\r\n",
        ((HAL_GetHalVersion() >> 24) & 0x000000FF),
        ((HAL_GetHalVersion() >> 16) & 0x000000FF),
        ((HAL_GetHalVersion() >> 8) & 0x000000FF),
        (HAL_GetHalVersion() & 0x000000FF));

    /* Initialize TF-M NS interface mutex for atomic PSA calls. */
    if (tfm_ns_interface_init() != 0)
    {
        NSAppCore_ErrorHandler();
    }

    /* Initialize other core stack modules. */
#if ENABLE_LOW_POWER_MGR_TASK
    LowPowerMgrTask_Init();
#endif

#if ENABLE_USERAPP_TASK
    UserAppTask_Init();
#endif

#if ENABLE_BTN_MONITOR_TASK
    BtnMonitorTask_Init();

#endif
#if ENABLE_DISPLAY_TASK
    DisplayTask_Init();
#endif

    ScmiMgrTask_Init();

#if ENABLE_OPENAMP_TASK
    OpenampTask_Init();

#endif
    RemoteProcTask_Init();
    WdgMonitorTask_Init();
#if ENABLE_FWU_MGR_TASK

    FwuMgrTask_Init();
#endif
}

/**
  * @brief  De-initialize all non-secure core application tasks and modules.
  *         This includes display, logger, OpenAMP, SCMI manager, remoteproc, and watchdog monitor tasks.
  */
void NSCoreApp_DeInit(void)
{
    WdgMonitorTask_DeInit();
    RemoteProcTask_DeInit();

#if ENABLE_FWU_MGR_TASK
    FwuMgrTask_DeInit();
#endif

#if ENABLE_OPENAMP_TASK
    OpenampTask_DeInit();

#endif
    ScmiMgrTask_DeInit();

#if ENABLE_DISPLAY_TASK
    DisplayTask_DeInit();

#endif

#if ENABLE_BTN_MONITOR_TASK
    BtnMonitorTask_DeInit();

#endif
#if ENABLE_USERAPP_TASK
    UserAppTask_DeInit();
#endif
#if ENABLE_LOW_POWER_MGR_TASK
    LowPowerMgrTask_DeInit();
#endif
    LoggerTask_DeInit();
}

/**
    * @}
    */

