/**
  ******************************************************************************
  * @file    stm32mp257f_eval_stpmic.c
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

/* Includes ----------------------------------------------------------------------*/
#include "stm32mp257f_eval.h"
#include "stm32mp257f_eval_bus.h"
#include "stm32mp257f_eval_pmic.h"
#include <string.h>
#include <stdio.h>

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32MP257F_EV1
  * @{
  */

/** @addtogroup STPMIC
  * @{
  */


/** @defgroup STPMIC_Private_Constants Private Constants
  * @{
  */
/* Driver for PMIC ---------------------------------------------------------------*/

/* Board Configuration ------------------------------------------------------------*/
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

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* I2C handler declaration */
I2C_HandleTypeDef I2cHandle;
extern I2C_HandleTypeDef hI2c7;

static STPMIC2_Drv_t *Stpmic2Drv = NULL;
static void         *CompObj = NULL;


/*
  Table of EVAL board regulators
*/

board_regul_struct_t board_regulators_table[] =
{
  {
    "VDDCPU",      VDDCPU,      STPMIC_BUCK1,  BUCK1_MAIN_CR1,  BUCK1_MAIN_CR2,  \
    BUCK1_ALT_CR1,  BUCK1_ALT_CR2,  BUCK1_PWRCTRL_CR,  800,  910
  },
  {
    "VDDCORE",     VDDCORE,     STPMIC_BUCK2,  BUCK2_MAIN_CR1,  BUCK2_MAIN_CR2,  \
    BUCK2_ALT_CR1,  BUCK2_ALT_CR2,  BUCK2_PWRCTRL_CR,  820,  820
  },
  {
    "VDDGPU",      VDDGPU,      STPMIC_BUCK3,  BUCK3_MAIN_CR1,  BUCK3_MAIN_CR2,  \
    BUCK3_ALT_CR1,  BUCK3_ALT_CR2,  BUCK3_PWRCTRL_CR,  800,  900
  },
  {
    "VDDIO_PMIC",  VDDIO_PMIC,  STPMIC_BUCK4,  BUCK4_MAIN_CR1,  BUCK4_MAIN_CR2,  \
    BUCK4_ALT_CR1,  BUCK4_ALT_CR2,  BUCK4_PWRCTRL_CR,  3300, 3300
  },
  {
    "VDDA1V8_AON", VDDA1V8_AON, STPMIC_LDO1,   LDO1_MAIN_CR,    LDO1_MAIN_CR,    \
    LDO1_ALT_CR,    LDO1_ALT_CR,    LDO1_PWRCTRL_CR,   1800, 1800
  },
  {
    "VDD_EMMC",    VDD_EMMC,    STPMIC_LDO2,   LDO2_MAIN_CR,    LDO2_MAIN_CR,    \
    LDO2_ALT_CR,    LDO2_ALT_CR,    LDO2_PWRCTRL_CR,   1800, 1800
  },
  {
    "V1V8",        V1V8,        STPMIC_BUCK5,  BUCK5_MAIN_CR1,  BUCK5_MAIN_CR2,  \
    BUCK5_ALT_CR1,  BUCK5_ALT_CR2,  BUCK5_PWRCTRL_CR,  1800, 1800
  },
  {
    "VDD3V3_USB",  VDD3V3_USB,  STPMIC_LDO4,   LDO4_MAIN_CR,    LDO4_MAIN_CR,    \
    LDO4_ALT_CR,    LDO4_ALT_CR,    LDO4_PWRCTRL_CR,   3300, 3300
  },
  {
    "VDD_DDR",     VDD_DDR,     STPMIC_BUCK6,  BUCK6_MAIN_CR1,  BUCK6_MAIN_CR2,  \
    BUCK6_ALT_CR1,  BUCK6_ALT_CR2,  BUCK6_PWRCTRL_CR,  1200, 1200
  },
  {
    "VREF_DDR",    VREF_DDR,    STPMIC_REFDDR, REFDDR_MAIN_CR,  REFDDR_MAIN_CR,  \
    REFDDR_ALT_CR,  REFDDR_ALT_CR,  REFDDR_PWRCTRL_CR, 0000, 0000
  },
  {
    "VTT_DDR",     VTT_DDR,     STPMIC_LDO3,   LDO3_MAIN_CR,    LDO3_MAIN_CR,    \
    LDO3_ALT_CR,    LDO3_ALT_CR,    LDO3_PWRCTRL_CR,   0000, 0000
  },
  {
    "VPP_DDR",     VPP_DDR,     STPMIC_LDO5,   LDO5_MAIN_CR,    LDO5_MAIN_CR,    \
    LDO5_ALT_CR,    LDO5_ALT_CR,    LDO5_PWRCTRL_CR,   2500, 2500
  },
  {
    "VDD_SDCARD",  VDD_SDCARD,  STPMIC_LDO7,   LDO7_MAIN_CR,    LDO7_MAIN_CR,    \
    LDO7_ALT_CR,    LDO7_ALT_CR,    LDO7_PWRCTRL_CR,   1800, 3300
  },
  {
    "VDDIO_SDCARD", VDDIO_SDCARD, STPMIC_LDO8,   LDO8_MAIN_CR,    LDO8_MAIN_CR,  \
    LDO8_ALT_CR,    LDO8_ALT_CR,    LDO8_PWRCTRL_CR,   3300, 3300
  },
  {
    "V3V3",        V3V3,        STPMIC_BUCK7,  BUCK7_MAIN_CR1,  BUCK7_MAIN_CR2,  \
    BUCK7_ALT_CR1,  BUCK7_ALT_CR2,  BUCK7_PWRCTRL_CR,  3300, 3300
  },
};


/**
  * @}
 */

/** @defgroup STPMIC_Private_Functions Private Functionss
  * @{
  */

/* Private function prototypes -----------------------------------------------*/

STPMIC2_Object_t   STPMIC2Obj = { 0 };
static int32_t STPMIC_Probe();
void STPMIC_Enable_Interrupt(PMIC_IRQn IRQn);
void STPMIC_Disable_Interrupt(PMIC_IRQn IRQn);
void STPMIC_INTn_Callback(PMIC_IRQn IRQn);

/* Private functions ---------------------------------------------------------*/


static int32_t STPMIC_Probe()
{
  int32_t ret = STPMIC2_OK;
  STPMIC2_IO_t              IOCtx;
  uint8_t                  id;

  /* Only perform the init if the object already exist */
  if (!CompObj)
  {
    /* Configure the I2C driver */
    IOCtx.Address     = STPMIC_I2C_ADDRESS;
    IOCtx.Init        = BSP_I2C7_Init;
    IOCtx.DeInit      = BSP_I2C7_DeInit;
    IOCtx.ReadReg     = BSP_I2C7_ReadReg;
    IOCtx.WriteReg    = BSP_I2C7_WriteReg;
    IOCtx.GetTick     = BSP_GetTick;

    ret = STPMIC2_RegisterBusIO(&STPMIC2Obj, &IOCtx);
    if (ret != STPMIC2_OK)
    {
      return BSP_ERROR_COMPONENT_FAILURE;
    }

    ret = STPMIC2_ReadID(&STPMIC2Obj, &id);
    if (ret != STPMIC2_OK)
    {
      return BSP_ERROR_COMPONENT_FAILURE;
    }

    if ((id & 0xF0) != STPMIC2_ID)
    {
      return BSP_ERROR_UNKNOWN_COMPONENT;
    }

    Stpmic2Drv = (STPMIC2_Drv_t *) &STPMIC2_Driver;
    CompObj = &STPMIC2Obj;
  }

  ret = Stpmic2Drv->Init(CompObj);
  if (ret != STPMIC2_OK)
  {
    return BSP_ERROR_COMPONENT_FAILURE;
  }

  return BSP_ERROR_NONE;
}

uint32_t BSP_PMIC_Init(void)
{
  int32_t status = BSP_ERROR_NONE;

  status = STPMIC_Probe();
  if (status != BSP_ERROR_NONE)
  {
    return status;
  }

#ifdef USE_WAKEUP_PIN

#else
  GPIO_InitTypeDef  GPIO_InitStruct;

  if (ResMgr_Request(RESMGR_RESOURCE_RIF_RCC, RESMGR_RCC_RESOURCE(90)) == RESMGR_STATUS_ACCESS_OK)
  {
    /* INTn - Interrupt Line - Active Low (Falling Edge) */
    PMIC_INTN_CLK_ENABLE();
  }
  /* Acquire GPIOA0 using Resource manager */
  if (RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIF_GPIOA, RESMGR_GPIO_PIN(0)))
  {
    return BSP_ERROR_PERIPH_FAILURE;
  }

  GPIO_InitStruct.Pin       = PMIC_INTN_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull      = GPIO_PULLUP; /* GPIO_NOPULL */
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = 0 ;
  HAL_GPIO_Init(PMIC_INTN_PORT, &GPIO_InitStruct);

  /* Enable and set INTn EXTI Interrupt  */
#if defined (CORE_CM33) || defined (CORE_CM0PLUS)
  HAL_NVIC_SetPriority(PMIC_INTN_EXTI_IRQ, 0, 0);
  HAL_NVIC_EnableIRQ(PMIC_INTN_EXTI_IRQ);
#endif /* defined (CORE_CM33) || defined (CORE_CM0PLUS) */
#endif /* USE_WAKEUP_PIN */

  STPMIC_Enable_Interrupt(STPMIC2_IT_PONKEY_F);
  STPMIC_Enable_Interrupt(STPMIC2_IT_PONKEY_R);

  return BSP_ERROR_NONE;
}

