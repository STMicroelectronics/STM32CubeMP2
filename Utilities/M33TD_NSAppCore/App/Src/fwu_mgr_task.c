/**
  ******************************************************************************
  * @file    fwu_mgr_task.c
  * @author  MPU Embedded Software
  * @brief   Firmware Update Manager task implementation.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/**
  * @addtogroup FwuMgrTask
  * @{
  */

/* Includes ------------------------------------------------------------------*/
#include "fwu_mgr_task.h"
#include "openamp_log.h"
#include <string.h>
#if ENABLE_OPENAMP_TASK
#include "openamp_task.h"
#endif
#include "remoteproc_task.h"

/* Private defines -----------------------------------------------------------*/
#define FWU_MGR_QUEUE_SIZE   8U

/* Private typedef -----------------------------------------------------------*/
/**
  * @brief  Runtime state tracked for a single FWU component.
  */
typedef struct
{
  bool pendingUpdate;
  psa_fwu_component_info_t bootedComponentInfo;
} FwuComponentState_t;

/* Private variables ---------------------------------------------------------*/
static osMessageQueueId_t fwuMgrQueueHandle;
static osThreadId_t       fwuMgrTaskHandle;
static FwuComponentState_t fwuComponents[FWU_COMPONENT_NUMBER];

static const osMessageQueueAttr_t fwuMgrQueueAttr =
{
  .name = "FwuMgrQueue"
};

static const osThreadAttr_t fwuMgrTaskAttr =
{
  .name       = "FwuMgrTask",
  .priority   = (osPriority_t)FWU_MGR_TASK_PRIORITY,
  .stack_size = FWU_MGR_TASK_STACK_SIZE,
};

/* Forward declarations ------------------------------------------------------*/
static void FwuMgrTask(void *argument);
static void FwuMgrTask_PostResponseToA35(const FwuMgrMessage_t *msg);
static const char *FwuMgrTask_GetComponentNameFromId(uint32_t component_id);
static void FwuMgrTask_PrepareResponse(FwuMgrMessage_t *rsp,
                                       uint32_t command,
                                       uint32_t component_id,
                                       psa_status_t error);
static void FwuMgrTask_PostStatusResponse(FwuMgrMessage_t *rsp,
                                          uint32_t command,
                                          uint32_t component_id,
                                          psa_status_t error);
static void FwuMgrTask_PostInfoResponse(FwuMgrMessage_t *rsp, uint32_t component_id);

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Send a structured FWU message to A35 over the FWU RPMsg endpoint.
  * @param  msg: Pointer to the message to send.
  */
static void FwuMgrTask_PostResponseToA35(const FwuMgrMessage_t *msg)
{
  OpenampTaskCommand_t openampCommand;

  if (msg != NULL)
  {
    memset(&openampCommand, 0, sizeof(openampCommand));
    openampCommand.type = OPENAMP_CMD_FWU_TX;
    openampCommand.payload.fwuMessage = *msg;
    (void)OpenampTask_PostCommand(&openampCommand);
  }
}

/**
  * @brief  Map a component ID to a printable name.
  * @param  component_id: FWU_COMPONENT_ID_xxx.
  * @retval Constant string or "unknown" if not recognized.
  */
static const char *FwuMgrTask_GetComponentNameFromId(uint32_t component_id)
{
  switch (component_id)
  {
    case FWU_COMPONENT_ID_DDR_FIRMWARE:
      return "ddr_fw";

    case FWU_COMPONENT_ID_TFM_S_NS:
      return "tfm_s_ns";

#if (FWU_COMPONENT_NUMBER == 3)
    case FWU_COMPONENT_ID_CA35_FIRMWARE:
      return "ca35_fw";
#endif

    default:
      return "unknown";
  }
}

/**
  * @brief  Initialize a FWU message header.
  * @param  rsp: Response object to initialize.
  * @param  command: FWU_MGR_CMD_xxx value.
  * @param  component_id: Target component identifier.
  * @param  error: PSA status to report.
  */
