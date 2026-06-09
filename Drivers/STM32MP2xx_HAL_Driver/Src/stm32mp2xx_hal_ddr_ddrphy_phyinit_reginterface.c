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

/*
 * This file provides a group of functions that are used to track PHY register
 * writes by intercepting io_write16 function calls.  Once the registers are
 * tracked, their value can be saved at a given time spot, and restored later
 * as required.  This implementation is useful to capture any PHY register
 * programing in any function during PHY initialization.
 */

#include <stdlib.h>

#include "stm32mp2xx_hal_ddr_ddrphy_phyinit.h"

/*
 * MAX_NUM_RET_REGS default Max number of retention registers
 *
 * This define is only used by the PhyInit Register interface to define the max
 * amount of registered that can be saved. The user may increase this variable
 * as desired if a larger number of registers need to be restored.
 */
#if STM32MP_DDR3_TYPE || STM32MP_DDR4_TYPE
#define MAX_NUM_RET_REGS 129
#elif STM32MP_LPDDR4_TYPE
#define MAX_NUM_RET_REGS 283
#endif /* STM32MP_LPDDR4_TYPE */

/*
 * Array of Address/value pairs used to store register values for the purpose
 * of retention restore.
 */
#define RETREG_AREA ((MAX_NUM_RET_REGS + 1) * sizeof(reg_addr_val_t))
#define RETREG_BASE (RETRAM_BASE + RETRAM_SIZE - RETREG_AREA)

/*
 * CSR_TXSLEWRATE reset value.
 * If related pstate/dbyte/anib is used, content is different from reset value.
 * This register is used to get pstate/dbyte/anib numbers when context is not
 * available.
 */
#define CSR_TXSLEWRATE_RST  0x7FFU
#define CSR_ATXSLEWRATE_RST 0x7FFU

static int32_t *retregsize = (int32_t *)(RETREG_BASE);
static reg_addr_val_t *retreglist = (reg_addr_val_t *)(RETREG_BASE + sizeof(int32_t));

static int32_t numregsaved; /* Current Number of registers saved. */
static int32_t tracken = 1; /* Enabled tracking of registers */

int32_t ddrphy_phyinit_setretreglistbase(uintptr_t base)
{
  int32_t *value = (int32_t *)base;
  int32_t save = *value;;
  int32_t pattern = 0x5555AAAA;

  /* Check access capability */
  *value = pattern;
  if (*value != pattern)
  {
    return -1;
  }

  /* Restore intial value */
  *value = save;

  retregsize = (int32_t *)base;
  retreglist = (reg_addr_val_t *)(base + 4);

  return 0;
}

static uint16_t ext_read_register(uint32_t address, uint32_t offset)
{
  uint16_t value;

  /*
   * Prepare for register reads\n
   * - Write the MicroContMuxSel CSR to 0x0 to allow access to the internal CSRs
   * - Write the UcclkHclkEnables CSR to 0x3 to enable all the clocks so the reads
   *   can complete.
   */
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (TAPBONLY | CSR_MICROCONTMUXSEL_ADDR))), 0x0U);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (TDRTUB | CSR_UCCLKHCLKENABLES_ADDR))), 0x3U);

  value = mmio_read_16((uintptr_t)(DDRPHYC_BASE + 4 * (offset | address)));

  /*
   * Prepare for mission mode
   * - Write the UcclkHclkEnables CSR to disable the appropriate clocks after all reads done.
   * - Write the MicroContMuxSel CSR to 0x1 to isolate the internal CSRs during mission mode
   */
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (TDRTUB | CSR_UCCLKHCLKENABLES_ADDR))), 0x0U);
  mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * (TAPBONLY | CSR_MICROCONTMUXSEL_ADDR))), 0x1U);

  return value;
}

static uint16_t ext_read_txslewrate(uint32_t offset)
{
  return ext_read_register(CSR_TXSLEWRATE_ADDR | TDBYTE, offset);
}

static uint16_t ext_read_atxslewrate(uint32_t offset)
{
  return ext_read_register(CSR_ATXSLEWRATE_ADDR | TANIB, offset);
}

int32_t ddrphy_phyinit_read_numpstates(void)
{
  uint16_t reg;

  /*
   * Only Pstate0 can be accessed.
   * Read related txslewrate register and test its content.
   * If equal to 0 or reset value, then initialization is not done.
   * Else, we have one pstate.
   * No other value is accepted.
   */
  reg = ext_read_txslewrate(0U);

  if ((!reg) || (reg == CSR_TXSLEWRATE_RST))
  {
    return 0;
  }
  else
  {
    return 1;
  }
}

int32_t ddrphy_phyinit_read_numanib(void)
{
  uint16_t reg;

  /*
   * Whatever the configuration, only 8 anibs are supported.
   * Read anib7 atxslewrate register and test its content
   * (anib7 is non-CK anib).
   * If equal to 0 or reset value, then initialization is not done.
   * Else, we have 8 anibs.
   * No other value is accepted.
   */
  reg = ext_read_atxslewrate(7U << 12);

  if ((!reg) || (reg == CSR_ATXSLEWRATE_RST))
  {
    return 0;
  }
  else
  {
    return 8;
  }
}

