/**
  ******************************************************************************
  * @file    LwIP/LwIP_TCP_Echo_Client/Src/tcp_echoclient.c
  * @author  MCD Application Team
  * @brief   tcp echoclient application using LwIP RAW API
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
#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/tcp.h"
#include "lwip/memp.h"
#include <stdio.h>
#include <string.h>
#include "tcp_echoclient.h"

#if LWIP_TCP
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
u8_t  recev_buf[50];
__IO uint32_t message_count=0;

u8_t   data[100];

struct tcp_pcb *echoclient_pcb;

/* ECHO protocol states */
enum echoclient_states
{
  ES_NOT_CONNECTED = 0,
  ES_CONNECTED,
  ES_RECEIVED,
  ES_CLOSING,
};

/* structure to be passed as argument to the tcp callbacks */
struct echoclient
{
  enum echoclient_states state; /* connection status */
  struct tcp_pcb *pcb;          /* pointer on the current tcp_pcb */
  struct pbuf *p_tx;            /* pointer on pbuf to be transmitted */
};

/* Private function prototypes -----------------------------------------------*/
static err_t tcp_echoclient_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
static void tcp_echoclient_connection_close(struct tcp_pcb *tpcb, struct echoclient * es);
static err_t tcp_echoclient_poll(void *arg, struct tcp_pcb *tpcb);
static err_t tcp_echoclient_sent(void *arg, struct tcp_pcb *tpcb, u16_t len);
static void tcp_echoclient_send(struct tcp_pcb *tpcb, struct echoclient * es);
static err_t tcp_echoclient_connected(void *arg, struct tcp_pcb *tpcb, err_t err);

#if LOOPBACK_TEST_ENABLE
/* Dedicated TCP port for the small local echo server used by the
   loopback self-test, distinct from DEST_PORT which targets the real
   remote echo server. */
#define LOOPBACK_TCP_PORT   7

/* Loopback test client states */
enum loopback_test_states
{
  LOOPBACK_IDLE = 0,
  LOOPBACK_CONNECTING,
  LOOPBACK_WAIT_ECHO
};

static struct tcp_pcb *loopback_server_pcb;
static enum loopback_test_states loopback_state = LOOPBACK_IDLE;
static struct tcp_pcb *loopback_pcb;
static uint32_t loopback_last_tick;
static uint32_t loopback_tx_seq;
static uint32_t loopback_pass_count;
static uint32_t loopback_fail_count;
static char loopback_tx_buffer[64];
static char loopback_rx_buffer[64];
static uint16_t loopback_tx_len;
static uint16_t loopback_rx_len;

static err_t loopback_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
static err_t loopback_server_accept(void *arg, struct tcp_pcb *newpcb, err_t err);
static void loopback_server_init(void);
static err_t loopback_test_connected(void *arg, struct tcp_pcb *tpcb, err_t err);
static err_t loopback_test_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
static void loopback_test_error(void *arg, err_t err);
static void loopback_test_report(uint8_t success);
static void loopback_test_reset(void);
#endif /* LOOPBACK_TEST_ENABLE */

/* Private functions ---------------------------------------------------------*/


/**
  * @brief  Connects to the TCP echo server
  * @param  None
  * @retval None
  */
void tcp_echoclient_connect(void)
{
  ip_addr_t DestIPaddr;

  /* create new tcp pcb */
  echoclient_pcb = tcp_new();

  if (echoclient_pcb != NULL)
  {
    IP4_ADDR( &DestIPaddr, DEST_IP_ADDR0, DEST_IP_ADDR1, DEST_IP_ADDR2, DEST_IP_ADDR3 );

    /* connect to destination address/port */
    tcp_connect(echoclient_pcb,&DestIPaddr,DEST_PORT,tcp_echoclient_connected);
  }
  else
  {
    /* deallocate the pcb */
    memp_free(MEMP_TCP_PCB, echoclient_pcb);
#ifdef SERIAL_DEBUG
    printf("\n\r can not create tcp pcb");
#endif
  }
}

