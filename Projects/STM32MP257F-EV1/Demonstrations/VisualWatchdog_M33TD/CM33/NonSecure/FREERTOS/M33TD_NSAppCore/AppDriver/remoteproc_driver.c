/**
  ******************************************************************************
  * @file    remoteproc_driver.c
  * @author  MCD Application Team
  * @brief   Remote processor driver Implementation.
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
  * @addtogroup RemoteProcDriver
  * @{
  */

/* Includes ------------------------------------------------------------------*/
#include "remoteproc_driver.h"
#include "main.h"

/* Private variables ---------------------------------------------------------*/
/**
  * @brief Registered crash callback function pointer.
  */
static RemoteProcCrashCallback remoteProcCallback = NULL;

/* Exported variables --------------------------------------------------------*/
/**
  * @brief EXTI handle for watchdog crash detection.
  */
EXTI_HandleTypeDef hexti1;

/* Private function prototypes -----------------------------------------------*/
static void RemoteProcDriver_WdgExtiFallingCallback(void);
static int RemoteProcDriver_Init(void);
static int RemoteProcDriver_Deinit(void);
static int RemoteProcDriver_RegisterCrashCallback(RemoteProcCrashCallback cb);
static int RemoteProcDriver_UnregisterCrashCallback(RemoteProcCrashCallback cb);

/* Exported driver instance --------------------------------------------------*/
/**
  * @brief Remote processor driver instance.
  */
RemoteProcDriverTypeDef remoteproc_driver =
{
  .init = RemoteProcDriver_Init,
  .deinit = RemoteProcDriver_Deinit,
  .register_crash_callback = RemoteProcDriver_RegisterCrashCallback,
  .unregister_crash_callback = RemoteProcDriver_UnregisterCrashCallback,
};

/* Private functions ---------------------------------------------------------*/

/**
  * @brief EXTI falling edge callback for watchdog crash detection.
  */
static void RemoteProcDriver_WdgExtiFallingCallback(void)
{
  if (remoteProcCallback)
  {
    remoteProcCallback(0); /* Use 0 or the appropriate CPU ID */
  }
}

/**
  * @brief Initialize the remote processor driver.
  * @retval 0 on success.
  */
static int RemoteProcDriver_Init(void)
{
  EXTI_ConfigTypeDef EXTI_ConfigStructure;

  /* Acquire EXTI2 line 47 using Resource manager */
  if (RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIF_EXTI2, RESMGR_EXTI_RESOURCE(47)))
  {
    Error_Handler();
  }

  /* Set configuration except Interrupt and Event mask of EXTI line 47 */
  EXTI_ConfigStructure.Line = EXTI2_LINE_47;
  EXTI_ConfigStructure.Trigger = EXTI_TRIGGER_FALLING;
  EXTI_ConfigStructure.Mode = EXTI_MODE_INTERRUPT;
  HAL_EXTI_SetConfigLine(&hexti1, &EXTI_ConfigStructure);

  /* Register callback to treat EXTI interrupts */
  HAL_EXTI_RegisterCallback(&hexti1, HAL_EXTI_FALLING_CB_ID, RemoteProcDriver_WdgExtiFallingCallback);

  /* Enable and set NVIC 4 line interrupt to the lowest priority */
  HAL_NVIC_SetPriority(IWDG1_RST_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + DEFAULT_IRQ_PRIO, 0);
  HAL_NVIC_EnableIRQ(IWDG1_RST_IRQn);
  return 0;
}

/**
  * @brief De-initialize the remote processor driver.
  * @retval 0 on success.
  */
static int RemoteProcDriver_Deinit(void)
{
  /* Release EXTI2 line 47 using Resource manager */
  ResMgr_Release(RESMGR_RESOURCE_RIF_EXTI2, RESMGR_EXTI_RESOURCE(47));

  /* Disable NVIC 4 line */
  HAL_NVIC_DisableIRQ(IWDG1_RST_IRQn);
  remoteProcCallback = NULL;
  return 0;
}

/**
  * @brief  Register a crash callback for the remote processor driver.
  * @param  cb Callback function pointer.
  * @retval 0 on success.
  */
static int RemoteProcDriver_RegisterCrashCallback(RemoteProcCrashCallback cb)
{
  remoteProcCallback = cb;
  return 0;
}

/**
  * @brief  Unregister a crash callback for the remote processor driver.
  * @param  cb Callback function pointer.
  * @retval 0 on success, -1 if not found.
  */
static int RemoteProcDriver_UnregisterCrashCallback(RemoteProcCrashCallback cb)
{
  if (remoteProcCallback == cb)
  {
    remoteProcCallback = NULL;
    return 0;
  }
  return -1;
}

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
