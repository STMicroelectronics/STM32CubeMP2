/**
  ******************************************************************************
  * @file    remoteproc_task.c
  * @author  MCD Application Team
  * @brief   Remote processor task implementation file.
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
#include "remoteproc_task.h"
#include "app_freertos.h"
#include "app_tasks_config.h"
#include "low_power_display_task.h"
#include <stdio.h>
#include <string.h>

/** @addtogroup RemoteProcTask
  * @{
  */

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/** @brief Global variable to store remote processor information. */
static struct cpu_info_res remoteProcCoproInfo;

/** @brief Handle for the remote processor command queue. */
static osMessageQueueId_t RemoteProcCmdQueueHandle;

/** @brief Attributes for the remote processor command queue. */
static const osMessageQueueAttr_t RemoteProcCmdQueueAttr = {
    .name = "RemoteProcCmdQueue"
};

/** @brief Handle for the remote processor task thread. */
static osThreadId_t RemoteProcTaskHandle;

/** @brief Attributes for the remote processor task thread. */
static const osThreadAttr_t RemoteProcTaskAttr = {
    .name = "RemoteProcTask",
    .priority = (osPriority_t) REMOTEPROC_TASK_PRIORITY,
    .stack_size = REMOTEPROC_TASK_STACK_SIZE
};

/** @brief Strings representing the CPU status for the coprocessor. */
static const char *CoproCpuStatusStr[] = {
    "offline",
    "suspended",
    "started",
    "running",
    "crashed",
    "unknown",
};

/** @brief Handle for EXTI2 interrupt. */
EXTI_HandleTypeDef hexti1;

/* Private function prototypes -----------------------------------------------*/
/**
  * @brief  Task to handle remote processor commands.
  * @param  argument: Not used.
  * @retval None
  */
static void RemoteProcTask(void *argument);

/**
  * @brief  Handle the start command for the remote processor.
  * @param  cpu_id: ID of the remote processor to start.
  * @retval None
  */
static void RemoteProc_HandleStart(uint32_t cpu_id);

/**
  * @brief  Handle the stop command for the remote processor.
  * @param  cpu_id: ID of the remote processor to stop.
  * @retval None
  */
static void RemoteProc_HandleStop(uint32_t cpu_id);

/**
  * @brief  Handle the crash command for the remote processor.
  * @param  cpu_id: ID of the remote processor that crashed.
  * @retval None
  */
static void RemoteProc_HandleCrash(uint32_t cpu_id);

/**
  * @brief  Initialize EXTI2 for the remote processor.
  * @retval None
  */
static void RemoteProc_EXTI2_Init(void);

/**
  * @brief  De-initialize EXTI2 for the remote processor.
  * @retval None
  */
static void RemoteProc_EXTI2_DeInit(void);

/**
  * @brief  Callback for EXTI2 falling edge interrupt.
  * @retval None
  */
static void RemoteProc_Exti2FallingCb(void);

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Initialize the Remote Processor Task and its resources.
  *         Creates the command queue and the task thread.
  * @retval None
  */
void RemoteProcTask_Init(void) {
    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* USER CODE BEGIN RTOS_MUTEX */
    /* Add mutexes, if required */
    /* USER CODE END RTOS_MUTEX */

    /* USER CODE BEGIN RTOS_SEMAPHORES */
    /* Add semaphores, if required */
    /* USER CODE END RTOS_SEMAPHORES */

    /* USER CODE BEGIN RTOS_TIMERS */
    /* Start timers, add new ones, if required */
    /* USER CODE END RTOS_TIMERS */

    /* USER CODE BEGIN RTOS_QUEUES */
    RemoteProcCmdQueueHandle = osMessageQueueNew(10, sizeof(RemoteProcCmd), &RemoteProcCmdQueueAttr);
    if (RemoteProcCmdQueueHandle == NULL) {
        App_ErrorHandler();
    }

#ifdef REMOTE_PROC_AUTO_START
    /* Auto-start the remote processor if the macro is enabled */
    RemoteProcCmd cmd = { .action = REMOTEPROC_ACTION_START, .cpu_id = 0 };
    RemoteProc_PostCmd(&cmd);
#endif /* REMOTE_PROC_AUTO_START */

    /* USER CODE END RTOS_QUEUES */
    /* Creation of Task */
    RemoteProcTaskHandle = osThreadNew(RemoteProcTask, NULL, &RemoteProcTaskAttr);
    if (RemoteProcTaskHandle == NULL) {
        App_ErrorHandler();
    }

    RemoteProc_EXTI2_Init();

    /* USER CODE BEGIN RTOS_THREADS */
    /* Add threads, if required */
    /* USER CODE END RTOS_THREADS */

    /* USER CODE BEGIN RTOS_EVENTS */
    /* USER CODE END RTOS_EVENTS */
}

