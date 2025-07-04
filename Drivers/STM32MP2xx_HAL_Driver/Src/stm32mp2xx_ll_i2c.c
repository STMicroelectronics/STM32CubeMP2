/**
  ******************************************************************************
  * @file    stm32mp2xx_ll_i2c.c
  * @author  MCD Application Team
  * @brief   I2C LL module driver.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
#if defined(USE_FULL_LL_DRIVER)

/* Includes ------------------------------------------------------------------*/
#include "stm32mp2xx_ll_i2c.h"
#include "stm32mp2xx_ll_bus.h"
#ifdef  USE_FULL_ASSERT
#include "stm32_assert.h"
#else
#define assert_param(expr) ((void)0U)
#endif /* USE_FULL_ASSERT */

/** @addtogroup STM32MP2xx_LL_Driver
  * @{
  */

#if defined (I2C1) || defined (I2C2) || defined (I2C3) || defined (I2C4) ||\
    defined (I2C5) || defined (I2C6) || defined (I2C7) || defined (I2C8)

/** @defgroup I2C_LL I2C
  * @{
  */

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/** @addtogroup I2C_LL_Private_Macros
  * @{
  */

#define IS_LL_I2C_PERIPHERAL_MODE(__VALUE__)    (((__VALUE__) == LL_I2C_MODE_I2C)          || \
                                                 ((__VALUE__) == LL_I2C_MODE_SMBUS_HOST)   || \
                                                 ((__VALUE__) == LL_I2C_MODE_SMBUS_DEVICE) || \
                                                 ((__VALUE__) == LL_I2C_MODE_SMBUS_DEVICE_ARP))

#define IS_LL_I2C_ANALOG_FILTER(__VALUE__)      (((__VALUE__) == LL_I2C_ANALOGFILTER_ENABLE) || \
                                                 ((__VALUE__) == LL_I2C_ANALOGFILTER_DISABLE))

#define IS_LL_I2C_DIGITAL_FILTER(__VALUE__)     ((__VALUE__) <= 0x0000000FU)

#define IS_LL_I2C_OWN_ADDRESS1(__VALUE__)       ((__VALUE__) <= 0x000003FFU)

#define IS_LL_I2C_TYPE_ACKNOWLEDGE(__VALUE__)   (((__VALUE__) == LL_I2C_ACK) || \
                                                 ((__VALUE__) == LL_I2C_NACK))

#define IS_LL_I2C_OWN_ADDRSIZE(__VALUE__)       (((__VALUE__) == LL_I2C_OWNADDRESS1_7BIT) || \
                                                 ((__VALUE__) == LL_I2C_OWNADDRESS1_10BIT))
/**
  * @}
  */

/* Private function prototypes -----------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
/** @addtogroup I2C_LL_Exported_Functions
  * @{
  */

/** @addtogroup I2C_LL_EF_Init
  * @{
  */

/**
  * @brief  De-initialize the I2C registers to their default reset values.
  * @param  I2Cx I2C Instance.
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: I2C registers are de-initialized
  *          - ERROR: I2C registers are not de-initialized
  */
ErrorStatus LL_I2C_DeInit(const I2C_TypeDef *I2Cx)
{
  ErrorStatus status = SUCCESS;

  /* Check the I2C Instance I2Cx */
  assert_param(IS_I2C_ALL_INSTANCE(I2Cx));

  if (I2Cx == I2C1)
  {
    /* Force reset of I2C clock */
    LL_RCC_I2C1_ForceReset();

    /* Release reset of I2C clock */
    LL_RCC_I2C1_ReleaseReset();
  }
  else if (I2Cx == I2C2)
  {
    /* Force reset of I2C clock */
    LL_RCC_I2C2_ForceReset();

    /* Release reset of I2C clock */
    LL_RCC_I2C2_ReleaseReset();

  }
#if defined(I2C3)
  else if (I2Cx == I2C3)
  {
    /* Force reset of I2C clock */
    LL_RCC_I2C3_ForceReset();

    /* Release reset of I2C clock */
    LL_RCC_I2C3_ReleaseReset();
  }
#endif /* I2C3 */
#if defined(I2C4)
  else if (I2Cx == I2C4)
  {
    /* Force reset of I2C clock */
    LL_RCC_I2C4_ForceReset();

    /* Release reset of I2C clock */
    LL_RCC_I2C4_ReleaseReset();
  }
#endif /* I2C4 */
#if defined(I2C5)
  else if (I2Cx == I2C5)
  {
    /* Force reset of I2C clock */
    LL_RCC_I2C5_ForceReset();

    /* Release reset of I2C clock */
    LL_RCC_I2C5_ReleaseReset();
  }
#endif /* I2C5 */
#if defined(I2C6)
  else if (I2Cx == I2C6)
  {
    /* Force reset of I2C clock */
    LL_RCC_I2C6_ForceReset();

    /* Release reset of I2C clock */
    LL_RCC_I2C6_ReleaseReset();
  }
#endif /* I2C6 */
#if defined(I2C7)
  else if (I2Cx == I2C7)
  {
    /* Force reset of I2C clock */
    LL_RCC_I2C7_ForceReset();

    /* Release reset of I2C clock */
    LL_RCC_I2C7_ReleaseReset();
  }
#endif /* I2C7 */
#if defined(I2C8)
#if defined(I2C8) && ! defined(CORE_CM0PLUS)
  else if (I2Cx == I2C8)
  {
    /* Force reset of I2C clock */
    LL_RCC_I2C8_ForceReset();

    /* Release reset of I2C clock */
    LL_RCC_I2C8_ReleaseReset();
  }
#endif /* I2C8 && ! CORE_CM0PLUS */
#endif /* I2C8 */
  else
  {
    status = ERROR;
  }

  return status;
}

