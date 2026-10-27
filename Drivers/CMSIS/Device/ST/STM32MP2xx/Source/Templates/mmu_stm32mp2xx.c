/**
  ******************************************************************************
  * @file    mmu_stm32mp2xx.c
  * @author  MCD Application Team
  * @brief   CMSIS Cortex-A35 AArch32 short-descriptor MMU setup source file.
  *
  * This file builds the level-1 translation table and any required level-2
  * coarse page tables from the project supplied MMU_REGION_TABLE.
  *
  * To use this template:
  * - include mmu_stm32mp2xx.h and define MMU_REGION_TABLE in stm32mp2xx_mmu_conf.h
  * - describe each region with virtual address, physical address, size,
  *   attributes, execute-never, security state and granularity
  * - keep section regions aligned to 1 Mbyte and page regions aligned to 4 Kbytes
  * - call MMU_CreateTranslationTable() from SystemInit() before MMU_Enable()
  *
  * The current implementation supports AArch32 short descriptors with TTBR0
  * covering the full 4 Gbyte virtual address space, domain 0 descriptors,
  * 1 Mbyte section mappings and 4 Kbyte small-page mappings. It is common to
  * STM32MP2xx projects; each project owns its region table in stm32mp2xx_mmu_conf.h.
  *
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* L1 cache information and CCSIR restrictions:
 * - Write-through support is not available
 * - Write-back support is available
 * - Read allocation support is available
 * - Write allocation support is available
 */

/* Use the shareable attribute carefully. For cores without coherency logic, such as
 * SCU, marking a region as shareable forces the processor to avoid caching that
 * region regardless of the inner cache settings. Cortex-A RTX versions use
 * LDREX/STREX instructions that rely on local monitors. Local monitors are used
 * only when the region is cached; non-cacheable regions use the global monitor.
 * Some Cortex-A implementations do not include global monitors, so a wrong
 * shareable attribute might cause STREX to fail.
 */

/* When the shareable attribute is applied to a memory region that is not
 * write-back normal memory, data held in this region is treated as non-cacheable.
 * When SMP bit is 0, inner WB/WA cacheable shareable attributes are treated as
 * non-cacheable. When SMP bit is 1, inner WB/WA cacheable shareable attributes
 * are treated as cacheable.
 */

/* Expected MMU configuration:
 * - SCTLR.AFE is 1, AP[2:1] define access permissions and AP[0] is an access flag
 * - SCTLR.TRE is 0, TEX remap is disabled and descriptor bits describe attributes
 * - Domain 0 is the client domain
 * - Descriptors place all memory in domain 0
 */

#include "stm32mp2xx.h"
#include "mmu_stm32mp2xx.h"
#include "stm32mp2xx_mmu_conf.h"

#define SECTION_SHIFT   20U
#define SECTION_SIZE    (1U << SECTION_SHIFT)    /* 1MB */
#define L1_ENTRY_COUNT  4096U

#define SMALL_PAGE_SHIFT  12U
#define SMALL_PAGE_SIZE   (1U << SMALL_PAGE_SHIFT)  /* 4KB */

#define MMU_MAX_COARSE_TABLES  32U /* max number of 4KB regions */


/* =========================================================================
 *  L1 Translation Table
 * =========================================================================
 *  4 GB / 1 MB = 4096 entries, 16 KB, 16 KB aligned
 */
__attribute__((aligned(16384)))
static uint32_t L1_TT[4096];

static const mmu_region_t mmu_regions[] = MMU_REGION_TABLE;

static const uint32_t mmu_regions_count =
  sizeof(mmu_regions) / sizeof(mmu_regions[0]);

/* =========================================================================
 *  Coarse Page Tables for 4KB pages
 * =========================================================================
 *
 * Each coarse table:
 *   - 1KB (256 entries * 4 bytes)
 *   - Covers 1MB of virtual address space: 256 * 4KB
 *
 * L1 entry points to a coarse table when using small pages.
 */

__attribute__((aligned(1024)))
static uint32_t L2_CoarseTables[MMU_MAX_COARSE_TABLES][256];

static uint32_t CoarseTableCount = 0U;

/* Utility: allocate a new coarse page table, return its index or -1 */
static int32_t MMU_AllocateCoarseTable(void)
{
  uint32_t index;

  if (CoarseTableCount >= MMU_MAX_COARSE_TABLES)
  {
    return -1;
  }

  index = CoarseTableCount++;
  /* Clear entries */
  for (uint32_t i = 0U; i < 256U; ++i)
  {
    L2_CoarseTables[index][i] = 0U;
  }

  return (int32_t)index;
}

/* =========================================================================
 *  Helpers for Descriptors and L1 TT Construction
 * =========================================================================
 */

