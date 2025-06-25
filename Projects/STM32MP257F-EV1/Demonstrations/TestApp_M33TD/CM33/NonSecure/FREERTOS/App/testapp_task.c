/**
  ******************************************************************************
  * @file    testapp_task.c
  * @author  MCD Application Team
  * @brief   Test application task implementation file.
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
#include "testapp_task.h"
#include "cmsis_os2.h"
#include "app_tasks_config.h"
#include <stdio.h>
#include "userapp_task.h"
#include "app_freertos.h"
#include "test_common.h"
#include "stm32mp2xx_hal.h"
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#ifdef ENABLE_TIMERS_TEST
#include "test_timer.h"
#endif

#ifdef ENABLE_I2C_TEST
#include "test_i2c.h"
#endif

#ifdef ENABLE_SPI_TEST
#include "test_spi.h"
#endif

#ifdef ENABLE_ADC_TEST
#include "test_adc.h"
#endif

#ifdef ENABLE_FDCAN_TEST
#include "test_fdcan.h"
#endif

#ifdef ENABLE_I3C_TEST
#include "test_i3c.h"
#endif

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
#define EXTI_EVENT_FLAG    (1U << 0)
#define TESTAPP_TEST_COUNT   (sizeof(testList)/sizeof(testList[0]))

/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/**
  * @brief  RTOS event for EXTI (button press).
  */
static osEventFlagsId_t extiEventFlag = NULL;
EXTI_HandleTypeDef hexti;

/* USER CODE BEGIN PV */
static osThreadId_t TestAppTaskHandle;
static volatile size_t currentTest = 0;

static const osThreadAttr_t TestAppTaskAttr = {
    .name = "TestAppTask",
    .priority = (osPriority_t)TESTAPP_TASK_PRIORITY,
    .stack_size = TESTAPP_TASK_STACK_SIZE
};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/**
  * @brief  Main test application task.
  * @param  argument: Not used.
  * @retval None
  */
static void TestAppTask(void *argument);

/**
  * @brief  Notify EXTI event.
  * @retval None
  */
static void TestApp_EXTI_Notify(void);

/**
  * @brief  Wait for user action (button press).
  * @param  instruction: Instruction to display to the user.
  * @retval None
  */
static void WaitForUserAction(const char* instruction);

typedef struct {
    const char *name;
    TEST_STATUS (*resourceAcquire)(void);
    TEST_STATUS (*init)(void);
    TEST_STATUS (*run)(void);
    TEST_STATUS (*deinit)(void);
    TEST_STATUS (*resourceRelease)(void);
    TestType type; 
    const char *instruction; 
} TestAppTestEntry;

static const TestAppTestEntry testList[] = {
#ifdef ENABLE_TIMERS_TEST
    { "TIMERS", Test_TIMERS_ResourceAcquire, Test_TIMER_Init, Test_TIMER_Run, Test_TIMER_DeInit, Test_TIMERS_ResourceRelease, TEST_TYPE_AUTO, "Press button to run TIMERS test" },
#endif
#ifdef ENABLE_I2C_TEST
    { "I2C", Test_I2C_ResourceAcquire, Test_I2C_Init, Test_I2C_Run, Test_I2C_DeInit, Test_I2C_ResourceRelease, TEST_TYPE_MANUAL, "Press button to run I2C test" },
#endif
#ifdef ENABLE_SPI_TEST
    { "SPI", Test_SPI_ResourceAcquire, Test_SPI_Init, Test_SPI_Run, Test_SPI_DeInit, Test_SPI_ResourceRelease, TEST_TYPE_MANUAL, "Press button to run SPI test" },
#endif
#ifdef ENABLE_ADC_TEST
    { "ADC", Test_ADC_ResourceAcquire, Test_ADC_Init, Test_ADC_Run, Test_ADC_DeInit, Test_ADC_ResourceRelease, TEST_TYPE_AUTO, "Press button to run ADC test" },
#endif
#ifdef ENABLE_FDCAN_TEST
    { "FDCAN", Test_FDCAN_ResourceAcquire, Test_FDCAN_Init, Test_FDCAN_Run, Test_FDCAN_DeInit, Test_FDCAN_ResourceRelease, TEST_TYPE_AUTO, "Press button to run FDCAN test" },
#endif
#ifdef ENABLE_I3C_TEST
    { "I3C", Test_I3C_ResourceAcquire, Test_I3C_Init, Test_I3C_Run, Test_I3C_DeInit, Test_I3C_ResourceRelease, TEST_TYPE_MANUAL, "Press button to run I3C test" },
#endif
};

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Exported functions --------------------------------------------------------*/
/**
  * @brief  Initialize the Test Application Task and its resources.
  * @retval None
  */
