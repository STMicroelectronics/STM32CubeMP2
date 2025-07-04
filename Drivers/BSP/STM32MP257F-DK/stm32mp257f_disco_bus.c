/**
  ******************************************************************************
  * @file    stm32mp257f_disco_bus.c
  * @author  MCD Application Team
  * @brief   This file provides a set of firmware functions to communicate
  *          with  external devices available on STM32MP257XX-DK board
  *          from STMicroelectronics.
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

/* Includes ------------------------------------------------------------------*/
#include "stm32mp257f_disco_bus.h"
#include "stm32mp257f_disco_errno.h"
#include "res_mgr.h"
/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32MP257F_DK
  * @{
  */

/** @defgroup STM32MP257F_DK_BUS BUS
  * @{
  */
extern void Error_Handler(void);
/** @defgroup STM32MP257F_DK_BUS_Private_Constants BUS Private Constants
  * @{
  */
#ifndef I2C_VALID_TIMING_NBR
#define I2C_VALID_TIMING_NBR                 128U
#endif /* I2C_VALID_TIMING_NBR */

#define I2C_HSI_TIMING_100KHZ           0x2050606F /* Analog Filter ON, Rise Time 25ns, Fall Time 10ns */
#define I2C_HSI_TIMING_400KHZ           0x2020121D /* Analog Filter ON, Rise Time 25ns, Fall Time 10ns */
#define I2C_HSI_TIMING_1MHZ             0x00910B1C /* Analog Filter ON, Rise Time 100ns, Fall Time 100ns */

#define I2C_TIMINGR                     I2C_HSI_TIMING_100KHZ

#define I2C_SPEED_FREQ_STANDARD                0U    /* 100 kHz */
#define I2C_SPEED_FREQ_FAST                    1U    /* 400 kHz */
#define I2C_SPEED_FREQ_FAST_PLUS               2U    /* 1 MHz */
#define I2C_ANALOG_FILTER_DELAY_MIN            50U   /* ns */
#define I2C_ANALOG_FILTER_DELAY_MAX            260U  /* ns */
#define I2C_USE_ANALOG_FILTER                  1U
#define I2C_DIGITAL_FILTER_COEF                0U
#define I2C_PRESC_MAX                          16U
#define I2C_SCLDEL_MAX                         16U
#define I2C_SDADEL_MAX                         16U
#define I2C_SCLH_MAX                           256U
#define I2C_SCLL_MAX                           256U
#define SEC2NSEC                               1000000000UL
/**
  * @}
  */

/** @defgroup STM32MP257F_DK_BUS_Private_Types BUS Private Types
  * @{
  */
typedef struct
{
  uint32_t freq;       /* Frequency in Hz */
  uint32_t freq_min;   /* Minimum frequency in Hz */
  uint32_t freq_max;   /* Maximum frequency in Hz */
  uint32_t hddat_min;  /* Minimum data hold time in ns */
  uint32_t vddat_max;  /* Maximum data valid time in ns */
  uint32_t sudat_min;  /* Minimum data setup time in ns */
  uint32_t lscl_min;   /* Minimum low period of the SCL clock in ns */
  uint32_t hscl_min;   /* Minimum high period of SCL clock in ns */
  uint32_t trise;      /* Rise time in ns */
  uint32_t tfall;      /* Fall time in ns */
  uint32_t dnf;        /* Digital noise filter coefficient */
} I2C_Charac_t;

typedef struct
{
  uint32_t presc;      /* Timing prescaler */
  uint32_t tscldel;    /* SCL delay */
  uint32_t tsdadel;    /* SDA delay */
  uint32_t sclh;       /* SCL high period */
  uint32_t scll;       /* SCL low period */
} I2C_Timings_t;
/**
  * @}
  */

#if defined (CORE_CA35) || defined (CORE_CM33)
/** @defgroup STM32MP257F_DK_BUS_Private_Constants BUS Private Constants
  * @{
  */
static const I2C_Charac_t I2C_Charac[] =
{
  [I2C_SPEED_FREQ_STANDARD] =
  {
    .freq = 100000,
    .freq_min = 80000,
    .freq_max = 120000,
    .hddat_min = 0,
    .vddat_max = 3450,
    .sudat_min = 250,
    .lscl_min = 4700,
    .hscl_min = 4000,
    .trise = 640,
    .tfall = 20,
    .dnf = I2C_DIGITAL_FILTER_COEF,
  },
  [I2C_SPEED_FREQ_FAST] =
  {
    .freq = 400000,
    .freq_min = 320000,
    .freq_max = 480000,
    .hddat_min = 0,
    .vddat_max = 900,
    .sudat_min = 100,
    .lscl_min = 1300,
    .hscl_min = 600,
    .trise = 250,
    .tfall = 100,
    .dnf = I2C_DIGITAL_FILTER_COEF,
  },
  [I2C_SPEED_FREQ_FAST_PLUS] =
  {
    .freq = 1000000,
    .freq_min = 800000,
    .freq_max = 1200000,
    .hddat_min = 0,
    .vddat_max = 450,
    .sudat_min = 50,
    .lscl_min = 500,
    .hscl_min = 260,
    .trise = 60,
    .tfall = 100,
    .dnf = I2C_DIGITAL_FILTER_COEF,
  },
};
/**
  * @}
  */

/** @defgroup STM32MP257F_DK_BUS_Private_Variables BUS Private Variables
  * @{
  */
#if (USE_HAL_I2C_REGISTER_CALLBACKS > 0)
static uint32_t IsI2c1MspCbValid = 0;
static uint32_t IsI2c2MspCbValid = 0;
static uint32_t IsI2c7MspCbValid = 0;
#endif /* USE_HAL_I2C_REGISTER_CALLBACKS */

static uint32_t      I2c1InitCounter = 0;
static uint32_t      I2c2InitCounter = 0;
static uint32_t      I2c7InitCounter = 0;
static I2C_Timings_t I2c_valid_timing[I2C_VALID_TIMING_NBR];
static uint32_t      I2c_valid_timing_nbr = 0;
#if defined(BSP_USE_CMSIS_OS)
static osSemaphoreId BspI2cSemaphore = 0;
#endif /* BSP_USE_CMSIS_OS */
#endif /* CORE_CA35 || CORE_CM33 */
/**
  * @}
  */

/** @defgroup STM32MP257F_DK_BUS_Exported_Variables BUS Exported Variables
  * @{
  */
I2C_HandleTypeDef hbus_i2c1;
I2C_HandleTypeDef hbus_i2c2;
I2C_HandleTypeDef hbus_i2c7;
/**
  * @}
  */

/** @defgroup STM32MP257F_DK_BUS_Private_FunctionPrototypes BUS Private FunctionPrototypes
  * @{
  */
#if defined (CORE_CA35) || defined (CORE_CM33)
static void     I2C1_MspInit(I2C_HandleTypeDef *hI2c);
static void     I2C1_MspDeInit(I2C_HandleTypeDef *hI2c);
static int32_t  I2C1_WriteReg(uint16_t DevAddr, uint16_t MemAddSize, uint16_t Reg, uint8_t *pData, uint16_t Length);
static int32_t  I2C1_ReadReg(uint16_t DevAddr, uint16_t MemAddSize, uint16_t Reg, uint8_t *pData, uint16_t Length);
static int32_t  I2C1_Recv(uint16_t DevAddr, uint8_t *pData, uint16_t Length);
static int32_t  I2C1_Send(uint16_t DevAddr, uint8_t *pData, uint16_t Length);

static void     I2C2_MspInit(I2C_HandleTypeDef *hI2c);
static void     I2C2_MspDeInit(I2C_HandleTypeDef *hI2c);
static int32_t  I2C2_WriteReg(uint16_t DevAddr, uint16_t MemAddSize, uint16_t Reg, uint8_t *pData, uint16_t Length);
static int32_t  I2C2_ReadReg(uint16_t DevAddr, uint16_t MemAddSize, uint16_t Reg, uint8_t *pData, uint16_t Length);
static int32_t  I2C2_Recv(uint16_t DevAddr, uint8_t *pData, uint16_t Length);
static int32_t  I2C2_Send(uint16_t DevAddr, uint8_t *pData, uint16_t Length);

static void     I2C7_MspInit(I2C_HandleTypeDef *hI2c);
static void     I2C7_MspDeInit(I2C_HandleTypeDef *hI2c);
static int32_t  I2C7_WriteReg(uint16_t DevAddr, uint16_t MemAddSize, uint16_t Reg, uint8_t *pData, uint16_t Length);
static int32_t  I2C7_ReadReg(uint16_t DevAddr, uint16_t MemAddSize, uint16_t Reg, uint8_t *pData, uint16_t Length);
static int32_t  I2C7_Recv(uint16_t DevAddr, uint8_t *pData, uint16_t Length);
static int32_t  I2C7_Send(uint16_t DevAddr, uint8_t *pData, uint16_t Length);

static uint32_t I2C_GetTiming(uint32_t clock_src_freq, uint32_t i2c_freq);
static uint32_t I2C_Compute_SCLL_SCLH(uint32_t clock_src_freq, uint32_t I2C_speed);
static void     I2C_Compute_PRESC_SCLDEL_SDADEL(uint32_t clock_src_freq, uint32_t I2C_speed);
#endif /* CORE_CA35 || CORE_CM33 */

/**
  * @}
  */

/** @defgroup STM32MP257F_DK_BUS_Exported_Functions BUS Exported Functions
  * @{
  */
#if defined (CORE_CA35) || defined (CORE_CM33)

/**
  * @brief  Initializes I2C HAL.
  * @retval BSP status
  */
int32_t BSP_I2C1_Init(void)
{
  int32_t ret = BSP_ERROR_NONE;

  hbus_i2c1.Instance = BUS_I2C1;

  if (I2c1InitCounter == 0U)
  {
    I2c1InitCounter++;

    if (HAL_I2C_GetState(&hbus_i2c1) == HAL_I2C_STATE_RESET)
    {
#if defined(BSP_USE_CMSIS_OS)
      if (BspI2cSemaphore == NULL)
      {
        /* Create semaphore to prevent multiple I2C access */
        osSemaphoreDef(BSP_I2C_SEM);
        BspI2cSemaphore = osSemaphoreCreate(osSemaphore(BSP_I2C_SEM), 1);
      }
#endif /* BSP_USE_CMSIS_OS */
#if (USE_HAL_I2C_REGISTER_CALLBACKS == 0)
      /* Init the I2C1 Msp */
      I2C1_MspInit(&hbus_i2c1);
#else
      if (IsI2c1MspCbValid == 0U)
      {
        if (BSP_I2C1_RegisterDefaultMspCallbacks() != BSP_ERROR_NONE)
        {
          ret = BSP_ERROR_MSP_FAILURE;
        }
      }
      if (ret == BSP_ERROR_NONE)
      {
#endif /* USE_HAL_I2C_REGISTER_CALLBACKS */
      if (MX_I2C1_Init(&hbus_i2c1,
                       I2C_GetTiming(HAL_RCC_GetFreq(RCC_CLOCKTYPE_ICN_HS_MCU), BUS_I2C1_FREQUENCY)) != HAL_OK)
      {
        ret = BSP_ERROR_BUS_FAILURE;
      }
#if (USE_HAL_I2C_REGISTER_CALLBACKS > 0)
    }
#endif /* USE_HAL_I2C_REGISTER_CALLBACKS */
  }
}
return ret;
}

/**
  * @brief  DeInitializes I2C HAL.
  * @retval BSP status
  */
int32_t BSP_I2C1_DeInit(void)
{
  int32_t ret  = BSP_ERROR_NONE;

  I2c1InitCounter--;

  if (I2c1InitCounter == 0U)
  {
#if (USE_HAL_I2C_REGISTER_CALLBACKS == 0)
    I2C1_MspDeInit(&hbus_i2c1);
#endif /* (USE_HAL_I2C_REGISTER_CALLBACKS == 0) */

    /* Init the I2C */
    if (HAL_I2C_DeInit(&hbus_i2c1) != HAL_OK)
    {
      ret = BSP_ERROR_BUS_FAILURE;
    }
  }

  return ret;
}

/**
  * @brief  MX I2C1 initialization.
  * @param  hI2c I2C handle
  * @param  timing I2C timing
  * @retval HAL status
  */
__weak HAL_StatusTypeDef MX_I2C1_Init(I2C_HandleTypeDef *hI2c, uint32_t timing)
{
  HAL_StatusTypeDef status = HAL_OK;

  hI2c->Init.Timing           = timing;
  hI2c->Init.OwnAddress1      = 0;
  hI2c->Init.AddressingMode   = I2C_ADDRESSINGMODE_7BIT;
  hI2c->Init.DualAddressMode  = I2C_DUALADDRESS_DISABLE;
  hI2c->Init.OwnAddress2      = 0;
  hI2c->Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hI2c->Init.GeneralCallMode  = I2C_GENERALCALL_DISABLE;
  hI2c->Init.NoStretchMode    = I2C_NOSTRETCH_DISABLE;

  if (HAL_I2C_Init(hI2c) != HAL_OK)
  {
    status = HAL_ERROR;
  }
  else
  {
    uint32_t analog_filter;

    analog_filter = I2C_ANALOGFILTER_ENABLE;
    if (HAL_I2CEx_ConfigAnalogFilter(hI2c, analog_filter) != HAL_OK)
    {
      status = HAL_ERROR;
    }
    else
    {
      if (HAL_I2CEx_ConfigDigitalFilter(hI2c, I2C_DIGITAL_FILTER_COEF) != HAL_OK)
      {
        status = HAL_ERROR;
      }
    }
  }

  return status;
}

/**
  * @brief  Write a 8bit value in a register of the device through BUS.
  * @param  DevAddr Device address on Bus.
  * @param  Reg    The target register address to write
  * @param  pData  The target register value to be written
  * @param  Length buffer size to be written
  * @retval BSP status
  */
int32_t BSP_I2C1_WriteReg(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length)
{
  int32_t ret;

#if defined(BSP_USE_CMSIS_OS)
  /* Get semaphore to prevent multiple I2C access */
  osSemaphoreWait(BspI2cSemaphore, osWaitForever);
#endif /* BSP_USE_CMSIS_OS */
  if (I2C1_WriteReg(DevAddr, Reg, I2C_MEMADD_SIZE_8BIT, pData, Length) == 0)
  {
    ret = BSP_ERROR_NONE;
  }
  else
  {
    if (HAL_I2C_GetError(&hbus_i2c1) == HAL_I2C_ERROR_AF)
    {
      ret = BSP_ERROR_BUS_ACKNOWLEDGE_FAILURE;
    }
    else
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
  }
#if defined(BSP_USE_CMSIS_OS)
  /* Release semaphore to prevent multiple I2C access */
  osSemaphoreRelease(BspI2cSemaphore);
#endif /* BSP_USE_CMSIS_OS */

  return ret;
}

/**
  * @brief  Read a 8bit register of the device through BUS
  * @param  DevAddr Device address on BUS
  * @param  Reg     The target register address to read
  * @param  pData   Pointer to data buffer
  * @param  Length  Length of the data
  * @retval BSP status
  */
int32_t BSP_I2C1_ReadReg(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length)
{
  int32_t ret;

#if defined(BSP_USE_CMSIS_OS)
  /* Get semaphore to prevent multiple I2C access */
  osSemaphoreWait(BspI2cSemaphore, osWaitForever);
#endif /* BSP_USE_CMSIS_OS */
  if (I2C1_ReadReg(DevAddr, Reg, I2C_MEMADD_SIZE_8BIT, pData, Length) == 0)
  {
    ret = BSP_ERROR_NONE;
  }
  else
  {
    if (HAL_I2C_GetError(&hbus_i2c1) == HAL_I2C_ERROR_AF)
    {
      ret = BSP_ERROR_BUS_ACKNOWLEDGE_FAILURE;
    }
    else
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
  }
#if defined(BSP_USE_CMSIS_OS)
  /* Release semaphore to prevent multiple I2C access */
  osSemaphoreRelease(BspI2cSemaphore);
#endif /* BSP_USE_CMSIS_OS */

  return ret;
}

/**
  * @brief  Write a 16bit value in a register of the device through BUS.
  * @param  DevAddr Device address on Bus.
  * @param  Reg    The target register address to write
  * @param  pData  The target register value to be written
  * @param  Length buffer size to be written
  * @retval BSP status
  */
int32_t BSP_I2C1_WriteReg16(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length)
{
  int32_t ret;

#if defined(BSP_USE_CMSIS_OS)
  /* Get semaphore to prevent multiple I2C access */
  osSemaphoreWait(BspI2cSemaphore, osWaitForever);
#endif /* BSP_USE_CMSIS_OS */
  if (I2C1_WriteReg(DevAddr, Reg, I2C_MEMADD_SIZE_16BIT, pData, Length) == 0)
  {
    ret = BSP_ERROR_NONE;
  }
  else
  {
    if (HAL_I2C_GetError(&hbus_i2c1) == HAL_I2C_ERROR_AF)
    {
      ret = BSP_ERROR_BUS_ACKNOWLEDGE_FAILURE;
    }
    else
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
  }
#if defined(BSP_USE_CMSIS_OS)
  /* Release semaphore to prevent multiple I2C access */
  osSemaphoreRelease(BspI2cSemaphore);
#endif /* BSP_USE_CMSIS_OS */

  return ret;
}

/**
  * @brief  Read a 16bit register of the device through BUS
  * @param  DevAddr Device address on BUS
  * @param  Reg     The target register address to read
  * @param  pData   Pointer to data buffer
  * @param  Length  Length of the data
  * @retval BSP status
  */
int32_t BSP_I2C1_ReadReg16(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length)
{
  int32_t ret;

#if defined(BSP_USE_CMSIS_OS)
  /* Get semaphore to prevent multiple I2C access */
  osSemaphoreWait(BspI2cSemaphore, osWaitForever);
#endif /* BSP_USE_CMSIS_OS */
  if (I2C1_ReadReg(DevAddr, Reg, I2C_MEMADD_SIZE_16BIT, pData, Length) == 0)
  {
    ret = BSP_ERROR_NONE;
  }
  else
  {
    if (HAL_I2C_GetError(&hbus_i2c1) == HAL_I2C_ERROR_AF)
    {
      ret = BSP_ERROR_BUS_ACKNOWLEDGE_FAILURE;
    }
    else
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
  }
#if defined(BSP_USE_CMSIS_OS)
  /* Release semaphore to prevent multiple I2C access */
  osSemaphoreRelease(BspI2cSemaphore);
#endif /* BSP_USE_CMSIS_OS */

  return ret;
}

/**
  * @brief  Read data
  * @param  DevAddr Device address on BUS
  * @param  pData   Pointer to data buffer
  * @param  Length  Length of the data
  * @retval BSP status
  */
int32_t BSP_I2C1_Recv(uint16_t DevAddr, uint8_t *pData, uint16_t Length)
{
  int32_t ret;

#if defined(BSP_USE_CMSIS_OS)
  /* Get semaphore to prevent multiple I2C access */
  osSemaphoreWait(BspI2cSemaphore, osWaitForever);
#endif /* BSP_USE_CMSIS_OS */
  if (I2C1_Recv(DevAddr, pData, Length) == 0)
  {
    ret = BSP_ERROR_NONE;
  }
  else
  {
    if (HAL_I2C_GetError(&hbus_i2c1) == HAL_I2C_ERROR_AF)
    {
      ret = BSP_ERROR_BUS_ACKNOWLEDGE_FAILURE;
    }
    else
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
  }
#if defined(BSP_USE_CMSIS_OS)
  /* Release semaphore to prevent multiple I2C access */
  osSemaphoreRelease(BspI2cSemaphore);
#endif /* BSP_USE_CMSIS_OS */

  return ret;
}

/**
  * @brief  Send data
  * @param  DevAddr Device address on BUS
  * @param  pData   Pointer to data buffer
  * @param  Length  Length of the data
  * @retval BSP status
  */
int32_t BSP_I2C1_Send(uint16_t DevAddr, uint8_t *pData, uint16_t Length)
{
  int32_t ret;

#if defined(BSP_USE_CMSIS_OS)
  /* Get semaphore to prevent multiple I2C access */
  osSemaphoreWait(BspI2cSemaphore, osWaitForever);
#endif /* BSP_USE_CMSIS_OS */
  if (I2C1_Send(DevAddr, pData, Length) == 0)
  {
    ret = BSP_ERROR_NONE;
  }
  else
  {
    if (HAL_I2C_GetError(&hbus_i2c1) == HAL_I2C_ERROR_AF)
    {
      ret = BSP_ERROR_BUS_ACKNOWLEDGE_FAILURE;
    }
    else
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
  }
#if defined(BSP_USE_CMSIS_OS)
  /* Release semaphore to prevent multiple I2C access */
  osSemaphoreRelease(BspI2cSemaphore);
#endif /* BSP_USE_CMSIS_OS */

  return ret;
}

/**
  * @brief  Checks if target device is ready for communication.
  * @note   This function is used with Memory devices
  * @param  DevAddr  Target device address
  * @param  Trials      Number of trials
  * @retval BSP status
  */
int32_t BSP_I2C1_IsReady(uint16_t DevAddr, uint32_t Trials)
{
  int32_t ret = BSP_ERROR_NONE;

#if defined(BSP_USE_CMSIS_OS)
  /* Get semaphore to prevent multiple I2C access */
  osSemaphoreWait(BspI2cSemaphore, osWaitForever);
#endif /* BSP_USE_CMSIS_OS */
  if (HAL_I2C_IsDeviceReady(&hbus_i2c1, DevAddr, Trials, 1000) != HAL_OK)
  {
    ret = BSP_ERROR_BUSY;
  }
#if defined(BSP_USE_CMSIS_OS)
  /* Release semaphore to prevent multiple I2C access */
  osSemaphoreRelease(BspI2cSemaphore);
#endif /* BSP_USE_CMSIS_OS */

  return ret;
}

/**
  * @brief  Initializes I2C HAL.
  * @retval BSP status
  */
int32_t BSP_I2C2_Init(void)
{
  int32_t ret = BSP_ERROR_NONE;

  hbus_i2c2.Instance = BUS_I2C2;

  if (I2c2InitCounter == 0U)
  {
    I2c2InitCounter++;

    if (HAL_I2C_GetState(&hbus_i2c2) == HAL_I2C_STATE_RESET)
    {
#if defined(BSP_USE_CMSIS_OS)
      if (BspI2cSemaphore == NULL)
      {
        /* Create semaphore to prevent multiple I2C access */
        osSemaphoreDef(BSP_I2C_SEM);
        BspI2cSemaphore = osSemaphoreCreate(osSemaphore(BSP_I2C_SEM), 1);
      }
#endif /* BSP_USE_CMSIS_OS */
#if (USE_HAL_I2C_REGISTER_CALLBACKS == 0)
      /* Init the I2C2 Msp */
      I2C2_MspInit(&hbus_i2c2);
#else
      if (IsI2c2MspCbValid == 0U)
      {
        if (BSP_I2C2_RegisterDefaultMspCallbacks() != BSP_ERROR_NONE)
        {
          ret = BSP_ERROR_MSP_FAILURE;
        }
      }
      if (ret == BSP_ERROR_NONE)
      {
#endif /* BSP_USE_CMSIS_OS */
      if (MX_I2C2_Init(&hbus_i2c2,
                       I2C_GetTiming(HAL_RCC_GetFreq(RCC_CLOCKTYPE_ICN_HS_MCU), BUS_I2C2_FREQUENCY)) != HAL_OK)
      {
        ret = BSP_ERROR_BUS_FAILURE;
      }
#if (USE_HAL_I2C_REGISTER_CALLBACKS > 0)
    }
#endif /* USE_HAL_I2C_REGISTER_CALLBACKS */
  }
}
return ret;
}

/**
  * @brief  DeInitializes I2C HAL.
  * @retval BSP status
  */
int32_t BSP_I2C2_DeInit(void)
{
  int32_t ret  = BSP_ERROR_NONE;

  I2c2InitCounter--;

  if (I2c2InitCounter == 0U)
  {
#if (USE_HAL_I2C_REGISTER_CALLBACKS == 0)
    I2C2_MspDeInit(&hbus_i2c2);
#endif /* (USE_HAL_I2C_REGISTER_CALLBACKS == 0) */

    /* Init the I2C */
    if (HAL_I2C_DeInit(&hbus_i2c2) != HAL_OK)
    {
      ret = BSP_ERROR_BUS_FAILURE;
    }
  }

  return ret;
}

/**
  * @brief  MX I2C2 initialization.
  * @param  hI2c I2C handle
  * @param  timing I2C timing
  * @retval HAL status
  */
__weak HAL_StatusTypeDef MX_I2C2_Init(I2C_HandleTypeDef *hI2c, uint32_t timing)
{
  HAL_StatusTypeDef status = HAL_OK;

  hI2c->Init.Timing           = timing;
  hI2c->Init.OwnAddress1      = 0x00;
  hI2c->Init.AddressingMode   = I2C_ADDRESSINGMODE_7BIT;
  hI2c->Init.DualAddressMode  = I2C_DUALADDRESS_DISABLE;
  hI2c->Init.OwnAddress2      = 0;
  hI2c->Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hI2c->Init.GeneralCallMode  = I2C_GENERALCALL_DISABLE;
  hI2c->Init.NoStretchMode    = I2C_NOSTRETCH_DISABLE;

  if (HAL_I2C_Init(hI2c) != HAL_OK)
  {
    status = HAL_ERROR;
  }
  else
  {
    uint32_t analog_filter;

    analog_filter = I2C_ANALOGFILTER_ENABLE;
    if (HAL_I2CEx_ConfigAnalogFilter(hI2c, analog_filter) != HAL_OK)
    {
      status = HAL_ERROR;
    }
    else
    {
      if (HAL_I2CEx_ConfigDigitalFilter(hI2c, I2C_DIGITAL_FILTER_COEF) != HAL_OK)
      {
        status = HAL_ERROR;
      }
    }
  }

  return status;
}

/**
  * @brief  Write a 8bit value in a register of the device through BUS.
  * @param  DevAddr Device address on Bus.
  * @param  Reg    The target register address to write
  * @param  pData  The target register value to be written
  * @param  Length buffer size to be written
  * @retval BSP status
  */
int32_t BSP_I2C2_WriteReg(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length)
{
  int32_t ret;

#if defined(BSP_USE_CMSIS_OS)
  /* Get semaphore to prevent multiple I2C access */
  osSemaphoreWait(BspI2cSemaphore, osWaitForever);
#endif /* BSP_USE_CMSIS_OS */
  if (I2C2_WriteReg(DevAddr, Reg, I2C_MEMADD_SIZE_8BIT, pData, Length) == 0)
  {
    ret = BSP_ERROR_NONE;
  }
  else
  {
    if (HAL_I2C_GetError(&hbus_i2c2) == HAL_I2C_ERROR_AF)
    {
      ret = BSP_ERROR_BUS_ACKNOWLEDGE_FAILURE;
    }
    else
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
  }
#if defined(BSP_USE_CMSIS_OS)
  /* Release semaphore to prevent multiple I2C access */
  osSemaphoreRelease(BspI2cSemaphore);
#endif /* BSP_USE_CMSIS_OS */

  return ret;
}

/**
  * @brief  Read a 8bit register of the device through BUS
  * @param  DevAddr Device address on BUS
  * @param  Reg     The target register address to read
  * @param  pData   Pointer to data buffer
  * @param  Length  Length of the data
  * @retval BSP status
  */
int32_t BSP_I2C2_ReadReg(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length)
{
  int32_t ret;

#if defined(BSP_USE_CMSIS_OS)
  /* Get semaphore to prevent multiple I2C access */
  osSemaphoreWait(BspI2cSemaphore, osWaitForever);
#endif /* BSP_USE_CMSIS_OS */
  if (I2C2_ReadReg(DevAddr, Reg, I2C_MEMADD_SIZE_8BIT, pData, Length) == 0)
  {
    ret = BSP_ERROR_NONE;
  }
  else
  {
    if (HAL_I2C_GetError(&hbus_i2c2) == HAL_I2C_ERROR_AF)
    {
      ret = BSP_ERROR_BUS_ACKNOWLEDGE_FAILURE;
    }
    else
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
  }
#if defined(BSP_USE_CMSIS_OS)
  /* Release semaphore to prevent multiple I2C access */
  osSemaphoreRelease(BspI2cSemaphore);
#endif /* BSP_USE_CMSIS_OS */

  return ret;
}

/**
  * @brief  Write a 16bit value in a register of the device through BUS.
  * @param  DevAddr Device address on Bus.
  * @param  Reg    The target register address to write
  * @param  pData  The target register value to be written
  * @param  Length buffer size to be written
  * @retval BSP status
  */
int32_t BSP_I2C2_WriteReg16(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length)
{
  int32_t ret;

#if defined(BSP_USE_CMSIS_OS)
  /* Get semaphore to prevent multiple I2C access */
  osSemaphoreWait(BspI2cSemaphore, osWaitForever);
#endif /* BSP_USE_CMSIS_OS */
  if (I2C2_WriteReg(DevAddr, Reg, I2C_MEMADD_SIZE_16BIT, pData, Length) == 0)
  {
    ret = BSP_ERROR_NONE;
  }
  else
  {
    if (HAL_I2C_GetError(&hbus_i2c2) == HAL_I2C_ERROR_AF)
    {
      ret = BSP_ERROR_BUS_ACKNOWLEDGE_FAILURE;
    }
    else
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
  }
#if defined(BSP_USE_CMSIS_OS)
  /* Release semaphore to prevent multiple I2C access */
  osSemaphoreRelease(BspI2cSemaphore);
#endif /* BSP_USE_CMSIS_OS */

  return ret;
}

/**
  * @brief  Read a 16bit register of the device through BUS
  * @param  DevAddr Device address on BUS
  * @param  Reg     The target register address to read
  * @param  pData   Pointer to data buffer
  * @param  Length  Length of the data
  * @retval BSP status
  */
int32_t BSP_I2C2_ReadReg16(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length)
{
  int32_t ret;

#if defined(BSP_USE_CMSIS_OS)
  /* Get semaphore to prevent multiple I2C access */
  osSemaphoreWait(BspI2cSemaphore, osWaitForever);
#endif /* BSP_USE_CMSIS_OS */
  if (I2C2_ReadReg(DevAddr, Reg, I2C_MEMADD_SIZE_16BIT, pData, Length) == 0)
  {
    ret = BSP_ERROR_NONE;
  }
  else
  {
    if (HAL_I2C_GetError(&hbus_i2c2) == HAL_I2C_ERROR_AF)
    {
      ret = BSP_ERROR_BUS_ACKNOWLEDGE_FAILURE;
    }
    else
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
  }
#if defined(BSP_USE_CMSIS_OS)
  /* Release semaphore to prevent multiple I2C access */
  osSemaphoreRelease(BspI2cSemaphore);
#endif /* BSP_USE_CMSIS_OS */

  return ret;
}

/**
  * @brief  Read data
  * @param  DevAddr Device address on BUS
  * @param  pData   Pointer to data buffer
  * @param  Length  Length of the data
  * @retval BSP status
  */
int32_t BSP_I2C2_Recv(uint16_t DevAddr, uint8_t *pData, uint16_t Length)
{
  int32_t ret;

#if defined(BSP_USE_CMSIS_OS)
  /* Get semaphore to prevent multiple I2C access */
  osSemaphoreWait(BspI2cSemaphore, osWaitForever);
#endif /* BSP_USE_CMSIS_OS */
  if (I2C2_Recv(DevAddr, pData, Length) == 0)
  {
    ret = BSP_ERROR_NONE;
  }
  else
  {
    if (HAL_I2C_GetError(&hbus_i2c2) == HAL_I2C_ERROR_AF)
    {
      ret = BSP_ERROR_BUS_ACKNOWLEDGE_FAILURE;
    }
    else
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
  }
#if defined(BSP_USE_CMSIS_OS)
  /* Release semaphore to prevent multiple I2C access */
  osSemaphoreRelease(BspI2cSemaphore);
#endif /* BSP_USE_CMSIS_OS */

  return ret;
}

/**
  * @brief  Send data
  * @param  DevAddr Device address on BUS
  * @param  pData   Pointer to data buffer
  * @param  Length  Length of the data
  * @retval BSP status
  */
int32_t BSP_I2C2_Send(uint16_t DevAddr, uint8_t *pData, uint16_t Length)
{
  int32_t ret;

#if defined(BSP_USE_CMSIS_OS)
  /* Get semaphore to prevent multiple I2C access */
  osSemaphoreWait(BspI2cSemaphore, osWaitForever);
#endif /* BSP_USE_CMSIS_OS */
  if (I2C2_Send(DevAddr, pData, Length) == 0)
  {
    ret = BSP_ERROR_NONE;
  }
  else
  {
    if (HAL_I2C_GetError(&hbus_i2c2) == HAL_I2C_ERROR_AF)
    {
      ret = BSP_ERROR_BUS_ACKNOWLEDGE_FAILURE;
    }
    else
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
  }
#if defined(BSP_USE_CMSIS_OS)
  /* Release semaphore to prevent multiple I2C access */
  osSemaphoreRelease(BspI2cSemaphore);
#endif /* BSP_USE_CMSIS_OS */

  return ret;
}

/**
  * @brief  Checks if target device is ready for communication.
  * @note   This function is used with Memory devices
  * @param  DevAddr  Target device address
  * @param  Trials      Number of trials
  * @retval BSP status
  */
int32_t BSP_I2C2_IsReady(uint16_t DevAddr, uint32_t Trials)
{
  int32_t ret = BSP_ERROR_NONE;

#if defined(BSP_USE_CMSIS_OS)
  /* Get semaphore to prevent multiple I2C access */
  osSemaphoreWait(BspI2cSemaphore, osWaitForever);
#endif /* BSP_USE_CMSIS_OS */
  if (HAL_I2C_IsDeviceReady(&hbus_i2c2, DevAddr, Trials, 1000) != HAL_OK)
  {
    ret = BSP_ERROR_BUSY;
  }
#if defined(BSP_USE_CMSIS_OS)
  /* Release semaphore to prevent multiple I2C access */
  osSemaphoreRelease(BspI2cSemaphore);
#endif /* BSP_USE_CMSIS_OS */

  return ret;
}

/**
  * @brief  Initializes I2C HAL.
  * @retval BSP status
  */
int32_t BSP_I2C7_Init(void)
{
  int32_t ret = BSP_ERROR_NONE;

  hbus_i2c7.Instance = BUS_I2C7;

  if (I2c7InitCounter == 0U)
  {
    I2c7InitCounter++;

    if (HAL_I2C_GetState(&hbus_i2c7) == HAL_I2C_STATE_RESET)
    {
#if defined(BSP_USE_CMSIS_OS)
      if (BspI2cSemaphore == NULL)
      {
        /* Create semaphore to prevent multiple I2C access */
        osSemaphoreDef(BSP_I2C_SEM);
        BspI2cSemaphore = osSemaphoreCreate(osSemaphore(BSP_I2C_SEM), 1);
      }
#endif /* BSP_USE_CMSIS_OS */
#if (USE_HAL_I2C_REGISTER_CALLBACKS == 0)
      /* Init the I2C7 Msp */
      I2C7_MspInit(&hbus_i2c7);
#else
      if (IsI2c7MspCbValid == 0U)
      {
        if (BSP_I2C7_RegisterDefaultMspCallbacks() != BSP_ERROR_NONE)
        {
          ret = BSP_ERROR_MSP_FAILURE;
        }
      }
      if (ret == BSP_ERROR_NONE)
      {
#endif /* USE_HAL_I2C_REGISTER_CALLBACKS */
      if (MX_I2C7_Init(&hbus_i2c7,
                       I2C_GetTiming(HAL_RCC_GetFreq(RCC_CLOCKTYPE_ICN_HS_MCU), BUS_I2C7_FREQUENCY)) != HAL_OK)
      {
        ret = BSP_ERROR_BUS_FAILURE;
      }
#if (USE_HAL_I2C_REGISTER_CALLBACKS > 0)
    }
#endif /* USE_HAL_I2C_REGISTER_CALLBACKS */
  }
}
return ret;
}

/**
  * @brief  DeInitializes I2C HAL.
  * @retval BSP status
  */
int32_t BSP_I2C7_DeInit(void)
{
  int32_t ret  = BSP_ERROR_NONE;

  I2c7InitCounter--;

  if (I2c7InitCounter == 0U)
  {
#if (USE_HAL_I2C_REGISTER_CALLBACKS == 0)
    I2C7_MspDeInit(&hbus_i2c7);
#endif /* (USE_HAL_I2C_REGISTER_CALLBACKS == 0) */

    /* Init the I2C */
    if (HAL_I2C_DeInit(&hbus_i2c7) != HAL_OK)
    {
      ret = BSP_ERROR_BUS_FAILURE;
    }
  }

  return ret;
}

/**
  * @brief  MX I2C7 initialization.
  * @param  hI2c I2C handle
  * @param  timing I2C timing
  * @retval HAL status
  */
__weak HAL_StatusTypeDef MX_I2C7_Init(I2C_HandleTypeDef *hI2c, uint32_t timing)
{
  HAL_StatusTypeDef status = HAL_OK;

  hI2c->Init.Timing           = timing;
  hI2c->Init.OwnAddress1      = 0;
  hI2c->Init.AddressingMode   = I2C_ADDRESSINGMODE_7BIT;
  hI2c->Init.DualAddressMode  = I2C_DUALADDRESS_DISABLE;
  hI2c->Init.OwnAddress2      = 0;
  hI2c->Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hI2c->Init.GeneralCallMode  = I2C_GENERALCALL_DISABLE;
  hI2c->Init.NoStretchMode    = I2C_NOSTRETCH_DISABLE;

  if (HAL_I2C_Init(hI2c) != HAL_OK)
  {
    status = HAL_ERROR;
  }
  else
  {
    uint32_t analog_filter;

    analog_filter = I2C_ANALOGFILTER_ENABLE;
    if (HAL_I2CEx_ConfigAnalogFilter(hI2c, analog_filter) != HAL_OK)
    {
      status = HAL_ERROR;
    }
    else
    {
      if (HAL_I2CEx_ConfigDigitalFilter(hI2c, I2C_DIGITAL_FILTER_COEF) != HAL_OK)
      {
        status = HAL_ERROR;
      }
    }
  }

  return status;
}

/**
  * @brief  Write a 8bit value in a register of the device through BUS.
  * @param  DevAddr Device address on Bus.
  * @param  Reg    The target register address to write
  * @param  pData  The target register value to be written
  * @param  Length buffer size to be written
  * @retval BSP status
  */
int32_t BSP_I2C7_WriteReg(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length)
{
  int32_t ret;

#if defined(BSP_USE_CMSIS_OS)
  /* Get semaphore to prevent multiple I2C access */
  osSemaphoreWait(BspI2cSemaphore, osWaitForever);
#endif /* BSP_USE_CMSIS_OS */
  if (I2C7_WriteReg(DevAddr, Reg, I2C_MEMADD_SIZE_8BIT, pData, Length) == 0)
  {
    ret = BSP_ERROR_NONE;
  }
  else
  {
    if (HAL_I2C_GetError(&hbus_i2c7) == HAL_I2C_ERROR_AF)
    {
      ret = BSP_ERROR_BUS_ACKNOWLEDGE_FAILURE;
    }
    else
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
  }
#if defined(BSP_USE_CMSIS_OS)
  /* Release semaphore to prevent multiple I2C access */
  osSemaphoreRelease(BspI2cSemaphore);
#endif /* BSP_USE_CMSIS_OS */

  return ret;
}

/**
  * @brief  Read a 8bit register of the device through BUS
  * @param  DevAddr Device address on BUS
  * @param  Reg     The target register address to read
  * @param  pData   Pointer to data buffer
  * @param  Length  Length of the data
  * @retval BSP status
  */
int32_t BSP_I2C7_ReadReg(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length)
{
  int32_t ret;

#if defined(BSP_USE_CMSIS_OS)
  /* Get semaphore to prevent multiple I2C access */
  osSemaphoreWait(BspI2cSemaphore, osWaitForever);
#endif /* BSP_USE_CMSIS_OS */
  if (I2C7_ReadReg(DevAddr, Reg, I2C_MEMADD_SIZE_8BIT, pData, Length) == 0)
  {
    ret = BSP_ERROR_NONE;
  }
  else
  {
    if (HAL_I2C_GetError(&hbus_i2c7) == HAL_I2C_ERROR_AF)
    {
      ret = BSP_ERROR_BUS_ACKNOWLEDGE_FAILURE;
    }
    else
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
  }
#if defined(BSP_USE_CMSIS_OS)
  /* Release semaphore to prevent multiple I2C access */
  osSemaphoreRelease(BspI2cSemaphore);
#endif /* BSP_USE_CMSIS_OS */

  return ret;
}

/**
  * @brief  Write a 16bit value in a register of the device through BUS.
  * @param  DevAddr Device address on Bus.
  * @param  Reg    The target register address to write
  * @param  pData  The target register value to be written
  * @param  Length buffer size to be written
  * @retval BSP status
  */
int32_t BSP_I2C7_WriteReg16(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length)
{
  int32_t ret;

#if defined(BSP_USE_CMSIS_OS)
  /* Get semaphore to prevent multiple I2C access */
  osSemaphoreWait(BspI2cSemaphore, osWaitForever);
#endif /* BSP_USE_CMSIS_OS */
  if (I2C7_WriteReg(DevAddr, Reg, I2C_MEMADD_SIZE_16BIT, pData, Length) == 0)
  {
    ret = BSP_ERROR_NONE;
  }
  else
  {
    if (HAL_I2C_GetError(&hbus_i2c7) == HAL_I2C_ERROR_AF)
    {
      ret = BSP_ERROR_BUS_ACKNOWLEDGE_FAILURE;
    }
    else
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
  }
#if defined(BSP_USE_CMSIS_OS)
  /* Release semaphore to prevent multiple I2C access */
  osSemaphoreRelease(BspI2cSemaphore);
#endif /* BSP_USE_CMSIS_OS */

  return ret;
}

/**
  * @brief  Read a 16bit register of the device through BUS
  * @param  DevAddr Device address on BUS
  * @param  Reg     The target register address to read
  * @param  pData   Pointer to data buffer
  * @param  Length  Length of the data
  * @retval BSP status
  */
int32_t BSP_I2C7_ReadReg16(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length)
{
  int32_t ret;

#if defined(BSP_USE_CMSIS_OS)
  /* Get semaphore to prevent multiple I2C access */
  osSemaphoreWait(BspI2cSemaphore, osWaitForever);
#endif /* BSP_USE_CMSIS_OS */
  if (I2C7_ReadReg(DevAddr, Reg, I2C_MEMADD_SIZE_16BIT, pData, Length) == 0)
  {
    ret = BSP_ERROR_NONE;
  }
  else
  {
    if (HAL_I2C_GetError(&hbus_i2c7) == HAL_I2C_ERROR_AF)
    {
      ret = BSP_ERROR_BUS_ACKNOWLEDGE_FAILURE;
    }
    else
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
  }
#if defined(BSP_USE_CMSIS_OS)
  /* Release semaphore to prevent multiple I2C access */
  osSemaphoreRelease(BspI2cSemaphore);
#endif /* BSP_USE_CMSIS_OS */

  return ret;
}

/**
  * @brief  Read data
  * @param  DevAddr Device address on BUS
  * @param  pData   Pointer to data buffer
  * @param  Length  Length of the data
  * @retval BSP status
  */
int32_t BSP_I2C7_Recv(uint16_t DevAddr, uint8_t *pData, uint16_t Length)
{
  int32_t ret;

#if defined(BSP_USE_CMSIS_OS)
  /* Get semaphore to prevent multiple I2C access */
  osSemaphoreWait(BspI2cSemaphore, osWaitForever);
#endif /* BSP_USE_CMSIS_OS */
  if (I2C7_Recv(DevAddr, pData, Length) == 0)
  {
    ret = BSP_ERROR_NONE;
  }
  else
  {
    if (HAL_I2C_GetError(&hbus_i2c7) == HAL_I2C_ERROR_AF)
    {
      ret = BSP_ERROR_BUS_ACKNOWLEDGE_FAILURE;
    }
    else
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
  }
#if defined(BSP_USE_CMSIS_OS)
  /* Release semaphore to prevent multiple I2C access */
  osSemaphoreRelease(BspI2cSemaphore);
#endif /* BSP_USE_CMSIS_OS */

  return ret;
}

/**
  * @brief  Send data
  * @param  DevAddr Device address on BUS
  * @param  pData   Pointer to data buffer
  * @param  Length  Length of the data
  * @retval BSP status
  */
int32_t BSP_I2C7_Send(uint16_t DevAddr, uint8_t *pData, uint16_t Length)
{
  int32_t ret;

#if defined(BSP_USE_CMSIS_OS)
  /* Get semaphore to prevent multiple I2C access */
  osSemaphoreWait(BspI2cSemaphore, osWaitForever);
#endif /* BSP_USE_CMSIS_OS */
  if (I2C7_Send(DevAddr, pData, Length) == 0)
  {
    ret = BSP_ERROR_NONE;
  }
  else
  {
    if (HAL_I2C_GetError(&hbus_i2c7) == HAL_I2C_ERROR_AF)
    {
      ret = BSP_ERROR_BUS_ACKNOWLEDGE_FAILURE;
    }
    else
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
  }
#if defined(BSP_USE_CMSIS_OS)
  /* Release semaphore to prevent multiple I2C access */
  osSemaphoreRelease(BspI2cSemaphore);
#endif /* BSP_USE_CMSIS_OS */

  return ret;
}

/**
  * @brief  Checks if target device is ready for communication.
  * @note   This function is used with Memory devices
  * @param  DevAddr  Target device address
  * @param  Trials      Number of trials
  * @retval BSP status
  */
int32_t BSP_I2C7_IsReady(uint16_t DevAddr, uint32_t Trials)
{
  int32_t ret = BSP_ERROR_NONE;

#if defined(BSP_USE_CMSIS_OS)
  /* Get semaphore to prevent multiple I2C access */
  osSemaphoreWait(BspI2cSemaphore, osWaitForever);
#endif /* BSP_USE_CMSIS_OS */
  if (HAL_I2C_IsDeviceReady(&hbus_i2c7, DevAddr, Trials, 1000) != HAL_OK)
  {
    ret = BSP_ERROR_BUSY;
  }
#if defined(BSP_USE_CMSIS_OS)
  /* Release semaphore to prevent multiple I2C access */
  osSemaphoreRelease(BspI2cSemaphore);
#endif /* BSP_USE_CMSIS_OS */

  return ret;
}

/**
  * @brief  Delay function
  * @retval Tick value
  */
int32_t BSP_GetTick(void)
{
  return (int32_t)HAL_GetTick();
}

/**
  * @}
  */

/** @defgroup STM32MP257F_DK_BUS_Private_Functions BUS Private Functions
  * @{
  */
/**
  * @brief  Compute I2C timing according current I2C clock source and required I2C clock.
  * @param  clock_src_freq I2C clock source in Hz.
  * @param  i2c_freq Required I2C clock in Hz.
  * @retval I2C timing or 0 in case of error.
  */
static uint32_t I2C_GetTiming(uint32_t clock_src_freq, uint32_t i2c_freq)
{
  uint32_t ret = 0;
  uint32_t speed;
  uint32_t idx;

  if ((clock_src_freq != 0U) && (i2c_freq != 0U))
  {
    for (speed = 0 ; speed <= (uint32_t)I2C_SPEED_FREQ_FAST_PLUS ; speed++)
    {
      if ((i2c_freq >= I2C_Charac[speed].freq_min) &&
          (i2c_freq <= I2C_Charac[speed].freq_max))
      {
        I2C_Compute_PRESC_SCLDEL_SDADEL(clock_src_freq, speed);
        idx = I2C_Compute_SCLL_SCLH(clock_src_freq, speed);

        if (idx < I2C_VALID_TIMING_NBR)
        {
          ret = ((I2c_valid_timing[idx].presc  & 0x0FU) << 28) | \
                ((I2c_valid_timing[idx].tscldel & 0x0FU) << 20) | \
                ((I2c_valid_timing[idx].tsdadel & 0x0FU) << 16) | \
                ((I2c_valid_timing[idx].sclh & 0xFFU) << 8) | \
                ((I2c_valid_timing[idx].scll & 0xFFU) << 0);
        }
        break;
      }
    }
  }

  return ret;
}

/**
  * @brief  Compute PRESC, SCLDEL and SDADEL.
  * @param  clock_src_freq I2C source clock in HZ.
  * @param  I2C_speed I2C frequency (index).
  * @retval None.
  */
static void I2C_Compute_PRESC_SCLDEL_SDADEL(uint32_t clock_src_freq, uint32_t I2C_speed)
{
  uint32_t prev_presc = I2C_PRESC_MAX;
  uint32_t ti2cclk;
  int32_t  tsdadel_min;
  int32_t  tsdadel_max;
  int32_t  tscldel_min;
  uint32_t presc;
  uint32_t scldel;
  uint32_t sdadel;
  uint32_t tafdel_min;
  uint32_t tafdel_max;

  ti2cclk   = (SEC2NSEC + (clock_src_freq / 2U)) / clock_src_freq;

  tafdel_min = I2C_ANALOG_FILTER_DELAY_MIN;
  tafdel_max = I2C_ANALOG_FILTER_DELAY_MAX;

  /* tDNF = DNF x tI2CCLK
     tPRESC = (PRESC+1) x tI2CCLK
     SDADEL >= {tf +tHD;DAT(min) - tAF(min) - tDNF - [3 x tI2CCLK]} / {tPRESC}
     SDADEL <= {tVD;DAT(max) - tr - tAF(max) - tDNF- [4 x tI2CCLK]} / {tPRESC} */

  tsdadel_min = (int32_t)I2C_Charac[I2C_speed].tfall + (int32_t)I2C_Charac[I2C_speed].hddat_min -
                (int32_t)tafdel_min - (int32_t)(((int32_t)I2C_Charac[I2C_speed].dnf + 3) * (int32_t)ti2cclk);

  tsdadel_max = (int32_t)I2C_Charac[I2C_speed].vddat_max - (int32_t)I2C_Charac[I2C_speed].trise -
                (int32_t)tafdel_max - (int32_t)(((int32_t)I2C_Charac[I2C_speed].dnf + 4) * (int32_t)ti2cclk);


  /* {[tr+ tSU;DAT(min)] / [tPRESC]} - 1 <= SCLDEL */
  tscldel_min = (int32_t)I2C_Charac[I2C_speed].trise + (int32_t)I2C_Charac[I2C_speed].sudat_min;

  if (tsdadel_min <= 0)
  {
    tsdadel_min = 0;
  }

  if (tsdadel_max <= 0)
  {
    tsdadel_max = 0;
  }

  for (presc = 0; presc < I2C_PRESC_MAX; presc++)
  {
    for (scldel = 0; scldel < I2C_SCLDEL_MAX; scldel++)
    {
      /* TSCLDEL = (SCLDEL+1) * (PRESC+1) * TI2CCLK */
      uint32_t tscldel = (scldel + 1U) * (presc + 1U) * ti2cclk;

      if (tscldel >= (uint32_t)tscldel_min)
      {
        for (sdadel = 0; sdadel < I2C_SDADEL_MAX; sdadel++)
        {
          /* TSDADEL = SDADEL * (PRESC+1) * TI2CCLK */
          uint32_t tsdadel = (sdadel * (presc + 1U)) * ti2cclk;

          if ((tsdadel >= (uint32_t)tsdadel_min) && (tsdadel <= (uint32_t)tsdadel_max))
          {
            if (presc != prev_presc)
            {
              I2c_valid_timing[I2c_valid_timing_nbr].presc = presc;
              I2c_valid_timing[I2c_valid_timing_nbr].tscldel = scldel;
              I2c_valid_timing[I2c_valid_timing_nbr].tsdadel = sdadel;
              prev_presc = presc;
              I2c_valid_timing_nbr ++;

              if (I2c_valid_timing_nbr >= I2C_VALID_TIMING_NBR)
              {
                return;
              }
            }
          }
        }
      }
    }
  }
}

/**
  * @brief  Calculate SCLL and SCLH and find best configuration.
  * @param  clock_src_freq I2C source clock in HZ.
  * @param  I2C_speed I2C frequency (index).
  * @retval config index (0 to I2C_VALID_TIMING_NBR], 0xFFFFFFFF for no valid config.
  */
static uint32_t I2C_Compute_SCLL_SCLH(uint32_t clock_src_freq, uint32_t I2C_speed)
{
  uint32_t ret = 0xFFFFFFFFU;
  uint32_t ti2cclk;
  uint32_t ti2cspeed;
  uint32_t prev_error;
  uint32_t dnf_delay;
  uint32_t clk_min;
  uint32_t clk_max;
  uint32_t scll;
  uint32_t sclh;
  uint32_t tafdel_min;

  ti2cclk   = (SEC2NSEC + (clock_src_freq / 2U)) / clock_src_freq;
  ti2cspeed   = (SEC2NSEC + (I2C_Charac[I2C_speed].freq / 2U)) / I2C_Charac[I2C_speed].freq;

  tafdel_min = I2C_ANALOG_FILTER_DELAY_MIN;

  /* tDNF = DNF x tI2CCLK */
  dnf_delay = I2C_Charac[I2C_speed].dnf * ti2cclk;

  clk_max = SEC2NSEC / I2C_Charac[I2C_speed].freq_min;
  clk_min = SEC2NSEC / I2C_Charac[I2C_speed].freq_max;

  prev_error = ti2cspeed;

  for (uint32_t count = 0; count < I2c_valid_timing_nbr; count++)
  {
    /* tPRESC = (PRESC+1) x tI2CCLK*/
    uint32_t tpresc = (I2c_valid_timing[count].presc + 1U) * ti2cclk;

    for (scll = 0; scll < I2C_SCLL_MAX; scll++)
    {
      /* tLOW(min) <= tAF(min) + tDNF + 2 x tI2CCLK + [(SCLL+1) x tPRESC ] */
      uint32_t tscl_l = tafdel_min + dnf_delay + (2U * ti2cclk) + ((scll + 1U) * tpresc);


      /* The I2CCLK period tI2CCLK must respect the following conditions:
      tI2CCLK < (tLOW - tfilters) / 4 and tI2CCLK < tHIGH */
      if ((tscl_l > I2C_Charac[I2C_speed].lscl_min) && (ti2cclk < ((tscl_l - tafdel_min - dnf_delay) / 4U)))
      {
        for (sclh = 0; sclh < I2C_SCLH_MAX; sclh++)
        {
          /* tHIGH(min) <= tAF(min) + tDNF + 2 x tI2CCLK + [(SCLH+1) x tPRESC] */
          uint32_t tscl_h = tafdel_min + dnf_delay + (2U * ti2cclk) + ((sclh + 1U) * tpresc);

          /* tSCL = tf + tLOW + tr + tHIGH */
          uint32_t tscl = tscl_l + tscl_h + I2C_Charac[I2C_speed].trise + I2C_Charac[I2C_speed].tfall;

          if ((tscl >= clk_min) && (tscl <= clk_max) && (tscl_h >= I2C_Charac[I2C_speed].hscl_min)
              && (ti2cclk < tscl_h))
          {
            int32_t error = (int32_t)tscl - (int32_t)ti2cspeed;

            if (error < 0)
            {
              error = -error;
            }

            /* look for the timings with the lowest clock error */
            if ((uint32_t)error < prev_error)
            {
              prev_error = (uint32_t)error;
              I2c_valid_timing[count].scll = scll;
              I2c_valid_timing[count].sclh = sclh;
              ret = count;
            }
          }
        }
      }
    }
  }

  return ret;
}

/**
  * @brief  Initializes I2C1 MSP.
  * @param  hI2c  I2C handler
  * @retval None
  */
static void I2C1_MspInit(I2C_HandleTypeDef *hI2c)
{
  GPIO_InitTypeDef  gpio_init_structure;

  /* Prevent unused argument(s) compilation warning */
  UNUSED(hI2c);

  /*** Configure the GPIOs ***/

  if (ResMgr_Request(RESMGR_RESOURCE_RIF_RCC, RESMGR_RCC_RESOURCE(96)) == RESMGR_STATUS_ACCESS_OK)
  {
    /* Enable SCL GPIO clock */
    BUS_I2C1_SCL_GPIO_CLK_ENABLE();
  }
  if (ResMgr_Request(RESMGR_RESOURCE_RIF_RCC, RESMGR_RCC_RESOURCE(98)) == RESMGR_STATUS_ACCESS_OK)
  {
    /* Enable SDA GPIO clock */
    BUS_I2C1_SDA_GPIO_CLK_ENABLE();
  }

  /* Acquire I2C1 using Resource manager */
  if (RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIFSC, STM32MP25_RIFSC_I2C1_ID))
  {
    Error_Handler();
  }
  /* Acquire GPIOG13 using Resource manager */
  if (RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIF_GPIOG, RESMGR_GPIO_PIN(13)))
  {
    Error_Handler();
  }
  /* Acquire GPIOI1 using Resource manager */
  if (RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIF_GPIOI, RESMGR_GPIO_PIN(1)))
  {
    Error_Handler();
  }

  /* Configure I2C Tx as alternate function */
  gpio_init_structure.Pin       = BUS_I2C1_SCL_PIN;
  gpio_init_structure.Mode      = GPIO_MODE_AF_OD;
  gpio_init_structure.Pull      = GPIO_PULLUP;
  gpio_init_structure.Speed     = GPIO_SPEED_FREQ_HIGH;
  gpio_init_structure.Alternate = BUS_I2C1_SCL_AF;
  HAL_GPIO_Init(BUS_I2C1_SCL_GPIO_PORT, &gpio_init_structure);

  /* Configure I2C Rx as alternate function */
  gpio_init_structure.Pin       = BUS_I2C1_SDA_PIN;
  gpio_init_structure.Mode      = GPIO_MODE_AF_OD;
  gpio_init_structure.Pull      = GPIO_PULLUP;
  gpio_init_structure.Speed     = GPIO_SPEED_FREQ_HIGH;
  gpio_init_structure.Alternate = BUS_I2C1_SDA_AF;
  HAL_GPIO_Init(BUS_I2C1_SDA_GPIO_PORT, &gpio_init_structure);

  /*** Configure the I2C peripheral ***/
  /* Enable I2C clock */
  BUS_I2C1_CLK_ENABLE();

  /* Force the I2C peripheral clock reset */
  BUS_I2C1_FORCE_RESET();

  /* Release the I2C peripheral clock reset */
  BUS_I2C1_RELEASE_RESET();
}

