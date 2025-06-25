/**
  ******************************************************************************
  * @file    stm32mp2xx_hal_msp.c
  * @author  MCD Application Team
  * @brief   This file provides code for the MSP Initialization 
  *                      and de-Initialization codes.
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

void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c)
{
  if(I2C1 == hi2c->Instance)
  {
    /* USER CODE BEGIN I2C1_MspInit 0 */
    /* USER CODE END I2C1_MspInit 0 */

	__HAL_RCC_I2C1_CLK_ENABLE();
	__HAL_RCC_I2C1_FORCE_RESET();
	__HAL_RCC_I2C1_RELEASE_RESET();

    /* USER CODE BEGIN I2C1_MspInit 1 */
    /* USER CODE END I2C1_MspInit 1 */
  }

}

void HAL_I2C_MspPostInit(I2C_HandleTypeDef* hi2c)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	/* USER CODE BEGIN I2C1_MspPostInit 0 */

	/* USER CODE END I2C1_MspPostInit 0 */
	/**I2C1 GPIO Configuration
	    PA2      ------> I2C1_SDA
	    PG13      ------> I2C1_SCL
	    */
	if(I2C1 == hi2c->Instance)
	{
	    GPIO_InitStruct.Pin = GPIO_PIN_2;
	    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
	    GPIO_InitStruct.Pull = GPIO_PULLUP;
	    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	    GPIO_InitStruct.Alternate = GPIO_AF10_I2C1;
	    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	    GPIO_InitStruct.Pin = GPIO_PIN_13;
	    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
	    GPIO_InitStruct.Pull = GPIO_PULLUP;
	    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	    GPIO_InitStruct.Alternate = GPIO_AF9_I2C1;
	    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
	}
}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef* hi2c)
{
  /* USER CODE BEGIN I2C1_MspDeInit 0 */

  /* USER CODE END I2C1_MspDeInit 0 */
  if(I2C1 == hi2c->Instance)
  {
	/* Peripheral clock disable */
  __HAL_RCC_I2C1_CLK_DISABLE();

    /**I2C1 GPIO Configuration
	    PA2      ------> I2C1_SDA
	    PG13      ------> I2C1_SCL
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2);
    HAL_GPIO_DeInit(GPIOG, GPIO_PIN_13);

	/* I2C1 interrupt DeInit */
//	HAL_NVIC_DisableIRQ(I2C1_IRQn);

	/* USER CODE BEGIN I2C1_MspDeInit 1 */
	/* USER CODE END I2C1_MspDeInit 1 */
  }

}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/**
  * @}
  */

/**
  * @}
  */

