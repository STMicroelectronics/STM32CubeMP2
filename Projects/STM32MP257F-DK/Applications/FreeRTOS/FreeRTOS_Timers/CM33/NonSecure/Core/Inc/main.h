/**
  ******************************************************************************
  * @file    main.h
  * @author  MCD Application Team
  * @brief   Header for main.c file
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

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32mp2xx_hal.h"

/* Private includes ----------------------------------------------------------*/
#include "stm32mp257f_disco.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void Error_Handler(void);
void MX_UART_DeInit(void);
void HAL_DeInitTick(void);
/* Private defines -----------------------------------------------------------*/
#define DEFAULT_IRQ_PRIO      1U

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
