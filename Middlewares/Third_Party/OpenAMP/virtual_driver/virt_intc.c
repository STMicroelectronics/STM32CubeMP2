/**
 ******************************************************************************
 * @file    virt_intc.c
 * @author  MCD Application Team
 * @brief   VIRT INTC HAL module driver.
 *          This file provides firmware functions to manage an rpmsg endpoint
 *          from user application
 *
 *
 @verbatim
 ===============================================================================
##### How to use this driver #####
===============================================================================
[..]
The VIRTUAL INTC driver can be used as follows:
(#) Initialize the Virtual INTC by calling the VIRT_INTC_Init() API.
(++) create an endpoint. listener on the OpenAMP-rpmsg channel is now enabled.
Receive data  is now possible if user registers a callback to this VIRTUAL INTC instance
by calling in providing a callback function when a message is received from
remote processor (VIRT_INTC_read_cb)
OpenAMP MW deals with memory allocation/free and signal events
(#) Transmit data on the created rpmsg channel by calling the VIRT_INTC_sendToMaster()
(#) Receive data in calling VIRT_INTC_RegisterCallback to register user callback


@endverbatim
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
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
#include "virt_intc.h"
#include "metal/utilities.h"
#include "openamp_log.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* this string will be sent to remote processor */
#define RPMSG_SERVICE_NAME "rpmsg-intc"

#define DIV_ROUND_UP(n, d) (((n) + (d) - 1) / (d))
#define BITS_TO_BYTES(nr)  DIV_ROUND_UP(nr, 8)
#define DIV_ROUND_UP(n, d) (((n) + (d) - 1) / (d))
#define BIT(nr)            (1UL << (nr))

#define IRQS_PER_BANK      32
#define NB_ICHIP(nb_irq)   DIV_ROUND_UP(nb_irq, IRQS_PER_BANK)
#define TO_BANK_ID(irq)    (irq / IRQS_PER_BANK)
#define TO_BANK_IRQ(irq)   (irq % IRQS_PER_BANK)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static int VIRT_INTC_read_cb(struct rpmsg_endpoint *ept, void *data,
           size_t len, uint32_t src, void *priv)
{
  VIRT_INTC_HandleTypeDef *intc_dev =
    metal_container_of(ept, VIRT_INTC_HandleTypeDef, ept);
  rpmsg_intc_msg *msg = (rpmsg_intc_msg *) data;

  if (len != sizeof(rpmsg_intc_msg)) {
    log_err("%s: msg sz fail\n\r", __func__);
    return VIRT_INTC_ERROR;
  }

  if (msg->bank_id >= intc_dev->nb_bank) {
    log_err("%s: chip_id fail\n\r", __func__);
    return VIRT_INTC_ERROR;
  }

  memcpy(&intc_dev->msg, msg, len);

  intc_dev->VirtIntcRxDat = VIRT_INTC_SET;

  if (intc_dev->RxCpltCallback != NULL)
    intc_dev->RxCpltCallback(intc_dev, len);
  else
     VIRT_INTC_interpreter(intc_dev);

  return VIRT_INTC_OK;
}

VIRT_INTC_StatusTypeDef VIRT_INTC_interpreter(VIRT_INTC_HandleTypeDef *intc_dev)
{
  rpmsg_intc_msg *msg = &intc_dev->msg;
  uint32_t res;

  if (intc_dev->VirtIntcRxDat != VIRT_INTC_SET)
    return -VIRT_INTC_ERROR;

  intc_dev->VirtIntcRxDat = VIRT_INTC_RESET;

  if (msg->reg != VIRT_INTC_CFG)
    return -VIRT_INTC_ERROR;

  msg->val = intc_dev->nb_bank;

  res = OPENAMP_send(&intc_dev->ept, msg, sizeof(rpmsg_intc_msg));

  return res < 0 ? VIRT_INTC_ERROR : VIRT_INTC_OK;
}

VIRT_INTC_StatusTypeDef VIRT_INTC_Init(VIRT_INTC_HandleTypeDef *intc_dev, uint32_t nb_irqs)
{
  int status;

  if (!nb_irqs)
    return VIRT_INTC_ERROR;

  intc_dev->nb_bank = NB_ICHIP(nb_irqs);
  intc_dev->banks = calloc(intc_dev->nb_bank, sizeof(rpmsg_intc_bank));
  intc_dev->VirtIntcRxDat = VIRT_INTC_RESET;
  intc_dev->RxCpltCallback = NULL;

  /* Create a endpoint for rmpsg communication */
  status = OPENAMP_create_endpoint(&intc_dev->ept, RPMSG_SERVICE_NAME,
                                   RPMSG_ADDR_ANY, VIRT_INTC_read_cb, NULL);

  if(status < 0) {
    return VIRT_INTC_ERROR;
  }

  return VIRT_INTC_OK;
}

VIRT_INTC_StatusTypeDef VIRT_INTC_DeInit(VIRT_INTC_HandleTypeDef *intc_dev)
{
  free(intc_dev->banks);
  intc_dev->nb_bank = 0;
  intc_dev->RxCpltCallback = NULL;
  intc_dev->VirtIntcRxDat = VIRT_INTC_RESET;
  OPENAMP_destroy_ept(&intc_dev->ept);

  return VIRT_INTC_OK;
}

VIRT_INTC_StatusTypeDef VIRT_INTC_RegisterCallback(VIRT_INTC_HandleTypeDef *intc_dev,
               VIRT_INTC_CallbackIDTypeDef CallbackID,
               void (* pCallback)(VIRT_INTC_HandleTypeDef *_intc_dev, size_t len))
{
  VIRT_INTC_StatusTypeDef status = VIRT_INTC_OK;

  switch (CallbackID)
  {
  case VIRT_INTC_RXCPLT_CB_ID :
    intc_dev->RxCpltCallback = pCallback;
    break;

  default :
    /* Return error status */
    status =  VIRT_INTC_ERROR;
    break;
  }
  return status;
}

VIRT_INTC_StatusTypeDef VIRT_INTC_sendIRQ(VIRT_INTC_HandleTypeDef *intc_dev, uint32_t hwirq)
{
  rpmsg_intc_msg msg = { VIRT_INTC_PEND, 0, 0};
  uint32_t bank_id = TO_BANK_ID(hwirq);
  int res;

  if (bank_id >= intc_dev->nb_bank)
    return VIRT_INTC_ERROR;

  msg.bank_id = bank_id;
  msg.val = BIT(TO_BANK_IRQ(hwirq));

  res = OPENAMP_send(&intc_dev->ept, &msg, sizeof(rpmsg_intc_msg));
  return res < 0 ? VIRT_INTC_ERROR : VIRT_INTC_OK;
}
