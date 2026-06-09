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
 * @brief  LTDC MSP Initialization
 * @param  hltdc LTDC handle pointer
 * @retval None
 *
 * This function configures the hardware resources used for LTDC:
 *   - Requests and configures GPIO pins for LTDC signals and control
 *   - Sets correct alternate functions for each pin
 *   - Handles LCD reset, display enable, and backlight control
 *   - Enables LTDC peripheral clock and reset
 *
 * Pin configuration summary (actual configuration):
 *   - GPIOA:  PA1 (R3, AF11), PA7 (B5, AF10), PA10 (R6, AF12)
 *   - GPIOB:  PB15 (R4, AF13)
 *   - GPIOC:  PC0 (G7, AF13), PC5 (DE, AF14), PC11 (R2, AF13)
 *   - GPIOF:  PF5 (B6, AF13), PF9 (backlight, GPIO), PF12 (CLK, AF13)
 *   - GPIOG:  PG2 (HSYNC, AF13), PG4 (panel enable, GPIO), PG5 (R5, AF13), PG7 (R7, AF13), PG8 (G2, AF13), PG9 (G3, AF13), PG10 (G4, AF13), PG11 (G5, AF13), PG12 (G6, AF13), PG15 (B2, AF13)
 *   - GPIOI:  PI0 (B3, AF13), PI1 (B4, AF13), PI4 (B7, AF13), PI6 (VSYNC, AF13)
 *   - LCD_RST_PIN: LCD reset (output)
 *   - LCD_DISP_CTRL_PIN: LCD display enable (output)
 *   - LCD_BL_CTRL_PIN: LCD backlight control (output, AF7_TIM2)
 */
