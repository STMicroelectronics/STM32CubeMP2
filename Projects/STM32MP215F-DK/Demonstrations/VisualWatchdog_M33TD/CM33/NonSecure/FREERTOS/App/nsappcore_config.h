/**
  ******************************************************************************
  * @file    nsappcore_config.h
  * @author  Application Team
  * @brief   Centralized configuration for NSAppCore utility stack integration.
  *
  * @details
  *   - Copy this file as nsappcore_config.h in your project and customize as needed.
  ******************************************************************************
  */

#ifndef NSAPPCORE_CONFIG_H
#define NSAPPCORE_CONFIG_H

/* ---------------------------------------------------------------------------
 * NSAppCore Version Check
 * ---------------------------------------------------------------------------
 * - Ensures the included nsappcore_version.h matches the required version.
 * - Reports a clear error if the version is missing or too old.
 * ------------------------------------------------------------------------- */
#include "nsappcore_version.h"

#ifndef NSAPPCORE_VERSION_MAJOR
#error "NSAppCore version header (nsappcore_version.h) is missing or not included."
#endif

#if (NSAPPCORE_VERSION_MAJOR < 2)
#error "NSAppCore utility stack version 2.0.0 or higher is required."
#endif

/* ---------------------------------------------------------------------------
 * HAL Device Header Dependency
 * ---------------------------------------------------------------------------
 * - Defines and includes the HAL device header required by NSAppCore.
 * - Reports a clear error if the expected HAL header is not present.
 * ------------------------------------------------------------------------- */
#ifndef NSAPPCORE_HAL_HEADER
#define NSAPPCORE_HAL_HEADER "stm32mp2xx_hal.h"
#endif

#include NSAPPCORE_HAL_HEADER

#ifndef __STM32MP2xx_HAL_H
#error "HAL device header (e.g., stm32mp2xx_hal.h) must be included for NSAppCore stack."
#endif

/* ---------------------------------------------------------------------------
 * Task Configuration
 * ---------------------------------------------------------------------------
 * - Includes project-level task configuration macros.
 * ------------------------------------------------------------------------- */

/* ---------------------------------------------------------------------------
 * Remote Processor Auto-Start
 * ---------------------------------------------------------------------------
 * Controls whether the RemoteProc task automatically starts the remote CPU
 * during system initialization.
 *
 * Default is enabled (1) to match the current behavior.
 *
 * Configuration:
 *   REMOTE_PROC_AUTO_START=1  Enable auto-start
 *   REMOTE_PROC_AUTO_START=0  Disable auto-start
 *
 * Example (build flags):
 *   -DREMOTE_PROC_AUTO_START=1
 *   -DREMOTE_PROC_AUTO_START=0
 * ------------------------------------------------------------------------- */
#ifndef REMOTE_PROC_AUTO_START
#define REMOTE_PROC_AUTO_START 1
#endif

/* ---------------------------------------------------------------------------
 * Default Low Power Policy
 * ---------------------------------------------------------------------------
 * Controls whether the Low Power Manager accepts suspend requests by default,
 * before any runtime policy update arrives through RPMsg, button, or another
 * task.
 *
 * This macro does not select the target suspend mode directly.
 * - 0: default low power entry disabled
 * - 1: default low power entry enabled
 *
 * When enabled, the reset suspend policy initializes the allowed low power
 * mode to the deepest low power mode currently supported by the manager,
 * which is LPLV_STOP2.
 *
 * Example (build flags):
 *   -DLOW_POWER_DEFAULT_POLICY_ENABLE=1
 *   -DLOW_POWER_DEFAULT_POLICY_ENABLE=0
 * ------------------------------------------------------------------------- */
#ifndef LOW_POWER_DEFAULT_POLICY_ENABLE
#define LOW_POWER_DEFAULT_POLICY_ENABLE 0
#endif

/* ---------------------------------------------------------------------------
 * Low Power Manager Capacity
 * ---------------------------------------------------------------------------
 * Optional project overrides for low-power runtime capacity:
 * - LOW_POWER_MGR_MAX_AGENTS: number of runtime low-power agents.
 * - LOW_POWER_MGR_MAX_LISTENERS: number of low-power event listeners.
 *
 * Increase LOW_POWER_MGR_MAX_LISTENERS when enabling more tasks that
 * subscribe to low-power events.
 * ------------------------------------------------------------------------- */
