/**
  ******************************************************************************
  * @file    stm32mp215f_disco_stpmic2l.h
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
#ifndef STM32MP215F_DISCO_STPMIC2L_H
#define STM32MP215F_DISCO_STPMIC2L_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32mp2xx_hal.h"
#include "stpmic2l.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32MP215F_DISCO
  * @{
  */

/** @addtogroup STPMIC2L
  * @{
  */

/** @defgroup STPMIC_Exported_Types Exported Types
  * @{
  */
typedef enum
{
  VDDCORE = 0,
  VDD2_DDR,
  VDDCPU,
  VDDA1V8_AON,
  VDDIO,
  VDD1_DDR,
  VDD3V3_USB,
  VDD_FLASH,
  VDDA_1V8,
  LDO7_FREE,
  GPO1_3V3,
  GPO2_FREE,
  GPO3_LED5,
  GPO4_FREE,
  GPO5_3V3_BKP,
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

/** @defgroup STPMIC_Exported_Constants Exported Constants
  * @{
  */
/* Board Configuration ------------------------------------------------------------*/
/* Definition of PMIC <=> stm32mp2 Signals */
#define PMIC_INTN_PIN               GPIO_PIN_0
#define PMIC_INTN_PORT              GPIOA
#define PMIC_INTN_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define PMIC_INTN_CLK_DISABLE()     __HAL_RCC_GPIOA_CLK_DISABLE()

#ifdef USE_WAKEUP_PIN
#define PMIC_INTN_EXTI_IRQ          CPU1_WAKEUP_PIN_IRQn   /* CPU1_WAKEUP_PIN_IRQn */
#define BSP_PMIC_INTN_IRQHANDLER    CPU1_WAKEUP_PIN_IRQHandler
#else
#define PMIC_INTN_EXTI_IRQ          EXTI2_0_IRQn   /* CPU1_WAKEUP_PIN_IRQn */
#define BSP_PMIC_INTN_IRQHANDLER    EXTI2_0_IRQHandler
#endif /* USE_WAKEUP_PIN */

#define BSP_PMIC_PWRCTRL_PIN_ASSERT()   HAL_GPIO_WritePin(PMIC_PWRCTRL_PORT, \
                                                          PMIC_PWRCTRL_PIN, GPIO_PIN_RESET);
#define BSP_PMIC_PWRCTRL_PIN_PULL()     HAL_GPIO_WritePin(PMIC_PWRCTRL_PORT, \
                                                          PMIC_PWRCTRL_PIN, GPIO_PIN_SET);

/**
  * @brief  STPMIC2L product ID
  */
#define  STPMIC2L_ID           0x30U /* Default PMIC product ID: 0x2X (X depends on PMIC variant) */


/**
  * @brief  STPMIC2L LOW POWER MODES
  */

#define  STPMIC2L_RUN1_STOP1           0x0U /*  */
#define  STPMIC2L_RUN2_STOP2           0x1U /*  */
#define  STPMIC2L_LP_STOP1             0x2U /*  */
#define  STPMIC2L_LP_STOP2             0x3U /*  */
#define  STPMIC2L_LPLV_STOP1           0x4U /*  */
#define  STPMIC2L_LPLV_STOP2           0x5U /*  */
#define  STPMIC2L_STANDBY_DDR_SR       0x6U /*  */
#define  STPMIC2L_STANDBY_DDR_OFF      0x7U /*  */


/**
  * @}
  */

/** @defgroup STPMIC_Private_Defines Private Defines
  * @{
  */
/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

/**
  * @}
  */

/** @defgroup STPMIC_Private_Variables Private Variables
  * @{
  */

/* Exported constants --------------------------------------------------------*/

/**
  * @}
  */

/** @defgroup STPMIC_Exported_Functions Exported Functions
  * @{
  */

/* Exported functions --------------------------------------------------------*/
uint32_t BSP_PMIC_Init(void);
uint32_t BSP_PMIC_DeInit(void);
uint32_t BSP_PMIC_DDR_Power_Init();
#if defined (STPMIC2L_DEBUG)
uint32_t BSP_PMIC_DumpRegs(void);
#endif /* defined (STPMIC2L_DEBUG) */
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

#endif /* STM32MP215F_DISCO_STPMIC2L_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
