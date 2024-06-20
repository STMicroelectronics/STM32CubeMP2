/**
  ******************************************************************************
  * @file    stm32mp257f_disco_usbpd_pwr.h
  * @author  MCD Application Team
  * @brief   Header file for stm32mp257f_disco_usbpd_pwr.c file
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STM32MP257F_DK_USBPD_PWR_H
#define STM32MP257F_DK_USBPD_PWR_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32mp257f_disco_conf.h"
#include "stm32mp257f_disco_errno.h"

#include "stm32mp2xx_hal_adc.h"

#include "stm32mp2xx_ll_bus.h"
#include "stm32mp2xx_ll_gpio.h"
#include "stm32mp2xx_ll_exti.h"

/* Include TCPP0203 component driver */
#include "../Components/tcpp0203/tcpp0203.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32MP257F_DK
  * @{
  */

/** @addtogroup USBPD-TCPP0203
  * @{
  */

/** @addtogroup STM32MP257F_DK_USBPD_PWR
  * @{
  */

/** @defgroup STM32MP257F_DK_USBPD_PWR_Exported_Types Exported Types
  * @{
  */

#if !defined(USBPD_PWR_USER_H_)

/**
  * @brief  Power role
  */
typedef enum
{
  POWER_ROLE_SOURCE = 0,
  POWER_ROLE_SINK,
  POWER_ROLE_DUAL
} USBPD_PWR_PowerRoleTypeDef;

/**
  * @brief  Low Power Mode of TypeC ports
  */
typedef enum
{
  USBPD_PWR_MODE_OFF = 0,
  USBPD_PWR_MODE_HIBERNATE,
  USBPD_PWR_MODE_LOWPOWER,
  USBPD_PWR_MODE_NORMAL
} USBPD_PWR_PowerModeTypeDef;

/**
  * @brief  VBUS connection status
  */
typedef enum
{
  VBUS_CONNECTED = 0,
  VBUS_NOT_CONNECTED
} USBPD_PWR_VBUSConnectionStatusTypeDef;

/**
  * @brief VBUS Detection Callback
  */
typedef void USBPD_PWR_VBUSDetectCallbackFunc(uint32_t PortNum,
                                              USBPD_PWR_VBUSConnectionStatusTypeDef VBUSConnectionStatus);

/**
  * @}
  */

/** @defgroup STM32MP257F_DK_USBPD_PWR_Exported_Constants Exported Constants
  * @{
  */
/**
  * @brief  HW configuration type of TypeC ports
  */
#define USBPD_PWR_HW_CONFIG_TYPE_DEFAULT          (0U)       /*!< Default configuration :
                                                                  Port available natively on board, no protection  */
#define USBPD_PWR_HW_CONFIG_TYPE_TCPP01           (1U)       /*!< TypeC Port protection available
                                                                  thanks to a TCPP01 component                     */
#define USBPD_PWR_HW_CONFIG_TYPE_TCPP02           (2U)       /*!< TypeC Port protection available
                                                                  thanks to a TCPP02 component                     */
#define USBPD_PWR_HW_CONFIG_TYPE_TCPP03           (3U)       /*!< TypeC Port protection available
                                                                  thanks to a TCPP03 component                     */


/**
  * @brief  Number of TypeC ports
  */
#define USBPD_PWR_INSTANCES_NBR           (1U)

/**
  * @brief  Type-C port identifier
  */
#define USBPD_PWR_TYPE_C_PORT_1           (0U)
#define USBPD_PWR_TYPE_C_PORT_2           (1U)
#define USBPD_PWR_TYPE_C_PORT_3           (2U)

/**
  * @brief  CC pin identifier
  */
#define USBPD_PWR_TYPE_C_CC1              (1U)
#define USBPD_PWR_TYPE_C_CC2              (2U)

/**
  * @brief  VBUS disconnection threshold values (in mV)
  */
#define USBPD_PWR_HIGH_VBUS_THRESHOLD     (2800U)
#define USBPD_PWR_LOW_VBUS_THRESHOLD      (750U)
#define USBPD_PWR_VBUS_THRESHOLD_5V       (3900U)
#define USBPD_PWR_VBUS_THRESHOLD_9V       (7000U)
#define USBPD_PWR_VBUS_THRESHOLD_15V      (12500U)
#define USBPD_PWR_VBUS_THRESHOLD_20V      (17000U)
#define USBPD_PWR_VBUS_THRESHOLD_APDO     (2150U)

/**
  * @brief  VBUS discharge parameters
  */
#define USBPD_PWR_DISCHARGE_MARGIN        (500U)
#define USBPD_PWR_DISCHARGE_TIME          (6U)

/**
  * @brief  Standard VBUS voltage levels
  */
#define USBPD_PWR_VBUS_5V                 5000U
#define USBPD_PWR_VBUS_9V                 9000U
#define USBPD_PWR_VBUS_15V                15000U

/**
  * @brief  power timeout
  */
#define USBPD_PWR_TIMEOUT_PDO             250U         /* Timeout for PDO to PDO or PDO to APDO at 250ms */
#define USBPD_PWR_TIMEOUT_APDO            25U          /* Timeout for APDO to APDO at 25ms */

/**
  * @brief  Invalid value set during issue with voltage setting
  */
#define USBPD_PWR_INVALID_VALUE           0xFFFFFFFFU

#endif /* !USBPD_PWR_USER_H_ */

#define USBPD_PWR_VCONN_CC1_EN_GPIO_PIN             GPIO_PIN_5
#define USBPD_PWR_VCONN_CC1_EN_GPIO_PORT            GPIOF
#define USBPD_PWR_VCONN_CC1_EN_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOF_CLK_ENABLE()
#define USBPD_PWR_VCONN_CC1_EN_GPIO_IS_CLK_ENABLE() (READ_BIT(RCC->GPIOFCFGR, RCC_GPIOFCFGR_GPIOxEN) == \
                                                     RCC_GPIOFCFGR_GPIOxEN)
