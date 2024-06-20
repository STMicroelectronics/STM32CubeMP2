/**
  ******************************************************************************
  * @file  : usbpd_ucsi_if.c
  * @brief : Source file for interfacing the usbpd_ucsi to hardware.
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
#include "usbpd_ucsi.h"
#include "ppm.h"
#include "usbpd_ucsi_if.h"

#include "main.h"
#include "usbpd_os_port.h"
#if defined(_TRACE)
#include "usbpd_trace.h"
#include "string.h"
#include "stdio.h"
#endif /* _TRACE */

/* Private typedef -----------------------------------------------------------*/
/**
  * @brief UCSI states
  */
typedef enum
{
  UCSI_STATE_INIT,

  /* Errors */
  UCSI_ERROR_SEQ_RECEIVE,
  UCSI_ERROR_SEQ_TRANSMIT,
  UCSI_ERROR_SEQ_UNKOWN,
  UCSI_ERROR_SEQ_TXCOUNT,
} UCSI_enum;

/**
  * @brief UCSI interface handle
  */
typedef struct
{
  void (*ReceiveCommandCb)(uint8_t, uint8_t *);      /*!< function used to return the UCSI command received by OPM */
} UCSI_IF_HandleTypeDef;

/**
  * @brief UCSI user handle
  */
typedef struct
{
  UCSI_enum       Error;
  uint8_t         Direction;
  UCSI_REG_t      Register;
  UCSI_enum       State;
  uint8_t        *PtrBuffer;
  uint8_t         DataSize;
} UCSI_USER_HandleTypeDef;

/* Private define ------------------------------------------------------------*/
/******************************************************************************
   Alert IRQ
  *****************************************************************************/
#define ALERT_IRQ_NBR          (0U)         /* Virtual INTC IRQ number        */

/******************************************************************************
   I2C configuration
  *****************************************************************************/
#define I2C_ADDRESS_UCSI       (0x35U)      /* Used for UCSI commands         */
/******************************************************************************/

#if defined(_TRACE)
/* Definition of debug levels */
#define UCSI_DEBUG_NONE        (0U)
#define UCSI_DEBUG_LEVEL_0     (1U << 0)
#define UCSI_DEBUG_LEVEL_1     (1U << 1)
#define UCSI_DEBUG_LEVEL_2     (1U << 2)
#endif /* _TRACE */

#if defined(USBPD_THREADX)
#define OS_I2CTASK_PRIORITY    (2U)
#elif defined(_RTOS)
#define OS_I2CTASK_PRIORITY    (osPriorityNormal)
#endif /* USBPD_THREADX */

#define OS_I2CTASK_STACK_SIZE  (1024U)

/* Private macro -------------------------------------------------------------*/
/* Free the mutex to indicate the operation complete */

/* Private variables ---------------------------------------------------------*/
#if defined(_TRACE)
/* UCSI debug verbose level */
uint8_t UCSI_DebugLevel =  /* UCSI_DEBUG_LEVEL_2 |  UCSI_DEBUG_LEVEL_1 | */ UCSI_DEBUG_LEVEL_0 | UCSI_DEBUG_NONE;
#endif /* _TRACE */

/* UCSI handle used for UCSI communication*/
static UCSI_USER_HandleTypeDef UCSI_Handles;

/* Mutex definition */
#if defined(USBPD_THREADX)
TX_SEMAPHORE sem_i2c_cmd_req;
TX_SEMAPHORE sem_ucsi_cmd_cplt;
#elif defined(_RTOS)
osSemaphoreDef(sem_i2c_cmd_req);
static osSemaphoreId sem_i2c_cmd_req;
static const osSemaphoreAttr_t sem_i2c_cmd_req_Attr = {
    .name = "sem_i2c_cmd_req",
};
#endif /* USBPD_THREADX */

#if defined(USBPD_THREADX)
/* ThreadX memory pool and event flags */
static TX_BYTE_POOL         *USBPD_memory_pool;
extern TX_EVENT_FLAGS_GROUP        EventSemaphore;
#endif /* USBPD_THREADX */

#if defined(_RTOS) || defined(USBPD_THREADX)
OS_TASK_ID I2CTaskThread;
#endif /* _RTOS || USBPD_THREADX */

/* Virt_I2C & Virt_INTC handle */
static VIRT_I2C_HandleTypeDef  hi2c;
static VIRT_INTC_HandleTypeDef hintc;

