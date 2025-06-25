/**
  ******************************************************************************
  * @file    stm32mp2xx_hal_msp.c
  * @author  MCD Application Team
  * @brief   HAL MSP module.
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
#include "res_mgr.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/


/** @defgroup HAL_MSP_Private_Functions
  * @{
  */

/**
  * @brief  Initializes the MSP.
  * @retval None
  */
void HAL_MspInit(void)
{

}

/**
  * @brief  This function is used to configure the low Level Initialization required for TIM BASE
  * @retval None
  */
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
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
