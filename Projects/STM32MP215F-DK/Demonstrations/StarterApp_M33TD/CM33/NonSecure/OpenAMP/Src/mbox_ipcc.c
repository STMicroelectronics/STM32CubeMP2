/**
 ******************************************************************************
 * @file    mbox_ipcc.c
 * @author  MCD Application Team
 * @brief   This file provides code for the configuration
 *          of the mailbox_ipcc_if.c Middleware.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2019-2026 STMicroelectronics.
 * All rights reserved.
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *     opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */

/*
 * Channel direction and usage for OpenAMP IPC:
 *
 *  ========   <-- new msg ---=============--------<------   =======
 * ||      ||                || CHANNEL 1 ||                ||     ||
 * ||  A7  ||  ------->-------=============--- buf free-->  || M4  ||
 * ||      ||                                               ||     ||
 * ||master||  <-- buf free---=============--------<------  ||slave||
 * ||      ||                || CHANNEL 2 ||                ||     ||
 *  ========   ------->-------=============----new msg -->   =======
 */

/* Includes ------------------------------------------------------------------*/
#include "mbox_ipcc.h"
#include "openamp/open_amp.h"
#include "stm32mp2xx_hal.h"
#include "openamp_conf.h"

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN Define */

/* USER CODE END Define */

#define MAILBOX_CHANNEL_1_IDX 0U
#define MAILBOX_CHANNEL_2_IDX 1U
#define MAILBOX_NUM_CHANNELS  2U
/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

static void IPCC_channel_callback(IPCC_HandleTypeDef *hipcc, uint32_t ChannelIndex,
                                  IPCC_CHANNELDirTypeDef ChannelDir);

typedef void (*MAILBOX_ChannelCallback_t)(IPCC_HandleTypeDef *hipcc,
                                          uint32_t ChannelIndex,
                                          IPCC_CHANNELDirTypeDef ChannelDir);

typedef struct
{
  uint32_t msg_receive_count;
  MAILBOX_ChannelCallback_t callback;
} MAILBOX_ChannelContext_t;

static MAILBOX_ChannelContext_t mailbox_chnl[MAILBOX_NUM_CHANNELS] =
{
  {0U, IPCC_channel_callback},
  {0U, IPCC_channel_callback}
};

/* Exported functions --------------------------------------------------------*/

/**
 * @brief  Initialize MAILBOX with IPCC peripheral
 * @param  hipcc: Pointer to IPCC handle
 * @retval 0 on success, -1 on error
 */
int MAILBOX_Init(IPCC_HandleTypeDef *hipcc, MAILBOX_ChannelCallback_t cb)
{

   /* USER CODE BEGIN PRE_MAILBOX_INIT */

   /* USER CODE END  PRE_MAILBOX_INIT */

  if (hipcc == NULL)
  {
    return -1;
  }

  for (uint32_t i = 0; i < MAILBOX_NUM_CHANNELS; i++)
  {
    if (HAL_IPCC_ActivateNotification(hipcc, i, IPCC_CHANNEL_DIR_RX,
                                      cb ? cb : mailbox_chnl[i].callback) != HAL_OK)
    {
      OPENAMP_log_err("%s: ch_%lu RX fail\n", __func__, i);
      return -1;
    }
  }

  /* USER CODE BEGIN POST_MAILBOX_INIT */

  /* USER CODE END  POST_MAILBOX_INIT */
  return 0;
}

/**
 * @brief  DeInitialize MAILBOX with IPCC peripheral
 * @param  hipcc: Pointer to IPCC handle
 * @retval None
 */
void MAILBOX_DeInit(IPCC_HandleTypeDef *hipcc)
{
  /* USER CODE BEGIN PRE_MAILBOX_DEINIT */

  /* USER CODE END  PRE_MAILBOX_DEINIT */

  if (hipcc == NULL)
  {
    return;
  }

  for (uint32_t i = 0; i < MAILBOX_NUM_CHANNELS; i++)
  {
    if (HAL_IPCC_DeActivateNotification(hipcc, i, IPCC_CHANNEL_DIR_RX) != HAL_OK)
    {
      OPENAMP_log_err("%s: ch_%lu RX fail", __func__, i);
    }
  }

  /* USER CODE BEGIN POST_MAILBOX_DEINIT */

  /* USER CODE END  POST_MAILBOX_DEINIT */
}

/**
 * @brief  Poll for new messages on the mailbox
 * @param  vdev: Pointer to virtio device
 * @retval 0 on success, negative value on error
 */
