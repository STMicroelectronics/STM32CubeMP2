/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usbpd_dpm_core.c
  * @author  MCD Application Team
  * @brief   USBPD dpm core file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

#define __USBPD_DPM_CORE_C

/* Includes ------------------------------------------------------------------*/
#include "usbpd_core.h"
#include "usbpd_trace.h"
#include "usbpd_dpm_core.h"
#include "usbpd_dpm_conf.h"
#include "usbpd_cad_hw_if.h"

/* OS management */
#include "usbpd_os_port_mx.h"

/* Private definition -------------------------------------------------------*/
/* function import prototypes -----------------------------------------------*/
/* Generic STM32 prototypes */
extern uint32_t HAL_GetTick(void);

/* Private function prototypes -----------------------------------------------*/
DEF_TASK_FUNCTION(USBPD_CAD_Task);
DEF_TASK_FUNCTION(USBPD_PE_Task);



/* Private typedef -----------------------------------------------------------*/
#define DPM_STACK_SIZE_ADDON_FOR_CMSIS    4

#define OS_PE_PRIORITY                    osPriorityHigh
#define OS_PE_STACK_SIZE                  (350 * DPM_STACK_SIZE_ADDON_FOR_CMSIS)

#define OS_CAD_PRIORITY                   osPriorityRealtime
#define OS_CAD_STACK_SIZE                 (300 * DPM_STACK_SIZE_ADDON_FOR_CMSIS)

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define CHECK_PE_FUNCTION_CALL(_function_)  do{                                     \
                                                _retr = _function_;                  \
                                               if(USBPD_OK != _retr) {goto error;}   \
                                              } while(0);

#define CHECK_CAD_FUNCTION_CALL(_function_) if(USBPD_CAD_OK != _function_)      \
  {                                   \
    _retr = USBPD_ERROR;              \
    goto error;                       \
  }

#if defined(_DEBUG_TRACE)
#define DPM_CORE_DEBUG_TRACE(_PORTNUM_, __MESSAGE__)  \
  USBPD_TRACE_Add(USBPD_TRACE_DEBUG, _PORTNUM_, 0u, (uint8_t *)(__MESSAGE__), sizeof(__MESSAGE__) - 1u);
#else
#define DPM_CORE_DEBUG_TRACE(_PORTNUM_, __MESSAGE__)
#endif /* _DEBUG_TRACE */

/* Private variables ---------------------------------------------------------*/
static OS_TASK_ID DPM_PEThreadId_Table[USBPD_PORT_COUNT];
static OS_QUEUE_ID CADQueueId = NULL;
static OS_TASK_ID CADThread;
static OS_QUEUE_ID PEQueueId[USBPD_PORT_COUNT] = {NULL};

USBPD_ParamsTypeDef   DPM_Params[USBPD_PORT_COUNT];
/* Private function prototypes -----------------------------------------------*/
static void USBPD_PE_TaskWakeUp(uint8_t PortNum);
static void DPM_StartPETask(uint8_t PortNum);

void USBPD_DPM_CADCallback(uint8_t PortNum, USBPD_CAD_EVENT State, CCxPin_TypeDef Cc);

static void USBPD_DPM_CADTaskWakeUp(void);

/**
  * @brief  Deinitialize the core stack (port power role, PWR_IF, CAD and PE Init procedures)
  * @retval USBPD status
  */
void USBPD_DPM_DeInitCore(void)
{
  /* Initialise the TRACE */
  USBPD_TRACE_DeInit();

  for (uint8_t _port_index = 0; _port_index < USBPD_PORT_COUNT; ++_port_index)
  {
    /* Disable CAD on Port 0 */
    USBPD_CAD_PortEnable(_port_index, USBPD_CAD_DISABLE);
    CAD_DeInit(_port_index);
  }
}

/**
  * @brief  Initialize the core stack (port power role, PWR_IF, CAD and PE Init procedures)
  * @retval USBPD status
  */
