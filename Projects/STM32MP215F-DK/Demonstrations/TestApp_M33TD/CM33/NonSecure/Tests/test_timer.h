/**
  ******************************************************************************
  * @file    test_timer.h
  * @author  MCD Application Team
  * @brief   This file contains the headers of the TIMER test routines.
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

#ifndef TEST_TIMER_H
#define TEST_TIMER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "test_common.h"

/* Exported macro ------------------------------------------------------------*/
/**
  * @brief  Get the current counter value of the specified LPTIM handle.
  */
#define __HAL_LPTIM_GET_COUNTER(__HANDLE__) ((__HANDLE__)->Instance->CNT)

/* Exported functions prototypes ---------------------------------------------*/
/**
  * @brief  Acquire timer resources.
  * @retval TEST_STATUS: TEST_OK if success, TEST_FAIL otherwise.
  */
TEST_STATUS Test_TIMER_ResourceAcquire(void);

/**
  * @brief  Release timer resources.
  * @retval TEST_STATUS: TEST_OK if success, TEST_FAIL otherwise.
  */
TEST_STATUS Test_TIMER_ResourceRelease(void);

/* Exported functions ------------------------------------------------------- */
/**
  * @brief  Initialize all timers and configure synchronization.
  * @retval TEST_STATUS: TEST_OK if success, TEST_FAIL otherwise.
  */
TEST_STATUS Test_TIMER_Init(void);

/**
  * @brief  Run timer synchronization test and observe results.
  * @retval TEST_STATUS: TEST_OK if success, TEST_FAIL otherwise.
  */
TEST_STATUS Test_TIMER_Run(void);

/**
  * @brief  De-initialize all timers and release resources.
  * @retval TEST_STATUS: TEST_OK if success, TEST_FAIL otherwise.
  */
TEST_STATUS Test_TIMER_DeInit(void);

#ifdef __cplusplus
}
#endif

#endif /* TEST_TIMER_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
