/**
  ******************************************************************************
  * @file    LwIP/LwIP_HTTP_Server_Socket_RTOS/Src/httpserver_socket.c
  * @author  MCD Application Team
  * @brief   Basic http server implementation using LwIP socket API
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
#include "lwip/opt.h"
#include "lwip/api.h"
#include "lwip/inet.h"
#include "lwip/sockets.h"
#include "lwip/apps/fs.h"
#include "string.h"
#include "httpserver_socket.h"
#include "cmsis_os.h"

#include <stdio.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define WEBSERVER_THREAD_PRIO    ( osPriorityAboveNormal )

#if LOOPBACK_TEST_ENABLE
#include "main.h"
#include "lwip/netif.h"
#include "lwip/ip4_addr.h"
#include "lwip/sys.h"

/* Dedicated UDP port and parameters for the optional loopback self-test. */
#define LOOPBACK_UDP_PORT          7
#define LOOPBACK_THREAD_PRIO       ( osPriorityNormal )
#define LOOPBACK_TEST_PERIOD_MS    2000U
#define LOOPBACK_TEST_COUNT        4U
#define LOOPBACK_MSG               "LwIP Loopback Test"
#endif /* LOOPBACK_TEST_ENABLE */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
void http_server_serve(int conn);
#if LOOPBACK_TEST_ENABLE
static void loopback_test_thread(void *arg);
#endif /* LOOPBACK_TEST_ENABLE */
/* Private functions ---------------------------------------------------------*/

/**
  * @brief serve tcp connection
  * @param conn: connection socket
  * @retval None
  */
void http_server_serve(int conn)
{
  int buflen = 1500;
  int ret;
  struct fs_file file;
  unsigned char recv_buffer[1500];

  /* Read in the request */
  ret = read(conn, recv_buffer, buflen);
  if(ret < 0) return;

  /* Check if request to get /img/sics.gif */
  if (strncmp((char *)recv_buffer,"GET /img/sics.gif", 17) == 0)
  {
    if (ERR_OK == fs_open(&file, "/img/sics.gif"))
    {
			write(conn, (const unsigned char*)(file.data), (size_t)file.len);
			fs_close(&file);
    }
    else
    {
    	if (ERR_OK == fs_open(&file, "/404.html"))
    	{
  			write(conn, (const unsigned char*)(file.data), (size_t)file.len);
  			fs_close(&file);
    	}
    }
  }
  /* Check if request to get index.html */
  else if ((strncmp((char *)recv_buffer,"GET /index.html", 15) == 0) || (strncmp((char *)recv_buffer, "GET / ", 6) == 0))
  {
    if (ERR_OK == fs_open(&file, "/index.html"))
    {
			write(conn, (const unsigned char*)(file.data), (size_t)file.len);
			fs_close(&file);
    }
    else
    {
    	if (ERR_OK == fs_open(&file, "/404.html"))
    	{
  			write(conn, (const unsigned char*)(file.data), (size_t)file.len);
  			fs_close(&file);
    	}
    }
  }
  else
  {
    /* Load 404 page */
  	if (ERR_OK == fs_open(&file, "/404.html"))
  	{
			write(conn, (const unsigned char*)(file.data), (size_t)file.len);
			fs_close(&file);
  	}
  }
  /* Close connection socket */
  close(conn);
}

/**
  * @brief  http server thread
  * @param arg: pointer on argument(not used here)
  * @retval None
  */
static void http_server_socket_thread(void *arg)
{
  int sock, newconn, size;
  struct sockaddr_in address, remotehost;

 /* create a TCP socket */
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    return;
  }

  /* bind to port 80 at any interface */
  address.sin_family = AF_INET;
  address.sin_port = htons(80);
  address.sin_addr.s_addr = INADDR_ANY;

  if (bind(sock, (struct sockaddr *)&address, sizeof (address)) < 0)
  {
    return;
  }

  /* listen for incoming connections (TCP listen backlog = 5) */
  listen(sock, 5);

  size = sizeof(remotehost);

  while (1)
  {
    newconn = accept(sock, (struct sockaddr *)&remotehost, (socklen_t *)&size);
    http_server_serve(newconn);
  }
}

/**
  * @brief  Initialize the HTTP server (start its thread)
  * @param  none
  * @retval None
  */
