/**
  ******************************************************************************
  * @file  : usbpd_ucsi_if.h
  * @brief : Header file for usbpd_ucsi interface file
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
/* Additional stack size for PPM thread, required by the use of VIRT INTC */
#define OS_PPM_STACK_ADDITIONAL_SIZE          (1024U)
/* External variables --------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void UCSI_I2C_DeInit(void);

#ifdef __cplusplus
}
#endif

/**
  * @}
  */

#endif /* USBPD_UCSI_IF_H__*/
