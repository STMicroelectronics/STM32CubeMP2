/**
  ******************************************************************************
  * @file    LwIP/LwIP_UDP_Echo_Client/Inc/udp_echoclient.h
  * @author  MCD Application Team
  * @brief   Header file for udp_echoclient.c
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __UDP_ECHOCLIENT_H__
#define __UDP_ECHOCLIENT_H__

/* Includes ------------------------------------------------------------------*/
#include "lwip/netif.h"
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void udp_echoclient_connect(void);
void udp_echoclient_send(void);

#if LOOPBACK_TEST_ENABLE
/* Period, in milliseconds, between two loopback test datagrams */
#define LOOPBACK_TEST_PERIOD_MS   2000U

/* Total number of loopback test datagrams sent before the test completes */
#define LOOPBACK_TEST_COUNT       4U

/* Payload used for the loopback test datagrams */
#define LOOPBACK_MSG              "LwIP Loopback Test"

/* Optional single-board loopback self-test: starts a small local UDP echo
  server (independent of the real remote server targeted by
  udp_echoclient_connect()/DEST_IP_ADDRx) and a client that sends
  datagrams to this board's own IP address, checking that they are
  echoed back correctly. See LOOPBACK_TEST_ENABLE in lwipopts.h. */
void loopback_test_periodic_handle(struct netif *netif);
#endif /* LOOPBACK_TEST_ENABLE */

#endif /* __UDP_ECHOCLIENT_H__ */



