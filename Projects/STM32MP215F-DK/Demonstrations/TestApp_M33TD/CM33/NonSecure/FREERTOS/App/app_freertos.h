/**
  ******************************************************************************
  * @file    app_freertos.h
  * @author  MCD Application Team
  * @brief   Header file for the FreeRTOS application.
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
#ifndef APP_FREERTOS_H
#define APP_FREERTOS_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/**
  * @brief Includes required for FreeRTOS application functionality.
  */
#include "cmsis_os2.h"
#include "FreeRTOSConfig.h"
#include "testapp_task.h"
#include "main.h"

/* Exported constants --------------------------------------------------------*/
/**
  * @defgroup Exported_Constants Exported Constants
  * @brief Constants used across the FreeRTOS application.
  * @{
  */
#define ANSI_COLOR_RED     "\x1b[31m" /**< ANSI escape code for red text. */
#define ANSI_COLOR_RESET   "\x1b[0m"  /**< ANSI escape code to reset text color. */
/** @} */

/* Exported macros -----------------------------------------------------------*/
/**
  * @defgroup Exported_Macros Exported Macros
  * @brief Logging macros for FreeRTOS tasks.
  * @{
  */
#define APP_LOG_INF(tag, fmt, ...)      printf("[NS] [%s] [INF] " fmt "\r\n", tag, ##__VA_ARGS__) /**< Info log macro. */
#define APP_LOG_ERR(tag, fmt, ...)      printf(ANSI_COLOR_RED "[NS] [%s] [ERR] " fmt ANSI_COLOR_RESET "\r\n", tag, ##__VA_ARGS__) /**< Error log macro. */
#define APP_LOG_INF_CONT(tag, fmt, ...) printf("[NS] [%s] [INF] " fmt, tag, ##__VA_ARGS__) /**< Continuous info log macro. */
#define APP_LOG_CRS(tag, fmt, ...)      printf("[NS] [%s] [CRASH] " fmt "\r\n", tag, ##__VA_ARGS__) /**< Crash log macro. */
/** @} */

/* Exported types ------------------------------------------------------------*/
/**
  * @defgroup Exported_Types Exported Types
  * @brief Types used in the FreeRTOS application.
  * @{
  */
/* USER CODE BEGIN ET */
/* Add exported types here if needed */
/* USER CODE END ET */
/** @} */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
/* Add exported constants here if needed */
/* USER CODE END EC */

/* Exported macros -----------------------------------------------------------*/
/* USER CODE BEGIN EM */
/* Add exported macros here if needed */
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
/**
  * @defgroup Exported_Functions Exported Functions
  * @brief Function prototypes for the FreeRTOS application.
  * @{
  */

/**
  * @brief  Initializes the FreeRTOS application.
  * @retval None
  */
void MX_FREERTOS_Init(void);

/**
  * @brief  Handles application-level errors.
  * @retval None
  */
void App_ErrorHandler(void);

/** @} */

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* Add private defines here if needed */
/* USER CODE END PD */

#ifdef __cplusplus
}
#endif
#endif /* APP_FREERTOS_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
