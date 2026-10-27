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

/* Dedicated UDP port used by the optional loopback self-test */
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
    printf("HTTP server: failed to bind socket\r\n");
    close(sock);
    return;
  }

  /* listen for incoming connections (TCP listen backlog = 5) */
  if (listen(sock, 5) < 0)
  {
    printf("HTTP server: failed to listen on socket\r\n");
    close(sock);
    return;
  }

  size = sizeof(remotehost);

  while (1)
  {
    newconn = accept(sock, (struct sockaddr *)&remotehost, (socklen_t *)&size);
    if (newconn >= 0)
    {
      http_server_serve(newconn);
    }
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
  /* Optional single-board loopback self-test: exercises the LwIP stack on the single board (No Second Board or PC required).
   * This test is enabled by defining LOOPBACK_TEST_ENABLE in lwipopts.h.  The test uses a dedicated UDP port (7) to send and
   * receive packets to itself.  The test runs in its own thread and does not interfere with the HTTP server operation.  The
   * test will run for a fixed number of iterations (LOOPBACK_TEST_COUNT) and then terminate.  The test will print the results
   * to the console via printf
   */
  sys_thread_new("Loopback", loopback_test_thread, NULL, DEFAULT_THREAD_STACKSIZE * 4, LOOPBACK_THREAD_PRIO);
#endif /* LOOPBACK_TEST_ENABLE */
}

#if LOOPBACK_TEST_ENABLE

/*
 * @brief Optional single-board loopback self-test thread: runs a small UDP echo server together with a client
 *        that sends datagrams to the board's own IP address, checking that they are echoed back correctly. This
 *        lets the LwIP stack be exercised on a single board, without requiring a second board or PC. The test runs
 *        for 4 iterations and then terminates. The test prints the results to the console via printf.
 */
static void loopback_test_thread(void *arg)
{
  int server_sock, client_sock;
  struct sockaddr_in server_addr, client_addr, dest_addr, remote_addr;
  socklen_t remote_len;
  struct timeval timeout;
  char tx_buff[64];
  char rx_buff[64];
  int tx_len, rx_len;
  uint32_t seq;
  uint32_t pass_count = 0;
  uint32_t fail_count = 0;

  /*
   * arg is unused in this function, but is required by the sys_thread_new() API. It can be used to pass parameters
   * to the thread if needed, but in this case we don't need any parameters, so we just mark it as unused.
   */
  LWIP_UNUSED_ARG(arg);

  /*
   * Wait until the network interface has a valid IP address and the link is up before starting the test. This ensures that
   * the test will not fail due to the network interface not being ready. The loop checks for three conditions:
   * 1. netif_default is not NULL: This checks that the default network interface has been initialized.
   * 2. netif_is_link_up(netif_default) is true: This checks that the link is up, meaning that the physical connection is active.
   * 3. ip4_addr_isany(netif_ip4_addr(netif_default)) is false: This checks that the IP address of the default network interface
   *    is not the "any" address (0.0.0.0).
   */
   while((netif_default == NULL) || !netif_is_link_up(netif_default) || ip4_addr_isany(netif_ip4_addr(netif_default)))
   {
     sys_msleep(100);
   }

   /* Create the local UDP echo server */
   server_sock = socket(AF_INET, SOCK_DGRAM, 0);

    if (server_sock < 0)
    {
      printf("Loopback test: failed to create server socket\r\n");
      return;
    }

    /*
     * Bind the server socket to the loopback UDP port (7) on any interface. This allows the server to receive
     * datagrams sent to this port from any source. The server will listen for incoming datagrams on this port.
     */
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(LOOPBACK_UDP_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    /* Bind the server socket to the specified address and port */
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
      printf("Loopback test: failed to bind server socket\r\n");
      close(server_sock);
      return;
    }

    /* 2 second timeout for recvfrom */
    timeout.tv_sec = 2;

	/* 0 microseconds */
    timeout.tv_usec = 0;

	setsockopt(server_sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    /* Create the client socket */
    client_sock = socket(AF_INET, SOCK_DGRAM, 0);

    /* Check if the client socket was created successfully */
    if (client_sock < 0)
    {
      printf("Loopback test: failed to create client socket\r\n");
      close(server_sock);
      return;
    }

    /* Set the receive timeout for the client socket */
    setsockopt(client_sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    /* Set the destination address for the client socket to the board's own IP address and loopback UDP port */
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(LOOPBACK_UDP_PORT);
    dest_addr.sin_addr.s_addr = netif_ip4_addr(netif_default)->addr;

    /* Perform the loopback test */
    for(seq = 0; seq < LOOPBACK_TEST_COUNT; seq++)
    {
      /* Prepare the datagram to be sent */
      tx_len = snprintf(tx_buff, sizeof(tx_buff), "%s#%lu", LOOPBACK_MSG, (unsigned long)seq);

      /* Send the datagram to the server */
      if(sendto(client_sock, tx_buff, tx_len, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0)
      {
        printf("Loopback test: failed to send datagram %lu\r\n", (unsigned long)seq);
        fail_count++;
        sys_msleep(LOOPBACK_TEST_PERIOD_MS);
        continue;
      }

      printf("Loopback test: sent %s to %s:%d\r\n", tx_buff, inet_ntoa(dest_addr.sin_addr), LOOPBACK_UDP_PORT);

      /* Echo the datagram back to its sender */
      remote_len = sizeof(remote_addr);
      rx_len = recvfrom(server_sock, rx_buff, sizeof(rx_buff), 0, (struct sockaddr *)&remote_addr, &remote_len);

      /* Echo the datagram back to its sender if received successfully */
      if(rx_len > 0)
      {
        sendto(server_sock, rx_buff, rx_len, 0, (struct sockaddr *)&remote_addr, remote_len);
      }

      /* Receive the echoed reply on the client socket and compare it */
      remote_len = sizeof(client_addr);
      rx_len = recvfrom(client_sock, rx_buff, sizeof(rx_buff), 0, (struct sockaddr *)&client_addr, &remote_len);

      if((rx_len == tx_len) && (memcmp(tx_buff, rx_buff, tx_len) == 0U))
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

    /* Print the loopback test result */
    if(fail_count == 0U)
    {
      printf("\r\nLwIP Loopback self-test PASSED (%lu/%lu packets)\r\n", (unsigned long)pass_count, (unsigned long)LOOPBACK_TEST_COUNT);
    }
    else
    {
      printf("\r\nLwIP Loopback self-test FAILED (%lu/%lu packets)\r\n", (unsigned long)fail_count, (unsigned long)LOOPBACK_TEST_COUNT);
    }

    /* Close the sockets */
    close(server_sock);
    close(client_sock);

    /* Nothing left to do: Keep the thread alive without consuming CPU */
    for(;;)
    {
      sys_msleep(1000);
    }
}


#endif