int MAILBOX_Poll(struct virtio_device *vdev)
{
  unsigned int nb_inbox_msg = 0;
  int ret = 0;

  /* USER CODE BEGIN PRE_MAILBOX_POLL */

  /* USER CODE END PRE_MAILBOX_POLL */

  if (mailbox_chnl[MAILBOX_CHANNEL_1_IDX].msg_receive_count)
  {
    nb_inbox_msg = mailbox_chnl[MAILBOX_CHANNEL_1_IDX].msg_receive_count;
    /* USER CODE BEGIN MSG_CHANNEL1 */
    /* USER CODE END MSG_CHANNEL1 */

    OPENAMP_log_dbg("Running virt0 (ch_1 buf free)\r\n");
    ret = rproc_virtio_notified(vdev, VRING0_ID);
    if (!ret)
    {
      mailbox_chnl[MAILBOX_CHANNEL_1_IDX].msg_receive_count -= nb_inbox_msg;
    }
  }

  if (mailbox_chnl[MAILBOX_CHANNEL_2_IDX].msg_receive_count)
  {
    /* USER CODE BEGIN MSG_CHANNEL2 */

    /* USER CODE END MSG_CHANNEL2 */

    OPENAMP_log_dbg("Running virt1 (ch_2 new msg)\r\n");
    ret = rproc_virtio_notified(vdev, VRING1_ID);
    if (!ret)
    {
      mailbox_chnl[MAILBOX_CHANNEL_2_IDX].msg_receive_count--;
    }
  }

  /* USER CODE BEGIN POST_MAILBOX_POLL */

  /* USER CODE END  POST_MAILBOX_POLL */

  return ret;
}

/**
 * @brief  Callback function called by OpenAMP MW to notify message processing
 * @param  priv: Pointer to IPCC handle
 * @param  id: VRING id
 * @retval 0 on success, -1 on error
 */
int MAILBOX_Notify(void *priv, uint32_t id)
{
  uint32_t channel;
  IPCC_HandleTypeDef *hipcc = (IPCC_HandleTypeDef *)priv;

  /* USER CODE BEGIN PRE_MAILBOX_NOTIFY */

  /* USER CODE END PRE_MAILBOX_NOTIFY */

  /* Called after virtqueue processing: time to inform the remote */
  if (id == VRING0_ID) {
    channel = IPCC_CHANNEL_1;
    OPENAMP_log_dbg("Send msg on ch_1\r\n");
  }
  else if (id == VRING1_ID) {
    /* Note: the OpenAMP framework never notifies this */
    channel = IPCC_CHANNEL_2;
    OPENAMP_log_dbg("Send 'buff free' on ch_2\r\n");
  }
  else {
    OPENAMP_log_err("invalid vring (%d)\r\n", (int)id);
    return -1;
  }

  /* Check that the channel is free (otherwise wait until it is) */
  if (HAL_IPCC_GetChannelStatus(hipcc, channel, IPCC_CHANNEL_DIR_TX) == IPCC_CHANNEL_STATUS_OCCUPIED) {
    OPENAMP_log_dbg("Waiting for channel to be freed\r\n");
    while (HAL_IPCC_GetChannelStatus(hipcc, channel, IPCC_CHANNEL_DIR_TX) == IPCC_CHANNEL_STATUS_OCCUPIED) {
      /* Wait for channel to be free */
    }
  }

  /* Inform A7 (either new message, or buf free) */
  HAL_IPCC_NotifyCPU(hipcc, channel, IPCC_CHANNEL_DIR_TX);

 /* USER CODE BEGIN POST_MAILBOX_NOTIFY */

 /* USER CODE END  POST_MAILBOX_NOTIFY */

  return 0;
}

/* Private functions  ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */
/* Callback from IPCC Interrupt Handler: Master Processor informs that there are some free buffers */
static void IPCC_channel_callback(IPCC_HandleTypeDef *hipcc, uint32_t ChannelIndex,
                                  IPCC_CHANNELDirTypeDef ChannelDir)
{
  /* USER CODE BEGIN PRE_MAILBOX_CHANNEL1_CALLBACK */

  /* USER CODE END  PRE_MAILBOX_CHANNEL1_CALLBACK */

  mailbox_chnl[ChannelIndex].msg_receive_count++;

  /* Inform A7 that we have received the msg */
  HAL_IPCC_NotifyCPU(hipcc, ChannelIndex, IPCC_CHANNEL_DIR_RX);

  /* USER CODE BEGIN POST_MAILBOX_CHANNEL1_CALLBACK */

  /* USER CODE END  POST_MAILBOX_CHANNEL1_CALLBACK */
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
