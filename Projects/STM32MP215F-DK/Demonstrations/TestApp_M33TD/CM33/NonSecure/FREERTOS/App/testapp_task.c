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
/**
  * @brief Includes required for the Test Application Task functionality.
  */
#include "testapp_task.h"
#include <stdio.h>
#include "test_common.h"
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/**
  * @brief Private includes for additional headers required by this file.
  */
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
/**
  * @brief Private typedefs used in this file.
  */
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/**
  * @brief Private defines used in this file.
  */
#define EXTI_EVENT_FLAG    (1U << 0)
#define TESTAPP_TEST_COUNT   (sizeof(testList)/sizeof(testList[0]))

/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/**
  * @brief Private macros used in this file.
  */
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/**
    * @brief  RTOS event for button press.
  */
static osEventFlagsId_t extiEventFlag = NULL;

/* USER CODE BEGIN PV */
static osThreadId_t TestAppTaskHandle; /**< Handle for the Test Application Task. */
static volatile size_t currentTest = 0; /**< Index of the current test being executed. */

static const osThreadAttr_t TestAppTaskAttr = {
    .name = "TestAppTask",
    .priority = (osPriority_t)TESTAPP_TASK_PRIORITY,
    .stack_size = TESTAPP_TASK_STACK_SIZE
}; /**< Attributes for the Test Application Task. */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/**
  * @brief  Main test application task.
  * @param  argument: Not used.
  * @retval None
  */
static void TestAppTask(void *argument);

/**
    * @brief  Button short-press handler (driven by BtnMonitorTask).
    * @param  event: Button event type.
    * @param  context: User context (unused).
    * @retval None
    */
static void TestApp_BtnHandler(ButtonEventType_t event, void *context);

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
    { "TIMERS", Test_TIMER_ResourceAcquire, Test_TIMER_Init, Test_TIMER_Run, Test_TIMER_DeInit, Test_TIMER_ResourceRelease, TEST_TYPE_AUTO, "Press button to run TIMERS test" },
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

/* Exported functions --------------------------------------------------------*/
/**
  * @brief  Initialize the Test Application Task and its resources.
  * @retval None
  */
void TestAppTask_Init(void)
{
    extiEventFlag = osEventFlagsNew(NULL);
    if (extiEventFlag == NULL) {
        printf("\033[31m[NS] [ERR] [TestApp] Failed to create EXTI event flag\033[0m\r\n");
        return;
    }

    /* Button press events are provided by the NSAppCore BtnMonitorTask.
     * - SHORT_PRESS drives test sequencing (this task).
     * - VERY_LONG_PRESS is reserved for reboot handling (OpenAMP/NSAppCore).
     */
    int btnRegStatus = BtnMonitorTask_RegisterListener(BUTTON_EVENT_SHORT_PRESS, TestApp_BtnHandler, NULL);
    if (btnRegStatus == -2)
    {
        printf("\033[32m[NS] [INF] [TestApp] Button listener (SHORT_PRESS) already registered\033[0m\r\n");
    }
    else if (btnRegStatus < 0)
    {
        printf("\033[31m[NS] [ERR] [TestApp] Failed to register button listener (SHORT_PRESS), err=%d\033[0m\r\n", btnRegStatus);
        osEventFlagsDelete(extiEventFlag);
        extiEventFlag = NULL;
        return;
    }

    TestAppTaskHandle = osThreadNew(TestAppTask, NULL, &TestAppTaskAttr);
    if (TestAppTaskHandle == NULL) {
        printf("\033[31m[NS] [ERR] [TestApp] Failed to create TestAppTask\033[0m\r\n");
        (void)BtnMonitorTask_UnregisterListener(BUTTON_EVENT_SHORT_PRESS);
        osEventFlagsDelete(extiEventFlag);
        extiEventFlag = NULL;
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

    (void)BtnMonitorTask_UnregisterListener(BUTTON_EVENT_SHORT_PRESS);

    if (extiEventFlag != NULL) {
        osEventFlagsDelete(extiEventFlag);
        extiEventFlag = NULL;
    }
}

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Main test task: waits for EXTI event before each test and for user action before running each test.
  * @param  argument: Not used.
  * @retval None
  */
static void TestAppTask(void *argument) {
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
static void TestApp_BtnHandler(ButtonEventType_t event, void *context)
{
    (void)context;

    if (event != BUTTON_EVENT_SHORT_PRESS)
    {
        return;
    }

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
