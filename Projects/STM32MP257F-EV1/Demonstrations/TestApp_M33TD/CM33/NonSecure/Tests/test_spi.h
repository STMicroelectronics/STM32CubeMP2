/**
  ******************************************************************************
  * @file    test_spi.h
  * @author  MCD Application Team
  * @brief   This file contains the headers of the SPI test routines.
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

#ifndef TEST_SPI_H
#define TEST_SPI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "test_common.h"

/* Exported functions prototypes ---------------------------------------------*/
/**
  * @brief  Acquire SPI resources.
  * @retval TEST_STATUS: TEST_OK if success, TEST_FAIL otherwise.
  */
TEST_STATUS Test_SPI_ResourceAcquire(void);

/**
  * @brief  Release SPI resources.
  * @retval TEST_STATUS: TEST_OK if success, TEST_FAIL otherwise.
  */
TEST_STATUS Test_SPI_ResourceRelease(void);

/**
  * @brief  Initialize the SPI peripheral and configure the test environment.
  * @retval TEST_STATUS: TEST_OK if success, TEST_FAIL otherwise.
  */
TEST_STATUS Test_SPI_Init(void);

/**
  * @brief  Run the SPI test routine.
  * @retval TEST_STATUS: TEST_OK if success, TEST_FAIL otherwise.
  */
TEST_STATUS Test_SPI_Run(void);

/**
  * @brief  De-initialize the SPI peripheral and release resources.
  * @retval TEST_STATUS: TEST_OK if success, TEST_FAIL otherwise.
  */
TEST_STATUS Test_SPI_DeInit(void);

#ifdef __cplusplus
}
#endif

#endif /* TEST_SPI_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