USBPD_StatusTypeDef USBPD_DPM_InitCore(void)
{
  /* variable to get dynamic memory allocated by USBPD stack */
  uint32_t stack_dynamemsize;
  USBPD_StatusTypeDef _retr = USBPD_OK;

  /* CAD callback definition */
  static const USBPD_PE_Callbacks dpmCallbacks =
  {
    USBPD_DPM_SetupNewPower,
    USBPD_DPM_HardReset,
    USBPD_DPM_EvaluatePowerRoleSwap,
    USBPD_DPM_Notification,
    USBPD_DPM_ExtendedMessageReceived,
    USBPD_DPM_GetDataInfo,
    USBPD_DPM_SetDataInfo,
    USBPD_DPM_EvaluateRequest,
    USBPD_DPM_SNK_EvaluateCapabilities,
    USBPD_DPM_PowerRoleSwap,
    USBPD_PE_TaskWakeUp,
#if defined(_VCONN_SUPPORT)
    USBPD_DPM_EvaluateVconnSwap,
    USBPD_DPM_PE_VconnPwr,
#else
    NULL,
    NULL,
#endif /* _VCONN_SUPPORT */
    USBPD_DPM_EnterErrorRecovery,
    USBPD_DPM_EvaluateDataRoleSwap,
    USBPD_DPM_IsPowerReady,
    USBPD_DPM_RequestWhatToDo,
  };

  static const USBPD_CAD_Callbacks CAD_cbs =
  {
    USBPD_DPM_CADCallback,
    USBPD_DPM_CADTaskWakeUp
  };

  /* Check the lib selected */
  if (USBPD_TRUE != USBPD_PE_CheckLIB(LIB_ID))
  {
    _retr = USBPD_ERROR;
    goto error;
  }

  /** USBPD_PE_GetMemoryConsumption returns the amount of dynamically allocated memory by the stack for the two managed ports.
    * If the application manage only one port, divide the returned value by 2.
    */
  stack_dynamemsize = USBPD_PE_GetMemoryConsumption();

  /* done to avoid warning */
  (void)stack_dynamemsize;

#if defined(_TRACE) || defined(_GUI_INTERFACE)
  /* Initialise the TRACE */
  USBPD_TRACE_Init();
#endif /* _TRACE || _GUI_INTERFACE */

  for (uint8_t _port_index = 0; _port_index < USBPD_PORT_COUNT; ++_port_index)
  {
    /* Variable to be sure that DPM is correctly initialized */
    DPM_Params[_port_index].DPM_Initialized = USBPD_FALSE;

    /* check the stack settings */
    DPM_Params[_port_index].PE_SpecRevision  = DPM_Settings[_port_index].PE_SpecRevision;
    DPM_Params[_port_index].PE_PowerRole     = DPM_Settings[_port_index].PE_DefaultRole;
    DPM_Params[_port_index].PE_SwapOngoing   = USBPD_FALSE;
    DPM_Params[_port_index].ActiveCCIs       = CCNONE;
    DPM_Params[_port_index].VconnCCIs        = CCNONE;
    DPM_Params[_port_index].VconnStatus      = USBPD_FALSE;

    /* CAD SET UP : Port 0 */
    CHECK_CAD_FUNCTION_CALL(USBPD_CAD_Init(_port_index,
                                           &CAD_cbs,
                                           &DPM_Settings[_port_index],
                                           &DPM_Params[_port_index]));

    /* PE SET UP : Port 0 */
    CHECK_PE_FUNCTION_CALL(USBPD_PE_Init(_port_index, (USBPD_SettingsTypeDef *)&DPM_Settings[_port_index],
                                         &DPM_Params[_port_index], &dpmCallbacks));

    /* DPM is correctly initialized */
    DPM_Params[_port_index].DPM_Initialized = USBPD_TRUE;

    /* Enable CAD on Port 0 */
    USBPD_CAD_PortEnable(_port_index, USBPD_CAD_ENABLE);
  }

error :
  return _retr;
}

/**
  * @brief  Initialize the OS parts (task, queue,... )
  * @retval USBPD status
  */
USBPD_StatusTypeDef USBPD_DPM_InitOS(void)
{
  OS_INIT();
  {
    OS_CREATE_QUEUE(CADQueueId, "QCAD", USBPD_PORT_COUNT, OS_ELEMENT_SIZE);
    OS_DEFINE_TASK(CAD, USBPD_CAD_Task, OS_CAD_PRIORITY, OS_CAD_STACK_SIZE, NULL);
    OS_CREATE_TASK(CADThread, CAD, USBPD_CAD_Task,  OS_CAD_PRIORITY, OS_CAD_STACK_SIZE, ((int)NULL));
  }

  /* Create the queue corresponding to PE task */
  for (uint8_t index = 0; index < USBPD_PORT_COUNT; index++)
  {
    OS_CREATE_QUEUE(PEQueueId[index], "QPE", 1, OS_ELEMENT_SIZE);

    if (index == USBPD_PORT_0)
    {
      /* Tasks definition */
      OS_DEFINE_TASK(PE_0, USBPD_PE_Task, OS_PE_PRIORITY,  OS_PE_STACK_SIZE,  USBPD_PORT_0);
      OS_CREATE_TASK(DPM_PEThreadId_Table[USBPD_PORT_0], PE_0, USBPD_PE_Task,
                     OS_PE_PRIORITY, OS_PE_STACK_SIZE, ((int)index));
    }
#if USBPD_PORT_COUNT > 1
    if (index == USBPD_PORT_1)
    {
      /* Tasks definition */
      OS_DEFINE_TASK(PE_1, USBPD_PE_Task, OS_PE_PRIORITY,  OS_PE_STACK_SIZE,  USBPD_PORT_1);
      OS_CREATE_TASK(DPM_PEThreadId_Table[USBPD_PORT_1], PE_1, USBPD_PE_Task,
                     OS_PE_PRIORITY, OS_PE_STACK_SIZE, ((int)index));
    }
#endif /* USBPD_PORT_COUNT > 1*/
  }
error:
  return _retr;
}

