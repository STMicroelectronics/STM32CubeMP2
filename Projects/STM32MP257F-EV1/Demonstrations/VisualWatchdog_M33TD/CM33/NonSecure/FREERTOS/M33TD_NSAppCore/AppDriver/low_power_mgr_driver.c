/**
  ******************************************************************************
  * @file    low_power_mgr_driver.c
  * @author  MCD Application Team
  * @brief   Low Power Manager driver implementation.
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
  * @addtogroup LowPowerMgrDriver
  * @{
  */

/* Includes ------------------------------------------------------------------*/
#include "low_power_mgr_driver.h"
#include "main.h"

/* Private function prototypes -----------------------------------------------*/
static int LowPowerMgrDriver_Init(void);
static int LowPowerMgrDriver_DeInit(void);
static int LowPowerMgrDriver_PrepareSleep(const LowPowerMgrSleepInfo_t *sleep_info);
static int LowPowerMgrDriver_ResumeFromSleep(const LowPowerMgrSleepInfo_t *sleep_info);

/* Exported driver instance --------------------------------------------------*/
LowPowerMgrDriverTypeDef lowPowerMgrDriver =
{
  .init = LowPowerMgrDriver_Init,
  .deinit = LowPowerMgrDriver_DeInit,
  .prepareSleep = LowPowerMgrDriver_PrepareSleep,
  .resumeFromSleep = LowPowerMgrDriver_ResumeFromSleep,
};

/* Private functions ---------------------------------------------------------*/
static int LowPowerMgrDriver_Init(void)
{
  return 0;
}

static int LowPowerMgrDriver_DeInit(void)
{
  return 0;
}

static int LowPowerMgrDriver_PrepareSleep(const LowPowerMgrSleepInfo_t *sleep_info)
{
  (void)sleep_info;

  HAL_SuspendTick();

  return 0;
}

static int LowPowerMgrDriver_ResumeFromSleep(const LowPowerMgrSleepInfo_t *sleep_info)
{
  (void)sleep_info;

  HAL_ResumeTick();
  return 0;
}

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