uint32_t BSP_PMIC_DeInit(void)
{
  uint32_t  status = BSP_ERROR_NONE;

  status = Stpmic2Drv->DeInit(CompObj);
  if (status != STPMIC2_OK)
  {
    return BSP_ERROR_COMPONENT_FAILURE;
  }
  if (RESMGR_STATUS_ACCESS_OK != ResMgr_Release(RESMGR_RESOURCE_RIF_GPIOA, RESMGR_GPIO_PIN(0)))
  {
    return BSP_ERROR_PERIPH_FAILURE;
  }
  return status;
}

uint32_t BSP_PMIC_ReadReg(uint8_t reg, uint8_t *pdata)
{
  int32_t  status = BSP_ERROR_NONE;

  status = Stpmic2Drv->ReadReg(CompObj, reg, pdata);
  if (status != STPMIC2_OK)
  {
    return BSP_ERROR_COMPONENT_FAILURE;
  }
  return status;
}

uint32_t BSP_PMIC_WriteReg(uint8_t reg, uint8_t data)
{
  int32_t  status = BSP_ERROR_NONE;

  status = Stpmic2Drv->WriteReg(CompObj, reg, data);
  if (status != STPMIC2_OK)
  {
    return BSP_ERROR_COMPONENT_FAILURE;
  }
  return status;
}

uint32_t BSP_PMIC_UpdateReg(uint8_t reg, uint8_t mask)
{
  int32_t  status = BSP_ERROR_NONE;

  status = Stpmic2Drv->UpdateReg(CompObj, reg, mask);
  if (status != STPMIC2_OK)
  {
    return BSP_ERROR_COMPONENT_FAILURE;
  }
  return status;
}

/*
 *
 * PMIC registers dump
 *
 */
/* following are configurations */
uint32_t BSP_PMIC_DumpRegs(void)
{
  uint32_t  status = BSP_ERROR_NONE;

  status = Stpmic2Drv->DumpRegs(CompObj);
  if (status != STPMIC2_OK)
  {
    return BSP_ERROR_COMPONENT_FAILURE;
  }

  return status;
}

/*
 *
 * @brief BSP_PMIC_DDR_Power_Init initialize DDR power
 *
 * DDR power on sequence is:
 * enable VPP_DDR, VREF_DDR, VTT_DDR, VDD_DDR
 *
 * @param  None
 * @retval status
 *
 */
/* following are configurations */
uint32_t BSP_PMIC_DDR_Power_Init()
{
  uint32_t  status = BSP_ERROR_NONE;

  /* vpp_ddr ==> LDO5 ==> 2500mV */
  if (BSP_PMIC_WriteReg(board_regulators_table[VPP_DDR].control_reg1, 0x1E) != BSP_ERROR_NONE)
  {
    return BSP_ERROR_PMIC;
  }

  /* vdd_ddr ==> BUCK6 ==> 1200mV */
  if (BSP_PMIC_WriteReg(board_regulators_table[VDD_DDR].control_reg1, 0x46) != BSP_ERROR_NONE)
  {
    return BSP_ERROR_PMIC;
  }

  /* vtt_ddr ==> enable flag SINK_SOURCE ==> LDO3 ==> BUCK6/2 ==> 600mV */
  if (BSP_PMIC_UpdateReg(board_regulators_table[VTT_DDR].control_reg1, 0x80) != BSP_ERROR_NONE)
  {
    return BSP_ERROR_PMIC;
  }

  /* enable vpp_ddr */
  status = BSP_PMIC_REGU_Set_On(VPP_DDR);
  if (status != BSP_ERROR_NONE)
  {
    return status;
  }

  /* enable vref_ddr */
   status = BSP_PMIC_REGU_Set_On(VREF_DDR);
  if (status != BSP_ERROR_NONE)
  {
    return status;
  }

  /* enable vtt_ddr */
  status = BSP_PMIC_REGU_Set_On(VTT_DDR);
  if (status != BSP_ERROR_NONE)
  {
    return status;
  }

  /* enable vdd_ddr */
  return BSP_PMIC_REGU_Set_On(VDD_DDR);
}

uint32_t BSP_PMIC_REGU_Set_On(board_regul_t regu)
{
  uint8_t data;

  switch (regu)
  {
    case VDDA1V8_AON:
    case VDD_EMMC:
    case VTT_DDR:
    case VDD3V3_USB:
    case VPP_DDR:
    case VDD_SDCARD:
    case VDDIO_SDCARD:
    case VREF_DDR:
      /* set enable bit */
      if (BSP_PMIC_UpdateReg(board_regulators_table[regu].control_reg1, MAIN_CR_EN) != BSP_ERROR_NONE)
      {
        return BSP_ERROR_PMIC;
      }
      break;
    case VDDCPU:
    case VDDCORE:
    case VDDGPU:
    case VDDIO_PMIC:
    case V1V8:
    case VDD_DDR:
    case V3V3:
      /* Forbid VDDGPU to be set on if VDDCORE is not enabled */
      if (regu == VDDGPU)
      {
        if (BSP_PMIC_ReadReg(board_regulators_table[VDDCORE].control_reg2, &data) != \
            BSP_ERROR_NONE) /* read control reg */
        {
          return BSP_ERROR_PMIC;
        }

        if ((data & MAIN_CR_EN) != MAIN_CR_EN)
        {
          return BSP_ERROR_PMIC;
        }
      }
      /* set enable bit */
      if (BSP_PMIC_UpdateReg(board_regulators_table[regu].control_reg2, MAIN_CR_EN) != BSP_ERROR_NONE)
      {
        return BSP_ERROR_PMIC;
      }
      break;
    default:
      return BSP_ERROR_WRONG_PARAM ;
      break;
  }

  /* Default ramp delay of 1ms */
  HAL_Delay(1);

  return BSP_ERROR_NONE;
}

uint32_t BSP_PMIC_REGU_Set_Off(board_regul_t regu)
{
  uint8_t data;

  switch (regu)
  {
    case VDDA1V8_AON:
    case VDD_EMMC:
    case VTT_DDR:
    case VDD3V3_USB:
    case VPP_DDR:
    case VDD_SDCARD:
    case VDDIO_SDCARD:
    case VREF_DDR:
      /* disable ==> clear enable bit */
      if (BSP_PMIC_ReadReg(board_regulators_table[regu].control_reg1,
                           &data) != BSP_ERROR_NONE) /* read control reg to save data */
      {
        return BSP_ERROR_PMIC;
      }

      data &= ~MAIN_CR_EN; /* clear enable bit */
      if (BSP_PMIC_WriteReg(board_regulators_table[regu].control_reg1,
                            data) != BSP_ERROR_NONE) /* write control reg to clear enable bit */
      {
        return BSP_ERROR_PMIC;
      }
      break;
    case VDDCPU:
    case VDDCORE:
    case VDDGPU:
    case VDDIO_PMIC:
    case V1V8:
    case VDD_DDR:
    case V3V3:
      /* disable ==> clear enable bit */
      if (BSP_PMIC_ReadReg(board_regulators_table[regu].control_reg2,
                           &data) != BSP_ERROR_NONE) /* read control reg to save data */
      {
        return BSP_ERROR_PMIC;
      }

      data &= ~MAIN_CR_EN; /* clear enable bit */
      if (BSP_PMIC_WriteReg(board_regulators_table[regu].control_reg2,
                            data) != BSP_ERROR_NONE) /* write control reg to clear enable bit */
      {
        return BSP_ERROR_PMIC;
      }
      break;
    default:
      return BSP_ERROR_WRONG_PARAM ;
      break;
  }

  /* Default ramp delay of 1ms */
  HAL_Delay(1);

  return BSP_ERROR_NONE;
}

