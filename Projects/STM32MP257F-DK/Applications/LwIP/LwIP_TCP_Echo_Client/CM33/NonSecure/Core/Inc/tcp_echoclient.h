/**
  ******************************************************************************
  * @file    LwIP/LwIP_TCP_Echo_Client/Inc/tcp_echoclient.h
  * @author  MCD Application Team
  * @brief   Header file for tcp_echoclient.c
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TCP_ECHOCLIENT_H__
#define __TCP_ECHOCLIENT_H__

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void tcp_echoclient_connect(void);

#if LOOPBACK_TEST_ENABLE
/* Period, in milliseconds, between two loopback test connections */
#define LOOPBACK_TEST_PERIOD_MS   2000U

/* Total number of loopback test connections attempted before the test completes */
#define LOOPBACK_TEST_COUNT       4U

/* Payload used for the loopback test messages */
#define LOOPBACK_MSG              "LwIP Loopback Test"

/* Optional single-board loopback self-test: starts a small local TCP echo
   server (independent of the real remote server targeted by
   tcp_echoclient_connect()/DEST_IP_ADDRx) and a client that connects to
   this board's own IP address, checking that the sent message is echoed
   back correctly. See LOOPBACK_TEST_ENABLE in lwipopts.h. */
void loopback_test_periodic_handle(struct netif *netif);
#endif /* LOOPBACK_TEST_ENABLE */

#endif /* __TCP_ECHOCLIENT_H__ */


