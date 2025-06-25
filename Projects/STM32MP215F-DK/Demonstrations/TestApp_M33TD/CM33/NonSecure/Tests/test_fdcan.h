/**
  ******************************************************************************
  * @file    test_fdcan.h
  * @author  MCD Application Team
  * @brief   This file contains the headers of the FDCAN test routines.
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

#ifndef TEST_FDCAN_H
#define TEST_FDCAN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "test_common.h"

/* Exported constants --------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
/**
  * @brief  Acquire FDCAN resources.
  * @retval TEST_STATUS: TEST_OK if success, TEST_FAIL otherwise.
  */
TEST_STATUS Test_FDCAN_ResourceAcquire(void);

/**
  * @brief  Release FDCAN resources.
  * @retval TEST_STATUS: TEST_OK if success, TEST_FAIL otherwise.
  */
TEST_STATUS Test_FDCAN_ResourceRelease(void);

/**
  * @brief  Initialize the FDCAN peripheral and configure filters.
  * @retval TEST_STATUS: TEST_OK if success, TEST_FAIL otherwise.
  */
TEST_STATUS Test_FDCAN_Init(void);

/**
  * @brief  Run FDCAN transmission and reception test.
  * @retval TEST_STATUS: TEST_OK if success, TEST_FAIL otherwise.
  */
TEST_STATUS Test_FDCAN_Run(void);

/**
  * @brief  De-initialize the FDCAN peripheral and release resources.
  * @retval TEST_STATUS: TEST_OK if success, TEST_FAIL otherwise.
  */
TEST_STATUS Test_FDCAN_DeInit(void);

#ifdef __cplusplus
}
#endif

#endif /* TEST_FDCAN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
