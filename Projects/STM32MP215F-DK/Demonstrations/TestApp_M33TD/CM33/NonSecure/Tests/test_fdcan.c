/**
  ******************************************************************************
  * @file    test_fdcan.c
  * @author  MCD Application Team
  * @brief   FDCAN test routines.
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
#include "test_fdcan.h"
#include "app_freertos.h"

#ifdef ENABLE_FDCAN_TEST  // Conditionally compile the FDCAN test code


/** @addtogroup STM32MP2xx_Application
  * @{
  */

/** @addtogroup StarterApp_M33TD_Tests
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN Test_FDCAN_Implementation */
/* USER CODE END Test_FDCAN_Implementation */

/**
  * @brief  Initialize the FDCAN peripheral and configure filters.
  * @retval TEST_STATUS: TEST_OK if success, TEST_FAIL otherwise.
  */
TEST_STATUS Test_FDCAN_Init(void)
{
    // USER CODE BEGIN Test_FDCAN_Init
    return TEST_OK;
    // USER CODE END Test_FDCAN_Init
}

/**
  * @brief  Run FDCAN transmission and reception test.
  * @retval TEST_STATUS: TEST_OK if success, TEST_FAIL otherwise.
  */
TEST_STATUS Test_FDCAN_Run(void)
{
    // USER CODE BEGIN Test_FDCAN_Run
    return TEST_OK;
    // USER CODE END Test_FDCAN_Run
}

/**
  * @brief  De-initialize the FDCAN peripheral and release resources.
  * @retval TEST_STATUS: TEST_OK if success, TEST_FAIL otherwise.
  */
TEST_STATUS Test_FDCAN_DeInit(void)
{
    // USER CODE BEGIN Test_FDCAN_DeInit
    return TEST_OK;
    // USER CODE END Test_FDCAN_DeInit
}

/**
  * @brief  Acquire FDCAN resources.
  * @retval TEST_STATUS: TEST_OK if success, TEST_FAIL otherwise.
  */
TEST_STATUS Test_FDCAN_ResourceAcquire(void)
{
    // Acquire FDCAN using the resource manager
    return TEST_OK;
}

/**
  * @brief  Release FDCAN resources.
  * @retval TEST_STATUS: TEST_OK if success, TEST_FAIL otherwise.
  */
TEST_STATUS Test_FDCAN_ResourceRelease(void)
{
    // Release FDCAN using the resource manager
    return TEST_OK;
}

/**
  * @}
  */

/**
  * @}
  */
#endif /* ENABLE_FDCAN_TEST */  // End of conditional compilation