uint32_t BSP_PMIC_DDR_Power_Off()
{
  uint32_t  status = BSP_ERROR_NONE;

  /* disable vpp_ddr */
  status = BSP_PMIC_REGU_Set_Off(VPP_DDR);
  if (status != BSP_ERROR_NONE)
  {
    return status;
  }

  /* disable vref_ddr */
   status = BSP_PMIC_REGU_Set_Off(VREF_DDR);
  if (status != BSP_ERROR_NONE)
  {
    return status;
  }

  /* disable vtt_ddr */
  status = BSP_PMIC_REGU_Set_Off(VTT_DDR);
  if (status != BSP_ERROR_NONE)
  {
    return status;
  }

  /* disable vdd_ddr */
  return BSP_PMIC_REGU_Set_Off(VDD_DDR);
}

/**
  * @brief  BSP_PMIC_Set_Power_Mode Set PMIC run/low power mode
  * @param  mode  mode to set
  * @retval status
  */
/**
  * MPU control signals PWRCTRL-connexion   RUN1 / STOP1  RUN2 / STOP2  LPLV-STOP1  LPLV-STOP2  STANDBY (DDR SR)  STANDBY (DDR OFF)
  * NRST                    1       1       1     1     1         1
  * PWR_ONLP      PWRCTRL1        1       1       0     0     0         0
  * PWR_CPU_ON      PWRCTRL2        1       0       1     0     0         0
  * NRSTC1MS      PWRCTRL3        1       0       1     0     0         0
  *
  *                           PWRCTRL                                                       RUN1 / STOP1      RUN2 /STOP2       LPLV-STOP1      LPLV-STOP2      STANDBY (DDR SR)  STANDBY (DDR OFF)
  * Name on schematic device tree   STPMIC25 name PWRCTRLx_SEL  PWRCTRLx_DLY_H  PWRCTRLx_DLY_L  PWRCTRLx_EN PWRCTRLx_RST  Pull Down Mask_Reset  FS_OCP  Source  Vrun1 / stop1 State Vrun2 / stop2 State Vlplv-stop1 State Vlplv-stop2 State Vstanby State   Vstanby State
  * VDDA1V8_AON     Vdda1V8_aon   ldo1      0       0       0       0     0             1     1   VOUT4 NA        ON    NA        ON    NA      ON    NA      ON    NA    ON      NA    ON
  * VDD_EMMC      Vdd_emmc    ldo2      3       0       0       0     1                       NA    3,3       ON    NA        OFF   3,3     ON    NA      OFF   NA    OFF     NA    OFF
  * VTT_DDR       vtt_ddr     ldo3      1       0       0       1     0                   1   NA    SINK-SOURCE   ON    SINK-SOURCE   ON    NA      OFF   NA      OFF   NA    OFF     NA    OFF
  * VDD3V3_USB      Vdd3v3_usb    ldo4      0       0       0       0     0                       NA    NA        ON    NA        ON    NA      ON    NA      OFF ? NA    OFF     NA    OFF
  * VPP_DDR       vpp_ddr     ldo5      1       0       0       1     0                   1   NA    2,5       ON    2,5       ON    2,5     ON    2,5     ON    2,5   ON      NA    OFF
  * VDD_SDCARD      Vdd_sdcard    ldo7      3       0       0       0     1                       NA    BYPASS      ON    NA        OFF   BYPASS    ON    NA      OFF   NA    OFF     NA    OFF
  * VDDIO_SDCARD    vddio_sdcard  ldo8      3       0       0       0     1                       NA    BYPASS/1,8V   ON    NA        OFF   BYPASS/1,8V ON    NA      OFF   NA    OFF     NA    OFF
  * VREFDDR       vref_ddr    REFDDR      1       0       0       1     0                   1   NA    NA        ON    NA        ON    NA      ON    NA      ON    NA    ON      NA    OFF
  * VDDCPU        Vddcpu      buck1     2       0       0       1     0                   1   NA    0,8 / 0,91    ON_HP NA        OFF   0,8 / 0,91  ON_HP NA      OFF   NA    OFF     NA    OFF
  * VDDCORE       Vddcore     buck2     1       0       0       1     0                   1   NA    0,82      ON_HP 0,82      ON_HP 0,67    ON_HP 0,67    ON_HP NA    OFF     NA    OFF
  * VDDGPU        Vddgpu      buck3     0       0       0       0     0                       NA    0,8 / 0,9   ON_HP NA        OFF   NA      OFF   NA      OFF   NA    OFF     NA    OFF
  * VDDIO       Vdd       buck4     0       0       0       0     0             1     1   NA    3,3       ON_HP 3,3       ON_HP 3,3     ON_HP 3,3     ON_HP 3,3   ON_LP   3,3   ON_LP
  * 1V8         Vdda1V8     buck5     1       0       0       1     0                   1   NA    1,8       ON_HP 1,8       ON_HP 1,8     ON_HP 1,8     ON_HP NA    OFF     NA    OFF
  * VDD_DDR       Vdd_ddr     buck6     1       0       0       1     0       fast          1   NA    1,2       ON_HP 1,2       ON_HP 1,2     ON_HP 1,2     ON_HP 1,2   ON_LP   NA    OFF
  * 3V3         3v3       buck7     1       0       0       1     0                       NA    3,3       ON_HP 3,3       ON_HP 3,3     ON_HP 3,3     ON_HP NA    OFF     NA    OFF
  *
  * PMIC state machine state:     RUN   RUN   RUN   RUN   RUN   STANDBY
  * STANDBY_PWRCTRL_SEL[1:0]      0   0   0   0   0   1
  * PMIC static configuration
  *
  * PWRCTRL1_POL  0
  * PWRCTRL2_POL  0
  * PWRCTRL3_POL  0
  * WDG_PWRCTRL_SEL[1:0]  1
  *
  */
