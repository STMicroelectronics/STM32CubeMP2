/**
  ******************************************************************************
  * @file    stm32mp257f_disco.c
  * @author  MCD Application Team
  * @brief   This file provides a set of firmware functions to manage
  *          LEDs
  *          push-buttons
  *          COM ports
  *          available on STM32MP257F_DK board (MB1605) from STMicroelectronics.
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
#include "stm32mp257f_disco.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32MP257F_DK
  * @{
  */

/** @defgroup LOW_LEVEL LOW LEVEL
  * @{
  */

/** @defgroup Private_TypesDefinitions  Private Types Definitions
  * @{
  */
typedef void (* BSP_EXTI_LineCallback)(void);
/**
  * @}
  */

/** @defgroup Private_FunctionPrototypes  Private Function Prototypes
  * @{
  */
#if defined (CORE_CA35) || defined (CORE_CM33)
static void BUTTON_WAKEUP_EXTI_Callback(void);
static void BUTTON_USER1_EXTI_Callback(void);
static void BUTTON_USER2_EXTI_Callback(void);
static void BUTTON_TAMPER_EXTI_Callback(void);
static int32_t BSP_LED_MspInit(Led_TypeDef Led);
static int32_t BSP_LED_MspDeInit(Led_TypeDef Led);
static int32_t BSP_PB_MspInit(Button_TypeDef Button);
static int32_t BSP_PB_MspDeInit(Button_TypeDef Button);
#endif /* CORE_CA35 || CORE_CM33 */
#if (USE_BSP_COM_FEATURE > 0)
static void USART_MspInit(UART_HandleTypeDef *huart);
static void USART_MspDeInit(UART_HandleTypeDef *huart);
#endif /* (USE_BSP_COM_FEATURE > 0) */
/**
  * @}
  */

/** @defgroup Exported_Variables Exported Variables
  * @{
  */
EXTI_HandleTypeDef hpb_exti[BUTTONn];
#if (USE_BSP_COM_FEATURE > 0)
UART_HandleTypeDef hcom_uart[COMn];
USART_TypeDef *COM_USART[COMn] =
#if defined (CORE_CA35) || defined (CORE_CM33)
{COM_CA35_UART, COM_CM33_UART, COM_CM0PLUS_UART};
#elif defined (CORE_CM0PLUS) /* Core_M0Plus uses only COM_CM0PLUS */
  {
    0, 0, COM_CM0PLUS_UART
  };
#endif /* CORE_<...> */

#endif /* (USE_BSP_COM_FEATURE > 0) */
/**
  * @}
  */

/** @defgroup Private_Variables Private Variables
  * @{
  */
#if defined (CORE_CA35) || defined (CORE_CM33)
static GPIO_TypeDef *LED_PORT[LEDn] = { LED1_GPIO_PORT,
                                        LED2_GPIO_PORT,
                                        LED3_GPIO_PORT,
                                        LED4_GPIO_PORT
                                      };

static const uint32_t LED_PIN[LEDn] = { LED1_PIN,
                                        LED2_PIN,
                                        LED3_PIN,
                                        LED4_PIN
                                      };

static const GPIO_PinState GPIO_LED_OFF[LEDn] = { GPIO_PIN_SET,
                                                  GPIO_PIN_RESET,
                                                  GPIO_PIN_RESET,
                                                  GPIO_PIN_RESET
                                                };

static const GPIO_PinState GPIO_LED_ON[LEDn] = { GPIO_PIN_RESET,
                                                 GPIO_PIN_SET,
                                                 GPIO_PIN_SET,
                                                 GPIO_PIN_SET
                                               };


static GPIO_TypeDef *BUTTON_PORT[BUTTONn] = {BUTTON_WAKEUP_GPIO_PORT, BUTTON_USER1_GPIO_PORT, BUTTON_USER2_GPIO_PORT,
                                             BUTTON_TAMPER_GPIO_PORT
                                            };
static const uint16_t BUTTON_PIN[BUTTONn] = {BUTTON_WAKEUP_PIN, BUTTON_USER1_PIN, BUTTON_USER2_PIN, BUTTON_TAMPER_PIN};
static const IRQn_Type BUTTON_IRQn[BUTTONn] = {BUTTON_WAKEUP_EXTI_IRQn, BUTTON_USER1_EXTI_IRQn, BUTTON_USER2_EXTI_IRQn,
                                               BUTTON_TAMPER_EXTI_IRQn
                                              };

#elif defined (CORE_CM0PLUS)
#endif /* CORE_CA35 || CORE_CM33 */

#if (USE_COM_LOG > 0)
/* define default active port to port dedicated to current Core
 * This is important for Validation applications which are not explicitly setting active log port */
static COM_TypeDef COM_ActiveLogPort = COM1;
#endif /* (USE_COM_LOG > 0) */
#if (USE_BSP_COM_FEATURE > 0)
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
static uint32_t IsComMspCbValid[COMn] = {0};
#endif /* (USE_HAL_UART_REGISTER_CALLBACKS == 0) */
#endif /* (USE_BSP_COM_FEATURE > 0) */

/**
  * @}
  */

/** @defgroup Exported_Functions Exported Functions
  * @{
  */

/**
  * @brief  This method returns the STM32MP257CXX EVAL BSP Driver revision
  * @retval version: 0xXYZR (8bits for each decimal, R for RC)
  */
int32_t BSP_GetVersion(void)
{
  return (int32_t)STM32MP257F_DK_BSP_VERSION;
}

/**
  * @brief  This method returns the board name
  * @retval pointer to the board name string
  */
