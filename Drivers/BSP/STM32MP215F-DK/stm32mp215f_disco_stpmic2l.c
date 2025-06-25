/**
  ******************************************************************************
  * @file    stm32mp215f_disco_stpmic2l.c
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
#include "stm32mp215f_disco.h" /* TODO */
#include "stm32mp215f_disco_bus.h" /* TODO */
#include "stm32mp215f_disco_stpmic2l.h"
#include <string.h>
#include <stdio.h>

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32MP215F_DISCO
  * @{
  */

/** @addtogroup STPMIC2L
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
extern I2C_HandleTypeDef hI2c3;

static STPMIC2L_Drv_t *Stpmic2lDrv = NULL;
static void         *CompObj = NULL;


/*
  Table of EVAL board regulators
*/

board_regul_struct_t board_regulators_table[] =
{
  {
    "VDDCORE",      VDDCORE,     STPMIC_BUCK1,  BUCK1_MAIN_CR1,  BUCK1_MAIN_CR2,  \
    BUCK1_ALT_CR1,  BUCK1_ALT_CR2,  BUCK1_PWRCTRL_CR,  820,  820
  },
  {
    "VDD2_DDR",     VDD2_DDR,    STPMIC_BUCK2,  BUCK2_MAIN_CR1,  BUCK2_MAIN_CR2,  \
    BUCK2_ALT_CR1,  BUCK2_ALT_CR2,  BUCK2_PWRCTRL_CR, 1100, 1100
  },
  {
    "VDDCPU",       VDDCPU,     STPMIC_BUCK3,  BUCK3_MAIN_CR1,  BUCK3_MAIN_CR2,  \
    BUCK3_ALT_CR1,  BUCK3_ALT_CR2,  BUCK3_PWRCTRL_CR,  800,  910
  },
  {
    "VDDA1V8_AON",  VDDA1V8_AON,  STPMIC_LDO1,   LDO1_MAIN_CR,    LDO1_MAIN_CR,    \
    LDO1_ALT_CR,    LDO1_ALT_CR,    LDO1_PWRCTRL_CR,  1800, 1800
  },
  {
    "VDDIO",        VDDIO,  STPMIC_LDO2,   LDO2_MAIN_CR,    LDO2_MAIN_CR,    \
    LDO2_ALT_CR,    LDO2_ALT_CR,    LDO2_PWRCTRL_CR,  3300, 3300
  },
  {
    "VDD1_DDR",     VDD1_DDR,     STPMIC_LDO3,   LDO3_MAIN_CR,    LDO3_MAIN_CR,    \
    LDO3_ALT_CR,    LDO3_ALT_CR,    LDO3_PWRCTRL_CR,  1800, 1800
  },
  {
    "VDD3V3_USB",   VDD3V3_USB,  STPMIC_LDO4,   LDO4_MAIN_CR,    LDO4_MAIN_CR,    \
    LDO4_ALT_CR,    LDO4_ALT_CR,    LDO4_PWRCTRL_CR,   3300, 3300
  },
  {
    "VDD_FLASH",    VDD_FLASH,    STPMIC_LDO5,   LDO5_MAIN_CR,    LDO5_MAIN_CR,    \
    LDO5_ALT_CR,    LDO5_ALT_CR,    LDO5_PWRCTRL_CR,   3300, 3300
  },
  {
    "VDDA_1V8",     VDDA_1V8,    STPMIC_LDO6,   LDO6_MAIN_CR,    LDO6_MAIN_CR,    \
    LDO6_ALT_CR,    LDO6_ALT_CR,    LDO6_PWRCTRL_CR,   3300, 3300
  },
  {
    "LDO7_FREE",    LDO7_FREE,    STPMIC_LDO7,   LDO7_MAIN_CR,    LDO7_MAIN_CR,    \
    LDO7_ALT_CR,    LDO7_ALT_CR,    LDO7_PWRCTRL_CR,   0, 0
  },
  {
    "GPO1_3V3",    GPO1_3V3,    STPMIC_GPO1,   GPO1_MAIN_CR,    GPO1_MAIN_CR,    \
    GPO1_ALT_CR,    GPO1_ALT_CR,    GPO1_PWRCTRL_CR,   0, 0
  },
  {
    "GPO2_FREE",    GPO2_FREE,    STPMIC_GPO2,   GPO2_MAIN_CR,    GPO2_MAIN_CR,    \
    GPO2_ALT_CR,    GPO2_ALT_CR,    GPO2_PWRCTRL_CR,   0, 0
  },
{
    "GPO3_LED5",    GPO3_LED5,    STPMIC_GPO3,   GPO3_MAIN_CR,    GPO3_MAIN_CR,    \
    GPO3_ALT_CR,    GPO3_ALT_CR,    GPO3_PWRCTRL_CR,   0, 0
  },
{
    "GPO4_FREE",    GPO4_FREE,    STPMIC_GPO4,   GPO4_MAIN_CR,    GPO4_MAIN_CR,    \
    GPO4_ALT_CR,    GPO4_ALT_CR,    GPO4_PWRCTRL_CR,   0, 0
  },
{
    "GPO5_3V3_BKP", GPO5_3V3_BKP,    STPMIC_GPO5,   GPO5_MAIN_CR,    GPO5_MAIN_CR,    \
    GPO5_ALT_CR,    GPO5_ALT_CR,    GPO5_PWRCTRL_CR,   0, 0
  },
};


/**
  * @}
 */

/** @defgroup STPMIC_Private_Functions Private Functionss
  * @{
  */

/* Private function prototypes -----------------------------------------------*/

STPMIC2L_Object_t   STPMIC2LObj = { 0 };
static int32_t STPMIC_Probe();
void STPMIC_Enable_Interrupt(PMIC_IRQn IRQn);
void STPMIC_Disable_Interrupt(PMIC_IRQn IRQn);
void STPMIC_INTn_Callback(PMIC_IRQn IRQn);

/* Private functions ---------------------------------------------------------*/


static int32_t STPMIC_Probe()
{
  int32_t ret = STPMIC2L_OK;
  STPMIC2L_IO_t       IOCtx;
  uint8_t                id;

  /* Only perform the init if the object already exist */
  if (!CompObj)
  {
    /* Configure the I2C driver */
    IOCtx.Address     = STPMIC_I2C_ADDRESS;
    IOCtx.Init        = BSP_I2C3_Init;
    IOCtx.DeInit      = BSP_I2C3_DeInit;
    IOCtx.ReadReg     = BSP_I2C3_ReadReg;
    IOCtx.WriteReg    = BSP_I2C3_WriteReg;
    IOCtx.GetTick     = BSP_GetTick;

    ret = STPMIC2L_RegisterBusIO(&STPMIC2LObj, &IOCtx);
    if (ret != STPMIC2L_OK)
    {
      return BSP_ERROR_COMPONENT_FAILURE;
    }

    ret = STPMIC2L_ReadID(&STPMIC2LObj, &id);
    if (ret != STPMIC2L_OK)
    {
      return BSP_ERROR_COMPONENT_FAILURE;
    }

    if ((id & 0xF0) != STPMIC2L_ID)
    {
      return BSP_ERROR_UNKNOWN_COMPONENT;
    }

    Stpmic2lDrv = (STPMIC2L_Drv_t *) &STPMIC2L_Driver;
    CompObj = &STPMIC2LObj;
  }

  ret = Stpmic2lDrv->Init(CompObj);
  if (ret != STPMIC2L_OK)
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

  /* INTn - Interrupt Line - Active Low (Falling Edge) */
  PMIC_INTN_CLK_ENABLE();

  GPIO_InitStruct.Pin       = PMIC_INTN_PIN;
  GPIO_InitStruct.Mode      = GPIO_MODE_IT_FALLING;
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

  STPMIC_Enable_Interrupt(STPMIC2L_IT_PONKEY_F);
  STPMIC_Enable_Interrupt(STPMIC2L_IT_PONKEY_R);

  return BSP_ERROR_NONE;
}

uint32_t BSP_PMIC_DeInit(void)
{
  uint32_t  status = BSP_ERROR_NONE;

  status = Stpmic2lDrv->DeInit(CompObj);
  if (status != STPMIC2L_OK)
  {
    return BSP_ERROR_COMPONENT_FAILURE;
  }

  return status;
}

uint32_t BSP_PMIC_ReadReg(uint8_t reg, uint8_t *pdata)
{
  int32_t  status = BSP_ERROR_NONE;

  status = Stpmic2lDrv->ReadReg(CompObj, reg, pdata);
  if (status != STPMIC2L_OK)
  {
    return BSP_ERROR_COMPONENT_FAILURE;
  }
  return status;
}

uint32_t BSP_PMIC_WriteReg(uint8_t reg, uint8_t data)
{
  int32_t  status = BSP_ERROR_NONE;

  status = Stpmic2lDrv->WriteReg(CompObj, reg, data);
  if (status != STPMIC2L_OK)
  {
    return BSP_ERROR_COMPONENT_FAILURE;
  }
  return status;
}

uint32_t BSP_PMIC_UpdateReg(uint8_t reg, uint8_t mask)
{
  int32_t  status = BSP_ERROR_NONE;

  status = Stpmic2lDrv->UpdateReg(CompObj, reg, mask);
  if (status != STPMIC2L_OK)
  {
    return BSP_ERROR_COMPONENT_FAILURE;
  }
  return status;
}

#if defined (STPMIC2L_DEBUG)
/*
 *
 * PMIC registers dump
 *
 */
/* following are configurations */
uint32_t BSP_PMIC_DumpRegs(void)
{
  uint32_t  status = BSP_ERROR_NONE;

  status = Stpmic2lDrv->DumpRegs(CompObj);
  if (status != STPMIC2L_OK)
  {
    return BSP_ERROR_COMPONENT_FAILURE;
  }

  return status;
}
#endif /* defined (STPMIC2L_DEBUG) */

/*
 *
 * @brief BSP_PMIC_DDR_Power_Init initialize DDR power
 *
 * DDR power on sequence is:
 * enable VDD1_DDR
 * enable VDD2_DDR
 *
 * @param  None
 * @retval status
 *
 */
/* following are configurations */
uint32_t BSP_PMIC_DDR_Power_Init()
{
  uint32_t  status = BSP_ERROR_NONE;

  /* VDD1_DDR ==> LDO3 ==> 1800mV */
  if (BSP_PMIC_WriteReg(board_regulators_table[VDD1_DDR].control_reg1, (0x9 << 1)) != BSP_ERROR_NONE)
  {
    return BSP_ERROR_PMIC;
  }

  /* VDD2_DDR ==> BUCK2 ==> 1100mV */
  if (BSP_PMIC_WriteReg(board_regulators_table[VDD2_DDR].control_reg1, 0x3C) != BSP_ERROR_NONE)
  {
    return BSP_ERROR_PMIC;
  }

  /* enable VDD1_DDR */
  status = BSP_PMIC_REGU_Set_On(VDD1_DDR);
  if (status != BSP_ERROR_NONE)
  {
    return status;
  }

  /* enable VDD2_DDR */
  return BSP_PMIC_REGU_Set_On(VDD2_DDR);
}

uint32_t BSP_PMIC_REGU_Set_On(board_regul_t regu)
{
  switch (regu)
  {
    case VDDA1V8_AON:
    case VDDIO:
    case VDD1_DDR:
    case VDD3V3_USB:
    case VDD_FLASH:
    case VDDA_1V8:
    case LDO7_FREE:
    case GPO1_3V3:
    case GPO2_FREE:
    case GPO3_LED5:
    case GPO4_FREE:
    case GPO5_3V3_BKP:
      /* set enable bit */
      if (BSP_PMIC_UpdateReg(board_regulators_table[regu].control_reg1, MAIN_CR_EN) != BSP_ERROR_NONE)
      {
        return BSP_ERROR_PMIC;
      }
      break;
    case VDDCORE:
    case VDD2_DDR:
    case VDDCPU:
      /* set enable bit */
      if (BSP_PMIC_UpdateReg(board_regulators_table[regu].control_reg2, MAIN_CR_EN) != BSP_ERROR_NONE)
      {
        return BSP_ERROR_PMIC;
      }
      break;
    default:
      return BSP_ERROR_WRONG_PARAM;
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
    case VDDIO:
    case VDD1_DDR:
    case VDD3V3_USB:
    case VDD_FLASH:
    case VDDA_1V8:
    case LDO7_FREE:
    case GPO1_3V3:
    case GPO2_FREE:
    case GPO3_LED5:
    case GPO4_FREE:
    case GPO5_3V3_BKP:
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

    case VDDCORE:
    case VDD2_DDR:
    case VDDCPU:
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
      return BSP_ERROR_WRONG_PARAM;
      break;
  }

  /* Default ramp delay of 1ms */
  HAL_Delay(1);

  return BSP_ERROR_NONE;
}

uint32_t BSP_PMIC_DDR_Power_Off()
{
  uint32_t  status = BSP_ERROR_NONE;

  /* disable VDD1_DDR */
  status = BSP_PMIC_REGU_Set_Off(VDD1_DDR);
  if (status != BSP_ERROR_NONE)
  {
    return status;
  }

  /* disable VDD2_DDR */
  return BSP_PMIC_REGU_Set_Off(VDD2_DDR);
}

/**
  * @brief  BSP_PMIC_Set_Power_Mode Set PMIC run/low power mode
  * @retval status
  */

uint32_t BSP_PMIC_Power_Mode_Init()
{
  uint32_t  status = BSP_ERROR_NONE;

  /* Fail-safe overcurrent protection */
  status = BSP_PMIC_WriteReg(FS_OCP_CR1, FS_OCP_BUCK2 | FS_OCP_BUCK2 | FS_OCP_BUCK1);
  status = BSP_PMIC_WriteReg(FS_OCP_CR2, FS_OCP_LDO6 | FS_OCP_LDO3 | FS_OCP_LDO2 | FS_OCP_LDO1);


  /* Mask reset LDO control register (LDOS_MRST_CR) for VDDA1V8_AON(LDO1) & VDDIO(LDO2) */
  status = BSP_PMIC_WriteReg(LDOS_MRST_CR, LDO1_MRST);
  status = BSP_PMIC_WriteReg(LDOS_MRST_CR, LDO2_MRST);

  /* VDDA1V8_AON: LDO1, PWRCTRL_SEL=0, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
  PWRCTRL_RST=0, Msk_Rst=1, Source=NA, V=NA, State=ON */
  status = BSP_PMIC_WriteReg(board_regulators_table[VDDA1V8_AON].control_reg1, MAIN_CR_EN);
  status = BSP_PMIC_WriteReg(board_regulators_table[VDDA1V8_AON].pwr_control_reg, 0x0);

  /* VDDIO: LDO2, PWRCTRL_SEL=0, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
  PWRCTRL_RST=0, Msk_Rst=1, Source=NA, V=3.3, State=ON */
  status = BSP_PMIC_WriteReg(board_regulators_table[VDDIO].control_reg1, \
                             (0x18 << 1) | MAIN_CR_EN);
  status = BSP_PMIC_WriteReg(board_regulators_table[VDDIO].pwr_control_reg, 0x0);

  /* VDD1_DDR: LDO3, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=1.8, State=ON */
  status = BSP_PMIC_WriteReg(board_regulators_table[VDD1_DDR].control_reg1, \
                             (0x9 << 1) | MAIN_CR_EN);
  status = BSP_PMIC_WriteReg(board_regulators_table[VDD1_DDR].pwr_control_reg, \
                             PWRCTRL_CR_SEL1 | PWRCTRL_CR_DLY_H0 | PWRCTRL_CR_DLY_L0 | PWRCTRL_CR_EN);

  /* VDD3V3_USB: LDO4, PWRCTRL_SEL=0, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=3.3, State=ON */
  status = BSP_PMIC_WriteReg(board_regulators_table[VDD3V3_USB].control_reg1, MAIN_CR_EN);
  status = BSP_PMIC_WriteReg(board_regulators_table[VDD3V3_USB].pwr_control_reg, 0x0);

  /* VDD_FLASH: LDO5, PWRCTRL_SEL=3, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
  PWRCTRL_RST=1, Msk_Rst=0, Source=NA, V=3.3, State=ON */
  status = BSP_PMIC_WriteReg(board_regulators_table[VDD_FLASH].control_reg1, \
                             (0x18 << 1) |  MAIN_CR_EN);
  status = BSP_PMIC_WriteReg(board_regulators_table[VDD_FLASH].pwr_control_reg, \
                             PWRCTRL_CR_SEL3 | PWRCTRL_CR_DLY_H0 | PWRCTRL_CR_DLY_L0 | PWRCTRL_CR_RST);

  /* VDDA_1V8: LDO6, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=1.8, State=ON */
  status = BSP_PMIC_WriteReg(board_regulators_table[VDDA_1V8].control_reg1, \
                             (0x9 << 1) |  MAIN_CR_EN);
  status = BSP_PMIC_WriteReg(board_regulators_table[VDDA_1V8].pwr_control_reg, \
                             PWRCTRL_CR_SEL1 | PWRCTRL_CR_DLY_H0 | PWRCTRL_CR_DLY_L0 | PWRCTRL_CR_EN);
  
  /* LDO7_FREE: LDO7, PWRCTRL_SEL=0, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=FF */
  status = BSP_PMIC_WriteReg(board_regulators_table[LDO7_FREE].control_reg1, 0x0);
  status = BSP_PMIC_WriteReg(board_regulators_table[LDO7_FREE].pwr_control_reg, 0x0);

  /* GPO1_3V3: GPO1, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=ON */
  status = BSP_PMIC_WriteReg(board_regulators_table[GPO1_3V3].control_reg1, MAIN_CR_EN);
  status = BSP_PMIC_WriteReg(board_regulators_table[GPO1_3V3].pwr_control_reg, \
                             PWRCTRL_CR_SEL1 | PWRCTRL_CR_DLY_H0 | PWRCTRL_CR_DLY_L0 | PWRCTRL_CR_EN);

  /* GPO2_FREE: GPO2, PWRCTRL_SEL=0, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=OFF */
  status = BSP_PMIC_WriteReg(board_regulators_table[GPO2_FREE].control_reg1, 0x0);
  status = BSP_PMIC_WriteReg(board_regulators_table[GPO2_FREE].pwr_control_reg, 0x0);

  /* GPO3_LED5: GPO3, PWRCTRL_SEL=0, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=ON */
  status = BSP_PMIC_WriteReg(board_regulators_table[GPO3_LED5].control_reg1, MAIN_CR_EN);
  status = BSP_PMIC_WriteReg(board_regulators_table[GPO3_LED5].pwr_control_reg, 0x0);

  /* GPO4_FREE: GPO4, PWRCTRL_SEL=0, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=OFF */
  status = BSP_PMIC_WriteReg(board_regulators_table[GPO4_FREE].control_reg1, 0x0);
  status = BSP_PMIC_WriteReg(board_regulators_table[GPO4_FREE].pwr_control_reg, 0x0);

  /* GPO5_3V3_BKP: GPO5, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=ON */
  status = BSP_PMIC_WriteReg(board_regulators_table[GPO5_3V3_BKP].control_reg1, MAIN_CR_EN);
  status = BSP_PMIC_WriteReg(board_regulators_table[GPO5_3V3_BKP].pwr_control_reg, \
                             PWRCTRL_CR_SEL1 | PWRCTRL_CR_DLY_H0 | PWRCTRL_CR_DLY_L0 | PWRCTRL_CR_EN);

  /* VDDCORE: BUCK1, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=0.82, State=ON_AUTO */
  status = BSP_PMIC_WriteReg(board_regulators_table[VDDCORE].control_reg1, 0x20); 
  status = BSP_PMIC_WriteReg(board_regulators_table[VDDCORE].control_reg2, \
                             MAIN_CR_PREG_AUTO_MODE | MAIN_CR_EN);
  status = BSP_PMIC_WriteReg(board_regulators_table[VDDCORE].pwr_control_reg, \
                             PWRCTRL_CR_SEL1 | PWRCTRL_CR_DLY_H0 | PWRCTRL_CR_DLY_L0 | PWRCTRL_CR_EN);

  /* VDD2_DDR: BUCK2, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=1.1, State=ON_AUTO */
  status = BSP_PMIC_WriteReg(board_regulators_table[VDD2_DDR].control_reg1, 0x3C);
  status = BSP_PMIC_WriteReg(board_regulators_table[VDD2_DDR].control_reg2, \
                             MAIN_CR_PREG_AUTO_MODE | MAIN_CR_EN);
  status = BSP_PMIC_WriteReg(board_regulators_table[VDD2_DDR].pwr_control_reg, \
                             PWRCTRL_CR_SEL1 | PWRCTRL_CR_DLY_H0 | PWRCTRL_CR_DLY_L0 | PWRCTRL_CR_EN);

  /* VDDCPU: BUCK3, PWRCTRL_SEL=2, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=0.8, State=ON_AUTO */
  status = BSP_PMIC_WriteReg(board_regulators_table[VDDCPU].control_reg1, 0x1E);
  status = BSP_PMIC_WriteReg(board_regulators_table[VDDCPU].control_reg2, \
                             MAIN_CR_PREG_AUTO_MODE | MAIN_CR_EN);
  status = BSP_PMIC_WriteReg(board_regulators_table[VDDCPU].pwr_control_reg, \
                             PWRCTRL_CR_SEL2 | PWRCTRL_CR_DLY_H0 | PWRCTRL_CR_DLY_L0 | PWRCTRL_CR_EN);

  return status;
}

/**
  * @brief  BSP_PMIC_Set_Power_Mode Set PMIC power mode
  * @param  mode  mode to set
  * @retval status
  * TODO: It needs modifications for Low Power Mode
  */
uint32_t BSP_PMIC_Set_Power_Mode(__attribute__((unused))uint32_t mode)
{
  uint32_t  status = BSP_ERROR_NONE;

  uint8_t data;

  switch (mode)
  {
case STPMIC2L_RUN1_STOP1:

	  /* VDDA1V8_AON: LDO1, PWRCTRL_SEL=0, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
	  PWRCTRL_RST=0, Msk_Rst=1, Source=NA, V=NA, State=ON */
	  status = BSP_PMIC_WriteReg(board_regulators_table[VDDA1V8_AON].control_reg1, MAIN_CR_EN);

	  /* VDDIO: LDO2, PWRCTRL_SEL=0, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
	  PWRCTRL_RST=0, Msk_Rst=1, Source=NA, V=3.3, State=ON */
	  status = BSP_PMIC_WriteReg(board_regulators_table[VDDIO].control_reg1, \
	                             (0x18 << 1) | MAIN_CR_EN);

	  /* VDD1_DDR: LDO3, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
	  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=1.8, State=ON */
	  status = BSP_PMIC_WriteReg(board_regulators_table[VDD1_DDR].control_reg1, \
	                             (0x9 << 1) | MAIN_CR_EN);

	  /* VDD3V3_USB: LDO4, PWRCTRL_SEL=0, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
	  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=3.3, State=ON */
	  status = BSP_PMIC_WriteReg(board_regulators_table[VDD3V3_USB].control_reg1, MAIN_CR_EN);

	  /* VDD_FLASH: LDO5, PWRCTRL_SEL=3, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
	  PWRCTRL_RST=1, Msk_Rst=0, Source=NA, V=3.3, State=ON */
	  status = BSP_PMIC_WriteReg(board_regulators_table[VDD_FLASH].control_reg1, \
	                             (0x18 << 1) |  MAIN_CR_EN);


	  /* VDDA_1V8: LDO6, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
	  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=1.8, State=ON */
	  status = BSP_PMIC_WriteReg(board_regulators_table[VDDA_1V8].control_reg1, \
	                             (0x9 << 1) |  MAIN_CR_EN);

	  /* LDO7_FREE: LDO7, PWRCTRL_SEL=0, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
	  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=FF */
	  status = BSP_PMIC_WriteReg(board_regulators_table[LDO7_FREE].control_reg1, 0x0);

	  /* GPO1_3V3: GPO1, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
	  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=ON */
	  status = BSP_PMIC_WriteReg(board_regulators_table[GPO1_3V3].control_reg1, MAIN_CR_EN);

	  /* GPO2_FREE: GPO2, PWRCTRL_SEL=0, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
	  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=OFF */
	  status = BSP_PMIC_WriteReg(board_regulators_table[GPO2_FREE].control_reg1, 0x0);

	  /* GPO3_LED5: GPO3, PWRCTRL_SEL=0, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
	  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=ON */
	  status = BSP_PMIC_WriteReg(board_regulators_table[GPO3_LED5].control_reg1, MAIN_CR_EN);

	  /* GPO4_FREE: GPO4, PWRCTRL_SEL=0, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
	  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=OFF */
	  status = BSP_PMIC_WriteReg(board_regulators_table[GPO4_FREE].control_reg1, 0x0);

	  /* GPO5_3V3_BKP: GPO5, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
	  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=ON */
	  status = BSP_PMIC_WriteReg(board_regulators_table[GPO5_3V3_BKP].control_reg1, MAIN_CR_EN);

	  /* VDDCORE: BUCK1, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
	  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=0.82, State=ON_AUTO */
	  status = BSP_PMIC_WriteReg(board_regulators_table[VDDCORE].control_reg1, 0x20);
	  status = BSP_PMIC_WriteReg(board_regulators_table[VDDCORE].control_reg2, \
	                             MAIN_CR_PREG_AUTO_MODE | MAIN_CR_EN);

	  /* VDD2_DDR: BUCK2, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
	  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=1.1, State=ON_AUTO */
	  status = BSP_PMIC_WriteReg(board_regulators_table[VDD2_DDR].control_reg1, 0x3C);
	  status = BSP_PMIC_WriteReg(board_regulators_table[VDD2_DDR].control_reg2, \
	                             MAIN_CR_PREG_AUTO_MODE | MAIN_CR_EN);

	  /* VDDCPU: BUCK3, PWRCTRL_SEL=2, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
	  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=0.8, State=ON_AUTO */
	  status = BSP_PMIC_WriteReg(board_regulators_table[VDDCPU].control_reg1, 0x1E);
	  status = BSP_PMIC_WriteReg(board_regulators_table[VDDCPU].control_reg2, \
	                             MAIN_CR_PREG_AUTO_MODE | MAIN_CR_EN);

   /* STANDBY_PWRCTRL_SEL[1:0] = 0 */
   status = BSP_PMIC_ReadReg(MAIN_CR, &data); /* read MAIN_CR to save data */
   data &= ~STANDBY_PWRCTRL_SEL_3; /* clear STANDBY_PWRCTRL_SEL bits */
   status = BSP_PMIC_WriteReg(MAIN_CR, data); /* write MAIN_CR to set STANDBY_PWRCTRL_SEL[1:0] = 0 */
break;

case STPMIC2L_RUN2_STOP2:

	  /* VDDA1V8_AON: LDO1, PWRCTRL_SEL=0, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
	  PWRCTRL_RST=0, Msk_Rst=1, Source=NA, V=NA, State=ON */
	  status = BSP_PMIC_WriteReg(board_regulators_table[VDDA1V8_AON].control_reg1, MAIN_CR_EN);

	  /* VDDIO: LDO2, PWRCTRL_SEL=0, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
	  PWRCTRL_RST=0, Msk_Rst=1, Source=NA, V=3.3, State=ON */
	  status = BSP_PMIC_WriteReg(board_regulators_table[VDDIO].control_reg1, \
	                             (0x18 << 1) | MAIN_CR_EN);

	  /* VDD1_DDR: LDO3, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
	  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=1.8, State=ON */
	  status = BSP_PMIC_WriteReg(board_regulators_table[VDD1_DDR].control_reg1, \
	                             (0x9 << 1) | MAIN_CR_EN);

	  /* VDD3V3_USB: LDO4, PWRCTRL_SEL=0, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
	  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=3.3, State=ON */
	  status = BSP_PMIC_WriteReg(board_regulators_table[VDD3V3_USB].control_reg1, MAIN_CR_EN);

	  /* VDD_FLASH: LDO5, PWRCTRL_SEL=3, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
	  PWRCTRL_RST=1, Msk_Rst=0, Source=NA, V=NA, State=OFF */
	  status = BSP_PMIC_WriteReg(board_regulators_table[VDD_FLASH].control_reg1, 0x0);


	  /* VDDA_1V8: LDO6, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
	  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=1.8, State=ON */
	  status = BSP_PMIC_WriteReg(board_regulators_table[VDDA_1V8].control_reg1, \
	                             (0x9 << 1) |  MAIN_CR_EN);

	  /* LDO7_FREE: LDO7, PWRCTRL_SEL=0, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
	  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=OFF */
	  status = BSP_PMIC_WriteReg(board_regulators_table[LDO7_FREE].control_reg1, 0x0);

	  /* GPO1_3V3: GPO1, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
	  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=ON */
	  status = BSP_PMIC_WriteReg(board_regulators_table[GPO1_3V3].control_reg1, MAIN_CR_EN);

	  /* GPO2_FREE: GPO2, PWRCTRL_SEL=0, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
	  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=OFF */
	  status = BSP_PMIC_WriteReg(board_regulators_table[GPO2_FREE].control_reg1, 0x0);

	  /* GPO3_LED5: GPO3, PWRCTRL_SEL=0, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
	  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=ON */
	  status = BSP_PMIC_WriteReg(board_regulators_table[GPO3_LED5].control_reg1, MAIN_CR_EN);

	  /* GPO4_FREE: GPO4, PWRCTRL_SEL=0, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
	  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=OFF */
	  status = BSP_PMIC_WriteReg(board_regulators_table[GPO4_FREE].control_reg1, 0x0);

	  /* GPO5_3V3_BKP: GPO5, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
	  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=ON */
	  status = BSP_PMIC_WriteReg(board_regulators_table[GPO5_3V3_BKP].control_reg1, MAIN_CR_EN);

	  /* VDDCORE: BUCK1, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
	  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=0.82, State=ON_AUTO */
	  status = BSP_PMIC_WriteReg(board_regulators_table[VDDCORE].control_reg1, 0x20);
	  status = BSP_PMIC_WriteReg(board_regulators_table[VDDCORE].control_reg2, \
	                             MAIN_CR_PREG_AUTO_MODE | MAIN_CR_EN);

	  /* VDD2_DDR: BUCK2, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
	  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=1.1, State=ON_AUTO */
	  status = BSP_PMIC_WriteReg(board_regulators_table[VDD2_DDR].control_reg1, 0x3C);
	  status = BSP_PMIC_WriteReg(board_regulators_table[VDD2_DDR].control_reg2, \
	                             MAIN_CR_PREG_AUTO_MODE | MAIN_CR_EN);

	  /* VDDCPU: BUCK3, PWRCTRL_SEL=2, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
	  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=OFF */
	  status = BSP_PMIC_WriteReg(board_regulators_table[VDDCPU].alt_control_reg1, 0x0);
	  status = BSP_PMIC_WriteReg(board_regulators_table[VDDCPU].alt_control_reg2, 0x0);


    /* STANDBY_PWRCTRL_SEL[1:0] = 0 */
    status = BSP_PMIC_ReadReg(MAIN_CR, &data); /* read MAIN_CR to save data */
    data &= ~STANDBY_PWRCTRL_SEL_3; /* clear STANDBY_PWRCTRL_SEL bits */
    status = BSP_PMIC_WriteReg(MAIN_CR, data); /* write MAIN_CR to set STANDBY_PWRCTRL_SEL[1:0] = 0 */
break;

case STPMIC2L_LPLV_STOP1:

	  /* VDDA1V8_AON: LDO1, PWRCTRL_SEL=0, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
	  PWRCTRL_RST=0, Msk_Rst=1, Source=NA, V=NA, State=ON */
	  status = BSP_PMIC_WriteReg(board_regulators_table[VDDA1V8_AON].control_reg1, MAIN_CR_EN);

	  /* VDDIO: LDO2, PWRCTRL_SEL=0, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
	  PWRCTRL_RST=0, Msk_Rst=1, Source=NA, V=3.3, State=ON */
	  status = BSP_PMIC_WriteReg(board_regulators_table[VDDIO].control_reg1, \
	                             (0x18 << 1) | MAIN_CR_EN);

	  /* VDD1_DDR: LDO3, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
	  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=1.8, State=ON */
	  status = BSP_PMIC_WriteReg(board_regulators_table[VDD1_DDR].alt_control_reg1, \
	                             (0x9 << 1) | MAIN_CR_EN);

	  /* VDD3V3_USB: LDO4, PWRCTRL_SEL=0, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
	  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=ON */
	  status = BSP_PMIC_WriteReg(board_regulators_table[VDD3V3_USB].control_reg1, MAIN_CR_EN);

	  /* VDD_FLASH: LDO5, PWRCTRL_SEL=3, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
	  PWRCTRL_RST=1, Msk_Rst=0, Source=NA, V=3.3, State=ON */
	  status = BSP_PMIC_WriteReg(board_regulators_table[VDD_FLASH].control_reg1, \
	                             (0x18 << 1) |  MAIN_CR_EN);


	  /* VDDA_1V8: LDO6, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
	  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=1.8, State=ON */
	  status = BSP_PMIC_WriteReg(board_regulators_table[VDDA_1V8].alt_control_reg1, \
	                             (0x9 << 1) |  MAIN_CR_EN);

	  /* LDO7_FREE: LDO7, PWRCTRL_SEL=0, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
	  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=OFF */
	  status = BSP_PMIC_WriteReg(board_regulators_table[LDO7_FREE].control_reg1, 0x0);

	  /* GPO1_3V3: GPO1, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
	  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=ON */
	  status = BSP_PMIC_WriteReg(board_regulators_table[GPO1_3V3].alt_control_reg1, MAIN_CR_EN);

	  /* GPO2_FREE: GPO2, PWRCTRL_SEL=0, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
	  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=OFF */
	  status = BSP_PMIC_WriteReg(board_regulators_table[GPO2_FREE].control_reg1, 0x0);

	  /* GPO3_LED5: GPO3, PWRCTRL_SEL=0, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
	  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=ON */
	  status = BSP_PMIC_WriteReg(board_regulators_table[GPO3_LED5].control_reg1, MAIN_CR_EN);

	  /* GPO4_FREE: GPO4, PWRCTRL_SEL=0, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
	  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=OFF */
	  status = BSP_PMIC_WriteReg(board_regulators_table[GPO4_FREE].control_reg1, 0x0);

	  /* GPO5_3V3_BKP: GPO5, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
	  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=ON */
	  status = BSP_PMIC_WriteReg(board_regulators_table[GPO5_3V3_BKP].alt_control_reg1, MAIN_CR_EN);

	  /* VDDCORE: BUCK1, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
	  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=0.67, State=ON_AUTO */
	  status = BSP_PMIC_WriteReg(board_regulators_table[VDDCORE].alt_control_reg1, 0x11);
	  status = BSP_PMIC_WriteReg(board_regulators_table[VDDCORE].alt_control_reg2, \
	                             MAIN_CR_PREG_AUTO_MODE | MAIN_CR_EN);

	  /* VDD2_DDR: BUCK2, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
	  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=1.1, State=ON_AUTO */
	  status = BSP_PMIC_WriteReg(board_regulators_table[VDD2_DDR].alt_control_reg1, 0x3C);
	  status = BSP_PMIC_WriteReg(board_regulators_table[VDD2_DDR].alt_control_reg2, \
	                             MAIN_CR_PREG_AUTO_MODE | MAIN_CR_EN);

	  /* VDDCPU: BUCK3, PWRCTRL_SEL=2, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
	  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=0.8, State=ON_AUTO */
	  status = BSP_PMIC_WriteReg(board_regulators_table[VDDCPU].control_reg1, 0x1E);
	  status = BSP_PMIC_WriteReg(board_regulators_table[VDDCPU].control_reg2, \
	                             MAIN_CR_PREG_AUTO_MODE | MAIN_CR_EN);

 /* STANDBY_PWRCTRL_SEL[1:0] = 0 */
 status = BSP_PMIC_ReadReg(MAIN_CR, &data); /* read MAIN_CR to save data */
 data &= ~STANDBY_PWRCTRL_SEL_3; /* clear STANDBY_PWRCTRL_SEL bits */
 status = BSP_PMIC_WriteReg(MAIN_CR, data); /* write MAIN_CR to set STANDBY_PWRCTRL_SEL[1:0] = 0 */
break;


case STPMIC2L_LPLV_STOP2:

	  /* VDDA1V8_AON: LDO1, PWRCTRL_SEL=0, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
	  PWRCTRL_RST=0, Msk_Rst=1, Source=NA, V=NA, State=ON */
	  status = BSP_PMIC_WriteReg(board_regulators_table[VDDA1V8_AON].control_reg1, MAIN_CR_EN);

	  /* VDDIO: LDO2, PWRCTRL_SEL=0, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
	  PWRCTRL_RST=0, Msk_Rst=1, Source=NA, V=3.3, State=ON */
	  status = BSP_PMIC_WriteReg(board_regulators_table[VDDIO].control_reg1, \
	                             (0x18 << 1) | MAIN_CR_EN);

	  /* VDD1_DDR: LDO3, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
	  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=1.8, State=ON */
	  status = BSP_PMIC_WriteReg(board_regulators_table[VDD1_DDR].alt_control_reg1, \
	                             (0x9 << 1) | MAIN_CR_EN);

	  /* VDD3V3_USB: LDO4, PWRCTRL_SEL=0, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
	  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=ON */
	  status = BSP_PMIC_WriteReg(board_regulators_table[VDD3V3_USB].control_reg1, MAIN_CR_EN);

	  /* VDD_FLASH: LDO5, PWRCTRL_SEL=3, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
	  PWRCTRL_RST=1, Msk_Rst=0, Source=NA, V=NA, State=OFF */
	  status = BSP_PMIC_WriteReg(board_regulators_table[VDD_FLASH].control_reg1,0x0);

	  /* VDDA_1V8: LDO6, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
	  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=1.8, State=ON */
	  status = BSP_PMIC_WriteReg(board_regulators_table[VDDA_1V8].alt_control_reg1, \
	                             (0x9 << 1) |  MAIN_CR_EN);

	  /* LDO7_FREE: LDO7, PWRCTRL_SEL=0, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
	  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=OFF */
	  status = BSP_PMIC_WriteReg(board_regulators_table[LDO7_FREE].control_reg1, 0x0);

	  /* GPO1_3V3: GPO1, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
	  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=ON */
	  status = BSP_PMIC_WriteReg(board_regulators_table[GPO1_3V3].control_reg1, MAIN_CR_EN);

	  /* GPO2_FREE: GPO2, PWRCTRL_SEL=0, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
	  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=OFF */
	  status = BSP_PMIC_WriteReg(board_regulators_table[GPO2_FREE].control_reg1, 0x0);

	  /* GPO3_LED5: GPO3, PWRCTRL_SEL=0, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
	  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=ON */
	  status = BSP_PMIC_WriteReg(board_regulators_table[GPO3_LED5].control_reg1, MAIN_CR_EN);

	  /* GPO4_FREE: GPO4, PWRCTRL_SEL=0, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
	  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=OFF */
	  status = BSP_PMIC_WriteReg(board_regulators_table[GPO4_FREE].control_reg1, 0x0);

	  /* GPO5_3V3_BKP: GPO5, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
	  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=ON */
	  status = BSP_PMIC_WriteReg(board_regulators_table[GPO5_3V3_BKP].alt_control_reg1, MAIN_CR_EN);

	  /* VDDCORE: BUCK1, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
	  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=0.67, State=ON_AUTO */
	  status = BSP_PMIC_WriteReg(board_regulators_table[VDDCORE].alt_control_reg1, 0x11);
	  status = BSP_PMIC_WriteReg(board_regulators_table[VDDCORE].alt_control_reg2, \
	                             MAIN_CR_PREG_AUTO_MODE | MAIN_CR_EN);

	  /* VDD2_DDR: BUCK2, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
	  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=1.1, State=ON_AUTO */
	  status = BSP_PMIC_WriteReg(board_regulators_table[VDD2_DDR].alt_control_reg1, 0x3C);
	  status = BSP_PMIC_WriteReg(board_regulators_table[VDD2_DDR].alt_control_reg2, \
	                             MAIN_CR_PREG_AUTO_MODE | MAIN_CR_EN);

	  /* VDDCPU: BUCK3, PWRCTRL_SEL=2, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
	  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=OFF */
	  status = BSP_PMIC_WriteReg(board_regulators_table[VDDCPU].alt_control_reg1,0x0);
	  status = BSP_PMIC_WriteReg(board_regulators_table[VDDCPU].alt_control_reg2,0x0);

   /* STANDBY_PWRCTRL_SEL[1:0] = 0 */
   status = BSP_PMIC_ReadReg(MAIN_CR, &data); /* read MAIN_CR to save data */
   data &= ~STANDBY_PWRCTRL_SEL_3; /* clear STANDBY_PWRCTRL_SEL bits */
   status = BSP_PMIC_WriteReg(MAIN_CR, data); /* write MAIN_CR to set STANDBY_PWRCTRL_SEL[1:0] = 0 */
break;

    case STPMIC2L_STANDBY_DDR_SR:

  	  /* VDDA1V8_AON: LDO1, PWRCTRL_SEL=0, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
  	  PWRCTRL_RST=0, Msk_Rst=1, Source=NA, V=NA, State=ON */
  	  status = BSP_PMIC_WriteReg(board_regulators_table[VDDA1V8_AON].control_reg1, MAIN_CR_EN);

  	  /* VDDIO: LDO2, PWRCTRL_SEL=0, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
  	  PWRCTRL_RST=0, Msk_Rst=1, Source=NA, V=3.3, State=ON */
  	  status = BSP_PMIC_WriteReg(board_regulators_table[VDDIO].control_reg1, \
  	                             (0x18 << 1) | MAIN_CR_EN);

  	  /* VDD1_DDR: LDO3, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
  	  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=1.8, State=ON */
  	  status = BSP_PMIC_WriteReg(board_regulators_table[VDD1_DDR].alt_control_reg1, \
  	                             (0x9 << 1) | MAIN_CR_EN);

  	  /* VDD3V3_USB: LDO4, PWRCTRL_SEL=0, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
  	  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=OFF */
  	  status = BSP_PMIC_WriteReg(board_regulators_table[VDD3V3_USB].control_reg1, 0x0);

  	  /* VDD_FLASH: LDO5, PWRCTRL_SEL=3, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
  	  PWRCTRL_RST=1, Msk_Rst=0, Source=NA, V=NA, State=OFF */
  	  status = BSP_PMIC_WriteReg(board_regulators_table[VDD_FLASH].control_reg1, 0x0);


  	  /* VDDA_1V8: LDO6, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
  	  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=OFF */
  	  status = BSP_PMIC_WriteReg(board_regulators_table[VDDA_1V8].alt_control_reg1, 0x0);

  	  /* LDO7_FREE: LDO7, PWRCTRL_SEL=0, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
  	  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=)FF */
  	  status = BSP_PMIC_WriteReg(board_regulators_table[LDO7_FREE].control_reg1, 0x0);

  	  /* GPO1_3V3: GPO1, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
  	  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=OFF */
  	  status = BSP_PMIC_WriteReg(board_regulators_table[GPO1_3V3].alt_control_reg1, 0x0);

  	  /* GPO2_FREE: GPO2, PWRCTRL_SEL=0, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
  	  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=OFF */
  	  status = BSP_PMIC_WriteReg(board_regulators_table[GPO2_FREE].control_reg1, 0x0);

  	  /* GPO3_LED5: GPO3, PWRCTRL_SEL=0, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
  	  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=ON */
  	  status = BSP_PMIC_WriteReg(board_regulators_table[GPO3_LED5].control_reg1, MAIN_CR_EN);

  	  /* GPO4_FREE: GPO4, PWRCTRL_SEL=0, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
  	  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=OFF */
  	  status = BSP_PMIC_WriteReg(board_regulators_table[GPO4_FREE].control_reg1, 0x0);

  	  /* GPO5_3V3_BKP: GPO5, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
  	  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=OFF */
  	  status = BSP_PMIC_WriteReg(board_regulators_table[GPO5_3V3_BKP].alt_control_reg1, 0x00);

  	  /* VDDCORE: BUCK1, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
  	  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=OFF */
  	  status = BSP_PMIC_WriteReg(board_regulators_table[VDDCORE].alt_control_reg1, 0x0);
  	  status = BSP_PMIC_WriteReg(board_regulators_table[VDDCORE].alt_control_reg2, 0x0);

  	  /* VDD2_DDR: BUCK2, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
  	  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=1.1, State=ON_AUTO */
  	  status = BSP_PMIC_WriteReg(board_regulators_table[VDD2_DDR].alt_control_reg1, 0x3C);
  	  status = BSP_PMIC_WriteReg(board_regulators_table[VDD2_DDR].alt_control_reg2, \
  	                             MAIN_CR_PREG_AUTO_MODE | MAIN_CR_EN);

  	  /* VDDCPU: BUCK3, PWRCTRL_SEL=2, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
  	  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=OFF */
  	  status = BSP_PMIC_WriteReg(board_regulators_table[VDDCPU].alt_control_reg1, 0x0);
  	  status = BSP_PMIC_WriteReg(board_regulators_table[VDDCPU].alt_control_reg2, 0x0);

     /* STANDBY_PWRCTRL_SEL[1:0] = 0 */
     status = BSP_PMIC_ReadReg(MAIN_CR, &data); /* read MAIN_CR to save data */
     data &= ~STANDBY_PWRCTRL_SEL_3; /* clear STANDBY_PWRCTRL_SEL bits */
     status = BSP_PMIC_WriteReg(MAIN_CR, data); /* write MAIN_CR to set STANDBY_PWRCTRL_SEL[1:0] = 0 */


      break;

    case STPMIC2L_STANDBY_DDR_OFF:

    	  /* VDDA1V8_AON: LDO1, PWRCTRL_SEL=0, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
    	  PWRCTRL_RST=0, Msk_Rst=1, Source=NA, V=NA, State=ON */
    	  status = BSP_PMIC_WriteReg(board_regulators_table[VDDA1V8_AON].control_reg1, MAIN_CR_EN);

    	  /* VDDIO: LDO2, PWRCTRL_SEL=0, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
    	  PWRCTRL_RST=0, Msk_Rst=1, Source=NA, V=3.3, State=ON */
    	  status = BSP_PMIC_WriteReg(board_regulators_table[VDDIO].control_reg1, \
    	                             (0x18 << 1) | MAIN_CR_EN);

    	  /* VDD1_DDR: LDO3, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
    	  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=OFF */
    	  status = BSP_PMIC_WriteReg(board_regulators_table[VDD1_DDR].alt_control_reg1,0x0);

    	  /* VDD3V3_USB: LDO4, PWRCTRL_SEL=0, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
    	  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=OFF */
    	  status = BSP_PMIC_WriteReg(board_regulators_table[VDD3V3_USB].control_reg1, 0x0);

    	  /* VDD_FLASH: LDO5, PWRCTRL_SEL=3, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
    	  PWRCTRL_RST=1, Msk_Rst=0, Source=NA, V=NA, State=OFF */
    	  status = BSP_PMIC_WriteReg(board_regulators_table[VDD_FLASH].control_reg1, 0x0);

    	  /* VDDA_1V8: LDO6, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
    	  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=OFF */
    	  status = BSP_PMIC_WriteReg(board_regulators_table[VDDA_1V8].alt_control_reg1, 0x0);

    	  /* LDO7_FREE: LDO7, PWRCTRL_SEL=0, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
    	  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=OFF */
    	  status = BSP_PMIC_WriteReg(board_regulators_table[LDO7_FREE].control_reg1, 0x0);

    	  /* GPO1_3V3: GPO1, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
    	  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=OFF */
    	  status = BSP_PMIC_WriteReg(board_regulators_table[GPO1_3V3].alt_control_reg1, 0x0);

    	  /* GPO2_FREE: GPO2, PWRCTRL_SEL=0, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
    	  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=OFF */
    	  status = BSP_PMIC_WriteReg(board_regulators_table[GPO2_FREE].control_reg1, 0x0);

    	  /* GPO3_LED5: GPO3, PWRCTRL_SEL=0, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
    	  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=ON */
    	  status = BSP_PMIC_WriteReg(board_regulators_table[GPO3_LED5].control_reg1, MAIN_CR_EN);

    	  /* GPO4_FREE: GPO4, PWRCTRL_SEL=0, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=0,
    	  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=OFF */
    	  status = BSP_PMIC_WriteReg(board_regulators_table[GPO4_FREE].control_reg1, 0x0);

    	  /* GPO5_3V3_BKP: GPO5, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
    	  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=OFF */
    	  status = BSP_PMIC_WriteReg(board_regulators_table[GPO5_3V3_BKP].alt_control_reg1, 0x00);

    	  /* VDDCORE: BUCK1, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
    	  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=OFF */
    	  status = BSP_PMIC_WriteReg(board_regulators_table[VDDCORE].alt_control_reg1, 0x0);
    	  status = BSP_PMIC_WriteReg(board_regulators_table[VDDCORE].alt_control_reg2, 0x0);

    	  /* VDD2_DDR: BUCK2, PWRCTRL_SEL=1, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
    	  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=OFF */
    	  status = BSP_PMIC_WriteReg(board_regulators_table[VDD2_DDR].alt_control_reg1, 0x0);
    	  status = BSP_PMIC_WriteReg(board_regulators_table[VDD2_DDR].alt_control_reg2, 0x0);

    	  /* VDDCPU: BUCK3, PWRCTRL_SEL=2, PWRCTRL_DLY_H=0, PWRCTRL_DLY_L=0, PWRCTRL_EN=1,
    	  PWRCTRL_RST=0, Msk_Rst=0, Source=NA, V=NA, State=OFF */
    	  status = BSP_PMIC_WriteReg(board_regulators_table[VDDCPU].alt_control_reg1, 0x0);
    	  status = BSP_PMIC_WriteReg(board_regulators_table[VDDCPU].alt_control_reg2, 0x0);

     /* STANDBY_PWRCTRL_SEL[1:0] = 0 */
      status = BSP_PMIC_ReadReg(MAIN_CR, &data); /* read MAIN_CR to save data */
      data &= ~STANDBY_PWRCTRL_SEL_3; /* clear STANDBY_PWRCTRL_SEL bits */
      status = BSP_PMIC_WriteReg(MAIN_CR, data| \
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
    case STPMIC2L_IT_PONKEY_F:
      printf("STPMIC2L_IT_PONKEY_F");
      break;
    case STPMIC2L_IT_PONKEY_R:
      printf("STPMIC2L_IT_PONKEY_R");
      break;
    case STPMIC2L_IT_VINLOW_FA:
      printf("STPMIC2L_IT_VINLOW_FA");
      break;
    case STPMIC2L_IT_VINLOW_RI:
      printf("STPMIC2L_IT_VINLOW_RI");
      break;
    case STPMIC2L_IT_THW_FA:
      printf("STPMIC2L_IT_THW_FA");
      break;
    case STPMIC2L_IT_THW_RI:
      printf("STPMIC2L_IT_THW_RI");
      break;
    case STPMIC2L_IT_BUCK1_OCP:
      printf("STPMIC2L_IT_BUCK1_OCP");
      break;
    case STPMIC2L_IT_BUCK2_OCP:
      printf("STPMIC2L_IT_BUCK2_OCP");
      break;
    case STPMIC2L_IT_BUCK3_OCP:
      printf("STPMIC2L_IT_BUCK2_OCP");
      break;
    case STPMIC2L_IT_LDO1_OCP:
      printf("STPMIC2L_IT_LDO1_OCP");
      break;
    case STPMIC2L_IT_LDO2_OCP:
      printf("STPMIC2L_IT_LDO2_OCP");
      break;
    case STPMIC2L_IT_LDO3_OCP:
      printf("STPMIC2L_IT_LDO3_OCP");
      break;
    case STPMIC2L_IT_LDO4_OCP:
      printf("STPMIC2L_IT_LDO4_OCP");
      break;
    case STPMIC2L_IT_LDO5_OCP:
      printf("STPMIC2L_IT_LDO5_OCP");
      break;
    case STPMIC2L_IT_LDO6_OCP:
      printf("STPMIC2L_IT_LDO6_OCP");
      break;
    case STPMIC2L_IT_LDO7_OCP:
      printf("STPMIC2L_IT_LDO7_OCP");
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
