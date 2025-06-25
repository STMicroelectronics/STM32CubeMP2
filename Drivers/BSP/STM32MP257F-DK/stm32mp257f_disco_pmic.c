/**
  ******************************************************************************
  * @file    stm32mp257f_disco_stpmic.c
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
#include "stm32mp257f_disco.h"
#include "stm32mp257f_disco_bus.h"
#include "stm32mp257f_disco_pmic.h"
#include <string.h>
#include <stdio.h>

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32MP257F_DK
  * @{
  */

/** @addtogroup STM32MP257F_DK_STPMU
  * @{
  */


/** @defgroup STM32MP257F_DK_STPMU_Private_Constants Private Constants
  * @{
  */
/* Driver for PMIC ---------------------------------------------------------------*/

/* Board Configuration ------------------------------------------------------------*/
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

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* I2C handler declaration */
I2C_HandleTypeDef I2cHandle;
extern I2C_HandleTypeDef hI2c7;

static STPMIC2_Drv_t *Stpmic2Drv = NULL;
static void         *CompObj = NULL;


/*
  Table of DK board regulators
*/

board_regul_struct_t board_regulators_table[] =
{
  {
    "VDDCPU",      VDDCPU,      STPMIC_BUCK1,  BUCK1_MAIN_CR1,  BUCK1_MAIN_CR2,  BUCK1_ALT_CR1,  BUCK1_ALT_CR2,
    BUCK1_PWRCTRL_CR,  800,  910
  },
  {
    "VDDCORE",     VDDCORE,     STPMIC_BUCK2,  BUCK2_MAIN_CR1,  BUCK2_MAIN_CR2,  BUCK2_ALT_CR1,  BUCK2_ALT_CR2,
    BUCK2_PWRCTRL_CR,  820,  820
  },
  {
    "VDDGPU",      VDDGPU,      STPMIC_BUCK3,  BUCK3_MAIN_CR1,  BUCK3_MAIN_CR2,  BUCK3_ALT_CR1,  BUCK3_ALT_CR2,
    BUCK3_PWRCTRL_CR,  800,  900
  },
  {
    "VDDIO_PMIC",  VDDIO_PMIC,  STPMIC_BUCK4,  BUCK4_MAIN_CR1,  BUCK4_MAIN_CR2,  BUCK4_ALT_CR1,  BUCK4_ALT_CR2,
    BUCK4_PWRCTRL_CR,  3300, 3300
  },
  {
    "VDDA1V8_AON", VDDA1V8_AON, STPMIC_LDO1,   LDO1_MAIN_CR,    LDO1_MAIN_CR,    LDO1_ALT_CR,    LDO1_ALT_CR,
    LDO1_PWRCTRL_CR,   1800, 1800
  },
  {
    "VDD_EMMC",    VDD_EMMC,    STPMIC_LDO2,   LDO2_MAIN_CR,    LDO2_MAIN_CR,    LDO2_ALT_CR,    LDO2_ALT_CR,
    LDO2_PWRCTRL_CR,   1800, 1800
  },
  {
    "V1V8",        V1V8,        STPMIC_BUCK5,  BUCK5_MAIN_CR1,  BUCK5_MAIN_CR2,  BUCK5_ALT_CR1,  BUCK5_ALT_CR2,
    BUCK5_PWRCTRL_CR,  1800, 1800
  },
  {
    "VDD3V3_USB",  VDD3V3_USB,  STPMIC_LDO4,   LDO4_MAIN_CR,    LDO4_MAIN_CR,    LDO4_ALT_CR,    LDO4_ALT_CR,
    LDO4_PWRCTRL_CR,   3300, 33
  },

  {
    "VDD1_DDR",    VDD1_DDR,    STPMIC_LDO3,   LDO3_MAIN_CR,    LDO3_MAIN_CR,    LDO3_ALT_CR,
    LDO3_ALT_CR, LDO3_PWRCTRL_CR,   0000, 0000
  },
  {
    "VDD2_DDR",    VDD2_DDR,    STPMIC_BUCK6,  BUCK6_MAIN_CR1,  BUCK6_MAIN_CR2,  BUCK6_ALT_CR1,
    BUCK6_ALT_CR2, BUCK6_PWRCTRL_CR,  1200, 1200
  },
  {
    "V_HDMI",      V_HDMI,      STPMIC_LDO5,   LDO5_MAIN_CR,    LDO5_MAIN_CR,    LDO5_ALT_CR,
    LDO5_ALT_CR, LDO5_PWRCTRL_CR,   2500, 2500
  },
  {
    "VDD_SDCARD",  VDD_SDCARD,  STPMIC_LDO7,   LDO7_MAIN_CR,    LDO7_MAIN_CR,    LDO7_ALT_CR,
    LDO7_ALT_CR, LDO7_PWRCTRL_CR,   1800, 3300
  },
  {
    "VDDIO_SDCARD", VDDIO_SDCARD, STPMIC_LDO8,   LDO8_MAIN_CR,    LDO8_MAIN_CR,    LDO8_ALT_CR,
    LDO8_ALT_CR, LDO8_PWRCTRL_CR,   3300, 3300
  },
  {
    "V3V3",        V3V3,        STPMIC_BUCK7,  BUCK7_MAIN_CR1,  BUCK7_MAIN_CR2,  BUCK7_ALT_CR1,
    BUCK7_ALT_CR2, BUCK7_PWRCTRL_CR,  3300, 3300
  },
  {
    "",            V_FREE,      STPMIC_LDO6,   LDO6_MAIN_CR,    LDO6_MAIN_CR,    LDO6_ALT_CR, LDO6_ALT_CR,
    LDO6_PWRCTRL_CR,   900,  900
  },
};