const uint8_t *BSP_GetBoardName(void)
{
  return (uint8_t *)STM32MP257F_DK_BSP_BOARD_NAME;
}

/**
  * @brief  This method returns the board ID
  * @retval pointer to the board name string
  */
const uint8_t *BSP_GetBoardID(void)
{
  return (uint8_t *)STM32MP257F_DK_BSP_BOARD_ID;
}

#if defined (CORE_CA35) || defined (CORE_CM33)
/**
  * @brief  Configures LED on GPIO.
  * @param  Led LED to be configured.
  *          This parameter can be one of the following values:
  *            @arg  LED1
  *            @arg  LED2
  *            @arg  LED3
  *            @arg  LED4
  * @retval BSP status
  */
int32_t BSP_LED_Init(Led_TypeDef Led)
{
  int32_t ret = BSP_ERROR_NONE;
  GPIO_InitTypeDef  GPIO_InitStruct;

  /*BSP LED Msp Init Configuration*/
  if (BSP_LED_MspInit(Led) != BSP_ERROR_NONE)
  {
    return BSP_ERROR_MSP_FAILURE;
  }

  /* Configure the GPIO_LED pin */
  GPIO_InitStruct.Pin = LED_PIN[Led];
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

  HAL_GPIO_Init(LED_PORT[Led], &GPIO_InitStruct);

  /* By default, turn off LED */
  HAL_GPIO_WritePin(LED_PORT[Led], LED_PIN[Led], GPIO_LED_OFF[Led]);
  return ret;
}

/**
  * @brief  DeInit LEDs.
  * @param  Led LED to be configured.
  *          This parameter can be one of the following values:
  *            @arg  LED1
  *            @arg  LED2
  *            @arg  LED3
  *            @arg  LED4
  * @retval BSP status
  */
int32_t  BSP_LED_DeInit(Led_TypeDef Led)
{
  int32_t ret = BSP_ERROR_NONE;
  GPIO_InitTypeDef  gpio_init_structure;

  /* DeInit the GPIO_LED pin */
  gpio_init_structure.Pin = LED_PIN [Led];
  /* Turn off LED */
  HAL_GPIO_WritePin(LED_PORT [Led], (uint16_t)LED_PIN[Led], GPIO_LED_OFF[Led]);
  HAL_GPIO_DeInit(LED_PORT [Led], gpio_init_structure.Pin);

  /* BSP LED  MSP DeInit configuration*/
  if (BSP_LED_MspDeInit(Led) != BSP_ERROR_NONE)
  {
    return BSP_ERROR_MSP_FAILURE;
  }

  return ret;
}

/**
  * @brief  MspInit LEDs.
  * @param  Led LED to be configured.
  *          This parameter can be one of the following values:
  *            @arg  LED1
  *            @arg  LED2
  *            @arg  LED3
  *            @arg  LED4
  * @retval int32_t
  */
static int32_t BSP_LED_MspInit(Led_TypeDef Led)
{
  int32_t ret = BSP_ERROR_NONE;
  GPIO_TypeDef *led = LED_PORT[Led];
  uint32_t pin = LED_PIN[Led];

  /* Enable the GPIO_LED clock */
  if (Led == LED1 && !(LED1_GPIO_IS_CLK_ENABLED()))
  {
    if (RESMGR_STATUS_ACCESS_OK == ResMgr_Request(RESMGR_RESOURCE_RIF_RCC, RESMGR_RCC_RESOURCE(97)))
    {
      LED1_GPIO_CLK_ENABLE();
    }
  }
  else if (Led == LED2 && !(LED2_GPIO_IS_CLK_ENABLED()))
  {
    if (IS_DEVELOPER_BOOT_MODE())
    {
      HAL_PWREx_EnableSupply(PWR_PVM_VDDIO3);
    }
    if (RESMGR_STATUS_ACCESS_OK == ResMgr_Request(RESMGR_RESOURCE_RIF_RCC, RESMGR_RCC_RESOURCE(97)))
    {
      LED2_GPIO_CLK_ENABLE();
    }
  }
  else if (Led == LED3 && !(LED3_GPIO_IS_CLK_ENABLED()))
  {
    if (RESMGR_STATUS_ACCESS_OK == ResMgr_Request(RESMGR_RESOURCE_RIF_RCC, RESMGR_RCC_RESOURCE(97)))
    {
      LED3_GPIO_CLK_ENABLE();
    }
  }
  else if (Led == LED4 && !(LED4_GPIO_IS_CLK_ENABLED()))
  {
    if (RESMGR_STATUS_ACCESS_OK == ResMgr_Request(RESMGR_RESOURCE_RIF_RCC, RESMGR_RCC_RESOURCE(97)))
    {
      LED4_GPIO_CLK_ENABLE();
    }
  }


  if (!IS_DEVELOPER_BOOT_MODE())
  {
    if ((led == LED1_GPIO_PORT && pin == LED1_PIN &&
         ResMgr_Request(LED1_GPIO_RIF_RES_TYP_GPIO, LED1_GPIO_RIF_RES_NUM_GPIO) != RESMGR_STATUS_ACCESS_OK)
        || (led == LED2_GPIO_PORT && pin == LED2_PIN &&
            ResMgr_Request(LED2_GPIO_RIF_RES_TYP_GPIO, LED2_GPIO_RIF_RES_NUM_GPIO) != RESMGR_STATUS_ACCESS_OK)
        || (led == LED3_GPIO_PORT && pin == LED3_PIN &&
            ResMgr_Request(LED3_GPIO_RIF_RES_TYP_GPIO, LED3_GPIO_RIF_RES_NUM_GPIO) != RESMGR_STATUS_ACCESS_OK)
        || (led == LED4_GPIO_PORT && pin == LED4_PIN &&
            ResMgr_Request(LED4_GPIO_RIF_RES_TYP_GPIO, LED4_GPIO_RIF_RES_NUM_GPIO) != RESMGR_STATUS_ACCESS_OK))
    {
      ret = BSP_ERROR_MSP_FAILURE;
    }
  }
  return ret;
}