/**
  * @brief  DeInitializes I2C1 MSP.
  * @param  hI2c  I2C handler
  * @retval None
  */
static void I2C1_MspDeInit(I2C_HandleTypeDef *hI2c)
{
  GPIO_InitTypeDef  gpio_init_structure;

  /* Prevent unused argument(s) compilation warning */
  UNUSED(hI2c);

  /* Configure I2C Tx, Rx as alternate function */
  gpio_init_structure.Pin = BUS_I2C1_SCL_PIN;
  HAL_GPIO_DeInit(BUS_I2C1_SCL_GPIO_PORT, gpio_init_structure.Pin);
  gpio_init_structure.Pin = BUS_I2C1_SDA_PIN;
  HAL_GPIO_DeInit(BUS_I2C1_SDA_GPIO_PORT, gpio_init_structure.Pin);

  /* Disable I2C clock */
  BUS_I2C1_CLK_DISABLE();
  (void)ResMgr_Release(RESMGR_RESOURCE_RIF_GPIOG, RESMGR_GPIO_PIN(13));
  (void)ResMgr_Release(RESMGR_RESOURCE_RIF_GPIOI, RESMGR_GPIO_PIN(1));
  (void)ResMgr_Release(RESMGR_RESOURCE_RIFSC, STM32MP25_RIFSC_I2C1_ID);
}