uint32_t BSP_PMIC_Power_Mode_Init()
{
  uint32_t  status = BSP_ERROR_NONE;

  /* Fail-safe overcurrent protection */
  status = BSP_PMIC_WriteReg(FS_OCP_CR1, FS_OCP_REFDDR | FS_OCP_BUCK6 | FS_OCP_BUCK5 \
                             | FS_OCP_BUCK4 | FS_OCP_BUCK2 | FS_OCP_BUCK1);
  status = BSP_PMIC_WriteReg(FS_OCP_CR2, FS_OCP_LDO5 | FS_OCP_LDO3 | FS_OCP_LDO1);

  /* Mask reset LDO control register (LDOS_MRST_CR) for VDDA1V8_AON (LDO1) */
  status = BSP_PMIC_WriteReg(LDOS_MRST_CR, LDO1_MRST);

  /* Mask reset BUCK control register (BUCKS_MRST_CR) for VDDIO (BUCK4) */
  status = BSP_PMIC_WriteReg(BUCKS_MRST_CR, BUCK4_MRST);

  /* for VBAT issue */
  /* Mask reset BUCK control register (BUCKS_MRST_CR) for VDDCPU (BUCK1), VDDCORE (BUCK2), VDDIO (BUCK4) */
  status = BSP_PMIC_WriteReg(BUCKS_MRST_CR, BUCK1_MRST | BUCK2_MRST | BUCK4_MRST);

  /* VDDA1V8_AON: LDO1, PWRCTRL_SEL=0, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
  PWRCTRL_RST=0, Msk_Rst=1, Source=VOUT4, V=N/A, State=ON */
  status = BSP_PMIC_WriteReg(board_regulators_table[VDDA1V8_AON].control_reg1, \
                             MAIN_CR_INPUT_SRC_VOUT4 | MAIN_CR_EN);
  status = BSP_PMIC_WriteReg(board_regulators_table[VDDA1V8_AON].pwr_control_reg, \
                             PWRCTRL_CR_SEL0 | PWRCTRL_CR_DLY_H0 | PWRCTRL_CR_DLY_L0);
  /* VDD_EMMC: LDO2, PWRCTRL_SEL=3, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
  PWRCTRL_RST=1, Msk_Rst=0, Source=NA, V=3.3, State=ON */
  status = BSP_PMIC_WriteReg(board_regulators_table[VDD_EMMC].control_reg1, \
                             (0x18 << 1) | MAIN_CR_EN);
  status = BSP_PMIC_WriteReg(board_regulators_table[VDD_EMMC].pwr_control_reg, \
                             PWRCTRL_CR_SEL3 | PWRCTRL_CR_DLY_H0 | PWRCTRL_CR_DLY_L0 | PWRCTRL_CR_RST);
  /* VTT_DDR: LDO3, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=SINK_SOURCE, State=ON */
  status = BSP_PMIC_WriteReg(board_regulators_table[VTT_DDR].control_reg1, \
                             MAIN_CR_SNK_SRC | MAIN_CR_EN);
  status = BSP_PMIC_WriteReg(board_regulators_table[VTT_DDR].pwr_control_reg, \
                             PWRCTRL_CR_SEL1 | PWRCTRL_CR_DLY_H0 | PWRCTRL_CR_DLY_L0 | PWRCTRL_CR_EN);
  /* VDD3V3_USB: LDO4, PWRCTRL_SEL=0, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=ON */
  status = BSP_PMIC_WriteReg(board_regulators_table[VDD3V3_USB].control_reg1, \
                             MAIN_CR_EN);
  status = BSP_PMIC_WriteReg(board_regulators_table[VDD3V3_USB].pwr_control_reg, 0x0);
  /* VPP_DDR: LDO5, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=2.5, State=ON */
  status = BSP_PMIC_WriteReg(board_regulators_table[VPP_DDR].control_reg1, \
                             (0x10 << 1) | MAIN_CR_EN);
  status = BSP_PMIC_WriteReg(board_regulators_table[VPP_DDR].pwr_control_reg, \
                             PWRCTRL_CR_SEL1 | PWRCTRL_CR_DLY_H0 | PWRCTRL_CR_DLY_L0 | PWRCTRL_CR_EN);
  /* VDD_SDCARD: LDO7, PWRCTRL_SEL=3, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
  PWRCTRL_RST=1, Msk_Rst=0, Source=NA, V=BYPASS, State=ON */
  status = BSP_PMIC_WriteReg(board_regulators_table[VDD_SDCARD].control_reg1, \
                             MAIN_CR_BYPASS | MAIN_CR_EN);
  status = BSP_PMIC_WriteReg(board_regulators_table[VDD_SDCARD].pwr_control_reg, \
                             PWRCTRL_CR_SEL3 | PWRCTRL_CR_DLY_H0 | PWRCTRL_CR_DLY_L0 | PWRCTRL_CR_RST);
  /* VDDIO_SDCARD: LDO8, PWRCTRL_SEL=3, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
  PWRCTRL_RST=1, Msk_Rst=0, Source=NA, V=BYPASS/1.8, State=ON */
  status = BSP_PMIC_WriteReg(board_regulators_table[VDDIO_SDCARD].control_reg1, \
                             (0x9 << 1) | MAIN_CR_BYPASS | MAIN_CR_EN); 
  status = BSP_PMIC_WriteReg(board_regulators_table[VDDIO_SDCARD].pwr_control_reg, \
                             PWRCTRL_CR_SEL3 | PWRCTRL_CR_DLY_H0 | PWRCTRL_CR_DLY_L0 | PWRCTRL_CR_RST);
  /* VREF_DDR: REFDDR, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=ON */
  status = BSP_PMIC_WriteReg(board_regulators_table[VREF_DDR].control_reg1, \
                             MAIN_CR_EN);
  status = BSP_PMIC_WriteReg(board_regulators_table[VREF_DDR].pwr_control_reg, \
                             PWRCTRL_CR_SEL1 | PWRCTRL_CR_DLY_H0 | PWRCTRL_CR_DLY_L0 | PWRCTRL_CR_EN);
  /* VDDCPU: BUCK1, PWRCTRL_SEL=2, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=0.8/0.91, State=ON_HP */
  status = BSP_PMIC_WriteReg(board_regulators_table[VDDCPU].control_reg1, 0x1E); /* 0x1E ==> V=0.8V, 0x29 ==> V=0.91 */
  status = BSP_PMIC_WriteReg(board_regulators_table[VDDCPU].control_reg2, \
                             MAIN_CR_PREG_MODE0 | MAIN_CR_EN);
  /* Add for multiple standby issue */
  status = BSP_PMIC_WriteReg(board_regulators_table[VDDCPU].pwr_control_reg, \
                             PWRCTRL_CR_SEL2 | PWRCTRL_CR_DLY_H0 | PWRCTRL_CR_DLY_L0 | PWRCTRL_CR_EN);
  /* VDDCORE: BUCK2, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=0.82, State=ON_HP */
  status = BSP_PMIC_WriteReg(board_regulators_table[VDDCORE].control_reg1, 0x20);
  status = BSP_PMIC_WriteReg(board_regulators_table[VDDCORE].control_reg2, \
                             MAIN_CR_PREG_MODE0 | MAIN_CR_EN);
  status = BSP_PMIC_WriteReg(board_regulators_table[VDDCORE].pwr_control_reg, \
                             PWRCTRL_CR_SEL1 | PWRCTRL_CR_DLY_H0 | PWRCTRL_CR_DLY_L0 | PWRCTRL_CR_EN);
  /* VDDGPU: BUCK3, PWRCTRL_SEL=0, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=0.8/0.9, State=ON_HP */
  status = BSP_PMIC_WriteReg(board_regulators_table[VDDGPU].control_reg1, 0x1E); /* 0x1E ==> V=0.8V, 0x28 ==> V=0.9 */
  status = BSP_PMIC_WriteReg(board_regulators_table[VDDGPU].control_reg2, \
                             MAIN_CR_PREG_MODE0 | MAIN_CR_EN);
  status = BSP_PMIC_WriteReg(board_regulators_table[VDDGPU].pwr_control_reg, \
                             PWRCTRL_CR_SEL0 | PWRCTRL_CR_DLY_H0 | PWRCTRL_CR_DLY_L0);
  /* VDDIO: BUCK4, PWRCTRL_SEL=0, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
  PWRCTRL_RST=0, Msk_Rst=1, Source=NA, V=3.3, State=ON_HP */
  status = BSP_PMIC_WriteReg(board_regulators_table[VDDIO_PMIC].control_reg1, 0x76);
  status = BSP_PMIC_WriteReg(board_regulators_table[VDDIO_PMIC].control_reg2, \
                             MAIN_CR_PREG_MODE0 | MAIN_CR_EN);
  status = BSP_PMIC_WriteReg(board_regulators_table[VDDIO_PMIC].pwr_control_reg, \
                             PWRCTRL_CR_SEL0 | PWRCTRL_CR_DLY_H0 | PWRCTRL_CR_DLY_L0);
  /* V1V8: BUCK5, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=1.8, State=ON_HP */
  status = BSP_PMIC_WriteReg(board_regulators_table[V1V8].control_reg1, 0x67);
  status = BSP_PMIC_WriteReg(board_regulators_table[V1V8].control_reg2, \
                             MAIN_CR_PREG_MODE0 | MAIN_CR_EN);
  status = BSP_PMIC_WriteReg(board_regulators_table[V1V8].pwr_control_reg, \
                             PWRCTRL_CR_SEL1 | PWRCTRL_CR_DLY_H0 | PWRCTRL_CR_DLY_L0 | PWRCTRL_CR_EN);
  /* VDD_DDR: BUCK6, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=1.2, State=ON_HP */
  status = BSP_PMIC_WriteReg(board_regulators_table[VDD_DDR].control_reg1, 0x46);
  status = BSP_PMIC_WriteReg(board_regulators_table[VDD_DDR].control_reg2, \
                             MAIN_CR_PREG_MODE0 | MAIN_CR_EN);
  status = BSP_PMIC_WriteReg(board_regulators_table[VDD_DDR].pwr_control_reg, \
                             PWRCTRL_CR_SEL1 | PWRCTRL_CR_DLY_H0 | PWRCTRL_CR_DLY_L0 | PWRCTRL_CR_EN);
  /* V3V3: BUCK7, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=3.3, State=ON_HP */
  status = BSP_PMIC_WriteReg(board_regulators_table[V3V3].control_reg1, 0x76);
  status = BSP_PMIC_WriteReg(board_regulators_table[V3V3].control_reg2, \
                             MAIN_CR_PREG_MODE0 | MAIN_CR_EN);
  status = BSP_PMIC_WriteReg(board_regulators_table[V3V3].pwr_control_reg, \
                             PWRCTRL_CR_SEL1 | PWRCTRL_CR_DLY_H0 | PWRCTRL_CR_DLY_L0 | PWRCTRL_CR_EN);

  return status;
}

