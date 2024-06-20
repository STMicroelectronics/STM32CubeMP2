/**
  ******************************************************************************
  * @file    ppm.c
  * @author  MCD Application Team
  * @brief   PPM file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "ppm.h"
#include "usbpd_dpm_core.h"
#include "usbpd_dpm_conf.h"
#include "usbpd_dpm_user.h"
#include "usbpd_pwr_if.h"
#include "usbpd_os_port.h"
#if defined(_TRACE)
#include "usbpd_trace.h"
#include "stdio.h"
#endif /* _TRACE */
#include "string.h"
#include "usbpd_ucsi_if.h"

/** @addtogroup STM32_USBPD_LIBRARY
  * @{
  */

/** @addtogroup USBPD_CORE
  * @{
  */

/** @addtogroup USBPD_CORE_PPM
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
typedef struct
{
  UCSI_VERSION_t      UcsiVersion;
  UCSI_SET_NOTIFICATION_ENABLE_t SetNotificationEnable;
  UCSI_GET_CAPABILITY_IN_t Capability;
  UCSI_GET_CONNECTOR_CAPABILITY_IN_t ConnectorCapability[USBPD_PORT_COUNT];
  UCSI_CC_OPERATION_MODE_t CC_Mode[USBPD_PORT_COUNT];
  UCSI_USB_OPERATION_ROLE_t USB_Role[USBPD_PORT_COUNT];
  UCSI_POWER_DIRECTION_MODE_t PowerMode[USBPD_PORT_COUNT];
  UCSI_POWER_DIRECTION_ROLE_t PowerRole[USBPD_PORT_COUNT];
  /* Used for I2C data transfer*/
  UCSI_CCI_t          AckCCI_Status;
  UCSI_CONTROL_t      Control;
  UCSI_MESSAGE_IN_t   MessageIn;
  UCSI_MESSAGE_OUT_t  MessageOut;
  uint8_t           Error;
} PPM_HandleTypeDef;

/* Private define ------------------------------------------------------------*/
/* Optional Features */
#define FEATURE_SET_UOM_SUPPORTED                     0U
#define FEATURE_SET_POWER_LEVEL_SUPPORTED             0U
#if defined(_VDM)
#define FEATURE_ALTERNATEMODEDETAILS_SUPPORTED        0U
#define FEATURE_ALTERNATEMODEOVERRIDE_SUPPORTED       0U
#endif /* _VDM */
#define FEATURE_PDODETAILS_SUPPORTED                  1U
#define FEATURE_CABLEDETAILS_SUPPORTED                0U
#define FEATURE_EXTERNALSUPPLYNOTIFICATION_SUPPORTED  0U
#define FEATURE_PDRESETNOTIFICATION_SUPPORTED         0U
#define FEATURE_GET_PD_MESSAGE_SUPPORTED              0U

#if defined(_TRACE)
#define PPM_DEBUG_NONE     (0U)
#define PPM_DEBUG_LEVEL_0  (1U << 0)
#define PPM_DEBUG_LEVEL_1  (1U << 1)
#define PPM_DEBUG_LEVEL_2  (1U << 2)
#endif /* _TRACE */

#ifndef __IO
#define __IO    volatile        /* __IO macro definition */
#endif /* __IO */

/* Format event
[ PORT | XX | YY | EVENT ]*/
#define MASK_EVENT                              0xFFU
#define SET_PORT_EVENT(__PORT__, __EVENT__)     (((__PORT__) << 24) | (__EVENT__))
#define GET_PORT_EVENT(__EVENT__)               (((__EVENT__) >> 24) & 0xFFU)

typedef enum
{
  PPM_USER_EVENT_TIMER,         /* TIMER EVENT           */
  PPM_USER_EVENT_COMMAND,       /* COMMAND RECEIVED      */
  PPM_USER_EVENT_NOTIFICATION,  /* NOTIFICATION RECEIVED */
  PPM_USER_EVENT_NONE,          /* NO EVENT              */
} PPM_USER_EVENT;

#if !defined(OS_PPM_STACK_ADDITIONAL_SIZE)
/* Used to add stack size in case the alert function in usbpd_ucsi_if needs more */
#define OS_PPM_STACK_ADDITIONAL_SIZE            (0U)
#endif /* !OS_PPM_STACK_ADDITIONAL_SIZE */

#if defined(_RTOS) || defined(USBPD_THREADX)
#if defined(_RTOS)
#define OS_PPM_PRIORITY                         osPriorityLow
#elif defined(USBPD_THREADX)
#define OS_PPM_PRIORITY                         (3U)
#endif /* _RTOS */
#define OS_PPM_STACK_SIZE                       (256U + OS_PPM_STACK_ADDITIONAL_SIZE)
#define PPM_BOX_MESSAGES_MAX                    (2U)
#endif /* _RTOS || USBPD_THREADX */

/* Timeout in ms for which to stay in state PPM_WAIT_FOR_CMD_COMPL_ACK */
#define PPM_WAIT_ACK_TIMEOUT                    (1000U)

typedef enum
{
  PPM_IDLE_NOT_READY,             /* PPM Idle (notification disabled) */
  PPM_IDLE_READY,                 /* PPM Idle (notification enabled)  */
  PPM_BUSY,                       /* PPM Busy                         */
  PPM_WAIT_FOR_ASYNCH_EVENT_ACK,  /* Wait for asynch event ack        */
  PPM_PROCESSING_COMMAND,         /* Processing command               */
  PPM_WAIT_FOR_CMD_COMPL_ACK,     /* Wait for command completion ack  */
} PPM_STATE;

/* Private macro -------------------------------------------------------------*/
#if defined(_TRACE)
#define PPM_DEBUG(_LEVEL_, _PORT_, __MSG__)                                                     \
  do                                                                                            \
  {                                                                                             \
    if((PPM_DebugLevel & (_LEVEL_)) != 0U)                                                      \
    {                                                                                           \
      USBPD_TRACE_Add(USBPD_TRACE_DEBUG, (_PORT_), 0U,(uint8_t*)(__MSG__), strlen(__MSG__));    \
    }                                                                                           \
  } while(0);

#define PPM_DEBUG_WITH_ARG(_LEVEL_, _PORT_, ...)                                                \
  do                                                                                            \
  {                                                                                             \
    if ((PPM_DebugLevel & (_LEVEL_)) != 0U)                                                     \
    {                                                                                           \
      char _str[70U];                                                                           \
      (void)snprintf(_str, 70U, __VA_ARGS__);                                                   \
      USBPD_TRACE_Add(USBPD_TRACE_DEBUG, (uint8_t)(_PORT_), 0U, (uint8_t*)_str, strlen(_str));  \
    }                                                                                           \
  } while(0);
#define PPM_TRACE(_LEVEL_, _ID_, _PTR_DATA_, _SIZE_)                                            \
  do                                                                                            \
  {                                                                                             \
    if ((PPM_DebugLevel & (_LEVEL_)) != 0U)                                                     \
    {                                                                                           \
      USBPD_TRACE_Add(USBPD_TRACE_UCSI, 0U, (uint8_t)_ID_, _PTR_DATA_, _SIZE_);                 \
    }                                                                                           \
  } while(0);
#else
#define PPM_TRACE(_LEVEL_,  _ID_, _PTR_DATA_, _SIZE_)
#define PPM_DEBUG(_LEVEL_, _PORT_, __MSG__)
#define PPM_DEBUG_WITH_ARG(_LEVEL_, _PORT_, ...)
#endif /* _TRACE */

#if defined(_TRACE)
#define SWITCH_TO_PPM_STATE(_STATE_)                                                            \
  do                                                                                            \
  {                                                                                             \
    PPM_State = (_STATE_);                                                                      \
    if (PPM_State != PPM_PreviousState)                                                         \
    {                                                                                           \
      PPM_PreviousState = PPM_State;                                                            \
      PPM_TRACE(PPM_DEBUG_LEVEL_2, UCSI_TRACE_PPM_STATE, (uint8_t*)&PPM_State, 1U);             \
    }                                                                                           \
  } while(0);
#else
#define SWITCH_TO_PPM_STATE(_STATE_)    PPM_State = (_STATE_);
#endif /* _TRACE */

/* Private variables ---------------------------------------------------------*/
#if defined(_TRACE)
/* PPM debug verbose level */
uint8_t PPM_DebugLevel = PPM_DEBUG_LEVEL_2 | PPM_DEBUG_LEVEL_1 | PPM_DEBUG_LEVEL_0 | PPM_DEBUG_NONE;
#endif /* _TRACE */

PPM_HandleTypeDef PPM_Handle;
extern USBPD_HandleTypeDef DPM_Ports[USBPD_PORT_COUNT];

#if defined(_RTOS) || defined(USBPD_THREADX)
static OS_QUEUE_ID PPMMsgBox;
static OS_TASK_ID PPMThread;
DEF_TASK_FUNCTION(USBPD_PPM_UserExecute);
#endif /* _RTOS || USBPD_THREADX */

#if defined(USBPD_THREADX)
static TX_BYTE_POOL *USBPD_memory_pool;
#endif /* USBPD_TREADX */

/* Local variable to store port status */
uint8_t CurrentPort;
uint8_t AsynchEventPending  = 0;
uint8_t CommandPending      = 0;

__IO PPM_STATE PPM_State         = PPM_IDLE_NOT_READY;
__IO PPM_STATE PPM_PreviousState = PPM_IDLE_NOT_READY;



/* Private function prototypes -----------------------------------------------*/
/** @defgroup USBPD_CORE_PPM_Private_Functions USBPD CORE PPM Private Functions
  * @{
  */
static void                PPM_ManageCommandNotReady(PUCSI_CONTROL_t PtrControl);
static void                PPM_ManageCommandReady(PUCSI_CONTROL_t PtrControl);
static void                PPM_NotifyOPM(uint32_t Command, uint32_t Event);
static USBPD_StatusTypeDef PPM_FillDefaultConfiguration(void);
static USBPD_StatusTypeDef PPM_FillConnectorCapability(uint8_t PortNumber,
                                                       PUCSI_GET_CONNECTOR_CAPABILITY_IN_t PtrConnectorCapability);
