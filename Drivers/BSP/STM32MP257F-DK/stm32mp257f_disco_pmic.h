/**
  ******************************************************************************
  * @file    stm32mp257f_disco_stpmic.h
  * @author  MCD Application Team
  * @brief   stpmu driver functions used for ST internal validation
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
#ifndef STM32MP257F_DK_STPMIC_H
#define STM32MP257F_DK_STPMIC_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32mp2xx_hal.h"
#include "stpmic2.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32MP257F_DK
  * @{
  */

/** @addtogroup STM32MP257F_DK_STPMU
  * @{
  */

/** @defgroup STM32MP257F_DK_STPMU_Exported_Types Exported Types
  * @{
  */
typedef enum
{
  VDDCPU = 0,
  VDDCORE,
  VDDGPU,
  VDDIO_PMIC,
  VDDA1V8_AON,
  VDD_EMMC,
  V1V8,
  VDD3V3_USB,
  VDD1_DDR,
  VDD2_DDR,
  V_HDMI,
  VDD_SDCARD,
  VDDIO_SDCARD,
  V3V3,
  V_FREE
} board_regul_t;

typedef struct
{
  char                 boardRegulName[15]; /* board regulator name */
  board_regul_t        boardRegulId;  /* board regulator Id */
  PMIC_RegulId_TypeDef pmicRegul;  /* pmic regulator */
  uint8_t              control_reg1;
  uint8_t              control_reg2;
  uint8_t              alt_control_reg1;
  uint8_t              alt_control_reg2;
  uint8_t              pwr_control_reg;
  uint32_t             regulMin; /* regulator-min-mvolt */
  uint32_t             regulMax; /* regulator-max-mvolt */
} board_regul_struct_t;

/* Exported types ------------------------------------------------------------*/
/**
  * @}
  */

/** @defgroup STM32MP257F_DK_STPMU_Exported_Constants Exported Constants
  * @{
  */
/* Board Configuration ------------------------------------------------------------*/
/* Definition of PMIC <=> stm32mp2 Signals */
#define PMIC_INTN_PIN               GPIO_PIN_0
#define PMIC_INTN_PORT              GPIOA
#define PMIC_INTN_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define PMIC_INTN_CLK_DISABLE()     __HAL_RCC_GPIOA_CLK_DISABLE()
#if defined(USE_WAKEUP_PIN)
#define PMIC_INTN_EXTI_IRQ          CPU1_WAKEUP_PIN_IRQn   /* CPU1_WAKEUP_PIN_IRQn */
#define BSP_PMIC_INTN_IRQHANDLER    CPU1_WAKEUP_PIN_IRQHandler
#else
#define PMIC_INTN_EXTI_IRQ          EXTI2_0_IRQn   /* CPU1_WAKEUP_PIN_IRQn */
#define BSP_PMIC_INTN_IRQHANDLER    EXTI2_0_IRQHandler
#endif /* USE_WAKEUP_PIN */

#define BSP_PMIC_PWRCTRL_PIN_ASSERT()   HAL_GPIO_WritePin(PMIC_PWRCTRL_PORT, PMIC_PWRCTRL_PIN, GPIO_PIN_RESET);
#define BSP_PMIC_PWRCTRL_PIN_PULL()     HAL_GPIO_WritePin(PMIC_PWRCTRL_PORT, PMIC_PWRCTRL_PIN, GPIO_PIN_SET);

/**
  * @brief  STPMIC2 product ID
  */
#define  STPMIC2_ID           0x20U /* Default PMIC product ID: 0x2X (X depends on PMIC variant) */


/**
  * @brief  STPMIC2 LOW POWER MODES
  */
#define  STPMIC2_RUN1_STOP1           0x0U /*  */
#define  STPMIC2_RUN2_STOP2           0x1U /*  */
#define  STPMIC2_LP_STOP1             0x2U /*  */
#define  STPMIC2_LP_STOP2             0x3U /*  */
#define  STPMIC2_LPLV_STOP1           0x4U /*  */
#define  STPMIC2_LPLV_STOP2           0x5U /*  */
#define  STPMIC2_STANDBY_DDR_SR       0x6U /*  */
#define  STPMIC2_STANDBY_DDR_OFF      0x7U /*  */


/**
  * @}
  */

/** @defgroup STM32MP257F_DK_STPMU_Private_Defines Private Defines
  * @{
  */
/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

/**
  * @}
  */

/** @defgroup STM32MP257F_DK_STPMU_Private_Variables Private Variables
  * @{
  */

/* Exported constants --------------------------------------------------------*/

/**
  * @}
  */

/** @defgroup STM32MP257F_DK_STPMU_Exported_Functions Exported Functions
  * @{
  */

/* Exported functions --------------------------------------------------------*/
uint32_t BSP_PMIC_Init(void);
uint32_t BSP_PMIC_DeInit(void);
uint32_t BSP_PMIC_Is_Device_Ready(void);
uint32_t BSP_PMIC_DDR_Power_Init();
uint32_t BSP_PMIC_DumpRegs(void);
uint32_t BSP_PMIC_Power_Mode_Init(void);
uint32_t BSP_PMIC_DDR_Power_Off(void);
uint32_t BSP_PMIC_REGU_Set_Off(board_regul_t regu);
uint32_t BSP_PMIC_REGU_Set_On(board_regul_t regu);
uint32_t BSP_PMIC_Set_Power_Mode(uint32_t mode);
void BSP_PMIC_INTn_Callback(PMIC_IRQn IRQn);
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

#endif /* STM32MP257F_DK_STPMIC_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
