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
  * @brief  Initializes the Global MSP.
  * @retval None
  */
void HAL_MspInit(void)
{
}

/**
  * @brief  DeInitializes the Global MSP.
  * @retval None
  */
void HAL_MspDeInit(void)
{
}
void HAL_I3C_MspInit(I3C_HandleTypeDef *hi3c)
{

  if (hi3c->Instance == I3C2)
  {
    /* USER CODE BEGIN I3C1_MspInit 0 */

    /* USER CODE END I3C1_MspInit 0 */

    __HAL_RCC_I3C2_CLK_ENABLE();
    __HAL_RCC_I3C2_FORCE_RESET();
    __HAL_RCC_I3C2_RELEASE_RESET();

  }
  /* USER CODE BEGIN I3C1_MspInit 1 */
  /* USER CODE END I3C1_MspInit 1 */

}

void HAL_I3C_MspPostInit(I3C_HandleTypeDef *hi3c)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  /* USER CODE BEGIN I3C_MspPostInit 0 */

  /* USER CODE END I3C_MspPostInit 0 */
  if (I3C2 == hi3c->Instance)
  {
    /**I3C1 GPIO Configuration
    PG13    ------> I3C1_SCL
    PA2     ------> I3C1_SDA
      */
    GPIO_InitStruct.Pin = GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_I3C2;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_I3C2;
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
  if (I3C2 == hi3c->Instance)
  {
    /* USER CODE BEGIN I3C1_MspDeInit 0 */

    /* USER CODE END I3C1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_I3C2_CLK_DISABLE();

    /**I3C1 GPIO Configuration
    PG13    ------> I3C1_SCL
    PA2     ------> I3C1_SDA
      */
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_12);
    HAL_GPIO_DeInit(GPIOG, GPIO_PIN_0);

    /* I3C1 DMA DeInit */
    HAL_DMA_DeInit(hi3c->hdmatx);
    HAL_DMA_DeInit(hi3c->hdmarx);
    HAL_DMA_DeInit(hi3c->hdmacr);

    /* I3C1 interrupt DeInit */
    HAL_NVIC_DisableIRQ(I3C2_IRQn);

    /* USER CODE BEGIN I3C1_MspDeInit 1 */
    /* USER CODE END I3C1_MspDeInit 1 */
  }

}
/**
  * @brief  Initializes the TIM Base MSP.
  * @param  htim TIM handle pointer
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
  * @param  htim TIM handle pointer
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
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