uint32_t BSP_PMIC_Set_Power_Mode(uint32_t mode)
{
  uint32_t  status = BSP_ERROR_NONE;
  uint8_t data;

  switch (mode)
  {
    case STPMIC2_RUN1_STOP1:
      /* VDD_EMMC: LDO2, PWRCTRL_SEL=3, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
      PWRCTRL_RST=1, Msk_Rst=0, Source=NA, V=3.3, State=ON */
      status = BSP_PMIC_WriteReg(board_regulators_table[VDD_EMMC].control_reg1, \
                                 (0x18 << 1) | MAIN_CR_EN);
      /* VTT_DDR: LDO3, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
      PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=SINK_SOURCE, State=ON */
      status = BSP_PMIC_WriteReg(board_regulators_table[VTT_DDR].control_reg1, \
                                 MAIN_CR_SNK_SRC | MAIN_CR_EN);
      /* VDD3V3_USB: LDO4, PWRCTRL_SEL=0, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
      PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=ON */
      status = BSP_PMIC_WriteReg(board_regulators_table[VDD3V3_USB].control_reg1, \
                                 MAIN_CR_EN);
      /* VPP_DDR: LDO5, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
      PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=2.5, State=ON */
      status = BSP_PMIC_WriteReg(board_regulators_table[VPP_DDR].control_reg1, \
                                 (0x10 << 1) | MAIN_CR_EN);
      /* VDD_SDCARD: LDO7, PWRCTRL_SEL=3, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
      PWRCTRL_RST=1, Msk_Rst=0, Source=NA, V=BYPASS, State=ON */
      status = BSP_PMIC_WriteReg(board_regulators_table[VDD_SDCARD].control_reg1, \
                                 MAIN_CR_BYPASS | MAIN_CR_EN);
      /* VDDIO_SDCARD: LDO8, PWRCTRL_SEL=3, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
      PWRCTRL_RST=1, Msk_Rst=0, Source=NA, V=BYPASS/1.8, State=ON */
      status = BSP_PMIC_WriteReg(board_regulators_table[VDD_SDCARD].control_reg1, \
                                 (0x9 << 1) | MAIN_CR_BYPASS | MAIN_CR_EN); 
      /* VREF_DDR: REFDDR, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
      PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=ON */
      status = BSP_PMIC_WriteReg(board_regulators_table[VREF_DDR].control_reg1, \
                                 MAIN_CR_EN);
      /* VDDCPU: BUCK1, PWRCTRL_SEL=2, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
      PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=0.8/0.91, State=ON_HP */
      status = BSP_PMIC_WriteReg(board_regulators_table[VDDCPU].control_reg1, \
                                 0x1E); /* 0x1E ==> V=0.8V, 0x29 ==> V=0.91 */
      status = BSP_PMIC_WriteReg(board_regulators_table[VDDCPU].control_reg2, \
                                 MAIN_CR_PREG_MODE0 | MAIN_CR_EN);
      /* VDDCORE: BUCK2, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
      PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=0.82, State=ON_HP */
      status = BSP_PMIC_WriteReg(board_regulators_table[VDDCORE].control_reg1, \
                                 0x20);
      status = BSP_PMIC_WriteReg(board_regulators_table[VDDCORE].control_reg2, \
                                 MAIN_CR_PREG_MODE0 | MAIN_CR_EN);
      /* VDDGPU: BUCK3, PWRCTRL_SEL=0, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
      PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=0.8/0.9, State=ON_HP */
      status = BSP_PMIC_WriteReg(board_regulators_table[VDDGPU].control_reg1, \
                                 0x1E); /* 0x1E ==> V=0.8V, 0x28 ==> V=0.9 */
      status = BSP_PMIC_WriteReg(board_regulators_table[VDDGPU].control_reg2, \
                                 MAIN_CR_PREG_MODE0 | MAIN_CR_EN);
      /* VDDIO: BUCK4, PWRCTRL_SEL=0, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
      PWRCTRL_RST=0, Msk_Rst=1, Source=NA, V=3.3, State=ON_HP */
      status = BSP_PMIC_WriteReg(board_regulators_table[VDDIO_PMIC].control_reg1, \
                                 0x76);
      status = BSP_PMIC_WriteReg(board_regulators_table[VDDIO_PMIC].control_reg2, \
                                 MAIN_CR_PREG_MODE0 | MAIN_CR_EN);
      /* V1V8: BUCK5, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
      PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=1.8, State=ON_HP */
      status = BSP_PMIC_WriteReg(board_regulators_table[V1V8].control_reg1, \
                                 0x67);
      status = BSP_PMIC_WriteReg(board_regulators_table[V1V8].control_reg2, \
                                 MAIN_CR_PREG_MODE0 | MAIN_CR_EN);
      /* VDD_DDR: BUCK6, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
      PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=1.2, State=ON_HP */
      status = BSP_PMIC_WriteReg(board_regulators_table[VDD_DDR].control_reg1, \
                                 0x46);
      status = BSP_PMIC_WriteReg(board_regulators_table[VDD_DDR].control_reg2, \
                                 MAIN_CR_PREG_MODE0 | MAIN_CR_EN);
      /* V3V3: BUCK7, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
      PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=3.3, State=ON_HP */
      status = BSP_PMIC_WriteReg(board_regulators_table[V3V3].control_reg1, \
                                 0x76);
      status = BSP_PMIC_WriteReg(board_regulators_table[V3V3].control_reg2, \
                                 MAIN_CR_PREG_MODE0 | MAIN_CR_EN);
      /* STANDBY_PWRCTRL_SEL[1:0] = 0 */
      status = BSP_PMIC_ReadReg(MAIN_CR, &data); /* read MAIN_CR to save data */
      data &= ~STANDBY_PWRCTRL_SEL_3; /* clear STANDBY_PWRCTRL_SEL bits */
      status = BSP_PMIC_WriteReg(MAIN_CR, data); /* write MAIN_CR to set STANDBY_PWRCTRL_SEL[1:0] = 0 */
      break;

    case STPMIC2_RUN2_STOP2:
      /* VDDCPU: BUCK1, PWRCTRL_SEL=2, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
         PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=OFF */
      status = BSP_PMIC_WriteReg(board_regulators_table[VDDCPU].alt_control_reg1, 0x0);
      status = BSP_PMIC_WriteReg(board_regulators_table[VDDCPU].alt_control_reg2, 0x0);
      /* STANDBY_PWRCTRL_SEL[1:0] = 0 */
      status = BSP_PMIC_ReadReg(MAIN_CR, &data); /* read MAIN_CR to save data */
      data &= ~STANDBY_PWRCTRL_SEL_3; /* clear STANDBY_PWRCTRL_SEL bits */
      status = BSP_PMIC_WriteReg(MAIN_CR, data); /* write MAIN_CR to set STANDBY_PWRCTRL_SEL[1:0] = 0 */
      break;

    case STPMIC2_LP_STOP1:
      /* VTT_DDR: LDO3, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
      PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=SINK_SOURCE, State=OFF */
      status = BSP_PMIC_WriteReg(board_regulators_table[VTT_DDR].alt_control_reg1, \
                                 MAIN_CR_SNK_SRC);
      /* VPP_DDR: LDO5, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
      PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=2.5, State=ON */
      status = BSP_PMIC_WriteReg(board_regulators_table[VPP_DDR].alt_control_reg1, \
                                 (0x10 << 1) | MAIN_CR_EN);
      /* VREF_DDR: REFDDR, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
      PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=ON */
      status = BSP_PMIC_WriteReg(board_regulators_table[VREF_DDR].alt_control_reg1, \
                                 MAIN_CR_EN);
      status = BSP_PMIC_WriteReg(board_regulators_table[VREF_DDR].alt_control_reg2, \
                                 0x0);
      /* VDDCPU: BUCK1, PWRCTRL_SEL=2, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
      PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=0.8/0.91, State=ON_HP */
      status = BSP_PMIC_WriteReg(board_regulators_table[VDDCPU].alt_control_reg1, \
                                 0x1E);
      status = BSP_PMIC_WriteReg(board_regulators_table[VDDCPU].alt_control_reg2, \
                                 MAIN_CR_PREG_MODE0 | MAIN_CR_EN);
      /* VDDCORE: BUCK2, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
      PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=0,82, State=ON_HP */
      status = BSP_PMIC_WriteReg(board_regulators_table[VDDCORE].alt_control_reg1, \
                                 0x20);
      status = BSP_PMIC_WriteReg(board_regulators_table[VDDCORE].alt_control_reg2, \
                                 MAIN_CR_PREG_MODE0 | MAIN_CR_EN);
      /* VDDIO: BUCK4, PWRCTRL_SEL=0, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
      PWRCTRL_RST=0, Msk_Rst=1, Source=NA, V=3.3, State=ON_HP */
      status = BSP_PMIC_WriteReg(board_regulators_table[VDDIO_PMIC].alt_control_reg1, \
                                 0x76);
      status = BSP_PMIC_WriteReg(board_regulators_table[VDDIO_PMIC].alt_control_reg2, \
                                 MAIN_CR_PREG_MODE0 | MAIN_CR_EN);
      /* V1V8: BUCK5, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
      PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=1.8, State=ON_HP */
      status = BSP_PMIC_WriteReg(board_regulators_table[V1V8].alt_control_reg1, \
                                 0x67);
      status = BSP_PMIC_WriteReg(board_regulators_table[V1V8].alt_control_reg2, \
                                 MAIN_CR_PREG_MODE0 | MAIN_CR_EN);
      /* VDD_DDR: BUCK6, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
      PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=1.2, State=ON_HP */
      status = BSP_PMIC_WriteReg(board_regulators_table[VDD_DDR].alt_control_reg1, \
                                 0x46);
      status = BSP_PMIC_WriteReg(board_regulators_table[VDD_DDR].alt_control_reg2, \
                                 MAIN_CR_PREG_MODE0 | MAIN_CR_EN);
      /* V3V3: BUCK7, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
      PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=3.3, State=ON_HP */
      status = BSP_PMIC_WriteReg(board_regulators_table[V3V3].alt_control_reg1, \
                                 0x76);
      status = BSP_PMIC_WriteReg(board_regulators_table[V3V3].alt_control_reg2, \
                                 MAIN_CR_PREG_MODE0 | MAIN_CR_EN);
      /* STANDBY_PWRCTRL_SEL[1:0] = 0 */
      status = BSP_PMIC_ReadReg(MAIN_CR, &data); /* read MAIN_CR to save data */
      data &= ~STANDBY_PWRCTRL_SEL_3; /* clear STANDBY_PWRCTRL_SEL bits */
      status = BSP_PMIC_WriteReg(MAIN_CR, data); /* write MAIN_CR to set STANDBY_PWRCTRL_SEL[1:0] = 0 */
      break;

    case STPMIC2_LP_STOP2:
      /* VTT_DDR: LDO3, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
      PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=OFF */
      status = BSP_PMIC_WriteReg(board_regulators_table[VTT_DDR].alt_control_reg1, \
                                 0x0);
      /* VPP_DDR: LDO5, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
      PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=2.5, State=ON */
      status = BSP_PMIC_WriteReg(board_regulators_table[VPP_DDR].alt_control_reg1, \
                                 (0x10 << 1) | MAIN_CR_EN);
      /* VREF_DDR: REFDDR, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
      PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=ON */
      status = BSP_PMIC_WriteReg(board_regulators_table[VREF_DDR].alt_control_reg1, \
                                 MAIN_CR_EN);
      status = BSP_PMIC_WriteReg(board_regulators_table[VREF_DDR].alt_control_reg2, \
                                 0x0);
      /* VDDCPU: BUCK1, PWRCTRL_SEL=2, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
      PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=OFF */
      status = BSP_PMIC_WriteReg(board_regulators_table[VDDCPU].alt_control_reg1, \
                                 0x0);
      status = BSP_PMIC_WriteReg(board_regulators_table[VDDCPU].alt_control_reg2, \
                                 0x0);
      /* VDDCORE: BUCK2, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
      PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=0.82, State=ON_HP */
      status = BSP_PMIC_WriteReg(board_regulators_table[VDDCORE].alt_control_reg1, \
                                 0x20);
      status = BSP_PMIC_WriteReg(board_regulators_table[VDDCORE].alt_control_reg2, \
                                 MAIN_CR_PREG_MODE0 | MAIN_CR_EN);
      /* VDDIO: BUCK4, PWRCTRL_SEL=0, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
      PWRCTRL_RST=0, Msk_Rst=1, Source=NA, V=3.3, State=ON_HP */
      status = BSP_PMIC_WriteReg(board_regulators_table[VDDIO_PMIC].alt_control_reg1, \
                                 0x76);
      status = BSP_PMIC_WriteReg(board_regulators_table[VDDIO_PMIC].alt_control_reg2, \
                                 MAIN_CR_PREG_MODE0 | MAIN_CR_EN);
      /* V1V8: BUCK5, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
      PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=1.8, State=ON_HP */
      status = BSP_PMIC_WriteReg(board_regulators_table[V1V8].alt_control_reg1, \
                                 0x67);
      status = BSP_PMIC_WriteReg(board_regulators_table[V1V8].alt_control_reg2, \
                                 MAIN_CR_PREG_MODE0 | MAIN_CR_EN);
      /* VDD_DDR: BUCK6, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
      PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=1.2, State=ON_HP */
      status = BSP_PMIC_WriteReg(board_regulators_table[VDD_DDR].alt_control_reg1, \
                                 0x46);
      status = BSP_PMIC_WriteReg(board_regulators_table[VDD_DDR].alt_control_reg2, \
                                 MAIN_CR_PREG_MODE0 | MAIN_CR_EN);
      /* V3V3: BUCK7, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
      PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=3.3, State=ON_HP */
      status = BSP_PMIC_WriteReg(board_regulators_table[V3V3].alt_control_reg1, \
                                 0x76);
      status = BSP_PMIC_WriteReg(board_regulators_table[V3V3].alt_control_reg2, \
                                 MAIN_CR_PREG_MODE0 | MAIN_CR_EN);
      /* STANDBY_PWRCTRL_SEL[1:0] = 0 */
      status = BSP_PMIC_ReadReg(MAIN_CR, &data); /* read MAIN_CR to save data */
      data &= ~STANDBY_PWRCTRL_SEL_3; /* clear STANDBY_PWRCTRL_SEL bits */
      status = BSP_PMIC_WriteReg(MAIN_CR, data); /* write MAIN_CR to set STANDBY_PWRCTRL_SEL[1:0] = 0 */
      break;

    case STPMIC2_LPLV_STOP1:
      /* VTT_DDR: LDO3, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
      PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=SINK_SOURCE, State=OFF */
      status = BSP_PMIC_WriteReg(board_regulators_table[VTT_DDR].alt_control_reg1, \
                                 0x0);
      /* VPP_DDR: LDO5, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
      PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=2.5, State=ON */
      status = BSP_PMIC_WriteReg(board_regulators_table[VPP_DDR].alt_control_reg1, \
                                 (0x10 << 1) | MAIN_CR_EN);
      /* VREF_DDR: REFDDR, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
      PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=ON */
      status = BSP_PMIC_WriteReg(board_regulators_table[VREF_DDR].alt_control_reg1, \
                                 MAIN_CR_EN);
      status = BSP_PMIC_WriteReg(board_regulators_table[VREF_DDR].alt_control_reg2, 0x0);
      /* VDDCPU: BUCK1, PWRCTRL_SEL=2, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
      PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=0.8/0.91, State=ON_HP */
      status = BSP_PMIC_WriteReg(board_regulators_table[VDDCPU].alt_control_reg1, 0x1E);
      status = BSP_PMIC_WriteReg(board_regulators_table[VDDCPU].alt_control_reg2, \
                                 MAIN_CR_PREG_MODE0 | MAIN_CR_EN);
      /* VDDCORE: BUCK2, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
      PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=0,67, State=ON_HP */
      status = BSP_PMIC_WriteReg(board_regulators_table[VDDCORE].alt_control_reg1, 0x11);
      status = BSP_PMIC_WriteReg(board_regulators_table[VDDCORE].alt_control_reg2, \
                                 MAIN_CR_PREG_MODE0 | MAIN_CR_EN);
      /* VDDIO: BUCK4, PWRCTRL_SEL=0, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
      PWRCTRL_RST=0, Msk_Rst=1, Source=NA, V=3.3, State=ON_HP */
      status = BSP_PMIC_WriteReg(board_regulators_table[VDDIO_PMIC].alt_control_reg1, 0x76);
      status = BSP_PMIC_WriteReg(board_regulators_table[VDDIO_PMIC].alt_control_reg2, \
                                 MAIN_CR_PREG_MODE0 | MAIN_CR_EN);
      /* V1V8: BUCK5, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
      PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=1.8, State=ON_HP */
      status = BSP_PMIC_WriteReg(board_regulators_table[V1V8].alt_control_reg1, 0x67);
      status = BSP_PMIC_WriteReg(board_regulators_table[V1V8].alt_control_reg2, \
                                 MAIN_CR_PREG_MODE0 | MAIN_CR_EN);
      /* VDD_DDR: BUCK6, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
      PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=1.2, State=ON_HP */
      status = BSP_PMIC_WriteReg(board_regulators_table[VDD_DDR].alt_control_reg1, 0x46);
      status = BSP_PMIC_WriteReg(board_regulators_table[VDD_DDR].alt_control_reg2, \
                                 MAIN_CR_PREG_MODE0 | MAIN_CR_EN);
      /* V3V3: BUCK7, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
      PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=3.3, State=ON_HP */
      status = BSP_PMIC_WriteReg(board_regulators_table[V3V3].alt_control_reg1, 0x76);
      status = BSP_PMIC_WriteReg(board_regulators_table[V3V3].alt_control_reg2, \
                                 MAIN_CR_PREG_MODE0 | MAIN_CR_EN);
      /* STANDBY_PWRCTRL_SEL[1:0] = 0 */
      status = BSP_PMIC_ReadReg(MAIN_CR, &data); /* read MAIN_CR to save data */
      data &= ~STANDBY_PWRCTRL_SEL_3; /* clear STANDBY_PWRCTRL_SEL bits */
      status = BSP_PMIC_WriteReg(MAIN_CR, data); /* write MAIN_CR to set STANDBY_PWRCTRL_SEL[1:0] = 0 */
      break;

    case STPMIC2_LPLV_STOP2:
      /* VTT_DDR: LDO3, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
         PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=OFF */
      status = BSP_PMIC_WriteReg(board_regulators_table[VTT_DDR].alt_control_reg1, 0x0);
      /* VPP_DDR: LDO5, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
      PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=2.5, State=ON */
      status = BSP_PMIC_WriteReg(board_regulators_table[VPP_DDR].alt_control_reg1, \
                                 (0x10 << 1) | MAIN_CR_EN);
      /* VREF_DDR: REFDDR, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
      PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=ON */
      status = BSP_PMIC_WriteReg(board_regulators_table[VREF_DDR].alt_control_reg1, \
                                 MAIN_CR_EN);
      status = BSP_PMIC_WriteReg(board_regulators_table[VREF_DDR].alt_control_reg2, 0x0);
      /* VDDCPU: BUCK1, PWRCTRL_SEL=2, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
         PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=OFF */
      status = BSP_PMIC_WriteReg(board_regulators_table[VDDCPU].alt_control_reg1, 0x0);
      status = BSP_PMIC_WriteReg(board_regulators_table[VDDCPU].alt_control_reg2, 0x0);
      /* VDDCORE: BUCK2, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
         PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=0.67, State=ON_HP */
      status = BSP_PMIC_WriteReg(board_regulators_table[VDDCORE].alt_control_reg1, 0x11);
      status = BSP_PMIC_WriteReg(board_regulators_table[VDDCORE].alt_control_reg2, \
                                 MAIN_CR_PREG_MODE0 | MAIN_CR_EN);
      /* VDDIO: BUCK4, PWRCTRL_SEL=0, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
      PWRCTRL_RST=0, Msk_Rst=1, Source=NA, V=3.3, State=ON_HP */
      status = BSP_PMIC_WriteReg(board_regulators_table[VDDIO_PMIC].alt_control_reg1, 0x76);
      status = BSP_PMIC_WriteReg(board_regulators_table[VDDIO_PMIC].alt_control_reg2, \
                                 MAIN_CR_PREG_MODE0 | MAIN_CR_EN);
      /* V1V8: BUCK5, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
      PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=1.8, State=ON_HP */
      status = BSP_PMIC_WriteReg(board_regulators_table[V1V8].alt_control_reg1, 0x67);
      status = BSP_PMIC_WriteReg(board_regulators_table[V1V8].alt_control_reg2, \
                                 MAIN_CR_PREG_MODE0 | MAIN_CR_EN);
      /* VDD_DDR: BUCK6, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
      PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=1.2, State=ON_HP */
      status = BSP_PMIC_WriteReg(board_regulators_table[VDD_DDR].alt_control_reg1, 0x46);
      status = BSP_PMIC_WriteReg(board_regulators_table[VDD_DDR].alt_control_reg2, \
                                 MAIN_CR_PREG_MODE0 | MAIN_CR_EN);
      /* V3V3: BUCK7, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
      PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=3.3, State=ON_HP */
      status = BSP_PMIC_WriteReg(board_regulators_table[V3V3].alt_control_reg1, 0x76);
      status = BSP_PMIC_WriteReg(board_regulators_table[V3V3].alt_control_reg2, \
                                 MAIN_CR_PREG_MODE0 | MAIN_CR_EN);
      /* STANDBY_PWRCTRL_SEL[1:0] = 0 */
      status = BSP_PMIC_ReadReg(MAIN_CR, &data); /* read MAIN_CR to save data */
      data &= ~STANDBY_PWRCTRL_SEL_3; /* clear STANDBY_PWRCTRL_SEL bits */
      status = BSP_PMIC_WriteReg(MAIN_CR, data); /* write MAIN_CR to set STANDBY_PWRCTRL_SEL[1:0] = 0 */
      break;

    case STPMIC2_STANDBY_DDR_SR:
      /* VTT_DDR: LDO3, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
         PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=OFF */
      status = BSP_PMIC_WriteReg(board_regulators_table[VTT_DDR].alt_control_reg1, 0x0);
      /* VPP_DDR: LDO5, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
      PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=2.5, State=ON */
      status = BSP_PMIC_WriteReg(board_regulators_table[VPP_DDR].alt_control_reg1, \
                                 (0x10 << 1) | MAIN_CR_EN);
      /* VREF_DDR: REFDDR, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
      PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=ON */
      status = BSP_PMIC_WriteReg(board_regulators_table[VREF_DDR].alt_control_reg1, MAIN_CR_EN);
      status = BSP_PMIC_WriteReg(board_regulators_table[VREF_DDR].alt_control_reg2, 0x0);
      /* VDDCPU: BUCK1, PWRCTRL_SEL=2, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
         PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=OFF */
      status = BSP_PMIC_WriteReg(board_regulators_table[VDDCPU].alt_control_reg1, 0x0);
      status = BSP_PMIC_WriteReg(board_regulators_table[VDDCPU].alt_control_reg2, 0x0);
      /* VDDCORE: BUCK2, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
         PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=OFF */
      status = BSP_PMIC_WriteReg(board_regulators_table[VDDCORE].alt_control_reg1, 0x0);
      status = BSP_PMIC_WriteReg(board_regulators_table[VDDCORE].alt_control_reg2, 0x0);
      /* VDDIO: BUCK4, PWRCTRL_SEL=0, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
         PWRCTRL_RST=0, Msk_Rst=1, Source=NA, V=3.3, State=ON_LP */
      status = BSP_PMIC_WriteReg(board_regulators_table[VDDIO_PMIC].alt_control_reg1, 0x76);
      status = BSP_PMIC_WriteReg(board_regulators_table[VDDIO_PMIC].alt_control_reg2, \
                                 MAIN_CR_PREG_MODE1 | MAIN_CR_EN);
      /* V1V8: BUCK5, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
      PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=OFF */
      status = BSP_PMIC_WriteReg(board_regulators_table[V1V8].alt_control_reg1, 0x0);
      status = BSP_PMIC_WriteReg(board_regulators_table[V1V8].alt_control_reg2, 0x0);
      /* VDD_DDR: BUCK6, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
         PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=1.2, State=ON_LP */
      status = BSP_PMIC_WriteReg(board_regulators_table[VDD_DDR].alt_control_reg1, 0x46);
      status = BSP_PMIC_WriteReg(board_regulators_table[VDD_DDR].alt_control_reg2, \
                                 MAIN_CR_PREG_MODE1 | MAIN_CR_EN);
      /* V3V3: BUCK7, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
      PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=OFF */
      status = BSP_PMIC_WriteReg(board_regulators_table[V3V3].alt_control_reg1, 0x0);
      status = BSP_PMIC_WriteReg(board_regulators_table[V3V3].alt_control_reg2, 0x0);

      /* STANDBY_PWRCTRL_SEL[1:0] = 0 */
      status = BSP_PMIC_ReadReg(MAIN_CR, &data); /* read MAIN_CR to save data */
      data &= ~STANDBY_PWRCTRL_SEL_3; /* clear STANDBY_PWRCTRL_SEL bits */
      status = BSP_PMIC_WriteReg(MAIN_CR, data); /* write MAIN_CR to set STANDBY_PWRCTRL_SEL[1:0] = 0 */
      break;

    case STPMIC2_STANDBY_DDR_OFF:
      /* VTT_DDR: LDO3, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
         PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=OFF */
      status = BSP_PMIC_WriteReg(board_regulators_table[VTT_DDR].alt_control_reg1, 0x0);
      /* VPP_DDR: LDO5, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
         PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=OFF */
      status = BSP_PMIC_WriteReg(board_regulators_table[VPP_DDR].alt_control_reg1, 0x0);
      /* VREF_DDR: REFDDR, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
         PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=OFF */
      status = BSP_PMIC_WriteReg(board_regulators_table[VREF_DDR].alt_control_reg1, 0x0);
      status = BSP_PMIC_WriteReg(board_regulators_table[VREF_DDR].alt_control_reg2, 0x0);
      /* VDDCPU: BUCK1, PWRCTRL_SEL=2, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
         PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=OFF */
      status = BSP_PMIC_WriteReg(board_regulators_table[VDDCPU].alt_control_reg1, 0x0);
      status = BSP_PMIC_WriteReg(board_regulators_table[VDDCPU].alt_control_reg2, 0x0);
      /* VDDCORE: BUCK2, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
         PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=OFF */
      status = BSP_PMIC_WriteReg(board_regulators_table[VDDCORE].alt_control_reg1, 0x0);
      status = BSP_PMIC_WriteReg(board_regulators_table[VDDCORE].alt_control_reg2, 0x0);
      /* VDDIO: BUCK4, PWRCTRL_SEL=0, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
         PWRCTRL_RST=0, Msk_Rst=1, Source=NA, V=3.3, State=ON_LP */
      status = BSP_PMIC_WriteReg(board_regulators_table[VDDIO_PMIC].alt_control_reg1, 0x76);
      status = BSP_PMIC_WriteReg(board_regulators_table[VDDIO_PMIC].alt_control_reg2, \
                                 MAIN_CR_PREG_MODE1 | MAIN_CR_EN);
      /* V1V8: BUCK5, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
      PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=OFF */
      status = BSP_PMIC_WriteReg(board_regulators_table[V1V8].alt_control_reg1, 0x0);
      status = BSP_PMIC_WriteReg(board_regulators_table[V1V8].alt_control_reg2, 0x0);
      /* VDD_DDR: BUCK6, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
         PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=BNA, State=OFF */
      status = BSP_PMIC_WriteReg(board_regulators_table[VDD_DDR].alt_control_reg1, 0x0);
      status = BSP_PMIC_WriteReg(board_regulators_table[VDD_DDR].alt_control_reg2, 0x0);
      /* V3V3: BUCK7, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
         PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=OFF */
      status = BSP_PMIC_WriteReg(board_regulators_table[V3V3].alt_control_reg1, 0x0);
      status = BSP_PMIC_WriteReg(board_regulators_table[V3V3].alt_control_reg2, 0x0);
      /* STANDBY_PWRCTRL_SEL[1:0] = 1 */
      status = BSP_PMIC_ReadReg(MAIN_CR, &data); /* read MAIN_CR to save data */
      data &= ~STANDBY_PWRCTRL_SEL_3; /* clear STANDBY_PWRCTRL_SEL bits */
      status = BSP_PMIC_WriteReg(MAIN_CR, data | \
                                 STANDBY_PWRCTRL_SEL_1); /* write MAIN_CR to set STANDBY_PWRCTRL_SEL[1:0] = 1 */
      break;

    default :
      status = BSP_ERROR_WRONG_PARAM;
  }

  return status;
}

