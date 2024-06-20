/**
  ******************************************************************************
  * @file    stm32mp2xx_hal_pcd_ex.c
  * @author  MCD Application Team
  * @brief   PCD Extended HAL module driver.
  *          This file provides firmware functions to manage the following
  *          functionalities of the USB Peripheral Controller:
  *           + Extended features functions
  *
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32mp2xx_hal.h"

/** @addtogroup STM32MP2xx_HAL_Driver
  * @{
  */

/** @defgroup PCDEx PCDEx
  * @brief PCD Extended HAL module driver
  * @{
  */

#ifdef HAL_PCD_MODULE_ENABLED

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/** @defgroup PCDEx_Exported_Functions PCDEx Exported Functions
  * @{
  */

/** @defgroup PCDEx_Exported_Functions_Group1 Peripheral Control functions
  * @brief    PCDEx control functions
  *
@verbatim
 ===============================================================================
                 ##### Extended features functions #####
 ===============================================================================
    [..]  This section provides functions allowing to:
      (+) Update FIFO configuration

@endverbatim
  * @{
  */

/**
  * @brief  Set Tx FIFO
  * @param  hpcd PCD handle
  * @param  fifo The number of Tx fifo
  * @param  size Fifo size
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCDEx_SetTxFiFo(const PCD_HandleTypeDef *hpcd, uint8_t fifo, uint16_t size)
{
  UNUSED(hpcd);
  UNUSED(fifo);
  UNUSED(size);

  return HAL_OK;
}

/**
  * @brief  Set Rx FIFO
  * @param  hpcd PCD handle
  * @param  size Size of Rx fifo
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCDEx_SetRxFiFo(const PCD_HandleTypeDef *hpcd, uint16_t size)
{
  UNUSED(hpcd);
  UNUSED(size);

  return HAL_OK;
}


/**
  * @}
  */

/**
  * @}
  */

#endif /* HAL_PCD_MODULE_ENABLED */
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
