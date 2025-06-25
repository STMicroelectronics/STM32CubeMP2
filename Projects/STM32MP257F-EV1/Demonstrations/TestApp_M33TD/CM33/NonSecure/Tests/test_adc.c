/**
  ******************************************************************************
  * @file    test_adc.c
  * @author  MCD Application Team
  * @brief   ADC test routines.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
/**
  * @brief Includes required for ADC test routines.
  */
#include "test_adc.h"
#include "app_freertos.h"

#ifdef ENABLE_ADC_TEST  // Conditionally compile the ADC test code

/** @addtogroup STM32MP2xx_Application
  * @{
  */

/** @addtogroup StarterApp_M33TD_Tests
  * @{
  */

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
/**
  * @brief ADC handler declaration.
  */
ADC_HandleTypeDef AdcHandle;

/**
  * @brief ADC channel configuration structure declaration.
  */
ADC_ChannelConfTypeDef sConfig;

/**
  * @brief Converted value declaration.
  */
uint32_t uwConvertedValue;

/**
  * @brief Input voltage declaration.
  */
uint32_t uwInputVoltage;

/* Private function prototypes -----------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
/**
  * @brief  Acquire ADC resources.
  * @retval TEST_STATUS: TEST_OK if success, TEST_FAIL otherwise.
  */
TEST_STATUS Test_ADC_ResourceAcquire(void)
{
    /* Acquire ADC3 using Resource manager */
    if (RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIFSC, STM32MP25_RIFSC_ADC3_ID))
    {
        return TEST_FAIL;
    }

    return TEST_OK;
}

/**
  * @brief  Release ADC resources.
  * @retval TEST_STATUS: TEST_OK if success, TEST_FAIL otherwise.
  */
TEST_STATUS Test_ADC_ResourceRelease(void)
{
    /* Release ADC3 using Resource manager */
    if (ResMgr_Release(RESMGR_RESOURCE_RIFSC, STM32MP25_RIFSC_ADC3_ID) != RESMGR_STATUS_ACCESS_OK)
    {
        return TEST_FAIL;
    }

    return TEST_OK;
}

/**
  * @brief  Initialize the ADC peripheral and configure the channel.
  * @retval TEST_STATUS: TEST_OK if success, TEST_FAIL otherwise.
  */
TEST_STATUS Test_ADC_Init(void)
{
    /* Configure the ADC peripheral */
    AdcHandle.Instance = ADC3;
    AdcHandle.Init.ClockPrescaler = ADC_CLOCK_CK_KER_ADC3_DIV2;
    AdcHandle.Init.Resolution = ADC_RESOLUTION_12B;
    AdcHandle.Init.ScanConvMode = DISABLE;
    AdcHandle.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    AdcHandle.Init.LowPowerAutoWait = DISABLE;
    AdcHandle.Init.ContinuousConvMode = ENABLE;
    AdcHandle.Init.NbrOfConversion = 1;
    AdcHandle.Init.DiscontinuousConvMode = DISABLE;
    AdcHandle.Init.NbrOfDiscConversion = 1;
    AdcHandle.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    AdcHandle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    AdcHandle.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DR;
    AdcHandle.Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;
    AdcHandle.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
    AdcHandle.Init.OversamplingMode = DISABLE;

    /* Enable peripherals and GPIO Clocks */
    __HAL_RCC_ADC3KERCLK_SETSOURCE(RCC_ADC3KERCLKSOURCE_CK_ICN_LS_MCU);
    __HAL_RCC_ADC3_CLK_ENABLE();
    __HAL_RCC_ADC3_FORCE_RESET();
    __HAL_RCC_ADC3_RELEASE_RESET();

    /* Initialize ADC peripheral */
    if (HAL_ADC_Init(&AdcHandle) != HAL_OK)
    {
        return TEST_FAIL;
    }

    /* Configure the ADC channel */
    sConfig.Channel = ADC_CHANNEL_VREFINT;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_1501CYCLES_5;
    sConfig.SingleDiff = ADC_SINGLE_ENDED;
    sConfig.OffsetNumber = ADC_OFFSET_NONE;
    sConfig.Offset = 0;
    sConfig.OffsetRightShift = DISABLE;
    sConfig.OffsetSignedSaturation = DISABLE;

    if (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK)
    {
        return TEST_FAIL;
    }

    /* Start calibration */
    if (HAL_ADCEx_Calibration_Start(&AdcHandle, ADC_SINGLE_ENDED) != HAL_OK)
    {
        return TEST_FAIL;
    }

    /* Start conversion */
    if (HAL_ADC_Start(&AdcHandle) != HAL_OK)
    {
        return TEST_FAIL;
    }

    return TEST_OK;
}

/**
  * @brief  Run ADC conversion and process the result.
  * @retval TEST_STATUS: TEST_OK if success, TEST_FAIL otherwise.
  */
TEST_STATUS Test_ADC_Run(void)
{
    if (HAL_ADC_PollForConversion(&AdcHandle, 10) != HAL_OK)
    {
        return TEST_FAIL;
    }

    /* Read the converted value */
    uwConvertedValue = HAL_ADC_GetValue(&AdcHandle);

    /* Convert the result from 12-bit value to the voltage dimension (mV unit) */
    uwInputVoltage = (uwConvertedValue * 1800) / 0xFFF;

    if (uwInputVoltage == 0)
    {
        return TEST_FAIL;
    }

    return TEST_OK;
}

/**
  * @brief  De-initialize the ADC peripheral and release resources.
  * @retval TEST_STATUS: TEST_OK if success, TEST_FAIL otherwise.
  */
TEST_STATUS Test_ADC_DeInit(void)
{
    /* Deinitialize the ADC peripheral */
    if (HAL_ADC_DeInit(&AdcHandle) != HAL_OK)
    {
        return TEST_FAIL;
    }

    /* Disable peripheral clock */
    __HAL_RCC_ADC3_CLK_DISABLE();

    return TEST_OK;
}

/**
  * @}
  */

/**
  * @}
  */

#endif /* ENABLE_ADC_TEST */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