/**
  * @brief Function called when TCP connection established
  * @param tpcb: pointer on the connection control block
  * @param err: when connection correctly established err should be ERR_OK
  * @retval err_t: returned error
  */
static err_t tcp_echoclient_connected(void *arg, struct tcp_pcb *tpcb, err_t err)
{
  struct echoclient *es = NULL;

  if (err == ERR_OK)
  {
    /* allocate structure es to maintain tcp connection information */
    es = (struct echoclient *)mem_malloc(sizeof(struct echoclient));

    if (es != NULL)
    {
      es->state = ES_CONNECTED;
      es->pcb = tpcb;

      sprintf((char*)data, "sending tcp client message %d", (int)message_count);

      /* allocate pbuf */
      es->p_tx = pbuf_alloc(PBUF_TRANSPORT, strlen((char*)data) , PBUF_POOL);

      if (es->p_tx)
      {
        /* copy data to pbuf */
        pbuf_take(es->p_tx, (char*)data, strlen((char*)data));

        /* pass newly allocated es structure as argument to tpcb */
        tcp_arg(tpcb, es);

        /* initialize LwIP tcp_recv callback function */
        tcp_recv(tpcb, tcp_echoclient_recv);

        /* initialize LwIP tcp_sent callback function */
        tcp_sent(tpcb, tcp_echoclient_sent);

        /* initialize LwIP tcp_poll callback function */
        tcp_poll(tpcb, tcp_echoclient_poll, 1);

        /* send data */
        tcp_echoclient_send(tpcb,es);

        return ERR_OK;
      }
    }
    else
    {
      /* close connection */
      tcp_echoclient_connection_close(tpcb, es);

      /* return memory allocation error */
      return ERR_MEM;
    }
  }
  else
  {
    /* close connection */
    tcp_echoclient_connection_close(tpcb, es);
  }
  return err;
}

/**
  * @brief tcp_receiv callback
  * @param arg: argument to be passed to receive callback
  * @param tpcb: tcp connection control block
  * @param err: receive error code
  * @retval err_t: returned error
  */
static err_t tcp_echoclient_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
  struct echoclient *es;
  err_t ret_err;

  LWIP_ASSERT("arg != NULL",arg != NULL);

  es = (struct echoclient *)arg;

  /* if we receive an empty tcp frame from server => close connection */
  if (p == NULL)
  {
    /* remote host closed connection */
    es->state = ES_CLOSING;
    if(es->p_tx == NULL)
    {
       /* we're done sending, close connection */
       tcp_echoclient_connection_close(tpcb, es);
    }
    else
    {
      /* send remaining data*/
      tcp_echoclient_send(tpcb, es);
    }
    ret_err = ERR_OK;
  }
  /* else : a non empty frame was received from echo server but for some reason err != ERR_OK */
  else if(err != ERR_OK)
  {
    /* free received pbuf*/
    if (p != NULL)
    {
      pbuf_free(p);
    }
    ret_err = err;
  }
  else if(es->state == ES_CONNECTED)
  {
    /* increment message count */
    message_count++;

    /* Acknowledge data reception */
    tcp_recved(tpcb, p->tot_len);

    pbuf_free(p);
    tcp_echoclient_connection_close(tpcb, es);
    ret_err = ERR_OK;
  }

  /* data received when connection already closed */
  else
  {
    /* Acknowledge data reception */
    tcp_recved(tpcb, p->tot_len);

    /* free pbuf and do nothing */
    pbuf_free(p);
    ret_err = ERR_OK;
  }
  return ret_err;
}

/**
  * @brief function used to send data
  * @param  tpcb: tcp control block
  * @param  es: pointer on structure of type echoclient containing info on data
  *             to be sent
  * @retval None
  */