void STPMIC_Enable_Interrupt(PMIC_IRQn IRQn)
{
  uint8_t irq_reg;
  uint8_t irq_reg_value;
  uint8_t mask ;

  if (IRQn >= IRQ_NR)
  {
    return ;
  }

  /* IRQ register is IRQ Number divided by 8 */
  irq_reg = IRQn >> 3 ;

  /* value to be set in IRQ register corresponds to BIT(N) where N is the Interrupt id modulo 8 */
  irq_reg_value = 1 << (IRQn % 8);

  /* Get active mask from register */
  BSP_PMIC_ReadReg(INT_MASK_R1 + irq_reg, &mask);
  mask &=  ~irq_reg_value ;
  /* Clear relevant mask to enable interrupt */
  BSP_PMIC_WriteReg(INT_MASK_R1 + irq_reg, mask);

}

void STPMIC_Disable_Interrupt(PMIC_IRQn IRQn)
{
  uint8_t irq_reg;
  uint8_t irq_reg_value;
  uint8_t mask ;

  if (IRQn >= IRQ_NR)
  {
    return ;
  }

  /* IRQ register is IRQ Number divided by 8 */
  irq_reg = IRQn >> 3 ;

  /* value to be set in IRQ register corresponds to BIT(N) where N is the Interrupt id modulo 8 */
  irq_reg_value = 1 << (IRQn % 8);

  /* Get active mask from register */
  BSP_PMIC_ReadReg(INT_MASK_R1 + irq_reg, &mask);
  mask |=  irq_reg_value ;
  /* Set relevant mask to disable interrupt */
  BSP_PMIC_WriteReg(INT_MASK_R1 + irq_reg, mask);

}