/**
  * @brief  Initialize the I2C registers according to the specified parameters in I2C_InitStruct.
  * @param  I2Cx I2C Instance.
  * @param  I2C_InitStruct pointer to a @ref LL_I2C_InitTypeDef structure.
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: I2C registers are initialized
  *          - ERROR: Not applicable
  */
ErrorStatus LL_I2C_Init(I2C_TypeDef *I2Cx, const LL_I2C_InitTypeDef *I2C_InitStruct)
{
  /* Check the I2C Instance I2Cx */
  assert_param(IS_I2C_ALL_INSTANCE(I2Cx));

  /* Check the I2C parameters from I2C_InitStruct */
  assert_param(IS_LL_I2C_PERIPHERAL_MODE(I2C_InitStruct->PeripheralMode));
  assert_param(IS_LL_I2C_ANALOG_FILTER(I2C_InitStruct->AnalogFilter));
  assert_param(IS_LL_I2C_DIGITAL_FILTER(I2C_InitStruct->DigitalFilter));
  assert_param(IS_LL_I2C_OWN_ADDRESS1(I2C_InitStruct->OwnAddress1));
  assert_param(IS_LL_I2C_TYPE_ACKNOWLEDGE(I2C_InitStruct->TypeAcknowledge));
  assert_param(IS_LL_I2C_OWN_ADDRSIZE(I2C_InitStruct->OwnAddrSize));

  /* Disable the selected I2Cx Peripheral */
  LL_I2C_Disable(I2Cx);

  /*---------------------------- I2Cx CR1 Configuration ------------------------
   * Configure the analog and digital noise filters with parameters :
   * - AnalogFilter: I2C_CR1_ANFOFF bit
   * - DigitalFilter: I2C_CR1_DNF[3:0] bits
   */
  LL_I2C_ConfigFilters(I2Cx, I2C_InitStruct->AnalogFilter, I2C_InitStruct->DigitalFilter);

  /*---------------------------- I2Cx TIMINGR Configuration --------------------
   * Configure the SDA setup, hold time and the SCL high, low period with parameter :
   * - Timing: I2C_TIMINGR_PRESC[3:0], I2C_TIMINGR_SCLDEL[3:0], I2C_TIMINGR_SDADEL[3:0],
   *           I2C_TIMINGR_SCLH[7:0] and I2C_TIMINGR_SCLL[7:0] bits
   */
  LL_I2C_SetTiming(I2Cx, I2C_InitStruct->Timing);

  /* Enable the selected I2Cx Peripheral */
  LL_I2C_Enable(I2Cx);

  /*---------------------------- I2Cx OAR1 Configuration -----------------------
   * Disable, Configure and Enable I2Cx device own address 1 with parameters :
   * - OwnAddress1:  I2C_OAR1_OA1[9:0] bits
   * - OwnAddrSize:  I2C_OAR1_OA1MODE bit
   */
  LL_I2C_DisableOwnAddress1(I2Cx);
  LL_I2C_SetOwnAddress1(I2Cx, I2C_InitStruct->OwnAddress1, I2C_InitStruct->OwnAddrSize);

  /* OwnAdress1 == 0 is reserved for General Call address */
  if (I2C_InitStruct->OwnAddress1 != 0U)
  {
    LL_I2C_EnableOwnAddress1(I2Cx);
  }

  /*---------------------------- I2Cx MODE Configuration -----------------------
  * Configure I2Cx peripheral mode with parameter :
   * - PeripheralMode: I2C_CR1_SMBDEN and I2C_CR1_SMBHEN bits
   */
  LL_I2C_SetMode(I2Cx, I2C_InitStruct->PeripheralMode);

  /*---------------------------- I2Cx CR2 Configuration ------------------------
   * Configure the ACKnowledge or Non ACKnowledge condition
   * after the address receive match code or next received byte with parameter :
   * - TypeAcknowledge: I2C_CR2_NACK bit
   */
  LL_I2C_AcknowledgeNextData(I2Cx, I2C_InitStruct->TypeAcknowledge);

  return SUCCESS;
}

/**
  * @brief  Set each @ref LL_I2C_InitTypeDef field to default value.
  * @param  I2C_InitStruct Pointer to a @ref LL_I2C_InitTypeDef structure.
  * @retval None
  */
void LL_I2C_StructInit(LL_I2C_InitTypeDef *I2C_InitStruct)
{
  /* Set I2C_InitStruct fields to default values */
  I2C_InitStruct->PeripheralMode  = LL_I2C_MODE_I2C;
  I2C_InitStruct->Timing          = 0U;
  I2C_InitStruct->AnalogFilter    = LL_I2C_ANALOGFILTER_ENABLE;
  I2C_InitStruct->DigitalFilter   = 0U;
  I2C_InitStruct->OwnAddress1     = 0U;
  I2C_InitStruct->TypeAcknowledge = LL_I2C_NACK;
  I2C_InitStruct->OwnAddrSize     = LL_I2C_OWNADDRESS1_7BIT;
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

#endif /* I2C1 || I2C2 || I2C3 || I2C4 || I2C5 || I2C6 || I2C7 || I2C8 */

/**
  * @}
  */

#endif /* USE_FULL_LL_DRIVER */
