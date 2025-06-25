/**
  ******************************************************************************
  * @file    test_adc.h
  * @author  MCD Application Team
  * @brief   This file contains the headers of the ADC test routines.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software component is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#ifndef TEST_ADC_H
#define TEST_ADC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "test_common.h"

/* Exported functions ------------------------------------------------------- */
/**
  * @brief  Acquire ADC resources.
  * @retval TEST_STATUS: TEST_OK if success, TEST_FAIL otherwise.
  */
TEST_STATUS Test_ADC_ResourceAcquire(void);

/**
  * @brief  Release ADC resources.
  * @retval TEST_STATUS: TEST_OK if success, TEST_FAIL otherwise.
  */
TEST_STATUS Test_ADC_ResourceRelease(void);

/**
  * @brief  Initialize the ADC peripheral and configure the channel.
  * @retval TEST_STATUS: TEST_OK if success, TEST_FAIL otherwise.
  */
TEST_STATUS Test_ADC_Init(void);

/**
  * @brief  Run ADC conversion and process the result.
  * @retval TEST_STATUS: TEST_OK if success, TEST_FAIL otherwise.
  */
TEST_STATUS Test_ADC_Run(void);

/**
  * @brief  De-initialize the ADC peripheral and release resources.
  * @retval TEST_STATUS: TEST_OK if success, TEST_FAIL otherwise.
  */
TEST_STATUS Test_ADC_DeInit(void);

#ifdef __cplusplus
}
#endif

#endif /* TEST_ADC_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
