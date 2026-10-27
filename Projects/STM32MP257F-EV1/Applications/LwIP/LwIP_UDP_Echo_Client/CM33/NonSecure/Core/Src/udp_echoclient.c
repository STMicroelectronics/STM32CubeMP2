/**
  ******************************************************************************
  * @file    LwIP/LwIP_UDP_Echo_Client/Src/udp_echoclient.c
  * @author  MCD Application Team
  * @brief   UDP echo client
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
#include "udp_echoclient.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#if LOOPBACK_TEST_ENABLE
/* Dedicated UDP port used by the optional loopback self-test, distinct
   from UDP_SERVER_PORT which targets the real remote echo server. */
#define LOOPBACK_UDP_PORT   7
#endif /* LOOPBACK_TEST_ENABLE */
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void udp_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port);

u8_t   data[100];
__IO uint32_t message_count = 0;
struct udp_pcb *upcb;

#if LOOPBACK_TEST_ENABLE
static struct udp_pcb *loopback_server_pcb;
static struct udp_pcb *loopback_client_pcb;
static uint32_t loopback_last_tick;
static uint32_t loopback_tx_seq;
static uint32_t loopback_pass_count;
static uint32_t loopback_fail_count;
static char loopback_tx_buffer[64];

static void loopback_server_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port);
static void loopback_client_init(void);
static void loopback_test_send(struct netif *netif);
static void loopback_client_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port);
#endif /* LOOPBACK_TEST_ENABLE */

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Connect to UDP echo server
  * @param  None
  * @retval None
  */