/**
  * @brief  Write a value in a register of the device through BUS.
  * @param  DevAddr    Device address on Bus.
  * @param  Reg        The target register address to write
  * @param  MemAddSize Size of internal memory address
  * @param  pData      The target register value to be written
  * @param  Length     data length in bytes
  * @retval BSP status
  */
static int32_t I2C1_WriteReg(uint16_t DevAddr, uint16_t Reg, uint16_t MemAddSize, uint8_t *pData, uint16_t Length)
{
  if (HAL_I2C_Mem_Write(&hbus_i2c1, DevAddr, Reg, MemAddSize, pData, Length, 10000) == HAL_OK)
  {
    return BSP_ERROR_NONE;
  }

  return BSP_ERROR_BUS_FAILURE;
}

/**
  * @brief  Read a register of the device through BUS
  * @param  DevAddr    Device address on BUS
  * @param  Reg        The target register address to read
  * @param  MemAddSize Size of internal memory address
  * @param  pData      The target register value to be written
  * @param  Length     data length in bytes
  * @retval BSP status
  */
static int32_t I2C1_ReadReg(uint16_t DevAddr, uint16_t Reg, uint16_t MemAddSize, uint8_t *pData, uint16_t Length)
{
  if (HAL_I2C_Mem_Read(&hbus_i2c1, DevAddr, Reg, MemAddSize, pData, Length, 10000) == HAL_OK)
  {
    return BSP_ERROR_NONE;
  }

  return BSP_ERROR_BUS_FAILURE;
}

