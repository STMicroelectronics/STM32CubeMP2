/**
  ******************************************************************************
  * @file    virt_intc.h
  * @author  MCD Application Team
  * @brief   Header file of INTC VIRT module.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __VIRT_INTC_H
#define __VIRT_INTC_H


#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "openamp.h"

/* Exported structures --------------------------------------------------------*/

/*rpmsg_intc_msg - client specific data*/
typedef struct __VIRT_INTC_rpmsg_intc_msg
{
  uint8_t reg;		/*!< register (mask, pending...) */
  uint32_t val;		/*!< value of register */
  uint32_t bank_id;	/*!< bank id, each bank manage 32 irq lines */
}rpmsg_intc_msg;

typedef struct __VIRT_INTC_rpmsg_intc_bank
{
  uint32_t pend;
  uint32_t mask;	/* 0: disable; 1: activated */
}rpmsg_intc_bank;

typedef enum
{
  VIRT_INTC_RESET    = 0x00U,
  VIRT_INTC_SET      = !VIRT_INTC_RESET,
} VIRT_INTC_FlagStatusTypeDef;

typedef struct __VIRT_INTC_HandleTypeDef
{
  struct rpmsg_endpoint ept;		/*!< rpmsg endpoint                             */
  struct rpmsg_virtio_device *rvdev;	/*< pointer to the rpmsg virtio device          */
  rpmsg_intc_msg msg;                  /*< pointer to the intc msg                     */

  rpmsg_intc_bank *banks;
  uint32_t nb_bank;

  volatile VIRT_INTC_FlagStatusTypeDef VirtIntcRxDat;

  void    (* RxCpltCallback)( struct __VIRT_INTC_HandleTypeDef * hppp, size_t len);    /*!< RX CPLT callback    */
}VIRT_INTC_HandleTypeDef;


typedef enum
{
  VIRT_INTC_OK       = 0x00U,
  VIRT_INTC_ERROR    = 0x01U,
  VIRT_INTC_BUSY     = 0x02U,
  VIRT_INTC_TIMEOUT  = 0x03U
} VIRT_INTC_StatusTypeDef;

typedef enum
{
  VIRT_INTC_PEND = 0x01U, /* sent interrupt to cortexA */
  VIRT_INTC_CFG = 0x02U,  /* get the virt intc configuration (number of interrupt lines managed by virt intc) */
} VIRT_INTC_RegTypeDef;

typedef enum
{
  VIRT_INTC_RXCPLT_CB_ID          = 0x00U,    /*!< PPP event 1 callback ID     */
}VIRT_INTC_CallbackIDTypeDef;

/* Exported functions --------------------------------------------------------*/

/* Initialization and de-initialization functions  ****************************/
VIRT_INTC_StatusTypeDef VIRT_INTC_Init(VIRT_INTC_HandleTypeDef *intc_dev, uint32_t nb_irqs);
VIRT_INTC_StatusTypeDef VIRT_INTC_DeInit(VIRT_INTC_HandleTypeDef *intc_dev);
VIRT_INTC_StatusTypeDef VIRT_INTC_RegisterCallback(VIRT_INTC_HandleTypeDef *intc_dev,
                                                   VIRT_INTC_CallbackIDTypeDef CallbackID,
                                                   void (* pCallback)(VIRT_INTC_HandleTypeDef *_intc_dev,size_t len));

/* IO operation functions *****************************************************/
VIRT_INTC_StatusTypeDef VIRT_INTC_sendIRQ(VIRT_INTC_HandleTypeDef *intc_dev, uint32_t hwirq);
VIRT_INTC_StatusTypeDef VIRT_INTC_interpreter(VIRT_INTC_HandleTypeDef *intc_dev);

#ifdef __cplusplus
}
#endif

#endif /* __VIRT_INTC_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