int32_t ddrphy_phyinit_read_numdbyte(void)
{
  uint16_t dbyte1;
  uint16_t dbyte3;

  /*
   * Whatever the configuration, only 2 and 4 dbytes are supported.
   * Read dbyte3/dbyte1 txslewrate registers and test their content.
   * If both equal to 0 or reset value, then initialization is not done.
   * Else we have 4 dbytes if dbyte3 is equal to dbyte1.
   * Else we have 2 dbytes.
   * No other value is accepted.
   */
  dbyte1 = ext_read_txslewrate(1U << 12);
  dbyte3 = ext_read_txslewrate(3U << 12);

  if ((!(dbyte1) && !(dbyte3)) ||
      ((dbyte1 == CSR_TXSLEWRATE_RST) && (dbyte3 == CSR_TXSLEWRATE_RST)))
  {
    return 0;
  }
  else if (dbyte1 == dbyte3)
  {
    return 4;
  }
  else
  {
    return 2;
  }
}

/*
 * Tags a register if tracking is enabled in the register
 * interface
 *
 * during PhyInit registers writes, keeps track of address
 * for the purpose of restoring the PHY register state during PHY
 * retention exit process.  Tracking can be turned on/off via the
 * ddrphy_phyinit_reginterface STARTTRACK, STOPTRACK instructions. By
 * default tracking is always turned on.
 *
 * \return 0 on success.
 */
int32_t ddrphy_phyinit_trackreg(uint32_t adr)
{
  int32_t regindx = 0;

  /* Return if tracking is disabled */
  if (tracken == 0)
  {
    return 0;
  }

  /* Search register address within the array */
  for (regindx = 0; regindx < numregsaved; regindx++)
  {
    if (retreglist[regindx].address == adr)
    {
      /* Register found */
      return 0;
    }
  }

  /* Register not found, so add it. */
  if (numregsaved > MAX_NUM_RET_REGS)
  {
    ERROR("[ddrphy_phyinit_reginterface:ddrphy_phyinit_trackreg]\n");
    ERROR("Max Number of Restore Registers reached: %d.\n", numregsaved);
    ERROR("Please recompile PhyInit with larger MAX_NUM_RET_REG value.\n");
    return -1;
  }

  retreglist[regindx].address = adr;
  numregsaved++;

  return 0;
}

/*
 * Register interface function used to track, save and restore retention registers.
 *
 * ### Usage
 * Register tracking is enabled by calling:
 *
 *  \code
 *  ddrphy_phyinit_reginterface(STARTTRACK,0,0);
 *  \endcode
 *
 * from this point on any call to ddrphy_phyinit_usercustom_io_write16() in
 * return will be capture by the register interface via a call to
 * ddrphy_phyinit_trackreg(). Tracking is disabled by calling:
 *
 *  \code
 *  ddrphy_phyinit_reginterface(STOPTRACK,0,0);
 *  \endcode
 *
 * On calling this function, register write via
 * ddrphy_phyinit_usercustom_io_write16 are no longer tracked until a
 * STARTTRACK call is made.  Once all the register write are complete, SAVEREGS
 * command can be issue to save register values into the internal data array of
 * the register interface.  Upon retention exit RESTOREREGS are command can be
 * used to issue register write commands to the PHY based on values stored in
 * the array.
 *  \code
 *   ddrphy_phyinit_reginterface(SAVEREGS,0,0);
 *   ddrphy_phyinit_reginterface(RESTOREREGS,0,0);
 *  \endcode
 * \return 0 on success.
 */
int32_t ddrphy_phyinit_reginterface(reginstr myreginstr, __unused uint32_t adr,
                                    __unused uint16_t dat)
{
  if (myreginstr == SAVEREGS)
  {
    int32_t regindx;

    /*
     * go through all the tracked registers, issue a register read and place
     * the result in the data structure for future recovery.
     */
    for (regindx = 0; regindx < numregsaved; regindx++)
    {
      uint16_t data;

      data = mmio_read_16((uintptr_t)(DDRPHYC_BASE + (4U * retreglist[regindx].address)));
      retreglist[regindx].value = data;
    }

    *retregsize = numregsaved;

    return 0;
  }
  else if (myreginstr == RESTOREREGS)
  {
    int32_t regindx;

    /*
     * write PHY registers based on Address, Data value pairs stores in
     * retreglist
     */
    for (regindx = 0; regindx < *retregsize; regindx++)
    {
      mmio_write_16((uintptr_t)(DDRPHYC_BASE + (4U * retreglist[regindx].address)),
                    retreglist[regindx].value);
    }

    return 0;
  }
  else if (myreginstr == STARTTRACK)
  {
    /* Enable tracking */
    tracken = 1;
    return 0;
  }
  else if (myreginstr == STOPTRACK)
  {
    /* Disable tracking */
    tracken = 0;
    return 0;
  }
  else if (myreginstr == DUMPREGS)
  {
    /* Dump restore state to file. */
    /* TBD */
    return 0;
  }
  else if (myreginstr == IMPORTREGS)
  {
    /* import register state from file. */
    /* TBD */
    return 0;
  }
  else
  {
    /* future instructions. */
    return -1;
  }
}