/**
  * @}
 */

/** @defgroup STM32MP257F_DK_STPMU_Private_Functions Private Functionss
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

#if defined(USE_WAKEUP_PIN)

#else
  GPIO_InitTypeDef  GPIO_InitStruct;

  /* INTn - Interrupt Line - Active Low (Falling Edge) */
  if (ResMgr_Request(RESMGR_RESOURCE_RIF_RCC, RESMGR_RCC_RESOURCE(90)) == RESMGR_STATUS_ACCESS_OK)
  {
    PMIC_INTN_CLK_ENABLE();
  }
  /* Acquire GPIOA0 using Resource manager */
  if (RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIF_GPIOA, RESMGR_GPIO_PIN(0)))
  {
    return BSP_ERROR_PERIPH_FAILURE;
  }

  GPIO_InitStruct.Pin       = PMIC_INTN_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = 0 ;
  HAL_GPIO_Init(PMIC_INTN_PORT, &GPIO_InitStruct);

  /* Enable and set INTn EXTI Interrupt  */
#if defined (CORE_CM33) || defined (CORE_CM0PLUS)
  HAL_NVIC_SetPriority(PMIC_INTN_EXTI_IRQ, 0, 0);
  HAL_NVIC_EnableIRQ(PMIC_INTN_EXTI_IRQ);
#endif /* CORE_CM33 || CORE_CM0PLUS */
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

#if defined(STPMIC2_DEBUG)
  status = Stpmic2Drv->DumpRegs(CompObj);
  if (status != STPMIC2_OK)
  {
    return BSP_ERROR_COMPONENT_FAILURE;
  }
#endif /* STPMIC2_DEBUG */

  return status;
}

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
  /* VDD2_DDR ==> BUCK6 ==> 1100mV */
  if (BSP_PMIC_WriteReg(board_regulators_table[VDD2_DDR].control_reg1, 0x3C) != BSP_ERROR_NONE)
  {
    return BSP_ERROR_PMIC;
  }

  /* VDD1_DDR ==> LDO3 ==> 1800mV */
  if (BSP_PMIC_UpdateReg(board_regulators_table[VDD1_DDR].control_reg1, (0x9 << 1)) != BSP_ERROR_NONE)
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
  uint8_t data;

  switch (regu)
  {
    case VDDA1V8_AON:
    case VDD_EMMC:
    case VDD1_DDR:
    case VDD3V3_USB:
    case VDD_SDCARD:
    case VDDIO_SDCARD:
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
    case VDD2_DDR:
    case V3V3:
      /* Forbid VDDGPU to be set on if VDDCORE is not enabled */
      if (regu == VDDGPU)
      {
        /* read control reg */
        if (BSP_PMIC_ReadReg(board_regulators_table[VDDCORE].control_reg2, &data) != BSP_ERROR_NONE)
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
    case VDD1_DDR:
    case VDD3V3_USB:
    case VDD_SDCARD:
    case VDDIO_SDCARD:
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
    case VDD2_DDR:
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

  /* disable vdd2_ddr */
  status = BSP_PMIC_REGU_Set_Off(VDD2_DDR);
  if (status != BSP_ERROR_NONE)
  {
    return status;
  }

  /* disable vdd1_ddr */
  return BSP_PMIC_REGU_Set_Off(VDD1_DDR);
}

/**
  * @brief  BSP_PMIC_Set_Power_Mode Set PMIC run/low power mode
  * @param  mode  mode to set
  * @retval status
  */
uint32_t BSP_PMIC_Power_Mode_Init()
{
  uint32_t  status = BSP_ERROR_NONE;
  return status;
}
uint32_t BSP_PMIC_Set_Power_Mode(uint32_t mode)
{
  uint32_t  status = BSP_ERROR_NONE;
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
void BSP_PMIC_INTN_IRQHANDLER(void)
{
  /*printf("%s:%d\n\r", __FUNCTION__, __LINE__);*/
  /* Call HAL EXTI IRQ Handler to clear appropriate flags */
  HAL_GPIO_EXTI_IRQHandler(PMIC_INTN_PIN);

  STPMIC_IrqHandler();
}
#endif /* SBA test */

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
