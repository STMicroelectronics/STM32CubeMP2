/**
  ******************************************************************************
  * @file    stpmic1l.h
  * @author  MCD Application Team
  * @brief   This file contains all the functions prototypes for the stpmic1l.c
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
#ifndef STPMIC1L_H
#define STPMIC1L_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stpmic1l_reg.h"
#include <stddef.h>

/** @addtogroup BSP
  * @{
  */

/** @addtogroup Components
  * @{
  */

/** @addtogroup stpmic1l
  * @{
  */

/** @defgroup STPMIC1L_Exported_Types
  * @{
  */

typedef int32_t (*STPMIC1L_Init_Func)    (void);
typedef int32_t (*STPMIC1L_DeInit_Func)  (void);
typedef int32_t (*STPMIC1L_GetTick_Func) (void);
typedef int32_t (*STPMIC1L_Delay_Func)   (uint32_t);
typedef int32_t (*STPMIC1L_WriteReg_Func)(uint16_t, uint16_t, uint8_t*, uint16_t);
typedef int32_t (*STPMIC1L_ReadReg_Func) (uint16_t, uint16_t, uint8_t*, uint16_t);

typedef struct
{
  STPMIC1L_Init_Func          Init;
  STPMIC1L_DeInit_Func        DeInit;
  uint16_t                    Address;
  STPMIC1L_WriteReg_Func      WriteReg;
  STPMIC1L_ReadReg_Func       ReadReg;
  STPMIC1L_GetTick_Func       GetTick;
} STPMIC1L_IO_t;

typedef struct
{
  STPMIC1L_IO_t         IO;
  stpmic1l_ctx_t        Ctx;
  uint8_t              IsInitialized;
} STPMIC1L_Object_t;

typedef struct
{
  int32_t  (*Init              )(STPMIC1L_Object_t*);
  int32_t  (*DeInit            )(STPMIC1L_Object_t*);
  int32_t  (*ReadID            )(STPMIC1L_Object_t*, uint8_t*);
  int32_t  (*ReadReg           )(STPMIC1L_Object_t*, uint8_t, uint8_t*);
  int32_t  (*WriteReg          )(STPMIC1L_Object_t*, uint8_t, uint8_t);
  int32_t  (*UpdateReg         )(STPMIC1L_Object_t*, uint8_t, uint8_t);
  int32_t  (*DumpRegs          )(STPMIC1L_Object_t*);
}STPMIC1L_Drv_t;

typedef struct
{
  uint8_t              RegAddr;
  char *               RegName;
} STPMIC1L_register_t;

/**
  * @}
  */

/** @defgroup STPMIC1L_Exported_Constants
  * @{
  */
#define STPMIC1L_OK                      (0)
#define STPMIC1L_ERROR                   (-1)

typedef enum
{
  STPMIC_BUCK1=0,
  STPMIC_BUCK2,
  STPMIC_LDO2,
  STPMIC_LDO3,
  STPMIC_LDO4,
  STPMIC_LDO5,
  STPMIC_GPO1,
  STPMIC_GPO2,
  STPMIC_NB_REG,
}PMIC_RegulId_TypeDef;

typedef enum {
	STPMIC1L_MASK_RESET = 0,
	STPMIC1L_PULL_DOWN,
	STPMIC1L_SINK_SOURCE,
	STPMIC1L_OCP,
}PMIC_RegulProp_TypeDef;

/* IRQ definitions */
typedef enum {
	STPMIC1L_IT_PONKEY_F=IT_PONKEY_F,
	STPMIC1L_IT_PONKEY_R,
	STPMIC1L_IT_VINLOW_FA=IT_VINLOW_FA,
	STPMIC1L_IT_VINLOW_RI,
	STPMIC1L_IT_THW_FA=IT_THW_FA,
	STPMIC1L_IT_THW_RI,
	STPMIC1L_IT_BUCK1_OCP=IT_BUCK1_OCP,
	STPMIC1L_IT_BUCK2_OCP,
	STPMIC1L_IT_LDO2_OCP=IT_LDO2_OCP,
	STPMIC1L_IT_LDO3_OCP,
	STPMIC1L_IT_LDO4_OCP,
	STPMIC1L_IT_LDO5_OCP,
	IRQ_NR,
} PMIC_IRQn;


/**
 * @brief  STPMIC1L Features Parameters
 */

/**
  * @}
  */

/** @defgroup STPMIC1L_Exported_Functions STPMIC1L Exported Functions
  * @{
  */
int32_t STPMIC1L_RegisterBusIO (STPMIC1L_Object_t *pObj, STPMIC1L_IO_t *pIO);
int32_t STPMIC1L_Init(STPMIC1L_Object_t *pObj);
int32_t STPMIC1L_DeInit(STPMIC1L_Object_t *pObj);
int32_t STPMIC1L_ReadID(STPMIC1L_Object_t *pObj, uint8_t *Id);
int32_t STPMIC1L_ReadReg(STPMIC1L_Object_t *pObj, uint8_t reg, uint8_t *pData);
int32_t STPMIC1L_WriteReg(STPMIC1L_Object_t *pObj, uint8_t reg, uint8_t data);
int32_t STPMIC1L_UpdateReg(STPMIC1L_Object_t *pObj, uint8_t reg, uint8_t mask);
#if defined (STPMIC1L_DEBUG)
int32_t STPMIC1L_DumpRegs(STPMIC1L_Object_t *pObj);
#endif

/* STPMIC1L driver structure */
extern STPMIC1L_Drv_t   STPMIC1L_Driver;
/**
  * @}
  */
#ifdef __cplusplus
}
#endif

#endif /* STPMIC1L_H */
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
