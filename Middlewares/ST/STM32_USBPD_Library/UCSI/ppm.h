/**
  ******************************************************************************
  * @file    ppm.h
  * @author  MCD Application Team
  * @brief   PPM header file
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
#ifndef PPM_H
#define PPM_H

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup STM32_USBPD_LIBRARY
  * @{
  */

/** @addtogroup USBPD_CORE
  * @{
  */

/** @addtogroup USBPD_CORE_PPM
  * @{
  */
/* Includes ------------------------------------------------------------------*/
#include "usbpd_def.h"
#include "usbpd_ucsi.h"
#include "usbpd_os_port.h"

/* Exported macro ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/
#if defined(_TRACE)
typedef enum
{
  UCSI_TRACE_ADDRCB_WAIT_REGID,
  UCSI_TRACE_ADDRCB_CHANGE_FOR_READ,
  UCSI_TRACE_ADDRCB_ERROR_RX,
  UCSI_TRACE_ADDRCB_ERROR_TX,
  UCSI_TRACE_LISTEN_CPLT,
  UCSI_TRACE_LISTEN_PORT,
  UCSI_TRACE_RX_CPLT_WAIT_REGID,
  UCSI_TRACE_RX_CPLT_NACK,
  UCSI_TRACE_RX_CPLT_DATAEND,
  UCSI_TRACE_TX_CPLT_COMMAND,
  UCSI_TRACE_TX_CPLT_ERROR,
  UCSI_TRACE_ERROR_CPLT,
  UCSI_TRACE_USER_GET,
  UCSI_TRACE_UCSI_GET,
  UCSI_TRACE_ALERT_ON,
  UCSI_TRACE_ALERT_OFF,
  UCSI_TRACE_UCSI_PPM_RECEIVE_DATA,
  UCSI_TRACE_PPM_STATE,
  UCSI_TRACE_PPM_INIT,
  UCSI_TRACE_PPM_EVENT,
  UCSI_TRACE_PPM_GET_REG,
  UCSI_TRACE_PPM_GET_CCI,
  UCSI_TRACE_PPM_GET_REG_ERROR_READ,
  UCSI_TRACE_PPM_READY,
  UCSI_TRACE_PPM_READY_UNKNOWN,
  UCSI_TRACE_PPM_NOT_READY,
  UCSI_TRACE_UNKNOWN
} UCSI_TRACE;
#endif /* _TRACE */

/* Exported types ------------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */

/** @addtogroup USBPD_CORE_PPM_Exported_Functions
  * @{
  */

#if defined(USBPD_THREADX)
USBPD_StatusTypeDef USBPD_PPM_Init(VOID *MemoryPtr);
#else
USBPD_StatusTypeDef USBPD_PPM_Init(void);
#endif /* USBPD_THREADX */
void                USBPD_PPM_DeInit(void);
uint8_t            *USBPD_PPM_GetDataPointer(UCSI_REG_t Register);
void                USBPD_PPM_Notification(uint8_t PortNumber, uint32_t EventType);
void                USBPD_PPM_PostCommand(void);

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* PPM_H */
