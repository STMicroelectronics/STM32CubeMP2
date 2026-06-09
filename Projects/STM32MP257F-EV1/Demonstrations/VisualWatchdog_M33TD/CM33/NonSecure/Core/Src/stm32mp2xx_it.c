/**
  ******************************************************************************
  * @file    stm32mp2xx_it.c
  * @author  MCD Application Team
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and
  *          peripherals interrupt service routine.
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
#include "stm32mp2xx_hal.h"
#include "stm32mp2xx_it.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern TIM_HandleTypeDef htim6;
extern EXTI_HandleTypeDef hexti1;
extern EXTI_HandleTypeDef hexti2;
extern IPCC_HandleTypeDef hipcc;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M33 Processor Exceptions Handlers                         */
/******************************************************************************/


/**
  * @brief  This function handles Debug Monitor exception.
  * @retval None
  */
void DebugMon_Handler(void)
{
  while (1)
  {
  }
}

/******************************************************************************/
/*                 STM32MP2xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32mp2xx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles external line 8 interrupt request.
  * @param  None
  * @retval None
  */

void IWDG1_RST_IRQHandler(void)
{
  HAL_EXTI_IRQHandler(&hexti1);
}


/**
  * @brief This function handles TIM6 global interrupt.
  */
void TIM6_IRQHandler(void)
{
  /* USER CODE BEGIN TIM6_IRQn 0 */

  /* USER CODE END TIM6_IRQn 0 */
  HAL_TIM_IRQHandler(&htim6);
  /* USER CODE BEGIN TIM6_IRQn 1 */

  /* USER CODE END TIM6_IRQn 1 */
}

/**
 * @brief  Interrupt handler for IPCC1 RX.
 *
 * This function handles the IPCC1 RX interrupt request. It calls the HAL
 * IPCC RX interrupt handler to process the received message.
 *
 * @note   This function should be called automatically by the hardware
 *         when an IPCC1 RX interrupt occurs.
 */
void IPCC1_RX_IRQHandler(void)
{
   HAL_IPCC_RX_IRQHandler(&hipcc);
}


/**
  * @brief  This function handles external line 8 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI2_8_IRQHandler(void)
{
  HAL_EXTI_IRQHandler(&hexti2);
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
