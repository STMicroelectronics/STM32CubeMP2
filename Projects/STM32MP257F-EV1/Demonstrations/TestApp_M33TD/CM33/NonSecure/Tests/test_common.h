/**
  ******************************************************************************
  * @file    test_common.h
  * @author  MCD Application Team
  * @brief   This file contains common test utility definitions and prototypes.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef TEST_COMMON_H
#define TEST_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

/* Exported types ------------------------------------------------------------*/
/**
  * @brief  Test status enumeration.
  */
typedef enum {
    TEST_OK = 0,  /*!< Test passed */
    TEST_FAIL     /*!< Test failed */
} TEST_STATUS;

/* Exported functions ------------------------------------------------------- */
/**
  * @brief  Run a test with init, run, and deinit function pointers.
  * @param  init:    Pointer to test initialization function.
  * @param  run:     Pointer to test execution function.
  * @param  deinit:  Pointer to test de-initialization function.
  * @retval TEST_STATUS: TEST_OK if all steps succeed, TEST_FAIL otherwise.
  */
TEST_STATUS Run_Test(TEST_STATUS (*init)(void), TEST_STATUS (*run)(void), TEST_STATUS (*deinit)(void));

#ifdef __cplusplus
}
#endif

#endif /* TEST_COMMON_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