static USBPD_StatusTypeDef PPM_Reset(void);
static USBPD_StatusTypeDef PPM_Cancel(void);
static USBPD_StatusTypeDef PPM_ConnectorReset(uint8_t PortNumber, uint8_t HardReset);
static USBPD_StatusTypeDef PPM_AckCcCi(uint8_t ConnectorChangeAck, uint8_t CommandCompletedAck);
static USBPD_StatusTypeDef PPM_SetNotificationEnable(uint16_t SetNotificationEnable);
static USBPD_StatusTypeDef PPM_GetCapabilityStatus(void);
static USBPD_StatusTypeDef PPM_GetConnectorCapabilityStatus(uint8_t PortNumber);
#if FEATURE_SET_UOM_SUPPORTED
static USBPD_StatusTypeDef PPM_SetCC_OperationMode(uint8_t PortNumber, UCSI_CC_OPERATION_MODE_t Mode);
#endif /* FEATURE_SET_UOM_SUPPORTED */
static USBPD_StatusTypeDef PPM_SetUSB_OperationRole(uint8_t PortNumber, UCSI_USB_OPERATION_ROLE_t Role);
#if FEATURE_SET_PDM_SUPPORTED
static USBPD_StatusTypeDef PPM_SetPowerDirectionMode(uint8_t PortNumber, UCSI_POWER_DIRECTION_MODE_t PowerMode);
#endif /* FEATURE_SET_PDM_SUPPORTED */
static USBPD_StatusTypeDef PPM_SetPowerDirectionRole(uint8_t PortNumber, UCSI_POWER_DIRECTION_ROLE_t PowerRole);
#if FEATURE_ALTERNATEMODEDETAILS_SUPPORTED
static USBPD_StatusTypeDef PPM_GetAlternateModes(uint8_t PortNumber,
                                                 PUCSI_GET_ALTERNATE_MODES_COMMAND PtrGetAlternatesModes);
static USBPD_StatusTypeDef PPM_GetCamSupported(uint8_t PortNumber);
static USBPD_StatusTypeDef PPM_GetCurrentCam(uint8_t PortNumber);
#if FEATURE_ALTERNATEMODEOVERRIDE_SUPPORTED
static USBPD_StatusTypeDef PPM_SetNewCam(uint8_t PortNumber, uint8_t EnterOrExit, uint8_t NewCAM, uint32_t AMSpecific);
#endif /* FEATURE_ALTERNATEMODEOVERRIDE_SUPPORTED */
#endif /* FEATURE_ALTERNATEMODEDETAILS_SUPPORTED */
#if FEATURE_PDODETAILS_SUPPORTED
static USBPD_StatusTypeDef PPM_GetPDOs(uint8_t PortNumber, PUCSI_GET_PDOS_COMMAND_t PtrGetPdos);
#endif /* FEATURE_PDODETAILS_SUPPORTED */
#if FEATURE_CABLEDETAILS_SUPPORTED
static USBPD_StatusTypeDef PPM_GetCableProperty(uint8_t PortNumber);
#endif /* FEATURE_CABLEDETAILS_SUPPORTED */
static USBPD_StatusTypeDef PPM_GetConnectorStatus(uint8_t PortNumber);
static USBPD_StatusTypeDef PPM_GetErrorStatus(void);
#if FEATURE_SET_POWER_LEVEL_SUPPORTED
static USBPD_StatusTypeDef PPM_SetPowerLevel(PUCSI_SET_POWER_LEVEL_COMMAND PtrSetPowerLevel);
#endif /* FEATURE_SET_POWER_LEVEL_SUPPORTED */
#if FEATURE_GET_PD_MESSAGE_SUPPORTED
static USBPD_StatusTypeDef PPM_GetPDMessage(PUCSI_GET_PD_MESSAGE_COMMAND PtrGetPDMessage);
#endif /* FEATURE_GET_PD_MESSAGE_SUPPORTED */
static USBPD_StatusTypeDef PPM_UnknownCommand(uint8_t PortNumber);

extern uint32_t HAL_GetTick(void);

/**
  * @}
  */


/** @defgroup USBPD_CORE_PPM_Exported_Functions USBPD CORE PPM Exported Functions
  * @{
  */
#define UCSI_CONNECTOR_IDLE   0x00U
#define UCSI_CONNECTOR_CHANGE 0x01U
uint8_t ConnectorChange[USBPD_PORT_COUNT];
static uint32_t tickstart = 0;

/**
  * @brief  Deinitialized USB-PD stack
  * @retval None
  */
void USBPD_PPM_DeInit(void)
{
  /* kill the ppm task */
  OS_TASK_KILL(PPMThread);
}

/**
  * @brief  Initialize the default configuration used by the USB-PD stack
  * @retval USBPD status
  */
#if defined(USBPD_THREADX)
USBPD_StatusTypeDef USBPD_PPM_Init(VOID *MemoryPtr)
#else
USBPD_StatusTypeDef USBPD_PPM_Init(void)
#endif /* USBPD_THREADX */
{
  uint32_t index;
  PPM_TRACE(PPM_DEBUG_LEVEL_0, UCSI_TRACE_PPM_INIT, NULL, 0U);

  /* Initialize command lists */
  PPM_Handle.Error                  = 0U;
  PPM_Handle.AckCCI_Status.AsUInt32 = 0U;

  PPM_FillDefaultConfiguration();

  /* Initialize each USB-PD connector capability */
  for (index = 0; index < USBPD_PORT_COUNT; index++)
  {
    PPM_FillConnectorCapability(index, &PPM_Handle.ConnectorCapability[index]);
    ConnectorChange[index] = UCSI_CONNECTOR_IDLE;
  }

#if defined(_RTOS) || defined(USBPD_THREADX)
  OS_INIT();
  OS_CREATE_QUEUE(PPMMsgBox, "PPM", PPM_BOX_MESSAGES_MAX, sizeof(uint32_t));
  OS_DEFINE_TASK(PPM, USBPD_PPM_UserExecute, OS_PPM_PRIORITY, OS_PPM_STACK_SIZE, &PPMMsgBox);
  OS_CREATE_TASK(PPMThread, PPM, USBPD_PPM_UserExecute, OS_PPM_PRIORITY, OS_PPM_STACK_SIZE, &PPMMsgBox);
#else
#warning "NRTOS Version not implemented"
#endif /* _RTOS || USBPD_THREADX */

  (void)_retr; /* To avoid gcc/g++ warnings */
  return USBPD_OK;
#if defined(_RTOS) || defined(USBPD_THREADX)
error:
  return USBPD_ERROR;
#endif /* _RTOS || USBPD_THREADX */
}

#if defined(_RTOS) || defined(USBPD_THREADX)
DEF_TASK_FUNCTION(USBPD_PPM_UserExecute)
{
#if defined(USBPD_THREADX)
  uint32_t _timing = TX_WAIT_FOREVER;
#elif defined(_RTOS)
  uint32_t _timing = osWaitForever;
#endif /* USBPD_THREADX */

  OS_QUEUE_ID *queue = (OS_QUEUE_ID *)argument;

  do
  {
#if defined(USBPD_THREADX)
    ULONG event = 0;
    tx_queue_receive(queue, (void *)&event, _timing);
    switch (((PPM_USER_EVENT)event & MASK_EVENT))
#elif defined(_RTOS)
#if (osCMSIS < 0x20000U)
    osEvent event = osMessageGet(queue, _timing);
    switch (((PPM_USER_EVENT)event.value.v & MASK_EVENT))
#else
    uint32_t event;
    if(osMessageQueueGet(*queue, &event, NULL, _timing) != osOK) {
      event = PPM_USER_EVENT_NONE;
    }
    switch (((PPM_USER_EVENT)event & MASK_EVENT))
#endif /* osCMSIS < 0x20000U */
#endif /* USBPD_THREADX */
    {
      case PPM_USER_EVENT_COMMAND:
      {
        PUCSI_CONTROL_t ptr_control = (PUCSI_CONTROL_t)&PPM_Handle.Control;
        switch (PPM_State)
        {
          case PPM_IDLE_NOT_READY:             /* PPM Idle (notification disabled) */
            PPM_ManageCommandNotReady(ptr_control);
            break;
          default:
            PPM_ManageCommandReady(ptr_control);
            break;
        }
        break;
      }
      case PPM_USER_EVENT_NOTIFICATION:
      case PPM_USER_EVENT_NONE:
      case PPM_USER_EVENT_TIMER:
      default:
        break;
    }

    /* Manage PPM State Machine */
    switch (PPM_State)
    {
      case PPM_IDLE_READY:                 /* PPM Idle (notification enabled) */
        if (0U != AsynchEventPending)
        {
          SWITCH_TO_PPM_STATE(PPM_WAIT_FOR_ASYNCH_EVENT_ACK);
        }
        break;
      case PPM_IDLE_NOT_READY:             /* PPM Idle (notification disabled) */
        AsynchEventPending = 0U;
        break;
      case PPM_WAIT_FOR_ASYNCH_EVENT_ACK:  /* Wait for asynch event ack */
        if (0U != AsynchEventPending)
        {
          /** Acknowledge Command Indicator:
            * The PPM shall set this field to one when it completes the ACK_CC_CI
            * (Acknowledge Command Completion and/or Change Indication) command. The PPM shall automatically reset this
            * bit when it receives the next command from the OPM.
            * If this field is set to one, then the only other field that can be set is the Connector Change
            * Indicator field.
            */
          PPM_Handle.AckCCI_Status.AsUInt32 &= 0x20000000U;

          PPM_Handle.AckCCI_Status.ConnectorChangeIndicator = AsynchEventPending;
          if (PPM_Handle.Error == 1U)
          {
            PPM_Handle.AckCCI_Status.ErrorIndicator = 1U;
            PPM_Handle.Error = 0U;
          }
          PPM_NotifyOPM(0U, 1U);
          PPM_TRACE(PPM_DEBUG_LEVEL_1, UCSI_TRACE_PPM_EVENT, (uint8_t *)&PPM_Handle.AckCCI_Status.AsUInt32, 4U);
          /* Clear Async event pending */
          AsynchEventPending = 0U;
        }
        break;
      case PPM_BUSY:                       /* PPM Busy                        */
      case PPM_PROCESSING_COMMAND:         /* Processing command              */
      case PPM_WAIT_FOR_CMD_COMPL_ACK:     /* Wait for command completion ack */
        if (PPM_Handle.AckCCI_Status.AcknowledgeCommandIndicator != 0U)
        {
          /* Switch to PPM Idle (Notification enabled) */
          SWITCH_TO_PPM_STATE(PPM_IDLE_READY);
        }
        else
        {
          /* Manage timeout in case OPM never ack command */
          if ((HAL_GetTick() - tickstart) > PPM_WAIT_ACK_TIMEOUT) /* 1s timeout */
          {
            PPM_DEBUG(PPM_DEBUG_LEVEL_1, USBPD_PORT_0,
                      "PPM_WAIT_FOR_CMD_COMPL_ACK timeout. Switching to state PPM_IDLE_READY");

            /* Abort waiting */
            PPM_Handle.AckCCI_Status.AcknowledgeCommandIndicator = 1U;
            SWITCH_TO_PPM_STATE(PPM_IDLE_READY);
          }
        }
        break;
      default:
        break;
    }

    if (0U != AsynchEventPending)
    {
      /* 2 Asynch has been raised. 1st one has been correctly managed. */
#if defined(USBPD_THREADX)
      _timing = TX_NO_WAIT;
#elif defined(_RTOS)
      _timing = 0U;
#endif /* USBPD_THREADX */
    }
    else
    {
#if defined(USBPD_THREADX)
      _timing = TX_WAIT_FOREVER;
#elif defined(_RTOS)
      _timing = osWaitForever;
#endif /* USBPD_THREADX */
    }
  } while (1);
}
#else
#warning "NRTOS Version not implemented"
#endif /* _RTOS || USBPD_THREADX */