/**
  * @brief  MspDeInit LEDs.
  * @param  Led LED to be configured.
  *          This parameter can be one of the following values:
  *            @arg  LED1
  *            @arg  LED2
  *            @arg  LED3
  *            @arg  LED4
  * @retval int32_t
  */
static int32_t BSP_LED_MspDeInit(Led_TypeDef Led)
{
  int32_t ret = BSP_ERROR_NONE;
  GPIO_TypeDef *led = LED_PORT[Led];
  uint32_t pin = LED_PIN[Led];

  if (!IS_DEVELOPER_BOOT_MODE())
  {
    if ((led == LED1_GPIO_PORT && pin == LED1_PIN &&
         ResMgr_Release(LED1_GPIO_RIF_RES_TYP_GPIO, LED1_GPIO_RIF_RES_NUM_GPIO) != RESMGR_STATUS_ACCESS_OK)
        || (led == LED2_GPIO_PORT && pin == LED2_PIN &&
            ResMgr_Release(LED2_GPIO_RIF_RES_TYP_GPIO, LED2_GPIO_RIF_RES_NUM_GPIO) != RESMGR_STATUS_ACCESS_OK)
        || (led == LED3_GPIO_PORT && pin == LED3_PIN &&
            ResMgr_Release(LED3_GPIO_RIF_RES_TYP_GPIO, LED3_GPIO_RIF_RES_NUM_GPIO) != RESMGR_STATUS_ACCESS_OK)
        || (led == LED4_GPIO_PORT && pin == LED4_PIN &&
            ResMgr_Release(LED4_GPIO_RIF_RES_TYP_GPIO, LED4_GPIO_RIF_RES_NUM_GPIO) != RESMGR_STATUS_ACCESS_OK))
    {
      ret = BSP_ERROR_MSP_FAILURE;
    }
  }
  return ret;
}

/**
  * @brief  Turns selected LED On.
  * @param  Led LED to be set on
  *          This parameter can be one of the following values:
  *            @arg  LED1
  *            @arg  LED2
  *            @arg  LED3
  *            @arg  LED4
  * @retval BSP status
  */
int32_t  BSP_LED_On(Led_TypeDef Led)
{
  int32_t ret = BSP_ERROR_NONE;

  HAL_GPIO_WritePin(LED_PORT [Led], (uint16_t)LED_PIN [Led], GPIO_LED_ON[Led]);
  return ret;
}

/**
  * @brief  Turns selected LED Off.
  * @param  Led LED to be set off
  *          This parameter can be one of the following values:
  *            @arg  LED1
  *            @arg  LED2
  *            @arg  LED3
  *            @arg  LED4
  * @retval BSP status
  */
int32_t  BSP_LED_Off(Led_TypeDef Led)
{
  int32_t ret = BSP_ERROR_NONE;
  HAL_GPIO_WritePin(LED_PORT [Led], (uint16_t)LED_PIN [Led], GPIO_LED_OFF[Led]);
  return ret;
}

/**
  * @brief  Toggles the selected LED.
  * @param  Led LED to be toggled
  *          This parameter can be one of the following values:
  *            @arg  LED1
  *            @arg  LED2
  *            @arg  LED3
  *            @arg  LED4
  * @retval BSP status
  */
int32_t  BSP_LED_Toggle(Led_TypeDef Led)
{
  int32_t ret = BSP_ERROR_NONE;
  HAL_GPIO_TogglePin(LED_PORT[Led], (uint16_t)LED_PIN[Led]);
  return ret;
}
/**
  * @brief  Get the selected LED state.
  * @param  Led LED to be get its state
  *          This parameter can be one of the following values:
  *            @arg  LED1
  *            @arg  LED2
  *            @arg  LED3
  *            @arg  LED4
  * @retval LED status
  */
int32_t BSP_LED_GetState(Led_TypeDef Led)
{
  int32_t ret = BSP_ERROR_NONE;
  ret = (int32_t)HAL_GPIO_ReadPin(LED_PORT [Led], (uint16_t)LED_PIN [Led]);
  return ret;
}

/**
  * @brief  Configures button GPIO and EXTI Line.
  * @param  Button Button to be configured
  *          This parameter can be one of the following values:
  *            @arg  BUTTON_WAKEUP: Wakeup Push Button
  * @param  ButtonMode Button mode
  *          This parameter can be one of the following values:
  *            @arg  BUTTON_MODE_GPIO: Button will be used as simple IO
  *            @arg  BUTTON_MODE_EXTI: Button will be connected to EXTI line
  *                                    with interrupt generation capability
  * @retval BSP status
  */
