/**
  ******************************************************************************
  * @file    LwIP/LwIP_UDP_Echo_Server/Src/udp_echoserver.c
  * @author  MCD Application Team
  * @brief   UDP echo server
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "lwip/tcp.h"
#include <string.h>
#include <stdio.h>
#include "udp_echoserver.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void udp_echoserver_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initialize the server application.
  * @param  None
  * @retval None
  */
void udp_echoserver_init(void)
{
   struct udp_pcb *upcb;
   err_t err;

   /* Create a new UDP control block  */
   upcb = udp_new();

   if (upcb)
   {
     /* Bind the upcb to the UDP_PORT port */
     /* Using IP_ADDR_ANY allow the upcb to be used by any local interface */
      err = udp_bind(upcb, IP_ADDR_ANY, UDP_SERVER_PORT);

      if(err == ERR_OK)
      {
        /* Set a receive callback for the upcb */
        udp_recv(upcb, udp_echoserver_receive_callback, NULL);
      }
      else
      {
        udp_remove(upcb);
      }
   }
}

/**
  * @brief This function is called when an UDP datagrm has been received on the port UDP_PORT.
  * @param arg user supplied argument (udp_pcb.recv_arg)
  * @param pcb the udp_pcb which received data
  * @param p the packet buffer that was received
  * @param addr the remote IP address from which the packet was received
  * @param port the remote port from which the packet was received
  * @retval None
  */
void udp_echoserver_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
  struct pbuf *p_tx;

  /* allocate pbuf from RAM*/
  p_tx = pbuf_alloc(PBUF_TRANSPORT,p->len, PBUF_RAM);

  if(p_tx != NULL)
  {
    pbuf_take(p_tx, (char*)p->payload, p->len);

    printf("UDP received (%u bytes): %.*s\n\r",
           (unsigned)p->len, (int)p->len, (char *)p->payload);
    /* Connect to the remote client */
    udp_connect(upcb, addr, port); // UDP_CLIENT_PORT);

    /* Tell the client that we have accepted it */
    udp_send(upcb, p_tx);

    /* free the UDP connection, so we can accept new clients */
    udp_disconnect(upcb);

    /* Free the p_tx buffer */
    pbuf_free(p_tx);

    /* Free the p buffer */
    pbuf_free(p);
  }
}

#if LOOPBACK_TEST_ENABLE
static struct udp_pcb *loopback_client_pcb;
static uint32_t loopback_last_tick;
static uint32_t loopback_tx_seq;
static uint32_t loopback_pass_count;
static uint32_t loopback_fail_count;
static char loopback_tx_buffer[64];

static void loopback_client_init(void);
static void loopback_test_send(struct netif *netif);
static void loopback_client_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port);

static void loopback_client_init(void)
{
  loopback_client_pcb = udp_new();

  if (loopback_client_pcb != NULL)
  {
    if (udp_bind(loopback_client_pcb, IP_ADDR_ANY, 0) == ERR_OK)
    {
      udp_recv(loopback_client_pcb, loopback_client_receive_callback, NULL);
      printf("Loopback test: client ready, local port %d\r\n", loopback_client_pcb->local_port);
    }
    else
    {
      printf("Loopback test: failed to bind the client UDP pcb\r\n");
      udp_remove(loopback_client_pcb);
      loopback_client_pcb = NULL;
    }
  }
  else
  {
    printf("Loopback test: failed to allocate the client UDP pcb\r\n");
  }
}

static void loopback_test_send(struct netif *netif)
{
  struct pbuf *p;
  int len;

  len = snprintf(loopback_tx_buffer, sizeof(loopback_tx_buffer), "%s #%lu", LOOPBACK_MSG, (unsigned long)loopback_tx_seq);
  p = pbuf_alloc(PBUF_TRANSPORT, (u16_t)len, PBUF_RAM);

  if (p != NULL)
  {
    pbuf_take(p, loopback_tx_buffer, (u16_t)len);

    if (udp_sendto(loopback_client_pcb, p, netif_ip4_addr(netif), UDP_SERVER_PORT) == ERR_OK)
    {
      printf("Loopback test: sent %s to %s:%d\r\n", loopback_tx_buffer, ip4addr_ntoa(netif_ip4_addr(netif)), UDP_SERVER_PORT);
      loopback_tx_seq++;
    }
    else
    {
      printf("Loopback test: failed to send datagram #%lu\r\n", (unsigned long)loopback_tx_seq);
    }

    pbuf_free(p);
  }
}

static void loopback_client_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
  if ((p->len == strlen(loopback_tx_buffer)) && (memcmp(p->payload, loopback_tx_buffer, p->len) == 0))
  {
    loopback_pass_count++;
    printf("Loopback test: PASSED - echoed data matches (pass=%lu, fail=%lu)\r\n", (unsigned long)loopback_pass_count, (unsigned long)loopback_fail_count);
    BSP_LED_On(LED_GREEN);
  }
  else
  {
    loopback_fail_count++;
    printf("Loopback test: FAILED - echoed data mismatch (pass=%lu, fail=%lu)\r\n", (unsigned long)loopback_pass_count, (unsigned long)loopback_fail_count);
    BSP_LED_On(LED_RED);
  }

  pbuf_free(p);

  if ((loopback_pass_count + loopback_fail_count) >= LOOPBACK_TEST_COUNT)
  {
    if (loopback_fail_count == 0U)
    {
      printf("\r\nLwIP loopback self-test PASSED (%lu/%lu packets)\r\n", (unsigned long)loopback_pass_count, (unsigned long)LOOPBACK_TEST_COUNT);
    }
    else
    {
      printf("\r\nLwIP loopback self-test completed with failures (%lu/%lu packets)\r\n", (unsigned long)loopback_pass_count, (unsigned long)LOOPBACK_TEST_COUNT);
    }
  }
}

void loopback_test_periodic_handle(struct netif *netif)
{
  static uint8_t waiting_msg_shown = 0;

  if (!netif_is_link_up(netif) || ip4_addr_isany(netif_ip4_addr(netif)))
  {
    if (!waiting_msg_shown)
    {
      printf("Loopback test: waiting for the link and an IP address...\r\n");
      waiting_msg_shown = 1;
    }
    return;
  }

  if (loopback_client_pcb == NULL)
  {
    loopback_client_init();
    loopback_last_tick = HAL_GetTick();
    return;
  }

  if (loopback_tx_seq >= LOOPBACK_TEST_COUNT)
  {
    return;
  }

  if ((HAL_GetTick() - loopback_last_tick) >= LOOPBACK_TEST_PERIOD_MS)
  {
    loopback_last_tick = HAL_GetTick();
    loopback_test_send(netif);
  }
}
#endif /* LOOPBACK_TEST_ENABLE */


