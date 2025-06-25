/**
  ******************************************************************************
  * @file    test_timer.c
  * @author  MCD Application Team
  * @brief   TIMERS test routines.
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
  * @brief Includes required for TIMERS test routines.
  */
#include "test_timer.h"
#include "app_freertos.h"
#include "stdlib.h"

#ifdef ENABLE_TIMERS_TEST  // Conditionally compile the TIMERS test code

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
  * @brief Timer handles.
  */
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim8;
LPTIM_HandleTypeDef hlptim4;

/**
  * @brief Synchronization check variables.
  */
static volatile uint16_t cnt = 0;
static volatile uint32_t tim2_last = 0, tim3_last = 0, tim8_last = 0, lptim4_last = 0;
static volatile int sync_success = 1;

/* Exported functions --------------------------------------------------------*/
/**
  * @brief  Timer period elapsed callback for synchronization check and LED toggle.
  * @param  htim: Pointer to TIM handle.
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM8) {
        tim8_last = __HAL_TIM_GET_COUNTER(&htim8);
        tim2_last = __HAL_TIM_GET_COUNTER(&htim2);
        tim3_last = __HAL_TIM_GET_COUNTER(&htim3);

        // Check if TIM2 and TIM3 are synchronized (should be near 0 after TIM8 triggers)
        if ((tim2_last > 1) || (tim3_last > 1)) {
            sync_success = 0;
        }
    } else if (htim->Instance == TIM2) {
        // TIM2-specific logic
    } else if (htim->Instance == TIM3) {
        // TIM3-specific logic
    }
}

/**
  * @brief  LPTIM4 auto-reload match callback for LED toggle.
  * @param  hlptim: Pointer to LPTIM handle.
  * @retval None
  */
void HAL_LPTIM_AutoReloadMatchCallback(LPTIM_HandleTypeDef *hlptim)
{
    if (hlptim->Instance == LPTIM4) {
        lptim4_last = __HAL_LPTIM_GET_COUNTER(hlptim);
        BSP_LED_Toggle(LED3);
    }
}

/**
  * @brief  Initialize all timers and configure synchronization.
  * @retval TEST_STATUS: TEST_OK if success, TEST_FAIL otherwise.
  */
