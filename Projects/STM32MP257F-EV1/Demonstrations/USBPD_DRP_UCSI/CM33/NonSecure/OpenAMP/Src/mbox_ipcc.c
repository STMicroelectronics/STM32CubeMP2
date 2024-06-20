/**
  ******************************************************************************
  * @file    mbox_ipcc.c
  * @author  MCD Application Team
  * @brief   This file provides code for the configuration
  *                      of the mailbox_ipcc_if.c MiddleWare.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                       opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/*
 * Channel direction and usage:
 *
 *  ========   <-- new msg ---=============--------<------   =======
 * ||      ||                || CHANNEL 1 ||                ||     ||
 * ||  A35 ||  ------->-------=============--- buf free-->  || M33 ||
 * ||      ||                                               ||     ||
 * ||master||  <-- buf free---=============--------<------  ||slave||
 * ||      ||                || CHANNEL 2 ||                ||     ||
 * ||      ||  ------->-------=============----new msg -->  ||     ||
 * ||      ||                                               ||     ||
 * ||master||  ------->-------=============---- Detach -->  ||slave||
 * ||      ||                || CHANNEL 4 ||                ||     ||
 *  ========   <-- Ack -------=============--------<------   =======
 */

/* Includes ------------------------------------------------------------------*/
#include "cmsis_os2.h"
#include "FreeRTOSConfig.h"
#include "portmacrocommon.h"
#include "openamp/open_amp.h"
#include "stm32mp2xx_hal.h"
#include "openamp_conf.h"

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN Define */

/* USER CODE END Define */

#define MASTER_CPU_ID       0
#define REMOTE_CPU_ID       1
#define IPCC_CPU_A7         MASTER_CPU_ID
#define IPCC_CPU_M4         REMOTE_CPU_ID

#define MBOX_NO_MSG           0
#define MBOX_NEW_MSG          1
#define MBOX_BUF_FREE         2

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

extern IPCC_HandleTypeDef hipcc;
uint32_t vring0_id = 0; /* used for channel 1 */
uint32_t vring1_id = 1; /* used for channel 2 */

