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

void HAL_TIM_OnePulse_MspInit(TIM_HandleTypeDef* htim_base)
{
  /* USER CODE BEGIN TIM12_MspInit 0 */
  /* USER CODE END TIM12_MspInit 0 */
  if(htim_base->Instance==TIM12)
  {
    /* TIM12 clock enable */
	__HAL_RCC_TIM12_CLK_ENABLE();

  }

  /* USER CODE BEGIN TIM12_MspInit 1 */

  /* USER CODE END TIM12_MspInit 1 */

}

void HAL_TIM_MspPostInit(TIM_HandleTypeDef* htim)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	if(htim->Instance==TIM12)
	{
		/* USER CODE BEGIN TIM8_MspPostInit 0 */

	    /* USER CODE END TIM8_MspPostInit 0 */
		/**TIM12 GPIO Configuration
		PB11    ------> TIM12_CH2
		PB7     ------> TIM12_CH1
		*/
		GPIO_InitStruct.Pin = GPIO_PIN_7 | GPIO_PIN_11;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_PULLDOWN;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF9_TIM12;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	}
}

void HAL_TIM_OnePulse_MspDeInit(TIM_HandleTypeDef* htim)
{
  /* USER CODE BEGIN TIM12_MspDeInit 0 */

  if(htim->Instance==TIM12)
  {
    /* Peripheral clock disable */
    __HAL_RCC_TIM12_CLK_DISABLE();

    /**TIM12 GPIO Configuration
    PB11    ------> TIM12_CH2
    PB7     ------> TIM12_CH1
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_7 | GPIO_PIN_11);
  }
  /* USER CODE END TIM12_MspDeInit 0 */

  /* USER CODE BEGIN TIM12_MspDeInit 1 */

  /* USER CODE END TIM12_MspDeInit 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/**
  * @}
  */

/**
  * @}
  */