/* UCSI IF handle */
UCSI_IF_HandleTypeDef UCSI_IF_Handle;

/* Local variable to store port status */
uint8_t Register;
uint8_t I2C_Direction;
uint8_t *PtrUCSI_Data;

#if defined(_TRACE)
/* Debug trace */
#define UCSI_IF_PPM_DEBUG_TRACE(_LEVEL_, ...)                                                             \
  do                                                                                                      \
  {                                                                                                       \
    if ((UCSI_DebugLevel & (_LEVEL_)) != 0U)                                                              \
    {                                                                                                     \
      char _str[70U];                                                                                     \
      (void)snprintf(_str, 70U, __VA_ARGS__);                                                             \
        USBPD_TRACE_Add(USBPD_TRACE_DEBUG, USBPD_PORT_0, 0U, (uint8_t*)_str, strlen(_str));               \
      }                                                                                                   \
  } while(0);

/* Simple debug trace */
#define UCSI_IF_PPM_DEBUG_TRACE_NO_ARG(_LEVEL_, __MESSAGE__)                                              \
  do                                                                                                      \
  {                                                                                                       \
    if ((UCSI_DebugLevel & (_LEVEL_)) != 0U)                                                              \
    {                                                                                                     \
      USBPD_TRACE_Add(USBPD_TRACE_DEBUG, USBPD_PORT_0, 0U, (uint8_t*)(__MESSAGE__), strlen(__MESSAGE__)); \
    }                                                                                                     \
  } while(0);

/* UCSI trace */
#define UCSI_IF_PPM_TRACE(_LEVEL_, _ADDR_, _ID_, _PTR_DATA_, _SIZE_)                                      \
  do                                                                                                      \
  {                                                                                                       \
    if ((UCSI_DebugLevel & (_LEVEL_)) != 0U)                                                              \
    {                                                                                                     \
      USBPD_TRACE_Add(USBPD_TRACE_UCSI, (uint8_t)_ADDR_, (uint8_t)_ID_, _PTR_DATA_, _SIZE_);              \
    }                                                                                                     \
  } while(0);
#else
#define UCSI_IF_PPM_DEBUG_TRACE(_LEVEL_, ...)
#define UCSI_IF_PPM_DEBUG_TRACE_NO_ARG(_LEVEL_, __MESSAGE__)
#define UCSI_IF_PPM_TRACE(_LEVEL_, _ADDR_, _ID_, _PTR_DATA_, _SIZE_)
#endif /* _TRACE */

/* Private function prototypes -----------------------------------------------*/
#if defined(USBPD_THREADX)
static USBPD_UCSI_Status_t  UCSI_I2C_Init(void (*ReceiveCommandCb)(uint8_t, uint8_t *), VOID *MemoryPtr);
#else
static USBPD_UCSI_Status_t  UCSI_I2C_Init(void (*ReceiveCommandCb)(uint8_t, uint8_t *));
#endif /* USBPD_THREADX */
static void                 UCSI_AlertInit(void);
static void                 UCSI_AlertON(void);
static void                 UCSI_AlertOFF(void);
DEF_TASK_FUNCTION(          USBPD_UCSI_PPM_TaskListenI2C);
static uint8_t              *UCSI_GetDataSizeRegisterPtr(UCSI_REG_t reg, uint8_t *PtrDataSize);
static void                 VIRT_I2C_RxCpltCallback(VIRT_I2C_HandleTypeDef *hi2c, size_t len);

/**
  * @brief  UCSI Driver definition.
  *
  * list all the driver interfaces used by the UCSI application.
  */
const USBPD_UCSI_Driver_t USBPD_UcsiDriver =
{
  UCSI_I2C_Init,
  UCSI_AlertON,
  UCSI_AlertOFF,
};

/**
  * @brief  Disable I2C used for UCSI
  * @param  None
  * @retval None
  */
void UCSI_I2C_DeInit(void)
{
  OS_TASK_KILL(I2CTaskThread);
  UCSI_IF_PPM_DEBUG_TRACE(UCSI_DEBUG_LEVEL_2, "Virtual I2C DeInit");
  if (VIRT_I2C_OK != VIRT_I2C_DeInit(&hi2c))
  {
    UCSI_IF_PPM_DEBUG_TRACE(UCSI_DEBUG_LEVEL_2, "VIRT_I2C_DeInit failed");
    /* Error */
    Error_Handler();
  }
}

