/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
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
 */
#ifndef __TCP_ECHOSERVER_H__
#define __TCP_ECHOSERVER_H__

#include "lwip/netif.h"

void tcp_echoserver_init(void);

#if LOOPBACK_TEST_ENABLE
/* Period, in milliseconds, between two loopback test connections */
#define LOOPBACK_TEST_PERIOD_MS   2000U

/* Total number of loopback test connections attempted before the test completes */
#define LOOPBACK_TEST_COUNT       4U

/* Payload used for the loopback test messages */
#define LOOPBACK_MSG              "LwIP Loopback Test"

/* Optional single-board loopback self-test: connects to the TCP echo
   server above using this board's own IP address, sends a test message
   and checks that it is echoed back correctly. See LOOPBACK_TEST_ENABLE
   in lwipopts.h. */
void loopback_test_periodic_handle(struct netif *netif);
#endif /* LOOPBACK_TEST_ENABLE */

#endif /* __TCP_ECHOSERVER */
