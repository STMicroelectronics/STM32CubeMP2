/**
  ******************************************************************************
  * @file    stm32mp215f_disco.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for STM32MP215F_DK:
  *          LEDs
  *          push-buttons
  *          COM ports
  *          hardware resources.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STM32MP215F_DK
#define STM32MP215F_DK

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32mp215f_disco_conf.h"
#include "stm32mp215f_disco_errno.h"
#include "res_mgr.h"

#if (USE_BSP_COM_FEATURE > 0)
#if (USE_COM_LOG > 0)
#ifndef __GNUC__
#include "stdio.h"
#endif /* __GNUC__ */
#endif /* USE_COM_LOG */
#endif /* USE_BSP_COM_FEATURE */

#if !defined (USE_BSP)
#define USE_BSP
#endif /* USE_BSP */

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32MP215F_DK
  * @{
  */

/** @addtogroup LOW_LEVEL
  * @{
  */

/** @defgroup Exported_Types Exported Types
  * @{
  */
typedef enum
{
  LED1 = 0U,
  LED_RED = LED1,
  LED2 = 1U,
  LED_GREEN = LED2,
  LED3 = 2U,
  LED_ORANGE = LED3,
  LED4 = 3U,
  LED_BLUE = LED4,
  LEDn
} Led_TypeDef;

typedef enum
{
  BUTTON_WAKEUP = 0U,
  BUTTON_USER1,
  BUTTON_USER2,
  BUTTON_TAMPER,
  BUTTONn
} Button_TypeDef;

typedef enum
{
  BUTTON_MODE_GPIO = 0U,
  BUTTON_MODE_EXTI = 1U
} ButtonMode_TypeDef;

#if (USE_BSP_COM_FEATURE > 0)
typedef enum
{
  /* Define COM_VCP_CA35, COM_VCP_CM33 to match COM_USART[] table in stm32mp215f_disco.c
   * These values are to be used in examples
   */
  COM_VCP_CA35 = 0U,
  COM_VCP_CM33 = 1U,
  /* Define COM1, COM2 aliases
   * These values are to be used in Validation applications
   * They allow to use same COM<n> whatever the used core
   * and avoid ifdefs in validation code
   */
#if defined(CORE_CA35)
  COM1 = COM_VCP_CA35,
  COM2 = COM_VCP_CM33,
#elif defined(CORE_CM33)
  COM2 = COM_VCP_CA35,
  COM1 = COM_VCP_CM33,
#endif /* CORE_<...> */
  COMn
} COM_TypeDef;

typedef enum
{
  COM_STOPBITS_1     =   UART_STOPBITS_1,
  COM_STOPBITS_2     =   UART_STOPBITS_2,
} COM_StopBitsTypeDef;

typedef enum
{
  COM_PARITY_NONE     =  UART_PARITY_NONE,
  COM_PARITY_EVEN     =  UART_PARITY_EVEN,
  COM_PARITY_ODD      =  UART_PARITY_ODD,
} COM_ParityTypeDef;

typedef enum
{
  COM_HWCONTROL_NONE    =  UART_HWCONTROL_NONE,
  COM_HWCONTROL_RTS     =  UART_HWCONTROL_RTS,
  COM_HWCONTROL_CTS     =  UART_HWCONTROL_CTS,
  COM_HWCONTROL_RTS_CTS =  UART_HWCONTROL_RTS_CTS,
} COM_HwFlowCtlTypeDef;

typedef enum
{
  COM_WORDLENGTH_7B = UART_WORDLENGTH_7B,
  COM_WORDLENGTH_8B = UART_WORDLENGTH_8B,
  COM_WORDLENGTH_9B = UART_WORDLENGTH_9B,
} COM_WordLengthTypeDef;

typedef struct
{
  uint32_t              BaudRate;
  COM_WordLengthTypeDef WordLength;
  COM_StopBitsTypeDef   StopBits;
  COM_ParityTypeDef     Parity;
  COM_HwFlowCtlTypeDef  HwFlowCtl;
} COM_InitTypeDef;

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
typedef struct
{
  void (* pMspInitCb)(UART_HandleTypeDef *);
  void (* pMspDeInitCb)(UART_HandleTypeDef *);
} BSP_COM_Cb_t;
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
#endif /* USE_BSP_COM_FEATURE */

/**
  * @}
  */

/** @defgroup Exported_Constants Exported Constants
  * @{
  */
/**
  * @brief  Define for STM32MP215F DK board
  */
#if !defined (USE_STM32MP215F_DK)
#define USE_STM32MP215F_DK
#endif /* USE_STM32MP215F_DK */
/**
  * @brief STM32MP215F DK BSP Driver version number V1.0.0
  */
#define STM32MP215F_DK_BSP_VERSION_MAIN   (0x01U) /*!< [31:24] main version */
#define STM32MP215F_DK_BSP_VERSION_SUB1   (0x00U) /*!< [23:16] sub1 version */
#define STM32MP215F_DK_BSP_VERSION_SUB2   (0x00U) /*!< [15:8]  sub2 version */
#define STM32MP215F_DK_BSP_VERSION_RC     (0x00U) /*!< [7:0]  release candidate */
#define STM32MP215F_DK_BSP_VERSION        ((STM32MP215F_DK_BSP_VERSION_MAIN << 24)\
                                           |(STM32MP215F_DK_BSP_VERSION_SUB1 << 16)\
                                           |(STM32MP215F_DK_BSP_VERSION_SUB2 << 8 )\
                                           |(STM32MP215F_DK_BSP_VERSION_RC))
