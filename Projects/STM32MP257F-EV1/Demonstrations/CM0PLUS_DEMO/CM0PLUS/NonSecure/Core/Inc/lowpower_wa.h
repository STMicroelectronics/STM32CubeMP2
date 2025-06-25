/**
  ******************************************************************************
  * @file    lowpower_wa.h
  * @author  MCD Application Team
  * @brief   Low Power Workaround
  *          The M0+ microcontroller may crash when power domains D1 or D2 enter
  *          or exit low power states (Standby 1 and LPLV-Stop 2). To prevent
  *          this issue, the M0+ is placed in a Wait For Interrupt (WFI) state
  *          for less than 800 us during the transitions of D1/D2 into or out of
  *          low power states.
  *          Requirement: The main CPU (either CPU1 or CPU2) must send an
  *          interrupt just before entering low power using EXTI2 (line 59 for
  *          CPU2 and line 60 for CPU1) to trigger the workaround.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef LOWPOWER_WA_H
#define LOWPOWER_WA_H

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

/**
 * @brief  Initializes the LPTIM4 peripheral for interrupt generation.
 *         Configures the LPTIM4 timer, sets the prescaler, counter mode,
 *         auto-reload value, and enables the interrupt.
 *         Also sets the NVIC priority and enables the IRQ.
 * @param  None
 * @retval None
 */
void LPTIM4_IRQ_Init(void);

/**
 * @brief  Initializes the EXTI2 line 9 for interrupt generation.
 *         Configures EXTI2 to send IRQ6 when GPIOZ9 rises.
 *         Also sets the NVIC priority and enables the IRQ.
 * @param  None
 * @retval None
 */
void EXTI2_LINE_9_IRQ_Init(void);

/**
 * @brief  Initializes the EXTI2 line 59 for interrupt generation.
 *         Configures EXTI2 to send IRQ26 when CPU2 SEV.
 *         Also sets the NVIC priority and enables the IRQ.
 * @param  None
 * @retval None
 */
void EXTI2_LINE_59_IRQ_Init(void);

/**
 * @brief  Initializes the EXTI2 line 60 for interrupt generation.
 *         Configures EXTI2 to send IRQ25 when CPU1 SEV.
 *         Also sets the NVIC priority and enables the IRQ.
 * @param  None
 * @retval None
 */
void EXTI2_LINE_60_IRQ_Init(void);

/**
 * @brief  Sets the PendSV exception priority and enables it.
 * @param  None
 * @retval None
 */
void PendSV_IRQ_Init(void);

/**
 * @brief  Call the above functions (LPTIM4_IRQ_Init, EXTI2_LINE_9_IRQ_Init,
 * EXTI2_LINE_59_IRQ_Init, EXTI2_LINE_60_IRQ_Init, PendSV_IRQ_Init).
 * @param  None
 * @retval None
 */
void lowpower_wa_Init(void);

/* IRQ Handlers */

/**
 * @brief  IRQ handler for LPTIM4.
 *         Clears the LPTIM4 event and resets the counter.
 * @param  None
 * @retval None
 */
void LPTIM4_IRQHandler(void);

/**
 * @brief  IRQ handler for EXTI2 line 9 (PWR_ON).
 *         Programs the LPTIM4, clears the IRQ and EXTI2, and enters WFI.
 * @param  None
 * @retval None
 */
void EXTI2_H_IRQHandler(void);

/**
 * @brief  IRQ handler for EXTI2 line 60 (SEV from CPU1).
 *         Programs the LPTIM4, clears the IRQ and EXTI2, and enters WFI.
 * @param  None
 * @retval None
 */
void CPU1_SEV_IRQHandler(void);

/**
 * @brief  IRQ handler for EXTI2 line 59 (SEV from CPU2).
 *         Programs the LPTIM4, clears the IRQ and EXTI2, and enters WFI.
 * @param  None
 * @retval None
 */
void CPU2_SEV_IRQHandler(void);

#endif /* LOWPOWER_WA_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/