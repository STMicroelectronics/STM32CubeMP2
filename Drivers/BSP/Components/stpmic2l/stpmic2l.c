/**
  ******************************************************************************
  * @file    stpmic2l.c
  * @author  MCD Application Team
  * @brief   This file provides the STPMIC2L PMU driver
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stpmic2l.h"
#if defined (STPMIC2L_DEBUG)
#include <stdio.h>
#endif /* STPMIC2L_DEBUG */

/** @addtogroup BSP
  * @{
  */

/** @addtogroup Components
  * @{
  */

/** @addtogroup STPMIC2L
  * @brief     This file provides a set of functions needed to drive the
  *            STPMIC2L PMU module.
  * @{
  */

/** @defgroup STPMIC2L_Private_TypesDefinitions
  * @{
  */
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

#define I2C_TIMEOUT_MS			25

#define VOLTAGE_INDEX_INVALID		((size_t)~0U)

struct regul_struct {
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
/** @defgroup STPMIC2L_Private_Variables
  * @{
  */

STPMIC2L_Drv_t   STPMIC2L_Driver =
{
  STPMIC2L_Init,
  STPMIC2L_DeInit,
  STPMIC2L_ReadID,
  STPMIC2L_ReadReg,
  STPMIC2L_WriteReg,
  STPMIC2L_UpdateReg,
#if defined (STPMIC2L_DEBUG)
  STPMIC2L_DumpRegs
#endif /* STPMIC2L_DEBUG */
};

#if defined (STPMIC2L_DEBUG)
STPMIC2L_register_t STPMIC2L_registers_table[] =
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
 {GPO_SR,                "GPO_SR              "},

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
 {BUCKS_PD_CR,           "BUCKS_PD_CR         "},
 {LDOS_PD_CR,             "LDOS_PD_CR         "},
 {GPO_MRST_CR,           "GPO_MRST_CR         "},
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
 {GPO1_MAIN_CR,          "GPO1_MAIN_CR        "},
 {GPO1_ALT_CR,           "GPO1_ALT_CR         "},
 {GPO1_PWRCTRL_CR,       "GPO1_PWRCTRL_CR     "},
 {GPO2_MAIN_CR,          "GPO2_MAIN_CR        "},
 {GPO2_ALT_CR,           "GPO2_ALT_CR         "},
 {GPO2_PWRCTRL_CR,       "GPO2_PWRCTRL_CR     "},
/* LDO CR */
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
 {NVM_FREE_SHR1,         "NVM_FREE_SHR1       "},    
 {NVM_FREE_SHR2,         "NVM_FREE_SHR2       "},    
 {NVM_RANK_SHR5,         "NVM_RANK_SHR5       "},    
 {NVM_RANK_SHR6,         "NVM_RANK_SHR6       "},    
 {NVM_RANK_SHR7,         "NVM_RANK_SHR7       "},    
 {NVM_RANK_SHR8,         "NVM_RANK_SHR8       "},    
 {NVM_BUCK_MODE_SHR1,    "NVM_BUCK_MODE_SHR1  "},    
 {NVM_FREE_SH3,          "NVM_FREE_SH3        "},    
 {NVM_BUCK1_VOUT_SHR,    "NVM_BUCK1_VOUT_SHR  "},    
 {NVM_BUCK2_VOUT_SHR,    "NVM_BUCK2_VOUT_SHR  "},    
 {NVM_MAIN_CTRL_SHR3,    "NVM_MAIN_CTRL_SHR3  "},    
 {NVM_RANK_SHR9,         "NVM_RANK_SHR9       "},    
 {NVM_LDO2_SHR,          "NVM_LDO2_SHR        "},    
 {NVM_LDO3_SHR,          "NVM_LDO3_SHR        "},    
 {NVM_LDO5_SHR,          "NVM_LDO5_SHR        "},    
 {NVM_FREE_SHR4,         "NVM_FREE_SHR4       "},    
 {NVM_PD_SHR1,           "NVM_PD_SHR1         "},    
 {NVM_FREE_SHR5,         "NVM_FREE_SHR5       "},    
 {NVM_PD_SHR3,           "NVM_PD_SHR3         "},    
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
 {NVM_MAIN_CTRL_SHR4,    "NVM_MAIN_CTRL_SHR4  "},    
};
#endif /* STPMIC2L_DEBUG */