/**
  * @brief  Handles remote processor commands to start, stop, or handle crashes.
  * @param  argument: Not used.
  * @retval None
  */
static void RemoteProcTask(void *argument)
{
    (void) argument;
    RemoteProcCmd cmd;

    for (;;) {
        if (osMessageQueueGet(RemoteProcCmdQueueHandle, &cmd, NULL, osWaitForever) == osOK) {
            switch (cmd.action) {
                case REMOTEPROC_ACTION_START:
                    RemoteProc_HandleStart(cmd.cpu_id);
                    break;
                case REMOTEPROC_ACTION_STOP:
                    RemoteProc_HandleStop(cmd.cpu_id);
                    break;
                case REMOTEPROC_ACTION_CRASH:
                    RemoteProc_HandleCrash(cmd.cpu_id);
                    break;
                default:
                    APP_LOG_ERR("RemoteProc", "Unknown command: %d", cmd.action);
                    break;
            }
            /* Notify other tasks of the state change */
    	    DisplayMessage msg = { .type = DISPLAY_MSG_COPRO_UPDATE, .data = 0 };
    	    LowPowerDisplay_PostMsg(&msg);
        }
    }
}

/**
  * @brief  Handle the start command for the remote processor.
  * @param  cpu_id: ID of the remote processor to start.
  * @retval None
  */
static void RemoteProc_HandleStart(uint32_t cpu_id)
{
    uint32_t err;
    uint32_t start_tick, ticks;

    err = tfm_platform_cpu_info(cpu_id, &remoteProcCoproInfo);
    if (err != TFM_PLATFORM_ERR_SUCCESS) {
        APP_LOG_ERR("RemoteProc", "Failed to get copro info for cpu%lu (err: %ld)", cpu_id, err);
        return;
    }

    if (remoteProcCoproInfo.status < 0 || remoteProcCoproInfo.status >= CPU_LAST) {
        APP_LOG_ERR("RemoteProc", "copro %s error %ld", remoteProcCoproInfo.name, remoteProcCoproInfo.status);
        return;
    }

    if (remoteProcCoproInfo.status != CPU_OFFLINE) {
        APP_LOG_INF("RemoteProc", "copro %s already started, status: %s",
               remoteProcCoproInfo.name, CoproCpuStatusStr[remoteProcCoproInfo.status]);
        return;
    }

    APP_LOG_INF_CONT("RemoteProc", "starting copro %s... ", remoteProcCoproInfo.name);

    err = tfm_platform_cpu_start(cpu_id, &remoteProcCoproInfo.status);
    if (err != TFM_PLATFORM_ERR_SUCCESS) {
        APP_LOG_ERR("RemoteProc", "start fail err: %ld", err);
        return;
    }

    start_tick = osKernelGetTickCount();
    while (remoteProcCoproInfo.status == CPU_STARTED) {
        err = tfm_platform_cpu_info(cpu_id, &remoteProcCoproInfo);
        if (err != TFM_PLATFORM_ERR_SUCCESS) {
            printf("\x1b[31mget copro info fail: %ld \x1b[0m", err);
            remoteProcCoproInfo.status = CPU_LAST;
            break;
        }
        ticks = osKernelGetTickCount();
        if (ticks - start_tick > osKernelGetTickFreq()) {
            printf("\x1b[31mtimeout \x1b[0m");
            break;
        }
    }

    if (remoteProcCoproInfo.status == CPU_RUNNING) {
        printf("\x1b[32mdone\x1b[0m\r\n");
    } else {
        printf("\x1b[31mfailed\x1b[0m");
        APP_LOG_INF("RemoteProc", "copro %s status: %s",
               remoteProcCoproInfo.name, CoproCpuStatusStr[remoteProcCoproInfo.status]);
        tfm_platform_cpu_stop(cpu_id, &remoteProcCoproInfo.status);
    }
}

/**
  * @brief  Handle the stop command for the remote processor.
  * @param  cpu_id: ID of the remote processor to stop.
  * @retval None
  */
static void RemoteProc_HandleStop(uint32_t cpu_id)
{
    uint32_t err;

    err = tfm_platform_cpu_info(cpu_id, &remoteProcCoproInfo);
    if (err != TFM_PLATFORM_ERR_SUCCESS) {
        printf("[NS] [RemoteProc] [ERR] Failed to get copro info for cpu%lu (err: %ld)\r\n", cpu_id, err);
        return;
    }

    if (remoteProcCoproInfo.status < 0 || remoteProcCoproInfo.status >= CPU_LAST) {
        printf("[NS] [RemoteProc] [ERR] copro %s error %ld\r\n", remoteProcCoproInfo.name, remoteProcCoproInfo.status);
        return;
    }

    printf("[NS] [RemoteProc] [INF] stopping copro %s...\r\n", remoteProcCoproInfo.name);
    err = tfm_platform_cpu_stop(cpu_id, &remoteProcCoproInfo.status);
    if (err != TFM_PLATFORM_ERR_SUCCESS) {
        APP_LOG_ERR("RemoteProc", "stop fail err: %ld", err);
        return;
    }
}

