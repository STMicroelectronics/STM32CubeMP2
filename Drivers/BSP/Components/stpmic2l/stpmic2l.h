/**
  ******************************************************************************
  * @file    stpmic2l.h
  * @author  MCD Application Team
  * @brief   This file contains all the functions prototypes for the stpmic2l.c
  *          driver.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STPMIC2L_H
#define STPMIC2L_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stpmic2l_reg.h"
#include <stddef.h>

/** @addtogroup BSP
  * @{
  */

/** @addtogroup Components
  * @{
  */

/** @addtogroup stpmic2l
  * @{
  */

/** @defgroup STPMIC2L_Exported_Types
  * @{
  */

typedef int32_t (*STPMIC2L_Init_Func)    (void);
typedef int32_t (*STPMIC2L_DeInit_Func)  (void);
typedef int32_t (*STPMIC2L_GetTick_Func) (void);
typedef int32_t (*STPMIC2L_Delay_Func)   (uint32_t);
typedef int32_t (*STPMIC2L_WriteReg_Func)(uint16_t, uint16_t, uint8_t*, uint16_t);
typedef int32_t (*STPMIC2L_ReadReg_Func) (uint16_t, uint16_t, uint8_t*, uint16_t);

typedef struct
{
  STPMIC2L_Init_Func          Init;
  STPMIC2L_DeInit_Func        DeInit;
  uint16_t                    Address;
  STPMIC2L_WriteReg_Func      WriteReg;
  STPMIC2L_ReadReg_Func       ReadReg;
  STPMIC2L_GetTick_Func       GetTick;
} STPMIC2L_IO_t;

typedef struct
{
  STPMIC2L_IO_t         IO;
  stpmic2l_ctx_t        Ctx;
  uint8_t              IsInitialized;
} STPMIC2L_Object_t;

typedef struct
{
  int32_t  (*Init              )(STPMIC2L_Object_t*);
  int32_t  (*DeInit            )(STPMIC2L_Object_t*);
  int32_t  (*ReadID            )(STPMIC2L_Object_t*, uint8_t*);
  int32_t  (*ReadReg           )(STPMIC2L_Object_t*, uint8_t, uint8_t*);
  int32_t  (*WriteReg          )(STPMIC2L_Object_t*, uint8_t, uint8_t);
  int32_t  (*UpdateReg         )(STPMIC2L_Object_t*, uint8_t, uint8_t);
#if defined (STPMIC2L_DEBUG)
  int32_t  (*DumpRegs          )(STPMIC2L_Object_t*);
#endif /* STPMIC2L_DEBUG */
}STPMIC2L_Drv_t;

typedef struct
{
  uint8_t              RegAddr;
  char *               RegName;
} STPMIC2L_register_t;

/**
  * @}
  */

/** @defgroup STPMIC2L_Exported_Constants
  * @{
  */
#define STPMIC2L_OK                      (0)
#define STPMIC2L_ERROR                   (-1)

typedef enum
{
  STPMIC_BUCK1=0,
  STPMIC_BUCK2,
  STPMIC_BUCK3,
  STPMIC_LDO1,
  STPMIC_LDO2,
  STPMIC_LDO3,
  STPMIC_LDO4,
  STPMIC_LDO5,
  STPMIC_LDO6,
  STPMIC_LDO7,
  STPMIC_GPO1,
  STPMIC_GPO2,
  STPMIC_GPO3,
  STPMIC_GPO4,
  STPMIC_GPO5,
  STPMIC_NB_REG,
}PMIC_RegulId_TypeDef;

typedef enum {
	STPMIC2L_MASK_RESET = 0,
	STPMIC2L_PULL_DOWN,
	STPMIC2L_SINK_SOURCE,
	STPMIC2L_OCP,
}PMIC_RegulProp_TypeDef;

/* IRQ definitions */
typedef enum {
	STPMIC2L_IT_PONKEY_F=IT_PONKEY_F,
	STPMIC2L_IT_PONKEY_R,
	STPMIC2L_IT_VINLOW_FA=IT_VINLOW_FA,
	STPMIC2L_IT_VINLOW_RI,
	STPMIC2L_IT_THW_FA=IT_THW_FA,
	STPMIC2L_IT_THW_RI,
	STPMIC2L_IT_BUCK1_OCP=IT_BUCK1_OCP,
	STPMIC2L_IT_BUCK2_OCP,
  STPMIC2L_IT_BUCK3_OCP,
  STPMIC2L_IT_LDO1_OCP=IT_LDO1_OCP,
	STPMIC2L_IT_LDO2_OCP,
	STPMIC2L_IT_LDO3_OCP,
	STPMIC2L_IT_LDO4_OCP,
	STPMIC2L_IT_LDO5_OCP,
  STPMIC2L_IT_LDO6_OCP,
	STPMIC2L_IT_LDO7_OCP,
	IRQ_NR,
} PMIC_IRQn;


/**
 * @brief  STPMIC2L Features Parameters
 */

/**
  * @}
  */

/** @defgroup STPMIC2L_Exported_Functions STPMIC2L Exported Functions
  * @{
  */
int32_t STPMIC2L_RegisterBusIO (STPMIC2L_Object_t *pObj, STPMIC2L_IO_t *pIO);
int32_t STPMIC2L_Init(STPMIC2L_Object_t *pObj);
int32_t STPMIC2L_DeInit(STPMIC2L_Object_t *pObj);
int32_t STPMIC2L_ReadID(STPMIC2L_Object_t *pObj, uint8_t *Id);
int32_t STPMIC2L_ReadReg(STPMIC2L_Object_t *pObj, uint8_t reg, uint8_t *pData);
int32_t STPMIC2L_WriteReg(STPMIC2L_Object_t *pObj, uint8_t reg, uint8_t data);
int32_t STPMIC2L_UpdateReg(STPMIC2L_Object_t *pObj, uint8_t reg, uint8_t mask);
#if defined (STPMIC2L_DEBUG)
int32_t STPMIC2L_DumpRegs(STPMIC2L_Object_t *pObj);
#endif

/* STPMIC2L driver structure */
extern STPMIC2L_Drv_t   STPMIC2L_Driver;
/**
  * @}
  */
#ifdef __cplusplus
}
#endif

#endif /* STPMIC2L_H */
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
