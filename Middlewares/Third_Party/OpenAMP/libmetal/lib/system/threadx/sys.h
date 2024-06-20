/*
 * Copyright (c) 2022 STMicroelectronnics. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file	threadx/sys.h
 * @brief	ThreadX system primitives for libmetal.
 */

#ifndef __METAL_SYS__H__
#error "Include metal/sys.h instead of metal/threadx/sys.h"
#endif

#ifndef __METAL_THREADX_SYS__H__
#define __METAL_THREADX_SYS__H__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef METAL_MAX_DEVICE_REGIONS
#define METAL_MAX_DEVICE_REGIONS 1
#endif

/** Structure of nuttx libmetal runtime state. */
struct metal_state {

	/** Common (system independent) data. */
	struct metal_common_state common;
};

#ifdef __cplusplus
}
#endif

#endif /* __METAL_THREADX_SYS__H__ */