static void tcp_echoclient_send(struct tcp_pcb *tpcb, struct echoclient * es)
{
  struct pbuf *ptr;
  err_t wr_err = ERR_OK;

  while ((wr_err == ERR_OK) &&
         (es->p_tx != NULL) &&
         (es->p_tx->len <= tcp_sndbuf(tpcb)))
  {

    /* get pointer on pbuf from es structure */
    ptr = es->p_tx;

    /* enqueue data for transmission */
    wr_err = tcp_write(tpcb, ptr->payload, ptr->len, 1);

    if (wr_err == ERR_OK)
    {
      /* continue with next pbuf in chain (if any) */
      es->p_tx = ptr->next;

      if(es->p_tx != NULL)
      {
        /* increment reference count for es->p */
        pbuf_ref(es->p_tx);
      }

      /* free pbuf: will free pbufs up to es->p (because es->p has a reference count > 0) */
      pbuf_free(ptr);
   }
   else if(wr_err == ERR_MEM)
   {
      /* we are low on memory, try later, defer to poll */
     es->p_tx = ptr;
   }
   else
   {
     /* other problem ?? */
   }
  }
}

/**
  * @brief  This function implements the tcp_poll callback function
  * @param  arg: pointer on argument passed to callback
  * @param  tpcb: tcp connection control block
  * @retval err_t: error code
  */
static err_t tcp_echoclient_poll(void *arg, struct tcp_pcb *tpcb)
{
  err_t ret_err;
  struct echoclient *es;

  es = (struct echoclient*)arg;
  if (es != NULL)
  {
    if (es->p_tx != NULL)
    {
      /* there is a remaining pbuf (chain) , try to send data */
      tcp_echoclient_send(tpcb, es);
    }
    else
    {
      /* no remaining pbuf (chain)  */
      if(es->state == ES_CLOSING)
      {
        /* close tcp connection */
        tcp_echoclient_connection_close(tpcb, es);
      }
    }
    ret_err = ERR_OK;
  }
  else
  {
    /* nothing to be done */
    tcp_abort(tpcb);
    ret_err = ERR_ABRT;
  }
  return ret_err;
}

/**
  * @brief  This function implements the tcp_sent LwIP callback (called when ACK
  *         is received from remote host for sent data)
  * @param  arg: pointer on argument passed to callback
  * @param  tcp_pcb: tcp connection control block
  * @param  len: length of data sent
  * @retval err_t: returned error code
  */
static err_t tcp_echoclient_sent(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
  struct echoclient *es;

  LWIP_UNUSED_ARG(len);

  es = (struct echoclient *)arg;

  if(es->p_tx != NULL)
  {
    /* still got pbufs to send */
    tcp_echoclient_send(tpcb, es);
  }

  return ERR_OK;
}

/**
  * @brief This function is used to close the tcp connection with server
  * @param tpcb: tcp connection control block
  * @param es: pointer on echoclient structure
  * @retval None
  */
static void tcp_echoclient_connection_close(struct tcp_pcb *tpcb, struct echoclient * es )
{
  /* remove callbacks */
  tcp_recv(tpcb, NULL);
  tcp_sent(tpcb, NULL);
  tcp_poll(tpcb, NULL,0);

  if (es != NULL)
  {
    mem_free(es);
  }

  /* close tcp connection */
  tcp_close(tpcb);
}


#if LOOPBACK_TEST_ENABLE
/* ---------------------------------------------------------------------
 * Small local TCP echo server used by the loopback self-test: accepts a
 * single connection and echoes back whatever it receives, so the board
 * can test the LwIP stack against itself without a remote server.
 * --------------------------------------------------------------------- */

/**
  * @brief  tcp_recv callback for the local loopback echo server: echoes
  *         back whatever data it receives.
  * @param  arg not used
  * @param  tpcb the tcp_pcb for the accepted connection
  * @param  p the packet buffer that was received, or NULL if the remote
  *         host closed the connection
  * @param  err not used
  * @retval err_t: error code
  */
static err_t loopback_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
  LWIP_UNUSED_ARG(arg);
  LWIP_UNUSED_ARG(err);

  if (p == NULL)
  {
    tcp_close(tpcb);
    return ERR_OK;
  }

  tcp_recved(tpcb, p->tot_len);

  if (tcp_write(tpcb, p->payload, p->len, TCP_WRITE_FLAG_COPY) == ERR_OK)
  {
    tcp_output(tpcb);
  }

  pbuf_free(p);
  return ERR_OK;
}

