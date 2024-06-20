/**
  ******************************************************************************
  * @file    usbpd_ucsi_ppm.c
  * @author  MCD Application Team
  * @brief   Abstraction layer between PPM and UCSI
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

/* Includes ------------------------------------------------------------------*/
#include "usbpd_ucsi.h"
#include "ppm.h"
#if defined(_TRACE)
#include "usbpd_trace.h"
#include "string.h"
#include "stdio.h"
#endif /* _TRACE */

/** @addtogroup STM32_USBPD_LIBRARY
  * @{
  */

/** @addtogroup USBPD_CORE
  * @{
  */

/** @addtogroup USBPD_CORE_UCSI
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
#if defined(_TRACE)
#define UCSI_PPM_DEBUG(__MSG__) USBPD_TRACE_Add(USBPD_TRACE_DEBUG, USBPD_PORT_0, 0U, \
                                                (uint8_t*)(__MSG__), sizeof(__MSG__));

#define UCSI_PPM_DEBUG_WITH_ARG(...)                                           \
  do                                                                           \
  {                                                                            \
    char _str[70U];                                                            \
    (void)snprintf(_str, 70U, __VA_ARGS__);                                    \
    USBPD_TRACE_Add(USBPD_TRACE_DEBUG, USBPD_PORT_0, 0U, (uint8_t*)_str, strlen(_str));      \
  } while(0);

#define UCSI_PPM_TRACE(_ID_, _PTR_DATA_, _SIZE_)                               \
  do                                                                           \
  {                                                                            \
    USBPD_TRACE_Add(USBPD_TRACE_UCSI, USBPD_PORT_0, (uint8_t)_ID_, _PTR_DATA_, _SIZE_);      \
  } while(0);

#else
#define UCSI_PPM_DEBUG(__MSG__)
#define UCSI_PPM_DEBUG_WITH_ARG(...)
#define UCSI_PPM_TRACE(_ID_, _PTR_DATA_, _SIZE_)
#endif /* _TRACE */
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/** @defgroup USBPD_CORE_UCSI_Private_Functions
  * @{
  */

static void         UCSI_ReceiveOPMData(uint8_t Register, uint8_t *PtrData);

/**
  * @}
  */

/** @defgroup USBPD_CORE_UCSI_Exported_Functions USBPD CORE UCSI Exported Functions
  * @{
  */

/**
  * @brief  Deinitialize UCSI
  * @retval USBPD status
  */
void USBPD_UCSI_DeInit(void)
{
  USBPD_PPM_DeInit();
}

/**
  * @brief  Initialize UCSI
  * @retval USBPD status
  */
#if defined(USBPD_THREADX)
USBPD_StatusTypeDef USBPD_UCSI_Init(void *MemoryPtr)
#else
USBPD_StatusTypeDef USBPD_UCSI_Init(void)
#endif /* USBPD_THREADX */
{
  USBPD_StatusTypeDef _retr = USBPD_OK;

#if defined(USBPD_THREADX)
  if (USBPD_UCSI_OK != USBPD_UcsiDriver.Init(UCSI_ReceiveOPMData, MemoryPtr))
#else
  if (USBPD_UCSI_OK != USBPD_UcsiDriver.Init(UCSI_ReceiveOPMData))
#endif /* USBPD_THREADX */
  {
    _retr = USBPD_ERROR;
  }

  /* Initialize USB-PD capability*/
#if defined(USBPD_THREADX)
  if (USBPD_OK != USBPD_PPM_Init(MemoryPtr))
#else
  if (USBPD_OK != USBPD_PPM_Init())
#endif /* USBPD_THREADX */
  {
    _retr = USBPD_ERROR;
  }

  return _retr;
}

static void UCSI_ReceiveOPMData(uint8_t Register, uint8_t *PtrData)
{
  /* Prevent unused argument compilation warning */
  (void)PtrData;

  /* Analyse the action to execute */
  switch ((UCSI_REG_t)Register)
  {
    case UcsiRegDataStructureControl:
      USBPD_PPM_PostCommand();
      break;
    case UcsiRegDataStructureMessageOut:
      break;
    case UcsiRegDataStructureReserved:
    case UcsiRegDataStructureVersion:
    case UcsiRegDataStructureCci:
    case UcsiRegDataStructureMessageIn:
    default:
      UCSI_PPM_TRACE(UCSI_TRACE_UCSI_PPM_RECEIVE_DATA, NULL, 0U);
      break;
  }
}

/**
  * @brief  Send a alert to PPM
  * @param  PortNumber Port Number
  * @retval None
  */
void USBPD_UCSI_SendNotification(uint32_t PortNumber)
{
  /* Prevent unused argument compilation warning */
  (void) PortNumber;

  /* Generate the alert signal */
  USBPD_UcsiDriver.AlertON();
}

/**
  * @brief  Clear a alert to PPM
  * @param  PortNumber Port Number
  * @retval None
  */
void  USBPD_UCSI_ClearAlert(uint32_t PortNumber)
{
  /* Prevent unused argument compilation warning */
  (void) PortNumber;

  /* Clear the alert signal */
  USBPD_UcsiDriver.AlertOFF();
}

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
