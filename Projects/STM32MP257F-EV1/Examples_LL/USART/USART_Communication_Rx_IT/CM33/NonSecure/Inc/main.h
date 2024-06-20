/**
  ******************************************************************************
  * @file    main.h
  * @author  MCD Application Team
  * @brief   Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "res_mgr.h"
#include "stm32mp2xx_hal_conf.h"
#include "stm32mp2xx_ll_bus.h"
#include "stm32mp2xx_ll_rcc.h"
#include "stm32mp2xx_ll_pwr.h"
#include "stm32mp2xx_ll_system.h"
#include "stm32mp2xx_ll_gpio.h"
#include "stm32mp2xx_ll_exti.h"
#include "stm32mp2xx_ll_usart.h"

/* USER CODE BEGIN Includes */
/* USER CODE END Includes */
/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
/**
  * @brief LED1
  */

#define LED3_PIN                           		LL_GPIO_PIN_6
#define LED3_GPIO_PORT                     		GPIOJ
#define LED3_GPIO_CLK_ENABLE()             		LL_RCC_GPIOJ_EnableClock()

/**
  * @brief Toggle periods for various blinking modes
  */

#define LED_BLINK_FAST  						200
#define LED_BLINK_SLOW  						500
#define LED_BLINK_ERROR 						1000

/**
  * @brief Key push-button
  */

#define USER2_BUTTON_PIN                        LL_GPIO_PIN_8
#define USER2_BUTTON_GPIO_PORT                  GPIOG
#define USER2_BUTTON_GPIO_CLK_ENABLE()			LL_RCC_GPIOG_EnableClock()
#define USER2_BUTTON_EXTI_LINE                  LL_EXTI2_LINE_8
#define USER2_BUTTON_EXTI_IRQn                  EXTI2_8_IRQn
#define USER2_BUTTON_EXTI_LINE_ENABLE()         LL_C2_EXTI_EnableIT_0_31(EXTI2, USER2_BUTTON_EXTI_LINE)
#define USER2_BUTTON_EXTI_FALLING_TRIG_ENABLE() LL_EXTI_EnableFallingTrig_0_31(EXTI2, USER2_BUTTON_EXTI_LINE)
#define USER2_BUTTON_IRQHANDLER                 EXTI2_8_IRQHandler

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);
/* IRQ Handler treatment.*/
void UserButton_Callback(void);

void USART_CharReception_Callback(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */
/* Private defines ------------------------------------------------------------*/
/* ########################## Assert Selection ############################## */
/**
  * @brief Uncomment the line below to expanse the "assert_param" macro in the 
  *        HAL drivers code
  */
/* #define USE_FULL_ASSERT    1U */
#define DEFAULT_IRQ_PRIO 1U
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
 extern "C" {
#endif
#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