#define STM32MP215F_DK_BSP_BOARD_NAME  "STM32MP215F-DK";
#define STM32MP215F_DK_BSP_BOARD_ID    "MB2059-A01";

/** @defgroup LOW_LEVEL_LED EVAL LOW LEVEL LED
  * @{
  */
#if defined (CORE_CA35) || defined (CORE_CM33)
#define LED1_GPIO_PORT                   GPIOH
#define LED1_PIN                         GPIO_PIN_4
#define LED1_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOH_CLK_ENABLE()
#define LED1_GPIO_CLK_DISABLE()          __HAL_RCC_GPIOH_CLK_DISABLE()
#define LED1_GPIO_IS_CLK_ENABLED()       __HAL_RCC_GPIOH_IS_CLK_ENABLED()
#define LED1_GPIO_RIF_RES_TYP_GPIO       RESMGR_RESOURCE_RIF_GPIOH
#define LED1_GPIO_RIF_RES_NUM_GPIO       RESMGR_GPIO_PIN(4)

#if defined (USE_STM32MP215F_DK)
#define LED2_GPIO_PORT                   GPIOF
#define LED2_PIN                         GPIO_PIN_10
#define LED2_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOF_CLK_ENABLE()
#define LED2_GPIO_CLK_DISABLE()          __HAL_RCC_GPIOF_CLK_DISABLE()
#define LED2_GPIO_IS_CLK_ENABLED()       __HAL_RCC_GPIOF_IS_CLK_ENABLED()
#define LED2_GPIO_RIF_RES_TYP_GPIO       RESMGR_RESOURCE_RIF_GPIOF
#define LED2_GPIO_RIF_RES_NUM_GPIO       RESMGR_GPIO_PIN(10)

#define LED3_GPIO_PORT                   GPIOH
#define LED3_PIN                         GPIO_PIN_12
#define LED3_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOH_CLK_ENABLE()
#define LED3_GPIO_CLK_DISABLE()          __HAL_RCC_GPIOH_CLK_DISABLE()
#define LED3_GPIO_IS_CLK_ENABLED()       __HAL_RCC_GPIOH_IS_CLK_ENABLED()
#define LED3_GPIO_RIF_RES_TYP_GPIO       RESMGR_RESOURCE_RIF_GPIOH
#define LED3_GPIO_RIF_RES_NUM_GPIO       RESMGR_GPIO_PIN(12)

#define LED4_GPIO_PORT                   GPIOZ
#define LED4_PIN                         GPIO_PIN_3
#define LED4_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOZ_CLK_ENABLE()
#define LED4_GPIO_CLK_DISABLE()          __HAL_RCC_GPIOZ_CLK_DISABLE()
#define LED4_GPIO_IS_CLK_ENABLED()       __HAL_RCC_GPIOZ_IS_CLK_ENABLED()
#define LED4_GPIO_RIF_RES_TYP_GPIO       RESMGR_RESOURCE_RIF_GPIOZ
#define LED4_GPIO_RIF_RES_NUM_GPIO       RESMGR_GPIO_PIN(3)

