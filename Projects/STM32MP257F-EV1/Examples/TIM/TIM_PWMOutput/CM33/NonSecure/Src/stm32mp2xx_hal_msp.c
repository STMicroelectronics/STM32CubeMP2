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

void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef* htim_pwm)
{

  /* USER CODE BEGIN TIM8_MspInit 0 */
  if(htim_pwm->Instance==TIM8)
  {
	/* Acquire TIM8 using Resource manager */
	if (RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIFSC, STM32MP25_RIFSC_TIM8_ID))
	{
		Error_Handler();
	}
    /* TIM8 clock enable */
	__HAL_RCC_TIM8_CLK_ENABLE();
  }
  /* USER CODE END TIM8_MspInit 0 */

  /* USER CODE BEGIN TIM8_MspInit 1 */

  /* USER CODE END TIM8_MspInit 1 */

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
	PJ5     ------> TIM8_CH1
	PJ3     ------> TIM8_CH3
	PJ4     ------> TIM8_CH4
	*/
	GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF8_TIM8;
	HAL_GPIO_Init(GPIOJ, &GPIO_InitStruct);
  }
  /* USER CODE END TIM8_MspPostInit 0 */

  /* USER CODE BEGIN TIM8_MspPostInit 1 */

  /* USER CODE END TIM8_MspPostInit 1 */
}

void HAL_TIM_PWM_MspDeInit(TIM_HandleTypeDef* htim_pwm)
{
  /* USER CODE BEGIN TIM8_MspDeInit 0 */

  if(htim_pwm->Instance==TIM8)
  {
    __HAL_RCC_TIM8_CLK_DISABLE();

    /* Peripheral clock disable */
    HAL_GPIO_DeInit(GPIOJ, (GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5));
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

