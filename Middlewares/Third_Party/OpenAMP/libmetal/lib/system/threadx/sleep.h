/*
 * Copyright (c) 2022 STMicroelectronnics. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file	threadx/sleep.h
 * @brief	ThreadX sleep primitives for libmetal.
 */

#ifndef __METAL_SLEEP__H__
#error "Include metal/sleep.h instead of metal/threadx/sleep.h"
#endif

#ifndef __METAL_THREADX_SLEEP__H__
#define __METAL_THREADX_SLEEP__H__

#include "tx_api.h"

#ifdef __cplusplus
extern "C" {
#endif

static inline int __metal_sleep_usec(unsigned int usec)
{
	unsigned long xdelay = usec / 1000 / TX_TIMER_TICKS_PER_SECOND;

	tx_thread_sleep(xdelay ? xdelay : 1);
	return 0;
}

#ifdef __cplusplus
}
#endif

#endif /* __METAL_THREADX_SLEEP__H__ */