#else
#error "Please define target board"
#endif /* USE_STM32MP215F_DK */

#endif /* CORE_CA35 || CORE_CM33 */
/**
  * @}
  */

/** @defgroup LOW_LEVEL_BUTTON LOW LEVEL BUTTON
  * @{
  */
/* Button state */
#define BUTTON_RELEASED                    0U
#define BUTTON_PRESSED                     1U
/**
  * @brief Wakeup push-button
  */
#if defined (CORE_CA35) || defined (CORE_CM33)
#define BUTTON_WAKEUP_PIN                   GPIO_PIN_0
#define BUTTON_WAKEUP_GPIO_PORT             GPIOA
#define BUTTON_WAKEUP_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOA_CLK_ENABLE()
#define BUTTON_WAKEUP_GPIO_CLK_DISABLE()    __HAL_RCC_GPIOA_CLK_DISABLE()
#define BUTTON_WAKEUP_GPIO_IS_CLK_ENABLE()  __HAL_RCC_GPIOA_IS_CLK_ENABLED()
#define BUTTON_WAKEUP_EXTI_IRQn             EXTI2_0_IRQn
#define BUTTON_WAKEUP_EXTI_LINE             EXTI2_LINE_0
#define BUTTON_WAKEUP_RIF_RES_TYP_GPIO      RESMGR_RESOURCE_RIF_GPIOA
#define BUTTON_WAKEUP_RIF_RES_NUM_GPIO      RESMGR_GPIO_PIN(0)
#endif /* CORE_CA35 || CORE_CM33 */
/**
  * @brief USER1 push-button
  */
#if defined (CORE_CA35) || defined (CORE_CM33)
#define BUTTON_USER1_PIN                   GPIO_PIN_4
#define BUTTON_USER1_GPIO_PORT             GPIOC
#define BUTTON_USER1_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOC_CLK_ENABLE()
#define BUTTON_USER1_GPIO_CLK_DISABLE()    __HAL_RCC_GPIOC_CLK_DISABLE()
#define BUTTON_USER1_GPIO_IS_CLK_ENABLE()  __HAL_RCC_GPIOC_IS_CLK_ENABLED()
#define BUTTON_USER1_EXTI_IRQn             EXTI2_4_IRQn
#define BUTTON_USER1_EXTI_IRQ_HANDLER      EXTI2_4_IRQHandler
#define BUTTON_USER1_EXTI_LINE             EXTI2_LINE_4
#define BUTTON_USER1_RIF_RES_TYP_GPIO      RESMGR_RESOURCE_RIF_GPIOC
#define BUTTON_USER1_RIF_RES_NUM_GPIO      RESMGR_GPIO_PIN(4)
#endif /* CORE_CA35 || CORE_CM33 */
/**
  * @brief USER2 push-button
  */
#if defined (CORE_CA35) || defined (CORE_CM33)
#define BUTTON_USER2_PIN                   GPIO_PIN_7
#define BUTTON_USER2_GPIO_PORT             GPIOF
#define BUTTON_USER2_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOF_CLK_ENABLE()
#define BUTTON_USER2_GPIO_CLK_DISABLE()    __HAL_RCC_GPIOF_CLK_DISABLE()
#define BUTTON_USER2_GPIO_IS_CLK_ENABLE()  __HAL_RCC_GPIOF_IS_CLK_ENABLED()
#define BUTTON_USER2_EXTI_IRQn             EXTI2_7_IRQn
#define BUTTON_USER2_EXTI_IRQ_HANDLER      EXTI2_7_IRQHandler
#define BUTTON_USER2_EXTI_LINE             EXTI2_LINE_7
#define BUTTON_USER2_RIF_RES_TYP_GPIO      RESMGR_RESOURCE_RIF_GPIOF
#define BUTTON_USER2_RIF_RES_NUM_GPIO      RESMGR_GPIO_PIN(7)
#endif /* CORE_CA35 || CORE_CM33 */
/**
  * @brief TAMPER push-button
  */