void TestAppTask_Init(void)
{

    GPIO_InitTypeDef   GPIO_InitStruct;
    EXTI_ConfigTypeDef EXTI_ConfigStructure;

    // Enable GPIOG clock
    if (RESMGR_STATUS_ACCESS_OK == ResMgr_Request(RESMGR_RESOURCE_RIF_RCC, RESMGR_RCC_RESOURCE(96)))
    {
        __HAL_RCC_GPIOG_CLK_ENABLE();
    }
    // Acquire GPIOG8 using Resource manager
    if (RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIF_GPIOG, RESMGR_GPIO_PIN(8)))
    {
        App_ErrorHandler();
    }

    // Configure PG.8 pin as input floating
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Pin = BUTTON_USER2_PIN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(BUTTON_USER2_GPIO_PORT, &GPIO_InitStruct);

    // Set configuration except Interrupt and Event mask of Exti line 8
    EXTI_ConfigStructure.Line = EXTI2_LINE_8;
    EXTI_ConfigStructure.Trigger = EXTI_TRIGGER_RISING_FALLING;
    EXTI_ConfigStructure.GPIOSel = EXTI_GPIOG;
    EXTI_ConfigStructure.Mode = EXTI_MODE_INTERRUPT;
    HAL_EXTI_SetConfigLine(&hexti, &EXTI_ConfigStructure);

    // Register callback to treat Exti interrupts in user Exti2FallingCb function
    HAL_EXTI_RegisterCallback(&hexti, HAL_EXTI_FALLING_CB_ID, TestApp_EXTI_Notify);

    // Enable and set line 2 Interrupt to the lowest priority
    HAL_NVIC_SetPriority(EXTI2_8_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + DEFAULT_IRQ_PRIO, 0);
    HAL_NVIC_EnableIRQ(EXTI2_8_IRQn);

    extiEventFlag = osEventFlagsNew(NULL);
    if (extiEventFlag == NULL) {
        APP_LOG_ERR("TestApp", "Failed to create EXTI event flag");
        return;
    }
    TestAppTaskHandle = osThreadNew(TestAppTask, NULL, &TestAppTaskAttr);
    if (TestAppTaskHandle == NULL) {
        APP_LOG_ERR("TestApp", "Failed to create TestAppTask");
    }
}

/**
  * @brief  De-initialize the Test Application Task and release its resources.
  * @retval None
  */
void TestAppTask_DeInit(void)
{
    if (TestAppTaskHandle != NULL) {
        osThreadTerminate(TestAppTaskHandle);
        TestAppTaskHandle = NULL;
    }
    if (extiEventFlag != NULL) {
        osEventFlagsDelete(extiEventFlag);
        extiEventFlag = NULL;
    }

    HAL_GPIO_DeInit(BUTTON_USER2_GPIO_PORT, BUTTON_USER2_PIN);
    ResMgr_Release(RESMGR_RESOURCE_RIF_GPIOG, RESMGR_GPIO_PIN(8));
    HAL_NVIC_DisableIRQ(EXTI2_8_IRQn);
}

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Main test task: waits for EXTI event before each test and for user action before running each test.
  * @param  argument: Not used.
  * @retval None
  */
void TestAppTask(void *argument) {
    (void)argument;

    for (size_t i = 0; i < sizeof(testList) / sizeof(testList[0]); ++i) {
        const TestAppTestEntry *test = &testList[i];

        // Step 1: Resource Acquire
        APP_LOG_INF("TestApp", "Acquiring resources for test: %s", test->name);
        TEST_STATUS resourceStatus = test->resourceAcquire();
        if (resourceStatus != TEST_OK) {
            APP_LOG_ERR("TestApp", "Resource acquisition failed for test: %s", test->name);
            continue;
        }

        // Step 2: Handle Auto/Manual Tests
        if (ENABLE_AUTO_TEST == 1 && test->type == TEST_TYPE_MANUAL) {
            APP_LOG_INF("TestApp", "Skipping manual test in auto Test mode: %s", test->name);
            APP_LOG_INF("TestApp", "Releasing resources for test: %s", test->name);
            test->resourceRelease();
            continue;
        }

        if (ENABLE_AUTO_TEST == 0) {
            // Wait for user action for all tests when ENABLE_AUTO_TEST is OFF
            WaitForUserAction(test->instruction);
        }

        // Step 3: Run the Test (Init -> Run -> Deinit)
        APP_LOG_INF("TestApp", "Running test: %s", test->name);
        TEST_STATUS result = Run_Test(test->init, test->run, test->deinit);

        // Log the result
        APP_LOG_INF("TestApp", "Test %s: %s", test->name,
                    result == TEST_OK ? "\x1b[32mPASSED\x1b[0m" : "\x1b[31mFAILED\x1b[0m");

        // Step 4: Resource Release
        APP_LOG_INF("TestApp", "Releasing resources for test: %s", test->name);
        if (test->resourceRelease() != TEST_OK) {
            APP_LOG_ERR("TestApp", "Resource release failed for test: %s", test->name);
        }
    }

    APP_LOG_INF("TestApp", "All tests completed.");
    osThreadSuspend(TestAppTaskHandle);
}

/**
  * @brief  Notify EXTI event (called from EXTI interrupt handler).
  * @retval None
  */
static void TestApp_EXTI_Notify(void)
{
    if (extiEventFlag != NULL) {
        osEventFlagsSet(extiEventFlag, EXTI_EVENT_FLAG);
    }

}

/**
  * @brief  Wait for EXTI event (button press).
  * @param  instruction: Instruction to display to the user.
  * @retval None
  */
static void WaitForUserAction(const char* instruction)
{
    if (extiEventFlag == NULL) {
        APP_LOG_ERR("TestApp", "EXTI event flag is NULL. Cannot wait for user action.");
        return;
    }
    APP_LOG_INF("TestApp", "USER ACTION: %s", instruction);
    // Wait for EXTI event (button press)
    osEventFlagsWait(extiEventFlag, EXTI_EVENT_FLAG, osFlagsWaitAny, osWaitForever);
}
/*********************** (C) COPYRIGHT STMicroelectronics *END OF FILE */
