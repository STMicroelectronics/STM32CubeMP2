/**
    ******************************************************************************
    * @file    btn_monitor_task.c
    * @author  MCD Application Team
    * @brief   Button monitor task implementation file.
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

/**
    * @addtogroup BtnMonitorTask
    * @{
    */

#include "btn_monitor_task.h"
#include "btn_monitor_driver.h"
#if ENABLE_LOW_POWER_MGR_TASK
#include "low_power_mgr_task.h"
#endif

#include "FreeRTOS.h"
#include "task.h"

/**
    * @brief Short press threshold in milliseconds.
    */
#define SHORT_PRESS_MS       100U

/**
    * @brief Long press threshold in milliseconds.
    */
#define LONG_PRESS_MS       2500U

/**
    * @brief Very long press threshold in milliseconds.
    */
#define VERY_LONG_PRESS_MS  5000U

/**
    * @brief Poll interval used only while button is held (release detection).
    */
#define BTN_MONITOR_RELEASE_POLL_MS  10U

/**
    * @brief Handle for the button monitor task thread.
    */
static osThreadId_t BtnMonitorTaskHandle;

/**
  * @brief Attributes for the button monitor task thread.
  */
static const osThreadAttr_t BtnMonitorTaskAttr = {
    .name = "BtnMonitorTask",
    .priority = (osPriority_t) BTN_MONITOR_TASK_PRIORITY,
    .stack_size = BTN_MONITOR_TASK_STACK_SIZE
};

/**
    * @brief Handle for the button monitor semaphore.
    */
static osSemaphoreId_t BtnMonitorSemHandle;

/**
    * @brief Attributes for the button monitor semaphore.
    */
static const osSemaphoreAttr_t BtnMonitorSemaphoreAttr = {
    .name = "BtnMonitorSemaphore"
};

/**
    * @brief Structure for a button event listener entry.
    */
typedef struct {
    ButtonEventCallback_t callback;
    void *context;
} ButtonEventListener_t;

/**
    * @brief List of registered button event listeners.
    */
static ButtonEventListener_t BtnMonitorTaskEventListeners[BUTTON_EVENT_TYPE_COUNT] = {0};

/* Forward declarations */
static void BtnMonitorTask(void *argument);
static void BtnMonitorTask_DispatchEvent(ButtonEventType_t eventType);
static void BtnMonitorTask_IRQCallback(void *context);

#if ENABLE_LOW_POWER_MGR_TASK
/**
    * @brief Low-power manager listener for button IRQ handling.
    * @param notif: Low-power manager notif.
    * @param mode: Low-power mode associated with the event.
    * @param context: User context pointer.
    */
static void BtnMonitorTask_LowPowerListener(LowPowerMgrNotif_t notif, LowPowerMgrMode_t mode, void *context);
#endif

/**
    * @brief IRQ callback used by the button monitor driver.
    * @param  context: User context pointer (unused).
    */
static void BtnMonitorTask_IRQCallback(void *context)
{
    (void)context;
    if (BtnMonitorSemHandle != NULL)
    {
        (void)osSemaphoreRelease(BtnMonitorSemHandle);
    }
}

/**
    * @brief Handle low-power notifications to mask or unmask button IRQs.
    * @param notif: Low-power manager notif.
    * @param mode: Low-power mode associated with the event.
    * @param context: User context pointer.
    */
#if ENABLE_LOW_POWER_MGR_TASK
static void BtnMonitorTask_LowPowerListener(LowPowerMgrNotif_t notif, LowPowerMgrMode_t mode, void *context)
{
    (void)mode;
    (void)context;

    if ((notif == LOW_POWER_MGR_NOTIF_PREPARE_STOP || (notif == LOW_POWER_MGR_NOTIF_PREPARE_STANDBY)) && (btnMonitorDriver.disable_btn_irq != NULL))
    {
        btnMonitorDriver.disable_btn_irq();
    }
    else if ((notif == LOW_POWER_MGR_NOTIF_RESUME_STOP || (notif == LOW_POWER_MGR_NOTIF_RESUME_STANDBY)) && (btnMonitorDriver.enable_btn_irq != NULL))
    {
        btnMonitorDriver.enable_btn_irq();
    }
}
#endif

/**
    * @brief Initialize the button monitor task and its resources.
    *        Creates the semaphore, starts the task thread, and initializes the driver.
    * @retval None
    */
void BtnMonitorTask_Init(void)
{
    BtnMonitorSemHandle = osSemaphoreNew(1, 0, &BtnMonitorSemaphoreAttr);
    if (BtnMonitorSemHandle == NULL)
    {
        NSAppCore_ErrorHandler();
    }

    BtnMonitorTaskHandle = osThreadNew(BtnMonitorTask, NULL, &BtnMonitorTaskAttr);
    if (BtnMonitorTaskHandle == NULL)
    {
        NSAppCore_ErrorHandler();
    }

    if (btnMonitorDriver.init == NULL || btnMonitorDriver.init(BtnMonitorTask_IRQCallback, NULL) < 0)
    {
        NSAppCore_ErrorHandler();
    }

#if ENABLE_LOW_POWER_MGR_TASK
    if (LowPowerMgrTask_RegisterListener(BtnMonitorTask_LowPowerListener, NULL) != HAL_OK)
    {
        NSAppCore_ErrorHandler();
    }
#endif
}

