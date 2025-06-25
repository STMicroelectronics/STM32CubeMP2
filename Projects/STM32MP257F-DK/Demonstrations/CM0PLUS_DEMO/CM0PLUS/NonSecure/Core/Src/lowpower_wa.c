/**
  ******************************************************************************
  * @file    lowpower_wa.c
  * @author  MCD Application Team
  * @brief   Low Power Workaround
  *          The M0+ microcontroller may crash when power domains D1 or D2 enter
  *          or exit low power states (Standby 1 and LPLV-Stop 2). To prevent
  *          this issue, the M0+ is placed in a Wait For Interrupt (WFI) state
  *          for less than 800 us during the transitions of D1/D2 into or out of
  *          low power states.
  *          Requirements:
  *          - The main CPU (either CPU1 or CPU2) must send an interrupt just
  *            before entering low power using EXTI2 (line 59 for CPU2 and line
  *            60 for CPU1) to trigger the workaround
  *          - The PWR_ON signal must be connected to the GPIOZ9 to trig an
  *            interrupt just before exiting low power using EXTI2 to trigger
  *            the workaround
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "lowpower_wa.h"
#include "stm32mp2xx_ll_lptim.h"
#include "stm32mp2xx_ll_exti.h"
#include "stm32mp2xx_ll_gpio.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#ifdef LOWPOWER_WA_STANDBY1

/* Low power entry delay: period to maintain the WFI on Low power entry */
#define LPTIM4_LP_ENTRY_WFI_US_DELAY 2

/*
 * Low Power Exit Delays:
 *
 * The `LPTIM4_LP_LEAVE_PENDING_US_DELAY` represents the delay between the
 * reception of the PWR_ON signal and the effective exit from low power mode.
 * This delay allows for optimizing the period spent in WFI (Wait For Interrupt).
 * During this period, the code in the normal context is executed.
 *
 * Subsequently, upon LPTIMER4 expiration, the `PendSV_Handler` exception is
 * called with a lower priority (interruptible by LPTIMER4). The LPTIMER4
 * is programmed with the `LPTIM4_LP_LEAVE_WFI_US_DELAY` before entering in WFI,
 * waiting LPTIMER4 interruption.
 */
#define LPTIM4_LP_LEAVE_PENDING_US_DELAY (LSI_VALUE * 160 / 1000000 - 4)
#define LPTIM4_LP_LEAVE_WFI_US_DELAY 30
#endif /* LOWPOWER_WA_STANDBY1 */

/* Private variables ---------------------------------------------------------*/
uint32_t multi_lptim4_usage_state = 0;

/* Exported variables --------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
 * @brief  Initializes the LPTIM4 peripheral for interrupt generation.
 *         Configures the LPTIM4 timer, sets the prescaler, counter mode,
 *         auto-reload value, and enables the interrupt.
 *         Also sets the NVIC priority and enables the IRQ.
 * @param  None
 * @retval None
 */
void LPTIM4_IRQ_Init(void)
{
  /* LPTIM4 disable */
  LL_LPTIM_Disable(LPTIM4);

  /* LPTIM4 setup that must be done when LPTIM4 is disable */
  LL_LPTIM_SetPrescaler(LPTIM4, LL_LPTIM_PRESCALER_DIV1);
  LL_LPTIM_SetCounterMode(LPTIM4, LL_LPTIM_COUNTER_MODE_INTERNAL);

  /* LPTIM4 enable */
  LL_LPTIM_Enable(LPTIM4);

  /* LPTIM4 setup that must be done once LPTIM4 is enable */
  LL_LPTIM_ResetCounter(LPTIM4);
  LL_LPTIM_EnableIT_ARRM(LPTIM4);

  /* Configure NVIC for LPTIM4 interrupts */
  NVIC_SetPriority(LPTIM4_IRQn, 0);
  NVIC_EnableIRQ(LPTIM4_IRQn);
}

/**
 * @brief  Initializes the EXTI2 line 9 for interrupt generation.
 *         Configures EXTI2 to send IRQ6 when GPIOZ9 rises.
 *         Also sets the NVIC priority and enables the IRQ.
 * @param  None
 * @retval None
 */
void EXTI2_LINE_9_IRQ_Init(void)
{
  /* Configure GPIOZ9 in input mode */
  LL_GPIO_SetPinMode(GPIOZ, LL_GPIO_PIN_9, LL_GPIO_MODE_INPUT);

  /* Configure EXTI2 to send IRQ6 when GPIOZ9 rises */
  LL_EXTI_SetEXTISource(EXTI2, EXTI_EXTICR1_EXTI0_PZ, LL_EXTI_EXTI_LINE9);
  LL_EXTI_EnableRisingTrig_0_31(EXTI2, LL_EXTI2_LINE_9);
  LL_C3_EXTI_EnableIT_0_31(EXTI2, LL_EXTI2_LINE_9);

  /* Configure NVIC for EXTI2 interrupts */
  NVIC_EnableIRQ(EXTI2_H_IRQn);
  NVIC_SetPriority(EXTI2_H_IRQn, 1);
}

/**
 * @brief  Initializes the EXTI2 line 59 for interrupt generation.
 *         Configures EXTI2 to send IRQ26 when CPU2 SEV.
 *         Also sets the NVIC priority and enables the IRQ.
 * @param  None
 * @retval None
 */
void EXTI2_LINE_59_IRQ_Init(void)
{
  /* Configure EXTI2 to send IRQ6 when GPIOZ9 rises */
  LL_C3_EXTI_EnableIT_32_63(EXTI2, LL_EXTI2_LINE_59);

  /* Configure NVIC for EXTI2 interrupts */
  NVIC_EnableIRQ(CPU2_SEV_IRQn);
  NVIC_SetPriority(CPU2_SEV_IRQn, 1);
}

