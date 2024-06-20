/**
  ******************************************************************************
  * @file    stm32mp2xx_hal_pcd_ex.h
  * @author  MCD Application Team
  * @brief   Header file of PCD HAL Extension module.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STM32MP2xx_HAL_PCD_EX_H
#define STM32MP2xx_HAL_PCD_EX_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "stm32mp2xx_hal_def.h"

/** @addtogroup STM32MP2xx_HAL_Driver
  * @{
  */

/** @addtogroup PCDEx
  * @{
  */
/* Exported types ------------------------------------------------------------*/


/* Exported constants --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/** @addtogroup PCDEx_Exported_Functions PCDEx Exported Functions
  * @{
  */
/** @addtogroup PCDEx_Exported_Functions_Group1 Peripheral Control functions
  * @{
  */
HAL_StatusTypeDef HAL_PCDEx_SetTxFiFo(const PCD_HandleTypeDef *hpcd, uint8_t fifo, uint16_t size);
HAL_StatusTypeDef HAL_PCDEx_SetRxFiFo(const PCD_HandleTypeDef *hpcd, uint16_t size);

/**
  * @}
  */

/**
  * @}
  */
#ifdef __cplusplus
}
#endif


#endif /* STM32MP2xx_HAL_PCD_EX_H */
/**
  * @}
  */

/**
  * @}
  */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
