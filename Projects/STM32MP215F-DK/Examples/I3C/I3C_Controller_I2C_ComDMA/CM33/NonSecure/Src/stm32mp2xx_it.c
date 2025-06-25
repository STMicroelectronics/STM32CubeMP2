/**
  ******************************************************************************
  * @file    stm32mp2xx_it.c
  * @author  MCD Application Team
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and
  *          peripherals interrupt service routine.
  ******************************************************************************
  *
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32mp2xx_it.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern DMA_HandleTypeDef handle_HPDMA3_Channel2;
extern DMA_HandleTypeDef handle_HPDMA3_Channel1;
extern DMA_HandleTypeDef handle_HPDMA3_Channel0;
extern I3C_HandleTypeDef hi3c1;
extern IPCC_HandleTypeDef   hipcc;
/******************************************************************************/
/*            Cortex Processor Interruption and Exception Handlers         */
/******************************************************************************/

/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */

  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
  /* USER CODE BEGIN HardFault_IRQn 1 */

  /* USER CODE END HardFault_IRQn 1 */
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
  /* USER CODE BEGIN MemoryManagement_IRQn 1 */

  /* USER CODE END MemoryManagement_IRQn 1 */
}

/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
  /* USER CODE BEGIN BusFault_IRQn 1 */

  /* USER CODE END BusFault_IRQn 1 */
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
  /* USER CODE BEGIN UsageFault_IRQn 1 */

  /* USER CODE END UsageFault_IRQn 1 */
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  HAL_SYSTICK_IRQHandler();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32MP2xx Peripheral Interrupt Handlers                                   */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32mp257faix.s).               */
/******************************************************************************/

/**
  * @brief This function handles the RX Occupied interrupt.
  */
void IPCC1_RX_IRQHandler(void)
{
  HAL_IPCC_RX_IRQHandler(&hipcc);
}

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
  * @brief This function handles HPDMA3 Channel 2 global interrupt.
  */
void HPDMA3_Channel2_IRQHandler(void)
{
  /* USER CODE BEGIN HPDMA3_Channel2_IRQn 0 */

  /* USER CODE END HPDMA3_Channel2_IRQn 0 */
  HAL_DMA_IRQHandler(&handle_HPDMA3_Channel2);
  /* USER CODE BEGIN HPDMA3_Channel2_IRQn 1 */

  /* USER CODE END HPDMA3_Channel2_IRQn 1 */
}

/**
  * @brief This function handles I3C1 event interrupt.
  */
void I3C1_IRQHandler(void)
{
  /* USER CODE BEGIN I3C1_EV_IRQn 0 */

  /* USER CODE END I3C1_EV_IRQn 0 */
  HAL_I3C_EV_IRQHandler(&hi3c1);
  HAL_I3C_ER_IRQHandler(&hi3c1);
  /* USER CODE BEGIN I3C1_EV_IRQn 1 */

  /* USER CODE END I3C1_EV_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
