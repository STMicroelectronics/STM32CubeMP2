/**
  ******************************************************************************
  * @file    stm32mp2xx_mmu_conf.h
  * @author  MCD Application Team
  * @brief   This file contains the memory mapping.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STM32MP2XX_MMU_CONF_H
#define STM32MP2XX_MMU_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "mmu_stm32mp2xx.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/*
 * User configuration:
 * -------------------
 * Define all MMU regions using the MMU_REGION_TABLE macro.
 *
 * Each entry is:
 *   { va_start, pa_start, size, attrs, xn, ns, name, granularity }
 *
 * Addresses and sizes must be aligned to the selected granularity:
 * 1 Mbyte for MMU_GRANULARITY_SECTION_1M, 4 Kbytes for MMU_GRANULARITY_PAGE4K.
 */
#define MMU_REGION_TABLE                                                \
{                                                                       \
    /* ============================================================= */ \
    /* SYSRAM SCMI shared memory: 0x20000000 - 0x200FFFFF (1 Mbyte) */ \
    /* Contains SCMI shared memory at 0x20041000                    */ \
    /* ============================================================= */ \
    {                                                                   \
        .va_start    = 0x20000000U,                                     \
        .pa_start    = 0x20000000U,                                     \
        .size        = 0x00100000U,                                     \
        .attrs       = MEM_ATTR_NORMAL_NONCACHEABLE_SHARED,             \
        .xn          = 1U,                                              \
        .name        = "sysram_scmi_shmem",                            \
        .granularity = MMU_GRANULARITY_SECTION_1M                       \
    },                                                                  \
                                                                        \
    /* ============================================================= */ \
    /* Peripherals: 0x40000000 - 0x5FFFFFFF (512 Mbytes), Device, XN */ \
    /* ============================================================= */ \
    {                                                                   \
        .va_start    = 0x40000000U,                                     \
        .pa_start    = 0x40000000U,                                     \
        .size        = 0x20000000U,   /* 512MB */                       \
        .attrs       = MEM_ATTR_DEVICE_SHARED,                          \
        .xn          = 1U,            /* non-executable */              \
        .name        = "Peripherals (0x4000_0000-0x5FFF_FFFF)",         \
        .granularity = MMU_GRANULARITY_SECTION_1M                       \
    },                                                                  \
                                                                        \
    /* ============================================================= */ \
    /* MM OSPI2: 0x60000000 - 0x6FFFFFFF (256 Mbytes), Device        */ \
    /* ============================================================= */ \
    {                                                                   \
        .va_start    = 0x60000000U,                                     \
        .pa_start    = 0x60000000U,                                     \
        .size        = 0x10000000U,   /* 256MB */                       \
        .attrs       = MEM_ATTR_DEVICE_SHARED,                          \
        .xn          = 0U,            /* executable (XIP possible) */   \
        .name        = "MM OSPI2 / Octo (0x6000_0000-0x6FFF_FFFF)",     \
        .granularity = MMU_GRANULARITY_SECTION_1M                       \
    },                                                                  \
                                                                        \
    /* ============================================================= */ \
    /* CA35 Cube firmware DDR: 0x82000000 - 0xFFFFFFFF, cacheable    */ \
    /* ============================================================= */ \
    {                                                                   \
        .va_start    = 0x82000000U,                                     \
        .pa_start    = 0x82000000U,                                     \
        .size        = 0x7E000000U,                                     \
        .attrs       = MEM_ATTR_NORMAL_WBWA_SHARED,                     \
        .xn          = 0U,                                              \
        .name        = "ca35_cube_fw",                                  \
        .granularity = MMU_GRANULARITY_SECTION_1M                       \
    },                                                                  \
                                                                        \
    /* IPC shared memory: 0x80A00000 - 0x80AFFFFF (1 Mbyte)         */ \
    {                                                                   \
        .va_start    = 0x80A00000U,                                     \
        .pa_start    = 0x80A00000U,                                     \
        .size        = 0x00100000U,                                     \
        .attrs       = MEM_ATTR_NORMAL_NONCACHEABLE_SHARED,             \
        .xn          = 1U,            /* non-executable */              \
        .ns          = 1U,            /* non-secure access */           \
        .name        = "ipc_shmem",                                     \
        .granularity = MMU_GRANULARITY_SECTION_1M                       \
    },                                                                  \
}

#ifdef __cplusplus
}
#endif

#endif /* STM32MP2XX_MMU_CONF_H */
