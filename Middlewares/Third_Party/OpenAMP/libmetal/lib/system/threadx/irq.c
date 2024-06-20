/*
 * Copyright (c) 2022 STMicroelectronnics. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file	threadx/irq.c
 * @brief	ThreadX libmetal irq definitions.
 */

#include <metal/errno.h>
#include <metal/irq.h>
#include <metal/sys.h>
#include <metal/log.h>
#include <metal/mutex.h>
#include <metal/list.h>
#include <metal/utilities.h>
#include <metal/alloc.h>

unsigned int metal_irq_save_disable(void)
{
}

void metal_irq_restore_enable(unsigned int flags)
{
	metal_unused(flags);
}

