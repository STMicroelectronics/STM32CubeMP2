/**
  ******************************************************************************
  * @file    copro_sync.h
  * @author  MCD Application Team
  * @date    08-October-2018
  * @brief   header for the copro_sync.c file
  ******************************************************************************
  *
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __COPRO_SYNC_H__
#define __COPRO_SYNC_H__

/* Includes ------------------------------------------------------------------*/
#include "stm32mp2xx_hal.h"
/** @addtogroup Utilities
  * @{
  */

/** @addtogroup STM32_EVAL
  * @{
  */

/** @addtogroup Common
  * @{
  */

/** @addtogroup COPRO_SYNC
  * @{
  */

/** @defgroup COPRO_SYNC
  * @brief
  * @{
  */


/** @defgroup COPRO_SYNC_Exported_Defines
  * @{
  */


/**
  * @}
  */

/** @defgroup COPRO_SYNC_Exported_Types
  * @{
  */
typedef enum
{  COPROSYNC_OK       = 0x00U,
   COPROSYNC_ERROR    = 0x01U,
} CoproSync_Status_t;

/**
  * @}
  */

/** @defgroup COPRO_SYNC_Exported_Macros
  * @{
  */

/**
  * @}
  */

/** @defgroup COPRO_SYNC_Exported_Variables
  * @{
  */

/**
  * @}
  */

/** @defgroup COPRO_SYNC_Exported_FunctionsPrototype
  * @{
  */
CoproSync_Status_t CoproSync_Init(void);
CoproSync_Status_t CoproSync_DeInit(void);
void CoproSync_ShutdownCb(IPCC_HandleTypeDef * hipcc, uint32_t ChannelIndex, IPCC_CHANNELDirTypeDef ChannelDir);

/**
  * @}
  */


#endif /* __COPRO_SYNC_H__ */

/**
  * @}
  */
