/**
  ******************************************************************************
  * @file  : usbpd_ucsi_if_template.h
  * @brief : Header file for usbpd_ucsi interface file.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef USBPD_UCSI_IF_H__
#define USBPD_UCSI_IF_H__

#include "main.h"
#include "usbpd_ucsi.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup UCSI_IF low layer interface of UCSI
  * @{
  */

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Definition for I2Cx's NVIC */
#define UCSI_I2C_IRQ_N                       I2C1_IRQn
#define UCSI_I2C_IRQ_HANDLER                 I2C1_IRQHandler
/* External variables --------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

/**
  * @}
  */

#endif /* USBPD_UCSI_IF_H__*/