#define USBPD_PWR_VCONN_CC1_EN_GPIO_CLK_DISABLE()   __HAL_RCC_GPIOF_CLK_DISABLE()
#define USBPD_PWR_VCONN_CC1_EN_RIF_RES_TYP_GPIO   RESMGR_RESOURCE_RIF_GPIOF
#define USBPD_PWR_VCONN_CC1_EN_RIF_RES_NUM_GPIO   RESMGR_GPIO_PIN(5)

#define USBPD_PWR_VCONN_CC2_EN_GPIO_PIN             GPIO_PIN_3
#define USBPD_PWR_VCONN_CC2_EN_GPIO_PORT            GPIOC
#define USBPD_PWR_VCONN_CC2_EN_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOC_CLK_ENABLE()
#define USBPD_PWR_VCONN_CC2_EN_GPIO_IS_CLK_ENABLE() (READ_BIT(RCC->GPIOCCFGR, RCC_GPIOCCFGR_GPIOxEN) == \
                                                     RCC_GPIOCCFGR_GPIOxEN)
#define USBPD_PWR_VCONN_CC2_EN_GPIO_CLK_DISABLE()  __HAL_RCC_GPIOC_CLK_DISABLE()
#define USBPD_PWR_VCONN_CC2_EN_RIF_RES_TYP_GPIO    RESMGR_RESOURCE_RIF_GPIOC
#define USBPD_PWR_VCONN_CC2_EN_RIF_RES_NUM_GPIO     RESMGR_GPIO_PIN(3)

/**
  * @brief TCPP0203_FLGn pin
  * To be defined for each Port, protected by a TCPP0203 component
  */
#define TCPP0203_PORT0_FLG_GPIO_CLK_ENABLE()        LL_RCC_GPIOG_EnableClock();
#define TCPP0203_PORT0_FLG_GPIO_CLK_DISABLE()       LL_RCC_GPIOG_DisableClock();
#define TCPP0203_PORT0_FLG_GPIO_IS_CLK_ENABLE()    (READ_BIT(RCC->GPIOGCFGR, RCC_GPIOGCFGR_GPIOxEN) == \
                                                    RCC_GPIOGCFGR_GPIOxEN)
