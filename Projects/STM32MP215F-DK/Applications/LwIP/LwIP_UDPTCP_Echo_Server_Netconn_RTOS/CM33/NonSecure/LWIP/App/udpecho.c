/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */
/* Includes ------------------------------------------------------------------*/
#include "udpecho.h"
#include "FreeRTOSConfig.h"
#include "lwip/opt.h"

#if LWIP_NETCONN

#include "lwip/api.h"
#include "lwip/sys.h"
#include <stdio.h>
#if LOOPBACK_TEST_ENABLE
#include "main.h"
#include "lwip/netif.h"
#include "lwip/ip4_addr.h"
#include <string.h>
#include <stdio.h>
#endif /* LOOPBACK_TEST_ENABLE */
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define UDPECHO_THREAD_PRIO  (osPriorityAboveNormal)

#if LOOPBACK_TEST_ENABLE
#define LOOPBACK_THREAD_PRIO     (osPriorityAboveNormal)
#define LOOPBACK_TEST_PERIOD_MS  2000U
#define LOOPBACK_TEST_COUNT      4U
#define LOOPBACK_MSG             "LwIP Loopback Test"
#endif /* LOOPBACK_TEST_ENABLE */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
#if LOOPBACK_TEST_ENABLE
static void loopback_test_thread(void *arg);
#endif /* LOOPBACK_TEST_ENABLE */
/* Private functions ---------------------------------------------------------*/
static void
udpecho_thread(void *arg)
{
  struct netconn *conn;
  struct netbuf *buf, *tx_buf;
  err_t err;
  LWIP_UNUSED_ARG(arg);

  conn = netconn_new(NETCONN_UDP);
  netconn_bind(conn, IP_ADDR_ANY, 7);

  LWIP_ERROR("udpecho: invalid conn", (conn != NULL), return;);

  while (1) {
    err = netconn_recv(conn, &buf);
    if (err == ERR_OK) {

      printf("UDP received (%u bytes): %.*s\r\n",
             (unsigned)buf->p->tot_len, (int)buf->p->tot_len, (char *)buf->p->payload);
      tx_buf = netbuf_new();
      netbuf_alloc(tx_buf, buf->p->tot_len);

      pbuf_take(tx_buf->p, (const void *)buf->p->payload, buf->p->tot_len);

      err = netconn_sendto(conn, tx_buf, (const ip_addr_t *)&(buf->addr), buf->port);
      if(err != ERR_OK) {
        LWIP_DEBUGF(LWIP_DBG_ON, ("netconn_send failed: %d\n", (int)err));
      } else {
        LWIP_DEBUGF(LWIP_DBG_ON, ("got %s\n", buf));
      }
      netbuf_delete(tx_buf);
    }
    netbuf_delete(buf);
  }
}
/*-----------------------------------------------------------------------------------*/
void
udpecho_init(void)
{
  sys_thread_new("udpecho_thread", udpecho_thread, NULL, (configMINIMAL_STACK_SIZE * 20), UDPECHO_THREAD_PRIO);

#if LOOPBACK_TEST_ENABLE
  /* Start the optional single-board netconn loopback test in its own thread. */
  sys_thread_new("loopback_thread", loopback_test_thread, NULL, (configMINIMAL_STACK_SIZE * 20), LOOPBACK_THREAD_PRIO);
#endif /* LOOPBACK_TEST_ENABLE */
}

#if LOOPBACK_TEST_ENABLE
/**
  * @brief  Run the optional single-board UDP netconn loopback self-test.
  * @param  arg not used
  * @retval None
  */
static void loopback_test_thread(void *arg)
{
  struct netconn *conn;
  struct netbuf *tx_buf;
  struct netbuf *rx_buf;
  void *rx_payload;
  u16_t rx_len;
  char tx_msg[64];
  int tx_len;
  uint32_t seq;
  uint32_t pass_count = 0U;
  uint32_t fail_count = 0U;
  err_t err;

  LWIP_UNUSED_ARG(arg);

  while ((netif_default == NULL) || !netif_is_link_up(netif_default) ||
         ip4_addr_isany(netif_ip4_addr(netif_default)))
  {
    sys_msleep(500);
  }

  conn = netconn_new(NETCONN_UDP);
  LWIP_ERROR("loopback test: invalid conn", (conn != NULL), return;);

  (void)netconn_bind(conn, IP_ADDR_ANY, 0);
  (void)netconn_connect(conn, netif_ip4_addr(netif_default), 7);
  netconn_set_recvtimeout(conn, 2000);

  for (seq = 0U; seq < LOOPBACK_TEST_COUNT; seq++)
  {
    tx_len = snprintf(tx_msg, sizeof(tx_msg), "%s #%lu", LOOPBACK_MSG, (unsigned long)seq);

    tx_buf = netbuf_new();
    if (tx_buf == NULL)
    {
      fail_count++;
      printf("Loopback test: failed to allocate transmit buffer\r\n");
      continue;
    }

    (void)netbuf_alloc(tx_buf, (u16_t)tx_len);
    (void)pbuf_take(tx_buf->p, tx_msg, (u16_t)tx_len);
    err = netconn_send(conn, tx_buf);
    netbuf_delete(tx_buf);

    if (err != ERR_OK)
    {
      printf("Loopback test: failed to send datagram #%lu\r\n", (unsigned long)seq);
      fail_count++;
      sys_msleep(LOOPBACK_TEST_PERIOD_MS);
      continue;
    }

    printf("Loopback test: sent %s to %s:7\r\n", tx_msg, ip4addr_ntoa(netif_ip4_addr(netif_default)));
    err = netconn_recv(conn, &rx_buf);

    if (err == ERR_OK)
    {
      netbuf_data(rx_buf, &rx_payload, &rx_len);

      if ((rx_len == (u16_t)tx_len) && (memcmp(rx_payload, tx_msg, rx_len) == 0))
      {
        pass_count++;
        printf("Loopback test: PASSED - echoed data matches (pass=%lu, fail=%lu)\r\n", (unsigned long)pass_count, (unsigned long)fail_count);
        BSP_LED_On(LED_GREEN);
      }
      else
      {
        fail_count++;
        printf("Loopback test: FAILED - echoed data mismatch (pass=%lu, fail=%lu)\r\n", (unsigned long)pass_count, (unsigned long)fail_count);
        BSP_LED_On(LED_RED);
      }

      netbuf_delete(rx_buf);
    }
    else
    {
      fail_count++;
      printf("Loopback test: FAILED - no reply received (pass=%lu, fail=%lu)\r\n", (unsigned long)pass_count, (unsigned long)fail_count);
      BSP_LED_On(LED_RED);
    }

    sys_msleep(LOOPBACK_TEST_PERIOD_MS);
  }

  if (fail_count == 0U)
  {
    printf("\r\nLwIP loopback self-test PASSED (%lu/%lu packets)\r\n", (unsigned long)pass_count, (unsigned long)LOOPBACK_TEST_COUNT);
  }
  else
  {
    printf("\r\nLwIP loopback self-test completed with failures (%lu/%lu packets)\r\n", (unsigned long)pass_count, (unsigned long)LOOPBACK_TEST_COUNT);
  }

  netconn_delete(conn);

  for (;;)
  {
    sys_msleep(60000);
  }
}
#endif /* LOOPBACK_TEST_ENABLE */

#endif /* LWIP_NETCONN */
