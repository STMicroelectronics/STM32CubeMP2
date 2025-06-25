/**
  ******************************************************************************
  * @file    stm32mp2xx_hal_msp.c
  * @author  MCD Application Team
  * @brief   This file provides code for the MSP Initialization 
  *                      and de-Initialization codes.
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "res_mgr.h"

extern void Error_Handler(void);

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */
/**
  * Initializes the Global MSP.
  */
void HAL_MspInit(void)
{
  /* USER CODE BEGIN MspInit 0 */

  /* USER CODE END MspInit 0 */

  /* USER CODE BEGIN MspInit 1 */

  /* USER CODE END MspInit 1 */
}

/**
  * @brief TIM MSP Initialization
  *        This function configures the hardware resources used in this example:
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration
  * @param htim: TIM handle pointer
  * @retval None
  */
void HAL_TIM_OnePulse_MspInit(TIM_HandleTypeDef* htim_base)
{
  /* USER CODE BEGIN TIM5_MspInit 0 */
  /* USER CODE END TIM5_MspInit 0 */
  if(htim_base->Instance==TIM5)
  {
    /* TIM5 clock enable */
	__HAL_RCC_TIM5_CLK_ENABLE();

  }

  /* USER CODE BEGIN TIM5_MspInit 1 */

  /* USER CODE END TIM5_MspInit 1 */

}

void HAL_TIM_MspPostInit(TIM_HandleTypeDef* htim)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	if(htim->Instance==TIM5)
	{
		/* USER CODE BEGIN TIM5_MspPostInit 0 */

	    /* USER CODE END TIM5_MspPostInit 0 */
		/**TIM5 GPIO Configuration
		PH8    ------> TIM5_CH1
		PI5    ------> TIM5_CH2
		*/
		GPIO_InitStruct.Pin = GPIO_PIN_8;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_PULLDOWN;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF9_TIM5;
		HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = GPIO_PIN_5;
		GPIO_InitStruct.Alternate = GPIO_AF8_TIM5;
		HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);
	}
}

void HAL_TIM_OnePulse_MspDeInit(TIM_HandleTypeDef* htim)
{
  /* USER CODE BEGIN TIM5_MspDeInit 0 */

  if(htim->Instance==TIM5)
  {
    /* Peripheral clock disable */
    __HAL_RCC_TIM5_CLK_DISABLE();

    /**TIM5 GPIO Configuration
    PH8    ------> TIM5_CH1
    PI5     ------> TIM5_CH2
    */
    HAL_GPIO_DeInit(GPIOH, GPIO_PIN_8);
    HAL_GPIO_DeInit(GPIOI, GPIO_PIN_5);
  }
  /* USER CODE END TIM5_MspDeInit 0 */

  /* USER CODE BEGIN TIM5_MspDeInit 1 */

  /* USER CODE END TIM5_MspDeInit 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/**
  * @}
  */

/**
  * @}
  */