static osSemaphoreId_t ipcc_semaphore;
static const osSemaphoreAttr_t semAttr_IpccSemaphore = {
    .name = "IPCC",
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

void IPCC_channel1_callback(IPCC_HandleTypeDef * hipcc, uint32_t ChannelIndex, IPCC_CHANNELDirTypeDef ChannelDir);
void IPCC_channel2_callback(IPCC_HandleTypeDef * hipcc, uint32_t ChannelIndex, IPCC_CHANNELDirTypeDef ChannelDir);
void CoproSync_DetachCb(IPCC_HandleTypeDef * hipcc, uint32_t ChannelIndex, IPCC_CHANNELDirTypeDef ChannelDir);


/**
  * @brief  Initialize MAILBOX with IPCC peripheral
  * @param  None
  * @retval : Operation result
  */
int MAILBOX_Init(void)
{

  /* The Linux defines the maximum message queue of 20 elements */
  ipcc_semaphore = osSemaphoreNew(20, 0, &semAttr_IpccSemaphore);
  if(ipcc_semaphore == NULL)
  {
    OPENAMP_log_err("%s: ipcc sem failed\n", __func__);

    return -1;
  }

  /* USER CODE BEGIN PRE_MAILBOX_INIT */

  /* USER CODE END  PRE_MAILBOX_INIT */

  if (HAL_IPCC_ActivateNotification(&hipcc, IPCC_CHANNEL_1, IPCC_CHANNEL_DIR_RX,
                                    IPCC_channel1_callback) != HAL_OK)
  {
    OPENAMP_log_err("%s: ch_1 RX fail\n", __func__);
    return -1;
  }

  if (HAL_IPCC_ActivateNotification(&hipcc, IPCC_CHANNEL_2, IPCC_CHANNEL_DIR_RX,
                                    IPCC_channel2_callback) != HAL_OK)
  {
    OPENAMP_log_err("%s: ch_2 RX fail\n", __func__);
    return -1;
  }

  if (HAL_IPCC_ActivateNotification(&hipcc, IPCC_CHANNEL_4, IPCC_CHANNEL_DIR_RX,
                                    CoproSync_DetachCb) != HAL_OK)
  {
    OPENAMP_log_err("%s: ch_4 RX fail\n", __func__);
    return -1;
  }

  /* USER CODE BEGIN POST_MAILBOX_INIT */

  /* USER CODE END  POST_MAILBOX_INIT */
  return 0;
}

/**
  * @brief  DeInitialize MAILBOX with IPCC peripheral
  * @param  None
  * @retval : None
  */
void MAILBOX_DeInit(void)
{
  /* USER CODE BEGIN PRE_MAILBOX_INIT */

  /* USER CODE END  PRE_MAILBOX_INIT */

  if (HAL_IPCC_DeActivateNotification(&hipcc, IPCC_CHANNEL_1, IPCC_CHANNEL_DIR_RX) != HAL_OK)
  {
    OPENAMP_log_err("%s: ch_1 RX fail\n", __func__);
  }

  if (HAL_IPCC_DeActivateNotification(&hipcc, IPCC_CHANNEL_2, IPCC_CHANNEL_DIR_RX) != HAL_OK)
  {
    OPENAMP_log_err("%s: ch_2 RX fail\n", __func__);
  }

  /* Do not deactivate IPCC_CHANNEL_4 as used for shutdown acknowledgment */

  /* USER CODE BEGIN POST_MAILBOX_INIT */

  /* USER CODE END  POST_MAILBOX_INIT */
}

/**
  * @brief  Initialize MAILBOX with IPCC peripheral
  * @param  virtio device
  * @retval : Operation result
  */
int MAILBOX_Poll(struct virtio_device *vdev)
{
  /* USER CODE BEGIN PRE_MAILBOX_POLL */

  /* USER CODE END  PRE_MAILBOX_POLL */

  if (osSemaphoreAcquire(ipcc_semaphore, osWaitForever) != osOK)
  {
    return -1;
  }

  /* USER CODE BEGIN MSG_CHANNEL2 */

  /* USER CODE END  MSG_CHANNEL2 */

  OPENAMP_log_dbg("Running virt1 (ch_2 new msg)\r\n");
  rproc_virtio_notified(vdev, VRING1_ID);

  /* USER CODE BEGIN POST_MAILBOX_POLL */

  /* USER CODE END  POST_MAILBOX_POLL */

  return 0;
}


/**
  * @brief  Callback function called by OpenAMP MW to notify message processing
  * @param  VRING id
  * @retval Operation result
  */
int MAILBOX_Notify(void *priv, uint32_t id)
{
  uint32_t channel;
  (void)priv;

   /* USER CODE BEGIN PRE_MAILBOX_NOTIFY */

   /* USER CODE END  PRE_MAILBOX_NOTIFY */

  /* Called after virtqueue processing: time to inform the remote */
  if (id == VRING0_ID)
  {
    channel = IPCC_CHANNEL_1;
    OPENAMP_log_dbg("Send msg on ch_1\r\n");
  }
  else if (id == VRING1_ID)
  {
    /* Note: the OpenAMP framework never notifies this */
    channel = IPCC_CHANNEL_2;
    OPENAMP_log_dbg("Send 'buff free' on ch_2\r\n");
  }
  else
  {
    OPENAMP_log_err("invalid vring (%d)\r\n", (int)id);
    return -1;
  }

  /* Check that the channel is free (otherwise wait until it is) */
  if (HAL_IPCC_GetChannelStatus(&hipcc, channel, IPCC_CHANNEL_DIR_TX) == IPCC_CHANNEL_STATUS_OCCUPIED)
  {
    OPENAMP_log_dbg("Waiting for channel to be freed\r\n");
    while (HAL_IPCC_GetChannelStatus(&hipcc, channel, IPCC_CHANNEL_DIR_TX) == IPCC_CHANNEL_STATUS_OCCUPIED);
  }

  /* Inform A7 (either new message, or buf free) */
  HAL_IPCC_NotifyCPU(&hipcc, channel, IPCC_CHANNEL_DIR_TX);

  /* USER CODE BEGIN POST_MAILBOX_NOTIFY */

  /* USER CODE END  POST_MAILBOX_NOTIFY */

  return 0;
}

/* Private function  ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */
/* Callback from IPCC Interrupt Handler: Master Processor informs that there are some free buffers */
void IPCC_channel1_callback(IPCC_HandleTypeDef * hipcc,
         uint32_t ChannelIndex, IPCC_CHANNELDirTypeDef ChannelDir)
{
  /* USER CODE BEGIN PRE_MAILBOX_CHANNEL1_CALLBACK */

  /* USER CODE END  PRE_MAILBOX_CHANNEL1_CALLBACK */

  /* Inform A7 that we have received the 'buff free' msg */
  HAL_IPCC_NotifyCPU(hipcc, ChannelIndex, IPCC_CHANNEL_DIR_RX);
  /* USER CODE BEGIN POST_MAILBOX_CHANNEL1_CALLBACK */

  /* USER CODE END  POST_MAILBOX_CHANNEL1_CALLBACK */
}

/* Callback from IPCC Interrupt Handler: new message received from Master Processor */
void IPCC_channel2_callback(IPCC_HandleTypeDef * hipcc,
         uint32_t ChannelIndex, IPCC_CHANNELDirTypeDef ChannelDir)
{

  /* USER CODE BEGIN PRE_MAILBOX_CHANNEL2_CALLBACK */

  /* USER CODE END  PRE_MAILBOX_CHANNEL2_CALLBACK */

  /* Inform A7 that we have received the new msg */
  HAL_IPCC_NotifyCPU(hipcc, ChannelIndex, IPCC_CHANNEL_DIR_RX);

  osSemaphoreRelease(ipcc_semaphore);
  /* USER CODE BEGIN POST_MAILBOX_CHANNEL2_CALLBACK */

  /* USER CODE END  POST_MAILBOX_CHANNEL2_CALLBACK */
}

/**
  * @brief  Callback from IPCC Interrupt Handler: Remote Processor asks local processor to stop to communicate with
  *  it.
  * @param  hipcc IPCC handle
  * @param  ChannelIndex Channel number
  * @param  ChannelDir Channel direction
  * @retval None
  */
__weak void CoproSync_DetachCb(IPCC_HandleTypeDef * hipcc, uint32_t ChannelIndex, IPCC_CHANNELDirTypeDef ChannelDir)
{
  /* by default nothing done */
  HAL_IPCC_NotifyCPU(hipcc, ChannelIndex, IPCC_CHANNEL_DIR_RX);

}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