static void FwuMgrTask_PrepareResponse(FwuMgrMessage_t *rsp,
                                       uint32_t command,
                                       uint32_t component_id,
                                       psa_status_t error)
{
  if (rsp == NULL)
  {
    return;
  }

  memset(rsp, 0, sizeof(*rsp));
  rsp->command = command;
  rsp->componentId = component_id;
  rsp->status = error;
}

/**
  * @brief  Send a simple FWU response that does not carry version info.
  * @param  rsp: Response object to reuse.
  * @param  command: FWU_MGR_CMD_xxx value.
  * @param  component_id: Target component identifier.
  * @param  error: PSA status to report.
  */
static void FwuMgrTask_PostStatusResponse(FwuMgrMessage_t *rsp,
                                          uint32_t command,
                                          uint32_t component_id,
                                          psa_status_t error)
{
  FwuMgrTask_PrepareResponse(rsp, command, component_id, error);
  FwuMgrTask_PostResponseToA35(rsp);
}

/**
  * @brief  Query FWU component info and send the corresponding INFO response.
  * @param  rsp: Response object to reuse.
  * @param  component_id: Target component identifier.
  */
static void FwuMgrTask_PostInfoResponse(FwuMgrMessage_t *rsp, uint32_t component_id)
{
  psa_fwu_component_info_t componentInfo;
  psa_status_t status;

  FwuMgrTask_PrepareResponse(rsp, FWU_MGR_CMD_INFO, component_id, PSA_SUCCESS);

  status = psa_fwu_query(component_id, &componentInfo);
  rsp->status = status;
  if (status == PSA_SUCCESS)
  {
    memcpy(&rsp->info, &componentInfo.version, sizeof(psa_fwu_image_version_t));
  }
  else
  {
    const char *componentName = FwuMgrTask_GetComponentNameFromId(component_id);
    APP_LOG_ERR("FwuMgr", "%s query failed (status=%ld)", componentName, (long)status);
  }

  FwuMgrTask_PostResponseToA35(rsp);
}

/**
  * @brief  FWU Manager Task main loop.
  * @param  argument: Not used.
  */
