/**
  ******************************************************************************
  * @file    mbox_scmi.c
  * @author  MCD Application Team
  * @brief   This file provides code for the configuration
  *                      of the mailbox_ipcc_if.c MiddleWare.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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

/* Includes ------------------------------------------------------------------*/
#include "mbox_scmi.h"

/* Private define ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
extern IPCC_HandleTypeDef hipcc;
volatile static uint8_t   scmi_req_done = 0;

/* Private function prototypes -----------------------------------------------*/
static void MBOX_SCMI_IPCC_channel_callback( IPCC_HandleTypeDef    *hipcc        ,
                                             uint32_t               ChannelIndex ,
                                             IPCC_CHANNELDirTypeDef ChannelDir );

/**
  * @brief  Initialize MAILBOX with IPCC peripheral
  * @param  None
  * @retval : Operation result
  */
int MAILBOX_SCMI_Init(void)
{

  if (HAL_IPCC_ActivateNotification(&hipcc, MBOX_SCMI_IPCC_CHANNEL, IPCC_CHANNEL_DIR_RX,
                                    MBOX_SCMI_IPCC_channel_callback) != HAL_OK)
  {
    loc_printf("MBOX_SCMI_IPCC_CHANNEL-%d RX ActivateNotification fail\n\r", MBOX_SCMI_IPCC_CHANNEL);
    return -1;
  }

  if (HAL_IPCC_ActivateNotification(&hipcc, MBOX_SCMI_IPCC_CHANNEL, IPCC_CHANNEL_DIR_TX,
                                    MBOX_SCMI_IPCC_channel_callback) != HAL_OK)
  {
    loc_printf("MBOX_SCMI_IPCC_CHANNEL-%d TX ActivateNotification fail\n\r", MBOX_SCMI_IPCC_CHANNEL);
    return -1;
  }

  return 0;
}

/**
  * @brief  DeInitialize MAILBOX with IPCC peripheral
  * @param  None
  * @retval : None
  */
void MAILBOX_SCMI_DeInit(void)
{
  if (HAL_IPCC_DeActivateNotification(&hipcc, MBOX_SCMI_IPCC_CHANNEL, IPCC_CHANNEL_DIR_RX) != HAL_OK)
  {
    loc_printf("MBOX_SCMI_IPCC_CHANNEL-%d RX DeActivateNotification fail\n\r", MBOX_SCMI_IPCC_CHANNEL);
  }

  if (HAL_IPCC_DeActivateNotification(&hipcc, MBOX_SCMI_IPCC_CHANNEL, IPCC_CHANNEL_DIR_TX) != HAL_OK)
  {
    loc_printf("MBOX_SCMI_IPCC_CHANNEL-%d TX DeActivateNotification fail\n\r", MBOX_SCMI_IPCC_CHANNEL);
  }
}

/**
  * @brief  Function call to send a request to A35
  *
  * The function blocks till the request is sent and then waits till A35 response is
  * received
  * @param  None
  * @retval Operation result
  */
int MAILBOX_SCMI_Req(void)
{
  uint32_t time = HAL_GetTick();

  /* Check that the channel is free (otherwise wait until it is) */
  while (HAL_IPCC_GetChannelStatus(&hipcc, MBOX_SCMI_IPCC_CHANNEL, IPCC_CHANNEL_DIR_TX)
         != IPCC_CHANNEL_STATUS_FREE)
  {
    if ((HAL_GetTick() - time) >= MBOX_SCMI_IPCC_TIMEOUT_CHAN_FREE_MS)
    {
      return -1;
    }

    HAL_Delay(10);
  }

  scmi_req_done = 0;
  HAL_IPCC_NotifyCPU(&hipcc, MBOX_SCMI_IPCC_CHANNEL, IPCC_CHANNEL_DIR_TX);

  /* Wait for response of above notification */
  time = HAL_GetTick();
  while (scmi_req_done == 0)
  {
    if ((HAL_GetTick() - time) >= MBOX_SCMI_IPCC_TIMEOUT_CALLBACK_MS)
    {
      return -1;
    }
  }

  /* Again Inform that acknowledgment has been received */
  do
  {
    HAL_IPCC_NotifyCPU(&hipcc, MBOX_SCMI_IPCC_CHANNEL, IPCC_CHANNEL_DIR_RX);
  }
  while (HAL_IPCC_GetChannelStatus(&hipcc, MBOX_SCMI_IPCC_CHANNEL, IPCC_CHANNEL_DIR_RX) == IPCC_CHANNEL_STATUS_OCCUPIED);

  return 0;
}

/* Private function  ---------------------------------------------------------*/

/* Callback from IPCC Interrupt Handler: Master Processor That request has been Done */
static void MBOX_SCMI_IPCC_channel_callback( IPCC_HandleTypeDef    *hipcc        ,
                                             uint32_t               ChannelIndex ,
                                             IPCC_CHANNELDirTypeDef ChannelDir )
{
  if ((ChannelDir == IPCC_CHANNEL_DIR_RX) && (ChannelIndex == MBOX_SCMI_IPCC_CHANNEL))
  {
    scmi_req_done = 1;
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
