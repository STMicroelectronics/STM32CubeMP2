/**
  ******************************************************************************
  * @file    stpmic2.c
  * @author  MCD Application Team
  * @brief   This file provides the STPMIC2 PMU driver
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stpmic2.h"
#if defined (STPMIC2_DEBUG)
#include <stdio.h>
#endif /* STPMIC2_DEBUG */

/** @addtogroup BSP
  * @{
  */

/** @addtogroup Components
  * @{
  */

/** @addtogroup STPMIC2
  * @brief     This file provides a set of functions needed to drive the
  *            STPMIC2 PMU module.
  * @{
  */

/** @defgroup STPMIC2_Private_TypesDefinitions
  * @{
  */
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

#define I2C_TIMEOUT_MS      25

#define VOLTAGE_INDEX_INVALID   ((size_t)~0U)

struct regul_struct
{
  const char *name;
  const uint16_t *volt_table;
  uint8_t volt_table_size;
  uint8_t volt_cr;
  uint8_t volt_shift;
  uint8_t en_cr;
  uint8_t alt_en_cr;
  uint8_t alt_volt_cr;
  uint8_t pwrctrl_cr;
  uint8_t msrt_reg;
  uint8_t msrt_mask;
  uint8_t pd_reg;
  uint8_t pd_val;
  uint8_t pd_mask;
  uint8_t ocp_reg;
  uint8_t ocp_mask;
};

/**
  * @}
  */
/** @defgroup STPMIC2_Private_Variables
  * @{
  */

STPMIC2_Drv_t   STPMIC2_Driver =
{
  STPMIC2_Init,
  STPMIC2_DeInit,
  STPMIC2_ReadID,
  STPMIC2_ReadReg,
  STPMIC2_WriteReg,
  STPMIC2_UpdateReg,
#if defined (STPMIC2_DEBUG)
  STPMIC2_DumpRegs
#endif /* STPMIC2_DEBUG */
};

