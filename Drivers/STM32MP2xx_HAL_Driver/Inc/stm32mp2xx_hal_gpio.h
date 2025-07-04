/**
  ******************************************************************************
  * @file    stm32mp2xx_hal_gpio.h
  * @author  MCD Application Team
  * @brief   Header file of GPIO HAL module.
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
#ifndef STM32MP2xx_HAL_GPIO_H
#define STM32MP2xx_HAL_GPIO_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32mp2xx_hal_def.h"

/** @addtogroup STM32MP2xx_HAL_Driver
  * @{
  */

/** @defgroup GPIO GPIO
  * @brief GPIO HAL module driver
  * @{
  */

/* Exported types ------------------------------------------------------------*/

/** @defgroup GPIO_Exported_Types GPIO Exported Types
  * @{
  */
/**
  * @brief   GPIO Init structure definition
  */
typedef struct
{
  uint32_t Pin;        /*!< Specifies the GPIO pins to be configured.
                           This parameter can be any value of @ref GPIO_pins */

  uint32_t Mode;       /*!< Specifies the operating mode for the selected pins.
                           This parameter can be a value of @ref GPIO_mode */

  uint32_t Pull;       /*!< Specifies the Pull-up or Pull-Down activation for the selected pins.
                           This parameter can be a value of @ref GPIO_pull */

  uint32_t Speed;      /*!< Specifies the speed for the selected pins.
                           This parameter can be a value of @ref GPIO_speed */

  uint32_t Alternate;  /*!< Peripheral to be connected to the selected pins
                            This parameter can be a value of @ref GPIOEx_Alternate_function_selection */
} GPIO_InitTypeDef;

/**
  * @brief  GPIO Bit SET and Bit RESET enumeration
  */
