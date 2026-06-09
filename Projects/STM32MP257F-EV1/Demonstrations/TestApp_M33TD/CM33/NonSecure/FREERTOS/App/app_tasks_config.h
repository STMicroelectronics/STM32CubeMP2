/**
  ******************************************************************************
  * @file    app_tasks_config_template.h
  * @author  MCD Application Team
  * @brief   TEMPLATE: FreeRTOS task configuration macros for all tasks.
  *          Copy this file to your project as app_tasks_config.h and customize.
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

#ifndef APP_TASKS_CONFIG_H
#define APP_TASKS_CONFIG_H

/*------------------- Watchdog Monitor Task -------------------*/
/**
  * @brief  Stack size, priority, and enable switch for Watchdog Monitor Task.
  */
#ifndef ENABLE_WDG_MONITOR_TASK
#define ENABLE_WDG_MONITOR_TASK        1
#endif
#define WDG_MONITOR_TASK_STACK_SIZE    1024 /**< Stack size for Watchdog Monitor Task. */
#define WDG_MONITOR_TASK_PRIORITY      osPriorityHigh /**< High priority for reliable watchdog ping. */

/*------------------- RemoteProc Task -------------------*/
/**
  * @brief  Stack size, priority, and enable switch for RemoteProc Task.
  */
#ifndef ENABLE_REMOTEPROC_TASK
#define ENABLE_REMOTEPROC_TASK         1
#endif
#define REMOTEPROC_TASK_STACK_SIZE     1024 /**< Stack size for RemoteProc Task. */
#define REMOTEPROC_TASK_PRIORITY       osPriorityNormal /**< Standard priority for remote CPU boot/init. */

/*------------------- UserApp Task -------------------*/
/**
  * @brief  Stack size, priority, and enable switch for UserApp Task.
  */
#ifndef ENABLE_USERAPP_TASK
#define ENABLE_USERAPP_TASK            0
#endif
#define USERAPP_TASK_STACK_SIZE        512 /**< Stack size for UserApp Task. */
#define USERAPP_TASK_PRIORITY          osPriorityBelowNormal /**< Lower priority for UserApp (non-critical). */

/*------------------- Logger Task -------------------*/
/**
  * @brief  Stack size, priority, and enable switch for Logger Task.
  */
#ifndef ENABLE_LOGGER_TASK
#define ENABLE_LOGGER_TASK             1
#endif
#define LOGGER_TASK_STACK_SIZE         1024 /**< Stack size for Logger Task. */
#define LOGGER_TASK_PRIORITY           osPriorityBelowNormal /**< Below-normal priority for UART logging. */

/*------------------- SCMI Manager Task -------------------*/
/**
  * @brief  Stack size, priority, and enable switch for SCMI Manager Task.
  */
#ifndef ENABLE_SCMI_MGR_TASK
#define ENABLE_SCMI_MGR_TASK           1
#endif
#define SCMI_MGR_TASK_STACK_SIZE       1024 /**< Stack size for SCMI Manager Task. */
#define SCMI_MGR_TASK_PRIORITY         osPriorityNormal /**< Standard priority for IPCC/SCMI management. */

/*------------------- Low Power Manager Task -------------------*/
/**
  * @brief  Stack size, priority, and enable switch for Low Power Manager Task.
  */
#ifndef ENABLE_LOW_POWER_MGR_TASK
#define ENABLE_LOW_POWER_MGR_TASK      1
#endif
#define LOW_POWER_MGR_TASK_STACK_SIZE  1024 /**< Stack size for Low Power Manager Task. */
#define LOW_POWER_MGR_TASK_PRIORITY    osPriorityLow /**< Low priority for low power sequencing. */

/*------------------- Button Monitor Task -------------------*/
/**
  * @brief  Stack size, priority, and enable switch for Button Monitor Task.
  */
#ifndef ENABLE_BTN_MONITOR_TASK
#define ENABLE_BTN_MONITOR_TASK        0
#endif
#define BTN_MONITOR_TASK_STACK_SIZE    1024 /**< Stack size for Button Monitor Task. */
#define BTN_MONITOR_TASK_PRIORITY      osPriorityNormal /**< Standard priority for button processing. */

/*------------------- OpenAMP Task -------------------*/
/**
  * @brief  Stack size, priority, and enable switch for OpenAMP Task.
  */
#ifndef ENABLE_OPENAMP_TASK
#define ENABLE_OPENAMP_TASK            0
#endif
#define OPENAMP_TASK_STACK_SIZE        1024 /**< Stack size for OpenAMP Task. */
#define OPENAMP_TASK_PRIORITY          osPriorityNormal /**< Standard priority for RPMsg/OpenAMP message processing. */

/*------------------- Display Task -------------------*/
/**
  * @brief  Stack size, priority, and enable switch for Display Task.
  */
#ifndef ENABLE_DISPLAY_TASK
#define ENABLE_DISPLAY_TASK            0
#endif
#define DISPLAY_TASK_STACK_SIZE        1024 /**< Stack size for Display Task. */
#define DISPLAY_TASK_PRIORITY          osPriorityNormal /**< Standard priority for display state machine. */

#endif /* APP_TASKS_CONFIG_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