/**
  * @brief  Get data pointer saved in PPM Handle
  * @param  Register
  * @retval Pointer of data
  */
uint8_t *USBPD_PPM_GetDataPointer(UCSI_REG_t Register)
{
#if defined(_TRACE)
  if (Register < UcsiRegDataStructureUnkown)
  {
    PPM_TRACE(PPM_DEBUG_LEVEL_2, UCSI_TRACE_PPM_GET_REG, (uint8_t *)&Register, 1U);
  }
#endif /* _TRACE */
  uint8_t *ptr = NULL;
  switch (Register)
  {
    case UcsiRegDataStructureVersion:
      ptr = (uint8_t *)&PPM_Handle.UcsiVersion;
      break;
    case UcsiRegDataStructureCci:
      ptr = (uint8_t *)&PPM_Handle.AckCCI_Status;
      break;
    case UcsiRegDataStructureControl:
      ptr = (uint8_t *)&PPM_Handle.Control;
      break;
    case UcsiRegDataStructureMessageIn:
      ptr = (uint8_t *)&PPM_Handle.MessageIn;
      PPM_TRACE(PPM_DEBUG_LEVEL_1, UCSI_TRACE_PPM_GET_REG, (uint8_t *)&PPM_Handle.MessageIn, UCSI_MAX_DATA_LENGTH);
      break;
    case UcsiRegDataStructureMessageOut:
      ptr = (uint8_t *)&PPM_Handle.MessageOut;
      break;
    case UcsiRegDataStructureUnkown:
    case UcsiRegDataStructureReserved:
    default:
      PPM_TRACE(PPM_DEBUG_LEVEL_0, UCSI_TRACE_PPM_GET_REG_ERROR_READ,
                (uint8_t *)&PPM_Handle.AckCCI_Status.AsUInt32, 4U);
      break;
  }

  return ptr;
}

/**
  * @brief  Callback function called by PE to inform DPM about PE event.
  * @param  PortNumber  The current port number
  * @param  EventVal    USBPD_NotifyEventValue_TypeDef
  * @retval None
  */
void USBPD_PPM_Notification(uint8_t PortNumber, uint32_t EventType)
{
  /*
  Mandatory:
  PPM_Handle.SetNotificationEnable.CommandComplete;
  PPM_Handle.SetNotificationEnable.PowerOperationModeChange;
  PPM_Handle.SetNotificationEnable.BatteryChargingStatusChange;
  PPM_Handle.SetNotificationEnable.ConnectorPartnerChange;
  PPM_Handle.SetNotificationEnable.PowerDirectionChange;
  PPM_Handle.SetNotificationEnable.ConnectChange;
  PPM_Handle.SetNotificationEnable.Error;

  Optional: */
#if FEATURE_EXTERNALSUPPLYNOTIFICATION_SUPPORTED
  PPM_Handle.SetNotificationEnable.ExternalSupplyChange;
#endif /* FEATURE_EXTERNALSUPPLYNOTIFICATION_SUPPORTED */
#if FEATURE_PDODETAILS_SUPPORTED
  PPM_Handle.SetNotificationEnable.SupportedProviderCapabilitiesChange = 1U;
  PPM_Handle.SetNotificationEnable.NegotiatedPowerLevelChange = 1U;
#endif /* FEATURE_PDODETAILS_SUPPORTED */
#if FEATURE_PDRESETNOTIFICATION_SUPPORTED
  PPM_Handle.SetNotificationEnable.PdResetComplete;
#endif /* FEATURE_PDRESETNOTIFICATION_SUPPORTED */
#if FEATURE_ALTERNATEMODEDETAILS_SUPPORTED
  PPM_Handle.SetNotificationEnable.SupportedCamChange;
#endif /* FEATURE_ALTERNATEMODEDETAILS_SUPPORTED */

  if (PPM_Handle.SetNotificationEnable.ConnectChange != 0U)
  {
    switch (EventType)
    {
      case USBPD_NOTIFY_DATAROLESWAP_NOT_SUPPORTED:
        PPM_Handle.Error                  = 1U;
      /* Event for attach */
      case USBPD_NOTIFY_POWER_EXPLICIT_CONTRACT:
      /* Event for detach */
      case USBPD_NOTIFY_DETACH:
      /* Event for Data Role Swap  */
      case USBPD_NOTIFY_DATAROLESWAP_UFP:
      case USBPD_NOTIFY_DATAROLESWAP_DFP:
      case USBPD_NOTIFY_USBSTACK_START:
      case USBPD_NOTIFY_PE_DISABLED:
        /* A notification has been detected */
      {
        uint32_t event = SET_PORT_EVENT(PortNumber, PPM_USER_EVENT_NOTIFICATION);
        ConnectorChange[PortNumber] = UCSI_CONNECTOR_CHANGE;
        /* Wake up PPM only if there are no pending Asynck event */
        if (AsynchEventPending == 0U)
        {
          AsynchEventPending = (PortNumber + 1U);
#if defined(USBPD_THREADX)
          ULONG _msg = event;
          if (TX_SUCCESS != tx_queue_send(&PPMMsgBox, &_msg, 0U))
          {
            PPM_DEBUG(PPM_DEBUG_LEVEL_0, 0U, "COMMAND: MB FULL\r\n");
          }
#elif defined(_RTOS)
#if (osCMSIS < 0x20000U)
          if (osOK != osMessagePut(PPMMsgBox, event, 0U))
          {
            PPM_DEBUG(PPM_DEBUG_LEVEL_0, PortNumber, "NOTIF1: MB FULL\r\n");
          }
#else
          (void)osMessageQueuePut(PPMMsgBox, &event, 0U, 0);
#endif /* osCMSIS < 0x20000U */
#else
#warning " NRTOS Version not implemented"
#endif /* USBPD_THREADX */
        }
      }
      break;

      default:
        break;
    }
  }

  /* No notifications match */
  return;
}

void USBPD_PPM_PostCommand(void)
{
  uint32_t event = PPM_USER_EVENT_COMMAND;
#if defined(USBPD_THREADX)
  ULONG _msg = event;
  if (TX_SUCCESS != tx_queue_send(&PPMMsgBox, &_msg, 0U))
  {
    PPM_DEBUG(PPM_DEBUG_LEVEL_0, 0U, "COMMAND: MB FULL\r\n");
  }
#elif defined(_RTOS)
#if (osCMSIS < 0x20000U)
  if (osOK != osMessagePut(PPMMsgBox, event, 0U))
  {
    PPM_DEBUG(PPM_DEBUG_LEVEL_0, 0U, "COMMAND: MB FULL\r\n");
  }
#else
 (void)osMessageQueuePut(PPMMsgBox, &event, 0U, 0);
#endif /* osCMSIS < 0x20000U */
#else
#warning " NRTOS Version not implemented"
#endif /* USBPD_THREADX */
  /* PPM Update Command Pending */
  CommandPending = 1U;
}

/**
  * @}
  */

/** @addtogroup USBPD_CORE_PPM_Private_Functions
  * @{
  */
static void PPM_ManageCommandNotReady(PUCSI_CONTROL_t PtrControl)
{
  UCSI_COMMAND_t command = (UCSI_COMMAND_t)PtrControl->Command;
  PPM_TRACE(PPM_DEBUG_LEVEL_0, UCSI_TRACE_PPM_NOT_READY, (uint8_t *)PtrControl, 8U);
  if (UcsiCommandPpmReset == command)
  {
    /* Disable all the notifications */
    PPM_Reset();
  }
  else if (UcsiCommandSetNotificationEnable == command)
  {
    PPM_SetNotificationEnable(PtrControl->SetNotificationEnable.Content.NotificationEnable);

    PPM_NotifyOPM(1U, 0U);
    /* Save time, for timeout mechanism */
	tickstart = HAL_GetTick();
    SWITCH_TO_PPM_STATE(PPM_WAIT_FOR_CMD_COMPL_ACK);
  }
  else
  {
    /* Nothing to do */
  }

  /* Clear Command pending */
  CommandPending = 0U;
}