#define STPMIC2L_REGISTERS_TABLE_SIZE  ARRAY_SIZE(STPMIC2L_registers_table)

/**
  * @}
  */

/** @defgroup STPMIC2L_Private_Functions_Prototypes Private Functions Prototypes
  * @{
  */
static int32_t STPMIC2L_ReadRegWrap(void *handle, uint16_t Reg, uint8_t* Data, uint16_t Length);
static int32_t STPMIC2L_WriteRegWrap(void *handle, uint16_t Reg, uint8_t* Data, uint16_t Length);

/**
  * @}
  */

/** @defgroup STPMIC2L_Exported_Functions STPMIC2L Exported Functions
  * @{
  */
/**
  * @brief  Register component IO bus
  * @param  Component object pointer
  * @retval Component status
  */
int32_t STPMIC2L_RegisterBusIO (STPMIC2L_Object_t *pObj, STPMIC2L_IO_t *pIO)
{
  int32_t ret;

  if (pObj == NULL)
  {
    ret = STPMIC2L_ERROR;
  }
  else
  {
    pObj->IO.Init      = pIO->Init;
    pObj->IO.DeInit    = pIO->DeInit;
    pObj->IO.Address   = pIO->Address;
    pObj->IO.WriteReg  = pIO->WriteReg;
    pObj->IO.ReadReg   = pIO->ReadReg;
    pObj->IO.GetTick   = pIO->GetTick;

    pObj->Ctx.ReadReg  = STPMIC2L_ReadRegWrap;
    pObj->Ctx.WriteReg = STPMIC2L_WriteRegWrap;
    pObj->Ctx.handle   = pObj;

    if(pObj->IO.Init != NULL)
    {
      ret = pObj->IO.Init();
    }
    else
    {
      ret = STPMIC2L_ERROR;
    }
  }

  return ret;
}

struct regval {
  uint16_t addr;
  uint8_t val;
};

int STPMIC2L_Reset(__attribute__((unused)) STPMIC2L_Object_t *pObj)
{
  int32_t ret = STPMIC2L_OK;

  return ret;
}

/**
  * @brief  Initializes the STPMIC2L component.
  * @param  pObj  pointer to component object
  * @retval Component status
  */
int32_t STPMIC2L_Init(STPMIC2L_Object_t *pObj)
{
  int32_t ret = STPMIC2L_OK;

  if ((pObj == NULL) || (pObj->IsInitialized == 1U))
  {
      ret = STPMIC2L_ERROR;
  }
  else
  {
    if (STPMIC2L_Reset(pObj) != STPMIC2L_OK)
    {
      ret = STPMIC2L_ERROR;
    }
    else
    {
      pObj->IsInitialized = 1U;
    }
  }

  return ret;
}

/**
  * @brief  De-initializes the STPMIC2L component.
  * @param  pObj  pointer to component object
  * @retval Component status
  */
int32_t STPMIC2L_DeInit(STPMIC2L_Object_t *pObj)
{
  if(pObj->IsInitialized == 1U)
  {
    /* De-initialize STPMIC interface */
    pObj->IsInitialized = 0U;
  }

  return STPMIC2L_OK;
}


/**
  * @brief  Read the STPMIC2L identity.
  * @param  pObj  pointer to component object
  * @param  Id    pointer to component ID
  * @retval Component status
  */
int32_t STPMIC2L_ReadID(STPMIC2L_Object_t *pObj, uint8_t *pId)
{
  int32_t ret=STPMIC2L_OK;

  /* Initialize I2C */
  pObj->IO.Init();

  if(stpmic2l_read_reg(&pObj->Ctx, PRODUCT_ID, pId, 1)!= STPMIC2L_OK)
  {
    ret = STPMIC2L_ERROR;
  }

  /* Component status */
  return ret;
}