typedef enum
{
  GPIO_PIN_RESET = 0U,
  GPIO_PIN_SET
} GPIO_PinState;
/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup GPIO_Exported_Constants GPIO Exported Constants
  * @{
  */
/** @defgroup GPIO_pins GPIO pins
  * @{
  */
#define GPIO_PIN_0                 ((uint16_t)0x0001)  /* Pin 0 selected    */
#define GPIO_PIN_1                 ((uint16_t)0x0002)  /* Pin 1 selected    */
#define GPIO_PIN_2                 ((uint16_t)0x0004)  /* Pin 2 selected    */
#define GPIO_PIN_3                 ((uint16_t)0x0008)  /* Pin 3 selected    */
#define GPIO_PIN_4                 ((uint16_t)0x0010)  /* Pin 4 selected    */
#define GPIO_PIN_5                 ((uint16_t)0x0020)  /* Pin 5 selected    */
#define GPIO_PIN_6                 ((uint16_t)0x0040)  /* Pin 6 selected    */
#define GPIO_PIN_7                 ((uint16_t)0x0080)  /* Pin 7 selected    */
#define GPIO_PIN_8                 ((uint16_t)0x0100)  /* Pin 8 selected    */
#define GPIO_PIN_9                 ((uint16_t)0x0200)  /* Pin 9 selected    */
#define GPIO_PIN_10                ((uint16_t)0x0400)  /* Pin 10 selected   */
#define GPIO_PIN_11                ((uint16_t)0x0800)  /* Pin 11 selected   */
#define GPIO_PIN_12                ((uint16_t)0x1000)  /* Pin 12 selected   */
#define GPIO_PIN_13                ((uint16_t)0x2000)  /* Pin 13 selected   */
#define GPIO_PIN_14                ((uint16_t)0x4000)  /* Pin 14 selected   */
#define GPIO_PIN_15                ((uint16_t)0x8000)  /* Pin 15 selected   */
#define GPIO_PIN_ALL               ((uint16_t)0xFFFF)  /* All pins selected */

#define GPIO_PIN_MASK              (0x0000FFFFu) /* PIN mask for assert test */
/**
  * @}
  */

/** @defgroup GPIO_mode GPIO mode
  * @brief GPIO Configuration Mode
  *        Elements values convention: 0xX0yz00YZ
  *           - X  : GPIO mode or EXTI Mode
  *           - y  : External IT or Event trigger detection
  *           - z  : IO configuration on External IT or Event
  *           - Y  : Output type (Push Pull or Open Drain)
  *           - Z  : IO Direction mode (Input, Output, Alternate or Analog)
  * @{
  */
#define  GPIO_MODE_INPUT                        (0x00000000U)   /*!< Input Floating Mode                   */
#define  GPIO_MODE_OUTPUT_PP                    (0x00000001U)   /*!< Output Push Pull Mode                 */
#define  GPIO_MODE_OUTPUT_OD                    (0x00000011U)   /*!< Output Open Drain Mode                */
#define  GPIO_MODE_AF_PP                        (0x00000002U)   /*!< Alternate Function Push Pull Mode     */
#define  GPIO_MODE_AF_OD                        (0x00000012U)   /*!< Alternate Function Open Drain Mode    */
#define  GPIO_MODE_ANALOG                       (0x00000003U)   /*!< Analog Mode  */

#define  GPIO_EXTI1_IT_RISING                    (0x10110000U)   /*!< External Interrupt Mode using EXTI1 with Rising edge trigger detection          */
#define  GPIO_EXTI1_IT_FALLING                   (0x10210000U)   /*!< External Interrupt Mode using EXTI1 with Falling edge trigger detection         */
#define  GPIO_EXTI1_IT_RISING_FALLING            (0x10310000U)   /*!< External Interrupt Mode using EXTI1 with Rising/Falling edge trigger detection  */
#define  GPIO_EXTI1_EVT_RISING                   (0x10120000U)   /*!< External Event Mode using EXTI1 with Rising edge trigger detection              */
#define  GPIO_EXTI1_EVT_FALLING                  (0x10220000U)   /*!< External Event Mode using EXTI1 with Falling edge trigger detection             */
#define  GPIO_EXTI1_EVT_RISING_FALLING           (0x10320000U)   /*!< External Event Mode using EXTI1 with Rising/Falling edge trigger detection      */

#define  GPIO_EXTI2_IT_RISING                    (0x20110000U)   /*!< External Interrupt Mode using EXTI2 with Rising edge trigger detection          */
#define  GPIO_EXTI2_IT_FALLING                   (0x20210000U)   /*!< External Interrupt Mode using EXTI2 with Falling edge trigger detection         */
#define  GPIO_EXTI2_IT_RISING_FALLING            (0x20310000U)   /*!< External Interrupt Mode using EXTI2 with Rising/Falling edge trigger detection  */
#define  GPIO_EXTI2_EVT_RISING                   (0x20120000U)   /*!< External Event Mode using EXTI2 with Rising edge trigger detection              */
#define  GPIO_EXTI2_EVT_FALLING                  (0x20220000U)   /*!< External Event Mode using EXTI2 with Falling edge trigger detection             */
#define  GPIO_EXTI2_EVT_RISING_FALLING           (0x20320000U)   /*!< External Event Mode using EXTI2 with Rising/Falling edge trigger detection      */


#define  GPIO_MODE_IT_RISING                    GPIO_EXTI2_IT_RISING
#define  GPIO_MODE_IT_FALLING                   GPIO_EXTI2_IT_FALLING
#define  GPIO_MODE_IT_RISING_FALLING            GPIO_EXTI2_IT_RISING_FALLING
#define  GPIO_MODE_EVT_RISING                   GPIO_EXTI2_EVT_RISING
#define  GPIO_MODE_EVT_FALLING                  GPIO_EXTI2_EVT_FALLING
#define  GPIO_MODE_EVT_RISING_FALLING           GPIO_EXTI2_EVT_RISING_FALLING

/**
  * @}
  */

/** @defgroup GPIO_speed GPIO speed
  * @brief GPIO Output Maximum frequency
  * @{
  */
#define  GPIO_SPEED_FREQ_LOW        (0x00000000U)  /*!< Low speed       */
#define  GPIO_SPEED_FREQ_MEDIUM     (0x00000001U)  /*!< Medium speed    */
#define  GPIO_SPEED_FREQ_HIGH       (0x00000002U)  /*!< High speed      */
#define  GPIO_SPEED_FREQ_VERY_HIGH  (0x00000003U)  /*!< Very high speed */
/**
  * @}
  */

/** @defgroup GPIO_pull GPIO pull
  * @brief GPIO Pull-Up or Pull-Down Activation
  * @{
  */
#define  GPIO_NOPULL        (0x00000000U)   /*!< No Pull-up or Pull-down activation  */
#define  GPIO_PULLUP        (0x00000001U)   /*!< Pull-up activation                  */
#define  GPIO_PULLDOWN      (0x00000002U)   /*!< Pull-down activation                */
/**
  * @}
  */

/** @defgroup GPIO_attributes GPIO attributes
  * @brief GPIO pin secure/non-secure  or/and privilege/non-privilege or/end CID attributes
  * @{
  */
#define GPIO_PIN_ATTR_PRIV_SELECT          0x01000000U
#define GPIO_PIN_ATTR_SEC_SELECT           0x02000000U
#define GPIO_PIN_ATTR_CID_SHARED_MASK      (GPIO_CIDCFGR0_SEMWLC3 |\
                                            GPIO_CIDCFGR0_SEMWLC2 | GPIO_CIDCFGR0_SEMWLC1 | GPIO_CIDCFGR0_SEMWLC0)
