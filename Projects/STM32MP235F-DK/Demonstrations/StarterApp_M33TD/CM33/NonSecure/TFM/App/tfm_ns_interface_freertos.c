/* 
 * This file provides implementation of TF-M Non-Secure (NS) OS wrapper functions
 * specifically for use with CMSIS-OS2 (FreeRTOS) in an RTOS environment.
 * It ensures multithread safety for TF-M NS interface dispatch and initialization,
 * allowing secure interaction between non-secure application code and TF-M services
 * in a multi-threaded FreeRTOS context.
 * 
 * The functions implemented here are designed to be thread-safe and suitable for
 * integration with RTOS-based applications, enabling secure service calls from
 * multiple tasks without compromising system integrity.
 */
/*
 * tfm_ns_interface_freertos.c
 * Implements TF-M NS interface dispatch and init using CMSIS-OS2 (FreeRTOS)
 */
#include <stdint.h>
#include "tfm_ns_interface.h"
#include "cmsis_os2.h"
#include "stdio.h"
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

    /* Acquire the mutex to ensure thread safety */
    if (osMutexAcquire(ns_mutex_id, timeout) != osOK) {
        return (int32_t)PSA_ERROR_GENERIC_ERROR;
    }

    /* Call the secure veneer function */
    result = fn(arg0, arg1, arg2, arg3);

    /* Release the mutex after operation */
    if (osMutexRelease(ns_mutex_id) != osOK) {
        return (int32_t)PSA_ERROR_GENERIC_ERROR;
    }

    return result;
}

uint32_t tfm_ns_interface_init(void)
{
    if (!ns_mutex_id) {
        ns_mutex_id = osMutexNew(NULL);
        if (!ns_mutex_id) {
            return 1;
        }
    }
    return 0;
}