/**
  * @brief  Initialize UCSI
  * @param  *ReceiveCommandCb Pointer of reception function callback
  * @retval UCSI status
  */
#if defined(USBPD_THREADX)
static USBPD_UCSI_Status_t UCSI_I2C_Init(void (*ReceiveCommandCb)(uint8_t, uint8_t *), VOID *MemoryPtr)
#else
static USBPD_UCSI_Status_t UCSI_I2C_Init(void (*ReceiveCommandCb)(uint8_t, uint8_t *))
#endif /* USBPD_THREADX */
{

  /* Start the listen I2C task for UCSI*/
#if defined(USBPD_THREADX)
  if (tx_semaphore_create(&sem_i2c_cmd_req, "sem_i2c_cmd_req" , 0U) != TX_SUCCESS)
  {
    /* Error occurred creating a Semaphore */
    return USBPD_UCSI_ERROR;
  }
#elif defined(_RTOS)

  sem_i2c_cmd_req = osSemaphoreNew(1U, 0U, &sem_i2c_cmd_req_Attr);
  if (sem_i2c_cmd_req == NULL)
  {
    UCSI_IF_PPM_DEBUG_TRACE(UCSI_DEBUG_LEVEL_2, "Semaphore I2C REQ init failed.");
    /* Error occurred creating a Semaphore */
    Error_Handler();
  }

#else
#warning "NRTOS Version not implemented"
#endif /* USBPD_THREADX */

  UCSI_IF_Handle.ReceiveCommandCb = ReceiveCommandCb;

  /* Init Alert pin */
  UCSI_AlertInit();

  /*
   * Add a delay to ensure that the interrupt controller is initialized on
   * remote side before the rpmsg i2c bus
   */
  OS_DELAY(10);


#if 0
  /* Take the mutex_I2C_CPLT semaphores */
#if defined(USBPD_THREADX)
  tx_semaphore_get(&sem_ucsi_i2c_cplt, TX_WAIT_FOREVER);
#elif defined(_RTOS)
  (void)osSemaphoreAcquire(sem_i2c_cplt_id, osWaitForever);
#else
#warning "NRTOS Version not implemented"
#endif /* USBPD_THREADX */
#endif

/* Start the listen I2C task for UCSI*/
#if defined(_RTOS) || defined(USBPD_THREADX)
  /* PPM listen I2C task creation */
  OS_INIT();
  OS_DEFINE_TASK(I2CTask, USBPD_UCSI_PPM_TaskListenI2C, OS_I2CTASK_PRIORITY, OS_I2CTASK_STACK_SIZE, NULL);
  OS_CREATE_TASK(I2CTaskThread, I2CTask, USBPD_UCSI_PPM_TaskListenI2C,
                 OS_I2CTASK_PRIORITY, OS_I2CTASK_STACK_SIZE, NULL);
#else
#warning "NRTOS Version not implemented"
#endif /* _RTOS || USBPD_THREADX */

  (void)_retr; /* To avoid gcc/g++ warnings */
  return USBPD_UCSI_OK;

#if defined(_RTOS) || defined(USBPD_THREADX)
error:
  Error_Handler();
  return USBPD_UCSI_ERROR;
#endif /* _RTOS || USBPD_THREADX */
}

/**
  * @brief  ALERT ON
  * @param  None
  * @retval None
  */
static void UCSI_AlertON(void)
{
  /* Set alert signal */
  UCSI_IF_PPM_TRACE(UCSI_DEBUG_LEVEL_1, I2C_ADDRESS_UCSI, UCSI_TRACE_ALERT_ON, NULL, 0U);
  if (VIRT_INTC_OK != VIRT_INTC_sendIRQ(&hintc, ALERT_IRQ_NBR))
  {
    UCSI_IF_PPM_DEBUG_TRACE(UCSI_DEBUG_LEVEL_1, "VIRT_INTC_sendIRQ failed");
  }
}

/**
  * @brief  ALERT OFF
  * @param  None
  * @retval None
  */
static void UCSI_AlertOFF(void)
{
  /* Reset alert signal */
  UCSI_IF_PPM_TRACE(UCSI_DEBUG_LEVEL_1, I2C_ADDRESS_UCSI, UCSI_TRACE_ALERT_OFF, NULL, 0U);
}