static void PPM_ManageCommandReady(PUCSI_CONTROL_t PtrControl)
{
  UCSI_COMMAND_t command = (UCSI_COMMAND_t)PtrControl->Command;
  PPM_TRACE(PPM_DEBUG_LEVEL_0, UCSI_TRACE_PPM_READY, (uint8_t *)PtrControl, 8U);

  if (UcsiCommandPpmReset == command)
  {
    /* Disable all the notifications */
    PPM_Reset();
  }
  else if (UcsiCommandCancel == command)
  {
    /* Cancel current operation if not yet processing */
    PPM_Cancel();

    /* Notificy OPM */
    PPM_NotifyOPM(1U, 0U);
  }
  else
  {
    /* if PPM is busy or command need to take more one UCSI_MIN_TIME_TO_RESPOND_WITH_BUSY ms set busy indicator */
    if (PPM_BUSY == PPM_State) /* || Cmd Need UCSI_MIN_TIME_TO_RESPOND_WITH_BUSY */
    {
      /* Reset Status variable */
      PPM_Handle.AckCCI_Status.AsUInt32 = 0U;
      /* Acknowledge the command */
      PPM_Handle.AckCCI_Status.BusyIndicator = 1U;
      /* Notificy OPM */
      PPM_NotifyOPM(1U, 0U);
      SWITCH_TO_PPM_STATE(PPM_PROCESSING_COMMAND);
    }
    else
    {
      switch (command)
      {
        case UcsiCommandConnectorReset:
          CurrentPort = (uint8_t)(PtrControl->ConnectorReset.ConnectorNumber - 1U);
          PPM_ConnectorReset(CurrentPort, (uint8_t)PtrControl->ConnectorReset.HardReset);
          break;
        case UcsiCommandAckCcCi:
          CurrentPort = (uint8_t)(PtrControl->ConnectorReset.ConnectorNumber - 1U);
          PPM_AckCcCi((uint8_t)PtrControl->AckCcCi.ConnectorChangeAcknowledge,
                      (uint8_t)PtrControl->AckCcCi.CommandCompletedAcknowledge);
          break;
        case UcsiCommandSetNotificationEnable:
          PPM_SetNotificationEnable(PtrControl->SetNotificationEnable.Content.NotificationEnable);
          break;
        case UcsiCommandGetCapability:
          PPM_GetCapabilityStatus();
          break;
        case UcsiCommandGetConnectorCapability:
          CurrentPort = (uint8_t)(PtrControl->GetConnectorCapability.ConnectorNumber - 1U);
          PPM_GetConnectorCapabilityStatus(CurrentPort);
          break;
        case UcsiCommandSetUor:
          CurrentPort = (uint8_t)(PtrControl->SetUor.ConnectorNumber - 1U);
          PPM_SetUSB_OperationRole(CurrentPort, (UCSI_USB_OPERATION_ROLE_t)PtrControl->SetUor.UsbOperationRole);
          break;
        case UcsiCommandSetPdr:
          CurrentPort = (uint8_t)(PtrControl->SetPdr.ConnectorNumber - 1U);
          PPM_SetPowerDirectionRole(CurrentPort, (UCSI_POWER_DIRECTION_ROLE_t)PtrControl->SetPdr.PowerDirectionRole);
          break;
        case UcsiCommandGetConnectorStatus:
          CurrentPort = (uint8_t)(PtrControl->GetConnectorStatus.ConnectorNumber - 1U);
          PPM_GetConnectorStatus(CurrentPort);
          break;
        case UcsiCommandGetErrorStatus:
          PPM_GetErrorStatus();
          break;
        case UcsiCommandSetCCom:
          CurrentPort = (uint8_t)(PtrControl->SetUom.ConnectorNumber - 1U);
#if FEATURE_SET_UOM_SUPPORTED
          PPM_SetCC_OperationMode(CurrentPort, (UCSI_CC_OPERATION_MODE_t)PtrControl->SetUom.UsbOperationMode);
#else
          PPM_TRACE(PPM_DEBUG_LEVEL_0, UCSI_TRACE_PPM_READY_UNKNOWN, NULL, 0U);
          PPM_UnknownCommand(CurrentPort);
#endif /* FEATURE_SET_UOM_SUPPORTED */
          break;
        /* Obsolete from Rev1.2, Jan20*/
        case UcsiCommandSetPdm:
          CurrentPort = (uint8_t)(PtrControl->SetPdm.ConnectorNumber - 1U);
#if FEATURE_SET_PDM_SUPPORTED
          PPM_SetPowerDirectionMode(CurrentPort, (UCSI_POWER_DIRECTION_MODE_t)PtrControl->SetPdm.PowerDirectionMode);
#else
          PPM_TRACE(PPM_DEBUG_LEVEL_0, UCSI_TRACE_PPM_READY_UNKNOWN, NULL, 0U);
          PPM_UnknownCommand(CurrentPort);
#endif /* FEATURE_SET_PDM_SUPPORTED */
          break;
        case UcsiCommandGetAlternateModes:
          CurrentPort = (uint8_t)(PtrControl->GetAlternateModes.ConnectorNumber - 1U);
#if FEATURE_ALTERNATEMODEDETAILS_SUPPORTED
          PPM_GetAlternateModes(CurrentPort, &PtrControl->GetAlternateModes);
#else
          PPM_TRACE(PPM_DEBUG_LEVEL_0, UCSI_TRACE_PPM_READY_UNKNOWN, NULL, 0U);
          PPM_UnknownCommand(CurrentPort);
#endif /* FEATURE_ALTERNATEMODEDETAILS_SUPPORTED */
          break;
        case UcsiCommandGetCamSupported:
          CurrentPort = (uint8_t)(PtrControl->GetCamSupported.ConnectorNumber - 1U);
#if FEATURE_ALTERNATEMODEDETAILS_SUPPORTED
          PPM_GetCamSupported(CurrentPort);
#else
          PPM_TRACE(PPM_DEBUG_LEVEL_0, UCSI_TRACE_PPM_READY_UNKNOWN, NULL, 0U);
          PPM_UnknownCommand(CurrentPort);
#endif /* FEATURE_ALTERNATEMODEDETAILS_SUPPORTED */
          break;
        case UcsiCommandGetCurrentCam:
          CurrentPort = (uint8_t)(PtrControl->GetCurrentCam.ConnectorNumber - 1U);
#if FEATURE_ALTERNATEMODEDETAILS_SUPPORTED
          PPM_GetCurrentCam(CurrentPort);
#else
          PPM_TRACE(PPM_DEBUG_LEVEL_0, UCSI_TRACE_PPM_READY_UNKNOWN, NULL, 0U);
          PPM_UnknownCommand(CurrentPort);
#endif /* FEATURE_ALTERNATEMODEDETAILS_SUPPORTED */
          break;
        case UcsiCommandSetNewCam:
          CurrentPort = (uint8_t)(PtrControl->SetNewCam.ConnectorNumber - 1U);
#if FEATURE_ALTERNATEMODEDETAILS_SUPPORTED && FEATURE_ALTERNATEMODEOVERRIDE_SUPPORTED
          PPM_SetNewCam(CurrentPort, PtrControl->SetNewCam.EnterOrExit, PtrControl->SetNewCam.NewCam,
                        PtrControl->SetNewCam.AmSpecific);
#else
          PPM_TRACE(PPM_DEBUG_LEVEL_0, UCSI_TRACE_PPM_READY_UNKNOWN, NULL, 0U);
          PPM_UnknownCommand(CurrentPort);
#endif /* FEATURE_ALTERNATEMODEDETAILS_SUPPORTED && FEATURE_ALTERNATEMODEOVERRIDE_SUPPORTED */
          break;
        case UcsiCommandGetPdos:
          CurrentPort = (uint8_t)(PtrControl->GetPdos.ConnectorNumber - 1U);
#if FEATURE_PDODETAILS_SUPPORTED
          PPM_GetPDOs(CurrentPort, &PtrControl->GetPdos);
#else
          PPM_TRACE(PPM_DEBUG_LEVEL_0, UCSI_TRACE_PPM_READY_UNKNOWN, NULL, 0U);
          PPM_UnknownCommand(CurrentPort);
#endif /* FEATURE_PDODETAILS_SUPPORTED */
          break;
        case UcsiCommandGetCableProperty:
          CurrentPort = (uint8_t)(PtrControl->GetCableProperty.ConnectorNumber - 1U);
#if FEATURE_CABLEDETAILS_SUPPORTED
          PPM_GetCableProperty(CurrentPort);
#else
          PPM_TRACE(PPM_DEBUG_LEVEL_0, UCSI_TRACE_PPM_READY_UNKNOWN, NULL, 0U);
          PPM_UnknownCommand(CurrentPort);
#endif /* FEATURE_CABLEDETAILS_SUPPORTED */
          break;
        case UcsiCommandSetPowerLevel:
          CurrentPort = (uint8_t)(PtrControl->SetPowerLevel.ConnectorNumber - 1U);
#if FEATURE_SET_POWER_LEVEL_SUPPORTED
          PPM_SetPowerLevel(PtrControl->SetPowerLevel);
#else
          PPM_TRACE(PPM_DEBUG_LEVEL_0, UCSI_TRACE_PPM_READY_UNKNOWN, NULL, 0U);
          PPM_UnknownCommand(CurrentPort);
#endif /* FEATURE_SET_POWER_LEVEL_SUPPORTED */
          break;
        case UcsiCommandGetPDMessage:
          CurrentPort = (uint8_t)(PtrControl->GetPDMessage.ConnectorNumber - 1U);
#if FEATURE_GET_PD_MESSAGE_SUPPORTED
          PPM_GetPDMessage(PtrControl->GetPDMessage);
#else
          PPM_TRACE(PPM_DEBUG_LEVEL_0, UCSI_TRACE_PPM_READY_UNKNOWN, NULL, 0U);
          PPM_UnknownCommand(CurrentPort);
#endif /* FEATURE_GET_PD_MESSAGE_SUPPORTED */
          break;
        default:
          PPM_TRACE(PPM_DEBUG_LEVEL_0, UCSI_TRACE_PPM_READY_UNKNOWN, NULL, 0U);
          PPM_UnknownCommand(CurrentPort);
          break;
      }
      /* Notificy OPM */
      PPM_NotifyOPM(1U, 0U);
      /* Save time, for timeout mechanism */
      tickstart = HAL_GetTick();
      SWITCH_TO_PPM_STATE(PPM_WAIT_FOR_CMD_COMPL_ACK);
    }
  }
  /* Clear Command pending */
  CommandPending = 0U;
}

