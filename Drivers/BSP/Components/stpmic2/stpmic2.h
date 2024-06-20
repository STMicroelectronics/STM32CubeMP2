/**
  ******************************************************************************
  * @file    stpmic2.h
  * @author  MCD Application Team
  * @brief   This file contains all the functions prototypes for the stpmic2.c
  *          driver.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STPMIC2_H
#define STPMIC2_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stpmic2_reg.h"
#include <stddef.h>

/** @addtogroup BSP
  * @{
  */

/** @addtogroup Components
  * @{
  */

/** @addtogroup stpmic2
  * @{
  */

/** @defgroup STPMIC2_Exported_Types
  * @{
  */

typedef int32_t (*STPMIC2_Init_Func)(void);
typedef int32_t (*STPMIC2_DeInit_Func)(void);
typedef int32_t (*STPMIC2_GetTick_Func)(void);
typedef int32_t (*STPMIC2_Delay_Func)(uint32_t);
typedef int32_t (*STPMIC2_WriteReg_Func)(uint16_t, uint16_t, uint8_t *, uint16_t);
typedef int32_t (*STPMIC2_ReadReg_Func)(uint16_t, uint16_t, uint8_t *, uint16_t);

typedef struct
{
  STPMIC2_Init_Func          Init;
  STPMIC2_DeInit_Func        DeInit;
  uint16_t                   Address;
  STPMIC2_WriteReg_Func      WriteReg;
  STPMIC2_ReadReg_Func       ReadReg;
  STPMIC2_GetTick_Func       GetTick;
} STPMIC2_IO_t;

typedef struct
{
  STPMIC2_IO_t         IO;
  stpmic2_ctx_t        Ctx;
  uint8_t              IsInitialized;
} STPMIC2_Object_t;

typedef struct
{
  int32_t (*Init)(STPMIC2_Object_t *);
  int32_t (*DeInit)(STPMIC2_Object_t *);
  int32_t (*ReadID)(STPMIC2_Object_t *, uint8_t *);
  int32_t (*ReadReg)(STPMIC2_Object_t *, uint8_t, uint8_t *);
  int32_t (*WriteReg)(STPMIC2_Object_t *, uint8_t, uint8_t);
  int32_t (*UpdateReg)(STPMIC2_Object_t *, uint8_t, uint8_t);
  int32_t (*DumpRegs)(STPMIC2_Object_t *);
} STPMIC2_Drv_t;

typedef struct
{
  uint8_t              RegAddr;
  char                *RegName;
} STPMIC2_register_t;

/**
  * @}
  */

/** @defgroup STPMIC2_Exported_Constants
  * @{
  */
#define STPMIC2_OK                      (0)
#define STPMIC2_ERROR                   (-1)

typedef enum
{
  STPMIC_BUCK1 = 0,
  STPMIC_BUCK2,
  STPMIC_BUCK3,
  STPMIC_BUCK4,
  STPMIC_BUCK5,
  STPMIC_BUCK6,
  STPMIC_BUCK7,
  STPMIC_LDO1,
  STPMIC_LDO2,
  STPMIC_LDO3,
  STPMIC_LDO4,
  STPMIC_LDO5,
  STPMIC_LDO6,
  STPMIC_LDO7,
  STPMIC_LDO8,
  STPMIC_REFDDR,
  STPMIC_NB_REG,
} PMIC_RegulId_TypeDef;

typedef enum
{
  STPMIC2_MASK_RESET = 0,
  STPMIC2_PULL_DOWN,
  STPMIC2_BYPASS,   /* arg: 1=set 0=reset */
  STPMIC2_SINK_SOURCE,
  STPMIC2_OCP,
} PMIC_RegulProp_TypeDef;

/* IRQ definitions */
typedef enum
{
  STPMIC2_IT_PONKEY_F = IT_PONKEY_F,
  STPMIC2_IT_PONKEY_R,
  STPMIC2_IT_WKP_FA,
  STPMIC2_IT_WKP_RI,
  STPMIC2_IT_VINLOW_FA,
  STPMIC2_IT_VINLOW_RI,
  STPMIC2_IT_VBUS_FA,
  STPMIC2_IT_VBUS_RI,
  STPMIC2_IT_THW_FA,
  STPMIC2_IT_THW_RI,
  STPMIC2_IT_BUCK1_OCP = IT_BUCK1_OCP,
  STPMIC2_IT_BUCK2_OCP,
  STPMIC2_IT_BUCK3_OCP,
  STPMIC2_IT_BUCK4_OCP,
  STPMIC2_IT_BUCK5_OCP,
  STPMIC2_IT_BUCK6_OCP,
  STPMIC2_IT_BUCK7_OCP,
  STPMIC2_IT_REFDDR_OCP,
  STPMIC2_IT_LDO1_OCP,
  STPMIC2_IT_LDO2_OCP,
  STPMIC2_IT_LDO3_OCP,
  STPMIC2_IT_LDO4_OCP,
  STPMIC2_IT_LDO5_OCP,
  STPMIC2_IT_LDO6_OCP,
  STPMIC2_IT_LDO7_OCP,
  STPMIC2_IT_LDO8_OCP,
  IRQ_NR,
} PMIC_IRQn;


/**
  * @brief  STPMIC2 Features Parameters
  */

/**
  * @}
  */

/** @defgroup STPMIC2_Exported_Functions STPMIC2 Exported Functions
  * @{
  */
int32_t STPMIC2_RegisterBusIO(STPMIC2_Object_t *pObj, STPMIC2_IO_t *pIO);
int32_t STPMIC2_Init(STPMIC2_Object_t *pObj);
int32_t STPMIC2_DeInit(STPMIC2_Object_t *pObj);
int32_t STPMIC2_ReadID(STPMIC2_Object_t *pObj, uint8_t *Id);
int32_t STPMIC2_ReadReg(STPMIC2_Object_t *pObj, uint8_t reg, uint8_t *pData);
int32_t STPMIC2_WriteReg(STPMIC2_Object_t *pObj, uint8_t reg, uint8_t data);
int32_t STPMIC2_UpdateReg(STPMIC2_Object_t *pObj, uint8_t reg, uint8_t mask);
#if defined (STPMIC2_DEBUG)
int32_t STPMIC2_DumpRegs(STPMIC2_Object_t *pObj);
#endif /* STPMIC2_DEBUG */

/* STPMIC2 driver structure */
extern STPMIC2_Drv_t   STPMIC2_Driver;
/**
  * @}
  */
#ifdef __cplusplus
}
#endif

#endif /* STPMIC2_H */
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