/* Section descriptor creation */
static inline uint32_t MMU_MakeSectionDesc(uint32_t phys, uint32_t attrs, uint8_t xn, uint8_t ns)
{
  uint32_t desc = 0U;

  desc |= (phys & 0xFFF00000U); /* base address [31:20] */
  desc |= SECTION_DESC;         /* section descriptor    */
  desc |= SECTION_DOMAIN0;
  desc |= SECTION_AP_PRW_UNO;   /* privileged RW, user NO */
  desc |= attrs;

  if (xn != 0U)
  {
    desc |= SECTION_XN;
  }

  if (ns != 0U)
  {
    desc |= SECTION_NS;
  }

  return desc;
}

/* Small page descriptor creation (simplified, same attributes semantics) */
static inline uint32_t MMU_MakeL2SmallPageDesc(uint32_t phys, uint32_t attrs, uint8_t xn)
{
  uint32_t desc = 0U;

  /* Base address [31:12] */
  desc |= (phys & 0xFFFFF000U);

  /* Small page descriptor */
  desc |= SMALL_PAGE_DESC;  /* bits[1:0] = 10 */

  /* XN for small pages is encoded in bit[0] (descriptor type becomes 0b11). */
  if (xn != 0U)
  {
    desc |= 1U;
  }

  /* AP[2:0] = 0b001: Priv RW, User NO
   * AP0 -> bit4 = 1
   * AP1 -> bit5 = 0
   * AP2 -> bit9 = 0
   */
  desc |= (1U << 4);  /* AP0=1 */
  /* C/B bits share the same position for sections and small pages (bits[3:2]). */
  desc |= attrs & (SECTION_CACHEABLE | SECTION_BUFFERABLE);

  /* TEX bits differ between section and small page descriptors:
   * - section TEX[2:0] are bits[14:12]
   * - small page TEX[2:0] are bits[8:6]
   * Keep the public attrs semantics (SECTION_TEXx) but map to the correct positions.
   */
  if ((attrs & SECTION_TEX0) != 0U)
  {
    desc |= (1U << 6);
  }
  if ((attrs & SECTION_TEX1) != 0U)
  {
    desc |= (1U << 7);
  }
  if ((attrs & SECTION_TEX2) != 0U)
  {
    desc |= (1U << 8);
  }

  /* For small page, S bit is 10; SECTION_SHAREABLE also uses bit 16 for sections,
     so we cannot reuse it directly. We must set S explicitly for pages. */
  if ((attrs & SECTION_SHAREABLE) != 0U)
  {
    desc |= (1U << 10);  /* S=1 */
  }

  return desc;
}

/* L1 coarse table descriptor */
static inline uint32_t MMU_MakeCoarseTableDesc(uint32_t l2_phys_addr, uint8_t ns)
{
  uint32_t desc = 0U;

  desc |= (l2_phys_addr & 0xFFFFFC00U);
  desc |= COARSE_TABLE_DESC;  /* bits[1:0]=01 */

  if (ns != 0U)
  {
    desc |= PAGETABLE_NS;
  }

  return desc;
}

static void MMU_ClearL1Table(void)
{
  for (uint32_t i = 0U; i < L1_ENTRY_COUNT; ++i)
  {
    L1_TT[i] = 0U;
  }
  /* Also reset coarse tables counter */
  CoarseTableCount = 0U;
}

/* Ensure an L1 entry for 'va' points to a coarse table, return L2 table pointer */
static uint32_t *MMU_GetOrCreateCoarseTable(uint32_t va, uint8_t ns)
{
  uint32_t l1_index = va >> SECTION_SHIFT;
  uint32_t desc = L1_TT[l1_index];
  uint32_t *l2_va;
  uint32_t  l2_pa;
  int32_t table_index;

  if ((desc & 0x3U) == COARSE_TABLE_DESC)
  {
    /* Already a coarse table; return its base */
    l2_pa = desc & 0xFFFFFC00U;
    return (uint32_t *)(uintptr_t)l2_pa;
  }

  /* Need to allocate a new coarse table */
  table_index = MMU_AllocateCoarseTable();
  if (table_index < 0)
  {
    /* Out of tables; in product code, assert or handle error */
    return (uint32_t *)0;
  }

  l2_va = &L2_CoarseTables[table_index][0];
  l2_pa = (uint32_t)(uintptr_t)l2_va;  /* flat mapping assumed */

  L1_TT[l1_index] = MMU_MakeCoarseTableDesc(l2_pa, ns);

  return l2_va;
}