void udp_echoclient_connect(void)
{
  ip_addr_t DestIPaddr;
  err_t err;

  /* Create a new UDP control block  */
  upcb = udp_new();

  if (upcb!=NULL)
  {
    /*assign destination IP address */
    IP4_ADDR( &DestIPaddr, DEST_IP_ADDR0, DEST_IP_ADDR1, DEST_IP_ADDR2, DEST_IP_ADDR3 );

    /* configure destination IP address and port */
    err= udp_connect(upcb, &DestIPaddr, UDP_SERVER_PORT);

    if (err == ERR_OK)
    {
      /* Set a receive callback for the upcb */
      udp_recv(upcb, udp_receive_callback, NULL);
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
void udp_echoclient_send(void)
{
  struct pbuf *p;

  sprintf((char*)data, "sending udp client message %d", (int)message_count);

  /* allocate pbuf from pool*/
  p = pbuf_alloc(PBUF_TRANSPORT,strlen((char*)data), PBUF_RAM);

  if (p != NULL)
  {
    /* copy data to pbuf */
    pbuf_take(p, (char*)data, strlen((char*)data));

    /* send udp data */
    udp_send(upcb, p);

    /* free pbuf */
    pbuf_free(p);
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
void udp_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
  /*increment message count */
  message_count++;

  /* Free receive pbuf */
  pbuf_free(p);
}



#if LOOPBACK_TEST_ENABLE
/**
  * @brief  Receive callback for the small local echo server used by the
  *         loopback self-test: simply sends back whatever it receives.
  * @param  arg user supplied argument (udp_pcb.recv_arg)
  * @param  upcb the udp_pcb which received data
  * @param  p the packet buffer that was received
  * @param  addr the remote IP address from which the packet was received
  * @param  port the remote port from which the packet was received
  * @retval None
  */
static void loopback_server_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
  struct pbuf *p_tx;

  p_tx = pbuf_alloc(PBUF_TRANSPORT, p->len, PBUF_RAM);

  if (p_tx != NULL)
  {
    pbuf_take(p_tx, (char*)p->payload, p->len);
    udp_connect(upcb, addr, port);
    udp_send(upcb, p_tx);
    udp_disconnect(upcb);
    pbuf_free(p_tx);
  }

  pbuf_free(p);
}

/**
  * @brief  Create the local echo server and the UDP control block used by
  *         the loopback test to send the self-addressed datagrams and
  *         receive the echoed reply.
  * @param  None
  * @retval None
  */
static void loopback_client_init(void)
{
  loopback_server_pcb = udp_new();

  if (loopback_server_pcb != NULL)
  {
    if (udp_bind(loopback_server_pcb, IP_ADDR_ANY, LOOPBACK_UDP_PORT) == ERR_OK)
    {
      udp_recv(loopback_server_pcb, loopback_server_receive_callback, NULL);
    }
    else
    {
      printf("Loopback test: failed to bind the local echo server\r\n");
      udp_remove(loopback_server_pcb);
      loopback_server_pcb = NULL;
      return;
    }
  }
  else
  {
    printf("Loopback test: failed to allocate the local echo server\r\n");
    return;
  }

  loopback_client_pcb = udp_new();

  if (loopback_client_pcb != NULL)
  {
    /* Bind to an ephemeral local port (port 0): using LOOPBACK_UDP_PORT here
       would clash with the local echo server bound above on IP_ADDR_ANY. */
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

/**
  * @brief  Build and send a loopback test datagram addressed to the board's
  *         own IP address. Since the destination matches the local
  *         interface address, lwIP loops the packet back internally
  *         (LWIP_NETIF_LOOPBACK) instead of transmitting it on the wire, so
  *         the whole UDP/IP stack is exercised without any external device.
  * @param  netif the lwIP network interface structure for this ethernetif
  * @retval None
  */
static void loopback_test_send(struct netif *netif)
{
  struct pbuf *p;
  int len;

  len = snprintf(loopback_tx_buffer, sizeof(loopback_tx_buffer), "%s #%lu", LOOPBACK_MSG, (unsigned long)loopback_tx_seq);

  p = pbuf_alloc(PBUF_TRANSPORT, (u16_t)len, PBUF_RAM);

  if (p != NULL)
  {
    pbuf_take(p, loopback_tx_buffer, (u16_t)len);

    if (udp_sendto(loopback_client_pcb, p, netif_ip4_addr(netif), LOOPBACK_UDP_PORT) == ERR_OK)
    {
      printf("Loopback test: sent %s to %s:%d\r\n", loopback_tx_buffer, ip4addr_ntoa(netif_ip4_addr(netif)), LOOPBACK_UDP_PORT);
      loopback_tx_seq++;
    }
    else
    {
      printf("Loopback test: failed to send datagram #%lu\r\n", (unsigned long)loopback_tx_seq);
    }

    pbuf_free(p);
  }
}

/**
  * @brief  Receive callback for the loopback client. Compares the payload
  *         echoed back by the local server against the last transmitted
  *         message and reports the result of the loopback test.
  * @param  arg user supplied argument (udp_pcb.recv_arg)
  * @param  upcb the udp_pcb which received data
  * @param  p the packet buffer that was received
  * @param  addr the remote IP address from which the packet was received
  * @param  port the remote port from which the packet was received
  * @retval None
  */
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
      printf("\r\nLwIP loopback self-test PASSED (%lu/%lu packets)\r\n",
             (unsigned long)loopback_pass_count, (unsigned long)LOOPBACK_TEST_COUNT);
    }
    else
    {
      printf("\r\nLwIP loopback self-test completed with failures (%lu/%lu packets)\r\n",
             (unsigned long)loopback_pass_count, (unsigned long)LOOPBACK_TEST_COUNT);
    }
  }
}

/**
  * @brief  Periodic loopback test handler, to be called from the
  *         application main loop when LOOPBACK_TEST_ENABLE is set to 1. As
  *         soon as the network interface has a valid IP address and the
  *         link is up, it periodically sends a self-addressed UDP
  *         datagram to exercise the full LwIP stack on this single board,
  *         without requiring a second board or PC.
  * @param  netif the lwIP network interface structure for this ethernetif
  * @retval None
  */
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
    /* All test datagrams have been sent: nothing left to do */
    return;
  }

  if ((HAL_GetTick() - loopback_last_tick) >= LOOPBACK_TEST_PERIOD_MS)
  {
    loopback_last_tick = HAL_GetTick();
    loopback_test_send(netif);
  }
}
#endif /* LOOPBACK_TEST_ENABLE */
