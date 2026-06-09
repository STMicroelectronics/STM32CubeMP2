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
 * Copyright (c) 2025 STMicroelectronics.
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

#if defined(__ICCARM__) || defined (__CC_ARM)
#include <stddef.h> /* needed  for offsetof definition*/
#endif
#include "stm32mp2xx_hal.h"
#include "rsc_table.h"
#include "openamp/open_amp.h"

#define RPMSG_VIRTIO_FEATURE     1
#define VRING_COUNT               2

/* VirtIO rpmsg device id */
#define VIRTIO_ID_RPMSG_          7

#if defined (__ICCARM__)
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

/* Since GCC is not initializing the resource_table at startup, it is declared as volatile to avoid compiler optimization
 * for the CM33 (see resource_table_init() below)
 */
const struct shared_resource_table resource_table = {

  .version = 1,
  .num = 1,
  .reserved = {0, 0},
  .offset = {
    offsetof(struct shared_resource_table, vdev),
  },

  /* Virtio device entry */
  .vdev= {
    RSC_VDEV, VIRTIO_ID_RPMSG_, 0, RPMSG_VIRTIO_FEATURE, 0, 0, 0,
    VRING_COUNT, {0, 0},
  },

  /* Vring rsc entry - part of vdev rsc entry */
  .vring0 = {VRING_TX_ADDRESS, VRING_ALIGNMENT, VRING_NUM_BUFFS, VRING0_ID, 0},
  .vring1 = {VRING_RX_ADDRESS, VRING_ALIGNMENT, VRING_NUM_BUFFS, VRING1_ID, 0},

} ;

void resource_table_init(int RPMsgRole, void **table_ptr, int *length)
{

  *length = sizeof(resource_table);
  *table_ptr = RSCTAB_START_ADDRESS;

  /* make a copy the resource table in shared memory that will be parsed by the Linux remoteproc*/
  memcpy(RSCTAB_START_ADDRESS, &resource_table, *length);
}