#if defined (CORE_CA35) || defined (CORE_CM33)
#define BUTTON_TAMPER_PIN                   GPIO_PIN_6
#define BUTTON_TAMPER_GPIO_PORT             GPIOF
#define BUTTON_TAMPER_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOF_CLK_ENABLE()
#define BUTTON_TAMPER_GPIO_CLK_DISABLE()    __HAL_RCC_GPIOF_CLK_DISABLE()
#define BUTTON_TAMPER_GPIO_IS_CLK_ENABLE()  __HAL_RCC_GPIOF_IS_CLK_ENABLED()
#define BUTTON_TAMPER_EXTI_IRQn             EXTI2_6_IRQn
#define BUTTON_TAMPER_EXTI_IRQ_HANDLER      EXTI2_6_IRQHandler
#define BUTTON_TAMPER_EXTI_LINE             EXTI2_LINE_6
#define BUTTON_TAMPER_RIF_RES_TYP_GPIO      RESMGR_RESOURCE_RIF_GPIOF
#define BUTTON_TAMPER_RIF_RES_NUM_GPIO      RESMGR_GPIO_PIN(6)
#endif /* CORE_CA35 || CORE_CM33 */
/**
  * @}
  */

/** @defgroup DK_LOW_LEVEL_COM LOW LEVEL COM
  * @{
  */
#if (USE_BSP_COM_FEATURE > 0)
/**
  * @brief Definition for COM port1
  */
#if defined (CORE_CA35) || defined (CORE_CM33)

/*********************** COM_CA35 ******************************/
#define COM_CA35_UART                       USART2
#define COM_CA35_CLK_ENABLE()               __HAL_RCC_USART2_CLK_ENABLE()
#define COM_CA35_CLK_DISABLE()              __HAL_RCC_USART2_CLK_DISABLE()

#define COM_CA35_TX_PIN                     GPIO_PIN_4
#define COM_CA35_TX_GPIO_PORT               GPIOA
#define COM_CA35_TX_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOA_CLK_ENABLE()
#define COM_CA35_TX_GPIO_CLK_DISABLE()      __HAL_RCC_GPIOA_CLK_DISABLE()
#define COM_CA35_TX_GPIO_IS_CLK_ENABLED()   __HAL_RCC_GPIOA_IS_CLK_ENABLED()
#define COM_CA35_TX_AF                      GPIO_AF6_USART2
#define COM_CA35_RIF_RES_TYP_TX_PIN         RESMGR_RESOURCE_RIF_GPIOA
#define COM_CA35_RIF_RES_NUM_TX_PIN         RESMGR_GPIO_PIN(4)

#define COM_CA35_RX_PIN                     GPIO_PIN_8
#define COM_CA35_RX_GPIO_PORT               GPIOA
#define COM_CA35_RX_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOA_CLK_ENABLE()
#define COM_CA35_RX_GPIO_CLK_DISABLE()      __HAL_RCC_GPIOA_CLK_DISABLE()
#define COM_CA35_RX_GPIO_IS_CLK_ENABLED()   __HAL_RCC_GPIOA_IS_CLK_ENABLED()
#define COM_CA35_RX_AF                      GPIO_AF8_USART2
#define COM_CA35_RIF_RES_TYP_RX_PIN         RESMGR_RESOURCE_RIF_GPIOA
#define COM_CA35_RIF_RES_NUM_RX_PIN         RESMGR_GPIO_PIN(8)

#define COM_CA35_RIF_RES_TYP_UART           RESMGR_RESOURCE_RIFSC
#define COM_CA35_RIF_RES_NUM_UART           RESMGR_RIFSC_USART2_ID

/*********************** COM_CM33 ******************************/
#define COM_CM33_UART                       UART4
#define COM_CM33_CLK_ENABLE()               __HAL_RCC_UART4_CLK_ENABLE()
#define COM_CM33_CLK_DISABLE()              __HAL_RCC_UART4_CLK_DISABLE()

