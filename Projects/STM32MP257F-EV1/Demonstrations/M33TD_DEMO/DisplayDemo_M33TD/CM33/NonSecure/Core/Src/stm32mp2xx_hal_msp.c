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
void HAL_I2C_MspPostInit(I2C_HandleTypeDef* hi2c);
void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c);
void HAL_I2C_MspDeInit(I2C_HandleTypeDef* hi2c);
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

void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c)
{
  if(I2C8 == hi2c->Instance)
  {
    /* USER CODE BEGIN I2C8_MspInit 0 */
    /* USER CODE END I2C8_MspInit 0 */

	__HAL_RCC_I2C8_CLK_ENABLE();
	__HAL_RCC_I2C8_FORCE_RESET();
	__HAL_RCC_I2C8_RELEASE_RESET();

    /* I2C8 interrupt Init */
    HAL_NVIC_SetPriority(I2C8_IRQn, DEFAULT_IRQ_PRIO, 0);
    HAL_NVIC_EnableIRQ(I2C8_IRQn);


  }

}

void HAL_I2C_MspPostInit(I2C_HandleTypeDef* hi2c)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	/* USER CODE BEGIN I2C_MspPostInit 0 */

	/* USER CODE END I2C_MspPostInit 0 */
	if(I2C8 == hi2c->Instance)
	{
	    GPIO_InitStruct.Pin = GPIO_PIN_3 | GPIO_PIN_4;
	    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
	    GPIO_InitStruct.Pull = GPIO_NOPULL;
	    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
	    GPIO_InitStruct.Alternate = GPIO_AF8_I2C8;
	    HAL_GPIO_Init(GPIOZ, &GPIO_InitStruct);
	}
}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef* hi2c)
{
  /* USER CODE BEGIN I2C8_MspDeInit 0 */

  /* USER CODE END I2C8_MspDeInit 0 */
  if(I2C8 == hi2c->Instance)
  {
	/* Peripheral clock disable */
  __HAL_RCC_I2C8_CLK_DISABLE();

    /**I2C8 GPIO Configuration
    PZ3      ------> I2C8_SDA
    PZ4      ------> I2C8_SCL
    */
    HAL_GPIO_DeInit(GPIOZ, GPIO_PIN_3 | GPIO_PIN_4);

    /* I2C8 DMA DeInit */
    HAL_DMA_DeInit(hi2c->hdmatx);
    HAL_DMA_DeInit(hi2c->hdmarx);

    /* I2C8 interrupt DeInit */
    HAL_NVIC_DisableIRQ(I2C8_IRQn);

    /* DMA interrupt DeInit */
    HAL_NVIC_DisableIRQ(HPDMA3_Channel2_IRQn);
    HAL_NVIC_DisableIRQ(HPDMA3_Channel3_IRQn);

    /* USER CODE BEGIN I2C8_MspDeInit 1 */
    /* USER CODE END I2C8_MspDeInit 1 */
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