void HAL_LTDC_MspInit(LTDC_HandleTypeDef* hltdc)
{
	GPIO_InitTypeDef GPIO_Init_Structure;
	UNUSED(hltdc);


	/* Request all needed GPIOs for LTDC */
	if (ResMgr_GPIO_Request(RESMGR_RESOURCE_RIF_GPIOA, GPIO_PIN_1 | GPIO_PIN_7 | GPIO_PIN_10) != RESMGR_STATUS_ACCESS_OK) Error_Handler();
	if (ResMgr_GPIO_Request(RESMGR_RESOURCE_RIF_GPIOF, GPIO_PIN_5 | GPIO_PIN_9 | GPIO_PIN_12) != RESMGR_STATUS_ACCESS_OK) Error_Handler();
	if (ResMgr_GPIO_Request(RESMGR_RESOURCE_RIF_GPIOG, GPIO_PIN_2 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_15) != RESMGR_STATUS_ACCESS_OK) Error_Handler();
	if (ResMgr_GPIO_Request(RESMGR_RESOURCE_RIF_GPIOI, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_6) != RESMGR_STATUS_ACCESS_OK) Error_Handler();
	if (ResMgr_GPIO_Request(RESMGR_RESOURCE_RIF_GPIOC, GPIO_PIN_0 | GPIO_PIN_5 | GPIO_PIN_11) != RESMGR_STATUS_ACCESS_OK) Error_Handler();
	if (ResMgr_GPIO_Request(RESMGR_RESOURCE_RIF_GPIOB, GPIO_PIN_15) != RESMGR_STATUS_ACCESS_OK) Error_Handler();

	/* GPIOF: PF5 (B6, AF13), PF12 (CLK, AF13) */
	GPIO_Init_Structure.Pin       = GPIO_PIN_5 | GPIO_PIN_12;
	GPIO_Init_Structure.Mode      = GPIO_MODE_AF_PP;
	GPIO_Init_Structure.Pull      = GPIO_NOPULL;
	GPIO_Init_Structure.Speed     = GPIO_SPEED_FREQ_HIGH;
	GPIO_Init_Structure.Alternate = GPIO_AF13_LCD;
	HAL_GPIO_Init(GPIOF, &GPIO_Init_Structure);

	/* GPIOG: PG2 (HSYNC), PG5 (R5), PG7 (R7), PG8 (G2), PG9 (G3), PG10 (G4), PG11 (G5), PG12 (G6), PG15 (B2) all AF13 */
	GPIO_Init_Structure.Pin = GPIO_PIN_2 | GPIO_PIN_5 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_15;
	HAL_GPIO_Init(GPIOG, &GPIO_Init_Structure);

	/* GPIOA: PA1 (R3, AF11), PA7 (B5, AF10), PA10 (R6, AF12) */
	GPIO_Init_Structure.Pin       = GPIO_PIN_1;
	GPIO_Init_Structure.Alternate = GPIO_AF11_LCD;
	HAL_GPIO_Init(GPIOA, &GPIO_Init_Structure);
	GPIO_Init_Structure.Pin       = GPIO_PIN_7;
	GPIO_Init_Structure.Alternate = GPIO_AF10_LCD;
	HAL_GPIO_Init(GPIOA, &GPIO_Init_Structure);
	GPIO_Init_Structure.Pin       = GPIO_PIN_10;
	GPIO_Init_Structure.Alternate = GPIO_AF12_LCD;
	HAL_GPIO_Init(GPIOA, &GPIO_Init_Structure);

	/* GPIOB: PB15 (R4, AF13) */
	GPIO_Init_Structure.Pin       = GPIO_PIN_15;
	GPIO_Init_Structure.Alternate = GPIO_AF13_LCD;
	HAL_GPIO_Init(GPIOB, &GPIO_Init_Structure);

	/* GPIOC: PC0 (G7, AF13), PC11 (R2, AF13), PC5 (DE, AF14) */
	GPIO_Init_Structure.Pin       = GPIO_PIN_0 | GPIO_PIN_11;
	GPIO_Init_Structure.Alternate = GPIO_AF13_LCD;
	HAL_GPIO_Init(GPIOC, &GPIO_Init_Structure);
	GPIO_Init_Structure.Pin       = GPIO_PIN_5;
	GPIO_Init_Structure.Alternate = GPIO_AF14_LCD;
	HAL_GPIO_Init(GPIOC, &GPIO_Init_Structure);

	/* GPIOI: PI0 (B3, AF13), PI1 (B4, AF13), PI4 (B7, AF13), PI6 (VSYNC, AF13) */
	GPIO_Init_Structure.Pin       = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_6;
	GPIO_Init_Structure.Alternate = GPIO_AF13_LCD;
	HAL_GPIO_Init(GPIOI, &GPIO_Init_Structure);

	/* Enable LTDC clocks and reset */
	__HAL_RCC_LTDC_CLK_ENABLE();

	__HAL_RCC_LTDC_FORCE_RESET();
	__HAL_RCC_LTDC_RELEASE_RESET();



	/* LCD display control pin */
	GPIO_Init_Structure.Mode      = GPIO_MODE_OUTPUT_PP;
	GPIO_Init_Structure.Pull      = GPIO_NOPULL;
	GPIO_Init_Structure.Speed     = GPIO_SPEED_FREQ_MEDIUM;
	GPIO_Init_Structure.Pin       = GPIO_PIN_4;
	HAL_GPIO_Init(GPIOG, &GPIO_Init_Structure);
	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_4, GPIO_PIN_SET);

	/* LCD backlight control pin (PF9, GPIO output) */
	GPIO_Init_Structure.Mode      = GPIO_MODE_OUTPUT_PP;
	GPIO_Init_Structure.Pull      = GPIO_NOPULL;
	GPIO_Init_Structure.Speed     = GPIO_SPEED_FREQ_MEDIUM;
	GPIO_Init_Structure.Alternate = GPIO_AF7_TIM2;
	GPIO_Init_Structure.Pin       = GPIO_PIN_9;
	HAL_GPIO_Init(GPIOF, &GPIO_Init_Structure);
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_9, GPIO_PIN_SET);

	ResMgr_GPIO_Release(RESMGR_RESOURCE_RIF_GPIOA, GPIO_PIN_1 | GPIO_PIN_7 | GPIO_PIN_10);
	ResMgr_GPIO_Release(RESMGR_RESOURCE_RIF_GPIOF, GPIO_PIN_5 | GPIO_PIN_9 | GPIO_PIN_12);
	ResMgr_GPIO_Release(RESMGR_RESOURCE_RIF_GPIOG, GPIO_PIN_2 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_15);
	ResMgr_GPIO_Release(RESMGR_RESOURCE_RIF_GPIOI, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_6);
	ResMgr_GPIO_Release(RESMGR_RESOURCE_RIF_GPIOC, GPIO_PIN_0 | GPIO_PIN_5 | GPIO_PIN_11);
	ResMgr_GPIO_Release(RESMGR_RESOURCE_RIF_GPIOB, GPIO_PIN_15);

}

