/**
  ******************************************************************************
  * @file    stpmic2l_reg.c
  * @author  MCD Application Team
  * @brief   This file provides unitary register function to control the STPMIC2L
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

/* Includes ------------------------------------------------------------------*/
#include "stpmic2l_reg.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup Components
  * @{
  */

/** @addtogroup STPMIC2L
  * @brief     This file provides a set of functions needed to drive the
  *            STPMIC2L Power Management Unit.
  * @{
  */

/**
  * @brief  Read STPMIC2L component registers
  * @param  ctx component contex
  * @param  reg Register to read from
  * @param  pdata Pointer to data buffer
  * @param  length Number of data to read
  * @retval Component status
  */
int32_t stpmic2l_read_reg(stpmic2l_ctx_t *ctx, uint16_t reg, uint8_t *pdata, uint16_t length)
{
  return ctx->ReadReg(ctx->handle, reg, pdata, length);
}

/**
  * @brief  Write STPMIC2L component registers
  * @param  ctx component contex
  * @param  reg Register to write to
  * @param  pdata Pointer to data buffer
  * @param  length Number of data to write
  * @retval Component status
  */
int32_t stpmic2l_write_reg(stpmic2l_ctx_t *ctx, uint16_t reg, uint8_t *pdata, uint16_t length)
{
  return ctx->WriteReg(ctx->handle, reg, pdata, length);
}

/**
  * @brief  Set STPMIC2L component registers
  * @param  ctx component contex
  * @param  reg Register to write to
  * @param  value data to write
  * @retval Component status
  */
int32_t stpmic2l_register_set(__attribute__((unused)) stpmic2l_ctx_t *ctx,
                              __attribute__((unused))uint16_t reg,
                              __attribute__((unused))uint8_t value)
{
  return 0;
}
/**
  * @brief  Update STPMIC2L component registers
  * @param  ctx component contex
  * @param  reg Register to write to
  * @param  pvalue Pointer to data to write
  * @retval Component status
  */
int32_t stpmic2l_register_get(__attribute__((unused))stpmic2l_ctx_t *ctx,
                              __attribute__((unused))uint16_t reg,
                              __attribute__((unused))uint8_t *pvalue)
{
  return 0;
}
/**
  * @brief  Update STPMIC2L component registers
  * @param  ctx component contex
  * @param  reg Register to write to
  * @param  pvalue Pointer to data buffer
  * @param  mask data mask
  * @retval Component status
  */
int32_t stpmic2l_register_update(__attribute__((unused))stpmic2l_ctx_t *ctx,
                                 __attribute__((unused))uint16_t reg,
                                 __attribute__((unused))uint8_t pvalue,
                                 __attribute__((unused))uint8_t mask)
{
  return 0;
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