#if defined (STPMIC2_DEBUG)
STPMIC2_register_t STPMIC2_registers_table[] =
{
  /* Status Registers */
  {PRODUCT_ID,            "PRODUCT_ID          "},
  {VERSION_SR,            "VERSION_SR          "},
  {TURN_ON_SR,            "TURN_ON_SR          "},
  {TURN_OFF_SR,           "TURN_OFF_SR         "},
  {RESTART_SR,            "RESTART_SR          "},
  {OCP_SR1,               "OCP_SR1             "},
  {OCP_SR2,               "OCP_SR2             "},
  {EN_SR1,                "EN_SR1              "},
  {EN_SR2,                "EN_SR2              "},
  {FS_CNT_SR1,            "FS_CNT_SR1          "},
  {FS_CNT_SR2,            "FS_CNT_SR2          "},
  {FS_CNT_SR3,            "FS_CNT_SR3          "},
  {MODE_SR,               "MODE_SR             "},
  /* Control Registers */
  {MAIN_CR,               "MAIN_CR             "},
  {VINLOW_CR,             "VINLOW_CR           "},
  {PKEY_LKP_CR,           "PKEY_LKP_CR         "},
  {WDG_CR,                "WDG_CR              "},
  {WDG_TMR_CR,            "WDG_TMR_CR          "},
  {WDG_TMR_SR,            "WDG_TMR_SR          "},
  {FS_OCP_CR1,            "FS_OCP_CR1          "},
  {FS_OCP_CR2,            "FS_OCP_CR2          "},
  {PADS_PULL_CR,          "PADS_PULL_CR        "},
  {BUCKS_PD_CR1,          "BUCKS_PD_CR1        "},
  {BUCKS_PD_CR2,          "BUCKS_PD_CR2        "},
  {LDOS_PD_CR1,           "LDOS_PD_CR1         "},
  {LDOS_PD_CR2,           "LDOS_PD_CR2         "},
  {BUCKS_MRST_CR,         "BUCKS_MRST_CR       "},
  {LDOS_MRST_CR,          "LDOS_MRST_CR        "},
  /* Buck CR */
  {BUCK1_MAIN_CR1,        "BUCK1_MAIN_CR1      "},
  {BUCK1_MAIN_CR2,        "BUCK1_MAIN_CR2      "},
  {BUCK1_ALT_CR1,         "BUCK1_ALT_CR1       "},
  {BUCK1_ALT_CR2,         "BUCK1_ALT_CR2       "},
  {BUCK1_PWRCTRL_CR,      "BUCK1_PWRCTRL_CR    "},
  {BUCK2_MAIN_CR1,        "BUCK2_MAIN_CR1      "},
  {BUCK2_MAIN_CR2,        "BUCK2_MAIN_CR2      "},
  {BUCK2_ALT_CR1,         "BUCK2_ALT_CR1       "},
  {BUCK2_ALT_CR2,         "BUCK2_ALT_CR2       "},
  {BUCK2_PWRCTRL_CR,      "BUCK2_PWRCTRL_CR    "},
  {BUCK3_MAIN_CR1,        "BUCK3_MAIN_CR1      "},
  {BUCK3_MAIN_CR2,        "BUCK3_MAIN_CR2      "},
  {BUCK3_ALT_CR1,         "BUCK3_ALT_CR1       "},
  {BUCK3_ALT_CR2,         "BUCK3_ALT_CR2       "},
  {BUCK3_PWRCTRL_CR,      "BUCK3_PWRCTRL_CR    "},
  {BUCK4_MAIN_CR1,        "BUCK4_MAIN_CR1      "},
  {BUCK4_MAIN_CR2,        "BUCK4_MAIN_CR2      "},
  {BUCK4_ALT_CR1,         "BUCK4_ALT_CR1       "},
  {BUCK4_ALT_CR2,         "BUCK4_ALT_CR2       "},
  {BUCK4_PWRCTRL_CR,      "BUCK4_PWRCTRL_CR    "},
  {BUCK5_MAIN_CR1,        "BUCK5_MAIN_CR1      "},
  {BUCK5_MAIN_CR2,        "BUCK5_MAIN_CR2      "},
  {BUCK5_ALT_CR1,         "BUCK5_ALT_CR1       "},
  {BUCK5_ALT_CR2,         "BUCK5_ALT_CR2       "},
  {BUCK5_PWRCTRL_CR,      "BUCK5_PWRCTRL_CR    "},
  {BUCK6_MAIN_CR1,        "BUCK6_MAIN_CR1      "},
  {BUCK6_MAIN_CR2,        "BUCK6_MAIN_CR2      "},
  {BUCK6_ALT_CR1,         "BUCK6_ALT_CR1       "},
  {BUCK6_ALT_CR2,         "BUCK6_ALT_CR2       "},
  {BUCK6_PWRCTRL_CR,      "BUCK6_PWRCTRL_CR    "},
  {BUCK7_MAIN_CR1,        "BUCK7_MAIN_CR1      "},
  {BUCK7_MAIN_CR2,        "BUCK7_MAIN_CR2      "},
  {BUCK7_ALT_CR1,         "BUCK7_ALT_CR1       "},
  {BUCK7_ALT_CR2,         "BUCK7_ALT_CR2       "},
  {BUCK7_PWRCTRL_CR,      "BUCK7_PWRCTRL_CR    "},
  /* LDO CR */
  {LDO1_MAIN_CR,          "LDO1_MAIN_CR        "},
  {LDO1_ALT_CR,           "LDO1_ALT_CR         "},
  {LDO1_PWRCTRL_CR,       "LDO1_PWRCTRL_CR     "},
  {LDO2_MAIN_CR,          "LDO2_MAIN_CR        "},
  {LDO2_ALT_CR,           "LDO2_ALT_CR         "},
  {LDO2_PWRCTRL_CR,       "LDO2_PWRCTRL_CR     "},
  {LDO3_MAIN_CR,          "LDO3_MAIN_CR        "},
  {LDO3_ALT_CR,           "LDO3_ALT_CR         "},
  {LDO3_PWRCTRL_CR,       "LDO3_PWRCTRL_CR     "},
  {LDO4_MAIN_CR,          "LDO4_MAIN_CR        "},
  {LDO4_ALT_CR,           "LDO4_ALT_CR         "},
  {LDO4_PWRCTRL_CR,       "LDO4_PWRCTRL_CR     "},
  {LDO5_MAIN_CR,          "LDO5_MAIN_CR        "},
  {LDO5_ALT_CR,           "LDO5_ALT_CR         "},
  {LDO5_PWRCTRL_CR,       "LDO5_PWRCTRL_CR     "},
  {LDO6_MAIN_CR,          "LDO6_MAIN_CR        "},
  {LDO6_ALT_CR,           "LDO6_ALT_CR         "},
  {LDO6_PWRCTRL_CR,       "LDO6_PWRCTRL_CR     "},
  {LDO7_MAIN_CR,          "LDO7_MAIN_CR        "},
  {LDO7_ALT_CR,           "LDO7_ALT_CR         "},
  {LDO7_PWRCTRL_CR,       "LDO7_PWRCTRL_CR     "},
  {LDO8_MAIN_CR,          "LDO8_MAIN_CR        "},
  {LDO8_ALT_CR,           "LDO8_ALT_CR         "},
  {LDO8_PWRCTRL_CR,       "LDO8_PWRCTRL_CR     "},
  {REFDDR_MAIN_CR,        "REFDDR_MAIN_CR      "},
  {REFDDR_ALT_CR,         "REFDDR_ALT_CR       "},
  {REFDDR_PWRCTRL_CR,     "REFDDR_PWRCTRL_CR   "},
  /* INTERRUPT CR  */
  {INT_PENDING_R1,        "INT_PENDING_R1      "},
  {INT_PENDING_R2,        "INT_PENDING_R2      "},
  {INT_PENDING_R3,        "INT_PENDING_R3      "},
  {INT_PENDING_R4,        "INT_PENDING_R4      "},
  {INT_CLEAR_R1,          "INT_CLEAR_R1        "},
  {INT_CLEAR_R2,          "INT_CLEAR_R2        "},
  {INT_CLEAR_R3,          "INT_CLEAR_R3        "},
  {INT_CLEAR_R4,          "INT_CLEAR_R4        "},
  {INT_MASK_R1,           "INT_MASK_R1         "},
  {INT_MASK_R2,           "INT_MASK_R2         "},
  {INT_MASK_R3,           "INT_MASK_R3         "},
  {INT_MASK_R4,           "INT_MASK_R4         "},
  {INT_SRC_R1,            "INT_SRC_R1          "},
  {INT_SRC_R2,            "INT_SRC_R2          "},
  {INT_SRC_R3,            "INT_SRC_R3          "},
  {INT_SRC_R4,            "INT_SRC_R4          "},
  {INT_DBG_LATCH_R1,      "INT_DBG_LATCH_R1    "},
  {INT_DBG_LATCH_R2,      "INT_DBG_LATCH_R2    "},
  {INT_DBG_LATCH_R3,      "INT_DBG_LATCH_R3    "},
  {INT_DBG_LATCH_R4,      "INT_DBG_LATCH_R4    "},
  /* NVM registers */
  {NVM_SR,                "NVM_SR              "},
  {NVM_CR,                "NVM_CR              "},
  {NVM_MAIN_CTRL_SHR1,    "NVM_MAIN_CTRL_SHR1  "},
  {NVM_MAIN_CTRL_SHR2,    "NVM_MAIN_CTRL_SHR2  "},
  {NVM_RANK_SHR1,         "NVM_RANK_SHR1       "},
  {NVM_RANK_SHR2,         "NVM_RANK_SHR2       "},
  {NVM_RANK_SHR3,         "NVM_RANK_SHR3       "},
  {NVM_RANK_SHR4,         "NVM_RANK_SHR4       "},
  {NVM_RANK_SHR5,         "NVM_RANK_SHR5       "},
  {NVM_RANK_SHR6,         "NVM_RANK_SHR6       "},
  {NVM_RANK_SHR7,         "NVM_RANK_SHR7       "},
  {NVM_RANK_SHR8,         "NVM_RANK_SHR8       "},
  {NVM_BUCK_MODE_SHR1,    "NVM_BUCK_MODE_SHR1  "},
  {NVM_BUCK_MODE_SHR2,    "NVM_BUCK_MODE_SHR2  "},
  {NVM_BUCK1_VOUT_SHR,    "NVM_BUCK1_VOUT_SHR  "},
  {NVM_BUCK2_VOUT_SHR,    "NVM_BUCK2_VOUT_SHR  "},
  {NVM_BUCK3_VOUT_SHR,    "NVM_BUCK3_VOUT_SHR  "},
  {NVM_BUCK4_VOUT_SHR,    "NVM_BUCK4_VOUT_SHR  "},
  {NVM_BUCK5_VOUT_SHR,    "NVM_BUCK5_VOUT_SHR  "},
  {NVM_BUCK6_VOUT_SHR,    "NVM_BUCK6_VOUT_SHR  "},
  {NVM_BUCK7_VOUT_SHR,    "NVM_BUCK7_VOUT_SHR  "},
  {NVM_LDO2_SHR,          "NVM_LDO2_SHR        "},
  {NVM_LDO3_SHR,          "NVM_LDO3_SHR        "},
  {NVM_LDO5_SHR,          "NVM_LDO5_SHR        "},
  {NVM_LDO6_SHR,          "NVM_LDO6_SHR        "},
  {NVM_LDO7_SHR,          "NVM_LDO7_SHR        "},
  {NVM_LDO8_SHR,          "NVM_LDO8_SHR        "},
  {NVM_PD_SHR1,           "NVM_PD_SHR1         "},
  {NVM_PD_SHR2,           "NVM_PD_SHR2         "},
  {NVM_PD_SHR,            "NVM_PD_SHR          "},
  {NVM_BUCKS_IOUT_SHR1,   "NVM_BUCKS_IOUT_SHR1 "},
  {NVM_BUCKS_IOUT_SHR2,   "NVM_BUCKS_IOUT_SHR2 "},
  {NVM_LDOS_IOUT_SHR,     "NVM_LDOS_IOUT_SHR   "},
  {NVM_FS_OCP_SHR1,       "NVM_FS_OCP_SHR1     "},
  {NVM_FS_OCP_SHR2,       "NVM_FS_OCP_SHR2     "},
  {NVM_FS_SHR1,           "NVM_FS_SHR1         "},
  {NVM_FS_SHR2,           "NVM_FS_SHR2         "},
  {NVM_FS_SHR3,           "NVM_FS_SHR3         "},
  {NVM_BUCK_AUTOCAL_SHR,  "NVM_BUCK_AUTOCAL_SHR"},
  {NVM_I2C_ADDR_SHR,      "NVM_I2C_ADDR_SHR    "},
  {NVM_USER_SHR1,         "NVM_USER_SHR1       "},
  {NVM_USER_SHR2,         "NVM_USER_SHR2       "},
  {NVM_BUCK_COMP_SHR1,    "NVM_BUCK_COMP_SHR1  "},
  {NVM_BUCK_COMP_SHR2,    "NVM_BUCK_COMP_SHR2  "}
};
#endif /* STPMIC2_DEBUG */

