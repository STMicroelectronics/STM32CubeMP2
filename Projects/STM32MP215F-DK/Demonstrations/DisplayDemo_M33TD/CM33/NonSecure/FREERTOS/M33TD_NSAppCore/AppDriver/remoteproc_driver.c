/**
  ******************************************************************************
  * @file    remoteproc_driver.c
  * @brief   Remote processor driver implementation.
  ******************************************************************************
  */

#include "remoteproc_driver.h"
#include "main.h"

static RemoteProcCrashCallback remoteProcCallback = NULL;
EXTI_HandleTypeDef hexti1;

static void RemoteProcDriver_WdgExtiFallingCallback(void);
static int RemoteProcDriver_Init(void);
static int RemoteProcDriver_Deinit(void);
static int RemoteProcDriver_RegisterCrashCallback(RemoteProcCrashCallback cb);
static int RemoteProcDriver_UnregisterCrashCallback(RemoteProcCrashCallback cb);

RemoteProcDriverTypeDef remoteproc_driver = {
  .init = RemoteProcDriver_Init,
  .deinit = RemoteProcDriver_Deinit,
  .register_crash_callback = RemoteProcDriver_RegisterCrashCallback,
  .unregister_crash_callback = RemoteProcDriver_UnregisterCrashCallback,
};

static void RemoteProcDriver_WdgExtiFallingCallback(void)
{
  if (remoteProcCallback)
  {
    remoteProcCallback(0);
  }
}

static int RemoteProcDriver_Init(void)
{
  EXTI_ConfigTypeDef extiConfig;

  if (RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIF_EXTI2, RESMGR_EXTI_RESOURCE(34)))
  {
    Error_Handler();
  }

  extiConfig.Line = EXTI2_LINE_34;
  extiConfig.Trigger = EXTI_TRIGGER_FALLING;
  extiConfig.Mode = EXTI_MODE_INTERRUPT;
  HAL_EXTI_SetConfigLine(&hexti1, &extiConfig);
  HAL_EXTI_RegisterCallback(&hexti1, HAL_EXTI_FALLING_CB_ID, RemoteProcDriver_WdgExtiFallingCallback);

  HAL_NVIC_SetPriority(IWDG1_RST_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + DEFAULT_IRQ_PRIO, 0);
  HAL_NVIC_EnableIRQ(IWDG1_RST_IRQn);

  return 0;
}

static int RemoteProcDriver_Deinit(void)
{
  ResMgr_Release(RESMGR_RESOURCE_RIF_EXTI2, RESMGR_EXTI_RESOURCE(34));
  HAL_NVIC_DisableIRQ(IWDG1_RST_IRQn);
  remoteProcCallback = NULL;
  return 0;
}

static int RemoteProcDriver_RegisterCrashCallback(RemoteProcCrashCallback cb)
{
  remoteProcCallback = cb;
  return 0;
}

static int RemoteProcDriver_UnregisterCrashCallback(RemoteProcCrashCallback cb)
{
  if (remoteProcCallback == cb)
  {
    remoteProcCallback = NULL;
    return 0;
  }

  return -1;
}