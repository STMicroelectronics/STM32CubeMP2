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

#include "stm32mp2xx_hal_ddr_ddrphy_phyinit_usercustom.h"

/*
 * This function is called after the execution of training firmware is
 * complete. The user can use this function to override any CSR value programmed
 * by PhyInit or training firmware.
 *
 * The purpose of this function is to override any register values programmed
 * by the training firmware or ddrphy_phyinit_progcsrskiptrain() when
 * skip_train is used. This function is executed after training firmware has
 * completed.
 *
 * \note **Warning!**
 * This function must not override any values in userinputbasic,
 * userinputadvanced or message block data structures.  Only PHY register values
 * are to be modified with this function. To override PHY register values
 * programmed by PhyInit, users must use mmio_write_16() function calls placed
 * within this function.
 *
 * Sequence of Events in this function must be:
 * -# Enable APB access
 * -# Issue register writes
 * -# Isolate APB access
 *
 * A \ref examples/simple/ddrphy_phyinit_usercustom_customposttrain.c example of this function
 *
 * \return void
 */
void ddrphy_phyinit_usercustom_posttrain(void)
{
  VERBOSE("%s Start\n", __func__);

#ifdef USE_STM32MP257CXX_EMU
#if STM32MP_DDR3_TYPE
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (TAPBONLY | CSR_MICROCONTMUXSEL_ADDR))), 0x0U);

  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB0 | P0 | U0 | TDBYTE | CSR_TXDQSDLYTG0_ADDR))),
                0x118U);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB0 | P0 | U1 | TDBYTE | CSR_TXDQSDLYTG0_ADDR))),
                0x118U);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB1 | P0 | U0 | TDBYTE | CSR_TXDQSDLYTG0_ADDR))),
                0x118U);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB1 | P0 | U1 | TDBYTE | CSR_TXDQSDLYTG0_ADDR))),
                0x118U);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB2 | P0 | U0 | TDBYTE | CSR_TXDQSDLYTG0_ADDR))),
                0x118U);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB2 | P0 | U1 | TDBYTE | CSR_TXDQSDLYTG0_ADDR))),
                0x118U);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB3 | P0 | U0 | TDBYTE | CSR_TXDQSDLYTG0_ADDR))),
                0x118U);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB3 | P0 | U1 | TDBYTE | CSR_TXDQSDLYTG0_ADDR))),
                0x118U);

  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB0 | P0 | R0 | TDBYTE | CSR_TXDQDLYTG0_ADDR))),
                0x10U);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB0 | P0 | R1 | TDBYTE | CSR_TXDQDLYTG0_ADDR))),
                0x10U);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB0 | P0 | R2 | TDBYTE | CSR_TXDQDLYTG0_ADDR))),
                0x10U);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB0 | P0 | R3 | TDBYTE | CSR_TXDQDLYTG0_ADDR))),
                0x10U);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB0 | P0 | R4 | TDBYTE | CSR_TXDQDLYTG0_ADDR))),
                0x10U);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB0 | P0 | R5 | TDBYTE | CSR_TXDQDLYTG0_ADDR))),
                0x10U);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB0 | P0 | R6 | TDBYTE | CSR_TXDQDLYTG0_ADDR))),
                0x10U);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB0 | P0 | R7 | TDBYTE | CSR_TXDQDLYTG0_ADDR))),
                0x10U);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB0 | P0 | R8 | TDBYTE | CSR_TXDQDLYTG0_ADDR))),
                0x10U);

  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB1 | P0 | R0 | TDBYTE | CSR_TXDQDLYTG0_ADDR))),
                0x10U);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB1 | P0 | R1 | TDBYTE | CSR_TXDQDLYTG0_ADDR))),
                0x10U);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB1 | P0 | R2 | TDBYTE | CSR_TXDQDLYTG0_ADDR))),
                0x10U);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB1 | P0 | R3 | TDBYTE | CSR_TXDQDLYTG0_ADDR))),
                0x10U);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB1 | P0 | R4 | TDBYTE | CSR_TXDQDLYTG0_ADDR))),
                0x10U);

  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB1 | P0 | R5 | TDBYTE | CSR_TXDQDLYTG0_ADDR))),
                0x10U);

  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB1 | P0 | R6 | TDBYTE | CSR_TXDQDLYTG0_ADDR))),
                0x10U);

  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB1 | P0 | R7 | TDBYTE | CSR_TXDQDLYTG0_ADDR))),
                0x10U);

  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB1 | P0 | R8 | TDBYTE | CSR_TXDQDLYTG0_ADDR))),
                0x10U);


  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB2 | P0 | R0 | TDBYTE | CSR_TXDQDLYTG0_ADDR))),
                0x10U);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB2 | P0 | R1 | TDBYTE | CSR_TXDQDLYTG0_ADDR))),
                0x10U);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB2 | P0 | R2 | TDBYTE | CSR_TXDQDLYTG0_ADDR))),
                0x10U);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB2 | P0 | R3 | TDBYTE | CSR_TXDQDLYTG0_ADDR))),
                0x10U);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB2 | P0 | R4 | TDBYTE | CSR_TXDQDLYTG0_ADDR))),
                0x10U);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB2 | P0 | R5 | TDBYTE | CSR_TXDQDLYTG0_ADDR))),
                0x10U);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB2 | P0 | R6 | TDBYTE | CSR_TXDQDLYTG0_ADDR))),
                0x10U);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB2 | P0 | R7 | TDBYTE | CSR_TXDQDLYTG0_ADDR))),
                0x10U);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB2 | P0 | R8 | TDBYTE | CSR_TXDQDLYTG0_ADDR))),
                0x10U);

  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB3 | P0 | R0 | TDBYTE | CSR_TXDQDLYTG0_ADDR))),
                0x10U);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB3 | P0 | R1 | TDBYTE | CSR_TXDQDLYTG0_ADDR))),
                0x10U);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB3 | P0 | R2 | TDBYTE | CSR_TXDQDLYTG0_ADDR))),
                0x10U);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB3 | P0 | R3 | TDBYTE | CSR_TXDQDLYTG0_ADDR))),
                0x10U);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB3 | P0 | R4 | TDBYTE | CSR_TXDQDLYTG0_ADDR))),
                0x10U);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB3 | P0 | R5 | TDBYTE | CSR_TXDQDLYTG0_ADDR))),
                0x10U);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB3 | P0 | R6 | TDBYTE | CSR_TXDQDLYTG0_ADDR))),
                0x10U);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB3 | P0 | R7 | TDBYTE | CSR_TXDQDLYTG0_ADDR))),
                0x10U);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB3 | P0 | R8 | TDBYTE | CSR_TXDQDLYTG0_ADDR))),
                0x10U);


  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (TAPBONLY | CSR_MICROCONTMUXSEL_ADDR))), 0x1U);