static void PPM_NotifyOPM(uint32_t Command, uint32_t Event)
{
  /* Prevent unused argument compilation warning */
  (void)Event;

  /* Notify OPM only if "Command Completed" notification is enabled */
  if (1U == Command)
  {
    /* Notify only if Command complete is enabled */
    if (1U == PPM_Handle.SetNotificationEnable.CommandComplete)
    {
      USBPD_UCSI_SendNotification(CurrentPort);
    }
  }
  else
  {
    /* Check if associated events is enabled */
    USBPD_UCSI_SendNotification(CurrentPort);
  }
  /* Cleart the ALERT */
  USBPD_UCSI_ClearAlert(CurrentPort);
}

/**
  * @brief  Manage reception of PPM_RESET message (Required)
  * @retval USBPD status
  */
static USBPD_StatusTypeDef PPM_Reset(void)
{
  /* Reset Status variable */
  PPM_Handle.AckCCI_Status.AsUInt32 = 0U;

  /* Acknowledge the command */
  PPM_Handle.AckCCI_Status.ResetCompletedIndicator = 1U;

  /* Disable all the notifications */
  PPM_Handle.SetNotificationEnable.NotificationEnable = 0x0000U;

  /* USB Operation Mode should be reset as default value DRP
    (can be changed with Set USB Operation Mode command) */
  for (uint32_t index = 0U; index < USBPD_PORT_COUNT; index++)
  {
    PPM_Handle.CC_Mode[index] = (UCSI_CC_OPERATION_MODE_t)(PPM_Handle.ConnectorCapability[index].OperationMode.AsUInt8 &
                                                           0x07U);
  }

  /* Switch state to PPM Idle (Notifications disabled) */
  SWITCH_TO_PPM_STATE(PPM_IDLE_NOT_READY);

  return USBPD_OK;
}

/**
  * @brief  Manage reception of CANCEL message (Required)
  * @retval USBPD status
  */
static USBPD_StatusTypeDef PPM_Cancel(void)
{
  /* Reset Status variable */
  PPM_Handle.AckCCI_Status.AsUInt32 = 0U;

  /* Acknowledge the command */
  PPM_Handle.AckCCI_Status.CancelCompletedIndicator  = 1U;
  PPM_Handle.AckCCI_Status.CommandCompletedIndicator = 1U;
  PPM_Handle.AckCCI_Status.ConnectorChangeIndicator  = AsynchEventPending;

  return USBPD_OK;
}

/**
  * @brief  Manage reception of CONNECTOR_RESET message (Required)
  * @param  PortNumber Port Number value
  * @param  HardReset  Request an hard or soft reset
  * @retval USBPD status
  */
static USBPD_StatusTypeDef PPM_ConnectorReset(uint8_t PortNumber, uint8_t HardReset)
{
  /* Reset Status variable */
  PPM_Handle.AckCCI_Status.AsUInt32 = 0U;

  /* Acknowledge the command */
  PPM_Handle.AckCCI_Status.CommandCompletedIndicator = 1U;
  PPM_Handle.AckCCI_Status.ConnectorChangeIndicator  = AsynchEventPending;

  if (HardReset != 0U)
  {
    /* Send a hard reset */
    USBPD_DPM_RequestHardReset(PortNumber);
  }
  else
  {
    /* Send a soft reset */
    /* Soft reset cannot be initiated by application. This is only a protocol error detected by the stack */
  }

  return USBPD_OK;
}

/**
  * @brief  Manage reception of ACK_CC_CCI message (Required)
  * @param  ConnectorChangeAck  Acknowledge a connector change
  * @param  CommandCompletedAck Acknowledge a command completed
  * @retval USBPD status
  */
static USBPD_StatusTypeDef PPM_AckCcCi(uint8_t ConnectorChangeAck, uint8_t CommandCompletedAck)
{
  /* Reset Status variable */
  PPM_Handle.AckCCI_Status.AsUInt32 = 0U;

  /* Acknowledge the command */
  PPM_Handle.AckCCI_Status.AcknowledgeCommandIndicator = 1U;

  /* Prevent unused argument compilation warning */
  (void)CommandCompletedAck;

  /* Reset the ConnectorStatusChange */
  (&PPM_Handle.MessageIn.ConnectorStatus)->ConnectorStatusChange.AsUInt16 = 0x0000U;

  if (ConnectorChangeAck != 0U)
  {
    uint8_t index;
    for (index = 0U; index < USBPD_PORT_COUNT; index++)
    {
      if (ConnectorChange[index] == UCSI_CONNECTOR_CHANGE)
      {
        AsynchEventPending = (index + 1U);
        break;
      }
    }
  }
  return USBPD_OK;
}

/**
  * @brief  Enable Notification (Required)
  * @param  SetNotificationEnable Value of the SET_NOTIF_ENABLE
  * @retval None
  */
static USBPD_StatusTypeDef PPM_SetNotificationEnable(uint16_t SetNotificationEnable)
{
  /* Reset Status variable */
  PPM_Handle.AckCCI_Status.AsUInt32 = 0U;

  /* Acknowledge the command */
  PPM_Handle.AckCCI_Status.CommandCompletedIndicator = 1U;
  PPM_Handle.AckCCI_Status.ConnectorChangeIndicator  = AsynchEventPending;

  PPM_Handle.SetNotificationEnable.NotificationEnable = SetNotificationEnable;

  return USBPD_OK;
}

/**
  * @brief  Acknowledge GET_CAPABILITY message (Required)
  * @retval None
  */
static USBPD_StatusTypeDef PPM_GetCapabilityStatus(void)
{
  /* Reset Status variable */
  PPM_Handle.AckCCI_Status.AsUInt32 = 0U;

  /* Acknowledge the command */
  PPM_Handle.AckCCI_Status.DataLength                = 0x10U;
  PPM_Handle.AckCCI_Status.CommandCompletedIndicator = 1U;
  PPM_Handle.AckCCI_Status.ConnectorChangeIndicator  = AsynchEventPending;

  /* If the command completed successfully then PPM shall copy the
     capability in the message_in as described in table 4-13 GET_CAPABILITY Data */
  (void)memcpy(&PPM_Handle.MessageIn.Capability, &PPM_Handle.Capability, sizeof(UCSI_GET_CAPABILITY_IN_t));

  return USBPD_OK;
}

/**
  * @brief  Acknowledge GET_CONNECTOR_CAPABILITY message (Required)
  * @param  PortNumber Port Number value
  * @retval None
  */
static USBPD_StatusTypeDef PPM_GetConnectorCapabilityStatus(uint8_t PortNumber)
{
  /* Reset Status variable */
  PPM_Handle.AckCCI_Status.AsUInt32 = 0U;

  /* Acknowledge the command */
  PPM_Handle.AckCCI_Status.DataLength                = 2U;
  PPM_Handle.AckCCI_Status.CommandCompletedIndicator = 1U;
  PPM_Handle.AckCCI_Status.ConnectorChangeIndicator  = AsynchEventPending;

  /* If the command completed successfully then PPM shall copy the
     capability in the message_in as described in table 4-13 GET_CAPABILITY Data */
  (void)memcpy(&PPM_Handle.MessageIn.ConnectorCapability, &PPM_Handle.ConnectorCapability[PortNumber],
               sizeof(UCSI_GET_CONNECTOR_CAPABILITY_IN_t));

  return USBPD_OK;
}


#if FEATURE_SET_UOM_SUPPORTED
/**
  * @brief  Set CC operation mode in USB-PD stack (Optional)
  * @param  PortNumber  Port Number value
  * @param  Mode  USB Mode can take the following value
  *               @arg UcsiCCOperationModeRpOnly
  *               @arg UcsiCCOperationModeRdOnly
  *               @arg UcsiCCOperationModeDrp
  * @retval USBPD status
  */
static USBPD_StatusTypeDef PPM_SetCC_OperationMode(uint8_t PortNumber, UCSI_CC_OPERATION_MODE_t Mode)
{
  /* Reset Status variable */
  PPM_Handle.AckCCI_Status.AsUInt32 = 0U;

  /* Acknowledge the command */
  PPM_Handle.AckCCI_Status.CommandCompletedIndicator = 1U;
  PPM_Handle.AckCCI_Status.ConnectorChangeIndicator  = AsynchEventPending;

  /* Possible only if port is DRP */
  if (1U == DPM_Settings[PortNumber].PE_RoleSwap)
  {
    /* Save the CC operation mode */
    PPM_Handle.CC_Mode[PortNumber] = Mode;
  }
  else
  {
    PPM_Handle.AckCCI_Status.ErrorIndicator = 1U;
  }

  return USBPD_OK;
}
#endif /* FEATURE_SET_UOM_SUPPORTED */

/**
  * @brief  Set new USB operation role in USB-PD stack (Required)
  * @param  PortNumber  Port Number value
  * @param  Role  USB Role can take the following value
  *               @arg UcsiUsbOperationRoleDfp
  *               @arg UcsiUsbOperationRoleUfp
  *               @arg UcsiUsbOperationRoleAcceptSwap
  * @retval USBPD status
  */