/**
  * @brief  Handle the crash command for the remote processor.
  * @param  cpu_id: ID of the remote processor that crashed.
  * @retval None
  */
 static void RemoteProc_HandleCrash(uint32_t cpu_id)
 {
     APP_LOG_CRS("RemoteProc", "Crash detected on cpu%lu. Attempting recovery...", cpu_id);
 
     RemoteProc_HandleStop(cpu_id);
     osDelay(100);
     RemoteProc_HandleStart(cpu_id);
 
     APP_LOG_INF("RemoteProc", "Crash recovery sequence completed.");
 }
 

/**
  * @brief  Request a command to be enqueued for the Remote Processor Task.
  * @param  cmd: Pointer to the command structure to enqueue.
  * @retval HAL_OK if the command was successfully enqueued, HAL_ERROR otherwise.
  */
HAL_StatusTypeDef RemoteProc_PostCmd(const RemoteProcCmd *cmd)
{
    if (RemoteProcCmdQueueHandle == NULL || cmd == NULL) {
        return HAL_ERROR;
    }
    if (osMessageQueuePut(RemoteProcCmdQueueHandle, cmd, 0, 0) == osOK) {
        return HAL_OK;
    }
    return HAL_ERROR;
}

/**
  * @brief  De-initialize the Remote Processor Task and its resources.
  *         Deletes the command queue and terminates the task thread.
  * @retval None
  */
void RemoteProcTask_DeInit(void)
{
    if (RemoteProcCmdQueueHandle != NULL) {
        osMessageQueueDelete(RemoteProcCmdQueueHandle);
        RemoteProcCmdQueueHandle = NULL;
    }
    if (RemoteProcTaskHandle != NULL) {
        osThreadTerminate(RemoteProcTaskHandle);
        RemoteProcTaskHandle = NULL;
    }
    RemoteProc_EXTI2_DeInit();
}

/**
  * @brief  Initialize EXTI2 for the remote processor.
  * @retval None
  */
static void RemoteProc_EXTI2_Init(void)
{
    EXTI_ConfigTypeDef EXTI_ConfigStructure;

    /* Acquire GPIOG8 using Resource manager */
    if (RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIF_EXTI2, RESMGR_EXTI_RESOURCE(47)))
    {
        App_ErrorHandler();
    }

    /* Set configuration except Interrupt and Event mask of Exti line 47*/
    EXTI_ConfigStructure.Line = EXTI2_LINE_47;
    EXTI_ConfigStructure.Trigger = EXTI_TRIGGER_FALLING;
    EXTI_ConfigStructure.Mode = EXTI_MODE_INTERRUPT;
    HAL_EXTI_SetConfigLine(&hexti1, &EXTI_ConfigStructure);

    /* Register callback to treat Exti interrupts in user RemoteProc_Exti2FallingCb function */
    HAL_EXTI_RegisterCallback(&hexti1, HAL_EXTI_FALLING_CB_ID, RemoteProc_Exti2FallingCb);

    /* Enable and set NVIC 4 line  Interrupt to the lowest priority */
    HAL_NVIC_SetPriority(IWDG1_RST_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + DEFAULT_IRQ_PRIO, 0);
    HAL_NVIC_EnableIRQ(IWDG1_RST_IRQn);
}

/**
  * @brief  De-initialize EXTI2 for the remote processor.
  * @retval None
  */
static void RemoteProc_EXTI2_DeInit(void)
{
    /* Release EXTI2 47 using Resource manager */
    ResMgr_Release(RESMGR_RESOURCE_RIF_EXTI2, RESMGR_EXTI_RESOURCE(47));

    /* Disable NVIC 4 Line*/
    HAL_NVIC_DisableIRQ(IWDG1_RST_IRQn);
}

/**
  * @brief  Callback for EXTI2 falling edge interrupt.
  * @retval None
  */
static void RemoteProc_Exti2FallingCb(void)
{
    /* Send the crash command to the remote processor */
    RemoteProcCmd cmd = { .action = REMOTEPROC_ACTION_CRASH, .cpu_id = 0 };
    RemoteProc_PostCmd(&cmd);
}

/**
  * @brief  Get a copy of the remote processor information structure.
  * @retval A copy of the `cpu_info_res` structure.
  */
struct cpu_info_res RemoteProc_GetCoproInfo(void)
{
    return remoteProcCoproInfo;
}

/**
  * @}
  */

/************************ (C) COPYRIGHT MCD Application Team *****END OF FILE****/
