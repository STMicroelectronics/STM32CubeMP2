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
void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN TIM8_MspInit 0 */
  if(htim->Instance==TIM8)
  {
    /* TIMx Peripheral clock enable */
	  __HAL_RCC_TIM8_CLK_ENABLE();
  }

  /*##-2- Configure the NVIC for TIMx #########################################*/

  HAL_NVIC_SetPriority(TIM8_CC_IRQn, 0, 1);

  /* Enable the TIMx global Interrupt */
  HAL_NVIC_EnableIRQ(TIM8_CC_IRQn);
}

void HAL_TIM_MspPostInit(TIM_HandleTypeDef* htim)
{
  /* USER CODE BEGIN TIM8_MspPostInit 0 */
  GPIO_InitTypeDef GPIO_InitStruct;
  if(htim->Instance==TIM8)
  {
    /* USER CODE BEGIN TIM8_MspPostInit 0 */

    /* USER CODE END TIM8_MspPostInit 0 */

    /**TIM8 GPIO Configuration
      PC10     ------> TIM8_CH4
    */
    /* Configure  (TIMx_Channel) in Alternate function, push-pull and high speed */
    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF8_TIM8;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    }
    /* USER CODE END TIM8_MspPostInit 0 */

    /* USER CODE BEGIN TIM8_MspPostInit 1 */

    /* USER CODE END TIM8_MspPostInit 1 */
}

void HAL_TIM_IC_MspDeInit(TIM_HandleTypeDef* htim)
{
  /* USER CODE BEGIN TIM8_MspDeInit 0 */

  if(htim->Instance==TIM8)
  {
    /* Peripheral clock disable */
    HAL_GPIO_DeInit(GPIOC, (GPIO_PIN_10));
    __HAL_RCC_TIM8_CLK_DISABLE();

  }
  /* USER CODE END TIM8_MspDeInit 0 */

  /* USER CODE BEGIN TIM8_MspDeInit 1 */

  /* USER CODE END TIM8_MspDeInit 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/**
  * @}
  */

/**
  * @}
  */