static USBPD_StatusTypeDef PPM_SetUSB_OperationRole(uint8_t PortNumber, UCSI_USB_OPERATION_ROLE_t Role)
{
  /* Reset Status variable */
  PPM_Handle.AckCCI_Status.AsUInt32 = 0U;

  /* Acknowledge the command */
  PPM_Handle.AckCCI_Status.CommandCompletedIndicator = 1U;
  PPM_Handle.AckCCI_Status.ConnectorChangeIndicator  = AsynchEventPending;

  /* Save the USB operation mode */
  PPM_Handle.USB_Role[PortNumber] = Role;

  /* No effect and command failed:
     - if connector does not have an active connection */
  /* Check if device is connected or not */
  PPM_Handle.AckCCI_Status.ErrorIndicator  = 1U;

  /* Fields valid only in connect status set to 1 */
  if (DPM_Params[PortNumber].PE_IsConnected != 0U)
  {
    PPM_Handle.AckCCI_Status.ErrorIndicator  = 0U;
    /* No effect and command successfully:
     - if connector is already in the operation role   */
    if (((USBPD_PORTDATAROLE_UFP == DPM_Params[PortNumber].PE_DataRole)
         && ((UcsiUsbOperationRoleDfp | UcsiUsbOperationRoleAcceptSwap) == Role))
        || ((USBPD_PORTDATAROLE_DFP == DPM_Params[PortNumber].PE_DataRole)
            && ((UcsiUsbOperationRoleUfp | UcsiUsbOperationRoleAcceptSwap) == Role)))
    {
      /* Initiate a DR swap*/
      USBPD_DPM_RequestDataRoleSwap(PortNumber);
    }
  }
  /* Command should be compatible with current operation mode */

  return USBPD_OK;
}

#if FEATURE_SET_PDM_SUPPORTED
/**
  * @brief  Set new Power Direction mode in USB-PD stack (Optional)
  * @param  PortNumber  Port Number value
  * @param  PowerMode   Power direction Mode based on UCSI_POWER_DIRECTION_MODE_t
  * @retval USBPD status
  */
static USBPD_StatusTypeDef PPM_SetPowerDirectionMode(uint8_t PortNumber, UCSI_POWER_DIRECTION_MODE_t PowerMode)
{
  /* Reset Status variable */
  PPM_Handle.AckCCI_Status.AsUInt32 = 0U;

  /* Acknowledge the command */
  PPM_Handle.AckCCI_Status.CommandCompletedIndicator = 1U;
  PPM_Handle.AckCCI_Status.ConnectorChangeIndicator  = AsynchEventPending;

  /* Save the Power Direction Mode */
  PPM_Handle.PowerMode[PortNumber] = PowerMode;

  return USBPD_OK;
}
#endif /* FEATURE_SET_PDM_SUPPORTED */

/**
  * @brief  Set new Power Direction role in USB-PD stack (Required)
  * @param  PortNumber  Port Number value
  * @param  PowerRole   Power direction Mode based on UCSI_POWER_DIRECTION_ROLE_t
  * @retval USBPD status
  */
static USBPD_StatusTypeDef PPM_SetPowerDirectionRole(uint8_t PortNumber, UCSI_POWER_DIRECTION_ROLE_t PowerRole)
{
  /* Reset Status variable */
  PPM_Handle.AckCCI_Status.AsUInt32 = 0U;

  /* Acknowledge the command */
  PPM_Handle.AckCCI_Status.CommandCompletedIndicator = 1U;
  PPM_Handle.AckCCI_Status.ConnectorChangeIndicator  = AsynchEventPending;

  /* Save the Power Direction Role*/
  PPM_Handle.PowerRole[PortNumber] = PowerRole;

  /* Initiate a DR swap*/
  if (USBPD_OK != USBPD_DPM_RequestPowerRoleSwap(PortNumber))
  {
    PPM_Handle.AckCCI_Status.ErrorIndicator  = 1U;
  }
  return USBPD_OK;
}

#if FEATURE_ALTERNATEMODEDETAILS_SUPPORTED
/**
  * @brief  Set new Power Direction role in USB-PD stack (Optional)
  * @param  PortNumber  Port Number value
  * @param  PtrGetAlternatesModes  Pointer to a get alternate modes command
  * @retval USBPD status
  */
static USBPD_StatusTypeDef PPM_GetAlternateModes(uint8_t PortNumber,
                                                 PUCSI_GET_ALTERNATE_MODES_COMMAND PtrGetAlternatesModes)
{
  USBPD_StatusTypeDef status = USBPD_OK;

  /* Reset Status variable */
  PPM_Handle.AckCCI_Status.AsUInt32 = 0U;

  /* Acknowledge the command */
  PPM_Handle.AckCCI_Status.CommandCompletedIndicator = 1U;
  PPM_Handle.AckCCI_Status.ConnectorChangeIndicator  = AsynchEventPending;
  
  PPM_DEBUG(PortNumber, "ADVICE: Update PPM_GetAlternateModes");

  return status;
}

/**
  * @brief  Manage reception of GET_CAM_SUPPORTED message (Optional)
  * @param  PortNumber  Port Number value
  * @retval USBPD status
  */
static USBPD_StatusTypeDef PPM_GetCamSupported(uint8_t PortNumber)
{
  /* Reset Status variable */
  PPM_Handle.AckCCI_Status.AsUInt32 = 0U;

  /* Acknowledge the command */
  PPM_Handle.AckCCI_Status.CommandCompletedIndicator = 1U;
  PPM_Handle.AckCCI_Status.ConnectorChangeIndicator  = AsynchEventPending;

  PPM_DEBUG(PortNumber, "ADVICE: Update PPM_GetCamSupported");

  return USBPD_OK;
}

/**
  * @brief  Manage reception of GET_CURRENT_CAM message (Optional)
  * @param  PortNumber  Port Number value
  * @retval USBPD status
  */
static USBPD_StatusTypeDef PPM_GetCurrentCam(uint8_t PortNumber)
{
  /* Reset Status variable */
  PPM_Handle.AckCCI_Status.AsUInt32 = 0U;

  /* Acknowledge the command */
  PPM_Handle.AckCCI_Status.CommandCompletedIndicator = 1U;
  PPM_Handle.AckCCI_Status.ConnectorChangeIndicator  = AsynchEventPending;

  PPM_DEBUG(PortNumber, "ADVICE: Update PPM_GetCurrentCam");

  return USBPD_OK;
}

#if FEATURE_ALTERNATEMODEOVERRIDE_SUPPORTED
/**
  * @brief  Manage reception of SET_NEW_CAM message (Optional)
  * @param  PortNumber  Port Number value
  * @param  EnterOrExit PPM wants to enter (1U) or exit (0U) in this alternate mode
  * @param  NewCAM      Offset into the list of alternates modes that PPM wants to operate in
  * @param  AMSpecific  Content of the specific alternate mode (ex DP status)
  * @retval USBPD status
  */
static USBPD_StatusTypeDef PPM_SetNewCam(uint8_t PortNumber, uint8_t EnterOrExit, uint8_t NewCAM, uint32_t AMSpecific)
{
  /* Reset Status variable */
  PPM_Handle.AckCCI_Status.AsUInt32 = 0U;

  /* Acknowledge the command */
  PPM_Handle.AckCCI_Status.CommandCompletedIndicator = 1U;
  PPM_Handle.AckCCI_Status.ConnectorChangeIndicator  = AsynchEventPending;

  PPM_DEBUG(PortNumber, "ADVICE: Update PPM_SetNewCam");

  return USBPD_OK;
}
#endif /* FEATURE_ALTERNATEMODEOVERRIDE_SUPPORTED */
#endif /* FEATURE_ALTERNATEMODEDETAILS_SUPPORTED */

#if FEATURE_PDODETAILS_SUPPORTED
/**
  * @brief  Manage reception of GET_PDOS message (Optional)
  * @param  PortNumber  Port Number value
  * @param  PtrGetPdos  Pointer to the UCSI_GET_PDOS_COMMAND structure
  * @retval USBPD status
  */
static USBPD_StatusTypeDef PPM_GetPDOs(uint8_t PortNumber, PUCSI_GET_PDOS_COMMAND_t PtrGetPdos)
{
  /* Reset Status variable */
  PPM_Handle.AckCCI_Status.AsUInt32 = 0U;

  /* Acknowledge the command */
  PPM_Handle.AckCCI_Status.CommandCompletedIndicator = 1U;
  PPM_Handle.AckCCI_Status.ConnectorChangeIndicator  = AsynchEventPending;

  uint8_t nbPDO;
  uint8_t maxnbPDOSNK;
  uint8_t maxnbPDOSRC;
  uint32_t *ptrListPDOSRC;
  uint32_t *ptrListPDOSNK;

  /* Retrieve the PDOs associated with the connector */
  if (PtrGetPdos->PartnerPdo == 0U)
  {
#if defined (_GUI_INTERFACE)
    maxnbPDOSNK = USBPD_NbPDO[0];
    maxnbPDOSRC = USBPD_NbPDO[1];
#else
    maxnbPDOSNK = (uint8_t)PORT0_NB_SINKPDO;
    maxnbPDOSRC = (uint8_t)PORT0_NB_SINKPDO;
#endif /* _GUI_INTERFACE */
    ptrListPDOSRC = PORT0_PDO_ListSRC;
    ptrListPDOSNK = PORT0_PDO_ListSNK;
  }
  /* Retrieve the PDOs of the device attached to the connector */
  else
  {
    maxnbPDOSNK = (uint8_t)DPM_Ports[PortNumber].DPM_NumberOfRcvSNKPDO;
    maxnbPDOSRC = (uint8_t)DPM_Ports[PortNumber].DPM_NumberOfRcvSRCPDO;
    ptrListPDOSRC = DPM_Ports[PortNumber].DPM_ListOfRcvSRCPDO;
    ptrListPDOSNK = DPM_Ports[PortNumber].DPM_ListOfRcvSNKPDO;
  }

  if (PtrGetPdos->SourceOrSinkPdos == UcsiGetPdosTypeSource)
  {
    /* Get the min between the asked number of PDOs and the available source PDOs after offset to avoid overflow */
    nbPDO = MIN((PtrGetPdos->NumberOfPdos + 1U), maxnbPDOSRC - PtrGetPdos->PdoOffset);
  }
  else
  {
    /* Get the min between the asked number of PDOs and the available sink PDOs after offset to avoid overflow  */
    nbPDO = MIN((PtrGetPdos->NumberOfPdos + 1U), maxnbPDOSNK - PtrGetPdos->PdoOffset);
  }

  /* DataLength field is 4 x Number of PDOs up to UCSI_MAX_DATA_LENGTH */
  if ((4U * nbPDO) > UCSI_MAX_DATA_LENGTH)
  {
    nbPDO = UCSI_MAX_DATA_LENGTH / 4U;
  }
  PPM_Handle.AckCCI_Status.DataLength = 4U * nbPDO;

  /* Reset the returned PDOs */
  memset(PPM_Handle.MessageIn.Pdos.Pdos, 0x00U, sizeof(PPM_Handle.MessageIn.Pdos.Pdos));

  /* Fill the structure UCSI_GET_PDOS_IN_t */
  if (PtrGetPdos->SourceOrSinkPdos == UcsiGetPdosTypeSource)
  {
    memcpy(PPM_Handle.MessageIn.Pdos.Pdos, ptrListPDOSRC + PtrGetPdos->PdoOffset, sizeof(uint32_t) * nbPDO);
  }
  else
  {
    memcpy(PPM_Handle.MessageIn.Pdos.Pdos, ptrListPDOSNK + PtrGetPdos->PdoOffset, sizeof(uint32_t) * nbPDO);
  }

  return USBPD_OK;
}
#endif /* FEATURE_PDODETAILS_SUPPORTED */