int32_t  BSP_PB_Init(Button_TypeDef Button, ButtonMode_TypeDef ButtonMode)
{
  GPIO_InitTypeDef gpio_init_structure;
  static BSP_EXTI_LineCallback ButtonCallback[BUTTONn] = {BUTTON_WAKEUP_EXTI_Callback, BUTTON_USER1_EXTI_Callback,
                                                          BUTTON_USER2_EXTI_Callback, BUTTON_TAMPER_EXTI_Callback
                                                         };
  static uint32_t  BSP_BUTTON_PRIO [BUTTONn] = {BSP_BUTTON_WAKEUP_IT_PRIORITY, BSP_BUTTON_USER_IT_PRIORITY,
                                                BSP_BUTTON_USER_IT_PRIORITY, BSP_BUTTON_TAMPER_IT_PRIORITY
                                               };
  static const uint32_t BUTTON_EXTI_LINE[BUTTONn] = {BUTTON_WAKEUP_EXTI_LINE, BUTTON_USER1_EXTI_LINE,
                                                     BUTTON_USER2_EXTI_LINE, BUTTON_TAMPER_EXTI_LINE
                                                    };

  /*BSP PB Msp Init Configuration*/
  if (BSP_PB_MspInit(Button) != BSP_ERROR_NONE)
  {
    return BSP_ERROR_MSP_FAILURE;
  }

  /* Enable the BUTTON clock*/
  if (RESMGR_STATUS_ACCESS_OK == ResMgr_Request(RESMGR_RESOURCE_RIF_RCC, RESMGR_RCC_RESOURCE(92)))
  {
	  BUTTON_USER2_GPIO_CLK_ENABLE();
  }
  gpio_init_structure.Pin = BUTTON_PIN [Button];
  gpio_init_structure.Pull = GPIO_NOPULL;
  gpio_init_structure.Speed = GPIO_SPEED_FREQ_HIGH;

  if (ButtonMode == BUTTON_MODE_GPIO)
  {
    /* Configure Button pin as input */
    gpio_init_structure.Mode = GPIO_MODE_INPUT;
    HAL_GPIO_Init(BUTTON_PORT [Button], &gpio_init_structure);
  }
  else /* (ButtonMode == BUTTON_MODE_EXTI) */
  {
    /* Configure Button pin as input with External interrupt */
    gpio_init_structure.Mode = GPIO_MODE_IT_RISING;

    HAL_GPIO_Init(BUTTON_PORT[Button], &gpio_init_structure);

    (void)HAL_EXTI_GetHandle(&hpb_exti[Button], BUTTON_EXTI_LINE[Button]);
    (void)HAL_EXTI_RegisterCallback(&hpb_exti[Button],  HAL_EXTI_COMMON_CB_ID, ButtonCallback[Button]);

    /* Enable and set Button EXTI Interrupt to the lowest priority */
#if defined(CORE_CA35)
    IRQ_SetPriority((BUTTON_IRQn[Button]), BSP_BUTTON_PRIO[Button]);
    IRQ_Enable((BUTTON_IRQn[Button]));
#else
    HAL_NVIC_SetPriority((BUTTON_IRQn[Button]), BSP_BUTTON_PRIO[Button], 0x00);
    HAL_NVIC_EnableIRQ((BUTTON_IRQn[Button]));
#endif /* CORE_CA35 */
  }
  return BSP_ERROR_NONE;
}

/**
  * @brief  Push Button DeInit.
  * @param  Button Button to be configured
  *          This parameter can be one of the following values:
  *            @arg  BUTTON_WAKEUP: Wakeup Push Button
  * @note PB DeInit does not disable the GPIO clock
  * @retval BSP status
  */
int32_t BSP_PB_DeInit(Button_TypeDef Button)
{
  GPIO_InitTypeDef gpio_init_structure;

  gpio_init_structure.Pin = BUTTON_PIN[Button];
#if defined(CORE_CA35)
  IRQ_Disable((IRQn_Type)(BUTTON_IRQn[Button]));
#else
  HAL_NVIC_DisableIRQ((IRQn_Type)(BUTTON_IRQn[Button]));
#endif /* CORE_CA35 */
  HAL_GPIO_DeInit(BUTTON_PORT[Button], gpio_init_structure.Pin);


  /* BSP PB  MSP DeInit configuration*/
  if (BSP_PB_MspDeInit(Button) != BSP_ERROR_NONE)
  {
    return BSP_ERROR_MSP_FAILURE;
  }

  return BSP_ERROR_NONE;
}

/**
  * @brief  MspInit Buttons.
  * @param  Button Button to be configured
  *          This parameter can be one of the following values:
  *            @arg  BUTTON_WAKEUP: Wakeup Push Button
  *            @arg  BUTTON_TAMPER: Tamper Push Button
  *            @arg  BUTTON_USER1 : User1 Push Button
  *            @arg  BUTTON_USER2 : User2 Push Button
  * @retval int32_t
  */
