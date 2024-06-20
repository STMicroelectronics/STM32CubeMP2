/**
  ******************************************************************************
  * @file    stm32mp2xx_hal_msp.c
  * @author  MCD Application Team
  * @brief   This file provides code for the MSP Initialization 
  *                      and de-Initialization codes.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
  * @brief  Initializes the FDCAN MSP.
  * @param  hfdcan: pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @retval None
  */
void HAL_FDCAN_MspInit(FDCAN_HandleTypeDef* hfdcan)
{
  GPIO_InitTypeDef  GPIO_InitStruct;

  /*##-1- Configure peripheral GPIO ##########################################*/
  /* FDCANx TX GPIO pin configuration  */
  GPIO_InitStruct.Pin       = FDCANx_TX_PIN;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = FDCANx_TX_AF;
  HAL_GPIO_Init(FDCANx_TX_GPIO_PORT, &GPIO_InitStruct);

  /* FDCANx RX GPIO pin configuration  */
  GPIO_InitStruct.Pin       = FDCANx_RX_PIN;
  GPIO_InitStruct.Alternate = FDCANx_RX_AF;
  HAL_GPIO_Init(FDCANx_RX_GPIO_PORT, &GPIO_InitStruct);

  /*##-2- Configure the NVIC #################################################*/
  /* NVIC for FDCANx */
  HAL_NVIC_SetPriority(FDCAN1_IT0_IRQn, 0, 1);
  HAL_NVIC_SetPriority(FDCAN1_IT1_IRQn, 0, 1);
  HAL_NVIC_SetPriority(FDCAN_CAL_IRQn, 0, 1);
  HAL_NVIC_EnableIRQ(FDCAN1_IT0_IRQn);
  HAL_NVIC_EnableIRQ(FDCAN1_IT0_IRQn);
  HAL_NVIC_EnableIRQ(FDCAN_CAL_IRQn);
}

/**
  * @brief  DeInitializes the FDCAN MSP.
  * @param  hfdcan: pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @retval None
  */
void HAL_FDCAN_MspDeInit(FDCAN_HandleTypeDef* hfdcan)
{
  /*##-1- Reset peripherals ##################################################*/
  FDCANx_FORCE_RESET();
  FDCANx_RELEASE_RESET();

  /*##-2- Disable peripherals and GPIO Clocks ################################*/
  /* Configure FDCANx Tx as alternate function  */
  HAL_GPIO_DeInit(FDCANx_TX_GPIO_PORT, FDCANx_TX_PIN);

  /* Configure FDCANx Rx as alternate function  */
  HAL_GPIO_DeInit(FDCANx_RX_GPIO_PORT, FDCANx_RX_PIN);

  /*##-3- Disable the NVIC for FDCANx ########################################*/
  HAL_NVIC_DisableIRQ(FDCAN1_IT0_IRQn);
  HAL_NVIC_DisableIRQ(FDCAN1_IT0_IRQn);
  HAL_NVIC_DisableIRQ(FDCAN_CAL_IRQn);
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/**
  * @}
  */

/**
  * @}
  */