static void FwuMgrTask(void *argument)
{
  FwuMgrMessage_t msg;
  FwuMgrMessage_t rsp;
  psa_status_t status = PSA_SUCCESS;
  #if TFM_CONFIG_FWU_STORAGE_ACCESS
  int componentId;
  #endif

  (void)argument;

  for (;;)
  {
    if (osMessageQueueGet(fwuMgrQueueHandle, &msg, NULL, osWaitForever) == osOK)
    {
      switch (msg.command)
      {
        case FWU_MGR_CMD_INIT:
          for (int i = 0; i < FWU_COMPONENT_NUMBER; ++i)
          {
            status = psa_fwu_query((uint32_t)i, &fwuComponents[i].bootedComponentInfo);
            if (status != PSA_SUCCESS)
            {
              const char *componentName = FwuMgrTask_GetComponentNameFromId((uint32_t)i);
              APP_LOG_ERR("FwuMgr", "%s query failed (status=%ld)", componentName, (long)status);
            }
          }
          break;

        case FWU_MGR_CMD_LIST:
          /* return the number of component. */
          FwuMgrTask_PostStatusResponse(&rsp,
                                    FWU_MGR_CMD_LIST,
                                    FWU_COMPONENT_NUMBER,
                                    PSA_SUCCESS);
          break;

        case FWU_MGR_CMD_INFO:
          if (msg.componentId == FWU_COMPONENT_ID_ALL)
          {
            for (uint32_t componentId = 0U; componentId < FWU_COMPONENT_NUMBER; ++componentId)
            {
              FwuMgrTask_PostInfoResponse(&rsp, componentId);
            }
          }
          else
          {
            FwuMgrTask_PostInfoResponse(&rsp, msg.componentId);
          }
          break;

        case FWU_MGR_CMD_CANCEL:
          if (msg.componentId == FWU_COMPONENT_ID_ALL)
          {
            for (int i = 0; i < FWU_COMPONENT_NUMBER; ++i)
            {
              fwuComponents[i].pendingUpdate = false;
            }
          }
          else
          {
            fwuComponents[msg.componentId].pendingUpdate = false;
          }

          FwuMgrTask_PostStatusResponse(&rsp,
                                    FWU_MGR_CMD_CANCEL,
                                    (msg.componentId == FWU_COMPONENT_ID_ALL) ? FWU_COMPONENT_ID_ALL : msg.componentId,
                                    PSA_SUCCESS);
          break;

        case FWU_MGR_CMD_INSTALL:
          if (msg.componentId == FWU_COMPONENT_ID_ALL)
          {
            for (int i = 0; i < FWU_COMPONENT_NUMBER; ++i)
            {
              fwuComponents[i].pendingUpdate = true;
            }
          }
          else
          {
            fwuComponents[msg.componentId].pendingUpdate = true;
          }

          FwuMgrTask_PostStatusResponse(&rsp,
                                    FWU_MGR_CMD_INSTALL,
                                    (msg.componentId == FWU_COMPONENT_ID_ALL) ? FWU_COMPONENT_ID_ALL : msg.componentId,
                                    PSA_SUCCESS);
          break;

        case FWU_MGR_CMD_REBOOT:
#if TFM_CONFIG_FWU_STORAGE_ACCESS
          APP_LOG_DBG("FwuMgr", "Stopping A35...");
          if (RemoteProcTask_PostEvent(REMOTEPROC_EVENT_STOP, 0))
          {
            APP_LOG_ERR("FwuMgr", "A35 stop failed");
          }
          else
          {
            osThreadYield();
            APP_LOG_DBG("FwuMgr", "Starting write process...");

            for (componentId = 0; componentId < FWU_COMPONENT_NUMBER; ++componentId)
            {
              if (fwuComponents[componentId].pendingUpdate)
              {
                uint32_t offset;
                uint32_t remaining;
                uint32_t chunkSize;
                uint8_t *baseAddress;

                status = psa_fwu_start(componentId, NULL, 0);
                if (status != PSA_SUCCESS)
                {
                  break;
                }

                baseAddress = (uint8_t *)FWU_COMPONENT_ADDR(componentId);
                remaining = FWU_COMPONENT_SIZE(componentId);
                offset = 0U;

                while (remaining > 0U)
                {
                  chunkSize = (remaining > PSA_FWU_MAX_WRITE_SIZE) ?
                              PSA_FWU_MAX_WRITE_SIZE :
                              remaining;

                  status = psa_fwu_write(componentId, offset, (void *)(baseAddress + offset), chunkSize);
                  if (status != PSA_SUCCESS)
                  {
                    APP_LOG_ERR("FwuMgr", "Write failed at 0x%p", (void *)baseAddress);
                    break;
                  }

                  offset += chunkSize;
                  remaining -= chunkSize;

                  osThreadYield();
                }

                if (status != PSA_SUCCESS)
                {
                  break;
                }

                status = psa_fwu_finish(componentId);
                if (status != PSA_SUCCESS)
                {
                  break;
                }
              }
            }

            if (status != PSA_SUCCESS)
            {
              APP_LOG_ERR("FwuMgr", "Component %d: write process failed (%ld)", componentId, status);
            }
            else
            {
              status = psa_fwu_install();
              if (status != PSA_SUCCESS)
              {
                APP_LOG_ERR("FwuMgr", "psa_fwu_install failed. status: %ld", status);
              }
            }
          }
#endif /* TFM_CONFIG_FWU_STORAGE_ACCESS */
          /* reset the platform, this will reset FWU partition state in case of error */
          APP_LOG_DBG("FwuMgr", "Rebooting platform...");
          (void)psa_fwu_request_reboot();
          break;

        case FWU_MGR_CMD_ACCEPT:
          status = psa_fwu_accept();

          if (status < PSA_SUCCESS)
          {
            APP_LOG_ERR("FwuMgr", "psa_fwu_accept failed (status=%ld)", (long)status);
          }

          FwuMgrTask_PostStatusResponse(&rsp,
                                    FWU_MGR_CMD_ACCEPT,
                                    FWU_COMPONENT_ID_ALL,
                                    status);
          break;

        case FWU_MGR_CMD_REJECT:
          status = psa_fwu_reject(PSA_ERROR_GENERIC_ERROR);

          if (status < PSA_SUCCESS)
          {
            APP_LOG_ERR("FwuMgr", "psa_fwu_reject failed (status=%ld)", (long)status);
          }

          FwuMgrTask_PostStatusResponse(&rsp,
                                    FWU_MGR_CMD_REJECT,
                                    FWU_COMPONENT_ID_ALL,
                                    status);
          break;

        default:
          APP_LOG_ERR("FwuMgr", "Unknown FWU command (%lu)", (unsigned long)msg.command);
          break;
      }
    }
  }
}