#define TCPP0203_PORT0_FLG_GPIO_PORT                GPIOG
#define TCPP0203_PORT0_FLG_GPIO_PIN                 LL_GPIO_PIN_1
#define TCPP0203_PORT0_FLG_GPIO_MODE                LL_GPIO_MODE_INPUT
#define TCPP0203_PORT0_FLG_GPIO_PUPD                LL_GPIO_PULL_UP
#define TCPP0203_PORT0_FLG_RIF_RES_TYP_GPIO         RESMGR_RESOURCE_RIF_GPIOG
#define TCPP0203_PORT0_FLG_GPIO_RIF_RES_NUM_GPIO    RESMGR_GPIO_PIN(1)
#define TCPP0203_PORT0_FLG_SET_EXTI()               LL_EXTI_SetEXTISource(EXTI2, LL_EXTI_EXTICR_PORTG,\
                                                                          LL_EXTI_EXTI_LINE1);
#define TCPP0203_PORT0_FLG_EXTI_LINE                LL_EXTI2_LINE_1
#define TCPP0203_PORT0_FLG_EXTI_ENABLE()                                  \
  do                                                                      \
  {                                                                       \
    LL_C2_EXTI_DisableEvent_0_31(EXTI2, TCPP0203_PORT0_FLG_EXTI_LINE);    \
    LL_C2_EXTI_EnableIT_0_31(EXTI2, TCPP0203_PORT0_FLG_EXTI_LINE);        \
  } while(0);
#define TCPP0203_PORT0_FLG_EXTI_DISABLE()           LL_C2_EXTI_DisableIT_0_31(EXTI2, TCPP0203_PORT0_FLG_EXTI_LINE);
#define TCPP0203_PORT0_FLG_TRIG_ENABLE()                                  \
  do                                                                      \
  {                                                                       \
    LL_EXTI_DisableRisingTrig_0_31(EXTI2, TCPP0203_PORT0_FLG_EXTI_LINE);  \
    LL_EXTI_EnableFallingTrig_0_31(EXTI2, TCPP0203_PORT0_FLG_EXTI_LINE);  \
  } while(0);
#define TCPP0203_PORT0_FLG_TRIG_DISABLE()           LL_EXTI_DisableFallingTrig_0_31(EXTI2, \
    TCPP0203_PORT0_FLG_EXTI_LINE);
#define TCPP0203_PORT0_FLG_EXTI_IS_ACTIVE_FLAG()    LL_EXTI_IsActiveFallingFlag_0_31(EXTI2, \
    TCPP0203_PORT0_FLG_EXTI_LINE)
#define TCPP0203_PORT0_FLG_EXTI_CLEAR_FLAG()        LL_EXTI_ClearFallingFlag_0_31(EXTI2, TCPP0203_PORT0_FLG_EXTI_LINE);
#define TCPP0203_PORT0_FLG_EXTI_IRQN                EXTI2_1_IRQn
#define TCPP0203_PORT0_FLG_EXTI_IRQHANDLER          EXTI2_1_IRQHandler
#define TCPP0203_PORT0_FLG_IT_PRIORITY              (12U)
#define TCPP0203_PORT0_FLG_GENERATE_IT()            LL_EXTI_GenerateSWI_0_31(EXTI2, TCPP0203_PORT0_FLG_EXTI_LINE);

/* Definition of ADCx instance */
#define TCPP0203_PORT0_ADC_INSTANCE                 ADC3
#define TCPP0203_PORT0_ADC_CLK_ENABLE()            __HAL_RCC_ADC3_CLK_ENABLE()
#define TCPP0203_PORT0_ADC_CLK_DISABLE()           __HAL_RCC_ADC3_CLK_DISABLE()
#define TCPP0203_PORT0_ADC_FORCE_RESET()           __HAL_RCC_ADC3_FORCE_RESET()
#define TCPP0203_PORT0_ADC_RELEASE_RESET()         __HAL_RCC_ADC3_RELEASE_RESET()

