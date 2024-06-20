/*
 * Copyright (c) 2022 STMicroelectronnics. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file	threadx/mutex.h
 * @brief	ThreadX mutex primitives for libmetal.
 */

#ifndef __METAL_MUTEX__H__
#error "Include metal/mutex.h instead of metal/threadx/mutex.h"
#endif

#ifndef __METAL_THREADX_MUTEX__H__
#define __METAL_THREADX_MUTEX__H__

#include <metal/assert.h>
#include "tx_api.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	TX_MUTEX m;
} metal_mutex_t;

/*
 * METAL_MUTEX_INIT - used for initializing an mutex element in a static struct
 * or global
 */
#if defined(__GNUC__)
#define METAL_MUTEX_INIT(m) { NULL }; \
_Pragma("GCC warning\"static initialisation of the mutex is deprecated\"")
#elif defined(__ICCARM__)
#define DO_PRAGMA(x) _Pragma(#x)
#define METAL_MUTEX_INIT(m) { NULL }; \
DO_PRAGMA(message("Warning: static initialisation of the mutex is deprecated"))
#else
#define METAL_MUTEX_INIT(m) { NULL }
#endif

/*
 * METAL_MUTEX_DEFINE - used for defining and initializing a global or
 * static singleton mutex
 */
#define METAL_MUTEX_DEFINE(m) metal_mutex_t m = METAL_MUTEX_INIT(m)

static inline void __metal_mutex_init(metal_mutex_t *mutex)
{
	metal_assert(mutex);
	metal_assert(tx_mutex_create(&mutex->m, "Mutex", TX_NO_INHERIT) == TX_SUCCESS);
}

static inline void __metal_mutex_deinit(metal_mutex_t *mutex)
{
	metal_assert(mutex);
	metal_assert(tx_mutex_delete(&mutex->m) == TX_SUCCESS);
}

static inline int __metal_mutex_try_acquire(metal_mutex_t *mutex)
{
	metal_assert(mutex);
	return tx_mutex_get(&mutex->m, TX_NO_WAIT);
}

static inline void __metal_mutex_acquire(metal_mutex_t *mutex)
{
	metal_assert(mutex);
	metal_assert(tx_mutex_get(&mutex->m, TX_WAIT_FOREVER) == TX_SUCCESS);
}

static inline void __metal_mutex_release(metal_mutex_t *mutex)
{
	metal_assert(mutex);
	metal_assert(tx_mutex_put(&mutex->m) == TX_SUCCESS);
}

static inline int __metal_mutex_is_acquired(metal_mutex_t *mutex)
{
	CHAR *name;
	ULONG count;
	TX_THREAD *owner;
	TX_THREAD *first_suspended;
	ULONG suspended_count;
	TX_MUTEX *next_mutex;

	metal_assert(mutex);
	metal_assert(tx_mutex_info_get(&mutex->m, &name, &count, &owner, &first_suspended,
				       &suspended_count, &next_mutex) == TX_SUCCESS);
	return (int)count;
}

#ifdef __cplusplus
}
#endif

#endif /* __METAL_THREADX_MUTEX__H__ */