/* #define LOW_POWER_MGR_MAX_AGENTS    2U */
/* #define LOW_POWER_MGR_MAX_LISTENERS 8U */

/* Optional task enables (project-level):
 * These tasks default to disabled in app_tasks_config.h.
 * Uncomment if your project requires them.
 */
 #define ENABLE_USERAPP_TASK         1
 #define ENABLE_BTN_MONITOR_TASK     1
 #define ENABLE_OPENAMP_TASK         1
// #define ENABLE_FWU_MGR_TASK         1
// #define ENABLE_LOW_POWER_MGR_TASK   1

/* If the project enables a display panel, automatically enable Display Task.
 */
#if defined(DISPLAY_PANEL_ENABLED)
#define ENABLE_DISPLAY_TASK 1
#endif

#include "app_tasks_config.h"

/* ---------------------------------------------------------------------------
 * Display Stack Dependency
 * ---------------------------------------------------------------------------
 * ------------------------------------------------------------------------- */
#if ENABLE_DISPLAY_TASK
/* Framebuffer base address (project may override) */
#ifndef LCD_FB_START_ADDRESS
/**
  * @brief LCD frame buffer located on the DDR after code.
  */
#define LCD_FB_START_ADDRESS    ((uint32_t *)0x80b00000UL)
#endif

#include "display_panel_config.h"
#include "display_utils.h"
#include "lcd_panel.h"
#endif

/* ---------------------------------------------------------------------------
 * Common Macros and Logging Utilities
 * ---------------------------------------------------------------------------
 * - Provides logging macros (APP_LOG_INF, etc.), color codes, and utilities.
 * - Ensure app_common.h is available in your project.
 * ------------------------------------------------------------------------- */

/* ---------------------------------------------------------------------------
 * Realtime Debug Logging
 * ---------------------------------------------------------------------------
 * When enabled, APP_LOG_DBG prints directly using printf (cyan), instead of
 * sending debug logs through the logger queue.
 *
 * Configuration:
 *   REALTIME_DEBUG_LOG_ENABLED=1  Enable direct debug prints
 *   REALTIME_DEBUG_LOG_ENABLED=0  Use logger queue for debug logs
 *
 * Example (build flags):
 *   -DREALTIME_DEBUG_LOG_ENABLED=1
 *   -DREALTIME_DEBUG_LOG_ENABLED=0
 * ------------------------------------------------------------------------- */
#ifndef REALTIME_DEBUG_LOG_ENABLED
#define REALTIME_DEBUG_LOG_ENABLED 0
#endif

#include "app_common.h"

/* ---------------------------------------------------------------------------
 * Fault Manager (FaultMgr) Integration
 * ---------------------------------------------------------------------------
 * - Controls exception handling, backtrace, and diagnostics.
 * - To disable all fault/exception handling, define FAULT_EXCEPTION_ENABLE as 0
 *   before including this file.
 * - All configuration is controlled via fault_config.h.
 * ------------------------------------------------------------------------- */
/* #define FAULT_EXCEPTION_ENABLE 0 */  /* Uncomment to disable FaultMgr globally */
#include "fault_config.h"
#include "fault_manager.h"
/* ---------------------------------------------------------------------------
 * IRQ Priority Configuration
 * ---------------------------------------------------------------------------
 * - Defines default IRQ priority for NSAppCore stack tasks and ISRs.
 * - Must match configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY in FreeRTOSConfig.h.
 * ------------------------------------------------------------------------- */
#ifndef DEFAULT_IRQ_PRIO
#define DEFAULT_IRQ_PRIO 1U
#endif

#ifndef NSAPPCORE_IRQ_PRIO_BASE
#define NSAPPCORE_IRQ_PRIO_BASE 5U
#endif

/**
  * @note
  *   - NSAPPCORE_IRQ_PRIO_BASE must match configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY.
  *   - Only ISRs at this priority or lower may call FreeRTOS API functions.
  */

