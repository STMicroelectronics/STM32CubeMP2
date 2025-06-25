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
#include "main.h"
#include "testapp_task.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern TIM_HandleTypeDef htim6;
extern EXTI_HandleTypeDef hexti;
extern EXTI_HandleTypeDef hexti1;
#ifdef ENABLE_TIMERS_TEST
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim8;
extern LPTIM_HandleTypeDef hlptim4;
#endif
#ifdef ENABLE_I3C_TEST
extern DMA_HandleTypeDef handle_HPDMA3_Channel2;
extern DMA_HandleTypeDef handle_HPDMA3_Channel1;
extern DMA_HandleTypeDef handle_HPDMA3_Channel0;
extern I3C_HandleTypeDef hi3c2;
#endif
#ifdef ENABLE_SPI_TEST
extern SPI_HandleTypeDef SpiHandle;
#endif
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M33 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

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
  * @brief  This function handles external line 34 interrupt request.
  * @param  None
  * @retval None
  */
void IWDG1_RST_IRQHandler(void)
{
  HAL_EXTI_IRQHandler(&hexti1);
}

#ifdef ENABLE_TIMERS_TEST
/**
* @brief This function handles TIM3 global interrupt.
*/
void TIM3_IRQHandler(void)
{
  /* USER CODE BEGIN TIM3_IRQn 0 */

  /* USER CODE END TIM3_IRQn 0 */
  HAL_TIM_IRQHandler(&htim3);
  /* USER CODE BEGIN TIM3_IRQn 1 */

  /* USER CODE END TIM3_IRQn 1 */
}

/**
* @brief This function handles LPTIM4 global interrupt.
*/
void LPTIM4_IRQHandler(void)
{
  /* USER CODE BEGIN LPTIM4_IRQn 0 */

  /* USER CODE END LPTIM4_IRQn 0 */
  HAL_LPTIM_IRQHandler(&hlptim4);
  /* USER CODE BEGIN LPTIM4_IRQn 1 */

  /* USER CODE END LPTIM4_IRQn 1 */
}

/**
* @brief This function handles TIM8 global interrupt.
*/
void TIM8_UP_IRQHandler(void)
{
  /* USER CODE BEGIN TIM8_IRQn 0 */

  /* USER CODE END TIM8_IRQn 0 */
  HAL_TIM_IRQHandler(&htim8);
  /* USER CODE BEGIN TIM8_IRQn 1 */

  /* USER CODE END TIM8_IRQn 1 */
}
#endif /* ENABLE_TEST_TIMER */

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
  * @brief  This function handles external line 7 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI2_7_IRQHandler(void)
{
  HAL_EXTI_IRQHandler(&hexti);
}


#ifdef ENABLE_I3C_TEST
/**
  * @brief This function handles HPDMA3 Channel 0 global interrupt.
  */
void HPDMA3_Channel0_IRQHandler(void)
{
  /* USER CODE BEGIN HPDMA3_Channel0_IRQn 0 */

  /* USER CODE END HPDMA3_Channel0_IRQn 0 */
  HAL_DMA_IRQHandler(&handle_HPDMA3_Channel0);
  /* USER CODE BEGIN HPDMA3_Channel0_IRQn 1 */

  /* USER CODE END HPDMA3_Channel0_IRQn 1 */
}

/**
  * @brief This function handles HPDMA3 Channel 1 global interrupt.
  */
void HPDMA3_Channel1_IRQHandler(void)
{
  /* USER CODE BEGIN HPDMA3_Channel1_IRQn 0 */

  /* USER CODE END HPDMA3_Channel1_IRQn 0 */
  HAL_DMA_IRQHandler(&handle_HPDMA3_Channel1);
  /* USER CODE BEGIN HPDMA3_Channel1_IRQn 1 */

  /* USER CODE END HPDMA3_Channel1_IRQn 1 */
}

/**
  * @brief This function handles I3C2 event interrupt.
  */
void I3C2_IRQHandler(void)
{
  /* USER CODE BEGIN I3C2_EV_IRQn 0 */

  /* USER CODE END I3C2_EV_IRQn 0 */
  HAL_I3C_EV_IRQHandler(&hi3c2);
  HAL_I3C_ER_IRQHandler(&hi3c2);
  /* USER CODE BEGIN I3C2_EV_IRQn 1 */

  /* USER CODE END I3C2_EV_IRQn 1 */
}
#endif /* ENABLE_TEST_I3C */

/**
* @brief This function handles HPDMA3 channel2 global interrupt.
*/
void HPDMA3_Channel2_IRQHandler(void)
{
#if defined(ENABLE_SPI_TEST) && defined(ENABLE_I3C_TEST)
    if (SpiHandle.hdmatx->Instance == HPDMA3_Channel2) {
        // SPI DMA handling
        HAL_DMA_IRQHandler(SpiHandle.hdmatx);
    } else if (hi3c2.hdmatx->Instance == HPDMA3_Channel2) {
        // I3C DMA handling
        HAL_DMA_IRQHandler(&handle_HPDMA3_Channel2);
    }
#elif defined(ENABLE_SPI_TEST)
    // SPI DMA handling
    HAL_DMA_IRQHandler(SpiHandle.hdmatx);
#elif defined(ENABLE_I3C_TEST)
    // I3C DMA handling
    HAL_DMA_IRQHandler(&handle_HPDMA3_Channel2);
#endif
}

#ifdef ENABLE_SPI_TEST

/**
* @brief This function handles HPDMA3 channel3 global interrupt.
*/
void HPDMA3_Channel3_IRQHandler(void)
{
  /* USER CODE BEGIN HPDMA3_Channel3_IRQn 0 */

  /* USER CODE END HPDMA3_Channel3_IRQn 0 */
  HAL_DMA_IRQHandler(SpiHandle.hdmarx);
  /* USER CODE BEGIN HPDMA3_Channel3_IRQn 1 */

  /* USER CODE END HPDMA3_Channel3_IRQn 1 */
}

/**
* @brief This function handles SPI1 interrupt.
*/
void SPI1_IRQHandler(void)
{
  /* USER CODE BEGIN SPI1_EV_IRQn 0 */

  /* USER CODE END SPI1_EV_IRQn 0 */
  HAL_SPI_IRQHandler(&SpiHandle);
  /* USER CODE BEGIN SPI1_EV_IRQn 1 */

  /* USER CODE END SPI1_EV_IRQn 1 */
}
#endif /* ENABLE_TEST_SPI */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