#define GPIO_PIN_ATTR_CID_SHARED_SELECT    0x10000000U
#define GPIO_PIN_ATTR_CID_STATIC_SELECT    0x20000000U
#define GPIO_PIN_CID_DISABLE               0x80000000U

#define GPIO_PIN_PRIV_VAL_Pos             0
#define GPIO_PIN_PRIV                     (GPIO_PIN_ATTR_PRIV_SELECT | (1U<<GPIO_PIN_PRIV_VAL_Pos))     /*!< Pin is privileged             */
#define GPIO_PIN_NPRIV                    (GPIO_PIN_ATTR_PRIV_SELECT)                                   /*!< Pin is unprivileged           */

#if defined (CORTEX_IN_SECURE_STATE)
#define GPIO_PIN_SEC_VAL_Pos              1
#define GPIO_PIN_SEC                      (GPIO_PIN_ATTR_SEC_SELECT | (1U<<GPIO_PIN_SEC_VAL_Pos))      /*!< Pin is secure                 */
#define GPIO_PIN_NSEC                     (GPIO_PIN_ATTR_SEC_SELECT)              /*!< Pin is non-secure             */
#endif /* __ARM_SECURE_STATE */

#define GPIO_PIN_CID_SHARED_0                   (GPIO_PIN_ATTR_CID_SHARED_SELECT | GPIO_CIDCFGR0_SEMWLC0)      /*!< Enable CID filtering (shared mode) and give pin control  to CID0 */
#define GPIO_PIN_CID_SHARED_1                   (GPIO_PIN_ATTR_CID_SHARED_SELECT | GPIO_CIDCFGR0_SEMWLC1)      /*!< Enable CID filtering (shared mode) and give pin control  to CID1 */
#define GPIO_PIN_CID_SHARED_2                   (GPIO_PIN_ATTR_CID_SHARED_SELECT | GPIO_CIDCFGR0_SEMWLC2)      /*!< Enable CID filtering (shared mode) and give pin control  to CID2 */
#define GPIO_PIN_CID_SHARED_3                   (GPIO_PIN_ATTR_CID_SHARED_SELECT | GPIO_CIDCFGR0_SEMWLC3)      /*!< Enable CID filtering (shared mode) and give pin control  to CID3 */