/**
  * @brief  Initialize the OS parts (task, queue,... )
  * @retval USBPD status
  */
void USBPD_DPM_DeInitOS(void)
{
  for (uint8_t index = 0; index < USBPD_PORT_COUNT; index++)
  {
    OS_TASK_KILL(DPM_PEThreadId_Table[index]);
  }

  OS_TASK_KILL(CADThread);
}

/**
  * @brief  Initialize the OS parts (port power role, PWR_IF, CAD and PE Init procedures)
  * @retval None
  */
/* Nothing to do kernel run is managed on CubeMX side */
/* this is obsolete in threadX context void USBPD_DPM_Run(void) */

/**
  * @brief  Initialize DPM (port power role, PWR_IF, CAD and PE Init procedures)
  * @retval USBPD status
  */
void USBPD_DPM_TimerCounter(void)
{
  /* Call PE/PRL timers functions only if DPM is initialized */
  if (USBPD_TRUE == DPM_Params[USBPD_PORT_0].DPM_Initialized)
  {
    USBPD_DPM_UserTimerCounter(USBPD_PORT_0);
    USBPD_PE_TimerCounter(USBPD_PORT_0);
    USBPD_PRL_TimerCounter(USBPD_PORT_0);
  }
#if USBPD_PORT_COUNT==2
  if (USBPD_TRUE == DPM_Params[USBPD_PORT_1].DPM_Initialized)
  {
    USBPD_DPM_UserTimerCounter(USBPD_PORT_1);
    USBPD_PE_TimerCounter(USBPD_PORT_1);
    USBPD_PRL_TimerCounter(USBPD_PORT_1);
  }
#endif /* USBPD_PORT_COUNT == 2 */

}

/**
  * @brief  WakeUp PE task
  * @param  PortNum port number
  * @retval None
  */
static void USBPD_PE_TaskWakeUp(uint8_t PortNum)
{
	if (NULL != PEQueueId[PortNum])
	{
		OS_PUT_MESSAGE_QUEUE(PEQueueId[PortNum], 0xFFFFU, 0U);
	}
}

/**
  * @brief  WakeUp CAD task
  * @retval None
  */
static void USBPD_DPM_CADTaskWakeUp(void)
{
	if (NULL != CADQueueId)
	{
		OS_PUT_MESSAGE_QUEUE(CADQueueId, 0xFFFF, 0);
	}
}

/**
  * @brief  Main task for PE layer
  * @param  argument Not used
  * @retval None
  */
DEF_TASK_FUNCTION(USBPD_PE_Task)
{
  uint8_t _port = (uint32_t)argument;
  uint32_t _timing;

#ifdef _LOW_POWER
  UTIL_LPM_SetOffMode(0 == _port ? LPM_PE_0 : LPM_PE_1, UTIL_LPM_DISABLE);
#endif /* _LOW_POWER */

  for (;;)
  {
    if (DPM_Params[_port].PE_IsConnected == USBPD_FALSE)
    {
      /* if the port is no more connected, suspend the PE thread */
      OS_TASK_SUSPEND(OS_TASK_GETID());
    }

    _timing = USBPD_PE_StateMachine_DRP(_port);
 /* _DRP || ( _SRC && _SNK) */

    OS_GETMESSAGE_QUEUE(PEQueueId[_port], _timing);
    }
  }

/**
  * @brief  Main task for CAD layer
  * @param  argument Not used
  * @retval None
  */
DEF_TASK_FUNCTION(USBPD_CAD_Task)
{
  uint32_t _timing;
#ifdef _LOW_POWER
  UTIL_LPM_SetOffMode(LPM_CAD, UTIL_LPM_DISABLE);
#endif /* _LOW_POWER */
  for (;;)
  {
    _timing = USBPD_CAD_Process();
    OS_GETMESSAGE_QUEUE(CADQueueId, _timing);
  }
}