/**
  * @brief  Receive data from the device through BUS
  * @param  DevAddr    Device address on BUS
  * @param  pData      The target register value to be received
  * @param  Length     data length in bytes
  * @retval BSP status
  */
static int32_t I2C1_Recv(uint16_t DevAddr, uint8_t *pData, uint16_t Length)
{
  if (HAL_I2C_Master_Receive(&hbus_i2c1, DevAddr, pData, Length, 10000) == HAL_OK)
  {
    return BSP_ERROR_NONE;
  }

  return BSP_ERROR_BUS_FAILURE;
}

/**
  * @brief  Send data to the device through BUS
  * @param  DevAddr    Device address on BUS
  * @param  pData      The target register value to be sent
  * @param  Length     data length in bytes
  * @retval BSP status
  */
static int32_t I2C1_Send(uint16_t DevAddr, uint8_t *pData, uint16_t Length)
{
  if (HAL_I2C_Master_Transmit(&hbus_i2c1, DevAddr, pData, Length, 10000) == HAL_OK)
  {
    return BSP_ERROR_NONE;
  }

  return BSP_ERROR_BUS_FAILURE;
}

/**
  * @brief  Initializes I2C2 MSP.
  * @param  hI2c  I2C handler
  * @retval None
  */
static void I2C2_MspInit(I2C_HandleTypeDef *hI2c)
{
  GPIO_InitTypeDef  gpio_init_structure;

  /* Prevent unused argument(s) compilation warning */
  UNUSED(hI2c);

  /*** Configure the GPIOs ***/
  if (IS_DEVELOPER_BOOT_MODE())
  {
    /* Enable VddIO4 for Port B */
    HAL_PWREx_EnableSupply(PWR_PVM_VDDIO4);
  }

  if (ResMgr_Request(RESMGR_RESOURCE_RIF_RCC, RESMGR_RCC_RESOURCE(91)) == RESMGR_STATUS_ACCESS_OK)
  {
    /* Enable SCL GPIO clock */
    BUS_I2C2_SCL_GPIO_CLK_ENABLE();
  }

  if (ResMgr_Request(RESMGR_RESOURCE_RIF_RCC, RESMGR_RCC_RESOURCE(91)) == RESMGR_STATUS_ACCESS_OK)
  {
    /* Enable SDA GPIO clock */
    BUS_I2C2_SDA_GPIO_CLK_ENABLE();
  }

  /* Configure I2C CLK as alternate function */
  gpio_init_structure.Pin               = BUS_I2C2_SCL_PIN;
  gpio_init_structure.Mode              = GPIO_MODE_AF_OD;
  gpio_init_structure.Pull              = GPIO_NOPULL;
  gpio_init_structure.Speed             = GPIO_SPEED_FREQ_HIGH;
  gpio_init_structure.Alternate         = BUS_I2C2_SCL_AF;
  HAL_GPIO_Init(BUS_I2C2_SCL_GPIO_PORT, &gpio_init_structure);

  /* Configure I2C SDA as alternate function */
  gpio_init_structure.Pin               = BUS_I2C2_SDA_PIN;
  gpio_init_structure.Mode              = GPIO_MODE_AF_OD;
  gpio_init_structure.Pull              = GPIO_NOPULL;
  gpio_init_structure.Speed             = GPIO_SPEED_FREQ_HIGH;
  gpio_init_structure.Alternate         = BUS_I2C2_SDA_AF;
  HAL_GPIO_Init(BUS_I2C2_SDA_GPIO_PORT, &gpio_init_structure);

  /*** Configure the I2C peripheral ***/
  /* Enable I2C clock */
  BUS_I2C2_CLK_ENABLE();

  /* Force the I2C peripheral clock reset */
  BUS_I2C2_FORCE_RESET();

  /* Release the I2C peripheral clock reset */
  BUS_I2C2_RELEASE_RESET();
}