#define GPIO_PIN_CID_STATIC_0                   (GPIO_PIN_ATTR_CID_STATIC_SELECT|(0<<GPIO_CIDCFGR0_SCID_Pos))   /*!< Enable CID filtering (static mode) and give pin control  to CID0 */
#define GPIO_PIN_CID_STATIC_1                   (GPIO_PIN_ATTR_CID_STATIC_SELECT|(1<<GPIO_CIDCFGR0_SCID_Pos))   /*!< Enable CID filtering (static mode) and give pin control  to CID1 */
#define GPIO_PIN_CID_STATIC_2                   (GPIO_PIN_ATTR_CID_STATIC_SELECT|(2<<GPIO_CIDCFGR0_SCID_Pos))   /*!< Enable CID filtering (static mode) and give pin control  to CID2 */
#define GPIO_PIN_CID_STATIC_3                   (GPIO_PIN_ATTR_CID_STATIC_SELECT|(3<<GPIO_CIDCFGR0_SCID_Pos))   /*!< Enable CID filtering (static mode) and give pin control  to CID3 */

/**
  * @}
  */

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/
/** @defgroup GPIO_Exported_Macros GPIO Exported Macros
  * @{
  */

/**
  * @brief  Check whether the specified EXTI line flag is set or not.
  * @param __EXTI_LINE__ specifies the EXTI line flag to check.
  *         This parameter can be GPIO_PIN_x where x can be(0..15)
  * @retval The new state of __EXTI_LINE__ (SET or RESET).
  */
#if !defined(CORE_CM0PLUS)
#define __HAL_GPIO_EXTI1_GET_FLAG(__EXTI_LINE__)       (EXTI1->PR1 & (__EXTI_LINE__))
#endif /*!defined(CORE_CM0PLUS)*/
#define __HAL_GPIO_EXTI_GET_FLAG(__EXTI_LINE__)       (EXTI2->PR1 & (__EXTI_LINE__))

/**
  * @brief  Clear the EXTI's line pending flags.
  * @param __EXTI_LINE__ specifies the EXTI lines flags to clear.
  *         This parameter can be any combination of GPIO_PIN_x where x can be (0..15)
  * @retval None
  */
#if !defined(CORE_CM0PLUS)
#define __HAL_GPIO_EXTI1_CLEAR_FLAG(__EXTI_LINE__)     (EXTI1->PR1 = (__EXTI_LINE__))
#endif /*!defined(CORE_CM0PLUS)*/
#define __HAL_GPIO_EXTI_CLEAR_FLAG(__EXTI_LINE__)     (EXTI2->PR1 = (__EXTI_LINE__))

/**
  * @brief  Checks whether the specified EXTI line is asserted or not for Rising edge.

  * @param  __EXTI_LINE__: specifies the EXTI line to check.
  *          This parameter can be GPIO_PIN_x where x can be(0..15)
  * @retval The new state of __EXTI_LINE__ (SET or RESET).
  */
#if !defined(CORE_CM0PLUS)
#define __HAL_GPIO_EXTI1_GET_RISING_IT(__EXTI_LINE__) (EXTI1->RPR1 & (__EXTI_LINE__))
#endif /*!defined(CORE_CM0PLUS)*/
#define __HAL_GPIO_EXTI_GET_RISING_IT(__EXTI_LINE__) (EXTI2->RPR1 & (__EXTI_LINE__))

/**
  * @brief  Checks whether the specified EXTI line is asserted or not for Falling edge.

  * @param  __EXTI_LINE__: specifies the EXTI line to check.
  *          This parameter can be GPIO_PIN_x where x can be(0..15)
  * @retval The new state of __EXTI_LINE__ (SET or RESET).
  */