static int32_t BSP_PB_MspInit(Button_TypeDef Button)
{
  int32_t ret = BSP_ERROR_NONE;
  GPIO_TypeDef *button = BUTTON_PORT[Button];
  uint16_t pin = BUTTON_PIN[Button];

  /* Enable the BUTTON clock*/
  if (Button == BUTTON_WAKEUP && !(BUTTON_WAKEUP_GPIO_IS_CLK_ENABLE()))
  {
    if (RESMGR_STATUS_ACCESS_OK == ResMgr_Request(RESMGR_RESOURCE_RIF_RCC, RESMGR_RCC_RESOURCE(90)))
    {
      BUTTON_WAKEUP_GPIO_CLK_ENABLE();
    }
  }
  else if (Button == BUTTON_TAMPER && !(BUTTON_TAMPER_GPIO_IS_CLK_ENABLE()))
  {
    if (RESMGR_STATUS_ACCESS_OK == ResMgr_Request(RESMGR_RESOURCE_RIF_RCC, RESMGR_RCC_RESOURCE(101)))
    {
      BUTTON_TAMPER_GPIO_CLK_ENABLE();
    }
  }
  else if (Button == BUTTON_USER1 && !(BUTTON_USER1_GPIO_IS_CLK_ENABLE()))
  {
    if (IS_DEVELOPER_BOOT_MODE())
    {
      HAL_PWR_EnableBkUpD3Access();
      HAL_PWREx_EnableSupply(PWR_PVM_VDDIO3);
    }
    if (RESMGR_STATUS_ACCESS_OK == ResMgr_Request(RESMGR_RESOURCE_RIF_RCC, RESMGR_RCC_RESOURCE(92)))
    {
      BUTTON_USER1_GPIO_CLK_ENABLE();
    }
  }
  else if (Button == BUTTON_USER2 && !(BUTTON_USER2_GPIO_IS_CLK_ENABLE()))
  {
    if (RESMGR_STATUS_ACCESS_OK == ResMgr_Request(RESMGR_RESOURCE_RIF_RCC, RESMGR_RCC_RESOURCE(92)))
    {
      BUTTON_USER2_GPIO_CLK_ENABLE();
    }
  }

  if (!IS_DEVELOPER_BOOT_MODE())
  {
    if ((button == BUTTON_WAKEUP_GPIO_PORT && pin == BUTTON_WAKEUP_PIN &&
         ResMgr_Request(BUTTON_WAKEUP_RIF_RES_TYP_GPIO, BUTTON_WAKEUP_RIF_RES_NUM_GPIO) != RESMGR_STATUS_ACCESS_OK)
        || (button == BUTTON_TAMPER_GPIO_PORT && pin == BUTTON_TAMPER_PIN &&
            ResMgr_Request(BUTTON_TAMPER_RIF_RES_TYP_GPIO, BUTTON_TAMPER_RIF_RES_NUM_GPIO) != RESMGR_STATUS_ACCESS_OK)
        || (button == BUTTON_USER1_GPIO_PORT && pin == BUTTON_USER1_PIN &&
            ResMgr_Request(BUTTON_USER1_RIF_RES_TYP_GPIO, BUTTON_USER1_RIF_RES_NUM_GPIO) != RESMGR_STATUS_ACCESS_OK)
        || (button == BUTTON_USER2_GPIO_PORT && pin == BUTTON_USER2_PIN &&
            ResMgr_Request(BUTTON_USER2_RIF_RES_TYP_GPIO, BUTTON_USER2_RIF_RES_NUM_GPIO) != RESMGR_STATUS_ACCESS_OK))
    {
      ret = BSP_ERROR_MSP_FAILURE;
    }
  }
  return ret;
}

/**
  * @brief  MspDeInit BUTTONs.
  * @param  Button BUTTON to be configured.
  *          This parameter can be one of the following values:
  *            @arg  BUTTON_WAKEUP: Wakeup Push Button
  *            @arg  BUTTON_TAMPER: Tamper Push Button
  *            @arg  BUTTON_USER1 : User1 Push Button
  *            @arg  BUTTON_USER2 : User2 Push Button
  * @retval int32_t
  */
static int32_t BSP_PB_MspDeInit(Button_TypeDef Button)
{
  int32_t ret = BSP_ERROR_NONE;
  GPIO_TypeDef *button = BUTTON_PORT[Button];
  uint16_t pin = BUTTON_PIN[Button];

  if (!IS_DEVELOPER_BOOT_MODE())
  {
    if ((button == BUTTON_WAKEUP_GPIO_PORT && pin == BUTTON_WAKEUP_PIN &&
         ResMgr_Release(BUTTON_WAKEUP_RIF_RES_TYP_GPIO, BUTTON_WAKEUP_RIF_RES_NUM_GPIO) != RESMGR_STATUS_ACCESS_OK)
        || (button == BUTTON_TAMPER_GPIO_PORT && pin == BUTTON_TAMPER_PIN &&
            ResMgr_Release(BUTTON_TAMPER_RIF_RES_TYP_GPIO, BUTTON_TAMPER_RIF_RES_NUM_GPIO) != RESMGR_STATUS_ACCESS_OK)
        || (button == BUTTON_USER1_GPIO_PORT && pin == BUTTON_USER1_PIN &&
            ResMgr_Release(BUTTON_USER1_RIF_RES_TYP_GPIO, BUTTON_USER1_RIF_RES_NUM_GPIO) != RESMGR_STATUS_ACCESS_OK)
        || (button == BUTTON_USER2_GPIO_PORT && pin == BUTTON_USER2_PIN &&
            ResMgr_Release(BUTTON_USER2_RIF_RES_TYP_GPIO, BUTTON_USER2_RIF_RES_NUM_GPIO) != RESMGR_STATUS_ACCESS_OK))
    {
      ret = BSP_ERROR_MSP_FAILURE;
    }
  }
  return ret;
}

/**
  * @brief  Returns the selected button state.
  * @param  Button Button to be checked
  *          This parameter can be one of the following values:
  *            @arg  BUTTON_WAKEUP: Wakeup Push Button
  * @retval The Button GPIO pin value
  */
int32_t BSP_PB_GetState(Button_TypeDef Button)
{
  return HAL_GPIO_ReadPin(BUTTON_PORT[Button], BUTTON_PIN[Button]);
}

/**
  * @brief  This function handles Push-Button interrupt requests.
  * @param  Button Specifies the pin connected EXTI line
  * @retval None
  */
void BSP_PB_IRQHandler(Button_TypeDef Button)
{
  HAL_EXTI_IRQHandler(&hpb_exti[Button]);
}

/**
  * @brief  KEY EXTI line detection callbacks.
  * @retval None
  */