/**
  * @brief  DeInitializes I2C MSP.
  * @param  hI2c  I2C handler
  * @retval None
  */
static void I2C2_MspDeInit(I2C_HandleTypeDef *hI2c)
{
  GPIO_InitTypeDef  gpio_init_structure;

  /* Prevent unused argument(s) compilation warning */
  UNUSED(hI2c);

  /* Configure I2C Tx, Rx as alternate function */
  gpio_init_structure.Pin = BUS_I2C2_SCL_PIN;
  HAL_GPIO_DeInit(BUS_I2C2_SCL_GPIO_PORT, gpio_init_structure.Pin);
  gpio_init_structure.Pin = BUS_I2C2_SDA_PIN;
  HAL_GPIO_DeInit(BUS_I2C2_SDA_GPIO_PORT, gpio_init_structure.Pin);

  /* Disable I2C clock */
  BUS_I2C2_CLK_DISABLE();

  (void)ResMgr_Release(RESMGR_RESOURCE_RIF_GPIOB, RESMGR_GPIO_PIN(4));
  (void)ResMgr_Release(RESMGR_RESOURCE_RIF_GPIOB, RESMGR_GPIO_PIN(5));
  (void)ResMgr_Release(RESMGR_RESOURCE_RIFSC, STM32MP25_RIFSC_I2C2_ID);
}

