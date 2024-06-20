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
#include <stdio.h>
#include "stm32mp2xx_hal.h"
#include "stm32mp257f_disco.h"
#include "openamp.h"
#include "openamp_log.h"
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define DEFAULT_IRQ_PRIO      1U
/* Exported macro ------------------------------------------------------------*/
#if defined(NO_PRINTF)
  #define loc_printf(...)
#else
  #define loc_printf(mess, ...)   printf(mess, ##__VA_ARGS__)
#endif

/* Exported variables --------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void Error_Handler(void);
/* Private defines -----------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