static void BUTTON_WAKEUP_EXTI_Callback(void)
{
  BSP_PB_Callback(BUTTON_WAKEUP);
}
/**
  * @brief  KEY EXTI line detection callbacks.
  * @retval None
  */
static void BUTTON_USER1_EXTI_Callback(void)
{
  BSP_PB_Callback(BUTTON_USER1);
}
/**
  * @brief  KEY EXTI line detection callbacks.
  * @retval None
  */
static void BUTTON_USER2_EXTI_Callback(void)
{
  BSP_PB_Callback(BUTTON_USER2);
}
/**
  * @brief  KEY EXTI line detection callbacks.
  * @retval None
  */
static void BUTTON_TAMPER_EXTI_Callback(void)
{
  BSP_PB_Callback(BUTTON_TAMPER);
}
/**
  * @brief  BSP Push Button callback
  * @param  Button Specifies the pin connected EXTI line
  * @retval None.
  */
__weak void BSP_PB_Callback(Button_TypeDef Button)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Button);
  /* This function should be implemented by the user application.
  It is called into this driver when an event on Button is triggered. */
}
#endif /* defined (CORE_CA35) || defined (CORE_CM33) */

#if (USE_BSP_COM_FEATURE > 0)
/**
  * @brief  Configures COM port.
  * @param  COM COM port to be configured.
  *          This parameter can be COM1, COM2, COM3
  * @param  COM_Init Pointer to a UART_HandleTypeDef structure that contains the
  *                configuration information for the specified USART peripheral.
  * @retval BSP status
  */
int32_t BSP_COM_Init(COM_TypeDef COM, COM_InitTypeDef *COM_Init)
{
  int32_t ret = BSP_ERROR_NONE;

  if (COM >= COMn)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Set the COM Instance */
    hcom_uart[COM].Instance = COM_USART[COM];

#if defined (CORE_CM33)
    if (!IS_DEVELOPER_BOOT_MODE() && COM == COM_VCP_CM33)
    {
      if (ResMgr_Request(COM_CM33_RIF_RES_TYP_TX_PIN, COM_CM33_RIF_RES_NUM_TX_PIN) != RESMGR_STATUS_ACCESS_OK
          && ResMgr_Request(COM_CM33_RIF_RES_TYP_RX_PIN, COM_CM33_RIF_RES_NUM_RX_PIN) != RESMGR_STATUS_ACCESS_OK
          && ResMgr_Request(COM_CM33_RIF_RES_TYP_UART, COM_CM33_RIF_RES_NUM_UART) != RESMGR_STATUS_ACCESS_OK)
      {
        return BSP_ERROR_MSP_FAILURE;
      }
    }
#endif /* defined (CORE_CM33) */

    /* Init the UART Msp */
#if (USE_HAL_UART_REGISTER_CALLBACKS == 0)
    USART_MspInit(&hcom_uart[COM]);
#else
    if (IsComMspCbValid[COM] == 0U)
    {
      if (BSP_COM_RegisterDefaultMspCallbacks(COM) != BSP_ERROR_NONE)
      {
        return BSP_ERROR_MSP_FAILURE;
      }
    }
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */

    if (MX_USART_Init(&hcom_uart[COM], COM_Init) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
  }

  return ret;
}

/**
  * @brief  DeInit COM port.
  * @param  COM COM port to be configured.
  *          This parameter can be COM1, COM2, COM3
  * @retval BSP status
  */
int32_t BSP_COM_DeInit(COM_TypeDef COM)
{
  int32_t ret = BSP_ERROR_NONE;

  if (COM >= COMn)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* USART configuration */
    hcom_uart[COM].Instance = COM_USART[COM];

    if (HAL_UART_DeInit(&hcom_uart[COM]) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }

#if (USE_HAL_UART_REGISTER_CALLBACKS == 0)
    USART_MspDeInit(&hcom_uart[COM]);
#endif /* (USE_HAL_UART_REGISTER_CALLBACKS == 0) */
  }

  return ret;
}

/**
  * @brief  Configures COM<n> port.
  * @param  huart USART handle
  * @param  COM_Init Pointer to a UART_HandleTypeDef structure that contains the
  *                  configuration information for the specified USART peripheral.
  * @retval HAL error code
  */
__weak HAL_StatusTypeDef MX_USART_Init(UART_HandleTypeDef *huart, MX_UART_InitTypeDef *COM_Init)
{
  HAL_StatusTypeDef ErrorCode;

  /* USART configuration */
  huart->Init.BaudRate     = (uint32_t)COM_Init->BaudRate;
  huart->Init.Mode         = UART_MODE_TX_RX;
  huart->Init.Parity       = (uint32_t)COM_Init->Parity;
  huart->Init.WordLength   = (uint32_t)COM_Init->WordLength;
  huart->Init.StopBits     = (uint32_t)COM_Init->StopBits;
  huart->Init.HwFlowCtl    = (uint32_t)COM_Init->HwFlowCtl;
  huart->Init.OverSampling = UART_OVERSAMPLING_8;
  ErrorCode = HAL_UART_Init(huart);

  return ErrorCode;
}

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
/**
  * @brief Register Default COM Msp Callbacks
  * @param  COM COM port to be configured.
  * @retval BSP status
  */