/**
  * @brief  Write a value in a register of the device through BUS.
  * @param  DevAddr    Device address on Bus.
  * @param  Reg        The target register address to write
  * @param  MemAddSize Size of internal memory address
  * @param  pData      The target register value to be written
  * @param  Length     data length in bytes
  * @retval BSP status
  */
static int32_t I2C2_WriteReg(uint16_t DevAddr, uint16_t Reg, uint16_t MemAddSize, uint8_t *pData, uint16_t Length)
{
  if (HAL_I2C_Mem_Write(&hbus_i2c2, DevAddr, Reg, MemAddSize, pData, Length, 10000) == HAL_OK)
  {
    return BSP_ERROR_NONE;
  }

  return BSP_ERROR_BUS_FAILURE;
}

/**
  * @brief  Read a register of the device through BUS
  * @param  DevAddr    Device address on BUS
  * @param  Reg        The target register address to read
  * @param  MemAddSize Size of internal memory address
  * @param  pData      The target register value to be written
  * @param  Length     data length in bytes
  * @retval BSP status
  */
static int32_t I2C2_ReadReg(uint16_t DevAddr, uint16_t Reg, uint16_t MemAddSize, uint8_t *pData, uint16_t Length)
{
  if (HAL_I2C_Mem_Read(&hbus_i2c2, DevAddr, Reg, MemAddSize, pData, Length, 10000) == HAL_OK)
  {
    return BSP_ERROR_NONE;
  }

  return BSP_ERROR_BUS_FAILURE;
}

