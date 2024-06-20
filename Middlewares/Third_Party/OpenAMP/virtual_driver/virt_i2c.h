/**
  ******************************************************************************
  * @file    virt_i2c.h
  * @author  MCD Application Team
  * @brief   Header file of I2C VIRT module.
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
#ifndef __VIRT_I2C_H
#define __VIRT_I2C_H


#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "openamp.h"


#define I2C_MASTER_READ         0x1
#define VIRT_I2C_HEAD_SIZE      7

/* Exported structures --------------------------------------------------------*/

/*rpmsg_i2c_msg - client specific data*/
typedef struct __VIRT_I2C_rpmsg_i2c_msg
{
  uint32_t len;                      /*!< Data Buffer size                           */
  uint8_t addr;                      /*!< i2c slave addr                             */
  uint8_t result;                    /*!< return value for the master                */
  uint8_t reserved[2];               /*!< header padding for 32-bit alignment        */
  uint8_t buf[0];                    /*!< i2c Data buffer                            */
} __attribute__((__packed__)) rpmsg_i2c_msg;

typedef struct __VIRT_I2C_HandleTypeDef
{
  struct rpmsg_endpoint ept;          /*!< rpmsg endpoint                             */
  struct rpmsg_virtio_device *rvdev;  /*< pointer to the rpmsg virtio device          */
  rpmsg_i2c_msg *msg;                  /*< pointer to the i2c msg                      */
  void    (* RxCpltCallback)( struct __VIRT_I2C_HandleTypeDef * hppp, size_t len);    /*!< RX CPLT callback    */
}VIRT_I2C_HandleTypeDef;


typedef enum
{
  VIRT_I2C_OK       = 0x00U,
  VIRT_I2C_ERROR    = 0x01U,
  VIRT_I2C_BUSY     = 0x02U,
  VIRT_I2C_TIMEOUT  = 0x03U
} VIRT_I2C_StatusTypeDef;

typedef enum
{
  VIRT_I2C_ACK  = 0x01U,
  VIRT_I2C_NACK = 0x02U
} VIRT_I2C_AckTypeDef;

typedef enum
{
  VIRT_I2C_RXCPLT_CB_ID          = 0x00U,    /*!< PPP event 1 callback ID     */
}VIRT_I2C_CallbackIDTypeDef;


/* Exported functions --------------------------------------------------------*/
uint8_t VIRT_I2C_addrFromMsg(VIRT_I2C_HandleTypeDef *i2c_dev);

/* Initialization and de-initialization functions  ****************************/
VIRT_I2C_StatusTypeDef VIRT_I2C_Init(VIRT_I2C_HandleTypeDef *i2c_dev);
VIRT_I2C_StatusTypeDef VIRT_I2C_DeInit (VIRT_I2C_HandleTypeDef *i2c_dev);
VIRT_I2C_StatusTypeDef VIRT_I2C_RegisterCallback(VIRT_I2C_HandleTypeDef *i2c_dev,
                                                   VIRT_I2C_CallbackIDTypeDef CallbackID,
                                                   void (* pCallback)(VIRT_I2C_HandleTypeDef *_i2c_dev,size_t len));

/* IO operation functions *****************************************************/
VIRT_I2C_StatusTypeDef VIRT_I2C_sendToMaster(VIRT_I2C_HandleTypeDef *i2c_dev, uint8_t *pData, uint16_t Size);
VIRT_I2C_StatusTypeDef VIRT_I2C_sendACK(VIRT_I2C_HandleTypeDef *i2c_dev, VIRT_I2C_AckTypeDef ack);

#ifdef __cplusplus
}
#endif

#endif /* __VIRT_I2C_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
