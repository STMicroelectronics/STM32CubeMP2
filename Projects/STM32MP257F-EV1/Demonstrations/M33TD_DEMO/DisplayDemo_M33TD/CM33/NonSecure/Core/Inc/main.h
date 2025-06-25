/**
  ******************************************************************************
  * @file    /NonSecure/Inc/main.h
  * @author  MCD Application Team
  * @brief   Header for main.c module
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

#ifndef MAIN_H
#define MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32mp2xx_hal.h"
#include "stm32mp257f_eval.h"
#include "tfm_ns_interface.h"
#include "tfm_platform_api.h"
#include "tfm_ioctl_cpu_api.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"

/* Exported types ------------------------------------------------------------*/
/**
  * @brief  Exported types for the main module.
  */

/* Exported constants --------------------------------------------------------*/
/**
  * @brief  Default interrupt priority.
  */
#define DEFAULT_IRQ_PRIO      1U

/* Exported macro ------------------------------------------------------------*/
/**
  * @brief  Macro definitions for the main module.
  */

/* Exported functions ------------------------------------------------------- */
/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void);

/**
  * @brief  Display the current RTC time and date.
  * @param  showDate: Pointer to the buffer to store the date string.
  * @param  showTime: Pointer to the buffer to store the time string.
  * @retval None
  */
void RTC_TimeShow(char *showDate, char *showTime);

/**
  * @brief  De-initialize EXTI2 peripheral.
  * @retval None
  */
void MX_EXTI2_DeInit(void);

/**
  * @brief  De-initialize I2C8 peripheral.
  * @retval None
  */
void MX_I2C8_DeInit(void);

#endif /* MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
