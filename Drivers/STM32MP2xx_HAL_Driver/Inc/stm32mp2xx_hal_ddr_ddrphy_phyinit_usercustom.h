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

#ifndef DDRPHY_PHYINIT_USERCUSTOM_H
#define DDRPHY_PHYINIT_USERCUSTOM_H

#include <stdbool.h>
#include <stdint.h>

#include "stm32mp2xx_hal.h"
#include "stm32mp2xx_hal_ddr_ddrphy_phyinit_struct.h"

#include "stm32mp2xx_hal_ddr_ddrphy_csr_all_cdefines.h"

/* Message Block Structure Definitions */
#if STM32MP_DDR3_TYPE
#include "stm32mp2xx_hal_ddr_mnpmusrammsgblock_ddr3.h"
#elif STM32MP_DDR4_TYPE
#include "stm32mp2xx_hal_ddr_mnpmusrammsgblock_ddr4.h"
#elif STM32MP_LPDDR4_TYPE
#include "stm32mp2xx_hal_ddr_mnpmusrammsgblock_lpddr4.h"
#endif /* STM32MP_LPDDR4_TYPE */

#ifndef __AARCH64__
extern uint32_t FW_DDR_START;
#endif /* __AARCH64__ */

/*
 * -------------------------------------------------------------
 * Defines for Firmware Images
 * - indicate IMEM/DMEM size (bytes)
 * -------------------------------------------------------------
 *
 * IMEM_SIZE max size of instruction memory.
 *
 * DMEM_SIZE max size of data memory.
 *
 * DMEM_ST_ADDR start of DMEM address in memory.
 */
#if STM32MP_DDR3_TYPE
#define IMEM_SIZE                  0x4c28
#define DMEM_SIZE                  0x6c8
#elif STM32MP_DDR4_TYPE
#define IMEM_SIZE                  0x6d24
#define DMEM_SIZE                  0x6cc
#elif STM32MP_LPDDR4_TYPE
#define IMEM_SIZE                  0x7e50
#define DMEM_SIZE                  0x67c
#endif /* STM32MP_LPDDR4_TYPE */

#define IMEM_ST_ADDR               0x50000U
#define DMEM_ST_ADDR               0x54000U
#define DMEM_BIN_OFFSET            0x200U

#ifndef __AARCH64__
#define STM32MP_DDR_FW_BASE        ((uintptr_t)(&FW_DDR_START))
#endif /* __AARCH64__ */
#define STM32MP_DDR_FW_DMEM_OFFSET 0x400U
#define STM32MP_DDR_FW_IMEM_OFFSET 0x800U

/*
 * -------------------------------------------------------------
 * Defines for SR Firmware Images
 * - indicate IMEM size (bytes)
 * -------------------------------------------------------------
 *
 * SR_IMEM_SIZE max IMEM size of retention save/restore firmware.
 */
#define SR_IMEM_SIZE          16384

/*
 * ------------------
 * Type definitions
 * ------------------
 */

/* Enumeration of instructions for PhyInit Register Interface */
typedef enum {
  STARTTRACK,  /* Start register tracking */
  STOPTRACK,   /* Stop register tracking */
  SAVEREGS,    /* Save(read) tracked register values */
  RESTOREREGS, /* Restore (write) saved register values */
  DUMPREGS,    /* Write register address,value pairs to file */
  IMPORTREGS   /* Import register address,value pairs to file */
} reginstr;

/* Data structure to store register address, value pairs */
typedef struct {
  uint32_t  address; /* Register address */
  uint16_t  value;   /* Register value */
} reg_addr_val_t;

/* TargetCSR Target CSR for the impedance value for ddrphy_phyinit_mapdrvstren() */
typedef enum {
  DRVSTRENFSDQP,
  DRVSTRENFSDQN,
  ODTSTRENP,
  ODTSTRENN,
  ADRVSTRENP,
  ADRVSTRENN
} drvtype;

/* Enumeration of message block software set fields */
typedef enum {
  SSMB_BPZNRESVAL,
  SSMB_CSPRESENTCHA,
  SSMB_CSPRESENTCHB,
  SSMB_DFIFREQRATIO,
  SSMB_DISABLEDDBYTE,
  SSMB_DRAMFREQ,
  SSMB_DRAMTYPE,
  SSMB_ENABLEDDQS,
  SSMB_ENABLEDDQSCHA,
  SSMB_ENABLEDDQSCHB,
  SSMB_PHYCFG,
  SSMB_PHYDRVIMPEDANCE,
  SSMB_PHYODTIMPEDANCE,
  SSMB_PLLBYPASSEN,
  SSMB_PSTATE,
  SSMB_X16PRESENT
} softsetmb_field;

/*
 * -------------------------------
 * Global variables - defined in ddrphy_phyinit_globals.c
 * -------------------------------
 */

extern int32_t ardptrinitval[NB_PS];

static inline void mmio_write_16(uintptr_t addr, uint16_t value)
{
  uint32_t data;

  data = READ_REG(*(volatile uint32_t*)addr);
  data &= 0xFFFF0000U;
  data |= (uint32_t)value;
  WRITE_REG(*(volatile uint32_t*)addr, data);
}

static inline uint16_t mmio_read_16(uintptr_t addr)
{
  return (uint16_t)READ_REG(*(volatile uint32_t*)addr);
}

static inline void mmio_write_32(uintptr_t addr, uint32_t value)
{
  WRITE_REG(*(volatile uint32_t*)addr, value);
}

#define VERBOSE(...)
#define ERROR(...)

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

/*
 * -------------------------------------------------------------
 * Fixed Function prototypes
 * -------------------------------------------------------------
 */
int32_t ddrphy_phyinit_sequence(bool skip_training, bool reten);
int32_t ddrphy_phyinit_restore_sequence(void);
int32_t ddrphy_phyinit_c_initphyconfig(void);
void ddrphy_phyinit_d_loadimem(void);
void ddrphy_phyinit_progcsrskiptrain(void);
int32_t ddrphy_phyinit_f_loaddmem(int32_t pstate);
int32_t ddrphy_phyinit_g_execfw(void);
void ddrphy_phyinit_i_loadpieimage(bool skip_training);
void ddrphy_phyinit_loadpieprodcode(void);
int32_t ddrphy_phyinit_mapdrvstren(int32_t drvstren_ohm, drvtype targetcsr);
int32_t ddrphy_phyinit_calcmb(void);
void ddrphy_phyinit_writeoutmem(const uint32_t *mem, uint32_t mem_offset, int32_t mem_size);
void ddrphy_phyinit_writeoutmsgblk(uint16_t *mem, uint32_t mem_offset, int32_t mem_size);
int32_t ddrphy_phyinit_isdbytedisabled(int32_t dbytenumber);
int32_t ddrphy_phyinit_setretreglistbase(uintptr_t base);
int32_t ddrphy_phyinit_read_numpstates(void);
int32_t ddrphy_phyinit_read_numanib(void);
int32_t ddrphy_phyinit_read_numdbyte(void);
int32_t ddrphy_phyinit_trackreg(uint32_t adr);
int32_t ddrphy_phyinit_reginterface(reginstr myreginstr, uint32_t adr, uint16_t dat);

extern void ddrphy_phyinit_usercustom_pretrain(void);
extern void ddrphy_phyinit_usercustom_posttrain(void);
extern int32_t ddrphy_phyinit_usercustom_g_waitfwdone(void);
extern int32_t ddrphy_phyinit_usercustom_saveretregs(void);

#endif /* DDRPHY_PHYINIT_USERCUSTOM_H */
