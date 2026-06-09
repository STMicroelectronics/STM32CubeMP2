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

#if defined(DISPLAY_PANEL_ENABLED)
/**
  * @brief  LVDS MSP Initialization
  * @param  hlvds LVDS handle pointer
  * @retval None
  */
void HAL_LVDS_MspInit(LVDS_HandleTypeDef* hlvds)
{

	GPIO_InitTypeDef GPIO_Init_Structure;

	/* Enable GPIOs clock */
	if(ResMgr_Request(RESMGR_RESOURCE_RIFSC, RESMGR_RCC_RESOURCE(96)) == RESMGR_STATUS_ACCESS_OK)
	{
		__HAL_RCC_GPIOG_CLK_ENABLE();
	}

	if(ResMgr_Request(RESMGR_RESOURCE_RIFSC, RESMGR_RCC_RESOURCE(98)) == RESMGR_STATUS_ACCESS_OK)
	{
		__HAL_RCC_GPIOI_CLK_ENABLE();
	}


	/* Requests access to GPIOG pin 15 and GPIOI pin 5 */

	if(ResMgr_GPIO_Request(RESMGR_RESOURCE_RIF_GPIOG, GPIO_PIN_15) != RESMGR_STATUS_ACCESS_OK)
	{
	   Error_Handler();
	}

	if(ResMgr_GPIO_Request(RESMGR_RESOURCE_RIF_GPIOI, GPIO_PIN_5) != RESMGR_STATUS_ACCESS_OK)
	{
		Error_Handler();
	}


	/* Common GPIO configuration */
	GPIO_Init_Structure.Mode      = GPIO_MODE_OUTPUT_PP;
	GPIO_Init_Structure.Pull      = GPIO_NOPULL;
	GPIO_Init_Structure.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;

	/**
	  *   GPIO configuration for LVDS Panel control signals
	  *   - GPIOG Pin 15
	  *   - GPIOI Pin 5
	  */

	GPIO_Init_Structure.Pin   = GPIO_PIN_15;
	HAL_GPIO_Init(GPIOG, &GPIO_Init_Structure);
	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_15, GPIO_PIN_SET);

	GPIO_Init_Structure.Pin   = GPIO_PIN_5;
	HAL_GPIO_Init(GPIOI, &GPIO_Init_Structure);
	HAL_GPIO_WritePin(GPIOI, GPIO_PIN_5, GPIO_PIN_SET);

	/* minimum required delay of 30ms after panel reset*/
	HAL_Delay(30);

	/* Enable the LTDC Clock */
	__HAL_RCC_LTDC_CLK_ENABLE();

	/* Enable LTDC reset state */
	__HAL_RCC_LTDC_FORCE_RESET();

	/* Release LTDC from reset state */
	__HAL_RCC_LTDC_RELEASE_RESET();

	/* Enable the LVDS module */
	__HAL_RCC_LVDS_CLK_ENABLE();

	/* Reset LVDS */

	__HAL_RCC_LVDS_FORCE_RESET();
	__HAL_RCC_LVDS_RELEASE_RESET();

    ResMgr_Release(RESMGR_RESOURCE_RIFSC, RESMGR_RCC_RESOURCE(96));
	  ResMgr_Release(RESMGR_RESOURCE_RIFSC, RESMGR_RCC_RESOURCE(98));
	  ResMgr_GPIO_Release(RESMGR_RESOURCE_RIF_GPIOG, GPIO_PIN_15);
	  ResMgr_GPIO_Release(RESMGR_RESOURCE_RIF_GPIOI, GPIO_PIN_5);

}

/**
  * @brief  LVDS MSP De-Initialization
  * @param  hlvds LVDS handle pointer
  * @retval None
  */
void HAL_LVDS_MspDeInit(LVDS_HandleTypeDef* hlvds)
{

	__HAL_RCC_LVDS_FORCE_RESET();
	__HAL_RCC_LVDS_RELEASE_RESET();

	__HAL_RCC_LTDC_FORCE_RESET();
	__HAL_RCC_LTDC_RELEASE_RESET();
}
#endif


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

