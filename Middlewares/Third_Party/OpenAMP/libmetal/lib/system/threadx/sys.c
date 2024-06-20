/*
 * Copyright (c) 2022 STMicroelectronnics. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file	threadx/sys.c
 * @brief	machine specific system primitives implementation.
 */

#include <metal/io.h>
#include <metal/sys.h>
#include <metal/utilities.h>
#include <stdint.h>

void metal_machine_cache_flush(void *addr, unsigned int len)
{
	metal_unused(addr);
	metal_unused(len);

	/* Add implementation here */
}

void metal_machine_cache_invalidate(void *addr, unsigned int len)
{
	metal_unused(addr);
	metal_unused(len);

	/* Add implementation here */
}
