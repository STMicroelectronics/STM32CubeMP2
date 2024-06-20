/**
  ******************************************************************************
  * @file    virt_i2c.c
  * @author  MCD Application Team
  * @brief   VIRT I2C HAL module driver.
  *          This file provides firmware functions to manage an rpmsg endpoint
  *          from user application
  *
  *
  @verbatim
 ===============================================================================
                        ##### How to use this driver #####
 ===============================================================================
  [..]
    The VIRTUAL I2C driver can be used as follows:
    (#) Initialize the Virtual I2C by calling the VIRT_I2C_Init() API.
        (++) create an endpoint. listener on the OpenAMP-rpmsg channel is now enabled.
        Receive data  is now possible if user registers a callback to this VIRTUAL I2C instance
        by calling in providing a callback function when a message is received from
        remote processor (VIRT_I2C_read_cb)
        OpenAMP MW deals with memory allocation/free and signal events
    (#) Transmit data on the created rpmsg channel by calling the VIRT_I2C_sendToMaster()
    (#) Receive data in calling VIRT_I2C_RegisterCallback to register user callback


  @endverbatim
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "virt_i2c.h"
#include "metal/utilities.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* this string will be sent to remote processor */
#define RPMSG_SERVICE_NAME              "rpmsg_i2c"

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t VIRT_I2C_addrFromMsg(VIRT_I2C_HandleTypeDef *i2c_dev)
{
  return i2c_dev->msg->addr >> 1;
}

static int VIRT_I2C_read_cb(struct rpmsg_endpoint *ept, void *data,
                            size_t len, uint32_t src, void *priv)
{
  VIRT_I2C_HandleTypeDef *i2c_dev = metal_container_of(ept, VIRT_I2C_HandleTypeDef, ept);
  (void)src;

  memcpy(i2c_dev->msg,data,len);

  if (i2c_dev->RxCpltCallback != NULL) {
    i2c_dev->RxCpltCallback(i2c_dev, i2c_dev->msg->len);
  }

  return 0;
}

VIRT_I2C_StatusTypeDef VIRT_I2C_Init(VIRT_I2C_HandleTypeDef *i2c_dev)
{

  int status;

  i2c_dev->msg = malloc((RPMSG_BUFFER_SIZE-16)*sizeof(uint8_t));

  /* Create a endpoint for rmpsg communication */

  status = OPENAMP_create_endpoint(&i2c_dev->ept, RPMSG_SERVICE_NAME, RPMSG_ADDR_ANY,
                                   VIRT_I2C_read_cb, NULL);

  if(status < 0) {
    return VIRT_I2C_ERROR;
  }

  return VIRT_I2C_OK;
}

VIRT_I2C_StatusTypeDef VIRT_I2C_DeInit (VIRT_I2C_HandleTypeDef *i2c_dev)
{

  free(i2c_dev->msg);

  OPENAMP_destroy_ept(&i2c_dev->ept);

  return VIRT_I2C_OK;
}

VIRT_I2C_StatusTypeDef VIRT_I2C_RegisterCallback(VIRT_I2C_HandleTypeDef *i2c_dev,
                                                   VIRT_I2C_CallbackIDTypeDef CallbackID,
                                                   void (* pCallback)(VIRT_I2C_HandleTypeDef *_i2c_dev, size_t len))
{
  VIRT_I2C_StatusTypeDef status = VIRT_I2C_OK;

  switch (CallbackID)
  {
  case VIRT_I2C_RXCPLT_CB_ID :
    i2c_dev->RxCpltCallback = pCallback;
    break;

  default :
   /* Return error status */
    status =  VIRT_I2C_ERROR;
    break;
  }
  return status;
}

VIRT_I2C_StatusTypeDef VIRT_I2C_sendToMaster(VIRT_I2C_HandleTypeDef *i2c_dev, uint8_t *pData, uint16_t Size)
{
  int res;
  int msg_size = sizeof(rpmsg_i2c_msg) + Size*sizeof(uint8_t);
  if(msg_size > (RPMSG_BUFFER_SIZE-16))
    return VIRT_I2C_ERROR;

  rpmsg_i2c_msg *msg = malloc(msg_size);
  if(!msg) {
    return VIRT_I2C_ERROR;
  }

  msg->addr = i2c_dev->msg->addr;
  msg->len = Size;
  msg->result = VIRT_I2C_ACK;
  memcpy(msg->buf,pData,msg_size);

  res = OPENAMP_send(&i2c_dev->ept, msg, msg_size);

  free(msg);

  return res < 0 ? VIRT_I2C_ERROR : VIRT_I2C_OK;
}

VIRT_I2C_StatusTypeDef VIRT_I2C_sendACK(VIRT_I2C_HandleTypeDef *i2c_dev,
                                        VIRT_I2C_AckTypeDef ack)
{
   i2c_dev->msg->len = 0;
   i2c_dev->msg->result = ack;

   if (OPENAMP_send(&i2c_dev->ept, i2c_dev->msg, sizeof(rpmsg_i2c_msg)) < 0)
     return VIRT_I2C_ERROR;

   return VIRT_I2C_OK;
}
