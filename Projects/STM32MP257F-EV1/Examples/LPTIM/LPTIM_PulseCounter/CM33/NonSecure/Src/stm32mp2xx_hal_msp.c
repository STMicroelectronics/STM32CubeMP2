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
extern void Error_Handler(void);

/* USER CODE BEGIN ExternalFunctions */

/* USER CODE END ExternalFunctions */

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

void HAL_LPTIM_MspInit(LPTIM_HandleTypeDef* hlptim)
{
  if(hlptim->Instance==LPTIM4)
  {
    /* Force the LPTIM Peripheral Clock Reset */
    __HAL_RCC_LPTIM4_FORCE_RESET();

    /* Release the LPTIM Peripheral Clock Reset */
    __HAL_RCC_LPTIM4_RELEASE_RESET();

    /* Enables WRITE access to the backup and D3 domain for LPTIM3, LPTIM4 and LPTIM5*/
    if(IS_DEVELOPER_BOOT_MODE())
    {
      HAL_PWR_EnableBkUpD3Access();
    }

    /* USER CODE BEGIN LPTIM4_MspInit 0 */

    /* USER CODE END LPTIM4_MspInit 0 */

    /* Enable LPTIM4 Clock */
    __HAL_RCC_LPTIM4_CLK_ENABLE();

    /* LPTIM1 interrupt Init */
    HAL_NVIC_SetPriority(LPTIM4_IRQn, DEFAULT_IRQ_PRIO, 0);
    HAL_NVIC_EnableIRQ(LPTIM4_IRQn);

  /* USER CODE BEGIN LPTIM1_MspInit 1 */

  /* USER CODE END LPTIM1_MspInit 1 */
  }

}

void HAL_LPTIM_MspPostInit(LPTIM_HandleTypeDef* hlptim)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(hlptim->Instance==LPTIM4)
  {
  /* USER CODE BEGIN LPTIM4_MspPostInit 0 */

  /* USER CODE END LPTIM4_MspPostInit 0 */
	GPIO_InitStruct.Pin = GPIO_PIN_3;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Alternate = GPIO_AF7_LPTIM4;
	HAL_GPIO_Init(GPIOZ, &GPIO_InitStruct);
  /* USER CODE BEGIN LPTIM4_MspPostInit 1 */

  /* USER CODE END LPTIM4_MspPostInit 1 */
  }
}


void HAL_LPTIM_MspDeInit(LPTIM_HandleTypeDef* hlptim)
{

  if(hlptim->Instance==LPTIM4)
  {

    /* Force the LPTIM Peripheral Clock Reset */
    __HAL_RCC_LPTIM4_FORCE_RESET();

    /* Release the LPTIM Peripheral Clock Reset */
    __HAL_RCC_LPTIM4_RELEASE_RESET();

    /* USER CODE BEGIN LPTIM1_MspDeInit 0 */

    /* USER CODE END LPTIM4_MspDeInit 0 */

    /* Peripheral clock disable */
    __HAL_RCC_LPTIM4_CLK_DISABLE();

    /**LPTIM4 GPIO Configuration
    PZ3     ------> LPTIM4_IN1
    */
    HAL_GPIO_DeInit(GPIOZ, GPIO_PIN_3);

    /* LPTIM1 interrupt DeInit */
    HAL_NVIC_DisableIRQ(LPTIM4_IRQn);
  }
  /* USER CODE BEGIN LPTIM4_MspDeInit 1 */

  /* USER CODE END LPTIM4_MspDeInit 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/**
  * @}
  */

/**
  * @}
  */