/**
  * @brief  tcp_accept callback for the local loopback echo server.
  * @param  arg not used
  * @param  newpcb the tcp_pcb for the newly accepted connection
  * @param  err not used
  * @retval err_t: error code
  */
static err_t loopback_server_accept(void *arg, struct tcp_pcb *newpcb, err_t err)
{
  LWIP_UNUSED_ARG(arg);
  LWIP_UNUSED_ARG(err);

  tcp_setprio(newpcb, TCP_PRIO_MIN);
  tcp_recv(newpcb, loopback_server_recv);

  return ERR_OK;
}

/**
  * @brief  Initialize the small local TCP echo server used by the
  *         loopback self-test, listening on LOOPBACK_TCP_PORT.
  * @param  None
  * @retval None
  */
static void loopback_server_init(void)
{
  loopback_server_pcb = tcp_new();

  if (loopback_server_pcb != NULL)
  {
    if (tcp_bind(loopback_server_pcb, IP_ADDR_ANY, LOOPBACK_TCP_PORT) == ERR_OK)
    {
      loopback_server_pcb = tcp_listen(loopback_server_pcb);
      tcp_accept(loopback_server_pcb, loopback_server_accept);
    }
    else
    {
      memp_free(MEMP_TCP_PCB, loopback_server_pcb);
      loopback_server_pcb = NULL;
      printf("Loopback test: failed to bind the local echo server\r\n");
    }
  }
  else
  {
    printf("Loopback test: failed to allocate the local echo server\r\n");
  }
}

/**
  * @brief  Reset the loopback test client state so a new connection
  *         attempt can be started on the next periodic call.
  * @param  None
  * @retval None
  */
static void loopback_test_reset(void)
{
  loopback_pcb = NULL;
  loopback_state = LOOPBACK_IDLE;
  loopback_last_tick = HAL_GetTick();
  loopback_tx_seq++;
}

/**
  * @brief  Report the outcome of one loopback test round-trip and, once
  *         LOOPBACK_TEST_COUNT round-trips have been attempted, the
  *         overall result of the test.
  * @param  success 1 if the echoed data matched what was sent, 0 otherwise
  * @retval None
  */
static void loopback_test_report(uint8_t success)
{
  if (success)
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

  if ((loopback_pass_count + loopback_fail_count) >= LOOPBACK_TEST_COUNT)
  {
    if (loopback_fail_count == 0U)
    {
      printf("\r\nLwIP loopback self-test PASSED (%lu/%lu connections)\r\n",
             (unsigned long)loopback_pass_count, (unsigned long)LOOPBACK_TEST_COUNT);
    }
    else
    {
      printf("\r\nLwIP loopback self-test completed with failures (%lu/%lu connections)\r\n",
             (unsigned long)loopback_pass_count, (unsigned long)LOOPBACK_TEST_COUNT);
    }
  }
}

/**
  * @brief  tcp_err callback for the loopback test client connection.
  * @param  arg not used
  * @param  err not used
  * @retval None
  */
static void loopback_test_error(void *arg, err_t err)
{
  LWIP_UNUSED_ARG(arg);
  LWIP_UNUSED_ARG(err);

  printf("Loopback test: connection error\r\n");
  loopback_test_report(0);
  loopback_test_reset();
}

/**
  * @brief  tcp_recv callback for the loopback test client: accumulates the
  *         echoed payload and, once as much data as was sent has been
  *         received back, compares it against the transmitted message.
  * @param  arg not used
  * @param  tpcb the tcp_pcb for the loopback test connection
  * @param  p the packet buffer that was received, or NULL if the remote
  *         host closed the connection
  * @param  err not used
  * @retval err_t: error code
  */