/**
  * @brief  CallBack reporting events on a specified port from CAD layer.
  * @param  PortNum   The handle of the port
  * @param  State     CAD state
  * @param  Cc        The Communication Channel for the USBPD communication
  * @retval None
  */
void USBPD_DPM_CADCallback(uint8_t PortNum, USBPD_CAD_EVENT State, CCxPin_TypeDef Cc)
{
#ifdef _TRACE
  USBPD_TRACE_Add(USBPD_TRACE_CADEVENT, PortNum, (uint8_t)State, NULL, 0);
#endif /* _TRACE */
  (void)(Cc);
  switch (State)
  {
    case USBPD_CAD_EVENT_ATTEMC :
    {
#if defined(_VCONN_SUPPORT)
      DPM_Params[PortNum].VconnStatus = USBPD_TRUE;
#endif /* _VCONN_SUPPORT */
      USBPD_DPM_UserCableDetection(PortNum, USBPD_CAD_EVENT_ATTEMC);
      DPM_StartPETask(PortNum);
      break;
    }
    case USBPD_CAD_EVENT_ATTACHED :
      USBPD_DPM_UserCableDetection(PortNum, USBPD_CAD_EVENT_ATTACHED);
      DPM_StartPETask(PortNum);
      break;

    case USBPD_CAD_EVENT_DETACHED :
    case USBPD_CAD_EVENT_EMC :
    {
      /* Terminate PE task */
      uint8_t _timeout = 0;
#ifdef _LOW_POWER
      UTIL_LPM_SetStopMode(0 == PortNum ? LPM_PE_0 : LPM_PE_1, UTIL_LPM_ENABLE);
      UTIL_LPM_SetOffMode(0 == PortNum ? LPM_PE_0 : LPM_PE_1, UTIL_LPM_ENABLE);
#endif /* _LOW_POWER */
      /* WakeUp PE task to let him enter suspend mode */
      USBPD_PE_TaskWakeUp(PortNum);
      /* Wait PE Let time to PE to complete the ongoing action */
      while (!OS_TASK_IS_SUSPENDED(DPM_PEThreadId_Table[PortNum]))
      {
        (void)OS_DELAY(1u);
        _timeout++;
        if (_timeout > 30u)
        {
          /* Kill the PE task */
          (void)OS_TASK_KILL(DPM_PEThreadId_Table[PortNum]);
#if defined(_RTOS)
          DPM_PEThreadId_Table[PortNum] = 0;
#elif defined(USBPD_THREADX)
          DPM_PEThreadId_Table[PortNum].tx_thread_id = 0;
#endif /* _RTOS */
          break;
        }
      };
      /* Stop the PE state machine */
      USBPD_PE_StateMachine_Stop(PortNum);
      DPM_Params[PortNum].PE_SwapOngoing = USBPD_FALSE;
      DPM_Params[PortNum].PE_Power   = USBPD_POWER_NO;
      USBPD_DPM_UserCableDetection(PortNum, State);
#ifdef _VCONN_SUPPORT
      DPM_Params[PortNum].VconnStatus = USBPD_FALSE;
#endif /* _VCONN_SUPPORT */
      break;
    }
    default :
      /* nothing to do */
      break;
  }
}

static void DPM_StartPETask(uint8_t PortNum)
{
  USBPD_PE_StateMachine_Reset(PortNum);
  USBPD_StatusTypeDef _retr = USBPD_OK; /* Added for comptibility with the macro OS_CreateTask */
  /* Resume the PE task */
  switch (PortNum)
  {
    case USBPD_PORT_0:
    case USBPD_PORT_1:
    {
      if (DPM_PEThreadId_Table[PortNum] != 0)
      {
        OS_TASK_RESUME(DPM_PEThreadId_Table[PortNum]);
      }
      else
      {
        OS_CREATE_TASK(DPM_PEThreadId_Table[PortNum], PE_0, USBPD_PE_Task, OS_PE_PRIORITY, OS_PE_STACK_SIZE,
                       ((int)PortNum));
      }
      break;
    }
    default :
    {
      USBPD_DPM_ErrorHandler();
      break;
    }
  }
error :
  (void)_retr;
}

__WEAK void USBPD_DPM_ErrorHandler(void)
{
  /* This function is called to block application execution
     in case of an unexpected behavior
     another solution could be to reset application */
  while (1u == 1u) {};
}