void STPMIC_IrqHandler(void)
{
  uint8_t irq_reg;
  uint8_t mask;
  uint8_t pending_events;
  uint8_t i;

  for (irq_reg = 0 ; irq_reg < STM32_PMIC_NUM_IRQ_REGS ; irq_reg++)
  {
    /* Get pending events & active mask */
    BSP_PMIC_ReadReg(INT_MASK_R1 + irq_reg, &mask);
    BSP_PMIC_ReadReg(INT_PENDING_R1 + irq_reg, &pending_events);
    pending_events &=  ~mask ;

    /* Go through all bits for each register */
    for (i = 0 ; i < 8 ; i++)
    {
      if (pending_events & (1 << i))
      {
        /* Callback with parameter computes as "PMIC Interrupt" enum */
        STPMIC_INTn_Callback((PMIC_IRQn)(irq_reg * 8 + (7 - i)));
      }
    }
    /* Clear events in appropriate register for the event with mask set */
    BSP_PMIC_WriteReg(INT_CLEAR_R1 + irq_reg, pending_events);
  }

}

void STPMIC_Sw_Reset(void)
{
  /* Write 1 in bit 0 of MAIN_CONTROL Register */
  BSP_PMIC_UpdateReg(MAIN_CR, SOFTWARE_SWITCH_OFF_ENABLED);
  return ;
}