#if FEATURE_CABLEDETAILS_SUPPORTED
/**
  * @brief  Manage reception of GET_CABLE_PROPERTY message (Optional)
  * @param  PortNumber  Port Number value
  * @retval USBPD status
  */
static USBPD_StatusTypeDef PPM_GetCableProperty(uint8_t PortNumber)
{
  /* Reset Status variable */
  PPM_Handle.AckCCI_Status.AsUInt32 = 0U;

  /* Acknowledge the command */
  PPM_Handle.AckCCI_Status.CommandCompletedIndicator = 1U;
  PPM_Handle.AckCCI_Status.ConnectorChangeIndicator  = AsynchEventPending;
  
  PPM_DEBUG(PortNumber, "ADVICE: Update PPM_GetCableProperty");

  return USBPD_OK;
}
#endif /* FEATURE_CABLEDETAILS_SUPPORTED */

/**
  * @brief  Get the connector status (Required)
  * @param  PortNumber  Port Number value
  * @retval USBPD status
  */
static USBPD_StatusTypeDef PPM_GetConnectorStatus(uint8_t PortNumber)
{
  PUCSI_GET_CONNECTOR_STATUS_IN_t p_connector_status = &PPM_Handle.MessageIn.ConnectorStatus;

  /* Reset Status variable */
  PPM_Handle.AckCCI_Status.AsUInt32 = 0U;

  /* Acknowledge the command */
  PPM_Handle.AckCCI_Status.CommandCompletedIndicator = 1U;
  PPM_Handle.AckCCI_Status.DataLength = 0x09U;

  /* Fill ConnectorStatusChange if a new connector change has been notified */
  if (ConnectorChange[PortNumber] == UCSI_CONNECTOR_CHANGE)
  {
    /* Fill the ConnectorStatusChange */
    p_connector_status->ConnectorStatusChange.AsUInt16                            = 0x0000U;
#if FEATURE_EXTERNALSUPPLYNOTIFICATION_SUPPORTED
    p_connector_status->ConnectorStatusChange.ExternalSupplyChange                = 0U;
#endif /* FEATURE_EXTERNALSUPPLYNOTIFICATION_SUPPORTED */
    p_connector_status->ConnectorStatusChange.PowerOperationModeChange            = 1U;
    p_connector_status->ConnectorStatusChange.SupportedProviderCapabilitiesChange = 0U;
    p_connector_status->ConnectorStatusChange.NegotiatedPowerLevelChange          = 0U;
#if FEATURE_PDRESETNOTIFICATION_SUPPORTED
    p_connector_status->ConnectorStatusChange.PdResetComplete                     = 0U;
#endif /* FEATURE_PDRESETNOTIFICATION_SUPPORTED */
#if FEATURE_ALTERNATEMODEDETAILS_SUPPORTED
    p_connector_status->ConnectorStatusChange.SupportedCamChange                  = 0U;
#endif /* FEATURE_ALTERNATEMODEDETAILS_SUPPORTED */
    p_connector_status->ConnectorStatusChange.BatteryChargingStatusChange         = 0U;
    p_connector_status->ConnectorStatusChange.ConnectorPartnerChange              = 0U;
    p_connector_status->ConnectorStatusChange.PowerDirectionChange                = 1U;
    p_connector_status->ConnectorStatusChange.ConnectChange                       = 1U;
    p_connector_status->ConnectorStatusChange.Error                               = 0U;
  }

  /* Reset current notification */
  ConnectorChange[PortNumber] = UCSI_CONNECTOR_IDLE;

  /* Check if device is connected or not */
  p_connector_status->ConnectStatus = (uint16_t)DPM_Params[PortNumber].PE_IsConnected;

  /* Fields valid only in connect status set to 1 */
  if (p_connector_status->ConnectStatus != 0U)
  {
    if (USBPD_POWER_EXPLICITCONTRACT == DPM_Params[PortNumber].PE_Power)
    {
      p_connector_status->PowerOperationMode = UcsiPowerOperationModePd;
    }
    else
    {
      p_connector_status->PowerOperationMode = UcsiPowerOperationModeDefaultUsb;
      if (USBPD_PORTPOWERROLE_SNK == DPM_Params[PortNumber].PE_PowerRole)
      {
        p_connector_status->PowerOperationMode = UcsiPowerOperationModeDefaultUsb;
        if (vRd_3_0A == DPM_Params[PortNumber].SNKExposedRP_AtAttach)
        {
          p_connector_status->PowerOperationMode = UcsiPowerOperationModeTypeC3000;
        }
        else
        {
          if (vRd_1_5A == DPM_Params[PortNumber].SNKExposedRP_AtAttach)
          {
            p_connector_status->PowerOperationMode = UcsiPowerOperationModeTypeC1500;
          }
        }
      }
    }
    p_connector_status->PowerDirection = (UCSI_POWER_DIRECTION_t)(USBPD_PORTPOWERROLE_SRC ==
                                                                  DPM_Params[PortNumber].PE_PowerRole);

    p_connector_status->ConnectorPartnerFlags = (uint16_t)UcsiConnectorPartnerFlagUsb;

#if defined(_VDM)
    if (DPM_Settings[PortNumber].PE_VDMSupport = USBPD_TRUE)
    {
      p_connector_status->ConnectorPartnerFlags |= UcsiConnectorPartnerFlagAlternateMode;
    }
#endif /* _VDM */

    /* Need to get information if cable is connected or not (information from CAD) */
    if (USBPD_PORTDATAROLE_UFP == DPM_Params[PortNumber].PE_DataRole)
    {
      p_connector_status->ConnectorPartnerType  = UcsiConnectorPartnerTypeDfp;
    }
    else
    {
      p_connector_status->ConnectorPartnerType  = UcsiConnectorPartnerTypeUfp;
    }

    if (UcsiPowerDirectionConsumer == p_connector_status->PowerDirection)
    {
      p_connector_status->BatteryChargingStatus     = UcsiBatteryChargingNotCharging;
    }
    else
    {
      p_connector_status->bmPowerBudgetLimitedReason.PowerBudgetLowered       = 0U;
      p_connector_status->bmPowerBudgetLimitedReason.ReachingPowerBudgetLimit = 0U;
    }
  }
  else
  {
    p_connector_status->PowerOperationMode        = UcsiPowerOperationModeNoConsumer;
    p_connector_status->PowerDirection            = UcsiPowerDirectionConsumer;
    p_connector_status->ConnectorPartnerFlags     = 0U;
    p_connector_status->ConnectorPartnerType      = UcsiConnectorPartnerUnknown;
    p_connector_status->BatteryChargingStatus     = UcsiBatteryChargingNotCharging;
    p_connector_status->PowerBudgetLimitedReason  = 0U;

  }

#if FEATURE_PDODETAILS_SUPPORTED
  /* Field valid only in connect status set to 1 and power operation mode is set to PD */
  if ((p_connector_status->ConnectStatus != 0U) && (UcsiPowerOperationModePd == p_connector_status->PowerOperationMode))
  {
    /* Return the negotiated power level */
    USBPD_PWR_IF_SearchRequestedPDO(PortNumber, DPM_Ports[PortNumber].DPM_RDOPosition,
                                    &p_connector_status->RequestDataObject);
  }
  else
#endif /* FEATURE_PDODETAILS_SUPPORTED */
  {
    p_connector_status->RequestDataObject     = 0U;
  }
  return USBPD_OK;
}

/**
  * @brief  Manage reception of GET_ERROR_STATUS message (Required)
  * @retval USBPD status
  */
static USBPD_StatusTypeDef PPM_GetErrorStatus(void)
{
  /* Reset Status variable */
  PPM_Handle.AckCCI_Status.AsUInt32 = 0U;

  /* Acknowledge the command */
  PPM_Handle.AckCCI_Status.CommandCompletedIndicator = 1U;

  PPM_Handle.AckCCI_Status.DataLength = UCSI_GET_ERROR_STATUS_DATA_LENGTH;

  return USBPD_OK;
}
#if FEATURE_SET_POWER_LEVEL_SUPPORTED
/**
  * @brief  Manage reception of SET POWER LEVEL message (Optional)
  * @note Command used by OPM to set the max negiable power level of Sink
  *           or Source
  * @param  PtrSetPowerLevel  Pointer on Set Power Level Command
  * @retval USBPD status
  */