static void MMU_BuildFromRegions(void)
{
  uint32_t num_sections;
  uint32_t num_pages;
  uint32_t index;
  uint32_t va;
  uint32_t pa;

  MMU_ClearL1Table();

  for (uint32_t region_index = 0U; region_index < mmu_regions_count; ++region_index)
  {
    const mmu_region_t *reg = &mmu_regions[region_index];

    va = reg->va_start;
    pa = reg->pa_start;

    if (reg->granularity == MMU_GRANULARITY_SECTION_1M)
    {
      num_sections = reg->size >> SECTION_SHIFT;

      /* --- 1MB Section mapping --- */

      if ((reg->va_start & (SECTION_SIZE - 1U)) != 0U ||
          (reg->pa_start & (SECTION_SIZE - 1U)) != 0U ||
          (reg->size     & (SECTION_SIZE - 1U)) != 0U)
      {
        /* Misaligned; skip or assert in production */
        continue;
      }

      for (uint32_t n = 0U; n < num_sections; ++n)
      {
        index = va >> SECTION_SHIFT;
        if (index >= L1_ENTRY_COUNT)
        {
          break; /* outside 4GB VA */
        }

        L1_TT[index] = MMU_MakeSectionDesc(pa, reg->attrs, reg->xn, reg->ns);

        va += SECTION_SIZE;
        pa += SECTION_SIZE;
      }

    }
    else
    {
      num_pages = reg->size >> SMALL_PAGE_SHIFT;

      /* --- 4KB Small page mapping --- */
      if ((reg->va_start & (SMALL_PAGE_SIZE - 1U)) != 0U ||
          (reg->pa_start & (SMALL_PAGE_SIZE - 1U)) != 0U ||
          (reg->size     & (SMALL_PAGE_SIZE - 1U)) != 0U)
      {
        /* Misaligned; skip or assert in production */
        continue;
      }

      for (uint32_t n = 0U; n < num_pages; ++n)
      {
        uint32_t *l2_table = MMU_GetOrCreateCoarseTable(va, reg->ns);
        /* L2 index: bits[19:12] select 4KB within the 1MB block */
        index = (va >> SMALL_PAGE_SHIFT) & 0xFFU;

        if (l2_table == (uint32_t *)0)
        {
          break; /* no L2 table available */
        }

        l2_table[index] = MMU_MakeL2SmallPageDesc(pa, reg->attrs, reg->xn);

        va += SMALL_PAGE_SIZE;
        pa += SMALL_PAGE_SIZE;
      }
    }
  }
}

/* =========================================================================
 *  CP15 / System Control Helpers (AArch32)
 * =========================================================================
 */

static inline void MMU_SetTTBCR(uint32_t val)
{
  __asm__ volatile(
    "mcr p15, 0, %0, c2, c0, 2\n"
    "isb\n"
    :
    : "r"(val)
    : "memory");
}

static void MMU_InvalidateCachesAndTLB(void)
{
  uint32_t zero = 0U;

  /* Invalidate entire I-cache */
  __asm__ volatile(
    "mcr p15, 0, %0, c7, c5, 0\n"
    :
    : "r"(zero)
    : "memory");

  /* Optional: Invalidate entire D-cache (uncomment if needed) */
  /*
  __asm__ volatile (
      "mcr p15, 0, %0, c7, c6, 0\n"
      :
      : "r"(zero)
      : "memory");
  */

  /* Invalidate entire unified TLB */
  __set_TLBIALL(0);

  __DSB();
  __ISB();
}

/* =========================================================================
 *  Public MMU Init API
 * =========================================================================
 */

/**
    * @brief  Create translation tables and program MMU translation registers.
    * @note   Enabling MMU/caches/branch prediction is optional and currently
    *         disabled in this template (see the #if 0 block).
    * @retval None
    */
void MMU_CreateTranslationTable(void)
{
  uint32_t ttbr0 = (uint32_t)L1_TT;
  uint32_t sctlr;

  /* 1) Disable MMU, caches, branch prediction */
  sctlr = __get_SCTLR();
  sctlr &= ~((1U << 0)   |    /* M: MMU */
             (1U << 2)   |   /* C: D-cache */
             (1U << 11)  |   /* Z: branch prediction */
             (1U << 12)  |   /* I: cache */
             (1U << 28));    /* TRE set to 0 */
  __set_SCTLR(sctlr);
  __ISB();

  /* 2) Initialize PRRR/NMRR for TEX/C/B scheme */


  /* 3) Build L1 and L2 tables from region table */
  MMU_BuildFromRegions();

  /* 4) TTBR0: base address of L1_TT (physical address in secure SRAM) */
  __set_TTBR0(ttbr0);
  __ISB();

  /* 5) TTBCR: N = 0 => all VA space via TTBR0 */
  MMU_SetTTBCR(0U);
  __ISB();

  /* 6) DACR: domain 0 as Manager (0b11), others 0 */
  __set_DACR(0x3U);
  __ISB();

  /* 7) Invalidate caches & TLB */
  MMU_InvalidateCachesAndTLB();

}

