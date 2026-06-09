/*
 * This file provides implementation of TF-M Non-Secure (NS) OS wrapper functions
 * specifically for use with CMSIS-OS2 (FreeRTOS) in an RTOS environment.
 */

#include <stdint.h>

#include "cmsis_os2.h"
#include "tfm_ns_interface.h"

static osMutexId_t ns_mutex_id = NULL;

int32_t tfm_ns_interface_dispatch(veneer_fn fn,
                                  uint32_t arg0, uint32_t arg1,
                                  uint32_t arg2, uint32_t arg3)
{
    int32_t result;
    uint32_t timeout = osWaitForever;
    osKernelState_t kernel_state;

    if ((ns_mutex_id == NULL) || (fn == NULL)) {
        return (int32_t)PSA_ERROR_GENERIC_ERROR;
    }

    kernel_state = osKernelGetState();
    if ((kernel_state == osKernelLocked) || (kernel_state == osKernelSuspended)) {
        timeout = 0U;
    }

    if (osMutexAcquire(ns_mutex_id, timeout) != osOK) {
        return (int32_t)PSA_ERROR_GENERIC_ERROR;
    }

    result = fn(arg0, arg1, arg2, arg3);

    if (osMutexRelease(ns_mutex_id) != osOK) {
        return (int32_t)PSA_ERROR_GENERIC_ERROR;
    }

    return result;
}

uint32_t tfm_ns_interface_init(void)
{
    if (ns_mutex_id == NULL) {
        ns_mutex_id = osMutexNew(NULL);
        if (ns_mutex_id == NULL) {
            return 1U;
        }
    }

    return 0U;
}