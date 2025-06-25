/**
  ******************************************************************************
  * @file    main.h
  * @author  MCD Application Team
  * @brief   Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32mp2xx_hal.h"

/* USER CODE BEGIN Includes */
#include "stm32mp215f_disco.h"
#include "stm32mp215f_disco_stpmic2l.h"

/* USER CODE END Includes */
/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
/* User can use this section to tailor FDCANx instance used and associated
   resources */
/* Definition for FDCANx clock resources */
#define FDCANx                      	FDCAN2
#define FDCANx_CLK_ENABLE()         	__HAL_RCC_FDCAN_CLK_ENABLE()
#define FDCANx_RX_GPIO_CLK_ENABLE() 	__HAL_RCC_GPIOA_CLK_ENABLE()
#define FDCANx_TX_GPIO_CLK_ENABLE() 	__HAL_RCC_GPIOC_CLK_ENABLE()

#define FDCANx_FORCE_RESET()   			__HAL_RCC_FDCAN_FORCE_RESET()
#define FDCANx_RELEASE_RESET() 			__HAL_RCC_FDCAN_RELEASE_RESET()

/* Definition for FDCANx Pins */
#define FDCANx_TX_PIN       			GPIO_PIN_3
#define FDCANx_TX_GPIO_PORT 			GPIOC
#define FDCANx_TX_AF        			GPIO_AF7_FDCAN2
#define FDCANx_RX_PIN       			GPIO_PIN_5
#define FDCANx_RX_GPIO_PORT 			GPIOA
#define FDCANx_RX_AF        			GPIO_AF7_FDCAN2

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */
/* Private defines ------------------------------------------------------------*/

#define DEFAULT_IRQ_PRIO 1U
/* ########################## Assert Selection ############################## */
/**
  * @brief Uncomment the line below to expanse the "assert_param" macro in the
  *        HAL drivers code
  */
/* #define USE_FULL_ASSERT    1U */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
 extern "C" {
#endif
#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