/* Exported functions --------------------------------------------------------*/
/**
  * @brief  Initialize the FWU Manager Task and its resources.
  * @retval None
  */
void FwuMgrTask_Init(void)
{
  FwuMgrMessage_t msg;

  memset(&fwuComponents, 0, sizeof(fwuComponents));

  fwuMgrQueueHandle = osMessageQueueNew(FWU_MGR_QUEUE_SIZE,
                                        sizeof(FwuMgrMessage_t),
                                        &fwuMgrQueueAttr);
  if (fwuMgrQueueHandle == NULL)
  {
    NSAppCore_ErrorHandler();
  }

  fwuMgrTaskHandle = osThreadNew(FwuMgrTask, NULL, &fwuMgrTaskAttr);
  if (fwuMgrTaskHandle == NULL)
  {
    NSAppCore_ErrorHandler();
  }

  memset(&msg, 0, sizeof(msg));
  msg.command      = FWU_MGR_CMD_INIT;
  msg.componentId = FWU_COMPONENT_ID_ALL;
  (void)FwuMgrTask_PostCommand(&msg);
}

/**
  * @brief  De-initialize the FWU Manager Task and release its resources.
  * @retval None
  */
void FwuMgrTask_DeInit(void)
{
  if (fwuMgrTaskHandle != NULL)
  {
    (void)osThreadTerminate(fwuMgrTaskHandle);
    fwuMgrTaskHandle = NULL;
  }

  if (fwuMgrQueueHandle != NULL)
  {
    (void)osMessageQueueDelete(fwuMgrQueueHandle);
    fwuMgrQueueHandle = NULL;
  }
}

/**
  * @brief  Post a FWU message to the FWU Manager task queue.
  * @param  msg: Pointer to the message to post.
  * @retval true if posted successfully, false otherwise.
  */
bool FwuMgrTask_PostCommand(const FwuMgrMessage_t *msg)
{
  if ((fwuMgrQueueHandle == NULL) || (msg == NULL))
  {
    return false;
  }

  return (osMessageQueuePut(fwuMgrQueueHandle, msg, 0U, 0U) == osOK);
}

/**
  * @brief  RPMsg reception callback for the FWU endpoint.
  * @param  ept: Local RPMsg endpoint.
  * @param  data: Pointer to received buffer.
  * @param  len: Buffer length.
  * @param  src: Remote endpoint address.
  * @param  priv: User context.
  * @retval 0 on success, <0 on error.
  */
int FwuMgrTask_RpmsgCallback(struct rpmsg_endpoint *ept,
                             void *data, size_t len,
                             uint32_t src, void *priv)
{
  FwuMgrMessage_t *message = (FwuMgrMessage_t *)data;

  (void)ept;
  (void)src;
  (void)priv;

  if ((data == NULL) || (len < sizeof(FwuMgrMessage_t)))
  {
    APP_LOG_ERR("FwuMgr", "Bad FWU message (len=%lu)", (unsigned long)len);
    return -1;
  }

  if (!FwuMgrTask_PostCommand(message))
  {
    APP_LOG_ERR("FwuMgr", "Failed to post FWU message from RPMsg");
    return -1;
  }

  return 0;
}

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/