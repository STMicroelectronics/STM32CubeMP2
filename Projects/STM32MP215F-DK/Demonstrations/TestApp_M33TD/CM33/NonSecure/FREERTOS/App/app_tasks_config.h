/**
  ******************************************************************************
  * @file    app_tasks_config.h
  * @author  MCD Application Team
  * @brief   This file contains FreeRTOS task configuration macros.
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

#include "cmsis_os2.h"

/* Exported constants --------------------------------------------------------*/
/**
  * @defgroup Exported_Constants Exported Constants
  * @brief Constants for FreeRTOS task configuration.
  * @{
  */

/**
  * @brief  Stack size and priority configuration for RemoteProc Task.
  */
#define REMOTEPROC_TASK_STACK_SIZE    1028 /**< Stack size for RemoteProc Task. */
#define REMOTEPROC_TASK_PRIORITY      osPriorityRealtime /**< Priority for RemoteProc Task. */

/**
  * @brief  Stack size and priority configuration for UserApp Task.
  */
#define USERAPP_TASK_STACK_SIZE       512 /**< Stack size for UserApp Task. */
#define USERAPP_TASK_PRIORITY         osPriorityBelowNormal /**< Priority for UserApp Task. */

/**
  * @brief  Stack size and priority configuration for TestApp Task.
  */
#define TESTAPP_TASK_STACK_SIZE       2048 /**< Stack size for TestApp Task. */
#define TESTAPP_TASK_PRIORITY         osPriorityAboveNormal /**< Priority for TestApp Task. */

/** @} */

#endif /* APP_TASKS_CONFIG_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