#elif STM32MP_LPDDR4_TYPE
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (TAPBONLY | CSR_MICROCONTMUXSEL_ADDR))), 0x0U);

  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB0 | P0 | U0 | TDBYTE | CSR_RXENDLYTG0_ADDR))),
                0x2C0U);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB0 | P0 | U1 | TDBYTE | CSR_RXENDLYTG0_ADDR))),
                0x2C0U);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB1 | P0 | U0 | TDBYTE | CSR_RXENDLYTG0_ADDR))),
                0x2C0U);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB1 | P0 | U1 | TDBYTE | CSR_RXENDLYTG0_ADDR))),
                0x2C0U);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB2 | P0 | U0 | TDBYTE | CSR_RXENDLYTG0_ADDR))),
                0x2C0U);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB2 | P0 | U1 | TDBYTE | CSR_RXENDLYTG0_ADDR))),
                0x2C0U);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB3 | P0 | U0 | TDBYTE | CSR_RXENDLYTG0_ADDR))),
                0x2C0U);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB3 | P0 | U1 | TDBYTE | CSR_RXENDLYTG0_ADDR))),
                0x2C0U);

  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB0 | P0 | U0 | TDBYTE | CSR_RXPBDLYTG0_ADDR))),
                0x7FU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB0 | P0 | U1 | TDBYTE | CSR_RXPBDLYTG0_ADDR))),
                0x7FU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB0 | P0 | (U1 * 2U) | TDBYTE |
                                                CSR_RXPBDLYTG0_ADDR))),
                0x7FU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB0 | P0 | (U1 * 3U) | TDBYTE |
                                                CSR_RXPBDLYTG0_ADDR))),
                0x7FU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB0 | P0 | (U1 * 4U) | TDBYTE |
                                                CSR_RXPBDLYTG0_ADDR))),
                0x7FU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB0 | P0 | (U1 * 5U) | TDBYTE |
                                                CSR_RXPBDLYTG0_ADDR))),
                0x7FU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB0 | P0 | (U1 * 6U) | TDBYTE |
                                                CSR_RXPBDLYTG0_ADDR))),
                0x7FU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB0 | P0 | (U1 * 7U) | TDBYTE |
                                                CSR_RXPBDLYTG0_ADDR))),
                0x7FU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB0 | P0 | (U1 * 8U) | TDBYTE |
                                                CSR_RXPBDLYTG0_ADDR))),
                0x7FU);

  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB1 | P0 | U0 | TDBYTE | CSR_RXPBDLYTG0_ADDR))),
                0x7FU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB1 | P0 | U1 | TDBYTE | CSR_RXPBDLYTG0_ADDR))),
                0x7FU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB1 | P0 | (U1 * 2U) | TDBYTE |
                                                CSR_RXPBDLYTG0_ADDR))),
                0x7FU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB1 | P0 | (U1 * 3U) | TDBYTE |
                                                CSR_RXPBDLYTG0_ADDR))),
                0x7FU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB1 | P0 | (U1 * 4U) | TDBYTE |
                                                CSR_RXPBDLYTG0_ADDR))),
                0x7FU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB1 | P0 | (U1 * 5U) | TDBYTE |
                                                CSR_RXPBDLYTG0_ADDR))),
                0x7FU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB1 | P0 | (U1 * 6U) | TDBYTE |
                                                CSR_RXPBDLYTG0_ADDR))),
                0x7FU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB1 | P0 | (U1 * 7U) | TDBYTE |
                                                CSR_RXPBDLYTG0_ADDR))),
                0x7FU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB1 | P0 | (U1 * 8U) | TDBYTE |
                                                CSR_RXPBDLYTG0_ADDR))),
                0x7FU);

  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB2 | P0 | U0 | TDBYTE | CSR_RXPBDLYTG0_ADDR))),
                0x7FU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB2 | P0 | U1 | TDBYTE | CSR_RXPBDLYTG0_ADDR))),
                0x7FU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB2 | P0 | (U1 * 2U) | TDBYTE |
                                                CSR_RXPBDLYTG0_ADDR))),
                0x7FU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB2 | P0 | (U1 * 3U) | TDBYTE |
                                                CSR_RXPBDLYTG0_ADDR))),
                0x7FU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB2 | P0 | (U1 * 4U) | TDBYTE |
                                                CSR_RXPBDLYTG0_ADDR))),
                0x7FU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB2 | P0 | (U1 * 5U) | TDBYTE |
                                                CSR_RXPBDLYTG0_ADDR))),
                0x7FU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB2 | P0 | (U1 * 6U) | TDBYTE |
                                                CSR_RXPBDLYTG0_ADDR))),
                0x7FU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB2 | P0 | (U1 * 7U) | TDBYTE |
                                                CSR_RXPBDLYTG0_ADDR))),
                0x7FU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB2 | P0 | (U1 * 8U) | TDBYTE |
                                                CSR_RXPBDLYTG0_ADDR))),
                0x7FU);

  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB3 | P0 | U0 | TDBYTE | CSR_RXPBDLYTG0_ADDR))),
                0x7FU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB3 | P0 | U1 | TDBYTE | CSR_RXPBDLYTG0_ADDR))),
                0x7FU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB3 | P0 | (U1 * 2U) | TDBYTE |
                                                CSR_RXPBDLYTG0_ADDR))),
                0x7FU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB3 | P0 | (U1 * 3U) | TDBYTE |
                                                CSR_RXPBDLYTG0_ADDR))),
                0x7FU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB3 | P0 | (U1 * 4U) | TDBYTE |
                                                CSR_RXPBDLYTG0_ADDR))),
                0x7FU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB3 | P0 | (U1 * 5U) | TDBYTE |
                                                CSR_RXPBDLYTG0_ADDR))),
                0x7FU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB3 | P0 | (U1 * 6U) | TDBYTE |
                                                CSR_RXPBDLYTG0_ADDR))),
                0x7FU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB3 | P0 | (U1 * 7U) | TDBYTE |
                                                CSR_RXPBDLYTG0_ADDR))),
                0x7FU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB3 | P0 | (U1 * 8U) | TDBYTE |
                                                CSR_RXPBDLYTG0_ADDR))),
                0x7FU);

  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB0 | P0 | R0 | TDBYTE | CSR_TXDQDLYTG0_ADDR))),
                0x5EU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB0 | P0 | R1 | TDBYTE | CSR_TXDQDLYTG0_ADDR))),
                0x5EU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB0 | P0 | R2 | TDBYTE | CSR_TXDQDLYTG0_ADDR))),
                0x5EU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB0 | P0 | R3 | TDBYTE | CSR_TXDQDLYTG0_ADDR))),
                0x5EU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB0 | P0 | R4 | TDBYTE | CSR_TXDQDLYTG0_ADDR))),
                0x5EU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB0 | P0 | R5 | TDBYTE | CSR_TXDQDLYTG0_ADDR))),
                0x5EU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB0 | P0 | R6 | TDBYTE | CSR_TXDQDLYTG0_ADDR))),
                0x5EU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB0 | P0 | R7 | TDBYTE | CSR_TXDQDLYTG0_ADDR))),
                0x5EU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB0 | P0 | R8 | TDBYTE | CSR_TXDQDLYTG0_ADDR))),
                0x5EU);

  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB1 | P0 | R0 | TDBYTE | CSR_TXDQDLYTG0_ADDR))),
                0x5EU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB1 | P0 | R1 | TDBYTE | CSR_TXDQDLYTG0_ADDR))),
                0x5EU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB1 | P0 | R2 | TDBYTE | CSR_TXDQDLYTG0_ADDR))),
                0x5EU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB1 | P0 | R3 | TDBYTE | CSR_TXDQDLYTG0_ADDR))),
                0x5EU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB1 | P0 | R4 | TDBYTE | CSR_TXDQDLYTG0_ADDR))),
                0x5EU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB1 | P0 | R5 | TDBYTE | CSR_TXDQDLYTG0_ADDR))),
                0x5EU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB1 | P0 | R6 | TDBYTE | CSR_TXDQDLYTG0_ADDR))),
                0x5EU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB1 | P0 | R7 | TDBYTE | CSR_TXDQDLYTG0_ADDR))),
                0x5EU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB1 | P0 | R8 | TDBYTE | CSR_TXDQDLYTG0_ADDR))),
                0x5EU);

  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB2 | P0 | R0 | TDBYTE | CSR_TXDQDLYTG0_ADDR))),
                0x5EU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB2 | P0 | R1 | TDBYTE | CSR_TXDQDLYTG0_ADDR))),
                0x5EU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB2 | P0 | R2 | TDBYTE | CSR_TXDQDLYTG0_ADDR))),
                0x5EU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB2 | P0 | R3 | TDBYTE | CSR_TXDQDLYTG0_ADDR))),
                0x5EU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB2 | P0 | R4 | TDBYTE | CSR_TXDQDLYTG0_ADDR))),
                0x5EU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB2 | P0 | R5 | TDBYTE | CSR_TXDQDLYTG0_ADDR))),
                0x5EU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB2 | P0 | R6 | TDBYTE | CSR_TXDQDLYTG0_ADDR))),
                0x5EU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB2 | P0 | R7 | TDBYTE | CSR_TXDQDLYTG0_ADDR))),
                0x5EU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB2 | P0 | R8 | TDBYTE | CSR_TXDQDLYTG0_ADDR))),
                0x5EU);

  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB3 | P0 | R0 | TDBYTE | CSR_TXDQDLYTG0_ADDR))),
                0x5EU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB3 | P0 | R1 | TDBYTE | CSR_TXDQDLYTG0_ADDR))),
                0x5EU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB3 | P0 | R2 | TDBYTE | CSR_TXDQDLYTG0_ADDR))),
                0x5EU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB3 | P0 | R3 | TDBYTE | CSR_TXDQDLYTG0_ADDR))),
                0x5EU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB3 | P0 | R4 | TDBYTE | CSR_TXDQDLYTG0_ADDR))),
                0x5EU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB3 | P0 | R5 | TDBYTE | CSR_TXDQDLYTG0_ADDR))),
                0x5EU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB3 | P0 | R6 | TDBYTE | CSR_TXDQDLYTG0_ADDR))),
                0x5EU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB3 | P0 | R7 | TDBYTE | CSR_TXDQDLYTG0_ADDR))),
                0x5EU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB3 | P0 | R8 | TDBYTE | CSR_TXDQDLYTG0_ADDR))),
                0x5EU);

  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB0 | P0 | U0 | TDBYTE | CSR_RXENDLYTG1_ADDR))),
                0x2C0U);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB0 | P0 | U1 | TDBYTE | CSR_RXENDLYTG1_ADDR))),
                0x2C0U);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB1 | P0 | U0 | TDBYTE | CSR_RXENDLYTG1_ADDR))),
                0x2C0U);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB1 | P0 | U1 | TDBYTE | CSR_RXENDLYTG1_ADDR))),
                0x2C0U);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB2 | P0 | U0 | TDBYTE | CSR_RXENDLYTG1_ADDR))),
                0x2C0U);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB2 | P0 | U1 | TDBYTE | CSR_RXENDLYTG1_ADDR))),
                0x2C0U);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB3 | P0 | U0 | TDBYTE | CSR_RXENDLYTG1_ADDR))),
                0x2C0U);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB3 | P0 | U1 | TDBYTE | CSR_RXENDLYTG1_ADDR))),
                0x2C0U);

  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB0 | P0 | U0 | TDBYTE | CSR_RXPBDLYTG1_ADDR))),
                0x7FU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB0 | P0 | U1 | TDBYTE | CSR_RXPBDLYTG1_ADDR))),
                0x7FU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB0 | P0 | (U1 * 2U) | TDBYTE |
                                                CSR_RXPBDLYTG1_ADDR))),
                0x7FU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB0 | P0 | (U1 * 3U) | TDBYTE |
                                                CSR_RXPBDLYTG1_ADDR))),
                0x7FU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB0 | P0 | (U1 * 4U) | TDBYTE |
                                                CSR_RXPBDLYTG1_ADDR))),
                0x7FU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB0 | P0 | (U1 * 5U) | TDBYTE |
                                                CSR_RXPBDLYTG1_ADDR))),
                0x7FU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB0 | P0 | (U1 * 6U) | TDBYTE |
                                                CSR_RXPBDLYTG1_ADDR))),
                0x7FU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB0 | P0 | (U1 * 7U) | TDBYTE |
                                                CSR_RXPBDLYTG1_ADDR))),
                0x7FU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB0 | P0 | (U1 * 8U) | TDBYTE |
                                                CSR_RXPBDLYTG1_ADDR))),
                0x7FU);

  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB1 | P0 | U0 | TDBYTE | CSR_RXPBDLYTG1_ADDR))),
                0x7FU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB1 | P0 | U1 | TDBYTE | CSR_RXPBDLYTG1_ADDR))),
                0x7FU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB1 | P0 | (U1 * 2U) | TDBYTE |
                                                CSR_RXPBDLYTG1_ADDR))),
                0x7FU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB1 | P0 | (U1 * 3U) | TDBYTE |
                                                CSR_RXPBDLYTG1_ADDR))),
                0x7FU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB1 | P0 | (U1 * 4U) | TDBYTE |
                                                CSR_RXPBDLYTG1_ADDR))),
                0x7FU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB1 | P0 | (U1 * 5U) | TDBYTE |
                                                CSR_RXPBDLYTG1_ADDR))),
                0x7FU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB1 | P0 | (U1 * 6U) | TDBYTE |
                                                CSR_RXPBDLYTG1_ADDR))),
                0x7FU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB1 | P0 | (U1 * 7U) | TDBYTE |
                                                CSR_RXPBDLYTG1_ADDR))),
                0x7FU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB1 | P0 | (U1 * 8U) | TDBYTE |
                                                CSR_RXPBDLYTG1_ADDR))),
                0x7FU);

  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB2 | P0 | U0 | TDBYTE | CSR_RXPBDLYTG1_ADDR))),
                0x7FU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB2 | P0 | U1 | TDBYTE | CSR_RXPBDLYTG1_ADDR))),
                0x7FU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB2 | P0 | (U1 * 2U) | TDBYTE |
                                                CSR_RXPBDLYTG1_ADDR))),
                0x7FU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB2 | P0 | (U1 * 3U) | TDBYTE |
                                                CSR_RXPBDLYTG1_ADDR))),
                0x7FU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB2 | P0 | (U1 * 4U) | TDBYTE |
                                                CSR_RXPBDLYTG1_ADDR))),
                0x7FU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB2 | P0 | (U1 * 5U) | TDBYTE |
                                                CSR_RXPBDLYTG1_ADDR))),
                0x7FU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB2 | P0 | (U1 * 6U) | TDBYTE |
                                                CSR_RXPBDLYTG1_ADDR))),
                0x7FU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB2 | P0 | (U1 * 7U) | TDBYTE |
                                                CSR_RXPBDLYTG1_ADDR))),
                0x7FU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB2 | P0 | (U1 * 8U) | TDBYTE |
                                                CSR_RXPBDLYTG1_ADDR))),
                0x7FU);

  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB3 | P0 | U0 | TDBYTE | CSR_RXPBDLYTG1_ADDR))),
                0x7FU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB3 | P0 | U1 | TDBYTE | CSR_RXPBDLYTG1_ADDR))),
                0x7FU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB3 | P0 | (U1 * 2U) | TDBYTE |
                                                CSR_RXPBDLYTG1_ADDR))),
                0x7FU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB3 | P0 | (U1 * 3U) | TDBYTE |
                                                CSR_RXPBDLYTG1_ADDR))),
                0x7FU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB3 | P0 | (U1 * 4U) | TDBYTE |
                                                CSR_RXPBDLYTG1_ADDR))),
                0x7FU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB3 | P0 | (U1 * 5U) | TDBYTE |
                                                CSR_RXPBDLYTG1_ADDR))),
                0x7FU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB3 | P0 | (U1 * 6U) | TDBYTE |
                                                CSR_RXPBDLYTG1_ADDR))),
                0x7FU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB3 | P0 | (U1 * 7U) | TDBYTE |
                                                CSR_RXPBDLYTG1_ADDR))),
                0x7FU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB3 | P0 | (U1 * 8U) | TDBYTE |
                                                CSR_RXPBDLYTG1_ADDR))),
                0x7FU);

  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB0 | P0 | R0 | TDBYTE | CSR_TXDQDLYTG1_ADDR))),
                0x5EU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB0 | P0 | R1 | TDBYTE | CSR_TXDQDLYTG1_ADDR))),
                0x5EU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB0 | P0 | R2 | TDBYTE | CSR_TXDQDLYTG1_ADDR))),
                0x5EU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB0 | P0 | R3 | TDBYTE | CSR_TXDQDLYTG1_ADDR))),
                0x5EU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB0 | P0 | R4 | TDBYTE | CSR_TXDQDLYTG1_ADDR))),
                0x5EU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB0 | P0 | R5 | TDBYTE | CSR_TXDQDLYTG1_ADDR))),
                0x5EU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB0 | P0 | R6 | TDBYTE | CSR_TXDQDLYTG1_ADDR))),
                0x5EU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB0 | P0 | R7 | TDBYTE | CSR_TXDQDLYTG1_ADDR))),
                0x5EU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB0 | P0 | R8 | TDBYTE | CSR_TXDQDLYTG1_ADDR))),
                0x5EU);

  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB1 | P0 | R0 | TDBYTE | CSR_TXDQDLYTG1_ADDR))),
                0x5EU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB1 | P0 | R1 | TDBYTE | CSR_TXDQDLYTG1_ADDR))),
                0x5EU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB1 | P0 | R2 | TDBYTE | CSR_TXDQDLYTG1_ADDR))),
                0x5EU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB1 | P0 | R3 | TDBYTE | CSR_TXDQDLYTG1_ADDR))),
                0x5EU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB1 | P0 | R4 | TDBYTE | CSR_TXDQDLYTG1_ADDR))),
                0x5EU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB1 | P0 | R5 | TDBYTE | CSR_TXDQDLYTG1_ADDR))),
                0x5EU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB1 | P0 | R6 | TDBYTE | CSR_TXDQDLYTG1_ADDR))),
                0x5EU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB1 | P0 | R7 | TDBYTE | CSR_TXDQDLYTG1_ADDR))),
                0x5EU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB1 | P0 | R8 | TDBYTE | CSR_TXDQDLYTG1_ADDR))),
                0x5EU);

  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB2 | P0 | R0 | TDBYTE | CSR_TXDQDLYTG1_ADDR))),
                0x5EU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB2 | P0 | R1 | TDBYTE | CSR_TXDQDLYTG1_ADDR))),
                0x5EU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB2 | P0 | R2 | TDBYTE | CSR_TXDQDLYTG1_ADDR))),
                0x5EU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB2 | P0 | R3 | TDBYTE | CSR_TXDQDLYTG1_ADDR))),
                0x5EU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB2 | P0 | R4 | TDBYTE | CSR_TXDQDLYTG1_ADDR))),
                0x5EU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB2 | P0 | R5 | TDBYTE | CSR_TXDQDLYTG1_ADDR))),
                0x5EU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB2 | P0 | R6 | TDBYTE | CSR_TXDQDLYTG1_ADDR))),
                0x5EU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB2 | P0 | R7 | TDBYTE | CSR_TXDQDLYTG1_ADDR))),
                0x5EU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB2 | P0 | R8 | TDBYTE | CSR_TXDQDLYTG1_ADDR))),
                0x5EU);

  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB3 | P0 | R0 | TDBYTE | CSR_TXDQDLYTG1_ADDR))),
                0x5EU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB3 | P0 | R1 | TDBYTE | CSR_TXDQDLYTG1_ADDR))),
                0x5EU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB3 | P0 | R2 | TDBYTE | CSR_TXDQDLYTG1_ADDR))),
                0x5EU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB3 | P0 | R3 | TDBYTE | CSR_TXDQDLYTG1_ADDR))),
                0x5EU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB3 | P0 | R4 | TDBYTE | CSR_TXDQDLYTG1_ADDR))),
                0x5EU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB3 | P0 | R5 | TDBYTE | CSR_TXDQDLYTG1_ADDR))),
                0x5EU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB3 | P0 | R6 | TDBYTE | CSR_TXDQDLYTG1_ADDR))),
                0x5EU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB3 | P0 | R7 | TDBYTE | CSR_TXDQDLYTG1_ADDR))),
                0x5EU);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (BB3 | P0 | R8 | TDBYTE | CSR_TXDQDLYTG1_ADDR))),
                0x5EU);

  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (TAPBONLY | CSR_MICROCONTMUXSEL_ADDR))), 0x1U);
#endif /* STM32MP_LPDDR4_TYPE */
#endif /* USE_STM32MP257CXX_EMU */

  VERBOSE("%s End\n", __func__);
}