static USBPD_StatusTypeDef PPM_SetPowerLevel(PUCSI_SET_POWER_LEVEL_COMMAND PtrSetPowerLevel)
{
  /* Reset Status variable */
  PPM_Handle.AckCCI_Status.AsUInt32 = 0U;

  /* Acknowledge the command */
  PPM_Handle.AckCCI_Status.CommandCompletedIndicator = 1U;

  /* Command has no effect and set Error information to Invalid Command (InvalidCommandParametersError):
      - no active connection
      - Power direction does not match */

  /* If command is valid, complete the command and renegotiate a power contract is necessary */
  return USBPD_OK;
}
#endif /* FEATURE_SET_POWER_LEVEL_SUPPORTED */

#if FEATURE_GET_PD_MESSAGE_SUPPORTED
/**
  * @brief  Manage reception of GET PD MESSAGE message (Optional)
  * @param  PtrGetPDMessage  Pointer on Get PD Message Command
  * @retval USBPD status
  */
static USBPD_StatusTypeDef PPM_GetPDMessage(PUCSI_GET_PD_MESSAGE_COMMAND PtrGetPDMessage)
{
  /* Reset Status variable */
  PPM_Handle.AckCCI_Status.AsUInt32 = 0U;

  /* Acknowledge the command */
  PPM_Handle.AckCCI_Status.CommandCompletedIndicator = 1U;

  return USBPD_OK;
}
#endif /* FEATURE_GET_PD_MESSAGE_SUPPORTED */

/**
  * @brief  Manage reception of Unknown command message (Required)
  * @retval USBPD status
  */
static USBPD_StatusTypeDef PPM_UnknownCommand(uint8_t PortNumber)
{
  PUCSI_GET_ERROR_STATUS_IN_t p_error_status;

  if (PortNumber >= USBPD_PORT_COUNT)
  {
    PPM_DEBUG(PPM_DEBUG_LEVEL_0, 0U, "UnknownCommand WRONG PORT");
  }

  p_error_status = &PPM_Handle.MessageIn.ErrorStatus;
  p_error_status->ErrorInformation.AsUInt16                 = 0U;
  p_error_status->ErrorInformation.UnrecognizedCommandError = 1U;

  /* Reset Status variable */
  PPM_Handle.AckCCI_Status.AsUInt32 = 0U;

  /* Acknowledge the command */
  PPM_Handle.AckCCI_Status.CommandCompletedIndicator  = 1U;
  PPM_Handle.AckCCI_Status.NotSupportedIndicator      = 1U;

  return USBPD_OK;
}

/**
  * @brief  Fill the default configuration used by the USB-PD stack
  * @retval USBPD status
  */
static USBPD_StatusTypeDef PPM_FillDefaultConfiguration(void)
{
  USBPD_StatusTypeDef status = USBPD_OK;

  PUCSI_VERSION_t _version = &PPM_Handle.UcsiVersion;

  /* UCSI revision 1.2 (January 2020) */
  _version->MajorVersion    = 0x1U;
  _version->MinorVersion    = 0x2U;
  _version->SubMinorVersion = 0x0U;

  PUCSI_GET_CAPABILITY_IN_t _pcabability = &PPM_Handle.Capability;

  /* Fill the capability bmAttributes */
  _pcabability->bmAttributes.DisabledStateSupport  = 1U;
  _pcabability->bmAttributes.BatteryCharging       = 0U;
  _pcabability->bmAttributes.UsbPowerDelivery      = 1U;
  _pcabability->bmAttributes.UsbTypeCCurrent       = 1U;
  _pcabability->bmAttributes.bmPowerSource         = (1U << 6); /* Uses VBUS */

  /* Fill the Number of connectors */
  _pcabability->bNumConnectors  = USBPD_PORT_COUNT;

  /* Fill the capability bmOptionalFeatures */
  bmOptionalFeatures_t feature;
  feature.AsUInt32 = 0U;
  feature.SetCComSupported                     = FEATURE_SET_UOM_SUPPORTED;
  feature.SetPowerLevelSupported               = FEATURE_SET_POWER_LEVEL_SUPPORTED;
#if FEATURE_ALTERNATEMODEDETAILS_SUPPORTED
  feature.AlternateModeDetailsSupported        = FEATURE_ALTERNATEMODEDETAILS_SUPPORTED;
#endif /* FEATURE_ALTERNATEMODEDETAILS_SUPPORTED */
#if FEATURE_ALTERNATEMODEOVERRIDE_SUPPORTED
  feature.AlternateModeOverrideSupported       = FEATURE_ALTERNATEMODEOVERRIDE_SUPPORTED;
#endif /* FEATURE_ALTERNATEMODEOVERRIDE_SUPPORTED */
  feature.PdoDetailsSupported                  = FEATURE_PDODETAILS_SUPPORTED;
  feature.CableDetailsSupported                = FEATURE_CABLEDETAILS_SUPPORTED;
  feature.ExternalSupplyNotificationSupported  = FEATURE_EXTERNALSUPPLYNOTIFICATION_SUPPORTED;
  feature.PdResetNotificationSupported         = FEATURE_PDRESETNOTIFICATION_SUPPORTED;
  feature.GetPDMessageSupported                = FEATURE_GET_PD_MESSAGE_SUPPORTED;
  _pcabability->OptionalFeatures = feature.AsUInt32;

  /* Fill the Number of supported alternate modes */
#if defined(_VDM)
  _pcabability->bNumAltModes  = 1U;
#else
  _pcabability->bNumAltModes  = 0U;
#endif /* _VDM */

  /* Fill Battery Charging Specification Release number */
  _pcabability->bcdBcVersion  = 0x0000U;

  /* Fill USB-PD Specification Release number */
  if (USBPD_SPECIFICATION_REV2 == DPM_Settings[USBPD_PORT_0].PE_SpecRevision)
  {
    _pcabability->bcdPdVersion  = 0x0200U;
  }
  else
  {
    _pcabability->bcdPdVersion  = 0x0300U;
  }

  /* Fill USB-Type C Specification Release number */
  _pcabability->bcdUsbTypeCVersion  = 0x0200U;

  return status;
}

/**
  * @brief  Fill the connector capability configuration used by the USB-PD stack
  * @param  PortNumber    PortNumber number value
  * @param  _pcabability  Pointer on UCSI_GET_CONNECTOR_CAPABILITY_IN structure
  * @retval USBPD status
  */
static USBPD_StatusTypeDef PPM_FillConnectorCapability(uint8_t PortNumber,
                                                       PUCSI_GET_CONNECTOR_CAPABILITY_IN_t PtrConnectorCapability)
{
  USBPD_StatusTypeDef status = USBPD_OK;

  /* Fill the connector capability OperationMode */
  if (1U == DPM_Settings[PortNumber].PE_RoleSwap)
  {
    /* Allow OPM to request a DRS as defined port is defined as DRD
          Port defined as TYPEC_PORT_DRD on OPM side */
    PtrConnectorCapability->OperationMode.Drp                = 1U;
    /* Does not care but set to 1 anyway */
    PtrConnectorCapability->OperationMode.RpOnly             = 1U;
    PtrConnectorCapability->OperationMode.RdOnly             = 1U;
  }
  else
  {
    /* DRS not supported */
    PtrConnectorCapability->OperationMode.Drp                 = 0U;
    /* Check default Data role of port */
    if (USBPD_PORTPOWERROLE_SRC == DPM_Settings[PortNumber].PE_DefaultRole)
    {
      /* Port defined as TYPEC_PORT_DFP on OPM side */
      PtrConnectorCapability->OperationMode.RpOnly             = 1U;
      PtrConnectorCapability->OperationMode.RdOnly             = 0U;
    }
    else
    {
      /* Port defined as TYPEC_PORT_UFP on OPM side */
      PtrConnectorCapability->OperationMode.RpOnly             = 0U;
      PtrConnectorCapability->OperationMode.RdOnly             = 1U;
    }
  }
  PtrConnectorCapability->OperationMode.AudioAccessoryMode  = 0U;
  PtrConnectorCapability->OperationMode.DebugAccessoryMode  = 0U;

  /* Retrieve if USB is supported from first PDO */
  if ((PORT0_PDO_ListSRC[0U] & USBPD_PDO_SRC_FIXED_USBCOMM_SUPPORTED) != 0U)
  {
    PtrConnectorCapability->OperationMode.Usb2              = 0U;
  }
  PtrConnectorCapability->OperationMode.Usb3                = 0U;
  PtrConnectorCapability->OperationMode.AlternateMode       = (uint8_t)DPM_Settings[PortNumber].PE_VDMSupport;

  /* Fill the connector capability Provider/Consumer */
  if (1U == DPM_Settings[PortNumber].PE_RoleSwap)
  {
    /* Port defined as TYPEC_PORT_DRP on OPM side */
    PtrConnectorCapability->Provider       = 1U;
    PtrConnectorCapability->Consumer       = 1U;

    /* Power Role Swap supported */
    PtrConnectorCapability->SwapToSRC      = 1U;
    PtrConnectorCapability->SwapToSNK      = 1U;
  }
  else
  {
    if (USBPD_PORTPOWERROLE_SRC == DPM_Settings[PortNumber].PE_DefaultRole)
    {
      /* Port defined as TYPEC_PORT_SRC on OPM side */
      PtrConnectorCapability->Provider       = 1U;
      PtrConnectorCapability->Consumer       = 0U;
    }
    else
    {
      /* Port defined as TYPEC_PORT_SNK on OPM side */
      PtrConnectorCapability->Provider       = 0U;
      PtrConnectorCapability->Consumer       = 1U;
    }

    /* Power Role Swap not supported */
    PtrConnectorCapability->SwapToSRC      = 1U;
    PtrConnectorCapability->SwapToSNK      = 1U;
  }

  /* Fill the connector capability Data Role Support */
  PtrConnectorCapability->SwapToDFP = (uint8_t)DPM_USER_Settings[PortNumber].PE_DR_Swap_To_DFP;
  PtrConnectorCapability->SwapToUFP = (uint8_t)DPM_USER_Settings[PortNumber].PE_DR_Swap_To_UFP;

  return status;
}

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
