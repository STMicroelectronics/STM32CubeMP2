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
#include "cmsis_os2.h"
#include "FreeRTOSConfig.h"

/* Exported constants --------------------------------------------------------*/
#define DEFAULT_IRQ_PRIO      1U

/* Exported variables --------------------------------------------------------*/
extern I2C_HandleTypeDef I2cHandle;

/* Exported functions ------------------------------------------------------- */
void Error_Handler(void);
void App_ErrorHandler(void);
void SystemClock_Config(void);
void MX_I2C8_Init(void);
void MX_I2C8_DeInit(void);
void MX_I2C_WriteCommand(uint8_t byte);
void MX_I2C_WriteData(uint8_t *buffer, size_t buff_size);

#endif /* MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