#define COM_CM33_TX_PIN                     GPIO_PIN_7
#define COM_CM33_TX_GPIO_PORT               GPIOH
#define COM_CM33_TX_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOH_CLK_ENABLE()
#define COM_CM33_TX_GPIO_CLK_DISABLE()      __HAL_RCC_GPIOH_CLK_DISABLE()
#define COM_CM33_TX_GPIO_IS_CLK_ENABLED()   __HAL_RCC_GPIOH_IS_CLK_ENABLED()
#define COM_CM33_TX_AF                      GPIO_AF4_UART4
#define COM_CM33_RIF_RES_TYP_TX_PIN         RESMGR_RESOURCE_RIF_GPIOH
#define COM_CM33_RIF_RES_NUM_TX_PIN         RESMGR_GPIO_PIN(7)

#define COM_CM33_RX_PIN                     GPIO_PIN_8
#define COM_CM33_RX_GPIO_PORT               GPIOH
#define COM_CM33_RX_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOH_CLK_ENABLE()
#define COM_CM33_RX_GPIO_CLK_DISABLE()      __HAL_RCC_GPIOH_CLK_DISABLE()
#define COM_CM33_RX_GPIO_IS_CLK_ENABLED()   __HAL_RCC_GPIOH_IS_CLK_ENABLED()
#define COM_CM33_RX_AF                      GPIO_AF4_UART4
#define COM_CM33_RIF_RES_TYP_RX_PIN         RESMGR_RESOURCE_RIF_GPIOH
#define COM_CM33_RIF_RES_NUM_RX_PIN         RESMGR_GPIO_PIN(8)

#define COM_CM33_RIF_RES_TYP_UART           RESMGR_RESOURCE_RIFSC
#define COM_CM33_RIF_RES_NUM_UART           RESMGR_RIFSC_UART4_ID

#endif /* CORE_CA35 || CORE_CM33 */


#define MX_UART_InitTypeDef COM_InitTypeDef

#define COM_POLL_TIMEOUT    1000
#define COM1_RX_TIMEOUT     HAL_MAX_DELAY

#endif /* USE_BSP_COM_FEATURE */
/**
  * @}
  */

/**
  * @}
  */

/** @addtogroup Exported_Variables
  * @{
  */
extern EXTI_HandleTypeDef hpb_exti[];
#if (USE_BSP_COM_FEATURE > 0)
extern UART_HandleTypeDef hcom_uart[];
extern USART_TypeDef *COM_USART[];
#endif /* USE_BSP_COM_FEATURE */

/**
  * @}
  */

/** @addtogroup Exported_Functions
  * @{
  */
int32_t BSP_GetVersion(void);
const uint8_t *BSP_GetBoardName(void);
const uint8_t *BSP_GetBoardID(void);

int32_t BSP_LED_Init(Led_TypeDef Led);
int32_t BSP_LED_DeInit(Led_TypeDef Led);
int32_t BSP_LED_On(Led_TypeDef Led);
int32_t BSP_LED_Off(Led_TypeDef Led);
int32_t BSP_LED_Toggle(Led_TypeDef Led);
int32_t BSP_LED_GetState(Led_TypeDef Led);
int32_t BSP_PB_Init(Button_TypeDef Button, ButtonMode_TypeDef ButtonMode);
int32_t BSP_PB_DeInit(Button_TypeDef Button);
int32_t BSP_PB_GetState(Button_TypeDef Button);
void    BSP_PB_Callback(Button_TypeDef Button);
#if (USE_BSP_COM_FEATURE > 0)
int32_t  BSP_COM_Init(COM_TypeDef COM, COM_InitTypeDef *COM_Init);
int32_t  BSP_COM_DeInit(COM_TypeDef COM);
#if (USE_COM_LOG > 0)
int32_t  BSP_COM_SelectLogPort(COM_TypeDef COM);
#endif /* USE_COM_LOG */

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
int32_t BSP_COM_RegisterDefaultMspCallbacks(COM_TypeDef COM);
int32_t BSP_COM_RegisterMspCallbacks(COM_TypeDef COM, BSP_COM_Cb_t *Callback);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
HAL_StatusTypeDef MX_USART_Init(UART_HandleTypeDef *huart, MX_UART_InitTypeDef *COM_Init);
#endif /* USE_BSP_COM_FEATURE */

void BSP_PB_IRQHandler(Button_TypeDef Button);

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

#ifdef __cplusplus
}
#endif

#endif /* STM32MP215F_DK */
