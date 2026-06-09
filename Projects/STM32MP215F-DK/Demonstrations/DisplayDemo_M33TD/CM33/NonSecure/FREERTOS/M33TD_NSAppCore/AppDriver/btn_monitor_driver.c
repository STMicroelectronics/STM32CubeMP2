/**
  ******************************************************************************
  * @file    btn_monitor_driver.c
  * @author  MCD Application Team
  * @brief   Button Monitor driver implementation.
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

/**
  * @addtogroup BtnMonitorDriver
  * @{
  */

/* Includes ------------------------------------------------------------------*/
#include "btn_monitor_driver.h"
#include "main.h"

/*
 * Button mapping used by this driver:
 * - BUTTON_USER2 on GPIOF pin 7
 * - EXTI2 line 7 (EXTI2_7_IRQn)
 */

/**
  * @brief EXTI handle for USER2 button (EXTI2 line 7).
  *
  * @note The EXTI2_7 IRQ handler is expected to call HAL_EXTI_IRQHandler(&hexti2).
  */
EXTI_HandleTypeDef hexti2;

/* Private variables ---------------------------------------------------------*/
static BtnMonitorDriver_IRQCallback BtnMonitor_IRQCallback;
static void *BtnMonitor_IRQCallbackContext;

/* Private function prototypes -----------------------------------------------*/
static void BtnMonitorDriver_ExtiRisingCb(void);
static int BtnMonitorDriver_Init(BtnMonitorDriver_IRQCallback cb, void *context);
static int BtnMonitorDriver_DeInit(void);
static void BtnMonitorDriver_EnableBtnIRQ(void);
static void BtnMonitorDriver_DisableBtnIRQ(void);
static bool BtnMonitorDriver_IsBtnPressed(void);

/* Exported driver instance --------------------------------------------------*/
/**
  * @brief Button monitor driver instance.
  */
BtnMonitorDriverTypeDef btnMonitorDriver =
{
  .init = BtnMonitorDriver_Init,
  .deinit = BtnMonitorDriver_DeInit,
  .enable_btn_irq = BtnMonitorDriver_EnableBtnIRQ,
  .disable_btn_irq = BtnMonitorDriver_DisableBtnIRQ,
  .is_btn_pressed = BtnMonitorDriver_IsBtnPressed,
};

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  EXTI rising-edge callback for the USER2 button.
  * @retval None
  */
static void BtnMonitorDriver_ExtiRisingCb(void)
{
  /* Disable IRQ to avoid bounce until task re-enables it. */
  HAL_NVIC_DisableIRQ(EXTI2_7_IRQn);

  if (BtnMonitor_IRQCallback)
  {
    BtnMonitor_IRQCallback(BtnMonitor_IRQCallbackContext);
  }
}

/**
  * @brief  Initialize the button monitor driver.
  * @param  cb IRQ callback to be invoked on button press.
  * @param  context User context passed back to the callback.
  * @retval 0 on success, negative value on error.
  */
static int BtnMonitorDriver_Init(BtnMonitorDriver_IRQCallback cb, void *context)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  EXTI_ConfigTypeDef EXTI_ConfigStructure;

  BtnMonitor_IRQCallback = cb;
  BtnMonitor_IRQCallbackContext = context;

  /* Enable GPIOF clock */
  if (RESMGR_STATUS_ACCESS_OK == ResMgr_Request(RESMGR_RESOURCE_RIF_RCC, RESMGR_RCC_RESOURCE(96)))
  {
    __HAL_RCC_GPIOF_CLK_ENABLE();
  }

  /* Acquire GPIOF7 */
  if (RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIF_GPIOF, RESMGR_GPIO_PIN(7)))
  {
    return -1;
  }

  /* Acquire EXTI2 line 7 */
  if (RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIF_EXTI2, RESMGR_EXTI_RESOURCE(7)))
  {
    (void)ResMgr_Release(RESMGR_RESOURCE_RIF_GPIOF, RESMGR_GPIO_PIN(7));
    return -2;
  }

  /* Configure USER2 pin as input floating */
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Pin = BUTTON_USER2_PIN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(BUTTON_USER2_GPIO_PORT, &GPIO_InitStruct);

  /* Configure EXTI2 line 7 rising edge on GPIOF */
  EXTI_ConfigStructure.Line = EXTI2_LINE_7;
  EXTI_ConfigStructure.Trigger = EXTI_TRIGGER_RISING;
  EXTI_ConfigStructure.GPIOSel = EXTI_GPIOF;
  EXTI_ConfigStructure.Mode = EXTI_MODE_INTERRUPT;
  HAL_EXTI_SetConfigLine(&hexti2, &EXTI_ConfigStructure);

  /* Register callback */
  HAL_EXTI_RegisterCallback(&hexti2, HAL_EXTI_RISING_CB_ID, BtnMonitorDriver_ExtiRisingCb);

  /* Enable IRQ at a priority compatible with FreeRTOS syscalls */
  HAL_NVIC_SetPriority(EXTI2_7_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + DEFAULT_IRQ_PRIO, 0);
  HAL_NVIC_EnableIRQ(EXTI2_7_IRQn);

  return 0;
}

/**
  * @brief  De-initialize the button monitor driver.
  * @retval 0 on success, negative value on error.
  */
static int BtnMonitorDriver_DeInit(void)
{
  HAL_NVIC_DisableIRQ(EXTI2_7_IRQn);

  /* Best-effort release of resources */
  (void)ResMgr_Release(RESMGR_RESOURCE_RIF_EXTI2, RESMGR_EXTI_RESOURCE(7));
  (void)ResMgr_Release(RESMGR_RESOURCE_RIF_GPIOF, RESMGR_GPIO_PIN(7));

  BtnMonitor_IRQCallback = NULL;
  BtnMonitor_IRQCallbackContext = NULL;
  return 0;
}

/**
  * @brief  Enable the button IRQ.
  * @retval None
  */
static void BtnMonitorDriver_EnableBtnIRQ(void)
{
  HAL_NVIC_EnableIRQ(EXTI2_7_IRQn);
}

/**
  * @brief  Disable the button IRQ.
  * @retval None
  */
static void BtnMonitorDriver_DisableBtnIRQ(void)
{
  HAL_NVIC_DisableIRQ(EXTI2_7_IRQn);
}

/**
  * @brief  Get the current button state.
  * @retval true if pressed, otherwise false.
  */
static bool BtnMonitorDriver_IsBtnPressed(void)
{
  return (HAL_GPIO_ReadPin(BUTTON_USER2_GPIO_PORT, BUTTON_USER2_PIN) == GPIO_PIN_SET);
}

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
