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

/**
  * @brief I3C MSP Initialization
  * This function configures the hardware resources used in this example
  * @param hi3c: I3C handle pointer
  * @retval None
  */
void HAL_I3C_MspInit(I3C_HandleTypeDef *hi3c)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  if (hi3c->Instance == I3C1)
  {
    /* USER CODE BEGIN I3C1_MspInit 0 */

    /* USER CODE END I3C1_MspInit 0 */

    __HAL_RCC_I3C1_CLK_ENABLE();
    __HAL_RCC_I3C1_FORCE_RESET();
    __HAL_RCC_I3C1_RELEASE_RESET();

    /* I3C1 interrupt Init */
    HAL_NVIC_SetPriority(I3C1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(I3C1_IRQn);

    /* Add a delay to let startup of High level on the Bus */
    HAL_Delay(1);

    /* Then set the pull at default value for I3C communication, mean no pull */
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_13;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
  }
  /* USER CODE BEGIN I3C1_MspInit 1 */
  /* USER CODE END I3C1_MspInit 1 */

}

void HAL_I3C_MspPostInit(I3C_HandleTypeDef *hi3c)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  /* USER CODE BEGIN I2C_MspPostInit 0 */

  /* USER CODE END I2C_MspPostInit 0 */
  if (I3C1 == hi3c->Instance)
  {
    /**I3C1 GPIO Configuration
    PG13    ------> I3C1_SCL
    PA2     ------> I3C1_SDA
      */
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF8_I3C1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_I3C1;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
  }
}

/**
  * @brief I3C MSP De-Initialization
  * This function freeze the hardware resources used in this example
  * @param hi3c: I3C handle pointer
  * @retval None
  */
void HAL_I3C_MspDeInit(I3C_HandleTypeDef *hi3c)
{
  if (I3C1 == hi3c->Instance)
  {
    /* USER CODE BEGIN I3C1_MspDeInit 0 */

    /* USER CODE END I3C1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_I3C1_CLK_DISABLE();

    /**I3C1 GPIO Configuration
    PG13     ------> I3C4_SCL
    PA2     ------> I3C4_SDA
      */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2);
    HAL_GPIO_DeInit(GPIOG, GPIO_PIN_13);

    /* I3C1 interrupt DeInit */
    HAL_NVIC_DisableIRQ(I3C1_IRQn);

    /* USER CODE BEGIN I3C1_MspDeInit 1 */
    /* USER CODE END I3C1_MspDeInit 1 */
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