static void clean_port(UCSI_USER_HandleTypeDef *ptr_port)
{
    UCSI_IF_PPM_DEBUG_TRACE_NO_ARG(UCSI_DEBUG_LEVEL_2, "I2C receive: Reset UCSI handles");
    ptr_port->Direction     = 0U;
    ptr_port->Register      = UcsiRegDataStructureUnkown;
    ptr_port->State         = UCSI_STATE_INIT;
}

DEF_TASK_FUNCTION(USBPD_UCSI_PPM_TaskListenI2C)
{
  UCSI_USER_HandleTypeDef *ptr_port = &UCSI_Handles;

  /* Register callback for message reception by channels */
  UCSI_IF_PPM_DEBUG_TRACE(UCSI_DEBUG_LEVEL_2, "Virtual I2C OpenAMP-rpmsg callback registering.");
  if(VIRT_I2C_RegisterCallback(&hi2c, VIRT_I2C_RXCPLT_CB_ID, VIRT_I2C_RxCpltCallback) != VIRT_I2C_OK)
  {
    UCSI_IF_PPM_DEBUG_TRACE(UCSI_DEBUG_LEVEL_2, "VIRT_I2C_RegisterCallback failed.");
    Error_Handler();
  }

  /* Initialize VIRT I2C */
  if (VIRT_I2C_Init(&hi2c) != VIRT_I2C_OK)
  {
     UCSI_IF_PPM_DEBUG_TRACE(UCSI_DEBUG_LEVEL_2, "VIRT_I2C_Init failed.");
     Error_Handler();
  }

  while (1)
  {
    /* Wait and check the I2C operation complete */
    #if defined(USBPD_THREADX)
      UCSI_IF_PPM_DEBUG_TRACE(UCSI_DEBUG_LEVEL_2, "Wait for semaphore sem_i2c_cplt_id");
      if (tx_semaphore_get(&sem_i2c_cmd_req, TX_WAIT_FOREVER) != TX_SUCCESS)
    #elif defined(_RTOS)
      if (osSemaphoreAcquire(sem_i2c_cmd_req, osWaitForever) != 0)
    #else
      #warning "NRTOS Version not implemented"
    #endif /* USBPD_THREADX */
      {
        UCSI_IF_PPM_DEBUG_TRACE_NO_ARG(UCSI_DEBUG_LEVEL_2, "i2c_req semaphore_get failed");
        return;
      }

    UCSI_IF_PPM_DEBUG_TRACE(UCSI_DEBUG_LEVEL_2, "Virtual I2C request");

    /* Save the transfer direction */
    ptr_port->Direction = (hi2c.msg->addr & I2C_MASTER_READ);

    /* MASTER TO SLAVE --> Slave is reading */
    if (I2C_DIRECTION_TRANSMIT ==  ptr_port->Direction)
    {
      UCSI_IF_PPM_DEBUG_TRACE(UCSI_DEBUG_LEVEL_2, "Direction MASTER TO SLAVE");

      if (hi2c.msg->len == 0U)
      {
        UCSI_IF_PPM_DEBUG_TRACE(UCSI_DEBUG_LEVEL_2, "WARNING %s: len is null", __func__);
        if (VIRT_I2C_OK != VIRT_I2C_sendACK(&hi2c, VIRT_I2C_ACK))
        {
          UCSI_IF_PPM_DEBUG_TRACE(UCSI_DEBUG_LEVEL_1, "VIRT I2C ACK failed");
        }
        continue;
      };

      /* Read/copy first received byte */
      UCSI_IF_PPM_DEBUG_TRACE(UCSI_DEBUG_LEVEL_2, "Copying buf[0] = %#x to ptr_port->Register", hi2c.msg->buf[0])
      ptr_port->Register = hi2c.msg->buf[0];

      /* Get information about how many data to send from first received byte */
      ptr_port->PtrBuffer = UCSI_GetDataSizeRegisterPtr(ptr_port->Register, &ptr_port->DataSize);

      if (ptr_port->PtrBuffer != NULL)
      {
        /* Copy the rest of the payload to the desired register if there is something left to copy */
        if ((hi2c.msg->len - 1U) != 0U)
        {
          UCSI_IF_PPM_DEBUG_TRACE(UCSI_DEBUG_LEVEL_2, "memcpy size = %ld", (hi2c.msg->len - 1U));

          memcpy(ptr_port->PtrBuffer, &hi2c.msg->buf[1], (hi2c.msg->len - 1U));

          UCSI_IF_Handle.ReceiveCommandCb((uint8_t)ptr_port->Register, ptr_port->PtrBuffer);

          /* Write completed : clean port for next request */
          clean_port(ptr_port);
          /* Acknowledge the received message */
          UCSI_IF_PPM_DEBUG_TRACE(UCSI_DEBUG_LEVEL_2, "VIRT I2C ACK");
          if (VIRT_I2C_OK != VIRT_I2C_sendACK(&hi2c, VIRT_I2C_ACK))
          {
            UCSI_IF_PPM_DEBUG_TRACE(UCSI_DEBUG_LEVEL_1, "VIRT I2C ACK failed");
          }
        }
        else
        {
          /* Acknowledge the received message */
          UCSI_IF_PPM_DEBUG_TRACE(UCSI_DEBUG_LEVEL_2, "VIRT I2C ACK");
          if (VIRT_I2C_OK != VIRT_I2C_sendACK(&hi2c, VIRT_I2C_ACK))
          {
            UCSI_IF_PPM_DEBUG_TRACE(UCSI_DEBUG_LEVEL_1, "VIRT I2C ACK failed");
          }
        }
      }
      else
      {
        UCSI_IF_PPM_DEBUG_TRACE(UCSI_DEBUG_LEVEL_2, "Error ptr_port->PtrBuffer is NULL");

        /* Error detected */
        ptr_port->Error = UCSI_ERROR_SEQ_TRANSMIT;
        clean_port(ptr_port);
        /* NACK the unknown request */
        UCSI_IF_PPM_DEBUG_TRACE(UCSI_DEBUG_LEVEL_2, "VIRT I2C NACK");
        if (VIRT_I2C_OK != VIRT_I2C_sendACK(&hi2c, VIRT_I2C_NACK))
        {
          UCSI_IF_PPM_DEBUG_TRACE(UCSI_DEBUG_LEVEL_1, "VIRT I2C NACK failed");
        }

      }
    }
    /* SLAVE TO MASTER --> Slave is sending */
    else
    {
      UCSI_IF_PPM_DEBUG_TRACE(UCSI_DEBUG_LEVEL_2, "Direction SLAVE TO MASTER");
      /* Avoid sending more data than asked by the master */
      ptr_port->DataSize = MIN(ptr_port->DataSize, hi2c.msg->len);

      /* Get information about how many data to send */
      if (ptr_port->PtrBuffer != NULL)
      {
        UCSI_IF_PPM_DEBUG_TRACE(UCSI_DEBUG_LEVEL_2, "Sending %d bytes: buff[0..1]=%#x %#x", ptr_port->DataSize, ptr_port->PtrBuffer[0], ptr_port->PtrBuffer[1]);
        if (VIRT_I2C_OK != VIRT_I2C_sendToMaster(&hi2c, ptr_port->PtrBuffer, ptr_port->DataSize))
        {
          /* Error detected */
          UCSI_IF_PPM_TRACE(UCSI_DEBUG_LEVEL_0, I2C_ADDRESS_UCSI, UCSI_TRACE_ADDRCB_ERROR_TX, NULL, 0U);
          ptr_port->Error = UCSI_ERROR_SEQ_TRANSMIT;
          UCSI_IF_PPM_DEBUG_TRACE(UCSI_DEBUG_LEVEL_2, "VIRT_I2C_sendToMaster failed. datasize=%d", ptr_port->DataSize);
        }
        /* Transfer complete, no error */
        else
        {
          if (ptr_port->Register < UcsiRegDataStructureUnkown)
          {
            UCSI_IF_PPM_TRACE(UCSI_DEBUG_LEVEL_1, I2C_ADDRESS_UCSI, UCSI_TRACE_TX_CPLT_COMMAND,
                            (uint8_t* )&ptr_port->Register, 1U);
            UCSI_IF_PPM_DEBUG_TRACE(UCSI_DEBUG_LEVEL_2, "VIRT_I2C_sendToMaster completed");
          }
          else
          {
            UCSI_IF_PPM_TRACE(UCSI_DEBUG_LEVEL_1, I2C_ADDRESS_UCSI, UCSI_TRACE_TX_CPLT_ERROR,
                            (uint8_t* )&ptr_port->Register, 1U);
          }
        }
        /* Clean ptr_port for next transfer */
        clean_port(ptr_port);
      }
      else
      {
        /* Error detected */
        UCSI_IF_PPM_DEBUG_TRACE(UCSI_DEBUG_LEVEL_2, "Error: Ptr is NULL");
        UCSI_IF_PPM_TRACE(UCSI_DEBUG_LEVEL_0, I2C_ADDRESS_UCSI, UCSI_TRACE_ADDRCB_ERROR_TX, NULL, 0U);
        ptr_port->Error = UCSI_ERROR_SEQ_TRANSMIT;
        /* Clean ptr_port for next transfer */
        clean_port(ptr_port);
      }
    }
  }
}