int32_t BSP_COM_RegisterDefaultMspCallbacks(COM_TypeDef COM)
{
  int32_t ret = BSP_ERROR_NONE;

  if (COM >= COMn)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    __HAL_UART_RESET_HANDLE_STATE(&hcom_uart[COM]);

    /* Register default MspInit/MspDeInit Callback */
    if (HAL_UART_RegisterCallback(&hcom_uart[COM], HAL_UART_MSPINIT_CB_ID, USART_MspInit) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else if (HAL_UART_RegisterCallback(&hcom_uart[COM], HAL_UART_MSPDEINIT_CB_ID, USART_MspDeInit) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else
    {
      IsComMspCbValid[COM] = 1U;
    }
  }

  /* BSP status */
  return ret;
}

/**
  * @brief  Register COM<n> Msp Callback registering
  * @param  COM COM port to be configured.
  * @param  Callbacks     pointer to COM MspInit/MspDeInit callback functions
  * @retval BSP status
  */
int32_t BSP_COM_RegisterMspCallbacks(COM_TypeDef COM, BSP_COM_Cb_t *Callback)
{
  int32_t ret = BSP_ERROR_NONE;

  if (COM >= COMn)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    __HAL_UART_RESET_HANDLE_STATE(&hcom_uart[COM]);

    /* Register MspInit/MspDeInit Callbacks */
    if (HAL_UART_RegisterCallback(&hcom_uart[COM], HAL_UART_MSPINIT_CB_ID, Callback->pMspInitCb) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else if (HAL_UART_RegisterCallback(&hcom_uart[COM], HAL_UART_MSPDEINIT_CB_ID, Callback->pMspDeInitCb) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else
    {
      IsComMspCbValid[COM] = 1U;
    }
  }

  /* BSP status */
  return ret;
}
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */

#if (USE_COM_LOG > 0)
/**
  * @brief  Select the active COM port.
  * @param  COM COM port to be activated.
  *          This parameter can be COM1, COM2, COM3
  * @retval BSP status
  */
int32_t BSP_COM_SelectLogPort(COM_TypeDef COM)
{
  if (COM_ActiveLogPort != COM)
  {
    COM_ActiveLogPort = COM;
  }
  return BSP_ERROR_NONE;
}

#ifdef __GNUC__
int __io_putchar(int ch)
#else
int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
{
  (void)HAL_UART_Transmit(&hcom_uart[COM_ActiveLogPort], (uint8_t *) &ch, 1, COM_POLL_TIMEOUT);
  return ch;
}

#ifdef __GNUC__
int __io_getchar(void)
{
  uint8_t ch = 0;
  HAL_UART_Receive(&hcom_uart[COM_ActiveLogPort], (uint8_t *)&ch, 1, COM1_RX_TIMEOUT);
  return ch;
}
#endif /* __GNUC__ */
#endif /* USE_COM_LOG */
#endif /* USE_BSP_COM_FEATURE */

/** @defgroup STM32MP257F_DK_LOW_LEVEL_Private_Functions Private Functions
  * @{
  */

#if (USE_BSP_COM_FEATURE > 0)
/**
  * @brief  Initializes UART MSP.
  * @param  huart UART handle
  * @retval None
  */
static void USART_MspInit(UART_HandleTypeDef *huart)
{
  GPIO_InitTypeDef gpio_init_structure;

  gpio_init_structure.Mode = GPIO_MODE_AF_PP;
  gpio_init_structure.Speed = GPIO_SPEED_FREQ_HIGH;
  gpio_init_structure.Pull = GPIO_PULLUP;

#if defined (CORE_CA35) || defined (CORE_CM33)


  if (huart->Instance == COM_CA35_UART)
  {
    /* USART2 clock config */
    (void)ResMgr_Request(RESMGR_RESOURCE_RIFSC, STM32MP25_RIFSC_USART2_ID);

    /* Enable USART clock */
    COM_CA35_TX_GPIO_CLK_ENABLE();
    COM_CA35_RX_GPIO_CLK_ENABLE();
    COM_CA35_CLK_ENABLE();
    /* Configure USART Tx as alternate function */
    gpio_init_structure.Pin = COM_CA35_TX_PIN;
    gpio_init_structure.Alternate = COM_CA35_TX_AF;
    HAL_GPIO_Init(COM_CA35_TX_GPIO_PORT, &gpio_init_structure);

    /* Configure USART Rx as alternate function */
    gpio_init_structure.Pin = COM_CA35_RX_PIN;
    gpio_init_structure.Alternate = COM_CA35_RX_AF;
    HAL_GPIO_Init(COM_CA35_RX_GPIO_PORT, &gpio_init_structure);
  }
  else if (huart->Instance == COM_CM33_UART)
  {

    /* Enable UART clock */
    COM_CM33_CLK_ENABLE();

    if (!COM_CM33_TX_GPIO_IS_CLK_ENABLED())
    {
      if (RESMGR_STATUS_ACCESS_OK == ResMgr_Request(RESMGR_RESOURCE_RIF_RCC, RESMGR_RCC_RESOURCE(96)))
      {
        COM_CM33_TX_GPIO_CLK_ENABLE();
      }
    }
    if (!COM_CM33_RX_GPIO_IS_CLK_ENABLED())
    {
      if (RESMGR_STATUS_ACCESS_OK == ResMgr_Request(RESMGR_RESOURCE_RIF_RCC, RESMGR_RCC_RESOURCE(96)))
      {
        COM_CM33_RX_GPIO_CLK_ENABLE();
      }
    }

    /* Configure USART Tx as alternate function */
    gpio_init_structure.Pin = COM_CM33_TX_PIN;
    gpio_init_structure.Alternate = COM_CM33_TX_AF;
    HAL_GPIO_Init(COM_CM33_TX_GPIO_PORT, &gpio_init_structure);

    /* Configure USART Rx as alternate function */
    gpio_init_structure.Pin = COM_CM33_RX_PIN;
    gpio_init_structure.Alternate = COM_CM33_RX_AF;
    HAL_GPIO_Init(COM_CM33_RX_GPIO_PORT, &gpio_init_structure);
  }
  else if (huart->Instance == COM_CM0PLUS_UART)
  {
    /* LPUART clock config */
    (void)ResMgr_Request(RESMGR_RESOURCE_RIFSC, STM32MP25_RIFSC_LPUART1_ID);

    /* Enable Backup domain access */
    HAL_PWR_EnableBkUpD3Access();

    /* Enable USART clock */
    COM_CM0PLUS_TX_GPIO_CLK_ENABLE();
    COM_CM0PLUS_RX_GPIO_CLK_ENABLE();
    COM_CM0PLUS_CLK_ENABLE();

    /* Configure USART Tx as alternate function */
    gpio_init_structure.Pin = COM_CM0PLUS_TX_PIN;
    gpio_init_structure.Alternate = COM_CM0PLUS_TX_AF;
    HAL_GPIO_Init(COM_CM0PLUS_TX_GPIO_PORT, &gpio_init_structure);

    /* Configure USART Rx as alternate function */
    gpio_init_structure.Pin = COM_CM0PLUS_RX_PIN;
    gpio_init_structure.Alternate = COM_CM0PLUS_RX_AF;
    HAL_GPIO_Init(COM_CM0PLUS_RX_GPIO_PORT, &gpio_init_structure);
  }

#elif defined(CORE_CM0PLUS)
  HAL_PWR_EnableBkUpAccess();
  /* Configure USART Tx as alternate function */
  gpio_init_structure.Pin = COM_CM0PLUS_TX_PIN;
  gpio_init_structure.Alternate = COM_CM0PLUS_TX_AF;
  HAL_GPIO_Init(COM_CM0PLUS_TX_GPIO_PORT, &gpio_init_structure);

  /* Configure USART Rx as alternate function */
  gpio_init_structure.Pin = COM_CM0PLUS_RX_PIN;
  gpio_init_structure.Alternate = COM_CM0PLUS_RX_AF;
  HAL_GPIO_Init(COM_CM0PLUS_RX_GPIO_PORT, &gpio_init_structure);
#endif /* CORE_CA35 || CORE_CM33 */
}

/**
  * @brief  Initialize USART6 Msp part
  * @param  huart UART handle
  * @retval None
  */
static void USART_MspDeInit(UART_HandleTypeDef *huart)
{
  GPIO_InitTypeDef          gpio_init_structure;

#if defined (CORE_CA35) || defined (CORE_CM33)
  if (huart->Instance == COM_CA35_UART)
  {
    /* COM GPIO pin configuration */
    gpio_init_structure.Pin  = COM_CA35_TX_PIN;
    HAL_GPIO_DeInit(COM_CA35_TX_GPIO_PORT, gpio_init_structure.Pin);

    gpio_init_structure.Pin  = COM_CA35_RX_PIN;
    HAL_GPIO_DeInit(COM_CA35_RX_GPIO_PORT, gpio_init_structure.Pin);

    /* Disable USART clock */
    COM_CA35_CLK_DISABLE();
  }
  else if (huart->Instance == COM_CM33_UART)
  {
    /* COM GPIO pin configuration */
    gpio_init_structure.Pin  = COM_CM33_TX_PIN;
    HAL_GPIO_DeInit(COM_CM33_TX_GPIO_PORT, gpio_init_structure.Pin);

    gpio_init_structure.Pin  = COM_CM33_RX_PIN;
    HAL_GPIO_DeInit(COM_CM33_RX_GPIO_PORT, gpio_init_structure.Pin);

    /* Disable USART clock */
    COM_CM33_CLK_DISABLE();

    /* Release all hardware resources acquired using resource manager */
    (void)ResMgr_Release(RESMGR_RESOURCE_RIFSC, STM32MP25_RIFSC_UART5_ID);
    (void)ResMgr_Release(COM_CM33_RIF_RES_TYP_TX_PIN, COM_CM33_RIF_RES_NUM_TX_PIN);
    (void)ResMgr_Release(COM_CM33_RIF_RES_TYP_RX_PIN, COM_CM33_RIF_RES_NUM_RX_PIN);
  }
  else if (huart->Instance == COM_CM0PLUS_UART)
  {
    /* COM GPIO pin configuration */
    gpio_init_structure.Pin  = COM_CM0PLUS_TX_PIN;
    HAL_GPIO_DeInit(COM_CM0PLUS_TX_GPIO_PORT, gpio_init_structure.Pin);

    gpio_init_structure.Pin  = COM_CM0PLUS_RX_PIN;
    HAL_GPIO_DeInit(COM_CM0PLUS_RX_GPIO_PORT, gpio_init_structure.Pin);

    /* Disable USART clock */
    COM_CM0PLUS_CLK_DISABLE();
  }

#elif defined(CORE_CM0PLUS)
  /* COM GPIO pin configuration */
  gpio_init_structure.Pin  = COM_CM0PLUS_TX_PIN;
  HAL_GPIO_DeInit(COM_CM0PLUS_TX_GPIO_PORT, gpio_init_structure.Pin);

  gpio_init_structure.Pin  = COM_CM0PLUS_RX_PIN;
  HAL_GPIO_DeInit(COM_CM0PLUS_RX_GPIO_PORT, gpio_init_structure.Pin);
#endif /* CORE_CA35 || CORE_CM33 */
}
#endif /* USE_BSP_COM_FEATURE */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