#define STPMIC2_REGISTERS_TABLE_SIZE  ARRAY_SIZE(STPMIC2_registers_table)

/**
  * @}
  */

/** @defgroup STPMIC2_Private_Functions_Prototypes Private Functions Prototypes
  * @{
  */
static int32_t STPMIC2_ReadRegWrap(void *handle, uint16_t Reg, uint8_t *Data, uint16_t Length);
static int32_t STPMIC2_WriteRegWrap(void *handle, uint16_t Reg, uint8_t *Data, uint16_t Length);

/**
  * @}
  */

/** @defgroup STPMIC2_Exported_Functions STPMIC2 Exported Functions
  * @{
  */
/**
  * @brief  Register component IO bus
  * @param  Component object pointer
  * @retval Component status
  */
int32_t STPMIC2_RegisterBusIO(STPMIC2_Object_t *pObj, STPMIC2_IO_t *pIO)
{
  int32_t ret;

  if (pObj == NULL)
  {
    ret = STPMIC2_ERROR;
  }
  else
  {
    pObj->IO.Init      = pIO->Init;
    pObj->IO.DeInit    = pIO->DeInit;
    pObj->IO.Address   = pIO->Address;
    pObj->IO.WriteReg  = pIO->WriteReg;
    pObj->IO.ReadReg   = pIO->ReadReg;
    pObj->IO.GetTick   = pIO->GetTick;

    pObj->Ctx.ReadReg  = STPMIC2_ReadRegWrap;
    pObj->Ctx.WriteReg = STPMIC2_WriteRegWrap;
    pObj->Ctx.handle   = pObj;

    if (pObj->IO.Init != NULL)
    {
      ret = pObj->IO.Init();
    }
    else
    {
      ret = STPMIC2_ERROR;
    }
  }

  return ret;
}

