/**
 ******************************************************************************
 * @file    rsc_table.c
 * @author  MCD Application Team
 * @brief   Ressource table
 *
 *   This file provides a default resource table requested by remote proc to
 *  load the elf file. It also allows to add debug trace using a shared buffer.
 *
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2019-2026 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

/** @addtogroup RSC_TABLE
  * @{
  */

/** @addtogroup resource_table
  * @{
  */

/** @addtogroup resource_table_Private_Includes
  * @{
  */

#include <stddef.h>
#include <string.h>
#if defined(STM32MP157Cxx)
#include "stm32mp1xx_hal.h"
#elif defined(STM32MP25xxxx) || defined(STM32MP2)
#include "stm32mp2xx_hal.h"
#endif
#include "openamp.h"
#include "rsc_table.h"

#define RPMSG_VIRTIO_FEATURE     1
#define VRING_COUNT              2

#if defined(__ICCARM__)
/*
 * For IAR, the .icf file should contain the following lines:
 * define symbol __RSC_TABLE_region_start__ = BASE_ADDRESS; (0x38000400 for example)
 * define symbol __RSC_TABLE_region_size__   = MEM_SIZE; (0xB000 as example)
 *
 * export symbol __RSC_TABLE_region_start__;
 * export symbol __RSC_TABLE_region_size__;
 */
extern const uint32_t  __RSC_TABLE_region_start__;
extern const uint8_t  __RSC_TABLE_region_size__;
#define RSCTABSTART_ADDRESS       ((metal_phys_addr_t)&__RSC_TABLE_region_start__)
#define RSCTABSIZE        ((size_t)&__RSC_TABLE_region_size__)

#elif defined(__CC_ARM)
/*
 * For MDK-ARM, the scatter file .sct should contain the following line:
 * LR_IROM1 ....  {
 *  ...
 *   __OpenAMP_SHMEM__ 0x38000400  EMPTY 0x0000B000 {} ; Shared Memory area used by OpenAMP
 *  }
 *
 */
extern unsigned int Image$$__OpenAMP_SHMEM__$$Base;
extern unsigned int Image$$__OpenAMP_SHMEM__$$ZI$$Length;
#define RSCTAB_START_ADDRESS (unsigned int)&Image$$__OpenAMP_SHMEM__$$Base
#define RSCTAB_SIZE          ((size_t)&Image$$__OpenAMP_SHMEM__$$ZI$$Length)

#else
/*
 * for GCC add the following content to the .ld file:
 * MEMORY
 * {
 * ...
 * OPEN_AMP_SHMEM (xrw) : ORIGIN = 0x38000400, LENGTH = 63K
 * }
 * __RSC_TABLE_region_start__  = ORIGIN(OPEN_AMP_SHMEM);
 * __RSC_TABLE_region_end__ = ORIGIN(OPEN_AMP_SHMEM) + LENGTH(OPEN_AMP_SHMEM);
 *
 * using the LENGTH(OPEN_AMP_SHMEM) to set the RSCTABSIZE lead to a crash thus we
 * use the start and end address.
 */

extern int __RSC_TABLE_region_start__[];  /* defined by linker script */
extern int __RSC_TABLE_region_end__[];    /* defined by linker script */

#define RSCTAB_START_ADDRESS       ((void *)__RSC_TABLE_region_start__)
#define RSCTAB_SIZE                (size_t)((void *)__RSC_TABLE_region_end__ - (void *) __RSC_TABLE_region_start__)

#endif

#if !defined (VIRTIO_STATIC_VRING)
#if (OPENAMP_ROLE == RPMSG_HOST) || !defined (LINUX_RPROC_MASTER)
  #define CONST
#else
  #define CONST const
#endif
#else
 #define CONST
#endif

#if (OPENAMP_ROLE == RPMSG_REMOTE)
/**
  * @}
  */

/** @addtogroup resource_table_Private_TypesDefinitions
  * @{
  */

/**
  * @}
  */

/** @addtogroup resource_table_Private_Defines
  * @{
  */

/* Place resource table in special ELF section */
#if defined(__GNUC__)
#if defined(LINUX_RPROC_MASTER)
#define __section_t(S)          __attribute__((__section__(#S)))
#define __resource              __section_t(.resource_table)
#else
#define __resource
#endif
#endif /* defined(__GNUC__ */

#if defined (__LOG_TRACE_IO_)
extern char system_log_buf[];
#endif /* defined (__LOG_TRACE_IO_) */

#if defined(__GNUC__)
CONST struct shared_resource_table __resource __attribute__((used)) resource_table = {
#elif defined(__ICCARM__)
__root CONST struct shared_resource_table resource_table @ ".resource_table" = {
#endif /* defined(__GNUC__) */
  .version = 1,
#if defined (__LOG_TRACE_IO_)
  .num = 2,
#else
  .num = 1,
#endif
  .reserved = {0, 0},
  .offset = {
    offsetof(struct shared_resource_table, vdev),
#if defined (__LOG_TRACE_IO_)
    offsetof(struct shared_resource_table, cm_trace),
#endif
  },

  /* Virtio device entry */
  .vdev= {
    RSC_VDEV, VIRTIO_ID_RPMSG, 0, RPMSG_VIRTIO_FEATURE, 0, 0, 0,
    VRING_COUNT, {0, 0},
  },

  /* Vring rsc entry - part of vdev rsc entry */
  .vring0 = {VRING_TX_ADDRESS, VRING_ALIGNMENT, VRING_NUM_BUFFS, VRING0_ID, 0},
  .vring1 = {VRING_RX_ADDRESS, VRING_ALIGNMENT, VRING_NUM_BUFFS, VRING1_ID, 0},

#if defined (__LOG_TRACE_IO_)
	.cm_trace = {
		RSC_TRACE,
		(uint32_t)system_log_buf, SYSTEM_TRACE_BUF_SZ, 0, "cm33_log",
	},
#endif /* defined (__LOG_TRACE_IO_) */
};

void resource_table_init(int RPMsgRole, void **table_ptr, int *length)
{
  (void)RPMsgRole;
  *length = sizeof(resource_table);
#if !defined(LINUX_RPROC_MASTER)

  /* make a copy the resource table in shared memory that will be parsed by the Linux remoteproc*/
  memcpy(RSCTAB_START_ADDRESS, &resource_table, *length);
  *table_ptr = RSCTAB_START_ADDRESS;
#else
  /* Directly provide the resource table address stored on link in .resource_table section */
  *table_ptr =  (void *)&resource_table;
#endif
}

#elif (OPENAMP_ROLE == RPMSG_HOST)
void resource_table_init(int RPMsgRole, void **table_ptr, int *length)
{
  /* USER CODE BEGIN PRE_RSC_TABLE_INIT */
	  /* USER CODE END PRE_RSC_TABLE_INIT */

#if defined(RSC_TABLE_USE_BCKUP_REG)
  /*
   * Get the resource table address and length from backup registers to provide
   * resource table information to the Cortex A
   */
  *table_ptr = (void *)HAL_RTCEx_BKUPRead(NULL, OPENAMP_RSCTAB_ADDR_BUP_IDX);
  *length = HAL_RTCEx_BKUPRead(NULL, OPENAMP_RSCTAB_LGTH_BUP_IDX);
#else
  (void)RPMsgRole;

  /*
   * Use the linker-defined resource table region when backup-register-based
   * handoff is not enabled.
   */
  *table_ptr = RSCTAB_START_ADDRESS;
  *length = (int)RSCTAB_SIZE;
#endif
}
#else
#error "OPENAMP_ROLE must be set"
#endif