/**
  * @brief  Read STPMIC2L register.
  * @param  pObj  pointer to component object
  * @param  reg   address of register to read
  * @param  pData    pointer to store read value
  * @retval Component status
  */
int32_t STPMIC2L_ReadReg(STPMIC2L_Object_t *pObj, uint8_t reg, uint8_t *pData)
{
  int32_t ret=STPMIC2L_OK;

  /* Initialize I2C */
  pObj->IO.Init();

  if(stpmic2l_read_reg(&pObj->Ctx, reg, pData, 1)!= STPMIC2L_OK)
  {
    ret = STPMIC2L_ERROR;
  }

  /* Component status */
  return ret;
}


/**
  * @brief  Write STPMIC2L register.
  * @param  pObj  pointer to component object
  * @param  reg   address of register to write
  * @param  data  data value to write
  * @retval Component status
  */
int32_t STPMIC2L_WriteReg(STPMIC2L_Object_t *pObj, uint8_t reg, uint8_t data)
{
  int32_t ret=STPMIC2L_OK;

  /* Initialize I2C */
  pObj->IO.Init();

  if(stpmic2l_write_reg(&pObj->Ctx, reg, &data, 1)!= STPMIC2L_OK)
  {
    ret = STPMIC2L_ERROR;
  }

  /* Component status */
  return ret;
}

/**
  * @brief  Update STPMIC2L register.
  * @param  pObj  pointer to component object
  * @param  reg   address of register to write
  * @param  mask  bit mask to update
  * @retval Component status
  */
int32_t STPMIC2L_UpdateReg(STPMIC2L_Object_t *pObj, uint8_t reg, uint8_t mask)
{
  int32_t ret=STPMIC2L_OK;
  uint8_t tmp;

  /* Initialize I2C */
  pObj->IO.Init();

  if(stpmic2l_read_reg(&pObj->Ctx, reg, &tmp, 1)!= STPMIC2L_OK)
  {
    ret = STPMIC2L_ERROR;
  }

  tmp |= mask;

  if(stpmic2l_write_reg(&pObj->Ctx, reg, &tmp, 1)!= STPMIC2L_OK)
  {
    ret = STPMIC2L_ERROR;
  }

  /* Component status */
  return ret;
}

#if defined (STPMIC2L_DEBUG)
int32_t STPMIC2L_DumpRegs(STPMIC2L_Object_t *pObj)
{
  int32_t  ret = STPMIC2L_OK;
  uint8_t data;

  /*  */
  for (int i = 0; i < STPMIC2L_REGISTERS_TABLE_SIZE; i++)
  {
	if (STPMIC2L_ReadReg(pObj, STPMIC2L_registers_table[i].RegAddr, &data) == STPMIC2L_OK)
	{
	  printf("(offset 0x%02X) %s = 0x%02X\r\n", STPMIC2L_registers_table[i].RegAddr, STPMIC2L_registers_table[i].RegName, data);
	}
	else
	{
	  printf("(offset 0x%02X)*** ERROR ***"
				" reading %s register = 0x%02X\r\n", STPMIC2L_registers_table[i].RegAddr, STPMIC2L_registers_table[i].RegName, data);
      ret = STPMIC2L_ERROR;
      break;
	}
  }

  return ret ;
}
#endif /* STPMIC2L_DEBUG */


/**
  * @}
  */

/** @defgroup STPMIC2L_Private_Functions Private Functions
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
static int32_t STPMIC2L_ReadRegWrap(void *handle, uint16_t Reg, uint8_t* pData, uint16_t Length)
{
  STPMIC2L_Object_t *pObj = (STPMIC2L_Object_t *)handle;

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
static int32_t STPMIC2L_WriteRegWrap(void *handle, uint16_t Reg, uint8_t* pData, uint16_t Length)
{
  STPMIC2L_Object_t *pObj = (STPMIC2L_Object_t *)handle;

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
