/*
 * Copyright (c) 2022 STMicroelectronnics. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file	zephyr/alloc.c
 * @brief	Zephyr libmetal memory allocation handling.
 */

#include <metal/alloc.h>
#include <metal/assert.h>
#include <metal/compiler.h>

void *metal_weak metal_threadx_allocate_memory(unsigned int size)
{
	/*
	 * with ThreadX the pool used to allocate the memory has to be specified
	 * let the user redefine the function.
	 * TODO: Implement a defaut allocator. The function ids used by the openamp library. this
	 * can lead to an unexpected metal_threadx_allocate_memory function used.
	 * the open amp could have only as reference this weak function.
	 * The  metal_assert(size == 0) should alert user in such case.
	 */

	metal_assert(size == 0);
	return NULL;
}

void metal_weak metal_threadx_free_memory(void *ptr)
{
	/* user has to redefine it in thread, else TODO: find a way to get it in global pool */
	metal_assert(!ptr);
}