/* Definition of ADCx channels */
#define TCPP0203_PORT0_VBUSC_ADC_CHANNEL            ADC_CHANNEL_2          /* PF10 : ADC3_INP2 */

#define TCPP0203_PORT0_VBUSC_GPIO_CLK_ENABLE()      LL_RCC_GPIOF_EnableClock();
#define TCPP0203_PORT0_VBUSC_GPIO_CLK_DISABLE()     LL_RCC_GPIOF_DisableClock();
#define TCPP0203_PORT0_VBUSC_GPIO_IS_CLK_ENABLE()   (READ_BIT(RCC->GPIOFCFGR, RCC_GPIOFCFGR_GPIOxEN) == \
                                                     RCC_GPIOFCFGR_GPIOxEN)
#define TCPP0203_PORT0_VBUSC_GPIO_PORT              GPIOF
#define TCPP0203_PORT0_VBUSC_GPIO_PIN               LL_GPIO_PIN_10
#define TCPP0203_PORT0_VBUSC_GPIO_MODE              LL_GPIO_MODE_ANALOG
#define TCPP0203_PORT0_VBUSCGPIO_RIF_RES_TYP_GPIO   RESMGR_RESOURCE_RIF_GPIOF
#define TCPP0203_PORT0_VBUSC_GPIO_RIF_RES_NUM_GPIO  RESMGR_GPIO_PIN(10)

/**
  * @brief TCPP0203_EN GPIO pin
  * To be defined for each Port, protected by a TCPP0203 component
  */
#if defined(USE_STM32MP257F_DK)
#define TCPP0203_PORT0_EN_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE();
#define TCPP0203_PORT0_EN_GPIO_CLK_DISABLE()     __HAL_RCC_GPIOA_CLK_DISABLE();
#define TCPP0203_PORT0_EN_GPIO_IS_CLK_ENABLE()   (READ_BIT(RCC->GPIOACFGR, RCC_GPIOACFGR_GPIOxEN) == \
                                                  RCC_GPIOACFGR_GPIOxEN)
#define TCPP0203_PORT0_EN_GPIO_PORT              GPIOA
#define TCPP0203_PORT0_EN_GPIO_PIN               GPIO_PIN_3
#define TCPP0203_PORT0_EN_RIF_RES_TYP_GPIO       RESMGR_RESOURCE_RIF_GPIOA
#define TCPP0203_PORT0_EN_GPIO_RIF_RES_NUM_GPIO  RESMGR_GPIO_PIN(3)
#endif /* USE_STM32MP257F_DK */

/**
  * @brief  VBUS sensing parameters
  */
#define VSENSE_ADC_INSTANCE                         TCPP0203_PORT0_ADC_INSTANCE
#define VSENSE_ADC_CLOCK_PRESCALER                  ADC_CLOCK_CK_ICN_LS_MCU_DIV4
#define VSENSE_ADC_RCC_PERIPHCLK                    RCC_PERIPHCLK_ADC3
#define VSENSE_ADC_COMMON                           ADC3_COMMON
#define VSENSE_ADC_RANK                             LL_ADC_REG_RANK_1
#define VSENSE_ADC_CHANNEL                          TCPP0203_PORT0_VBUSC_ADC_CHANNEL

#define STM32MP257F_DK_VSENSE_RA              140U /* VBUS voltage divider RA in kiloohm */
#define STM32MP257F_DK_VSENSE_RB              10U  /* VBUS voltage divider RB in kiloohm */

/**
  * @}
  */

/** @addtogroup STM32MP257F_DK_USBPD_PWR_Exported_Variables
  * @{
  */
/**
  * @}
  */

