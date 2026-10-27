/**
  ******************************************************************************
  * @file    mmu_stm32mp2xx.h
  * @author  MCD Application Team
  * @brief   This file contains MMU AArch32 short-descriptor types, attributes
  *          and APIs to build translation tables.
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
#ifndef STM32MP2XX_MMU_H
#define STM32MP2XX_MMU_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "stm32mp2xx.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/

/* Section descriptor type, bits [1:0] = 0b10 */
#define SECTION_DESC          (0x2U)

/* Coarse page table descriptor type, level 1 entry bits [1:0] = 0b01 */
#define COARSE_TABLE_DESC     (0x1U)

/* Small page descriptor type, level 2 entry bits [1:0] = 0b10 */
#define SMALL_PAGE_DESC       (0x2U)

/* Attribute bits for short-descriptor sections */
#define SECTION_BUFFERABLE    (1U << 2)
#define SECTION_CACHEABLE     (1U << 3)
#define SECTION_XN            (1U << 4)
#define SECTION_DOMAIN0       (0U << 5)
#define SECTION_AP_PRW_UNO    (1U << 10)

/* TEX bits for section descriptors */
#define SECTION_TEX0          (1U << 12)
#define SECTION_TEX1          (1U << 13)
#define SECTION_TEX2          (1U << 14)

/* Shareable bit */
#define SECTION_SHAREABLE     (1U << 16)

/* Non-secure bits */
#define SECTION_NS            (1U << 19)
#define PAGETABLE_NS          (1U << 3)

/* Normal, Inner/Outer WBWA, shareable, TEX=001, C=1, B=1, S=1 */
#define ATTR_NORMAL_WBWA_SHARED  (SECTION_TEX0       | \
                                  SECTION_CACHEABLE  | \
                                  SECTION_BUFFERABLE | \
                                  SECTION_SHAREABLE)

/* Normal, non-cacheable, shareable, TEX=001, C=0, B=0, S=1 */
#define ATTR_NORMAL_NONCACHEABLE_SHARED  (SECTION_TEX0 | \
                                           SECTION_SHAREABLE)

/* Device, non-shareable, TEX=000, C=0, B=1, S=0 */
#define ATTR_DEVICE_NSHARED      (SECTION_BUFFERABLE)

/* Device, shareable, TEX=000, C=0, B=1, S=1 */
#define ATTR_DEVICE_SHARED       (SECTION_SHAREABLE)

/**
  * @brief MMU memory attribute templates.
  */
typedef enum
{
  MEM_ATTR_NORMAL_WBWA_SHARED         = ATTR_NORMAL_WBWA_SHARED,
  MEM_ATTR_NORMAL_NONCACHEABLE_SHARED = ATTR_NORMAL_NONCACHEABLE_SHARED,
  MEM_ATTR_DEVICE_NSHARED             = ATTR_DEVICE_NSHARED,
  MEM_ATTR_DEVICE_SHARED              = ATTR_DEVICE_SHARED,
} mem_attr_t;

/**
  * @brief MMU mapping granularity.
  */
typedef enum
{
  MMU_GRANULARITY_SECTION_1M = 0,
  MMU_GRANULARITY_PAGE4K     = 1,
} mmu_granularity_t;

/**
  * @brief MMU region descriptor.
  */
typedef struct
{
  uint32_t           va_start;
  uint32_t           pa_start;
  uint32_t           size;
  mem_attr_t         attrs;
  uint8_t            xn;
  uint8_t            ns;
  const char        *name;
  mmu_granularity_t  granularity;
} mmu_region_t;

/* Exported functions --------------------------------------------------------*/
void MMU_CreateTranslationTable(void);

#ifdef __cplusplus
}
#endif

#endif /* STM32MP2XX_MMU_H */