/**
  * @brief  Receive data from the device through BUS
  * @param  DevAddr    Device address on BUS
  * @param  pData      The target register value to be received
  * @param  Length     data length in bytes
  * @retval BSP status
  */
static int32_t I2C2_Recv(uint16_t DevAddr, uint8_t *pData, uint16_t Length)
{
  if (HAL_I2C_Master_Receive(&hbus_i2c2, DevAddr, pData, Length, 10000) == HAL_OK)
  {
    return BSP_ERROR_NONE;
  }

  return BSP_ERROR_BUS_FAILURE;
}

/**
  * @brief  Send data to the device through BUS
  * @param  DevAddr    Device address on BUS
  * @param  pData      The target register value to be sent
  * @param  Length     data length in bytes
  * @retval BSP status
  */
static int32_t I2C2_Send(uint16_t DevAddr, uint8_t *pData, uint16_t Length)
{
  if (HAL_I2C_Master_Transmit(&hbus_i2c2, DevAddr, pData, Length, 10000) == HAL_OK)
  {
    return BSP_ERROR_NONE;
  }

  return BSP_ERROR_BUS_FAILURE;
}

/**
  * @brief  Initializes I2C7 MSP.
  * @param  hI2c  I2C handler
  * @retval None
  */
static void I2C7_MspInit(I2C_HandleTypeDef *hI2c)
{
  GPIO_InitTypeDef  gpio_init_structure;

  /* Prevent unused argument(s) compilation warning */
  UNUSED(hI2c);

  /*** Configure the GPIOs ***/

  if (ResMgr_Request(RESMGR_RESOURCE_RIF_RCC, RESMGR_RCC_RESOURCE(93)) == RESMGR_STATUS_ACCESS_OK)
  {
    /* Enable SCL GPIO clock */
    BUS_I2C7_SCL_GPIO_CLK_ENABLE();
  }

  if (ResMgr_Request(RESMGR_RESOURCE_RIF_RCC, RESMGR_RCC_RESOURCE(93)) == RESMGR_STATUS_ACCESS_OK)
  {
    /* Enable SDA GPIO clock */
    BUS_I2C7_SDA_GPIO_CLK_ENABLE();
  }
  /* Acquire I2C7 using Resource manager */
  if (RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIFSC, STM32MP25_RIFSC_I2C7_ID))
  {
    Error_Handler();
  }
  /* Acquire GPIOD15 using Resource manager */
  if (RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIF_GPIOD, RESMGR_GPIO_PIN(15)))
  {
    Error_Handler();
  }
  /* Acquire GPIOD14 using Resource manager */
  if (RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIF_GPIOD, RESMGR_GPIO_PIN(14)))
  {
    Error_Handler();
  }

  /* Configure I2C Tx as alternate function */
  gpio_init_structure.Pin       = BUS_I2C7_SCL_PIN;
  gpio_init_structure.Mode      = GPIO_MODE_AF_OD;
  gpio_init_structure.Pull      = GPIO_PULLUP;
  gpio_init_structure.Speed     = GPIO_SPEED_FREQ_HIGH;
  gpio_init_structure.Alternate = BUS_I2C7_SCL_AF;
  HAL_GPIO_Init(BUS_I2C7_SCL_GPIO_PORT, &gpio_init_structure);

  /* Configure I2C Rx as alternate function */
  gpio_init_structure.Pin       = BUS_I2C7_SDA_PIN;
  gpio_init_structure.Mode      = GPIO_MODE_AF_OD;
  gpio_init_structure.Pull      = GPIO_PULLUP;
  gpio_init_structure.Speed     = GPIO_SPEED_FREQ_HIGH;
  gpio_init_structure.Alternate = BUS_I2C7_SDA_AF;
  HAL_GPIO_Init(BUS_I2C7_SDA_GPIO_PORT, &gpio_init_structure);

  /*** Configure the I2C peripheral ***/
  /* Enable I2C clock */
  BUS_I2C7_CLK_ENABLE();

  /* Force the I2C peripheral clock reset */
  BUS_I2C7_FORCE_RESET();

  /* Release the I2C peripheral clock reset */
  BUS_I2C7_RELEASE_RESET();
}

