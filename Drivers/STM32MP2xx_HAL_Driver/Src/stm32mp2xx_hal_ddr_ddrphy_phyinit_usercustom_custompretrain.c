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

#include <stdlib.h>

#include "stm32mp2xx_hal_ddr_ddrphy_phyinit_usercustom.h"
#include "stm32mp2xx_hal_ddr_ddrphy_phyinit.h"

/* DDRDBG registers */
#define DDRDBG_DDR34_AC_SWIZZLE_ADD3_0 0x100U

/*
 * This function is called before training firmware is executed. Any
 * register override in this function might affect the firmware training
 * results.
 *
 * This function is executed before firmware execution loop. Thus this function
 * should be used only for the following:
 *
 *  - Override PHY register values written by
 *  ddrphy_phyinit_c_initphyconfig.  An example use case is when this
 *  function does not perform the exact programing desired by the user.
 *  - Write custom PHY registers that need to take effect before training
 *  firmware execution.
 *
 * User shall use mmio_write_16 to write PHY registers in order for the register
 * to be tracked by PhyInit for retention restore.
 *
 * To override settings in the message block, users can assign values to the
 * fields in the message block data structure directly
 *
 * \ref examples/simple/ddrphy_phyinit_usercustom_custompretrain.c example of this function
 *
 * @return Void
 */
void ddrphy_phyinit_usercustom_pretrain(void)
{
  VERBOSE("%s Start\n", __func__);

#ifdef USE_STM32MP257CXX_EMU
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB0 | P0 | B0 | TDBYTE |
                                                  CSR_TXODTDRVSTREN_ADDR))),
                0x0U);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB0 | P0 | B1 | TDBYTE |
                                                  CSR_TXODTDRVSTREN_ADDR))),
                0x0U);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB1 | P0 | B0 | TDBYTE |
                                                  CSR_TXODTDRVSTREN_ADDR))),
                0x0U);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB1 | P0 | B1 | TDBYTE |
                                                  CSR_TXODTDRVSTREN_ADDR))),
                0x0U);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB2 | P0 | B0 | TDBYTE |
                                                  CSR_TXODTDRVSTREN_ADDR))),
                0x0U);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB2 | P0 | B1 | TDBYTE |
                                                  CSR_TXODTDRVSTREN_ADDR))),
                0x0U);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB3 | P0 | B0 | TDBYTE |
                                                  CSR_TXODTDRVSTREN_ADDR))),
                0x0U);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB3 | P0 | B1 | TDBYTE |
                                                  CSR_TXODTDRVSTREN_ADDR))),
                0x0U);

#if STM32MP_LPDDR4_TYPE
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB0 | P0 | B0 | TDBYTE |
                                                  CSR_DQDQSRCVCNTRL_ADDR))),
                0xB22U);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB0 | P0 | B1 | TDBYTE |
                                                  CSR_DQDQSRCVCNTRL_ADDR))),
                0xB22U);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB1 | P0 | B0 | TDBYTE |
                                                  CSR_DQDQSRCVCNTRL_ADDR))),
                0xB22U);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB1 | P0 | B1 | TDBYTE |
                                                  CSR_DQDQSRCVCNTRL_ADDR))),
                0xB22U);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB2 | P0 | B0 | TDBYTE |
                                                  CSR_DQDQSRCVCNTRL_ADDR))),
                0xB22U);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB2 | P0 | B1 | TDBYTE |
                                                  CSR_DQDQSRCVCNTRL_ADDR))),
                0xB22U);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB3 | P0 | B0 | TDBYTE |
                                                  CSR_DQDQSRCVCNTRL_ADDR))),
                0xB22U);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB3 | P0 | B1 | TDBYTE |
                                                  CSR_DQDQSRCVCNTRL_ADDR))),
                0xB22U);

  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB0 | TDBYTE | CSR_DQDQSRCVCNTRL1_ADDR))),
                0x400U);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB1 | TDBYTE | CSR_DQDQSRCVCNTRL1_ADDR))),
                0x400U);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB2 | TDBYTE | CSR_DQDQSRCVCNTRL1_ADDR))),
                0x400U);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB3 | TDBYTE | CSR_DQDQSRCVCNTRL1_ADDR))),
                0x400U);
#endif /* STM32MP_LPDDR4_TYPE */
#else /* USE_STM32MP257CXX_EMU */
  uint32_t __unused byte;
  uint32_t i = 0U;
  uint32_t j;
  uintptr_t base;

#if STM32MP_DDR3_TYPE || STM32MP_DDR4_TYPE
  base = (uintptr_t)(DDRPHYC_BASE + (4U * (TMASTER | CSR_HWTSWIZZLEHWTADDRESS0_ADDR)));

  for (i = 0U; i < NB_HWT_SWIZZLE; i++)
  {
    mmio_write_16(base + i * sizeof(uint32_t), (uint16_t)userinputswizzle.swizzle[i]);
  }

  base = (uintptr_t)(DDRDBG_BASE + DDRDBG_DDR34_AC_SWIZZLE_ADD3_0);

  for (j = 0U; j < NB_AC_SWIZZLE; j++)
  {
    mmio_write_32(base + j * sizeof(uint32_t), userinputswizzle.swizzle[i]);
    i++;
  }
#else
  for (byte = 0U; byte < (uint32_t)userinputbasic.numdbyte; byte++)
  {
    base = (uintptr_t)(DDRPHYC_BASE + (4U * ((byte << 12) | TDBYTE | CSR_DQ0LNSEL_ADDR)));

    for (j = 0U; j < NB_DQLNSEL_SWIZZLE_PER_BYTE; j++)
    {
      mmio_write_16(base + j * sizeof(uint32_t), (uint16_t)userinputswizzle.swizzle[i]);
      i++;
    }
  }

  base = (uintptr_t)(DDRPHYC_BASE + (4U * (TMASTER | CSR_MAPCAA0TODFI_ADDR)));

  for (j = 0U; j < NB_MAPCAATODFI_SWIZZLE; j++)
  {
    mmio_write_16(base + j * sizeof(uint32_t), (uint16_t)userinputswizzle.swizzle[i]);
    i++;
  }

  base = (uintptr_t)(DDRPHYC_BASE + (4U * (TMASTER | CSR_MAPCAB0TODFI_ADDR)));

  for (j = 0U; j < NB_MAPCABTODFI_SWIZZLE; j++)
  {
    mmio_write_16(base + j * sizeof(uint32_t), (uint16_t)userinputswizzle.swizzle[i]);
    i++;
  }
#endif /* STM32MP_DDR3_TYPE || STM32MP_DDR4_TYPE */
#endif /* USE_STM32MP257CXX_EMU */

  VERBOSE("%s End\n", __func__);
}
