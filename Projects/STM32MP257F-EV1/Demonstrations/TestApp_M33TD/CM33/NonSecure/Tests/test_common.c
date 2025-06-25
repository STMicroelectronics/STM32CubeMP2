/**
  ******************************************************************************
  * @file    test_common.c
  * @author  MCD Application Team
  * @brief   Common test utility functions.
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

/* Includes ------------------------------------------------------------------*/
#include "test_common.h"
#include "app_freertos.h"

/**
  * @brief  Run a test with init, run, and deinit function pointers.
  * @param  init:    Pointer to test initialization function.
  * @param  run:     Pointer to test execution function.
  * @param  deinit:  Pointer to test de-initialization function.
  * @retval TEST_STATUS: TEST_OK if all steps succeed, TEST_FAIL otherwise.
  */
TEST_STATUS Run_Test(TEST_STATUS (*init)(void), TEST_STATUS (*run)(void), TEST_STATUS (*deinit)(void))
{
    TEST_STATUS status = TEST_OK;

    if (!init || !run || !deinit) {
        // Null function pointer detected
        return TEST_FAIL;
    }
    if (init() != TEST_OK) {
        status = TEST_FAIL;
        return status;
    }
    if (run() != TEST_OK) {
        status = TEST_FAIL;
        (void)deinit();
        return status;
    }
    if (deinit() != TEST_OK) {
        status = TEST_FAIL;
    }
    return status;
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/