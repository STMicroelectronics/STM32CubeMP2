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

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32mp2xx_hal.h"

/* USER CODE BEGIN Includes */
#include "stm32mp215f_disco.h"
#include "stm32mp215f_disco_stpmic2l.h"
#include "scmi.h"
#include "mbox_scmi.h"

/* USER CODE END Includes */
/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */
/* Private defines ------------------------------------------------------------*/
#define DEFAULT_IRQ_PRIO      1U

/* Definition for ADCx's Oversampling parameters */
#define OVERSAMPLING_RATIO              1023                               /* 1024-oversampling */
#define RIGHTBITSHIFT                   ADC_RIGHTBITSHIFT_6                /* 6-bit right shift of the oversampled summation */
#define TRIGGEREDMODE                   ADC_TRIGGEREDMODE_SINGLE_TRIGGER   /* A single trigger for all channel oversampled conversions */
#define OVERSAMPLINGSTOPRESET           ADC_REGOVERSAMPLING_CONTINUED_MODE /* Oversampling buffer maintained during injection sequence */

/* Compute the prescaler value to have TIMx counter clock equal to 10000 Hz */
#define PRESCALER_VALUE (uint32_t)((SystemCoreClock / 10000) - 1)
#define PERIOD_VALUE    (20000 - 1);

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