struct regval
{
  uint16_t addr;
  uint8_t val;
};

int32_t STPMIC2_Reset(STPMIC2_Object_t *pObj)
{
  int32_t ret = STPMIC2_OK;

  return ret;
}

/**
  * @brief  Initializes the STPMIC2 component.
  * @param  pObj  pointer to component object
  * @retval Component status
  */
int32_t STPMIC2_Init(STPMIC2_Object_t *pObj)
{
  int32_t ret = STPMIC2_OK;

  if ((pObj == NULL) || (pObj->IsInitialized == 1U))
  {
    ret = STPMIC2_ERROR;
  }
  else
  {
    if (STPMIC2_Reset(pObj) != STPMIC2_OK)
    {
      ret = STPMIC2_ERROR;
    }
    else
    {
      pObj->IsInitialized = 1U;
    }
  }

  return ret;
}

/**
  * @brief  De-initializes the STPMIC2 component.
  * @param  pObj  pointer to component object
  * @retval Component status
  */
int32_t STPMIC2_DeInit(STPMIC2_Object_t *pObj)
{
  if (pObj->IsInitialized == 1U)
  {
    /* De-initialize STPMIC interface */
    pObj->IsInitialized = 0U;
  }

  return STPMIC2_OK;
}


/**
  * @brief  Read the STPMIC2 identity.
  * @param  pObj  pointer to component object
  * @param  Id    pointer to component ID
  * @retval Component status
  */