/*************************************************************************/
/* VIRT I2C callback implementation                                      */
/*************************************************************************/
static void VIRT_I2C_RxCpltCallback(VIRT_I2C_HandleTypeDef *hi2c, size_t len)
{
  uint8_t addr;

  addr = VIRT_I2C_addrFromMsg(hi2c);

  /* test the validity of the address on the bus */
  if (addr == I2C_ADDRESS_UCSI)
  {
    (void)osSemaphoreRelease(sem_i2c_cmd_req);
    UCSI_IF_PPM_DEBUG_TRACE(UCSI_DEBUG_LEVEL_2, "VIRT_I2C_RxCpltCallback: release sem_i2c_cmd_req.");
  }
  else
  {
    /* unexpected address, NAK the message */
    VIRT_I2C_sendACK(hi2c, VIRT_I2C_NACK);
  }
}

/**
  * @brief  Initialize UCSI Alert
  * @param  None
  * @retval None
  */
static void UCSI_AlertInit(void)
{
  UCSI_IF_PPM_DEBUG_TRACE(UCSI_DEBUG_LEVEL_2, "Virtual INTC OpenAMP-rpmsg channel creation.");
  if (VIRT_INTC_Init(&hintc, 32) != VIRT_INTC_OK) {
    UCSI_IF_PPM_DEBUG_TRACE(UCSI_DEBUG_LEVEL_2, "VIRT_INTC_Init failed.");
    Error_Handler();
  }

  /* Register callback for message reception by channels */
  UCSI_IF_PPM_DEBUG_TRACE(UCSI_DEBUG_LEVEL_2, "Virtual INTC OpenAMP-rpmsg callback registering.");
  if(VIRT_INTC_RegisterCallback(&hintc, VIRT_INTC_RXCPLT_CB_ID, NULL) != VIRT_INTC_OK)
  {
    UCSI_IF_PPM_DEBUG_TRACE(UCSI_DEBUG_LEVEL_2, "VIRT_INTC_RegisterCallback failed.");
    Error_Handler();
  }
}

