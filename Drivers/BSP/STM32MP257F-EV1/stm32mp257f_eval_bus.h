/**
  ******************************************************************************
  * @file    stm32mp257f_eval_bus.h
  * @author  MCD Application Team
  * @brief   This file is the header of stm32mp257f_eval_bus.c file
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
#ifndef STM32MP257F_EVAL_BUS_H
#define STM32MP257F_EVAL_BUS_H

#ifdef __cplusplus
extern "C" {
#endif


/* Includes ------------------------------------------------------------------*/
#include "stm32mp257f_eval_conf.h"

#if defined(BSP_USE_CMSIS_OS)
#include "cmsis_os.h"
#endif /* BSP_USE_CMSIS_OS */
/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32MP257F_EV1
  * @{
  */

/** @addtogroup BUS
  * @{
  */
#if defined (CORE_CA35) || defined (CORE_CM33)
/** @defgroup BUS_Exported_Types BUS Exported Types
  * @{
  */
#if (USE_HAL_I2C_REGISTER_CALLBACKS > 0)
typedef struct
{
  pI2C_CallbackTypeDef  pMspI2cInitCb;
  pI2C_CallbackTypeDef  pMspI2cDeInitCb;
} BSP_I2C_Cb_t;
#endif /* (USE_HAL_I2C_REGISTER_CALLBACKS > 0) */

/**
  * @}
  */
/** @defgroup BUS_Exported_Constants BUS Exported Constants
  * @{
  */
/* Definition for I2C1 clock resources */
#define BUS_I2C1                              I2C1

#define BUS_I2C1_CLK_ENABLE()                  __HAL_RCC_I2C1_CLK_ENABLE()
#define BUS_I2C1_CLK_DISABLE()                 __HAL_RCC_I2C1_CLK_DISABLE()

#define BUS_I2C1_SCL_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOG_CLK_ENABLE()
#define BUS_I2C1_SCL_GPIO_CLK_DISABLE()        __HAL_RCC_GPIOG_CLK_DISABLE()

#define BUS_I2C1_SDA_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOI_CLK_ENABLE()
#define BUS_I2C1_SDA_GPIO_CLK_DISABLE()        __HAL_RCC_GPIOI_CLK_DISABLE()

#define BUS_I2C1_FORCE_RESET()                 __HAL_RCC_I2C1_FORCE_RESET()
#define BUS_I2C1_RELEASE_RESET()               __HAL_RCC_I2C1_RELEASE_RESET()

/* Definition for I2C1 Pins */
#define BUS_I2C1_SCL_PIN                       GPIO_PIN_13
#define BUS_I2C1_SCL_GPIO_PORT                 GPIOG
#define BUS_I2C1_SCL_AF                        GPIO_AF9_I2C1

#define BUS_I2C1_SDA_PIN                       GPIO_PIN_1
#define BUS_I2C1_SDA_GPIO_PORT                 GPIOI
#define BUS_I2C1_SDA_AF                        GPIO_AF9_I2C1

#ifndef BUS_I2C1_FREQUENCY
#define BUS_I2C1_FREQUENCY  400000U /* Frequency of I2C1 = 400 KHz*/
#endif /* BUS_I2C1_FREQUENCY */

/* Definition for I2C2 clock resources */
#define BUS_I2C2                              I2C2

#define BUS_I2C2_CLK_ENABLE()                  __HAL_RCC_I2C2_CLK_ENABLE()
#define BUS_I2C2_CLK_DISABLE()                 __HAL_RCC_I2C2_CLK_DISABLE()

#define BUS_I2C2_SCL_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOB_CLK_ENABLE()
#define BUS_I2C2_SCL_GPIO_CLK_DISABLE()        __HAL_RCC_GPIOB_CLK_DISABLE()

#define BUS_I2C2_SDA_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOB_CLK_ENABLE()
#define BUS_I2C2_SDA_GPIO_CLK_DISABLE()        __HAL_RCC_GPIOB_CLK_DISABLE()

#define BUS_I2C2_FORCE_RESET()                 __HAL_RCC_I2C2_FORCE_RESET()
#define BUS_I2C2_RELEASE_RESET()               __HAL_RCC_I2C2_RELEASE_RESET()

/* Definition for I2C2 Pins */
#define BUS_I2C2_SCL_PIN                       GPIO_PIN_5
#define BUS_I2C2_SCL_GPIO_PORT                 GPIOB
#define BUS_I2C2_SCL_AF                        GPIO_AF9_I2C2

#define BUS_I2C2_SDA_PIN                       GPIO_PIN_4
#define BUS_I2C2_SDA_GPIO_PORT                 GPIOB
#define BUS_I2C2_SDA_AF                        GPIO_AF9_I2C2

#ifndef BUS_I2C2_FREQUENCY
#define BUS_I2C2_FREQUENCY  400000U /* Frequency of I2C2 = 400 KHz*/
#endif /* BUS_I2C2_FREQUENCY */

/* A.Ou : to update */
/* Definition for I2C3 clock resources */
#define BUS_I2C3                              I2C3

#define BUS_I2C3_CLK_ENABLE()                  __HAL_RCC_I2C3_CLK_ENABLE()
#define BUS_I2C3_CLK_DISABLE()                 __HAL_RCC_I2C3_CLK_DISABLE()

#define BUS_I2C3_SCL_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOG_CLK_ENABLE()
#define BUS_I2C3_SCL_GPIO_CLK_DISABLE()        __HAL_RCC_GPIOG_CLK_DISABLE()

#define BUS_I2C3_SDA_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOG_CLK_ENABLE()
#define BUS_I2C3_SDA_GPIO_CLK_DISABLE()        __HAL_RCC_GPIOG_CLK_DISABLE()

#define BUS_I2C3_FORCE_RESET()                 __HAL_RCC_I2C3_FORCE_RESET()
#define BUS_I2C3_RELEASE_RESET()               __HAL_RCC_I2C3_RELEASE_RESET()

/* Definition for I2C3 Pins */
#define BUS_I2C3_SCL_PIN                       GPIO_PIN_1
#define BUS_I2C3_SCL_GPIO_PORT                 GPIOG
#define BUS_I2C3_SCL_AF                        GPIO_AF9_I2C3

#define BUS_I2C3_SDA_PIN                       GPIO_PIN_2
#define BUS_I2C3_SDA_GPIO_PORT                 GPIOG
#define BUS_I2C3_SDA_AF                        GPIO_AF9_I2C3

#ifndef BUS_I2C3_FREQUENCY
#define BUS_I2C3_FREQUENCY  400000U /* Frequency of I2C3 = 400 KHz*/
#endif /* BUS_I2C3_FREQUENCY */

/* Definition for I2C6 clock resources */
#define BUS_I2C6                              I2C6

#define BUS_I2C6_CLK_ENABLE()                  __HAL_RCC_I2C6_CLK_ENABLE()
#define BUS_I2C6_CLK_DISABLE()                 __HAL_RCC_I2C6_CLK_DISABLE()

#define BUS_I2C6_SCL_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOK_CLK_ENABLE()
#define BUS_I2C6_SCL_GPIO_CLK_DISABLE()        __HAL_RCC_GPIOK_CLK_DISABLE()

#define BUS_I2C6_SDA_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOK_CLK_ENABLE()
#define BUS_I2C6_SDA_GPIO_CLK_DISABLE()        __HAL_RCC_GPIOK_CLK_DISABLE()

#define BUS_I2C6_FORCE_RESET()                 __HAL_RCC_I2C6_FORCE_RESET()
#define BUS_I2C6_RELEASE_RESET()               __HAL_RCC_I2C6_RELEASE_RESET()

/* Definition for I2C6 Pins */
#define BUS_I2C6_SCL_PIN                       GPIO_PIN_6
#define BUS_I2C6_SCL_GPIO_PORT                 GPIOK
#define BUS_I2C6_SCL_AF                        GPIO_AF9_I2C6

#define BUS_I2C6_SDA_PIN                       GPIO_PIN_2
#define BUS_I2C6_SDA_GPIO_PORT                 GPIOK
#define BUS_I2C6_SDA_AF                        GPIO_AF9_I2C6

#ifndef BUS_I2C6_FREQUENCY
#define BUS_I2C6_FREQUENCY  400000U /* Frequency of I2C6 = 400 KHz*/
#endif /* BUS_I2C6_FREQUENCY */

/* Definition for I2C7 clock resources */
#define BUS_I2C7                              I2C7

#define BUS_I2C7_CLK_ENABLE()                  __HAL_RCC_I2C7_CLK_ENABLE()
#define BUS_I2C7_CLK_DISABLE()                 __HAL_RCC_I2C7_CLK_DISABLE()

#define BUS_I2C7_SCL_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOD_CLK_ENABLE()
#define BUS_I2C7_SCL_GPIO_CLK_DISABLE()        __HAL_RCC_GPIOD_CLK_DISABLE()

#define BUS_I2C7_SDA_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOD_CLK_ENABLE()
#define BUS_I2C7_SDA_GPIO_CLK_DISABLE()        __HAL_RCC_GPIOD_CLK_DISABLE()

#define BUS_I2C7_FORCE_RESET()                 __HAL_RCC_I2C7_FORCE_RESET()
#define BUS_I2C7_RELEASE_RESET()               __HAL_RCC_I2C7_RELEASE_RESET()

/* Definition for I2C7 Pins */
#define BUS_I2C7_SCL_PIN                       GPIO_PIN_15
#define BUS_I2C7_SCL_GPIO_PORT                 GPIOD
#define BUS_I2C7_SCL_AF                        GPIO_AF10_I2C7

#define BUS_I2C7_SDA_PIN                       GPIO_PIN_14
#define BUS_I2C7_SDA_GPIO_PORT                 GPIOD
#define BUS_I2C7_SDA_AF                        GPIO_AF10_I2C7

#ifndef BUS_I2C7_FREQUENCY
#define BUS_I2C7_FREQUENCY  400000U /* Frequency of I2C7 = 400 KHz*/
#endif /* BUS_I2C7_FREQUENCY */

/**
  * @}
  */

/** @addtogroup BUS_Exported_Variables BUS Exported Variables
  * @{
  */
extern I2C_HandleTypeDef hbus_i2c2;
extern I2C_HandleTypeDef hbus_i2c3;
extern I2C_HandleTypeDef hbus_i2c6;
extern I2C_HandleTypeDef hbus_i2c7;
/**
  * @}
  */

/** @addtogroup BUS_Exported_Functions BUS Exported Functions
  * @{
  */
int32_t BSP_GetTick(void);

/* Function prototypes for I2C1 */
int32_t BSP_I2C1_Init(void);
int32_t BSP_I2C1_DeInit(void);
int32_t BSP_I2C1_WriteReg(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C1_ReadReg(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C1_WriteReg16(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C1_ReadReg16(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C1_Recv(uint16_t DevAddr, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C1_Send(uint16_t DevAddr, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C1_IsReady(uint16_t DevAddr, uint32_t Trials);

#if (USE_HAL_I2C_REGISTER_CALLBACKS > 0)
int32_t BSP_I2C1_RegisterDefaultMspCallbacks(void);
int32_t BSP_I2C1_RegisterMspCallbacks(BSP_I2C_Cb_t *Callback);
#endif /* USE_HAL_I2C_REGISTER_CALLBACKS */
__weak HAL_StatusTypeDef MX_I2C1_Init(I2C_HandleTypeDef *hI2c, uint32_t timing);

/* Function prototypes for I2C2 */
int32_t BSP_I2C2_Init(void);
int32_t BSP_I2C2_DeInit(void);
int32_t BSP_I2C2_WriteReg(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C2_ReadReg(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C2_WriteReg16(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C2_ReadReg16(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C2_Recv(uint16_t DevAddr, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C2_Send(uint16_t DevAddr, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C2_IsReady(uint16_t DevAddr, uint32_t Trials);

#if (USE_HAL_I2C_REGISTER_CALLBACKS > 0)
int32_t BSP_I2C2_RegisterDefaultMspCallbacks(void);
int32_t BSP_I2C2_RegisterMspCallbacks(BSP_I2C_Cb_t *Callback);
#endif /* USE_HAL_I2C_REGISTER_CALLBACKS */
__weak HAL_StatusTypeDef MX_I2C2_Init(I2C_HandleTypeDef *hI2c, uint32_t timing);

/* Function prototypes for I2C3 */
int32_t BSP_I2C3_Init(void);
int32_t BSP_I2C3_DeInit(void);
int32_t BSP_I2C3_WriteReg(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C3_ReadReg(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C3_WriteReg16(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C3_ReadReg16(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C3_Recv(uint16_t DevAddr, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C3_Send(uint16_t DevAddr, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C3_IsReady(uint16_t DevAddr, uint32_t Trials);

#if (USE_HAL_I2C_REGISTER_CALLBACKS > 0)
int32_t BSP_I2C3_RegisterDefaultMspCallbacks(void);
int32_t BSP_I2C3_RegisterMspCallbacks(BSP_I2C_Cb_t *Callback);
#endif /* USE_HAL_I2C_REGISTER_CALLBACKS */
__weak HAL_StatusTypeDef MX_I2C3_Init(I2C_HandleTypeDef *hI2c, uint32_t timing);

/* Function prototypes for I2C6 */
int32_t BSP_I2C6_Init(void);
int32_t BSP_I2C6_DeInit(void);
int32_t BSP_I2C6_WriteReg(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C6_ReadReg(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C6_WriteReg16(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C6_ReadReg16(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C6_Recv(uint16_t DevAddr, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C6_Send(uint16_t DevAddr, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C6_IsReady(uint16_t DevAddr, uint32_t Trials);

#if (USE_HAL_I2C_REGISTER_CALLBACKS > 0)
int32_t BSP_I2C6_RegisterDefaultMspCallbacks(void);
int32_t BSP_I2C6_RegisterMspCallbacks(BSP_I2C_Cb_t *Callback);
#endif /* USE_HAL_I2C_REGISTER_CALLBACKS */
__weak HAL_StatusTypeDef MX_I2C6_Init(I2C_HandleTypeDef *hI2c, uint32_t timing);

/* Function prototypes for I2C7 */
int32_t BSP_I2C7_Init(void);
int32_t BSP_I2C7_DeInit(void);
int32_t BSP_I2C7_WriteReg(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C7_ReadReg(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C7_WriteReg16(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C7_ReadReg16(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C7_Recv(uint16_t DevAddr, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C7_Send(uint16_t DevAddr, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C7_IsReady(uint16_t DevAddr, uint32_t Trials);

#if (USE_HAL_I2C_REGISTER_CALLBACKS > 0)
int32_t BSP_I2C7_RegisterDefaultMspCallbacks(void);
int32_t BSP_I2C7_RegisterMspCallbacks(BSP_I2C_Cb_t *Callback);
#endif /* USE_HAL_I2C_REGISTER_CALLBACKS */
__weak HAL_StatusTypeDef MX_I2C7_Init(I2C_HandleTypeDef *hI2c, uint32_t timing);
#endif /* CORE_CA35 || CORE_CM33 */
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

#endif /* STM32MP257F_EVAL_BUS_H */