/** @defgroup STM32MP257F_DK_USBPD_PWR_Exported_Functions Exported Functions
  * @{
  */
/* Common functions */
int32_t BSP_USBPD_PWR_Init(uint32_t PortNum);
int32_t BSP_USBPD_PWR_Deinit(uint32_t PortNum);

int32_t BSP_USBPD_PWR_SetRole(uint32_t PortNum, USBPD_PWR_PowerRoleTypeDef Role);
int32_t BSP_USBPD_PWR_SetPowerMode(uint32_t PortNum, USBPD_PWR_PowerModeTypeDef PwrMode);
int32_t BSP_USBPD_PWR_GetPowerMode(uint32_t PortNum, USBPD_PWR_PowerModeTypeDef *PwrMode);

int32_t BSP_USBPD_PWR_VBUSInit(uint32_t PortNum);
int32_t BSP_USBPD_PWR_VBUSDeInit(uint32_t PortNum);

int32_t BSP_USBPD_PWR_VBUSOn(uint32_t PortNum);
int32_t BSP_USBPD_PWR_VBUSOff(uint32_t PortNum);
int32_t BSP_USBPD_PWR_VBUSIsOn(uint32_t PortNum, uint8_t *pState);

int32_t BSP_USBPD_PWR_VBUSSetVoltage_Fixed(uint32_t PortNum, uint32_t VbusTargetInmv, uint32_t OperatingCurrent,
                                           uint32_t MaxOperatingCurrent);
int32_t BSP_USBPD_PWR_VBUSSetVoltage_Variable(uint32_t PortNum, uint32_t VbusTargetMaxInmv,
                                              uint32_t VbusTargetMinInmv, uint32_t OperatingCurrent,
                                              uint32_t MaxOperatingCurrent);
int32_t BSP_USBPD_PWR_VBUSSetVoltage_Battery(uint32_t PortNum, uint32_t VbusTargetMin, uint32_t VbusTargetMax,
                                             uint32_t OperatingPower, uint32_t MaxOperatingPower);
int32_t BSP_USBPD_PWR_VBUSSetVoltage_APDO(uint32_t PortNum, uint32_t VbusTargetInmv, uint32_t OperatingCurrent,
                                          int32_t Delta);

int32_t BSP_USBPD_PWR_SetVBUSDisconnectionThreshold(uint32_t PortNum, uint32_t VoltageThreshold);

int32_t BSP_USBPD_PWR_RegisterVBUSDetectCallback(uint32_t PortNum,
                                                 USBPD_PWR_VBUSDetectCallbackFunc *pfnVBUSDetectCallback);

int32_t BSP_USBPD_PWR_VBUSGetVoltage(uint32_t PortNum, uint32_t *pVoltage);
int32_t BSP_USBPD_PWR_VBUSGetCurrent(uint32_t PortNum, int32_t *pCurrent);

int32_t BSP_USBPD_PWR_VBUSDischargeOn(uint32_t PortNum);
int32_t BSP_USBPD_PWR_VBUSDischargeOff(uint32_t PortNum);

int32_t BSP_USBPD_PWR_VCONNInit(uint32_t PortNum, uint32_t CCPinId);
int32_t BSP_USBPD_PWR_VCONNDeInit(uint32_t PortNum, uint32_t CCPinId);

int32_t BSP_USBPD_PWR_VCONNOn(uint32_t PortNum, uint32_t CCPinId);
int32_t BSP_USBPD_PWR_VCONNOff(uint32_t PortNum, uint32_t CCPinId);
int32_t BSP_USBPD_PWR_VCONNIsOn(uint32_t PortNum, uint32_t CCPinId, uint8_t *pState);

int32_t BSP_USBPD_PWR_VCONNDischargeOn(uint32_t PortNum);
int32_t BSP_USBPD_PWR_VCONNDischargeOff(uint32_t PortNum);

void    BSP_USBPD_PWR_EventCallback(uint32_t PortNum);

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

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* STM32MP257F_DK_USBPD_PWR_H */

