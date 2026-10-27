/*
 *  Copyright (C) 2018, STMicroelectronics, all rights reserved.
 *  Copyright (C) 2006-2018, ARM Limited, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of mbed TLS (https://tls.mbed.org)
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include <string.h>
#include <stdint.h>

#if defined(MBEDTLS_NET_C)

#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#else
#include <stdlib.h>
#endif

#include "mbedtls/net_sockets.h"

#include "lwip/dhcp.h"
#include "lwip/tcpip.h"
#include "lwip/ip_addr.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"
#include "lwip/netif.h"
#include "netif/ethernet.h"
#include "app_ethernet.h"
#include "ethernetif.h"
#include "main.h"

static int initialized = 0;
struct sockaddr_storage client_addr;

static int net_would_block( const mbedtls_net_context *ctx );

extern struct netif gnetif;
osThreadId_t ethernet_link_thread_handle;
static const osThreadAttr_t ethernet_link_thread_attributes = {
  .name = "Ethernet Link Thread",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 1024 * 2
};

osThreadId_t DHCPThreadHandle;
static const osThreadAttr_t DHCPThread_attributes = {
  .name = "DHCP Thread",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 1024 * 2
};

/*
 * Initialize LwIP stack and get a dynamic IP address.
 */
void mbedtls_net_init( mbedtls_net_context *ctx )
{
  ip4_addr_t ipaddr;
  ip4_addr_t netmask;
  ip4_addr_t gw;

  if (initialized != 0){
    return;
  }

  tcpip_init(NULL, NULL);

#if LWIP_DHCP
    ip_addr_set_zero_ip4(&ipaddr);
    ip_addr_set_zero_ip4(&netmask);
    ip_addr_set_zero_ip4(&gw);
#else
    IP_ADDR4(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
    IP_ADDR4(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
    IP_ADDR4(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
#endif /* LWIP_DHCP */

  /* add the network interface */
  netif_add(&gnetif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &ethernet_input);

  /* Set the first interface as the default */
  netif_set_default(&gnetif);

  ethernet_link_status_updated(&gnetif);

#if LWIP_NETIF_LINK_CALLBACK
  netif_set_link_callback(&gnetif, ethernet_link_status_updated);

  ethernet_link_thread_handle = osThreadNew(ethernet_link_thread, &gnetif, &ethernet_link_thread_attributes);
  if (NULL == ethernet_link_thread_handle)
  {
    Error_Handler();
  }
#endif

#if LWIP_DHCP
  /* Start DHCP client */
  printf("Starting DHCP client thread...\n\r");
  DHCPThreadHandle = osThreadNew(DHCP_Thread, &gnetif, &DHCPThread_attributes);
  if (NULL == DHCPThreadHandle)
  {
    printf("Error: Failed to create DHCP client thread\n\r");
    Error_Handler();
  }
#endif

#if LWIP_DHCP
  /* Wait for DHCP address assignment */
  while (netif_ip4_addr(&gnetif)->addr == 0U)
  {
    osDelay(250);
  }
#endif

  if (netif_ip4_addr(&gnetif)->addr == 0U)
  {
    printf(" Failed to get ip address! Please check your network configuration.\n");
    Error_Handler();
  }
  else
  {
    printf("\nIpAddress = %s\n\r", ip4addr_ntoa(netif_ip4_addr(&gnetif)));
    initialized = 1;
  }
}

/*
 * Initiate a TCP connection with host:port and the given protocol
 */
int mbedtls_net_connect( mbedtls_net_context *ctx, const char *host, const char *port, int proto )
{
  int ret;
  struct addrinfo hints;
  struct addrinfo *list;
  struct addrinfo *current;

  /* Do name resolution with both IPv6 and IPv4 */
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = proto == MBEDTLS_NET_PROTO_UDP ? SOCK_DGRAM : SOCK_STREAM;
  hints.ai_protocol = proto == MBEDTLS_NET_PROTO_UDP ? IPPROTO_UDP : IPPROTO_TCP;

  if(getaddrinfo(host, port, &hints, &list) != 0)
    return MBEDTLS_ERR_NET_UNKNOWN_HOST;

  /* Try the sockaddrs until a connection succeeds */
  ret = MBEDTLS_ERR_NET_UNKNOWN_HOST;
  for( current = list; current != NULL; current = current->ai_next)
  {
    ctx->fd = (int) socket(current->ai_family, current->ai_socktype, current->ai_protocol);
    if(ctx->fd < 0)
    {
      ret = MBEDTLS_ERR_NET_SOCKET_FAILED;
      continue;
    }

    if(connect(ctx->fd, current->ai_addr, (uint32_t)current->ai_addrlen) == 0)
    {
      ret = 0;
      break;
    }

    close( ctx->fd );
    ret = MBEDTLS_ERR_NET_CONNECT_FAILED;
  }

  freeaddrinfo(list);

  return ret;

}

/*
 * Create a listening socket on bind_ip:port
 */
int mbedtls_net_bind( mbedtls_net_context *ctx, const char *bind_ip, const char *port, int proto )
{
  int ret = 0;
  mbedtls_printf ("%s() NOT IMPLEMENTED!!\n", __FUNCTION__);

  return ret;
}

/*
 * Accept a connection from a remote client
 */
int mbedtls_net_accept( mbedtls_net_context *bind_ctx,
                        mbedtls_net_context *client_ctx,
                        void *client_ip, size_t buf_size, size_t *ip_len )
{
  mbedtls_printf ("%s() NOT IMPLEMENTED!!\n", __FUNCTION__);
  return 0;
}

/*
 * Set the socket blocking or non-blocking
 */
int mbedtls_net_set_block( mbedtls_net_context *ctx )
{
  mbedtls_printf ("%s() NOT IMPLEMENTED!!\n", __FUNCTION__);
  return 0;
}

int mbedtls_net_set_nonblock( mbedtls_net_context *ctx )
{
  mbedtls_printf ("%s() NOT IMPLEMENTED!!\n", __FUNCTION__);
  return 0;
}

/*
 * Portable usleep helper
 */
void mbedtls_net_usleep( unsigned long usec )
{
  mbedtls_printf ("%s() NOT IMPLEMENTED!!\n", __FUNCTION__);
}

/*
 * Read at most 'len' characters
 */
int mbedtls_net_recv( void *ctx, unsigned char *buf, size_t len )
{
  int32_t ret;
  fd_set readset;
  int32_t fd = ((mbedtls_net_context *) ctx)->fd;

  if( fd < 0 )
  {
    return MBEDTLS_ERR_NET_INVALID_CONTEXT;
  }

  do
  {
    FD_ZERO(&readset);
    FD_SET(fd, &readset);
    ret = select(fd + 1, &readset, NULL, NULL, NULL);
   } while (ret == -1 && errno == EINTR);

  if (ret > 0)
  {
    if (FD_ISSET(fd, &readset))
    {
      ret = (int32_t) read( fd, buf, len );
    }
  }
  else
  {
    if(net_would_block(ctx) != 0)
    {
      return MBEDTLS_ERR_SSL_WANT_READ;
    }

    if(errno == EPIPE || errno == ECONNRESET)
    {
      return MBEDTLS_ERR_NET_CONN_RESET;
    }

    if(errno == EINTR)
    {
      return MBEDTLS_ERR_SSL_WANT_READ;
    }

    return MBEDTLS_ERR_NET_RECV_FAILED;
  }

  return ret;
}

/*
 * Read at most 'len' characters, blocking for at most 'timeout' ms
 */
int mbedtls_net_recv_timeout( void *ctx, unsigned char *buf, size_t len,
                      uint32_t timeout )
{
  int ret = 0;
  mbedtls_printf ("%s() NOT IMPLEMENTED!!\n", __FUNCTION__);

  return ret;
}


static int net_would_block( const mbedtls_net_context *ctx )
{
  /*
   * Never return 'WOULD BLOCK' on a non-blocking socket
   */

  int val = 0;
  UNUSED(val);

  if( ( fcntl( ctx->fd, F_GETFL, val) & O_NONBLOCK ) != O_NONBLOCK )
    return( 0 );

  switch( errno )
  {
#if defined EAGAIN
    case EAGAIN:
#endif
#if defined EWOULDBLOCK && EWOULDBLOCK != EAGAIN
    case EWOULDBLOCK:
#endif
    return( 1 );
  }

  return( 0 );
}

/*
 * Write at most 'len' characters
 */
int mbedtls_net_send( void *ctx, const unsigned char *buf, size_t len )
{
  int32_t ret;
  int fd = ((mbedtls_net_context *) ctx)->fd;

  if( fd < 0 )
  {
    return MBEDTLS_ERR_NET_INVALID_CONTEXT;
  }

  ret = (int32_t) write(fd, buf, len);

  if( ret < 0 )
  {
    if(net_would_block(ctx) != 0)
    {
      return MBEDTLS_ERR_SSL_WANT_WRITE;
    }

    if(errno == EPIPE || errno == ECONNRESET)
    {
      return MBEDTLS_ERR_NET_CONN_RESET;
    }

    if(errno == EINTR)
    {
      return MBEDTLS_ERR_SSL_WANT_WRITE;
    }

    return MBEDTLS_ERR_NET_SEND_FAILED;
  }

  return ret;
}

/*
 * Gracefully close the connection
 */
void mbedtls_net_free( mbedtls_net_context *ctx )
{
  if( ctx->fd == -1 )
    return;

  shutdown( ctx->fd, 2 );
  close( ctx->fd );

  ctx->fd = -1;
 }

#endif /* MBEDTLS_NET_C */
