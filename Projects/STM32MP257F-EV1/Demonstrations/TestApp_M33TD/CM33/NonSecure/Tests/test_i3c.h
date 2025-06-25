/**
  ******************************************************************************
  * @file    test_i3c.h
  * @author  MCD Application Team
  * @brief   Header file for I3C test routines.
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

#ifndef TEST_I3C_H
#define TEST_I3C_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "test_common.h"

/** @addtogroup I3C_Test
  * @{
  */

/** @defgroup I3C_Exported_Functions Exported Functions
  * @brief    Exported functions for I3C test routines.
  * @{
  */

/**
  * @brief  Acquire I3C resources.
  * @retval TEST_STATUS: TEST_OK if success, TEST_FAIL otherwise.
  */
TEST_STATUS Test_I3C_ResourceAcquire(void);

/**
  * @brief  Release I3C resources.
  * @retval TEST_STATUS: TEST_OK if success, TEST_FAIL otherwise.
  */
TEST_STATUS Test_I3C_ResourceRelease(void);

/**
  * @brief  Initialize the I3C peripheral and configure the test environment.
  * @retval TEST_STATUS: TEST_OK if success, TEST_FAIL otherwise.
  */
TEST_STATUS Test_I3C_Init(void);

/**
  * @brief  Run the I3C test routine for the OLED display.
  * @retval TEST_STATUS: TEST_OK if success, TEST_FAIL otherwise.
  */
TEST_STATUS Test_I3C_Run(void);

/**
  * @brief  De-initialize the I3C peripheral and release resources.
  * @retval TEST_STATUS: TEST_OK if success, TEST_FAIL otherwise.
  */
TEST_STATUS Test_I3C_DeInit(void);

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* TEST_I3C_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/