static err_t loopback_test_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
  LWIP_UNUSED_ARG(arg);
  LWIP_UNUSED_ARG(err);

  if (p == NULL)
  {
    printf("Loopback test: connection closed by server before echo was complete\r\n");
    tcp_close(tpcb);
    loopback_test_report(0);
    loopback_test_reset();
    return ERR_OK;
  }

  if ((loopback_rx_len + p->tot_len) <= sizeof(loopback_rx_buffer))
  {
    pbuf_copy_partial(p, &loopback_rx_buffer[loopback_rx_len], p->tot_len, 0);
    loopback_rx_len += p->tot_len;
  }

  tcp_recved(tpcb, p->tot_len);
  pbuf_free(p);

  if (loopback_rx_len >= loopback_tx_len)
  {
    uint8_t success = ((loopback_rx_len == loopback_tx_len) &&
                        (memcmp(loopback_rx_buffer, loopback_tx_buffer, loopback_tx_len) == 0));

    tcp_arg(tpcb, NULL);
    tcp_recv(tpcb, NULL);
    tcp_err(tpcb, NULL);
    tcp_close(tpcb);

    loopback_test_report(success);
    loopback_test_reset();
  }

  return ERR_OK;
}

/**
  * @brief  tcp_connect callback for the loopback test client: sends the
  *         test message once the connection to the local TCP echo server
  *         is established.
  * @param  arg not used
  * @param  tpcb the tcp_pcb for the loopback test connection
  * @param  err error status of the connection attempt
  * @retval err_t: error code
  */
static err_t loopback_test_connected(void *arg, struct tcp_pcb *tpcb, err_t err)
{
  LWIP_UNUSED_ARG(arg);

  if (err != ERR_OK)
  {
    printf("Loopback test: failed to connect to the local echo server\r\n");
    loopback_test_report(0);
    loopback_test_reset();
    return err;
  }

  loopback_tx_len = (uint16_t)snprintf(loopback_tx_buffer, sizeof(loopback_tx_buffer), "%s #%lu", LOOPBACK_MSG, (unsigned long)loopback_tx_seq);
  loopback_rx_len = 0;

  tcp_recv(tpcb, loopback_test_recv);

  if ((tcp_write(tpcb, loopback_tx_buffer, loopback_tx_len, TCP_WRITE_FLAG_COPY) != ERR_OK) ||
      (tcp_output(tpcb) != ERR_OK))
  {
    printf("Loopback test: failed to send the test message\r\n");
    tcp_close(tpcb);
    loopback_test_report(0);
    loopback_test_reset();
    return ERR_OK;
  }

  printf("Loopback test: sent \"%s\" to the local echo server\r\n", loopback_tx_buffer);
  loopback_state = LOOPBACK_WAIT_ECHO;

  return ERR_OK;
}

/**
  * @brief  Periodic loopback test handler, to be called from the
  *         application main loop when LOOPBACK_TEST_ENABLE is set to 1. As
  *         soon as the network interface has a valid IP address and the
  *         link is up, it starts the local echo server (once) and then
  *         periodically connects to it using this board's own IP address
  *         to exercise the full LwIP stack without requiring a second
  *         board or PC.
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

  if (loopback_server_pcb == NULL)
  {
    loopback_server_init();
    loopback_last_tick = HAL_GetTick();
    return;
  }

  if (loopback_tx_seq >= LOOPBACK_TEST_COUNT)
  {
    /* All test connections have been attempted: nothing left to do */
    return;
  }

  if ((loopback_state == LOOPBACK_IDLE) &&
      ((HAL_GetTick() - loopback_last_tick) >= LOOPBACK_TEST_PERIOD_MS))
  {
    loopback_pcb = tcp_new();

    if (loopback_pcb == NULL)
    {
      printf("Loopback test: failed to allocate the client tcp_pcb\r\n");
      loopback_test_reset();
      return;
    }

    tcp_err(loopback_pcb, loopback_test_error);
    loopback_state = LOOPBACK_CONNECTING;

    if (tcp_connect(loopback_pcb, netif_ip4_addr(netif), LOOPBACK_TCP_PORT, loopback_test_connected) != ERR_OK)
    {
      printf("Loopback test: failed to start the connection to the local echo server\r\n");
      loopback_test_report(0);
      loopback_test_reset();
    }
  }
}
#endif /* LOOPBACK_TEST_ENABLE */

#endif /* LWIP_TCP */