/**
  * @brief  Return the datasize/pointer @ for a register used only for read operation
  * @param  reg          UCSI command
  * @param  PtrDataSize  Pointer on the size of the UCSI command
  * @retval Pointer on the register
  */
static uint8_t *UCSI_GetDataSizeRegisterPtr(UCSI_REG_t reg, uint8_t *PtrDataSize)
{
  switch (reg)
  {
    case UcsiRegDataStructureVersion:
      *PtrDataSize = (uint8_t)sizeof(UCSI_VERSION_t);
      break;
    case UcsiRegDataStructureCci:
      *PtrDataSize = (uint8_t)sizeof(UCSI_CCI_t);
      break;
    case UcsiRegDataStructureControl:
      *PtrDataSize = (uint8_t)sizeof(UCSI_CONTROL_t);
      break;
    case UcsiRegDataStructureMessageIn:
      *PtrDataSize = (uint8_t)sizeof(UCSI_MESSAGE_IN_t);
      break;
    case UcsiRegDataStructureMessageOut:
      *PtrDataSize = (uint8_t)sizeof(UCSI_MESSAGE_OUT_t);
      break;
    case UcsiRegDataStructureUnkown:
    case UcsiRegDataStructureReserved:
    default:
      /* Should not happen */
      *PtrDataSize = 0U;
      UCSI_IF_PPM_TRACE(UCSI_DEBUG_LEVEL_0, I2C_ADDRESS_UCSI, UCSI_TRACE_UCSI_GET, NULL, 0U);
      break;
  }
  return USBPD_PPM_GetDataPointer(reg);
}
