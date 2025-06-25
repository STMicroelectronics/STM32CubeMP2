/**
  ******************************************************************************
  * @file    testapp_task.h
  * @author  MCD Application Team
  * @brief   Header file for Test Application Task implementation.
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

#ifndef TESTAPP_TASK_H
#define TESTAPP_TASK_H

#ifdef __cplusplus
extern "C" {
#endif

/* Exported macros -----------------------------------------------------------*/
/**
  * @defgroup Exported_Macros Exported Macros
  * @brief Macros to enable or disable specific tests.
  * @{
  */

#ifndef DISABLE_TIMERS_TEST
#define ENABLE_TIMERS_TEST /**< Enable Timers Test. */
#endif

#ifndef DISABLE_I2C_TEST
//#define ENABLE_I2C_TEST    /**< I2C test is not supported yet. */
#endif

#ifndef DISABLE_SPI_TEST
#define ENABLE_SPI_TEST /**< Enable SPI Test. */
#endif

#ifndef DISABLE_ADC_TEST
//#define ENABLE_ADC_TEST    /**< ADC test is not supported yet. */
#endif

#ifndef DISABLE_FDCAN_TEST
//#define ENABLE_FDCAN_TEST  /**< FDCAN test is not supported yet. */
#endif

#ifndef DISABLE_I3C_TEST
#define ENABLE_I3C_TEST /**< Enable I3C Test. */
#endif

/**
  * @brief Define ENABLE_AUTO_TEST with ON/OFF values.
  */
#ifndef ENABLE_AUTO_TEST
#define ENABLE_AUTO_TEST 0 /**< 0: OFF, 1: ON. */
#endif

/** @} */

/* Exported types ------------------------------------------------------------*/
/**
  * @defgroup Exported_Types Exported Types
  * @brief Types used in the Test Application Task.
  * @{
  */

/**
  * @brief Enumeration for test types.
  */
typedef enum {
    TEST_TYPE_AUTO,   /**< Automatic test type. */
    TEST_TYPE_MANUAL  /**< Manual test type. */
} TestType;

/** @} */

/* Exported functions prototypes ---------------------------------------------*/
/**
  * @defgroup Exported_Functions Exported Functions
  * @brief Function prototypes for the Test Application Task.
  * @{
  */

/**
  * @brief  Initialize the Test Application Task and its resources.
  * @retval None
  */
void TestAppTask_Init(void);

/**
  * @brief  De-initialize the Test Application Task and release its resources.
  * @retval None
  */
void TestAppTask_DeInit(void);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* TESTAPP_TASK_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