int32_t STPMIC2_ReadID(STPMIC2_Object_t *pObj, uint8_t *pId)
{
  int32_t ret = STPMIC2_OK;

  /* Initialize I2C */
  pObj->IO.Init();

  if (stpmic2_read_reg(&pObj->Ctx, PRODUCT_ID, pId, 1) != STPMIC2_OK)
  {
    ret = STPMIC2_ERROR;
  }

  /* Component status */
  return ret;
}


/**
  * @brief  Read STPMIC2 register.
  * @param  pObj  pointer to component object
  * @param  reg   address of register to read
  * @param  pData    pointer to store read value
  * @retval Component status
  */
int32_t STPMIC2_ReadReg(STPMIC2_Object_t *pObj, uint8_t reg, uint8_t *pData)
{
  int32_t ret = STPMIC2_OK;

  /* Initialize I2C */
  pObj->IO.Init();

  if (stpmic2_read_reg(&pObj->Ctx, reg, pData, 1) != STPMIC2_OK)
  {
    ret = STPMIC2_ERROR;
  }

  /* Component status */
  return ret;
}


/**
  * @brief  Write STPMIC2 register.
  * @param  pObj  pointer to component object
  * @param  reg   address of register to write
  * @param  data  data value to write
  * @retval Component status
  */
