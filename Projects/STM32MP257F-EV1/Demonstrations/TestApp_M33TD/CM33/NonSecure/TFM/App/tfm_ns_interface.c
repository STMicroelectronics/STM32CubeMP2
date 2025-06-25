/*
 * Copyright (c) 2017-2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*
 * An example to implement tfm_ns_interface_dispatch() in NS RTOS to integrate
 * TF-M interface on Armv8-M TrustZone based platforms.
 *
 * In this example, NS OS calls mutex in tfm_ns_interface_dispatch() to
 * synchronize multiple NS client calls.
 * NS OS pseudo code in this example is not based on any specific RTOS.
 *
 * Please note that this example cannot be built directly.
 */

#include <stdint.h>
#include "tfm_ns_interface.h"



int32_t tfm_ns_interface_dispatch(veneer_fn fn,
                                  uint32_t arg0, uint32_t arg1,
                                  uint32_t arg2, uint32_t arg3)
{
    int32_t result;

    result = fn(arg0, arg1, arg2, arg3);

    return result;
}