/**
 * @brief  Initializes the EXTI2 line 60 for interrupt generation.
 *         Configures EXTI2 to send IRQ25 when CPU1 SEV.
 *         Also sets the NVIC priority and enables the IRQ.
 * @param  None
 * @retval None
 */
void EXTI2_LINE_60_IRQ_Init(void)
{
  /* Configure EXTI2 to send IRQ6 when GPIOZ9 rises */
  LL_C3_EXTI_EnableIT_32_63(EXTI2, LL_EXTI2_LINE_60);

  /* Configure NVIC for EXTI2 interrupts */
  NVIC_EnableIRQ(CPU1_SEV_IRQn);
  NVIC_SetPriority(CPU1_SEV_IRQn, 1);
}

/**
 * @brief  Sets the PendSV exception priority and enables it.
 * @param  None
 * @retval None
 */
void PendSV_IRQ_Init(void)
{
  /* Configure NVIC for PendSV interrupts */
  NVIC_SetPriority(-2, 1);
}

/**
 * @brief  Call the above functions (LPTIM4_IRQ_Init, EXTI2_LINE_9_IRQ_Init,
 * EXTI2_LINE_59_IRQ_Init, EXTI2_LINE_60_IRQ_Init, PendSV_IRQ_Init).
 * @param  None
 * @retval None
 */
void lowpower_wa_Init(void)
{
  PendSV_IRQ_Init();
  LPTIM4_IRQ_Init();
  EXTI2_LINE_9_IRQ_Init();
  EXTI2_LINE_59_IRQ_Init();
  EXTI2_LINE_60_IRQ_Init();
}

/**
 * @brief  IRQ handler for LPTIM4.
 *         Clears the LPTIM4 event and resets the counter.
 * @param  None
 * @retval None
 */
void LPTIM4_IRQHandler(void)
{
  /* Clear LPTIM4 event */
  LL_LPTIM_ClearFlag_ARRM(LPTIM4);
  NVIC_ClearPendingIRQ(LPTIM4_IRQn);
  LL_LPTIM_ResetCounter(LPTIM4);

  /* LPTIM4 multi usage handler */
  if (multi_lptim4_usage_state)
  {
    multi_lptim4_usage_state = 0;
    SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
  }
}

/**
 * @brief  IRQ handler for EXTI2 line 9 (PWR_ON).
 *         Programs the LPTIM4, clears the IRQ and EXTI2.
 * @param  None
 * @retval None
 */
void EXTI2_H_IRQHandler(void)
{

  /* Clear IRQ and EXTI2 */
  NVIC_ClearPendingIRQ(EXTI2_H_IRQn);
  LL_EXTI_ClearRisingFlag_0_31(EXTI2, LL_EXTI2_LINE_9);

  /* Change LPTIM4 handler state */
  multi_lptim4_usage_state = 1;

  /* Program LPTIM4 */
  LL_LPTIM_SetAutoReload(LPTIM4, LPTIM4_LP_LEAVE_PENDING_US_DELAY);
  LL_LPTIM_StartCounter(LPTIM4, LL_LPTIM_OPERATING_MODE_ONESHOT);
}

/**
 * @brief  IRQ handler for EXTI2 line 60 (SEV from CPU1).
 *         Programs the LPTIM4, clears the IRQ and EXTI2, and enters WFI.
 * @param  None
 * @retval None
 */
void CPU1_SEV_IRQHandler(void)
{
  /* Program LPTIM4 */
  LL_LPTIM_SetAutoReload(LPTIM4, LPTIM4_LP_ENTRY_WFI_US_DELAY);
  LL_LPTIM_StartCounter(LPTIM4, LL_LPTIM_OPERATING_MODE_ONESHOT);

  /* Clear IRQ and EXTI2 */
  NVIC_ClearPendingIRQ(CPU1_SEV_IRQn);
  LL_EXTI_ClearRisingFlag_32_63(EXTI2, LL_EXTI2_LINE_60);

  /* Enter WFI */
  __WFI();
}

/**
 * @brief  IRQ handler for EXTI2 line 59 (SEV from CPU2).
 *         Programs the LPTIM4, clears the IRQ and EXTI2, and enters WFI.
 * @param  None
 * @retval None
 */
void CPU2_SEV_IRQHandler(void)
{
  /* Program LPTIM4 */
  LL_LPTIM_SetAutoReload(LPTIM4, LPTIM4_LP_ENTRY_WFI_US_DELAY);
  LL_LPTIM_StartCounter(LPTIM4, LL_LPTIM_OPERATING_MODE_ONESHOT);

  /* Clear IRQ and EXTI2 */
  NVIC_ClearPendingIRQ(CPU2_SEV_IRQn);
  LL_EXTI_ClearRisingFlag_32_63(EXTI2, LL_EXTI2_LINE_59);

  /* Enter WFI */
  __WFI();
}

/**
  * @brief  This function handles PendSVC exception trigged by
  * the LPTIM4 in EXTI2_H_IRQHandler.
  * Programs the LPTIM4, clears the PENSV IRQ, and enters WFI.
  * @retval None
  */
void PendSV_Handler(void)
{
  /* Program LPTIM4 */
  LL_LPTIM_SetAutoReload(LPTIM4, LPTIM4_LP_LEAVE_WFI_US_DELAY);
  LL_LPTIM_StartCounter(LPTIM4, LL_LPTIM_OPERATING_MODE_ONESHOT);

  /* Clear PendSV */
  SCB->ICSR |= SCB_ICSR_PENDSVCLR_Msk;

  /* Enter WFI */
  __WFI();
}

/* External variables --------------------------------------------------------*/
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
