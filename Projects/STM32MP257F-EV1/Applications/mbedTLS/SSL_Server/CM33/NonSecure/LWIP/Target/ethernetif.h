/**
  ******************************************************************************
  * @file    mbedTLS/SSL_Server/Inc/ethernetif.h
  * @author  MCD Application Team
  * @brief   Header for ethernetif.c module
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#ifndef __ETHERNETIF_H__
#define __ETHERNETIF_H__


#include "lwip/err.h"
#include "lwip/netif.h"
#include "cmsis_os.h"
#include "main.h"

err_t ethernetif_init(struct netif *netif);
void ethernet_link_status_updated(struct netif *netif);
void ethernetif_input(void* argument);
void ethernet_link_thread(void* argument);

/* Exported types ------------------------------------------------------------*/
/* Structure that includes link thread parameters */
/* Exported functions ------------------------------------------------------- */
err_t ethernetif_init(struct netif *netif);
void MX_ETH1_Init(void);

void ethernet_link_thread( void *argument );
#endif /* __ETHERNETIF_H__ */
