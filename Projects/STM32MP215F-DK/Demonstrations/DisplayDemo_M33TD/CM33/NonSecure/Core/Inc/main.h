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
#include "stm32mp215f_disco.h"
#include "cmsis_os2.h"
#include "FreeRTOSConfig.h"

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
  * @brief  Initialize I3C1 peripheral used by the OLED panel.
  * @retval None
  */
void MX_I3C1_Init(void);

/**
  * @brief  De-initialize I3C1 peripheral.
  * @retval None
  */
void MX_I3C1_DeInit(void);

/**
  * @brief  Write an SSD1306 command on the I3C/I2C bus.
  * @param  byte Command byte.
  * @retval None
  */
void MX_I3C_WriteCommand(uint8_t byte);

/**
  * @brief  Write SSD1306 payload bytes on the I3C/I2C bus.
  * @param  buffer Payload buffer.
  * @param  buff_size Payload size.
  * @retval None
  */
void MX_I3C_WriteData(uint8_t *buffer, size_t buff_size);

#endif /* MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