void http_server_socket_init()
{
  sys_thread_new("HTTP", http_server_socket_thread, NULL, DEFAULT_THREAD_STACKSIZE * 20, WEBSERVER_THREAD_PRIO);

#if LOOPBACK_TEST_ENABLE
  /* Start the optional single-board UDP loopback test in its own thread. */
  sys_thread_new("Loopback", loopback_test_thread, NULL, DEFAULT_THREAD_STACKSIZE * 4, LOOPBACK_THREAD_PRIO);
#endif /* LOOPBACK_TEST_ENABLE */
}

#if LOOPBACK_TEST_ENABLE
/**
  * @brief  Run the optional single-board UDP loopback self-test.
  * @param  arg not used
  * @retval None
  */
static void loopback_test_thread(void *arg)
{
  int server_sock;
  int client_sock;
  struct sockaddr_in server_addr;
  struct sockaddr_in client_addr;
  struct sockaddr_in dest_addr;
  struct sockaddr_in remote_addr;
  socklen_t remote_len;
  struct timeval timeout;
  char tx_buffer[64];
  char rx_buffer[64];
  int tx_len;
  int rx_len;
  uint32_t seq;
  uint32_t pass_count = 0U;
  uint32_t fail_count = 0U;

  LWIP_UNUSED_ARG(arg);

  while ((netif_default == NULL) || !netif_is_link_up(netif_default) ||
         ip4_addr_isany(netif_ip4_addr(netif_default)))
  {
    sys_msleep(100);
  }

  server_sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (server_sock < 0)
  {
    printf("Loopback test: failed to create server socket\r\n");
    return;
  }

  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(LOOPBACK_UDP_PORT);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
  {
    printf("Loopback test: failed to bind server socket\r\n");
    close(server_sock);
    return;
  }

  timeout.tv_sec = 2;
  timeout.tv_usec = 0;
  (void)setsockopt(server_sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

  client_sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (client_sock < 0)
  {
    printf("Loopback test: failed to create client socket\r\n");
    close(server_sock);
    return;
  }

  (void)setsockopt(client_sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

  memset(&dest_addr, 0, sizeof(dest_addr));
  dest_addr.sin_family = AF_INET;
  dest_addr.sin_port = htons(LOOPBACK_UDP_PORT);
  dest_addr.sin_addr.s_addr = netif_ip4_addr(netif_default)->addr;

  for (seq = 0U; seq < LOOPBACK_TEST_COUNT; seq++)
  {
    tx_len = snprintf(tx_buffer, sizeof(tx_buffer), "%s #%lu", LOOPBACK_MSG, (unsigned long)seq);

    if (sendto(client_sock, tx_buffer, tx_len, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0)
    {
      printf("Loopback test: failed to send datagram %lu\r\n", (unsigned long)seq);
      fail_count++;
      sys_msleep(LOOPBACK_TEST_PERIOD_MS);
      continue;
    }

    printf("Loopback test: sent %s to %s:%d\r\n", tx_buffer, inet_ntoa(dest_addr.sin_addr), LOOPBACK_UDP_PORT);

    remote_len = sizeof(remote_addr);
    rx_len = recvfrom(server_sock, rx_buffer, sizeof(rx_buffer), 0, (struct sockaddr *)&remote_addr, &remote_len);
    if (rx_len > 0)
    {
      (void)sendto(server_sock, rx_buffer, rx_len, 0, (struct sockaddr *)&remote_addr, remote_len);
    }

    remote_len = sizeof(client_addr);
    rx_len = recvfrom(client_sock, rx_buffer, sizeof(rx_buffer), 0, (struct sockaddr *)&client_addr, &remote_len);

    if ((rx_len == tx_len) && (memcmp(tx_buffer, rx_buffer, tx_len) == 0))
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

    sys_msleep(LOOPBACK_TEST_PERIOD_MS);
  }

  if (fail_count == 0U)
  {
    printf("\r\nLwIP loopback self-test PASSED (%lu/%lu packets)\r\n", (unsigned long)pass_count, (unsigned long)LOOPBACK_TEST_COUNT);
  }
  else
  {
    printf("\r\nLwIP loopback self-test FAILED (%lu/%lu packets)\r\n", (unsigned long)pass_count, (unsigned long)LOOPBACK_TEST_COUNT);
  }

  close(server_sock);
  close(client_sock);

  for (;;)
  {
    sys_msleep(1000);
  }
}
#endif /* LOOPBACK_TEST_ENABLE */