#if !defined(CORE_CM0PLUS)
#define __HAL_GPIO_EXTI1_GET_FALLING_IT(__EXTI_LINE__) (EXTI1->FPR1 & (__EXTI_LINE__))
#endif /*!defined(CORE_CM0PLUS)*/
#define __HAL_GPIO_EXTI_GET_FALLING_IT(__EXTI_LINE__) (EXTI2->FPR1 & (__EXTI_LINE__))

/**
  * @brief  Clears the EXTI's line pending bits for Risng edge.
  * @param  __EXTI_LINE__: specifies the EXTI lines to clear.
  *          This parameter can be any combination of GPIO_PIN_x where x can be (0..15)
  * @retval None
  */
#if !defined(CORE_CM0PLUS)
#define __HAL_GPIO_EXTI1_CLEAR_RISING_IT(__EXTI_LINE__) do { \
                                                             EXTI1->RPR1 = (__EXTI_LINE__); \
                                                           } while (0);
#endif /*!defined(CORE_CM0PLUS)*/
#define __HAL_GPIO_EXTI_CLEAR_RISING_IT(__EXTI_LINE__) do { \
                                                            EXTI2->RPR1 = (__EXTI_LINE__); \
                                                          } while (0);

/**
  * @brief  Clears the EXTI's line pending bits for Falling edge.
  * @param  __EXTI_LINE__: specifies the EXTI lines to clear.
  *          This parameter can be any combination of GPIO_PIN_x where x can be (0..15)
  * @retval None
  */
#if !defined(CORE_CM0PLUS)
#define __HAL_GPIO_EXTI1_CLEAR_FALLING_IT(__EXTI_LINE__) do { \
                                                              EXTI1->FPR1 = (__EXTI_LINE__); \
                                                            } while (0);
#endif /*!defined(CORE_CM0PLUS)*/
#define __HAL_GPIO_EXTI_CLEAR_FALLING_IT(__EXTI_LINE__) do { \
                                                             EXTI2->FPR1 = (__EXTI_LINE__); \
                                                           } while (0);

/**
  * @brief  Generate a Software interrupt on selected EXTI line.
  * @param __EXTI_LINE__ specifies the EXTI line to check.
  *          This parameter can be GPIO_PIN_x where x can be(0..15)
  * @retval None
  */
#if !defined(CORE_CM0PLUS)
#define __HAL_GPIO_EXTI1_GENERATE_SWIT(__EXTI_LINE__)  (EXTI1->SWIER1 |= (__EXTI_LINE__))
#endif /*!defined(CORE_CM0PLUS)*/
#define __HAL_GPIO_EXTI_GENERATE_SWIT(__EXTI_LINE__)  (EXTI2->SWIER1 |= (__EXTI_LINE__))

/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/** @defgroup GPIO_Private_Macros GPIO Private Macros
  * @{
  */
#define IS_GPIO_PIN_ACTION(ACTION)  (((ACTION) == GPIO_PIN_RESET) || ((ACTION) == GPIO_PIN_SET))

#define IS_GPIO_PIN(__PIN__)        ((((uint32_t)(__PIN__) & GPIO_PIN_MASK) != 0x00U) &&\
                                     (((uint32_t)(__PIN__) & ~GPIO_PIN_MASK) == 0x00U))

#define IS_GPIO_COMMON_PIN(__RESETMASK__, __SETMASK__)  \
  (((uint32_t)(__RESETMASK__) & (uint32_t)(__SETMASK__)) == 0x00u)
#if !defined(CORE_CA35)
#define IS_GPIO_EXTI2_EVT(__MODE__) (((__MODE__) == GPIO_EXTI2_EVT_RISING)         ||\
                                     ((__MODE__) == GPIO_EXTI2_EVT_FALLING)        ||\
                                     ((__MODE__) == GPIO_EXTI2_EVT_RISING_FALLING  ))
#else   /* !defined(CORE_CA35) */
#define IS_GPIO_EXTI2_EVT(__MODE__)   (0U)
#endif  /* !defined(CORE_CA35) */

