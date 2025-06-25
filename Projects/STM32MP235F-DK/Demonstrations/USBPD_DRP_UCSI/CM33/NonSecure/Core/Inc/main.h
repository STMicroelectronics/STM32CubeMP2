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
#include "FreeRTOSConfig.h"
#include "cmsis_os2.h"
#include "stm32mp2xx_hal.h"

#include "stm32mp2xx_ll_dma.h"
#include "stm32mp2xx_ll_ucpd.h"
#include "stm32mp2xx_ll_usart.h"
#include "stm32mp2xx_ll_rcc.h"
#include "stm32mp2xx_ll_bus.h"
#include "stm32mp2xx_ll_cortex.h"
#include "stm32mp2xx_ll_pwr.h"
#include "stm32mp2xx_ll_gpio.h"
#include "stm32mp2xx_ll_exti.h"

/* Private includes ----------------------------------------------------------*/
#include "stm32mp235f_disco.h"
#include "stm32mp235f_disco_pmic.h"
#include "stm32mp235f_disco_bus.h"

#include "openamp.h"
#include "copro_sync.h"
#include "openamp_log.h"
#include "scmi.h"
#include "mbox_scmi.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void Error_Handler(void);
void MX_UCPD1_Deinit(void);
void MX_HPDMA3_Deinit(void);
void MX_IPCC_DeInit(void);

/* Private defines -----------------------------------------------------------*/
#define DEFAULT_IRQ_PRIO      5U

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