TEST_STATUS Test_TIMER_Init(void)
{
    // Force reset and release for LPTIM4 before init
    __HAL_RCC_LPTIM4_FORCE_RESET();
    __HAL_RCC_LPTIM4_RELEASE_RESET();

    // TIM2: Master, TIM3/TIM8: Slaves, LPTIM4: Standalone
    __HAL_RCC_TIM2_CLK_ENABLE();
    __HAL_RCC_TIM3_CLK_ENABLE();
    __HAL_RCC_TIM8_CLK_ENABLE();
    __HAL_RCC_LPTIM4_CLK_ENABLE();

    // Enable and set priorities for timer interrupts
    HAL_NVIC_SetPriority(TIM2_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);

    HAL_NVIC_SetPriority(TIM3_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(TIM3_IRQn);

    HAL_NVIC_SetPriority(TIM8_UP_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(TIM8_UP_IRQn);

    HAL_NVIC_SetPriority(LPTIM4_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(LPTIM4_IRQn);

    // --- TIM8: Advanced timer, 1MHz counter clock, 100Hz output (for visible sync) ---
    uint32_t tim8_prescaler = (HAL_RCCEx_GetTimerCLKFreq(RCC_PERIPHCLK_TIM8) / 1000000UL) - 1; // 1 MHz
    uint32_t tim8_period = 10000 - 1;     // 100 Hz (overflow every 10ms)

    htim8.Instance = TIM8;
    htim8.Init.Prescaler = tim8_prescaler;
    htim8.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim8.Init.Period = tim8_period;
    htim8.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim8.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&htim8) != HAL_OK) return TEST_FAIL;

    TIM_MasterConfigTypeDef masterConfig8 = {0};
    masterConfig8.MasterOutputTrigger = TIM_TRGO_UPDATE;
    masterConfig8.MasterSlaveMode = TIM_MASTERSLAVEMODE_ENABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim8, &masterConfig8) != HAL_OK) return TEST_FAIL;

    // --- TIM2/TIM3: General purpose, 10kHz counter clock, 100Hz output ---
    uint32_t timx_prescaler = (HAL_RCCEx_GetTimerCLKFreq(RCC_PERIPHCLK_TIM2) / 10000UL) - 1; // 10 kHz
    uint32_t timx_period = 100 - 1;                          // 100 Hz (overflow every 10ms)

    // TIM2: Slave config (triggered by TIM8)
    htim2.Instance = TIM2;
    htim2.Init.Prescaler = timx_prescaler;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = timx_period;
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&htim2) != HAL_OK) return TEST_FAIL;

    TIM_SlaveConfigTypeDef slaveConfig2 = {0};
    slaveConfig2.SlaveMode = TIM_SLAVEMODE_RESET;
    slaveConfig2.InputTrigger = TIM_TS_ITR5; // TIM8->TIM2 mapping (check RM for ITRx mapping)
    if (HAL_TIM_SlaveConfigSynchro(&htim2, &slaveConfig2) != HAL_OK) return TEST_FAIL;

    // TIM3: Slave config (triggered by TIM8)
    htim3.Instance = TIM3;
    htim3.Init.Prescaler = timx_prescaler;
    htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim3.Init.Period = timx_period;
    htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&htim3) != HAL_OK) return TEST_FAIL;

    TIM_SlaveConfigTypeDef slaveConfig3 = {0};
    slaveConfig3.SlaveMode = TIM_SLAVEMODE_RESET;
    slaveConfig3.InputTrigger = TIM_TS_ITR5; // TIM8->TIM3 mapping (check RM for ITRx mapping)
    if (HAL_TIM_SlaveConfigSynchro(&htim3, &slaveConfig3) != HAL_OK) return TEST_FAIL;

    
    // 2. Configure LPTIM4
    hlptim4.Instance = LPTIM4;
    hlptim4.Init.Clock.Source = LPTIM_CLOCKSOURCE_APBCLOCK_LPOSC;
    hlptim4.Init.Clock.Prescaler = LPTIM_PRESCALER_DIV128;
    hlptim4.Init.UltraLowPowerClock.Polarity = LPTIM_CLOCKPOLARITY_RISING;
    hlptim4.Init.UltraLowPowerClock.SampleTime = LPTIM_CLOCKSAMPLETIME_DIRECTTRANSITION;
    hlptim4.Init.Trigger.Source = LPTIM_TRIGSOURCE_SOFTWARE;
    hlptim4.Init.Period = 0xFFFF;
    hlptim4.Init.UpdateMode = LPTIM_UPDATE_ENDOFPERIOD;
    hlptim4.Init.CounterSource = LPTIM_COUNTERSOURCE_INTERNAL;
    hlptim4.Init.RepetitionCounter = 0;

    if (HAL_LPTIM_Init(&hlptim4) != HAL_OK) return TEST_FAIL;

    sync_success = 1; // Reset sync check flag

    return TEST_OK;
}

/**
  * @brief  Run timer synchronization test and observe results.
  * @retval TEST_STATUS: TEST_OK if success, TEST_FAIL otherwise.
  */