#if defined(CORE_CM33)
#define IS_GPIO_EXTI1_EVT(__MODE__) (((__MODE__) == GPIO_EXTI1_EVT_RISING)         ||\
                                     ((__MODE__) == GPIO_EXTI1_EVT_FALLING)        ||\
                                     ((__MODE__) == GPIO_EXTI1_EVT_RISING_FALLING  ))
#else   /* CORE_CM33 */
#define IS_GPIO_EXTI1_EVT(__MODE__)   (0U)
#endif  /* CORE_CM33 */

#define IS_GPIO_MODE(__MODE__)     ( \
                                     ( \
                                       ((__MODE__) == GPIO_MODE_INPUT)               ||   \
                                       ((__MODE__) == GPIO_MODE_OUTPUT_PP)           ||   \
                                       ((__MODE__) == GPIO_MODE_OUTPUT_OD)           ||   \
                                       ((__MODE__) == GPIO_MODE_AF_PP)               ||   \
                                       ((__MODE__) == GPIO_MODE_AF_OD)               ||   \
                                       ((__MODE__) == GPIO_EXTI1_IT_RISING)          ||   \
                                       ((__MODE__) == GPIO_EXTI1_IT_FALLING)         ||   \
                                       ((__MODE__) == GPIO_EXTI1_IT_RISING_FALLING)  ||   \
                                       ((__MODE__) == GPIO_EXTI2_IT_RISING)          ||   \
                                       ((__MODE__) == GPIO_EXTI2_IT_FALLING)         ||   \
                                       ((__MODE__) == GPIO_EXTI2_IT_RISING_FALLING)  ||   \
                                       ((__MODE__) == GPIO_MODE_ANALOG)                   \
                                     )                                                    \
                                     ||                                                   \
                                     (IS_GPIO_EXTI1_EVT(__MODE__))                        \
                                     ||                                                   \
                                     (IS_GPIO_EXTI2_EVT(__MODE__))                        \
                                   )


#define IS_GPIO_SPEED(__SPEED__)    ( \
                                      ((__SPEED__) == GPIO_SPEED_FREQ_LOW)           ||   \
                                      ((__SPEED__) == GPIO_SPEED_FREQ_MEDIUM)        ||   \
                                      ((__SPEED__) == GPIO_SPEED_FREQ_HIGH)          ||   \
                                      ((__SPEED__) == GPIO_SPEED_FREQ_VERY_HIGH)          \
                                    )

#define IS_GPIO_PULL(__PULL__)      ( \
                                      ((__PULL__) == GPIO_NOPULL)                    ||   \
                                      ((__PULL__) == GPIO_PULLUP)                    ||   \
                                      ((__PULL__) == GPIO_PULLDOWN)                       \
                                    )

#if defined (CORTEX_IN_SECURE_STATE)
#define IS_GPIO_PIN_ATTRIBUTES(__ATTRIBUTES__)  ( \
                                                  (((__ATTRIBUTES__)&GPIO_PIN_ATTR_CID_SHARED_SELECT) ==    \
                                                   GPIO_PIN_ATTR_CID_SHARED_SELECT                  )       \
                                                  ||                                                        \
                                                  (((__ATTRIBUTES__)&GPIO_PIN_ATTR_CID_STATIC_SELECT) ==    \
                                                   GPIO_PIN_ATTR_CID_STATIC_SELECT                  )       \
                                                  ||                                                        \
                                                  (((__ATTRIBUTES__)&GPIO_PIN_CID_DISABLE           ) ==    \
                                                   GPIO_PIN_CID_DISABLE                             )       \
                                                  ||                                                        \
                                                  (((__ATTRIBUTES__)&GPIO_PIN_ATTR_SEC_SELECT       ) ==    \
                                                   GPIO_PIN_ATTR_SEC_SELECT                         )       \
                                                  ||                                                        \
                                                  (((__ATTRIBUTES__)&GPIO_PIN_ATTR_PRIV_SELECT      ) ==    \
                                                   GPIO_PIN_ATTR_PRIV_SELECT                        )       \
                                                )
