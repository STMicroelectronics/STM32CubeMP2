/**
  ******************************************************************************
  * @file    test_i2c.h
  * @author  MCD Application Team
  * @brief   This file contains the headers of the I2C test routines.
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

#ifndef TEST_I2C_H
#define TEST_I2C_H

#ifdef __cplusplus
extern "C" {
#endif

#include "test_common.h"

/* Exported functions prototypes ---------------------------------------------*/
/**
  * @brief  Acquire I2C resources.
  * @retval TEST_STATUS: TEST_OK if success, TEST_FAIL otherwise.
  */
TEST_STATUS Test_I2C_ResourceAcquire(void);

/**
  * @brief  Release I2C resources.
  * @retval TEST_STATUS: TEST_OK if success, TEST_FAIL otherwise.
  */
TEST_STATUS Test_I2C_ResourceRelease(void);

/**
  * @brief  Initialize the I2C peripheral and configure the test environment.
  * @retval TEST_STATUS: TEST_OK if success, TEST_FAIL otherwise.
  */
TEST_STATUS Test_I2C_Init(void);

/**
  * @brief  Run the I2C test routine.
  * @retval TEST_STATUS: TEST_OK if success, TEST_FAIL otherwise.
  */
TEST_STATUS Test_I2C_Run(void);

/**
  * @brief  De-initialize the I2C peripheral and release resources.
  * @retval TEST_STATUS: TEST_OK if success, TEST_FAIL otherwise.
  */
TEST_STATUS Test_I2C_DeInit(void);

#ifdef __cplusplus
}
#endif

#endif /* TEST_I2C_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