/* ---------------------------------------------------------------------------
 * FreeRTOS and CMSIS-RTOS2 Dependency
 * ---------------------------------------------------------------------------
 * - Includes CMSIS-OS2 for RTOS abstraction.
 * - Make sure CMSIS-OS2 is available in your project include paths.
 * ------------------------------------------------------------------------- */
#include "cmsis_os2.h"

#ifndef NSAPPCORE_IRQ_PRIO_BASE
#define NSAPPCORE_IRQ_PRIO_BASE configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY
#endif

#ifndef CMSIS_OS2_H_
#error "CMSIS-RTOS2 (cmsis_os2.h) must be included and available for NSAppCore."
#endif

/* ---------------------------------------------------------------------------
 * TFM (Trusted Firmware-M) Dependency
 * ---------------------------------------------------------------------------
 * - Required for SCMI, remoteproc, watchdog, etc.
 * - Either include main.h (if it includes all TFM headers), or include them directly.
 * ------------------------------------------------------------------------- */
/* #include "main.h" */  /* Optionally include main.h if it includes all TFM headers */
#include "tfm_ns_interface.h"
#include "tfm_platform_api.h"
#include "tfm_scmi_api.h"

/* Header availability checks for TFM APIs */
#ifndef __TFM_PLATFORM_API__
#error "TFM Platform API (tfm_platform_api.h) is required for NSAppCore."
#endif
#ifndef TFM_SCMI_API_H
#error "TFM SCMI API (tfm_scmi_api.h) is required for NSAppCore."
#endif
#ifndef __TFM_NS_INTERFACE_H__
#error "TFM NS Interface (tfm_ns_interface.h) is required for NSAppCore."
#endif

/* ---------------------------------------------------------------------------
 * Logger Verbosity Configuration
 * ---------------------------------------------------------------------------
 * - Sets default logger verbosity (0 = LOW, 1 = MEDIUM, 2 = HIGH).
 * ------------------------------------------------------------------------- */
#ifndef LOGGER_VERBOSITY_DEFAULT
#define LOGGER_VERBOSITY_DEFAULT 0  /* LOG_VERBOSITY_LOW */
#endif

/* ---------------------------------------------------------------------------
 * Task Dependency Checks
 * ---------------------------------------------------------------------------
 * - Ensures that required core tasks are enabled.
 * ------------------------------------------------------------------------- */

/* NSCoreApp_Init() always initializes these core tasks. */
#if !ENABLE_LOGGER_TASK
#error "Logger Task is required (NSCoreApp_Init always starts it). Set ENABLE_LOGGER_TASK=1 in app_tasks_config.h."
#endif

#if !ENABLE_SCMI_MGR_TASK
#error "SCMI Manager Task is required (NSCoreApp_Init always starts it). Set ENABLE_SCMI_MGR_TASK=1 in app_tasks_config.h."
#endif

#if !ENABLE_REMOTEPROC_TASK
#error "RemoteProc Task is required (NSCoreApp_Init always starts it). Set ENABLE_REMOTEPROC_TASK=1 in app_tasks_config.h."
#endif

#if !ENABLE_WDG_MONITOR_TASK
#error "Watchdog Monitor Task is required (NSCoreApp_Init always starts it). Set ENABLE_WDG_MONITOR_TASK=1 in app_tasks_config.h."
#endif

#if ENABLE_FWU_MGR_TASK && !ENABLE_OPENAMP_TASK
#error "FWU Manager Task requires OpenAMP Task. Set ENABLE_OPENAMP_TASK=1 when ENABLE_FWU_MGR_TASK=1."
#endif

/* ---------------------------------------------------------------------------
 * Error Handler Integration
 * ---------------------------------------------------------------------------
 * - Project must provide Error_Handler() implementation.
 * - NSAppCore_ErrorHandler macro can be overridden to use a project-specific handler.
 * ------------------------------------------------------------------------- */
extern void Error_Handler(void);

#ifndef NSAppCore_ErrorHandler
#define NSAppCore_ErrorHandler Error_Handler
#endif

#endif /* NSAPPCORE_CONFIG_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