int32_t STPMIC2_WriteReg(STPMIC2_Object_t *pObj, uint8_t reg, uint8_t data)
{
  int32_t ret = STPMIC2_OK;

  /* Initialize I2C */
  pObj->IO.Init();

  if (stpmic2_write_reg(&pObj->Ctx, reg, &data, 1) != STPMIC2_OK)
  {
    ret = STPMIC2_ERROR;
  }

  /* Component status */
  return ret;
}

/**
  * @brief  Update STPMIC2 register.
  * @param  pObj  pointer to component object
  * @param  reg   address of register to write
  * @param  mask  bit mask to update
  * @retval Component status
  */
int32_t STPMIC2_UpdateReg(STPMIC2_Object_t *pObj, uint8_t reg, uint8_t mask)
{
  int32_t ret = STPMIC2_OK;
  uint8_t tmp;

  /* Initialize I2C */
  pObj->IO.Init();

  if (stpmic2_read_reg(&pObj->Ctx, reg, &tmp, 1) != STPMIC2_OK)
  {
    ret = STPMIC2_ERROR;
  }

  tmp |= mask;

  if (stpmic2_write_reg(&pObj->Ctx, reg, &tmp, 1) != STPMIC2_OK)
  {
    ret = STPMIC2_ERROR;
  }

  /* Component status */
  return ret;
}

#if defined (STPMIC2_DEBUG)
int32_t STPMIC2_DumpRegs(STPMIC2_Object_t *pObj)
{
  int32_t  ret = STPMIC2_OK;
  uint8_t data;

  /*  */
  for (int32_t i = 0; i < STPMIC2_REGISTERS_TABLE_SIZE; i++)
  {
    if (STPMIC2_ReadReg(pObj, STPMIC2_registers_table[i].RegAddr, &data) == STPMIC2_OK)
    {
      printf("(offset 0x%02X) %s = 0x%02X\r\n", STPMIC2_registers_table[i].RegAddr,
             STPMIC2_registers_table[i].RegName, data);
    }
    else
    {
      printf("(offset 0x%02X)*** ERROR ***"
             " reading %s register = 0x%02X\r\n", STPMIC2_registers_table[i].RegAddr,
             STPMIC2_registers_table[i].RegName, data);
      ret = STPMIC2_ERROR;
      break;
    }
  }

  return ret ;
}
#endif /* STPMIC2_DEBUG */


/**
  * @}
  */

/** @defgroup STPMIC2_Private_Functions Private Functions
  * @{
  */

/**
  * @brief  Wrap component ReadReg to Bus Read function
  * @param  handle  Component object handle
  * @param  Reg  The target register address to write
  * @param  pData  The target register value to be written
  * @param  Length  buffer size to be written
  * @retval error status
  */
static int32_t STPMIC2_ReadRegWrap(void *handle, uint16_t Reg, uint8_t *pData, uint16_t Length)
{
  STPMIC2_Object_t *pObj = (STPMIC2_Object_t *)handle;

  return pObj->IO.ReadReg(pObj->IO.Address, Reg, pData, Length);
}

/**
  * @brief  Wrap component WriteReg to Bus Write function
  * @param  handle  Component object handle
  * @param  Reg  The target register address to write
  * @param  pData  The target register value to be written
  * @param  Length  buffer size to be written
  * @retval error status
  */
static int32_t STPMIC2_WriteRegWrap(void *handle, uint16_t Reg, uint8_t *pData, uint16_t Length)
{
  STPMIC2_Object_t *pObj = (STPMIC2_Object_t *)handle;

  return pObj->IO.WriteReg(pObj->IO.Address, Reg, pData, Length);
}

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
