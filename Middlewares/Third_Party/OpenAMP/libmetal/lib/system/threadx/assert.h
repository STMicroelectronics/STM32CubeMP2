/*
 * Copyright (c) 2022 STMicroelectronnics. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file	assert.h
 * @brief	ThreadX assertion support.
 */
#ifndef __METAL_ASSERT__H__
#error "Include metal/assert.h instead of metal/threadx/assert.h"
#endif

#ifndef __METAL_THREADX_ASSERT__H__
#define __METAL_THREADX_ASSERT__H__

#include <assert.h>

/**
 * @brief Assertion macro for ThreadX applications.
 * @param cond Condition to evaluate.
 */
#define metal_sys_assert(cond) assert(cond)

#endif /* __METAL_THREADX_ASSERT__H__ */

