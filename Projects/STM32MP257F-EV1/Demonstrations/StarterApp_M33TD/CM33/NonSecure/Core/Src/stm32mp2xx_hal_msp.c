/**
  ******************************************************************************
  * @file    stm32mp2xx_hal_msp.c
  * @author  MCD Application Team
  * @brief   HAL MSP module.
  *          This file template is located in the HAL folder and should be copied
  *          to the user folder.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32mp2xx_hal.h"

/** @addtogroup STM32MP2xx_HAL_Driver
  * @{
  */

/** @defgroup HAL_MSP HAL MSP module driver
  * @brief HAL MSP module.
  * @{
  */
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup HAL_MSP_Private_Functions
  * @{
  */

/**
  * @brief  Initialize the Global MSP.
  * @retval None
  */
void HAL_MspInit(void)
{
}


/**
  * @brief  DeInitialize the Global MSP.
  * @retval None
  */
void HAL_MspDeInit(void)
{
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{

	if(htim->Instance == TIM6)
	{
    /* Enable TIM6 clock */
    __HAL_RCC_TIM6_CLK_ENABLE();
	}

}

/**
  * @brief  This function is used to de initialization of low level configuration
  * @retval None
  */
void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef *htim)
{

	if(htim->Instance == TIM6)
	{
    /* Disable TIM6 clock */
    __HAL_RCC_TIM6_CLK_DISABLE();
	}

}

/**
  * @brief  Initialize the PPP MSP.
  * @retval None
  */
/*
void HAL_PPP_MspInit(void)
{
}
*/

/**
  * @brief  DeInitialize the PPP MSP.
  * @retval None
  */
/*
void HAL_PPP_MspDeInit(void)
{
}
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
