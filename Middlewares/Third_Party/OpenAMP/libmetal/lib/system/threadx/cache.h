/*
 * Copyright (c) 2022 STMicroelectronnics. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file	threadx/cache.h
 * @brief	ThreadX cache operation primitives for libmetal.
 */

#ifndef __METAL_CACHE__H__
#error "Include metal/cache.h instead of metal/threadx/cache.h"
#endif

#ifndef __METAL_THREADX_CACHE__H__
#define __METAL_THREADX_CACHE__H__

#ifdef __cplusplus
extern "C" {
#endif

extern void metal_machine_cache_flush(void *addr, unsigned int len);
extern void metal_machine_cache_invalidate(void *addr, unsigned int len);

static inline void __metal_cache_flush(void *addr, unsigned int len)
{
	metal_machine_cache_flush(addr, len);
}

static inline void __metal_cache_invalidate(void *addr, unsigned int len)
{
	metal_machine_cache_invalidate(addr, len);
}

#ifdef __cplusplus
}
#endif

#endif /* __METAL_THREADX_CACHE__H__ */