/**
 * @brief  LTDC MSP De-Initialization
 * @param  hltdc LTDC handle pointer
 * @retval None
 *
 * This function deinitializes the hardware resources used for LTDC:
 *   - Deinitializes only the GPIO pins configured in MSP Init
 *   - Disables LTDC peripheral clock
 *
 * Pin deinitialization summary (matches MSP Init):
 *   - GPIOA:  PA1, PA7, PA10
 *   - GPIOB:  PB15
 *   - GPIOC:  PC0, PC5, PC11
 *   - GPIOF:  PF5, PF9 (backlight, GPIO), PF12
 *   - GPIOG:  PG2, PG4 (panel enable, GPIO), PG5, PG7, PG8, PG9, PG10, PG11, PG12, PG15
 *   - GPIOI:  PI0, PI1, PI4, PI6
 *   - LCD_RST_PIN, LCD_DISP_CTRL_PIN, LCD_BL_CTRL_PIN (if needed)
 */
void HAL_LTDC_MspDeInit(LTDC_HandleTypeDef* hltdc)
{
	UNUSED(hltdc);

	/* Disable LTDC block */
	__HAL_LTDC_DISABLE(hltdc);

	/* DeInit GPIOF pins: PF5 (B6), PF9 (extra), PF12 (CLK) */
	HAL_GPIO_DeInit(GPIOF, GPIO_PIN_5);
	HAL_GPIO_DeInit(GPIOF, GPIO_PIN_9);
	HAL_GPIO_DeInit(GPIOF, GPIO_PIN_12);

	/* DeInit GPIOG pins: PG2 (HSYNC), PG5 (R5), PG7 (R7), PG8 (G2), PG9 (G3), PG10 (G4), PG11 (G5), PG12 (G6), PG15 (B2) */
	HAL_GPIO_DeInit(GPIOG, GPIO_PIN_2);
	HAL_GPIO_DeInit(GPIOG, GPIO_PIN_5);
	HAL_GPIO_DeInit(GPIOG, GPIO_PIN_7);
	HAL_GPIO_DeInit(GPIOG, GPIO_PIN_8);
	HAL_GPIO_DeInit(GPIOG, GPIO_PIN_9);
	HAL_GPIO_DeInit(GPIOG, GPIO_PIN_10);
	HAL_GPIO_DeInit(GPIOG, GPIO_PIN_11);
	HAL_GPIO_DeInit(GPIOG, GPIO_PIN_12);
	HAL_GPIO_DeInit(GPIOG, GPIO_PIN_15);

	/* DeInit GPIOA pins: PA1 (R3), PA7 (B5), PA10 (R6) */
	HAL_GPIO_DeInit(GPIOA, GPIO_PIN_1);
	HAL_GPIO_DeInit(GPIOA, GPIO_PIN_7);
	HAL_GPIO_DeInit(GPIOA, GPIO_PIN_10);

	/* DeInit GPIOB pin: PB15 (R4) */
	HAL_GPIO_DeInit(GPIOB, GPIO_PIN_15);

	/* DeInit GPIOC pins: PC0 (G7), PC5 (DE), PC11 (R2) */
	HAL_GPIO_DeInit(GPIOC, GPIO_PIN_0);
	HAL_GPIO_DeInit(GPIOC, GPIO_PIN_5);
	HAL_GPIO_DeInit(GPIOC, GPIO_PIN_11);

	/* DeInit GPIOI pins: PI0 (B3), PI1 (B4), PI4 (B7), PI6 (VSYNC) */
	HAL_GPIO_DeInit(GPIOI, GPIO_PIN_0);
	HAL_GPIO_DeInit(GPIOI, GPIO_PIN_1);
	HAL_GPIO_DeInit(GPIOI, GPIO_PIN_4);
	HAL_GPIO_DeInit(GPIOI, GPIO_PIN_6);

	/* Disable LTDC clock */
	__HAL_RCC_LTDC_CLK_DISABLE();
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

