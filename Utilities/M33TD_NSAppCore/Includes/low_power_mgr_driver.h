/**
  ******************************************************************************
  * @file    low_power_mgr_driver.h
  * @author  MCD Application Team
  * @brief   Application driver interface for Low Power Manager sleep hooks.
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

#ifndef LOW_POWER_MGR_DRIVER_H
#define LOW_POWER_MGR_DRIVER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "low_power_mgr_task.h"

/**
  * @brief  Low Power Manager sleep information passed to driver hooks.
  */
typedef struct
{
  LowPowerMgrMode_t mode;
} LowPowerMgrSleepInfo_t;

/**
  * @brief  Low Power Manager driver interface.
  *
  * Project provides one concrete driver by defining the global instance:
  * `LowPowerMgrDriverTypeDef lowPowerMgrDriver;`
  */
typedef struct
{
  int (*init)(void);
  int (*deinit)(void);
  int (*prepareSleep)(const LowPowerMgrSleepInfo_t *sleep_info);
  int (*resumeFromSleep)(const LowPowerMgrSleepInfo_t *sleep_info);
} LowPowerMgrDriverTypeDef;

/**
  * @brief  Global low power manager driver instance.
  * @note   Project must define this symbol in exactly one C file.
  */
extern LowPowerMgrDriverTypeDef lowPowerMgrDriver;

#ifdef __cplusplus
}
#endif

#endif /* LOW_POWER_MGR_DRIVER_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/