/**
    * @brief De-initialize the button monitor task and its resources.
    *        De-initializes the driver, terminates the task thread, and deletes the semaphore.
    * @retval None
    */
void BtnMonitorTask_DeInit(void)
{
#if ENABLE_LOW_POWER_MGR_TASK
    (void)LowPowerMgrTask_UnregisterListener(BtnMonitorTask_LowPowerListener);
#endif

    if (btnMonitorDriver.deinit)
    {
        (void)btnMonitorDriver.deinit();
    }

    if (BtnMonitorTaskHandle)
    {
        osThreadTerminate(BtnMonitorTaskHandle);
        BtnMonitorTaskHandle = NULL;
    }

    if (BtnMonitorSemHandle)
    {
        osSemaphoreDelete(BtnMonitorSemHandle);
        BtnMonitorSemHandle = NULL;
    }
}

/**
    * @brief Register a listener for a button event type.
    * @param  eventType: Button event type to subscribe.
    * @param  cb: Callback function pointer.
    * @param  context: User context pointer passed back to callback.
    * @retval 0 on success, negative value on error.
    */
int BtnMonitorTask_RegisterListener(ButtonEventType_t eventType, ButtonEventCallback_t cb, void *context)
{
    if (eventType >= BUTTON_EVENT_TYPE_COUNT)
        return -1;
    if (BtnMonitorTaskEventListeners[eventType].callback != NULL)
        return -2;
    BtnMonitorTaskEventListeners[eventType].callback = cb;
    BtnMonitorTaskEventListeners[eventType].context = context;
    return 0;
}

/**
    * @brief Unregister a previously registered listener.
    * @param  eventType: Button event type to unsubscribe.
    * @retval 0 on success, negative value on error.
    */
int BtnMonitorTask_UnregisterListener(ButtonEventType_t eventType)
{
    if (eventType >= BUTTON_EVENT_TYPE_COUNT)
        return -1;
    if (BtnMonitorTaskEventListeners[eventType].callback == NULL)
        return -2;
    BtnMonitorTaskEventListeners[eventType].callback = NULL;
    BtnMonitorTaskEventListeners[eventType].context = NULL;
    return 0;
}

/**
    * @brief Dispatch an event to the registered listener (if any).
    * @param  eventType: Button event type.
    */
static void BtnMonitorTask_DispatchEvent(ButtonEventType_t eventType)
{
    if (eventType >= BUTTON_EVENT_TYPE_COUNT)
        return;
    if (BtnMonitorTaskEventListeners[eventType].callback)
    {
        BtnMonitorTaskEventListeners[eventType].callback(eventType, BtnMonitorTaskEventListeners[eventType].context);
    }
}

/**
    * @brief Button monitor task thread.
    * @param  argument: Task argument (unused).
    */
static void BtnMonitorTask(void *argument)
{
    (void)argument;

    for (;;)
    {
        if (osSemaphoreAcquire(BtnMonitorSemHandle, osWaitForever) == osOK)
        {
            /* Spurious wakeups or bounce can occur; only proceed if pressed. */
            if (!btnMonitorDriver.is_btn_pressed || !btnMonitorDriver.is_btn_pressed())
            {
                if (btnMonitorDriver.enable_btn_irq)
                {
                    btnMonitorDriver.enable_btn_irq();
                }
                continue;
            }

            TickType_t press_start_ticks = xTaskGetTickCount();

            /* Wait for release (portable: does not require both-edge EXTI). */
            while (btnMonitorDriver.is_btn_pressed && btnMonitorDriver.is_btn_pressed())
            {
                osDelay(BTN_MONITOR_RELEASE_POLL_MS);
            }

            TickType_t press_end_ticks = xTaskGetTickCount();
            TickType_t press_duration_ticks = press_end_ticks - press_start_ticks;
            uint32_t press_duration_ms = (uint32_t)(press_duration_ticks * portTICK_PERIOD_MS);

            if (press_duration_ms < SHORT_PRESS_MS)
            {
                /* Ignore as noise */
            }
            else if (press_duration_ms < LONG_PRESS_MS)
            {
                BtnMonitorTask_DispatchEvent(BUTTON_EVENT_SHORT_PRESS);
            }
            else if (press_duration_ms < VERY_LONG_PRESS_MS)
            {
                BtnMonitorTask_DispatchEvent(BUTTON_EVENT_LONG_PRESS);
            }
            else
            {
                BtnMonitorTask_DispatchEvent(BUTTON_EVENT_VERY_LONG_PRESS);
            }

            if (btnMonitorDriver.enable_btn_irq)
            {
                btnMonitorDriver.enable_btn_irq();
            }
        }
    }
}

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