__weak void BSP_PMIC_INTn_Callback(PMIC_IRQn IRQn)
{
  switch (IRQn)
  {
    case STPMIC2_IT_PONKEY_F:
      printf("STPMIC2_IT_PONKEY_F");
      break;
    case STPMIC2_IT_PONKEY_R:
      printf("STPMIC2_IT_PONKEY_R");
      break;
    case STPMIC2_IT_WKP_FA:
      printf("STPMIC2_IT_WKP_FA");
      break;
    case STPMIC2_IT_WKP_RI:
      printf("STPMIC2_IT_WKP_RI");
      break;
    case STPMIC2_IT_VINLOW_FA:
      printf("STPMIC2_IT_VINLOW_FA");
      break;
    case STPMIC2_IT_VINLOW_RI:
      printf("STPMIC2_IT_VINLOW_RI");
      break;
    case STPMIC2_IT_VBUS_FA:
      printf("STPMIC2_IT_VBUS_FA");
      break;
    case STPMIC2_IT_VBUS_RI:
      printf("STPMIC2_IT_VBUS_RI");
      break;
    case STPMIC2_IT_THW_FA:
      printf("STPMIC2_IT_THW_FA");
      break;
    case STPMIC2_IT_THW_RI:
      printf("STPMIC2_IT_THW_RI");
      break;
    case STPMIC2_IT_BUCK1_OCP:
      printf("STPMIC2_IT_BUCK1_OCP");
      break;
    case STPMIC2_IT_BUCK2_OCP:
      printf("STPMIC2_IT_BUCK2_OCP");
      break;
    case STPMIC2_IT_BUCK3_OCP:
      printf("STPMIC2_IT_BUCK3_OCP");
      break;
    case STPMIC2_IT_BUCK4_OCP:
      printf("STPMIC2_IT_BUCK4_OCP");
      break;
    case STPMIC2_IT_BUCK5_OCP:
      printf("STPMIC2_IT_BUCK5_OCP");
      break;
    case STPMIC2_IT_BUCK6_OCP:
      printf("STPMIC2_IT_BUCK6_OCP");
      break;
    case STPMIC2_IT_BUCK7_OCP:
      printf("STPMIC2_IT_BUCK7_OCP");
      break;
    case STPMIC2_IT_REFDDR_OCP:
      printf("STPMIC2_IT_REFDDR_OCP");
      break;
    case STPMIC2_IT_LDO1_OCP:
      printf("STPMIC2_IT_LDO1_OCP");
      break;
    case STPMIC2_IT_LDO2_OCP:
      printf("STPMIC2_IT_LDO2_OCP");
      break;
    case STPMIC2_IT_LDO3_OCP:
      printf("STPMIC2_IT_LDO3_OCP");
      break;
    case STPMIC2_IT_LDO4_OCP:
      printf("STPMIC2_IT_LDO4_OCP");
      break;
    case STPMIC2_IT_LDO5_OCP:
      printf("STPMIC2_IT_LDO5_OCP");
      break;
    case STPMIC2_IT_LDO6_OCP:
      printf("STPMIC2_IT_LDO6_OCP");
      break;
    case STPMIC2_IT_LDO7_OCP:
      printf("STPMIC2_IT_LDO7_OCP");
      break;
    case STPMIC2_IT_LDO8_OCP:
      printf("STPMIC2_IT_LDO8_OCP");
      break;
    default:
      printf("Unknown IRQ %d", IRQn);
      break;
  }
  printf(" Interrupt received\n\r");
}

void STPMIC_INTn_Callback(PMIC_IRQn IRQn)
{
  BSP_PMIC_INTn_Callback(IRQn);
};
#if 0 /* SBA test */
void BSP_PMIC_INTn_IRQHandler(void)
{
  /*printf("%s:%d\n\r", __FUNCTION__, __LINE__);*/
  /* Call HAL EXTI IRQ Handler to clear appropriate flags */
  HAL_GPIO_EXTI_IRQHandler(PMIC_INTN_PIN);

  STPMIC_IrqHandler();
}
#endif /* 0 */

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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
