/**
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#include "stm32mp2xx_hal_ddr_ddrphy_phyinit.h"

/*
 * Writes local memory content into the SRAM via APB interface.
 *
 * This function issued APB writes commands to SRAM address based on values
 * stored in a local PhyInit array that contains consolidated IMEM and DMEM
 * data.
 * @param[in] mem[] Local memory array.
 * @param[in] mem_offset offset index. if provided, skips to the offset index
 * from the local array and issues APB commands from mem_offset to mem_size.
 * @param[in] mem_size size of the memroy (in mem array index)
 * @returns void
 */
void ddrphy_phyinit_writeoutmem(const uint32_t *mem, uint32_t mem_offset, int32_t mem_size)
{
  uint32_t index;

  /*
   * 1. Enable access to the internal CSRs by setting the MicroContMuxSel CSR to 0.
   *    This allows the memory controller unrestricted access to the configuration CSRs.
   */
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (TAPBONLY | CSR_MICROCONTMUXSEL_ADDR))), 0x0U);

  for (index = 0; index < ((uint32_t)mem_size / sizeof(uint32_t)); index++)
  {
    uint32_t data = mem[index];

#ifdef USE_STM32MP257CXX_EMU
    mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * ((index * 2U) + mem_offset))),
                  (uint16_t)((data >> 16) & 0xFFFFU));
    mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * ((index * 2U) + 1U + mem_offset))),
                  (uint16_t)(data & 0xFFFFU));
#else /* USE_STM32MP257CXX_EMU */
    mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * ((index * 2U) + mem_offset))),
                  (uint16_t)(data & 0xFFFFU));
    mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * ((index * 2U) + 1U + mem_offset))),
                  (uint16_t)((data >> 16) & 0xFFFFU));
#endif /* USE_STM32MP257CXX_EMU */
  }

  /*
   * 2. Isolate the APB access from the internal CSRs by setting the MicroContMuxSel CSR to 1.
   *    This allows the firmware unrestricted access to the configuration CSRs.
   */
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (TAPBONLY | CSR_MICROCONTMUXSEL_ADDR))), 0x1U);
}

/*
 * Similar function for message block
 */
void ddrphy_phyinit_writeoutmsgblk(uint16_t *mem, uint32_t mem_offset, int32_t mem_size)
{
  uint32_t index;

  /*
   * 1. Enable access to the internal CSRs by setting the MicroContMuxSel CSR to 0.
   *    This allows the memory controller unrestricted access to the configuration CSRs.
   */
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (TAPBONLY | CSR_MICROCONTMUXSEL_ADDR))), 0x0U);


  for (index = 0; index < ((uint32_t)mem_size / sizeof(uint16_t)); index++)
  {
    mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (index + mem_offset))), mem[index]);
  }

  /*
   * 2. Isolate the APB access from the internal CSRs by setting the MicroContMuxSel CSR to 1.
   *    This allows the firmware unrestricted access to the configuration CSRs.
   */
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (TAPBONLY | CSR_MICROCONTMUXSEL_ADDR))), 0x1U);
}
