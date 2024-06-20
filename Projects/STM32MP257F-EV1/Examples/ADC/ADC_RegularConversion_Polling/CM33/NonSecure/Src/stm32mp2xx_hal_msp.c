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

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */
/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN Define */

/* USER CODE END Define */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN Macro */

/* USER CODE END Macro */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* External functions ------------------------------------------------------- */

/* USER CODE BEGIN ExternalFunctions */

/* USER CODE END ExternalFunctions */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */
/**
  * Initializes the Global MSP.
  */

/**
  * @brief ADC MSP Initialization
  *        This function configures the hardware resources used in this example:
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration
  * @param hadc: ADC handle pointer
  * @retval None
  */
void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
  GPIO_InitTypeDef          GPIO_InitStruct;

  if (ADC3 == hadc->Instance)
  {
    /*## Configure peripheral GPIO ##########################################*/
    /* ADC Channel GPIO pin configuration */
  	/* Select ADC kernel source clock */
  	__HAL_RCC_ADC3KERCLK_SETSOURCE(RCC_ADC3KERCLKSOURCE_CK_ICN_LS_MCU);
  	/* ADC Periph clock enable */
  	__HAL_RCC_ADC3_CLK_ENABLE();

    __HAL_RCC_ADC3_FORCE_RESET();
    __HAL_RCC_ADC3_RELEASE_RESET();

    GPIO_InitStruct.Pin = GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
  }
}

/**
  * @brief ADC MSP De-Initialization
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  *          - Revert GPIO to their default state
  * @param hadc: ADC handle pointer
  * @retval None
  */
void HAL_ADC_MspDeInit(ADC_HandleTypeDef *hadc)
{
  if (ADC3 == hadc->Instance)
  {
	/* setting clock source to default value */
    __HAL_RCC_ADC3KERCLK_SETSOURCE(RCC_ADC3KERCLKSOURCE_CK_KER_ADC3);
    /* Peripheral clock disable */

    __HAL_RCC_ADC3_CLK_DISABLE();

    /*##-2- Disable peripherals and GPIO Clocks ################################*/
    /* De-initialize the ADC Channel GPIO pin */
    HAL_GPIO_DeInit(GPIOF, GPIO_PIN_11);
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
