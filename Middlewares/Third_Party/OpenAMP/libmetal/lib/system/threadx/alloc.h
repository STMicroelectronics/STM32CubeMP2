/*
 * Copyright (c) 2022 STMicroelectronnics. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file	threadx/alloc.c
 * @brief	ThreadX libmetal memory allocation definitions.
 */

#ifndef __METAL_ALLOC__H__
#error "Include metal/alloc.h instead of metal/threadx/alloc.h"
#endif

#ifndef __METAL_THREADX_ALLOC__H__
#define __METAL_THREADX_ALLOC__H__

#include "tx_api.h"

#ifdef __cplusplus
extern "C" {
#endif

void *metal_threadx_allocate_memory(unsigned int size);
void metal_threadx_free_memory(void *ptr);

static inline void *metal_allocate_memory(unsigned int size)
{
	return metal_threadx_allocate_memory(size);
}

static inline void metal_free_memory(void *ptr)
{
	metal_threadx_free_memory(ptr);
}

#ifdef __cplusplus
}
#endif

#endif /* __METAL_THREADX_ALLOC__H__ */