TEST_STATUS Test_TIMER_Run(void)
{
    // Start TIM3 and TIM8 in interrupt mode (they will wait for TIM2 trigger)
    if (HAL_TIM_Base_Start_IT(&htim3) != HAL_OK) return TEST_FAIL;
    if (HAL_TIM_Base_Start_IT(&htim2) != HAL_OK) return TEST_FAIL;

    // Start TIM2 (master) in interrupt mode
    if (HAL_TIM_Base_Start_IT(&htim8) != HAL_OK) return TEST_FAIL;

    // Start LPTIM4 in interrupt mode
    if (HAL_LPTIM_Counter_Start_IT(&hlptim4) != HAL_OK) return TEST_FAIL;

    // Wait for a few seconds to observe synchronization (replace with RTOS delay or polling as needed)
    osDelay(5000);

    // Stop all timers
    HAL_TIM_Base_Stop_IT(&htim2);
    HAL_TIM_Base_Stop_IT(&htim3);
    HAL_TIM_Base_Stop_IT(&htim8);
    HAL_LPTIM_Counter_Stop_IT(&hlptim4);

    // Analyze synchronization result
    if (sync_success)
        return TEST_OK;
    else
        return TEST_FAIL;
}

/**
  * @brief  De-initialize all timers and release resources.
  * @retval TEST_STATUS: TEST_OK if success, TEST_FAIL otherwise.
  */
TEST_STATUS Test_TIMER_DeInit(void)
{
    HAL_TIM_Base_DeInit(&htim2);
    HAL_TIM_Base_DeInit(&htim3);
    HAL_TIM_Base_DeInit(&htim8);
    HAL_LPTIM_DeInit(&hlptim4);

    __HAL_RCC_TIM2_CLK_DISABLE();
    __HAL_RCC_TIM3_CLK_DISABLE();
    __HAL_RCC_TIM8_CLK_DISABLE();
    __HAL_RCC_LPTIM4_CLK_DISABLE();

    // Force reset and release for LPTIM4 after deinit
    __HAL_RCC_LPTIM4_FORCE_RESET();
    __HAL_RCC_LPTIM4_RELEASE_RESET();

    // Disable timer interrupts
    HAL_NVIC_DisableIRQ(TIM2_IRQn);
    HAL_NVIC_DisableIRQ(TIM3_IRQn);
    HAL_NVIC_DisableIRQ(TIM8_UP_IRQn);
    HAL_NVIC_DisableIRQ(LPTIM4_IRQn);

    return TEST_OK;
}

/**
  * @brief  Acquire TIMERS resources.
  * @retval TEST_STATUS: TEST_OK if success, TEST_FAIL otherwise.
  */
TEST_STATUS Test_TIMERS_ResourceAcquire(void)
{
    // Acquire TIM2, TIM3, TIM8, and LPTIM4 using Resource manager
    if (RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIFSC, STM32MP25_RIFSC_TIM2_ID) ||
        RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIFSC, STM32MP25_RIFSC_TIM3_ID) ||
        RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIFSC, STM32MP25_RIFSC_TIM8_ID) ||
        RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIFSC, STM32MP25_RIFSC_LPTIM4_ID)) {
        return TEST_FAIL;
    }

    return TEST_OK;
}

/**
  * @brief  Release TIMERS resources.
  * @retval TEST_STATUS: TEST_OK if success, TEST_FAIL otherwise.
  */
TEST_STATUS Test_TIMERS_ResourceRelease(void)
{
    // Release TIM2, TIM3, TIM8, and LPTIM4 using Resource manager
    if (ResMgr_Release(RESMGR_RESOURCE_RIFSC, STM32MP25_RIFSC_TIM2_ID) != RESMGR_STATUS_ACCESS_OK ||
        ResMgr_Release(RESMGR_RESOURCE_RIFSC, STM32MP25_RIFSC_TIM3_ID) != RESMGR_STATUS_ACCESS_OK ||
        ResMgr_Release(RESMGR_RESOURCE_RIFSC, STM32MP25_RIFSC_TIM8_ID) != RESMGR_STATUS_ACCESS_OK ||
        ResMgr_Release(RESMGR_RESOURCE_RIFSC, STM32MP25_RIFSC_LPTIM4_ID) != RESMGR_STATUS_ACCESS_OK) {
        return TEST_FAIL;
    }

    return TEST_OK;
}

/**
  * @}
  */

/**
  * @}
  */
#endif /* ENABLE_TIMERS_TEST */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