#else   /* CORTEX_IN_SECURE_STATE */
#define IS_GPIO_PIN_ATTRIBUTES(__ATTRIBUTES__)  ( \
                                                  (((__ATTRIBUTES__)&GPIO_PIN_ATTR_CID_SHARED_SELECT) ==    \
                                                   GPIO_PIN_ATTR_CID_SHARED_SELECT                  )       \
                                                  ||                                                        \
                                                  (((__ATTRIBUTES__)&GPIO_PIN_ATTR_CID_STATIC_SELECT) ==    \
                                                   GPIO_PIN_ATTR_CID_STATIC_SELECT                  )       \
                                                  ||                                                        \
                                                  (((__ATTRIBUTES__)&GPIO_PIN_CID_DISABLE           ) ==    \
                                                   GPIO_PIN_CID_DISABLE                             )       \
                                                  ||                                                        \
                                                  (((__ATTRIBUTES__)&GPIO_PIN_ATTR_PRIV_SELECT      ) ==    \
                                                   GPIO_PIN_ATTR_PRIV_SELECT                        )       \
                                                )
#endif  /* CORTEX_IN_SECURE_STATE */
/**
  * @}
  */

/* Include GPIO HAL Extended module */
#include "stm32mp2xx_hal_gpio_ex.h"

/* Exported functions --------------------------------------------------------*/
/** @defgroup GPIO_Exported_Functions GPIO Exported Functions
  *  @brief    GPIO Exported Functions
  * @{
  */

/** @defgroup GPIO_Exported_Functions_Group1 Initialization/de-initialization functions
  *  @brief    Initialization and Configuration functions
  * @{
  */

/* Initialization and de-initialization functions *****************************/
void              HAL_GPIO_Init(GPIO_TypeDef  *GPIOx, const GPIO_InitTypeDef *GPIO_Init);
void              HAL_GPIO_DeInit(GPIO_TypeDef  *GPIOx, uint32_t GPIO_Pin);

/**
  * @}
  */

/** @defgroup GPIO_Exported_Functions_Group2 IO operation functions
  *  @brief    IO operation functions
  * @{
  */

/* IO operation functions *****************************************************/
GPIO_PinState     HAL_GPIO_ReadPin(const GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void              HAL_GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState);
void              HAL_GPIO_WriteMultipleStatePin(GPIO_TypeDef *GPIOx, uint16_t PinReset, uint16_t PinSet);
void              HAL_GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
HAL_StatusTypeDef HAL_GPIO_LockPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
#if !defined(CORE_CM0PLUS)
EXTI_TypeDef     *getUsedExti(const GPIO_TypeDef  *GPIOx, uint32_t pinPosition);
void              HAL_GPIO_EXTI1_IRQHandler(uint16_t GPIO_Pin);
void              HAL_GPIO_EXTI1_Rising_Callback(uint16_t GPIO_Pin);
void              HAL_GPIO_EXTI1_Falling_Callback(uint16_t GPIO_Pin);
#endif  /* CORE_CM0PLUS */
void              HAL_GPIO_EXTI_IRQHandler(uint16_t GPIO_Pin);
void              HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin);
void              HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin);

/**
  * @}
  */

/** @defgroup GPIO_Exported_Functions_Group3 GPIO Attributes functions
  * @brief    GPIO Attributes functions
  * @{
  */
HAL_StatusTypeDef HAL_GPIO_ConfigPinAttributes(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, uint32_t PinAttributes);
HAL_StatusTypeDef HAL_GPIO_GetConfigPinAttributes(const GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin,
                                                  uint32_t *pPinAttributes);
HAL_StatusTypeDef HAL_GPIO_TakePinSemaphore(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
HAL_StatusTypeDef HAL_GPIO_ReleasePinSemaphore(const GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);


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

#endif /* STM32MP2xx_HAL_GPIO_H */