/**
  * @brief  DeInitializes I2C7 MSP.
  * @param  hI2c  I2C handler
  * @retval None
  */
static void I2C7_MspDeInit(I2C_HandleTypeDef *hI2c)
{
  GPIO_InitTypeDef  gpio_init_structure;

  /* Prevent unused argument(s) compilation warning */
  UNUSED(hI2c);

  /* Configure I2C Tx, Rx as alternate function */
  gpio_init_structure.Pin = BUS_I2C7_SCL_PIN;
  HAL_GPIO_DeInit(BUS_I2C7_SCL_GPIO_PORT, gpio_init_structure.Pin);
  gpio_init_structure.Pin = BUS_I2C7_SDA_PIN;
  HAL_GPIO_DeInit(BUS_I2C7_SDA_GPIO_PORT, gpio_init_structure.Pin);

  /* Disable I2C clock */
  BUS_I2C7_CLK_DISABLE();

  (void)ResMgr_Release(RESMGR_RESOURCE_RIF_GPIOD, RESMGR_GPIO_PIN(15));
  (void)ResMgr_Release(RESMGR_RESOURCE_RIF_GPIOD, RESMGR_GPIO_PIN(14));
  (void)ResMgr_Release(RESMGR_RESOURCE_RIFSC, STM32MP25_RIFSC_I2C7_ID);
}

/**
  * @brief  Write a value in a register of the device through BUS.
  * @param  DevAddr    Device address on Bus.
  * @param  Reg        The target register address to write
  * @param  MemAddSize Size of internal memory address
  * @param  pData      The target register value to be written
  * @param  Length     data length in bytes
  * @retval BSP status
  */
static int32_t I2C7_WriteReg(uint16_t DevAddr, uint16_t Reg, uint16_t MemAddSize, uint8_t *pData, uint16_t Length)
{
  if (HAL_I2C_Mem_Write(&hbus_i2c7, DevAddr, Reg, MemAddSize, pData, Length, 10000) == HAL_OK)
  {
    return BSP_ERROR_NONE;
  }

  return BSP_ERROR_BUS_FAILURE;
}

/**
  * @brief  Read a register of the device through BUS
  * @param  DevAddr    Device address on BUS
  * @param  Reg        The target register address to read
  * @param  MemAddSize Size of internal memory address
  * @param  pData      The target register value to be written
  * @param  Length     data length in bytes
  * @retval BSP status
  */
static int32_t I2C7_ReadReg(uint16_t DevAddr, uint16_t Reg, uint16_t MemAddSize, uint8_t *pData, uint16_t Length)
{
  if (HAL_I2C_Mem_Read(&hbus_i2c7, DevAddr, Reg, MemAddSize, pData, Length, 10000) == HAL_OK)
  {
    return BSP_ERROR_NONE;
  }

  return BSP_ERROR_BUS_FAILURE;
}

/**
  * @brief  Receive data from the device through BUS
  * @param  DevAddr    Device address on BUS
  * @param  pData      The target register value to be received
  * @param  Length     data length in bytes
  * @retval BSP status
  */
static int32_t I2C7_Recv(uint16_t DevAddr, uint8_t *pData, uint16_t Length)
{
  if (HAL_I2C_Master_Receive(&hbus_i2c7, DevAddr, pData, Length, 10000) == HAL_OK)
  {
    return BSP_ERROR_NONE;
  }

  return BSP_ERROR_BUS_FAILURE;
}

/**
  * @brief  Send data to the device through BUS
  * @param  DevAddr    Device address on BUS
  * @param  pData      The target register value to be sent
  * @param  Length     data length in bytes
  * @retval BSP status
  */
static int32_t I2C7_Send(uint16_t DevAddr, uint8_t *pData, uint16_t Length)
{
  if (HAL_I2C_Master_Transmit(&hbus_i2c7, DevAddr, pData, Length, 10000) == HAL_OK)
  {
    return BSP_ERROR_NONE;
  }

  return BSP_ERROR_BUS_FAILURE;
}

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
