/**
  ******************************************************************************
  * @file    stm32mp2xx_hal_ddr.c
  * @author  MCD Application Team
  * @brief   DDR HAL module driver.
  *          This file provides firmware functions to manage the following
  *          functionalities of the Double Data Rate Synchronous Dynamic Random
  *          Access Memory (DDR-SDRAM) Peripheral, through its controller
  *          (DDRCTRL) and its physical interface controller (DDRPHYC).
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
  @verbatim
 ===============================================================================
                        ##### How to use this driver #####
 ===============================================================================

  @endverbatim
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "stm32mp2xx_hal.h"

#ifdef HAL_DDR_MODULE_ENABLED

#include "stm32mp2xx_hal_ddr_ddrphy_phyinit.h"

#if (STM32MP_DDR3_TYPE && (DDR_SIZE_Gb == 8)) && defined(STM32MP_DDR_16_BIT_INTERFACE)
#ifdef USE_STM32MP257CXX_EMU
#if (DDR_FREQ == 800)
#include "stm32mp2xx-ddr3-1x8Gbits-1x16bits-800MHz-palladium.h"
#endif /* (DDR_FREQ == 800) */
#endif /* USE_STM32MP257CXX_EMU */
#ifndef USE_STM32MP257CXX_EMU
#if (DDR_FREQ == 800)
#include "stm32mp2xx-ddr3-1x8Gbits-1x16bits-800MHz.h"
#endif /* (DDR_FREQ == 800) */
#endif /* !USE_STM32MP257CXX_EMU */
#endif /* (STM32MP_DDR3_TYPE && (DDR_SIZE_Gb == 8)) && defined(STM32MP_DDR_16_BIT_INTERFACE) */

#if (STM32MP_DDR3_TYPE && (DDR_SIZE_Gb == 8)) && !defined(STM32MP_DDR_16_BIT_INTERFACE)
#ifndef USE_STM32MP257CXX_EMU
#if (DDR_FREQ == 933)
#include "stm32mp2xx-ddr3-2x4Gbits-2x16bits-933MHz.h"
#endif /* (DDR_FREQ == 933) */
#endif /* !USE_STM32MP257CXX_EMU */
#endif /* (STM32MP_DDR3_TYPE && (DDR_SIZE_Gb == 8)) && !defined(STM32MP_DDR_16_BIT_INTERFACE) */

#if (STM32MP_DDR3_TYPE && (DDR_SIZE_Gb == 16)) && !defined(STM32MP_DDR_16_BIT_INTERFACE)
#ifdef USE_STM32MP257CXX_EMU
#if (DDR_FREQ == 933)
#include "stm32mp2xx-ddr3-2x8Gbits-2x16bits-933MHz-palladium.h"
#endif /* (DDR_FREQ == 933) */
#endif /* USE_STM32MP257CXX_EMU */
#endif /* (STM32MP_DDR3_TYPE && (DDR_SIZE_Gb == 16)) && !defined(STM32MP_DDR_16_BIT_INTERFACE) */

#if (STM32MP_DDR4_TYPE && (DDR_SIZE_Gb == 32)) && defined(STM32MP_DDR_16_BIT_INTERFACE)
#ifdef USE_STM32MP257CXX_EMU
#if (DDR_FREQ == 800)
#include "stm32mp2xx-ddr4-1x32Gbits-1x16bits-800MHz-palladium.h"
#endif /* (DDR_FREQ == 800) */
#endif /* USE_STM32MP257CXX_EMU */
#ifndef USE_STM32MP257CXX_EMU
#if (DDR_FREQ == 800)
#include "stm32mp2xx-ddr4-1x32Gbits-1x16bits-800MHz.h"
#endif /* (DDR_FREQ == 800) */
#endif /* USE_STM32MP257CXX_EMU */
#endif /* (STM32MP_DDR4_TYPE && (DDR_SIZE_Gb == 32)) && defined(STM32MP_DDR_16_BIT_INTERFACE) */

#if (STM32MP_DDR4_TYPE && (DDR_SIZE_Gb == 32)) && !defined(STM32MP_DDR_16_BIT_INTERFACE)
#ifdef USE_STM32MP257CXX_EMU
#if (DDR_FREQ == 1200)
#include "stm32mp2xx-ddr4-2x16Gbits-2x16bits-1200MHz-palladium.h"
#endif /* (DDR_FREQ == 1200) */
#endif /* USE_STM32MP257CXX_EMU */
#ifndef USE_STM32MP257CXX_EMU
#if (DDR_FREQ == 1200)
#include "stm32mp2xx-ddr4-2x16Gbits-2x16bits-1200MHz.h"
#endif /* (DDR_FREQ == 1200) */
#endif /* !USE_STM32MP257CXX_EMU */
#endif /* (STM32MP_DDR4_TYPE && (DDR_SIZE_Gb == 32)) && !defined(STM32MP_DDR_16_BIT_INTERFACE) */

#if (STM32MP_LPDDR4_TYPE && (DDR_SIZE_Gb == 16)) && defined(STM32MP_DDR_16_BIT_INTERFACE)
#ifdef USE_STM32MP257CXX_EMU
#if (DDR_FREQ == 800)
#include "stm32mp2xx-lpddr4-1x16Gbits-1x16bits-800MHz-palladium.h"
#endif /* (DDR_FREQ == 800) */
#endif /* USE_STM32MP257CXX_EMU */
#ifndef USE_STM32MP257CXX_EMU
#if (DDR_FREQ == 800)
#include "stm32mp2xx-lpddr4-1x16Gbits-1x16bits-800MHz.h"
#endif /* (DDR_FREQ == 800) */
#if (DDR_FREQ == 1200)
#include "stm32mp2xx-lpddr4-1x16Gbits-1x16bits-1200MHz.h"
#endif /* (DDR_FREQ == 1200) */
#endif /* !USE_STM32MP257CXX_EMU */
#endif /* (STM32MP_LPDDR4_TYPE && (DDR_SIZE_Gb == 16)) && defined(STM32MP_DDR_16_BIT_INTERFACE) */

#if (STM32MP_LPDDR4_TYPE && (DDR_SIZE_Gb == 32)) && !defined(STM32MP_DDR_16_BIT_INTERFACE)
#ifdef USE_STM32MP257CXX_EMU
#if (DDR_FREQ == 1200)
#include "stm32mp2xx-lpddr4-1x32Gbits-1x32bits-1200MHz-palladium.h"
#endif /* (DDR_FREQ == 1200) */
#endif /* USE_STM32MP257CXX_EMU */
#ifndef USE_STM32MP257CXX_EMU
#if (DDR_FREQ == 1200)
#include "stm32mp2xx-lpddr4-1x32Gbits-1x32bits-1200MHz.h"
#endif /* (DDR_FREQ == 1200) */
#endif /* !USE_STM32MP257CXX_EMU */
#endif /* (STM32MP_LPDDR4_TYPE && (DDR_SIZE_Gb == 32)) && !defined(STM32MP_DDR_16_BIT_INTERFACE) */

/** @addtogroup STM32MP2xx_HAL_Driver
  * @{
  */

/** @defgroup DDR DDR
  * @brief HAL DDR module driver
  * @{
  */
#if defined (DDRC)

/* Private typedef -----------------------------------------------------------*/
typedef enum
{
  BASE_DDRCTRL,
  BASE_PHY_UI_BASIC,
  BASE_PHY_UI_ADVANCED,
  BASE_PHY_UI_MODE_REGISTER,
  BASE_PHY_UI_SWIZZLE,
  BASE_NONE
} base_type;

typedef struct
{
  uint16_t offset; /* Offset for base address */
  uint8_t par_offset; /* Offset for parameter array */
  bool qd; /* quasi-dynamic register if true */
} reg_desc_t;

typedef struct
{
  const char *name;
  const reg_desc_t *desc;
  uint8_t size;
  base_type base;
} ddr_reg_info_t;

typedef enum
{
  REG_REG,
  REG_TIMING,
  REG_PERF,
  REG_MAP,
  PHY_UI_BASIC,
  PHY_UI_ADVANCED,
  PHY_UI_MODE_REGISTER,
  PHY_UI_SWIZZLE,
  REG_TYPE_NB
} reg_type;

/* Private define ------------------------------------------------------------*/
#define DFLT_SR_MODE HAL_DDR_SW_SELF_REFRESH_MODE
#define INIT_SR_MODE HAL_DDR_AUTO_SELF_REFRESH_MODE
#define MAX_SR_MODE  HAL_DDR_HW_SELF_REFRESH_MODE

#define INVALID_OFFSET 0xFFU

#define DDRCTL_REG(x, y, z) \
  { \
    .offset = offsetof(DDRC_TypeDef, x), \
    .par_offset = offsetof(y, x), \
    .qd = z \
  }

#define DDRCTL_REG_REG(x, z) DDRCTL_REG(x, HAL_DDR_RegTypeDef, z)
static const reg_desc_t ddr_reg_desc[] =
{
  DDRCTL_REG_REG(MSTR, true),
  DDRCTL_REG_REG(MRCTRL0, false),
  DDRCTL_REG_REG(MRCTRL1, false),
  DDRCTL_REG_REG(MRCTRL2, false),
  DDRCTL_REG_REG(DERATEEN, true),
  DDRCTL_REG_REG(DERATEINT, false),
  DDRCTL_REG_REG(DERATECTL, false),
  DDRCTL_REG_REG(PWRCTL, false),
  DDRCTL_REG_REG(PWRTMG, true),
  DDRCTL_REG_REG(HWLPCTL, true),
  DDRCTL_REG_REG(RFSHCTL0, false),
  DDRCTL_REG_REG(RFSHCTL1, false),
  DDRCTL_REG_REG(RFSHCTL3, true),
  DDRCTL_REG_REG(CRCPARCTL0, false),
  DDRCTL_REG_REG(CRCPARCTL1, false),
  DDRCTL_REG_REG(INIT0, true),
  DDRCTL_REG_REG(INIT1, false),
  DDRCTL_REG_REG(INIT2, false),
  DDRCTL_REG_REG(INIT3, true),
  DDRCTL_REG_REG(INIT4, true),
  DDRCTL_REG_REG(INIT5, false),
  DDRCTL_REG_REG(INIT6, true),
  DDRCTL_REG_REG(INIT7, true),
  DDRCTL_REG_REG(DIMMCTL, false),
  DDRCTL_REG_REG(RANKCTL, true),
  DDRCTL_REG_REG(RANKCTL1, true),
  DDRCTL_REG_REG(ZQCTL0, true),
  DDRCTL_REG_REG(ZQCTL1, false),
  DDRCTL_REG_REG(ZQCTL2, false),
  DDRCTL_REG_REG(DFITMG0, true),
  DDRCTL_REG_REG(DFITMG1, true),
  DDRCTL_REG_REG(DFILPCFG0, false),
  DDRCTL_REG_REG(DFILPCFG1, false),
  DDRCTL_REG_REG(DFIUPD0, true),
  DDRCTL_REG_REG(DFIUPD1, false),
  DDRCTL_REG_REG(DFIUPD2, false),
  DDRCTL_REG_REG(DFIMISC, true),
  DDRCTL_REG_REG(DFITMG2, true),
  DDRCTL_REG_REG(DFITMG3, false),
  DDRCTL_REG_REG(DBICTL, true),
  DDRCTL_REG_REG(DFIPHYMSTR, false),
  DDRCTL_REG_REG(DBG0, false),
  DDRCTL_REG_REG(DBG1, false),
  DDRCTL_REG_REG(DBGCMD, false),
  DDRCTL_REG_REG(SWCTL, false), /* forced qd value */
  DDRCTL_REG_REG(SWCTLSTATIC, false),
  DDRCTL_REG_REG(POISONCFG, false),
  DDRCTL_REG_REG(PCCFG, false),
};

#define DDRCTL_REG_TIMING(x, z) DDRCTL_REG(x, HAL_DDR_TimingTypeDef, z)
static const reg_desc_t ddr_timing_desc[] =
{
  DDRCTL_REG_TIMING(RFSHTMG, false),
  DDRCTL_REG_TIMING(RFSHTMG1, false),
  DDRCTL_REG_TIMING(DRAMTMG0, true),
  DDRCTL_REG_TIMING(DRAMTMG1, true),
  DDRCTL_REG_TIMING(DRAMTMG2, true),
  DDRCTL_REG_TIMING(DRAMTMG3, true),
  DDRCTL_REG_TIMING(DRAMTMG4, true),
  DDRCTL_REG_TIMING(DRAMTMG5, true),
  DDRCTL_REG_TIMING(DRAMTMG6, true),
  DDRCTL_REG_TIMING(DRAMTMG7, true),
  DDRCTL_REG_TIMING(DRAMTMG8, true),
  DDRCTL_REG_TIMING(DRAMTMG9, true),
  DDRCTL_REG_TIMING(DRAMTMG10, true),
  DDRCTL_REG_TIMING(DRAMTMG11, true),
  DDRCTL_REG_TIMING(DRAMTMG12, true),
  DDRCTL_REG_TIMING(DRAMTMG13, true),
  DDRCTL_REG_TIMING(DRAMTMG14, true),
  DDRCTL_REG_TIMING(DRAMTMG15, true),
  DDRCTL_REG_TIMING(ODTCFG, true),
  DDRCTL_REG_TIMING(ODTMAP, false),
};

#define DDRCTL_REG_MAP(x) DDRCTL_REG(x, HAL_DDR_MapTypeDef, false)
static const reg_desc_t ddr_map_desc[] =
{
  DDRCTL_REG_MAP(ADDRMAP0),
  DDRCTL_REG_MAP(ADDRMAP1),
  DDRCTL_REG_MAP(ADDRMAP2),
  DDRCTL_REG_MAP(ADDRMAP3),
  DDRCTL_REG_MAP(ADDRMAP4),
  DDRCTL_REG_MAP(ADDRMAP5),
  DDRCTL_REG_MAP(ADDRMAP6),
  DDRCTL_REG_MAP(ADDRMAP7),
  DDRCTL_REG_MAP(ADDRMAP8),
  DDRCTL_REG_MAP(ADDRMAP9),
  DDRCTL_REG_MAP(ADDRMAP10),
  DDRCTL_REG_MAP(ADDRMAP11),
};

#define DDRCTL_REG_PERF(x, z) DDRCTL_REG(x, HAL_DDR_PerfTypeDef, z)
static const reg_desc_t ddr_perf_desc[] =
{
  DDRCTL_REG_PERF(SCHED, true),
  DDRCTL_REG_PERF(SCHED1, false),
  DDRCTL_REG_PERF(PERFHPR1, true),
  DDRCTL_REG_PERF(PERFLPR1, true),
  DDRCTL_REG_PERF(PERFWR1, true),
  DDRCTL_REG_PERF(SCHED3, false),
  DDRCTL_REG_PERF(SCHED4, false),
  DDRCTL_REG_PERF(PCFGR_0, false),
  DDRCTL_REG_PERF(PCFGW_0, false),
  DDRCTL_REG_PERF(PCTRL_0, false),
  DDRCTL_REG_PERF(PCFGQOS0_0, true),
  DDRCTL_REG_PERF(PCFGQOS1_0, true),
  DDRCTL_REG_PERF(PCFGWQOS0_0, true),
  DDRCTL_REG_PERF(PCFGWQOS1_0, true),
#if STM32MP_DDR_DUAL_AXI_PORT
  DDRCTL_REG_PERF(PCFGR_1, false),
  DDRCTL_REG_PERF(PCFGW_1, false),
  DDRCTL_REG_PERF(PCTRL_1, false),
  DDRCTL_REG_PERF(PCFGQOS0_1, true),
  DDRCTL_REG_PERF(PCFGQOS1_1, true),
  DDRCTL_REG_PERF(PCFGWQOS0_1, true),
  DDRCTL_REG_PERF(PCFGWQOS1_1, true),
#endif /* STM32MP_DDR_DUAL_AXI_PORT */
};

#define PHY_UIB_PARAM(x) \
  { \
    .offset = offsetof(user_input_basic_t, x), \
    .par_offset = offsetof(HAL_DDR_BasicUiTypeDef, x) \
  }

#define PHY_UIB_PARAM_INDEX(x, y) \
  { \
    .offset = offsetof(user_input_basic_t, x[(y)]), \
    .par_offset = offsetof(HAL_DDR_BasicUiTypeDef, x[(y)]) \
  }

static const reg_desc_t phy_uib_desc[] =
{
  PHY_UIB_PARAM(dramtype),
  PHY_UIB_PARAM(dimmtype),
  PHY_UIB_PARAM(lp4xmode),
  PHY_UIB_PARAM(numdbyte),
  PHY_UIB_PARAM(numactivedbytedfi0),
  PHY_UIB_PARAM(numactivedbytedfi1),
  PHY_UIB_PARAM(numanib),
  PHY_UIB_PARAM(numrank_dfi0),
  PHY_UIB_PARAM(numrank_dfi1),
  PHY_UIB_PARAM(dramdatawidth),
  PHY_UIB_PARAM(numpstates),
  PHY_UIB_PARAM_INDEX(frequency, 0),
  PHY_UIB_PARAM_INDEX(pllbypass, 0),
  PHY_UIB_PARAM_INDEX(dfifreqratio, 0),
  PHY_UIB_PARAM(dfi1exists),
  PHY_UIB_PARAM(train2d),
  PHY_UIB_PARAM(hardmacrover),
  PHY_UIB_PARAM_INDEX(readdbienable, 0),
  PHY_UIB_PARAM(dfimode),
};

#define PHY_UIA_PARAM(x) \
  { \
    .offset = offsetof(user_input_advanced_t, x), \
    .par_offset = offsetof(HAL_DDR_AdvancedUiTypeDef, x) \
  }

#define PHY_UIA_PARAM_INDEX(x, y) \
  { \
    .offset = offsetof(user_input_advanced_t, x[(y)]), \
    .par_offset = offsetof(HAL_DDR_AdvancedUiTypeDef, x[(y)]) \
  }

static const reg_desc_t phy_uia_desc[] =
{
  PHY_UIA_PARAM_INDEX(lp4rxpreamblemode, 0),
  PHY_UIA_PARAM_INDEX(lp4postambleext, 0),
  PHY_UIA_PARAM_INDEX(d4rxpreamblelength, 0),
  PHY_UIA_PARAM_INDEX(d4txpreamblelength, 0),
  PHY_UIA_PARAM(extcalresval),
  PHY_UIA_PARAM_INDEX(is2ttiming, 0),
  PHY_UIA_PARAM_INDEX(odtimpedance, 0),
  PHY_UIA_PARAM_INDEX(tximpedance, 0),
  PHY_UIA_PARAM(atximpedance),
  PHY_UIA_PARAM(memalerten),
  PHY_UIA_PARAM(memalertpuimp),
  PHY_UIA_PARAM(memalertvreflevel),
  PHY_UIA_PARAM(memalertsyncbypass),
  PHY_UIA_PARAM_INDEX(disdynadrtri, 0),
  PHY_UIA_PARAM_INDEX(phymstrtraininterval, 0),
  PHY_UIA_PARAM_INDEX(phymstrmaxreqtoack, 0),
  PHY_UIA_PARAM(wdqsext),
  PHY_UIA_PARAM(calinterval),
  PHY_UIA_PARAM(calonce),
  PHY_UIA_PARAM_INDEX(lp4rl, 0),
  PHY_UIA_PARAM_INDEX(lp4wl, 0),
  PHY_UIA_PARAM_INDEX(lp4wls, 0),
  PHY_UIA_PARAM_INDEX(lp4dbird, 0),
  PHY_UIA_PARAM_INDEX(lp4dbiwr, 0),
  PHY_UIA_PARAM_INDEX(lp4nwr, 0),
  PHY_UIA_PARAM(lp4lowpowerdrv),
  PHY_UIA_PARAM(drambyteswap),
  PHY_UIA_PARAM(rxenbackoff),
  PHY_UIA_PARAM(trainsequencectrl),
  PHY_UIA_PARAM(snpsumctlopt),
  PHY_UIA_PARAM_INDEX(snpsumctlf0rc5x, 0),
  PHY_UIA_PARAM_INDEX(txslewrisedq, 0),
  PHY_UIA_PARAM_INDEX(txslewfalldq, 0),
  PHY_UIA_PARAM(txslewriseac),
  PHY_UIA_PARAM(txslewfallac),
  PHY_UIA_PARAM(disableretraining),
  PHY_UIA_PARAM(disablephyupdate),
  PHY_UIA_PARAM(enablehighclkskewfix),
  PHY_UIA_PARAM(disableunusedaddrlns),
  PHY_UIA_PARAM(phyinitsequencenum),
  PHY_UIA_PARAM(enabledficspolarityfix),
  PHY_UIA_PARAM(phyvref),
  PHY_UIA_PARAM_INDEX(sequencectrl, 0),
};

#define PHY_UIM_PARAM_INDEX(x, y) \
  { \
    .offset = offsetof(user_input_mode_register_t, x[(y)]), \
    .par_offset = offsetof(HAL_DDR_ModeRegisterUiTypeDef, x[(y)]) \
  }

static const reg_desc_t phy_uim_desc[] =
{
  PHY_UIM_PARAM_INDEX(mr0, 0),
  PHY_UIM_PARAM_INDEX(mr1, 0),
  PHY_UIM_PARAM_INDEX(mr2, 0),
  PHY_UIM_PARAM_INDEX(mr3, 0),
  PHY_UIM_PARAM_INDEX(mr4, 0),
  PHY_UIM_PARAM_INDEX(mr5, 0),
  PHY_UIM_PARAM_INDEX(mr6, 0),
  PHY_UIM_PARAM_INDEX(mr11, 0),
  PHY_UIM_PARAM_INDEX(mr12, 0),
  PHY_UIM_PARAM_INDEX(mr13, 0),
  PHY_UIM_PARAM_INDEX(mr14, 0),
  PHY_UIM_PARAM_INDEX(mr22, 0),
};

#define PHY_UIS_PARAM_INDEX(x) \
  { \
    .offset = offsetof(user_input_swizzle_t, swizzle[(x)]), \
    .par_offset = offsetof(HAL_DDR_SwizzleUiTypeDef, swizzle[(x)]) \
  }

static const reg_desc_t phy_uis_desc[] =
{
  PHY_UIS_PARAM_INDEX(0),
  PHY_UIS_PARAM_INDEX(1),
  PHY_UIS_PARAM_INDEX(2),
  PHY_UIS_PARAM_INDEX(3),
  PHY_UIS_PARAM_INDEX(4),
  PHY_UIS_PARAM_INDEX(5),
  PHY_UIS_PARAM_INDEX(6),
  PHY_UIS_PARAM_INDEX(7),
  PHY_UIS_PARAM_INDEX(8),
  PHY_UIS_PARAM_INDEX(9),
  PHY_UIS_PARAM_INDEX(10),
  PHY_UIS_PARAM_INDEX(11),
  PHY_UIS_PARAM_INDEX(12),
  PHY_UIS_PARAM_INDEX(13),
  PHY_UIS_PARAM_INDEX(14),
  PHY_UIS_PARAM_INDEX(15),
  PHY_UIS_PARAM_INDEX(16),
  PHY_UIS_PARAM_INDEX(17),
  PHY_UIS_PARAM_INDEX(18),
  PHY_UIS_PARAM_INDEX(19),
  PHY_UIS_PARAM_INDEX(20),
  PHY_UIS_PARAM_INDEX(21),
  PHY_UIS_PARAM_INDEX(22),
  PHY_UIS_PARAM_INDEX(23),
  PHY_UIS_PARAM_INDEX(24),
  PHY_UIS_PARAM_INDEX(25),
  PHY_UIS_PARAM_INDEX(26),
  PHY_UIS_PARAM_INDEX(27),
  PHY_UIS_PARAM_INDEX(28),
  PHY_UIS_PARAM_INDEX(29),
  PHY_UIS_PARAM_INDEX(30),
  PHY_UIS_PARAM_INDEX(31),
  PHY_UIS_PARAM_INDEX(32),
  PHY_UIS_PARAM_INDEX(33),
  PHY_UIS_PARAM_INDEX(34),
  PHY_UIS_PARAM_INDEX(35),
  PHY_UIS_PARAM_INDEX(36),
  PHY_UIS_PARAM_INDEX(37),
  PHY_UIS_PARAM_INDEX(38),
  PHY_UIS_PARAM_INDEX(39),
  PHY_UIS_PARAM_INDEX(40),
  PHY_UIS_PARAM_INDEX(41),
  PHY_UIS_PARAM_INDEX(42),
  PHY_UIS_PARAM_INDEX(43),
};

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
static const ddr_reg_info_t ddr_registers[REG_TYPE_NB] =
{
  [REG_REG] = {
    .name = "static",
    .desc = ddr_reg_desc,
    .size = ARRAY_SIZE(ddr_reg_desc),
    .base = BASE_DDRCTRL
  },
  [REG_TIMING] = {
    .name = "timing",
    .desc = ddr_timing_desc,
    .size = ARRAY_SIZE(ddr_timing_desc),
    .base = BASE_DDRCTRL
  },
  [REG_PERF] = {
    .name = "perf",
    .desc = ddr_perf_desc,
    .size = ARRAY_SIZE(ddr_perf_desc),
    .base = BASE_DDRCTRL
  },
  [REG_MAP] = {
    .name = "map",
    .desc = ddr_map_desc,
    .size = ARRAY_SIZE(ddr_map_desc),
    .base = BASE_DDRCTRL
  },
  [PHY_UI_BASIC] = {
    .name = "basic",
    .desc = phy_uib_desc,
    .size = ARRAY_SIZE(phy_uib_desc),
    .base = BASE_PHY_UI_BASIC
  },
  [PHY_UI_ADVANCED] = {
    .name = "advanced",
    .desc = phy_uia_desc,
    .size = ARRAY_SIZE(phy_uia_desc),
    .base = BASE_PHY_UI_ADVANCED
  },
  [PHY_UI_MODE_REGISTER] = {
    .name = "mode_register",
    .desc = phy_uim_desc,
    .size = ARRAY_SIZE(phy_uim_desc),
    .base = BASE_PHY_UI_MODE_REGISTER
  },
  [PHY_UI_SWIZZLE] = {
    .name = "swizzle",
    .desc = phy_uis_desc,
    .size = ARRAY_SIZE(phy_uis_desc),
    .base = BASE_PHY_UI_SWIZZLE
  }
};

#ifdef STM32MP_GATHER_DDRCTRL_SETTING_IN_STATIC_ARRAY
const __attribute__((section (".STATIC_PARAMS"))) uint32_t tab_static_param[] =
{
  DDR_MSTR,
  DDR_MRCTRL0,
  DDR_MRCTRL1,
  DDR_MRCTRL2,
  DDR_DERATEEN,
  DDR_DERATEINT,
  DDR_DERATECTL,
  DDR_PWRCTL,
  DDR_PWRTMG,
  DDR_HWLPCTL,
  DDR_RFSHCTL0,
  DDR_RFSHCTL1,
  DDR_RFSHCTL3,
  DDR_CRCPARCTL0,
  DDR_CRCPARCTL1,
  DDR_INIT0,
  DDR_INIT1,
  DDR_INIT2,
  DDR_INIT3,
  DDR_INIT4,
  DDR_INIT5,
  DDR_INIT6,
  DDR_INIT7,
  DDR_DIMMCTL,
  DDR_RANKCTL,
  DDR_RANKCTL1,
  DDR_ZQCTL0,
  DDR_ZQCTL1,
  DDR_ZQCTL2,
  DDR_DFITMG0,
  DDR_DFITMG1,
  DDR_DFILPCFG0,
  DDR_DFILPCFG1,
  DDR_DFIUPD0,
  DDR_DFIUPD1,
  DDR_DFIUPD2,
  DDR_DFIMISC,
  DDR_DFITMG2,
  DDR_DFITMG3,
  DDR_DBICTL,
  DDR_DFIPHYMSTR,
  DDR_DBG0,
  DDR_DBG1,
  DDR_DBGCMD,
  DDR_SWCTL,
  DDR_SWCTLSTATIC,
  DDR_POISONCFG,
  DDR_PCCFG,
  DDR_RFSHTMG,
  DDR_RFSHTMG1,
  DDR_DRAMTMG0,
  DDR_DRAMTMG1,
  DDR_DRAMTMG2,
  DDR_DRAMTMG3,
  DDR_DRAMTMG4,
  DDR_DRAMTMG5,
  DDR_DRAMTMG6,
  DDR_DRAMTMG7,
  DDR_DRAMTMG8,
  DDR_DRAMTMG9,
  DDR_DRAMTMG10,
  DDR_DRAMTMG11,
  DDR_DRAMTMG12,
  DDR_DRAMTMG13,
  DDR_DRAMTMG14,
  DDR_DRAMTMG15,
  DDR_ODTCFG,
  DDR_ODTMAP,
  DDR_ADDRMAP0,
  DDR_ADDRMAP1,
  DDR_ADDRMAP2,
  DDR_ADDRMAP3,
  DDR_ADDRMAP4,
  DDR_ADDRMAP5,
  DDR_ADDRMAP6,
  DDR_ADDRMAP7,
  DDR_ADDRMAP8,
  DDR_ADDRMAP9,
  DDR_ADDRMAP10,
  DDR_ADDRMAP11,
  DDR_SCHED,
  DDR_SCHED1,
  DDR_PERFHPR1,
  DDR_PERFLPR1,
  DDR_PERFWR1,
  DDR_SCHED3,
  DDR_SCHED4,
  DDR_PCFGR_0,
  DDR_PCFGW_0,
  DDR_PCTRL_0,
  DDR_PCFGQOS0_0,
  DDR_PCFGQOS1_0,
  DDR_PCFGWQOS0_0,
  DDR_PCFGWQOS1_0,
#if STM32MP_DDR_DUAL_AXI_PORT
  DDR_PCFGR_1,
  DDR_PCFGW_1,
  DDR_PCTRL_1,
  DDR_PCFGQOS0_1,
  DDR_PCFGQOS1_1,
  DDR_PCFGWQOS0_1,
  DDR_PCFGWQOS1_1,
#endif /* STM32MP_DDR_DUAL_AXI_PORT */
  DDR_UIA_PHYMSTRTRAININTERVAL_0,
};
#endif /* STM32MP_GATHER_DDRCTRL_SETTING_IN_STATIC_ARRAY */

#ifdef __AARCH64__
#define DDR_PATTERN                          0xAAAAAAAAAAAAAAAAU
#define DDR_ANTIPATTERN                      0x5555555555555555U
#else /* __AARCH64__ */
#define DDR_PATTERN                          0xAAAAAAAAU
#define DDR_ANTIPATTERN                      0x55555555U
#endif /* __AARCH64__ */

#define DDR_DELAY_1_US                       1U
#define DDR_TIMEOUT_1_US                     1U
#define DDR_TIMEOUT_500_US                   500U * DDR_TIMEOUT_1_US
#define DDR_TIMEOUT_US_1S                    1000000U * DDR_TIMEOUT_1_US

/* DDRCTRL register fields */
#define DDRC_STAT_OPERATING_MODE_SR       (DDRC_STAT_OPERATING_MODE_0 | \
                                           DDRC_STAT_OPERATING_MODE_1)
#define DDRC_STAT_OPERATING_MODE_NORMAL   DDRC_STAT_OPERATING_MODE_0
#define DDRC_STAT_SELFREF_TYPE_ASR        (DDRC_STAT_SELFREF_TYPE_0 | \
                                           DDRC_STAT_SELFREF_TYPE_1)
#define DDRC_STAT_SELFREF_TYPE_SR         DDRC_STAT_SELFREF_TYPE_1
#define DDRC_STAT_SELFREF_STATE_SRPD      DDRC_STAT_SELFREF_STATE_1
#define DDRC_DBGCAM_DATA_PIPELINE_EMPTY \
  (DDRC_DBGCAM_WR_DATA_PIPELINE_EMPTY | \
  DDRC_DBGCAM_RD_DATA_PIPELINE_EMPTY)
#define DDRC_DBGCAM_Q_AND_DATA_PIPELINE_EMPTY \
  (DDRC_DBGCAM_DBG_WR_Q_EMPTY | \
   DDRC_DBGCAM_DBG_RD_Q_EMPTY | \
   DDRC_DBGCAM_DATA_PIPELINE_EMPTY)

/* DDRPHY registers */
#define DDRPHY_INITENG0_P0_SEQ0BDISABLEFLAG6 0x240004U
#define DDRPHY_INITENG0_P0_PHYINLPX          0x2400A0U
#define DDRPHY_DRTUB0_UCCLKHCLKENABLES       0x300200U
#define DDRPHY_APBONLY0_MICROCONTMUXSEL      0x340000U

/* DDRPHY register fields */
#define DDRPHY_INITENG0_P0_PHYINLPX_PHYINLP3            (1U << 0)
#define DDRPHY_DRTUB0_UCCLKHCLKENABLES_UCCLKEN          (1U << 0)
#define DDRPHY_DRTUB0_UCCLKHCLKENABLES_HCLKEN           (1U << 1)
#define DDRPHY_APBONLY0_MICROCONTMUXSEL_MICROCONTMUXSEL (1U << 0)

/* Other register fields */
#define RCC_DDRITFCFGR_DDRCKMOD_HSR          (0x2UL << 4)

/* HW idle period (unit: Multiples of 32 DFI clock cycles) */
#define HW_IDLE_PERIOD                       0x3U

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
HAL_DDR_SelfRefreshModeTypeDef sr_mode = DFLT_SR_MODE;

HAL_DDR_ConfigTypeDef static_ddr_config =
{
  .info = {
    .name  = DDR_MEM_NAME,
    .speed = DDR_MEM_SPEED,
    .size  = DDR_MEM_SIZE
  },

#ifndef STM32MP_GATHER_DDRCTRL_SETTING_IN_STATIC_ARRAY
  .c_reg = {
    .MSTR       = DDR_MSTR,
    .MRCTRL0    = DDR_MRCTRL0,
    .MRCTRL1    = DDR_MRCTRL1,
    .MRCTRL2    = DDR_MRCTRL2,
    .DERATEEN   = DDR_DERATEEN,
    .DERATEINT  = DDR_DERATEINT,
    .DERATECTL  = DDR_DERATECTL,
    .PWRCTL     = DDR_PWRCTL,
    .PWRTMG     = DDR_PWRTMG,
    .HWLPCTL    = DDR_HWLPCTL,
    .RFSHCTL0   = DDR_RFSHCTL0,
    .RFSHCTL1   = DDR_RFSHCTL1,
    .RFSHCTL3   = DDR_RFSHCTL3,
    .CRCPARCTL0 = DDR_CRCPARCTL0,
    .CRCPARCTL1 = DDR_CRCPARCTL1,
    .INIT0      = DDR_INIT0,
    .INIT1      = DDR_INIT1,
    .INIT2      = DDR_INIT2,
    .INIT3      = DDR_INIT3,
    .INIT4      = DDR_INIT4,
    .INIT5      = DDR_INIT5,
    .INIT6      = DDR_INIT6,
    .INIT7      = DDR_INIT7,
    .DIMMCTL    = DDR_DIMMCTL,
    .RANKCTL    = DDR_RANKCTL,
    .RANKCTL1    = DDR_RANKCTL1,
    .ZQCTL0     = DDR_ZQCTL0,
    .ZQCTL1     = DDR_ZQCTL1,
    .ZQCTL2     = DDR_ZQCTL2,
    .DFITMG0    = DDR_DFITMG0,
    .DFITMG1    = DDR_DFITMG1,
    .DFILPCFG0  = DDR_DFILPCFG0,
    .DFILPCFG1  = DDR_DFILPCFG1,
    .DFIUPD0    = DDR_DFIUPD0,
    .DFIUPD1    = DDR_DFIUPD1,
    .DFIUPD2    = DDR_DFIUPD2,
    .DFIMISC    = DDR_DFIMISC,
    .DFITMG2    = DDR_DFITMG2,
    .DFITMG3    = DDR_DFITMG3,
    .DBICTL     = DDR_DBICTL,
    .DFIPHYMSTR = DDR_DFIPHYMSTR,
    .DBG0       = DDR_DBG0,
    .DBG1       = DDR_DBG1,
    .DBGCMD     = DDR_DBGCMD,
    .SWCTL      = DDR_SWCTL,
    .SWCTLSTATIC = DDR_SWCTLSTATIC,
    .POISONCFG  = DDR_POISONCFG,
    .PCCFG      = DDR_PCCFG
  },

  .c_timing = {
    .RFSHTMG   = DDR_RFSHTMG,
    .RFSHTMG1  = DDR_RFSHTMG1,
    .DRAMTMG0  = DDR_DRAMTMG0,
    .DRAMTMG1  = DDR_DRAMTMG1,
    .DRAMTMG2  = DDR_DRAMTMG2,
    .DRAMTMG3  = DDR_DRAMTMG3,
    .DRAMTMG4  = DDR_DRAMTMG4,
    .DRAMTMG5  = DDR_DRAMTMG5,
    .DRAMTMG6  = DDR_DRAMTMG6,
    .DRAMTMG7  = DDR_DRAMTMG7,
    .DRAMTMG8  = DDR_DRAMTMG8,
    .DRAMTMG9  = DDR_DRAMTMG9,
    .DRAMTMG10 = DDR_DRAMTMG10,
    .DRAMTMG11 = DDR_DRAMTMG11,
    .DRAMTMG12 = DDR_DRAMTMG12,
    .DRAMTMG13 = DDR_DRAMTMG13,
    .DRAMTMG14 = DDR_DRAMTMG14,
    .DRAMTMG15 = DDR_DRAMTMG15,
    .ODTCFG    = DDR_ODTCFG,
    .ODTMAP    = DDR_ODTMAP
  },

  .c_map = {
    .ADDRMAP0  = DDR_ADDRMAP0,
    .ADDRMAP1  = DDR_ADDRMAP1,
    .ADDRMAP2  = DDR_ADDRMAP2,
    .ADDRMAP3  = DDR_ADDRMAP3,
    .ADDRMAP4  = DDR_ADDRMAP4,
    .ADDRMAP5  = DDR_ADDRMAP5,
    .ADDRMAP6  = DDR_ADDRMAP6,
    .ADDRMAP7  = DDR_ADDRMAP7,
    .ADDRMAP8  = DDR_ADDRMAP8,
    .ADDRMAP9  = DDR_ADDRMAP9,
    .ADDRMAP10 = DDR_ADDRMAP10,
    .ADDRMAP11 = DDR_ADDRMAP11
  },

  .c_perf = {
    .SCHED       = DDR_SCHED,
    .SCHED1      = DDR_SCHED1,
    .PERFHPR1    = DDR_PERFHPR1,
    .PERFLPR1    = DDR_PERFLPR1,
    .PERFWR1     = DDR_PERFWR1,
    .SCHED3      = DDR_SCHED3,
    .SCHED4      = DDR_SCHED4,
    .PCFGR_0     = DDR_PCFGR_0,
    .PCFGW_0     = DDR_PCFGW_0,
    .PCTRL_0     = DDR_PCTRL_0,
    .PCFGQOS0_0  = DDR_PCFGQOS0_0,
    .PCFGQOS1_0  = DDR_PCFGQOS1_0,
    .PCFGWQOS0_0 = DDR_PCFGWQOS0_0,
    .PCFGWQOS1_0 = DDR_PCFGWQOS1_0,
#if STM32MP_DDR_DUAL_AXI_PORT
    .PCFGR_1     = DDR_PCFGR_1,
    .PCFGW_1     = DDR_PCFGW_1,
    .PCTRL_1     = DDR_PCTRL_1,
    .PCFGQOS0_1  = DDR_PCFGQOS0_1,
    .PCFGQOS1_1  = DDR_PCFGQOS1_1,
    .PCFGWQOS0_1 = DDR_PCFGWQOS0_1,
    .PCFGWQOS1_1 = DDR_PCFGWQOS1_1
#endif /* STM32MP_DDR_DUAL_AXI_PORT */
  },
#endif /* !STM32MP_GATHER_DDRCTRL_SETTING_IN_STATIC_ARRAY */

  .p_uib = {
    .dramtype           = DDR_UIB_DRAMTYPE,
    .dimmtype           = DDR_UIB_DIMMTYPE,
    .lp4xmode           = DDR_UIB_LP4XMODE,
    .numdbyte           = DDR_UIB_NUMDBYTE,
    .numactivedbytedfi0 = DDR_UIB_NUMACTIVEDBYTEDFI0,
    .numactivedbytedfi1 = DDR_UIB_NUMACTIVEDBYTEDFI1,
    .numanib            = DDR_UIB_NUMANIB,
    .numrank_dfi0       = DDR_UIB_NUMRANK_DFI0,
    .numrank_dfi1       = DDR_UIB_NUMRANK_DFI1,
    .dramdatawidth      = DDR_UIB_DRAMDATAWIDTH,
    .numpstates         = DDR_UIB_NUMPSTATES,
    .frequency[0]       = DDR_UIB_FREQUENCY_0,
    .pllbypass[0]       = DDR_UIB_PLLBYPASS_0,
    .dfifreqratio[0]    = DDR_UIB_DFIFREQRATIO_0,
    .dfi1exists         = DDR_UIB_DFI1EXISTS,
    .train2d            = DDR_UIB_TRAIN2D,
    .hardmacrover       = DDR_UIB_HARDMACROVER,
    .readdbienable[0]   = DDR_UIB_READDBIENABLE_0,
    .dfimode            = DDR_UIB_DFIMODE
  },

  .p_uia = {
    .lp4rxpreamblemode[0]    = DDR_UIA_LP4RXPREAMBLEMODE_0,
    .lp4postambleext[0]      = DDR_UIA_LP4POSTAMBLEEXT_0,
    .d4rxpreamblelength[0]   = DDR_UIA_D4RXPREAMBLELENGTH_0,
    .d4txpreamblelength[0]   = DDR_UIA_D4TXPREAMBLELENGTH_0,
    .extcalresval            = DDR_UIA_EXTCALRESVAL,
    .is2ttiming[0]           = DDR_UIA_IS2TTIMING_0,
    .odtimpedance[0]         = DDR_UIA_ODTIMPEDANCE_0,
    .tximpedance[0]          = DDR_UIA_TXIMPEDANCE_0,
    .atximpedance            = DDR_UIA_ATXIMPEDANCE,
    .memalerten              = DDR_UIA_MEMALERTEN,
    .memalertpuimp           = DDR_UIA_MEMALERTPUIMP,
    .memalertvreflevel       = DDR_UIA_MEMALERTVREFLEVEL,
    .memalertsyncbypass      = DDR_UIA_MEMALERTSYNCBYPASS,
    .disdynadrtri[0]         = DDR_UIA_DISDYNADRTRI_0,
    .phymstrtraininterval[0] = DDR_UIA_PHYMSTRTRAININTERVAL_0,
    .phymstrmaxreqtoack[0]   = DDR_UIA_PHYMSTRMAXREQTOACK_0,
    .wdqsext                 = DDR_UIA_WDQSEXT,
    .calinterval             = DDR_UIA_CALINTERVAL,
    .calonce                 = DDR_UIA_CALONCE,
    .lp4rl[0]                = DDR_UIA_LP4RL_0,
    .lp4wl[0]                = DDR_UIA_LP4WL_0,
    .lp4wls[0]               = DDR_UIA_LP4WLS_0,
    .lp4dbird[0]             = DDR_UIA_LP4DBIRD_0,
    .lp4dbiwr[0]             = DDR_UIA_LP4DBIWR_0,
    .lp4nwr[0]               = DDR_UIA_LP4NWR_0,
    .lp4lowpowerdrv          = DDR_UIA_LP4LOWPOWERDRV,
    .drambyteswap            = DDR_UIA_DRAMBYTESWAP,
    .rxenbackoff             = DDR_UIA_RXENBACKOFF,
    .trainsequencectrl       = DDR_UIA_TRAINSEQUENCECTRL,
    .snpsumctlopt            = DDR_UIA_SNPSUMCTLOPT,
    .snpsumctlf0rc5x[0]      = DDR_UIA_SNPSUMCTLF0RC5X_0,
    .txslewrisedq[0]         = DDR_UIA_TXSLEWRISEDQ_0,
    .txslewfalldq[0]         = DDR_UIA_TXSLEWFALLDQ_0,
    .txslewriseac            = DDR_UIA_TXSLEWRISEAC,
    .txslewfallac            = DDR_UIA_TXSLEWFALLAC,
    .disableretraining       = DDR_UIA_DISABLERETRAINING,
    .disablephyupdate        = DDR_UIA_DISABLEPHYUPDATE,
    .enablehighclkskewfix    = DDR_UIA_ENABLEHIGHCLKSKEWFIX,
    .disableunusedaddrlns    = DDR_UIA_DISABLEUNUSEDADDRLNS,
    .phyinitsequencenum      = DDR_UIA_PHYINITSEQUENCENUM,
    .enabledficspolarityfix  = DDR_UIA_ENABLEDFICSPOLARITYFIX,
    .phyvref                 = DDR_UIA_PHYVREF,
    .sequencectrl[0]         = DDR_UIA_SEQUENCECTRL_0
  },

  .p_uim = {
    .mr0[0]  = DDR_UIM_MR0_0,
    .mr1[0]  = DDR_UIM_MR1_0,
    .mr2[0]  = DDR_UIM_MR2_0,
    .mr3[0]  = DDR_UIM_MR3_0,
    .mr4[0]  = DDR_UIM_MR4_0,
    .mr5[0]  = DDR_UIM_MR5_0,
    .mr6[0]  = DDR_UIM_MR6_0,
    .mr11[0] = DDR_UIM_MR11_0,
    .mr12[0] = DDR_UIM_MR12_0,
    .mr13[0] = DDR_UIM_MR13_0,
    .mr14[0] = DDR_UIM_MR14_0,
    .mr22[0] = DDR_UIM_MR22_0
  },

  .p_uis = {
    .swizzle[0]  = DDR_UIS_SWIZZLE_0,
    .swizzle[1]  = DDR_UIS_SWIZZLE_1,
    .swizzle[2]  = DDR_UIS_SWIZZLE_2,
    .swizzle[3]  = DDR_UIS_SWIZZLE_3,
    .swizzle[4]  = DDR_UIS_SWIZZLE_4,
    .swizzle[5]  = DDR_UIS_SWIZZLE_5,
    .swizzle[6]  = DDR_UIS_SWIZZLE_6,
    .swizzle[7]  = DDR_UIS_SWIZZLE_7,
    .swizzle[8]  = DDR_UIS_SWIZZLE_8,
    .swizzle[9]  = DDR_UIS_SWIZZLE_9,
    .swizzle[10] = DDR_UIS_SWIZZLE_10,
    .swizzle[11] = DDR_UIS_SWIZZLE_11,
    .swizzle[12] = DDR_UIS_SWIZZLE_12,
    .swizzle[13] = DDR_UIS_SWIZZLE_13,
    .swizzle[14] = DDR_UIS_SWIZZLE_14,
    .swizzle[15] = DDR_UIS_SWIZZLE_15,
    .swizzle[16] = DDR_UIS_SWIZZLE_16,
    .swizzle[17] = DDR_UIS_SWIZZLE_17,
    .swizzle[18] = DDR_UIS_SWIZZLE_18,
    .swizzle[19] = DDR_UIS_SWIZZLE_19,
    .swizzle[20] = DDR_UIS_SWIZZLE_20,
    .swizzle[21] = DDR_UIS_SWIZZLE_21,
    .swizzle[22] = DDR_UIS_SWIZZLE_22,
    .swizzle[23] = DDR_UIS_SWIZZLE_23,
    .swizzle[24] = DDR_UIS_SWIZZLE_24,
    .swizzle[25] = DDR_UIS_SWIZZLE_25,
    .swizzle[26] = DDR_UIS_SWIZZLE_26,
    .swizzle[27] = DDR_UIS_SWIZZLE_27,
    .swizzle[28] = DDR_UIS_SWIZZLE_28,
    .swizzle[29] = DDR_UIS_SWIZZLE_29,
    .swizzle[30] = DDR_UIS_SWIZZLE_30,
    .swizzle[31] = DDR_UIS_SWIZZLE_31,
    .swizzle[32] = DDR_UIS_SWIZZLE_32,
    .swizzle[33] = DDR_UIS_SWIZZLE_33,
    .swizzle[34] = DDR_UIS_SWIZZLE_34,
    .swizzle[35] = DDR_UIS_SWIZZLE_35,
    .swizzle[36] = DDR_UIS_SWIZZLE_36,
    .swizzle[37] = DDR_UIS_SWIZZLE_37,
    .swizzle[38] = DDR_UIS_SWIZZLE_38,
    .swizzle[39] = DDR_UIS_SWIZZLE_39,
    .swizzle[40] = DDR_UIS_SWIZZLE_40,
    .swizzle[41] = DDR_UIS_SWIZZLE_41,
    .swizzle[42] = DDR_UIS_SWIZZLE_42,
    .swizzle[43] = DDR_UIS_SWIZZLE_43
  }
};

static bool axi_port_reenable_request;
static bool host_interface_reenable_request;
static uint32_t sem_mutex_save;

/* Private function prototypes -----------------------------------------------*/

/**
  * @brief  Board specific DDR power initialization (weak here)
  * @param  type DDR type (unused here)
  * @retval 0 if success
  */
__weak int32_t HAL_DDR_MspInit(__unused ddr_type type)
{
  return 0;
}

/**
  * @brief  This function tests a simple read/write access to the DDR.
  *         Note that the previous content is restored after test.
  * @param  None
  * @retval 0 if success, and address value else
  */
static uintptr_t ddr_test_rw_access(void)
{
  uintptr_t saved_value = *(uintptr_t *)DDR_MEM_BASE;
  uintptr_t *addr = (uintptr_t *)DDR_MEM_BASE;

  *addr = DDR_PATTERN;

  if (*addr != DDR_PATTERN)
  {
    return (uintptr_t)addr;
  }

  if (saved_value != DDR_PATTERN)
  {
    *addr = saved_value;
  }

  return 0UL;
}

/**
  * @brief  This function tests the DDR data bus wiring.
  *         This is inspired from the Data Bus Test algorithm written by Michael Barr
  *         in "Programming Embedded Systems in C and C++" book.
  *         resources.oreilly.com/examples/9781565923546/blob/master/Chapter6/
  *         File: memtest.c - This source code belongs to Public Domain.
  * @param  None
  * @retval 0 if success, and address value else
  */
static uintptr_t ddr_test_data_bus(void)
{
  uint32_t i;
  uintptr_t pattern;
  uintptr_t *addr = (uintptr_t *)DDR_MEM_BASE;

  for (i = 0U; i < 64U; i++)
  {
    pattern = 1 << i;

    *addr = pattern;

    if (*addr != pattern)
    {
      return (uintptr_t)addr;
    }
  }

  return 0UL;
}

/**
  * @brief  This function tests the DDR address bus wiring.
  *         This is inspired from the Data Bus Test algorithm written by Michael Barr
  *         in "Programming Embedded Systems in C and C++" book.
  *         resources.oreilly.com/examples/9781565923546/blob/master/Chapter6/
  *         File: memtest.c - This source code belongs to Public Domain.
  * @param  None
  * @retval 0 if success, and address value else
  */
static uintptr_t ddr_test_addr_bus(void)
{
  uintptr_t addressmask = (static_ddr_config.info.size - 1U);
  uintptr_t offset;
  uintptr_t testoffset = 0UL;
  uintptr_t *addr = (uintptr_t *)DDR_MEM_BASE;
  uintptr_t ret = 0UL;

  /* Write the default pattern at each of the power-of-two offsets. */
  for (offset = sizeof(uintptr_t); offset < addressmask; offset <<= 1)
  {
    *(addr + (offset / sizeof(uintptr_t))) = DDR_PATTERN;
  }

  /* Check for address bits stuck high. */
  *(addr + testoffset) = DDR_ANTIPATTERN;

  for (offset = sizeof(uintptr_t); offset < addressmask; offset <<= 1)
  {
    if (*(addr + (offset / sizeof(uintptr_t))) != DDR_PATTERN)
    {
      return (uintptr_t)(addr + offset / sizeof(uintptr_t));
    }
  }

  *(addr + testoffset) = DDR_PATTERN;

  /* Check for address bits stuck low or shorted. */
  for (testoffset = sizeof(uintptr_t); testoffset < addressmask;
       testoffset <<= 1)
  {
    *(addr + (testoffset / sizeof(uintptr_t))) = DDR_ANTIPATTERN;

    if (*addr != DDR_PATTERN)
    {
      return (uintptr_t)addr;
    }

    for (offset = sizeof(uintptr_t); offset < addressmask; offset <<= 1)
    {
      if ((*(addr + (offset / sizeof(uintptr_t))) != DDR_PATTERN) &&
          (offset != testoffset))
      {
        return (uintptr_t)(addr + (offset / sizeof(uintptr_t)));
      }
    }

    *(addr + (testoffset / sizeof(uintptr_t))) = DDR_PATTERN;
  }

  return ret;
}

/**
  * @brief  This function checks the DDR size. It has to be run with Data Cache off.
  *         This test is run before data have been put in DDR, and is only done for
  *         cold boot. The DDR data can then be overwritten, and it is not useful to
  *         restore its content.
  * @param  None
  * @retval DDR computed size
  */
static uintptr_t ddr_check_size(void)
{
  uintptr_t offset = sizeof(uintptr_t);
  uintptr_t *addr = (uintptr_t *)DDR_MEM_BASE;

  *addr = DDR_PATTERN;

  while (offset < static_ddr_config.info.size)
  {
    *(addr + (offset / sizeof(uintptr_t))) = DDR_ANTIPATTERN;
    __DSB();

    if (*addr != DDR_PATTERN)
    {
      break;
    }

    offset <<= 1;
  }

  return offset;
}

/**
  * @brief  Wait a delay expressed in us
  * @param  delay_us delay in us
  * @retval None
  */
static void ddr_delay_us(uint32_t delay_us)
{
  __IO uint32_t wait_loop_index = 0U;

  wait_loop_index = (delay_us * (SystemCoreClock / (1000000UL * 2UL)));

#ifdef USE_STM32MP257CXX_EMU
  wait_loop_index *= 10U;
#endif /* USE_STM32MP257CXX_EMU */

  while (wait_loop_index != 0UL)
  {
    wait_loop_index--;
  }
}

/**
  * @brief  Initialize DDR timeout in us
  * @param  timeout_us timeout in us
  * @retval wait_loop_index
  */
static __IO uint32_t ddr_timeout_init_us(uint32_t timeout_us)
{
  __IO uint32_t wait_loop_index = 0U;

  wait_loop_index = timeout_us * (SystemCoreClock / (1000000UL * 2UL));

#ifdef USE_STM32MP257CXX_EMU
  wait_loop_index *= 10U;
#endif /* USE_STM32MP257CXX_EMU */

  return wait_loop_index;
}

/**
  * @brief  Check if timeout has expired
  * @param  timeout timeout in us
  * @retval Timeout status
  */
static bool ddr_timeout_elapsed(__IO uint32_t timeout)
{
  if (timeout == 0U)
  {
    return true;
  }

  return false;
}

/**
  * @brief  Start quasi dynamic register update
  * @param  None
  * @retval None
  */
static void start_sw_done(void)
{
  CLEAR_BIT(DDRC->SWCTL, DDRC_SWCTL_SW_DONE);
}

/**
  * @brief  Wait quasi dynamic register update
  * @param  None
  * @retval 0 if success, -1 if timeout
  */
static int32_t wait_sw_done_ack(void)
{
  uint32_t swstat;
  __IO uint32_t timeout = ddr_timeout_init_us(DDR_TIMEOUT_1_US);

  SET_BIT(DDRC->SWCTL, DDRC_SWCTL_SW_DONE);

  do
  {
    swstat = READ_REG(DDRC->SWSTAT);

    timeout--;
    if (ddr_timeout_elapsed(timeout))
    {
      /* Timeout initialising DRAM */
      return -1;
    }
  }
  while ((swstat & DDRC_SWSTAT_SW_DONE_ACK) == 0U);

  return 0;
}

/**
  * @brief  Enable AXI ports
  * @param  None
  * @retval None
  */
static void enable_axi_port(void)
{
  /* Enable uMCTL2 AXI port 0 */
  SET_BIT(DDRC->PCTRL_0, DDRC_PCTRL_0_PORT_EN);

#if STM32MP_DDR_DUAL_AXI_PORT
  /* Enable uMCTL2 AXI port 1 */
  SET_BIT(DDRC->PCTRL_1, DDRC_PCTRL_1_PORT_EN);
#endif /* STM32MP_DDR_DUAL_AXI_PORT */
}

/**
  * @brief  Disable AXI ports
  * @param  None
  * @retval 0 if success, -1 if timeout
  */
static int32_t disable_axi_port(void)
{
  __IO uint32_t timeout;

  /* Disable uMCTL2 AXI port 0 */
  CLEAR_BIT(DDRC->PCTRL_0, DDRC_PCTRL_0_PORT_EN);
#ifdef STM32MP_DDR_DUAL_AXI_PORT
  /* Disable uMCTL2 AXI port 1 */
  CLEAR_BIT(DDRC->PCTRL_1, DDRC_PCTRL_0_PORT_EN);
#endif /* STM32MP_DDR_DUAL_AXI_PORT */

  /*
   * Waits unit all AXI ports are idle
   * Poll PSTAT.rd_port_busy_n = 0
   * Poll PSTAT.wr_port_busy_n = 0
   */
  timeout = ddr_timeout_init_us(DDR_TIMEOUT_US_1S);
  while (READ_REG(DDRC->PSTAT) != 0U)
  {
    timeout--;
    if (ddr_timeout_elapsed(timeout))
    {
      return -1;
    }
  }

  return 0;
}

/**
  * @brief  Check if AXI ports are enabled.
  *         Only port 0 is checked, as always used.
  * @param  None
  * @retval true if enabled, false if not
  */
static bool ddr_is_axi_port_enabled(void)
{
  return (READ_REG(DDRC->PCTRL_0) & DDRC_PCTRL_0_PORT_EN) != 0U;
}

/**
  * @brief  Enable host interface
  * @param  None
  * @retval None
  */
static void enable_host_interface(void)
{
  CLEAR_BIT(DDRC->DBG1, DDRC_DBG1_DIS_HIF);
}

/**
  * @brief  Disable host interface
  * @param  None
  * @retval 0 if success, -1 if timeout
  */
static int32_t disable_host_interface(void)
{
  __IO uint32_t timeout;
  uint32_t dbgcam;
  uint32_t count = 0U;

  SET_BIT(DDRC->DBG1, DDRC_DBG1_DIS_HIF);

  /*
   * Waits until all queues and pipelines are empty
   * Poll DBGCAM.dbg_wr_q_empty = 1
   * Poll DBGCAM.dbg_rd_q_empty = 1
   * Poll DBGCAM.dbg_wr_data_pipeline_empty = 1
   * Poll DBGCAM.dbg_rd_data_pipeline_empty = 1
   *
   * data_pipeline fields must be polled twice to ensure
   * value propoagation, so count is added to loop condition.
   */
  timeout = ddr_timeout_init_us(DDR_TIMEOUT_US_1S);
  do
  {
    dbgcam = READ_REG(DDRC->DBGCAM);

    timeout--;
    if (ddr_timeout_elapsed(timeout))
    {
      return -1;
    }

    count++;
  } while (((dbgcam & DDRC_DBGCAM_Q_AND_DATA_PIPELINE_EMPTY) !=
            DDRC_DBGCAM_Q_AND_DATA_PIPELINE_EMPTY) || (count < 2U));

  return 0;
}

/**
  * @brief  Check if host interface is enabled.
  * @param  None
  * @retval true if enabled, false if not
  */
static bool ddr_is_host_interface_enabled(void)
{
  return (READ_REG(DDRC->DBG1) & DDRC_DBG1_DIS_HIF) == 0U;
}

/**
  * @brief  Software self refresh entry function
  * @param  None
  * @retval 0 if success, -1 if timeout
  */
static int32_t sw_selfref_entry(void)
{
  __IO uint32_t timeout;
  uint32_t stat;
  uint32_t operating_mode;
  uint32_t selref_type;

  /* SW Self-Refresh entry */
  SET_BIT(DDRC->PWRCTL, DDRC_PWRCTL_SELFREF_SW);

  /*
   * Wait operating mode change in self-refresh mode
   * with STAT.operating_mode[1:0]==11.
   * Ensure transition to self-refresh was due to software
   * by checking also that STAT.selfref_type[1:0]=2.
   */
  timeout = ddr_timeout_init_us(DDR_TIMEOUT_500_US);

  while (!ddr_timeout_elapsed(timeout))
  {
    stat = READ_REG(DDRC->STAT);
    operating_mode = stat & DDRC_STAT_OPERATING_MODE_Msk;
    selref_type = stat & DDRC_STAT_SELFREF_TYPE_Msk;
    timeout--;

    if ((operating_mode == DDRC_STAT_OPERATING_MODE_SR)
        && (selref_type == DDRC_STAT_SELFREF_TYPE_SR))
    {
      return 0;
    }
  }

  return -1;
}

/**
  * @brief  Software self refresh exit function
  * @param  None
  * @retval None
  */
static void sw_selfref_exit(void)
{
  CLEAR_BIT(DDRC->PWRCTL, DDRC_PWRCTL_SELFREF_SW);
}

/**
  * @brief  Set conditions for quasi dynamic group 3 register update
  * @param  None
  * @retval 0 if success, error code else
  */
static int32_t set_qd3_update_conditions(void)
{
  if (ddr_is_axi_port_enabled())
  {
    if (disable_axi_port() != 0)
    {
      return -1;
    }

    axi_port_reenable_request = true;
  }

  if (ddr_is_host_interface_enabled())
  {
    if (disable_host_interface() != 0)
    {
      return -1;
    }

    host_interface_reenable_request = true;
  }

  start_sw_done();

  return 0;
}

/**
  * @brief  Set conditions for quasi dynamic groups 1 and 3 register update
  * @param  None
  * @retval 0 if success, error code else
  */
static int32_t set_qd1_qd3_update_conditions(void)
{
  SET_BIT(DDRC->DBG1, DDRC_DBG1_DIS_DQ);

  return set_qd3_update_conditions();
}

/**
  * @brief  Unset conditions for quasi dynamic group 3 register update
  * @param  None
  * @retval 0 if success, error code else
  */
static int32_t unset_qd3_update_conditions(void)
{
  if (wait_sw_done_ack() != 0)
  {
    return -1;
  }

  if (host_interface_reenable_request)
  {
    enable_host_interface();
    host_interface_reenable_request = false;
  }

  if (axi_port_reenable_request)
  {
    enable_axi_port();
    axi_port_reenable_request = false;
  }

  return 0;
}

/**
  * @brief  Unset conditions for quasi dynamic groups 1 and 3 register update
  * @param  None
  * @retval 0 if success, error code else
  */
static int32_t unset_qd1_qd3_update_conditions(void)
{
  int32_t ret = unset_qd3_update_conditions();

  if (ret == 0)
  {
      CLEAR_BIT(DDRC->DBG1, DDRC_DBG1_DIS_DQ);
  }

  return ret;
}

/**
  * @brief  Waits for DFI init completion
  * @param  None
  * @retval 0 if success, error code else
  */
static int32_t wait_dfi_init_complete(void)
{
  __IO uint32_t timeout;
  uint32_t dfistat;

  timeout = ddr_timeout_init_us(DDR_TIMEOUT_US_1S);
  do
  {
    dfistat = READ_REG(DDRC->DFISTAT);

    timeout--;
    if (ddr_timeout_elapsed(timeout))
    {
      return -1;
    }
  } while ((dfistat & DDRC_DFISTAT_DFI_INIT_COMPLETE) == 0U);

  return 0;
}

/**
  * @brief  Disables DFI low power interface
  * @param  None
  * @retval 0 if success, error code else
  */
static int32_t disable_dfi_low_power_interface(void)
{
  __IO uint32_t timeout;
  uint32_t dfistat;
  uint32_t stat;

  CLEAR_BIT(DDRC->DFILPCFG0, DDRC_DFILPCFG0_DFI_LP_EN_SR);

  timeout = ddr_timeout_init_us(DDR_TIMEOUT_US_1S);
  do
  {
    dfistat = READ_REG(DDRC->DFISTAT);
    stat = READ_REG(DDRC->STAT);

    timeout--;
    if (ddr_timeout_elapsed(timeout))
    {
      return -1;
    }
  } while (((dfistat & DDRC_DFISTAT_DFI_LP_ACK) != 0U) ||
           ((stat & DDRC_STAT_OPERATING_MODE) == DDRC_STAT_OPERATING_MODE_SR));

  return 0;
}

/**
  * @brief  Acitivates DDR controller
  * @param  sr_entry indicates if self refresh entry sequence in progress
  * @retval 0 if success, error code else
  */
static int32_t activate_controller(bool sr_entry)
{
  if (set_qd1_qd3_update_conditions() != 0)
  {
    return -1;
  }

  if (sr_entry)
  {
    SET_BIT(DDRC->DFIMISC, DDRC_DFIMISC_DFI_FREQUENCY);
  }
  else
  {
    CLEAR_BIT(DDRC->DFIMISC, DDRC_DFIMISC_DFI_FREQUENCY);
  }

  do
  {
    SET_BIT(DDRC->DFIMISC, DDRC_DFIMISC_DFI_INIT_START);
  } while ((READ_REG(DDRC->DFIMISC) & DDRC_DFIMISC_DFI_INIT_START) == 0U);

  CLEAR_BIT(DDRC->DFIMISC, DDRC_DFIMISC_DFI_INIT_START);

  if (wait_dfi_init_complete() != 0)
  {
    return -1;
  }

  ddr_delay_us(DDR_DELAY_1_US);

  if (sr_entry)
  {
    CLEAR_BIT(DDRC->DFIMISC, DDRC_DFIMISC_DFI_INIT_COMPLETE_EN);
  }
  else
  {
    SET_BIT(DDRC->DFIMISC, DDRC_DFIMISC_DFI_INIT_COMPLETE_EN);
  }

  ddr_delay_us(DDR_DELAY_1_US);

  if (unset_qd1_qd3_update_conditions() != 0)
  {
    return -1;
  }

  return 0;
}

/**
  * @brief  Waits deepest low power mode LP3 entry or exit
  * @param  state expected state
  * @retval 0 if success, error code else
  */
static int32_t wait_lp3_mode(bool state)
{
  __IO uint32_t timeout;
  uint16_t phyinlpx;
  bool repeat_loop = false;

  /* Enable APB access to internal CSR registers */
  WRITE_REG(*(volatile uint32_t *)(DDRPHYC_BASE + DDRPHY_APBONLY0_MICROCONTMUXSEL), 0U);
  WRITE_REG(*(volatile uint32_t *)(DDRPHYC_BASE + DDRPHY_DRTUB0_UCCLKHCLKENABLES),
            DDRPHY_DRTUB0_UCCLKHCLKENABLES_UCCLKEN | DDRPHY_DRTUB0_UCCLKHCLKENABLES_HCLKEN);

  timeout = ddr_timeout_init_us(DDR_TIMEOUT_US_1S);
  do
  {
    phyinlpx = READ_REG(*(volatile uint32_t *)(DDRPHYC_BASE + DDRPHY_INITENG0_P0_PHYINLPX));

    timeout--;
    if (ddr_timeout_elapsed(timeout))
    {
      return -1;
    }

    if (state)
    {
      repeat_loop = (phyinlpx & DDRPHY_INITENG0_P0_PHYINLPX_PHYINLP3) == 0U;
    }
    else
    {
      repeat_loop = ((phyinlpx & DDRPHY_INITENG0_P0_PHYINLPX_PHYINLP3) != 0U);
    }
  } while (repeat_loop);

  /* Disable APB access to internal CSR registers */
#if STM32MP_DDR3_TYPE || STM32MP_DDR4_TYPE
  WRITE_REG(*(volatile uint32_t *)(DDRPHYC_BASE + DDRPHY_DRTUB0_UCCLKHCLKENABLES), 0U);
#elif STM32MP_LPDDR4_TYPE
  WRITE_REG(*(volatile uint32_t *)(DDRPHYC_BASE + DDRPHY_DRTUB0_UCCLKHCLKENABLES),
            DDRPHY_DRTUB0_UCCLKHCLKENABLES_HCLKEN);
#endif /* STM32MP_LPDDR4_TYPE */
  WRITE_REG(*(volatile uint32_t *)(DDRPHYC_BASE + DDRPHY_APBONLY0_MICROCONTMUXSEL),
            DDRPHY_APBONLY0_MICROCONTMUXSEL_MICROCONTMUXSEL);

  return 0;
}

#if STM32MP_LPDDR4_TYPE
/**
  * @brief  Disables DDR drift compensation
  * @param  None
  * @retval None
  */
static void disable_phy_ddc(void)
{
  /* Enable APB access to internal CSR registers */
  WRITE_REG(*(volatile uint32_t *)(DDRPHYC_BASE + DDRPHY_APBONLY0_MICROCONTMUXSEL), 0U);
  WRITE_REG(*(volatile uint32_t *)(DDRPHYC_BASE + DDRPHY_DRTUB0_UCCLKHCLKENABLES),
            DDRPHY_DRTUB0_UCCLKHCLKENABLES_UCCLKEN | DDRPHY_DRTUB0_UCCLKHCLKENABLES_HCLKEN);

  /* Disable DRAM drift compensation */
  WRITE_REG(*(volatile uint32_t *)(DDRPHYC_BASE + DDRPHY_INITENG0_P0_SEQ0BDISABLEFLAG6),
            0xFFFFU);

  /* Disable APB access to internal CSR registers */
  WRITE_REG(*(volatile uint32_t *)(DDRPHYC_BASE + DDRPHY_DRTUB0_UCCLKHCLKENABLES),
            DDRPHY_DRTUB0_UCCLKHCLKENABLES_HCLKEN);
  WRITE_REG(*(volatile uint32_t *)(DDRPHYC_BASE + DDRPHY_APBONLY0_MICROCONTMUXSEL),
            DDRPHY_APBONLY0_MICROCONTMUXSEL_MICROCONTMUXSEL);
}
#endif /* STM32MP_LPDDR4_TYPE */

/**
  * @brief  Get register or setting base address
  * @param  type register or setting type
  * @retval Base address
  */
static void *get_base_addr(base_type type)
{
  void *ptr;

  switch (type)
  {
    case BASE_DDRCTRL:
      ptr = (void *)DDRC_BASE;
      break;
    case BASE_PHY_UI_BASIC:
      ptr = ddrphy_get_ui_basic_base();
      break;
    case BASE_PHY_UI_ADVANCED:
      ptr = ddrphy_get_ui_advanced_base();
      break;
    case BASE_PHY_UI_MODE_REGISTER:
      ptr = ddrphy_get_ui_mode_register_base();
      break;
    case BASE_PHY_UI_SWIZZLE:
      ptr = ddrphy_get_ui_swizzle_base();
      break;
    default:
      ptr = (void *)0U;
      break;
  }

  return ptr;
}

/**
  * @brief  Writes a list of register or settings
  * @param  type  register or setting type
  *         param pointer to source content
  * @retval 0 if success, error code else
  */
static int32_t set_reg(reg_type type, uintptr_t param)
{
  uint8_t i;
  void *base_addr = get_base_addr(ddr_registers[type].base);
  const reg_desc_t *desc = ddr_registers[type].desc;

  if (base_addr == (void *)0U)
  {
    return -1;
  }

  for (i = 0U; i < ddr_registers[type].size; i++)
  {
    if (desc[i].par_offset == INVALID_OFFSET)
    {
      return -1;
    }
    else
    {
      uintptr_t address = param + (uintptr_t)desc[i].par_offset;
      uint32_t *ptr = (uint32_t *)address;

      if (base_addr == (void *)DDRC_BASE)
      {
        if (desc[i].qd)
        {
          start_sw_done();
        }

      WRITE_REG(*(volatile uint32_t *)(base_addr + (uint32_t)desc[i].offset),
                *ptr);

        if (desc[i].qd)
        {
          if (wait_sw_done_ack() != 0)
          {
            /* wait_sw_done_ack timeout for offset */
            return -1;
          }
        }
      }
      else
      {
        *(int32_t *)(base_addr + (uint32_t)desc[i].offset) = (int32_t)*ptr;
      }
    }
  }

  return 0;
}

/**
  * @brief  Check if the CID filtering for DDR controller is enabled
  * @param  None
  * @retval true if enabled, false else
  */
bool is_ddr_cid_filtering_enabled(void)
{
  return (RCC->R[RCC_LOCALRES_104].CIDCFGR & RCC_RxCIDCFGR_CFEN) == RCC_RxCIDCFGR_CFEN;
}

/**
  * @brief  Enable CID filtering for the DDR controller
  * @param  None
  * @retval None
  */
void ddr_enable_cid_filtering(void)
{
  SET_BIT(RCC->R[RCC_LOCALRES_104].CIDCFGR, RCC_RxCIDCFGR_CFEN);
  if (sem_mutex_save != 0)
  {
    SET_BIT(RCC->R[RCC_LOCALRES_104].SEMCR, RCC_RxSEMCR_SEM_MUTEX);
  }
}

/**
  * @brief  Disable CID filtering for the DDR controller
  * @param  None
  * @retval None
  */
void ddr_disable_cid_filtering(void)
{
  sem_mutex_save = READ_REG(RCC->R[RCC_LOCALRES_104].SEMCR) & RCC_RxSEMCR_SEM_MUTEX;
  CLEAR_BIT(RCC->R[RCC_LOCALRES_104].CIDCFGR, RCC_RxCIDCFGR_CFEN);
}

/**
  * @brief  Generic waiting function for self-refresh entry/exit
  * @param  is_entry transition (self-refresh entry or exit)
  * @retval 0 if success, error code else
  */
static int32_t sr_loop(bool is_entry)
{
  uint32_t type;
#if STM32MP_LPDDR4_TYPE
  uint32_t state;
#endif /* STM32MP_LPDDR4_TYPE */
  __IO uint32_t timeout = ddr_timeout_init_us(DDR_TIMEOUT_1_US);
  bool repeat_loop = false;

  /* Wait for DDRCTRL to be out of or back to "normal/mission mode" */
  do
  {
    type = READ_REG(DDRC->STAT) & DDRC_STAT_SELFREF_TYPE_Msk;
#if STM32MP_LPDDR4_TYPE
    state = READ_REG(DDRC->STAT) & DDRC_STAT_SELFREF_STATE_Msk;
#endif /* STM32MP_LPDDR4_TYPE */

    timeout--;
    if (ddr_timeout_elapsed(timeout))
    {
      return -1;
    }

    if (is_entry)
    {
#if STM32MP_LPDDR4_TYPE
      repeat_loop = (type == 0x0U) || (state != DDRC_STAT_SELFREF_STATE_SRPD);
#else /* STM32MP_LPDDR4_TYPE */
      repeat_loop = (type == 0x0U);
#endif /* STM32MP_LPDDR4_TYPE */
    }
    else
    {
#if STM32MP_LPDDR4_TYPE
      repeat_loop = (type != 0x0U) || (state != 0x0U);
#else /* STM32MP_LPDDR4_TYPE */
      repeat_loop = (type != 0x0U);
#endif /* STM32MP_LPDDR4_TYPE */
    }
  } while (repeat_loop);

  return 0;
}

/**
  * @brief  Waiting function for self-refresh entry
  * @param  None
  * @retval 0 if success, error code else
  */
static int32_t sr_entry_loop(void)
{
  return sr_loop(true);
}

/**
  * @brief  Waiting function for self-refresh exit
  * @param  None
  * @retval 0 if success, error code else
  */
static int32_t sr_exit_loop(void)
{
  return sr_loop(false);
}

/**
  * @brief  Software self-refresh entry for all modes
  * @param  standby low power mode (standby or stop mode)
  * @retval 0 if success, error code else
  */
static int32_t ssr_entry(bool standby)
{
  if (disable_axi_port() != 0)
  {
    return -1;
  }

#if STM32MP_LPDDR4_TYPE
  if (standby)
  {
    /* Disable DRAM drift compensation */
    disable_phy_ddc();
  }
#endif /* STM32MP_LPDDR4_TYPE */

  if (disable_dfi_low_power_interface() != 0)
  {
    return -1;
  }

  /* SW self refresh entry prequested */
  SET_BIT(DDRC->PWRCTL, DDRC_PWRCTL_SELFREF_SW);
#if STM32MP_LPDDR4_TYPE
  CLEAR_BIT(DDRC->PWRCTL, DDRC_PWRCTL_STAY_IN_SELFREF);
#endif /* STM32MP_LPDDR4_TYPE */

  if (sr_entry_loop() != 0)
  {
    return -1;
  }

  if (activate_controller(true) != 0)
  {
    return -1;
  }

  /* Poll on ddrphy_initeng0_phyinlpx.phyinlp3 = 1 */
  if (wait_lp3_mode(true) != 0)
  {
    return -1;
  }

  if (standby)
  {
    bool cid_filtering = is_ddr_cid_filtering_enabled();

    /* Enable IO retention */
    if (cid_filtering)
    {
      ddr_disable_cid_filtering();
    }

    CLEAR_BIT(PWR->CR11, PWR_CR11_DDRRETDIS);

    if (cid_filtering)
    {
      ddr_enable_cid_filtering();
    }
  }

  /* Disable DDRSS bus clocks */
  CLEAR_BIT(RCC->DDRCPCFGR, RCC_DDRCPCFGR_DDRCPEN);
  CLEAR_BIT(RCC->DDRCFGR, RCC_DDRCFGR_DDRCFGEN);
  CLEAR_BIT(RCC->DDRCAPBCFGR, RCC_DDRCAPBCFGR_DDRCAPBEN);
  CLEAR_BIT(RCC->DDRPHYCAPBCFGR, RCC_DDRPHYCAPBCFGR_DDRPHYCAPBEN);

  /* Configure DDRSS kernel clocks for low power */
  SET_BIT(RCC->DDRPHYCCFGR, RCC_DDRPHYCCFGR_DDRPHYCEN);
  SET_BIT(RCC->DDRITFCFGR, RCC_DDRITFCFGR_DDRPHYDLP);

  return 0;
}

/**
  * @brief  Software self-refresh entry for stop mode
  * @param  None
  * @retval 0 if success, error code else
  */
static int32_t sr_ssr_entry(void)
{
  return ssr_entry(false);
}

/**
  * @brief  Software self-refresh entry for standby mode
  * @param  None
  * @retval 0 if success, error code else
  */
static int32_t stdby_sr_ssr_entry(void)
{
  return ssr_entry(true);
}

/**
  * @brief  Software self-refresh exit
  * @param  None
  * @retval 0 if success, error code else
  */
static int32_t sr_ssr_exit(void)
{
  /* Enable DDRSS bus clocks */
  SET_BIT(RCC->DDRCPCFGR, RCC_DDRCPCFGR_DDRCPEN);
  SET_BIT(RCC->DDRCFGR, RCC_DDRCFGR_DDRCFGEN);
  SET_BIT(RCC->DDRCAPBCFGR, RCC_DDRCAPBCFGR_DDRCAPBEN);
  SET_BIT(RCC->DDRPHYCAPBCFGR, RCC_DDRPHYCAPBCFGR_DDRPHYCAPBEN);

  /* Restore DDRSS kernel clocks */
  CLEAR_BIT(RCC->DDRITFCFGR, RCC_DDRITFCFGR_DDRPHYDLP);
  SET_BIT(RCC->DDRPHYCCFGR, RCC_DDRPHYCCFGR_DDRPHYCEN);

  ddr_delay_us(DDR_DELAY_1_US);

  if (activate_controller(false) != 0)
  {
    return -1;
  }

  /* Poll on ddrphy_initeng0_phyinlpx.phyinlp3 = 0 */
  if (wait_lp3_mode(false) != 0)
  {
    return -1;
  }

  /* SW self refresh entry prequested */
  CLEAR_BIT(DDRC->PWRCTL, DDRC_PWRCTL_SELFREF_SW);

  if (sr_exit_loop() != 0)
  {
    return -1;
  }

  /* Re-enable DFI low-power interface */
  SET_BIT(DDRC->DFILPCFG0, DDRC_DFILPCFG0_DFI_LP_EN_SR);

  enable_axi_port();

  return 0;
}

/**
  * @brief  Set software self-refresh mode
  * @param  None
  * @retval 0
  */
static int32_t sr_ssr_set(void)
{
  /*
   * Disable Clock disable with LP modes
   * (used in RUN mode for LPDDR2 with specific timing).
   */
  CLEAR_BIT(DDRC->PWRCTL, DDRC_PWRCTL_EN_DFI_DRAM_CLK_DISABLE);

  /* Disable automatic Self-Refresh mode */
  CLEAR_BIT(DDRC->PWRCTL, DDRC_PWRCTL_SELFREF_EN);

  WRITE_REG(DDRDBG->LP_DISABLE,
            DDRDBG_LP_DISABLE_LPI_XPI_DISABLE | DDRDBG_LP_DISABLE_LPI_DDRC_DISABLE);

  return 0;
}

/**
  * @brief  Automatic self-refresh entry for all modes
  * @param  standby low power mode (standby or stop mode)
  * @retval 0 if success, error code else
  */
static int32_t asr_entry(__unused bool standby)
{
  /*
   * Automatically enter into self refresh when there is no ddr traffic
   * for the delay programmed into SYSCONF_DDRC_AUTO_SR_DELAY register.
   * Default value is 0x20 (unit: Multiples of 32 DFI clock cycles).
   */
  return sr_entry_loop();
}

/**
  * @brief  Automatic self-refresh entry for stop mode
  * @param  None
  * @retval 0 if success, error code else
  */
static int32_t sr_asr_entry(void)
{
  return asr_entry(false);
}

/**
  * @brief  Automatic self-refresh entry for standby mode
  * @param  None
  * @retval 0 if success, error code else
  */
static int32_t stdby_sr_asr_entry(void)
{
  return asr_entry(true);
}

/**
  * @brief  Automatic self-refresh exit
  * @param  None
  * @retval 0 if success, error code else
  */
static int32_t sr_asr_exit(void)
{
   return sr_exit_loop();
}

/**
  * @brief  Set automatic self-refresh mode
  * @param  None
  * @retval 0
  */
static int32_t sr_asr_set(void)
{
  WRITE_REG(DDRDBG->LP_DISABLE, 0U);

  return 0;
}

/**
  * @brief  Hardware self-refresh entry for all modes
  * @param  standby low power mode (standby or stop mode)
  * @retval 0 if success, error code else
  */
static int32_t hsr_entry(__unused bool standby)
{
  WRITE_REG(RCC->DDRCPCFGR, RCC_DDRCPCFGR_DDRCPLPEN);

  return sr_entry_loop(); /* read_data should be equal to 0x223 */
}

/**
  * @brief  Hardware self-refresh entry for stop mode
  * @param  None
  * @retval 0 if success, error code else
  */
static int32_t sr_hsr_entry(void)
{
  return hsr_entry(false);
}

/**
  * @brief  Hardware self-refresh entry for standby mode
  * @param  None
  * @retval 0 if success, error code else
  */
static int32_t stdby_sr_hsr_entry(void)
{
  return hsr_entry(true);
}

/**
  * @brief  Hardware self-refresh exit
  * @param  None
  * @retval 0 if success, error code else
  */
static int32_t sr_hsr_exit(void)
{
  WRITE_REG(RCC->DDRCPCFGR, RCC_DDRCPCFGR_DDRCPLPEN | RCC_DDRCPCFGR_DDRCPEN);

  return 0;
}

/**
  * @brief  Set hardware self-refresh mode
  * @param  None
  * @retval 0 if success, error code else
  */
static int32_t sr_hsr_set(void)
{
  MODIFY_REG(RCC->DDRITFCFGR, RCC_DDRITFCFGR_DDRCKMOD, RCC_DDRITFCFGR_DDRCKMOD_HSR);

  /*
   * manage quasi-dynamic registers modification
   * hwlpctl.hw_lp_en : Group 2
   */
  if (sw_selfref_entry() != 0)
  {
    return -1;
  }

  start_sw_done();

  WRITE_REG(DDRC->HWLPCTL,
            DDRC_HWLPCTL_HW_LP_EN | DDRC_HWLPCTL_HW_LP_EXIT_IDLE_EN |
            (HW_IDLE_PERIOD << DDRC_HWLPCTL_HW_LP_IDLE_X32_Pos));

  if (wait_sw_done_ack() != 0)
  {
    return -1;
  }

  sw_selfref_exit();

  return 0;
}

/**
  * @brief  Reset DDR sub-system
  * @param  None
  * @retval None
  */
static void ddr_reset(void)
{
  ddr_delay_us(DDR_DELAY_1_US);

  SET_BIT(RCC->DDRITFCFGR, RCC_DDRITFCFGR_DDRRST);
  WRITE_REG(RCC->DDRPHYCAPBCFGR, RCC_DDRPHYCAPBCFGR_DDRPHYCAPBEN |
                                 RCC_DDRPHYCAPBCFGR_DDRPHYCAPBLPEN |
                                 RCC_DDRPHYCAPBCFGR_DDRPHYCAPBRST);
  WRITE_REG(RCC->DDRCAPBCFGR, RCC_DDRCAPBCFGR_DDRCAPBEN |
                              RCC_DDRCAPBCFGR_DDRCAPBLPEN |
                              RCC_DDRCAPBCFGR_DDRCAPBRST);
  WRITE_REG(RCC->DDRCFGR, RCC_DDRCFGR_DDRCFGEN | RCC_DDRCFGR_DDRCFGLPEN |
                          RCC_DDRCFGR_DDRCFGRST);

  ddr_delay_us(DDR_DELAY_1_US);

  /* Reset release */
  CLEAR_BIT(RCC->DDRPHYCAPBCFGR, RCC_DDRPHYCAPBCFGR_DDRPHYCAPBRST);
  CLEAR_BIT(RCC->DDRCAPBCFGR, RCC_DDRCAPBCFGR_DDRCAPBRST);
  CLEAR_BIT(RCC->DDRCFGR, RCC_DDRCFGR_DDRCFGRST);

  ddr_delay_us(DDR_DELAY_1_US);
}

/**
  * @brief  Configure PLL2 dedicated to DDR
  * @param  None
  * @retval 0 if success, error code else
  */
static int32_t ddr_pll2_configure(void)
{
#if defined(USE_STM32MP257CXX_EMU)
  __IO uint32_t timeout;

  WRITE_REG(RCC->PLL2CFGR4, RCC_PLL2CFGR4_VAL);
  WRITE_REG(RCC->PLL2CFGR1, RCC_PLL2CFGR1_VAL1);
  WRITE_REG(RCC->PLL2CFGR2, RCC_PLL2CFGR2_VAL);
  WRITE_REG(RCC->PLL2CFGR6, RCC_PLL2CFGR6_VAL);
  WRITE_REG(RCC->PLL2CFGR7, RCC_PLL2CFGR7_VAL);
  WRITE_REG(RCC->PLL2CFGR3, RCC_PLL2CFGR3_VAL);
  WRITE_REG(RCC->PLL2CFGR5, RCC_PLL2CFGR5_VAL);
  WRITE_REG(RCC->PLL2CFGR1, RCC_PLL2CFGR1_VAL2);

  ddr_delay_us(DDR_DELAY_1_US);

  timeout = ddr_timeout_init_us(DDR_TIMEOUT_US_1S);
  while ((READ_REG(DDRDBG->FRAC_PLL_LOCK) & DDRDBG_FRAC_PLL_LOCK_LOCK) !=
         DDRDBG_FRAC_PLL_LOCK_LOCK)
  {
    timeout--;

    if (ddr_timeout_elapsed(timeout))
    {
      return -1;
    }

    ddr_delay_us(DDR_DELAY_1_US);
  }
#endif /* USE_STM32MP257CXX_EMU */

#if !defined(USE_STM32MP257CXX_EMU)
  RCC_PLLInitTypeDef RCC_Pll2InitStruct = {0};

  RCC_Pll2InitStruct.PLLSource = DDR_PLL_SOURCE;
  RCC_Pll2InitStruct.PLLMode = DDR_PLL_MODE;
  RCC_Pll2InitStruct.FBDIV = DDR_PLL_FBDIV;
  RCC_Pll2InitStruct.FREFDIV = DDR_PLL_FREFDIV;
  RCC_Pll2InitStruct.FRACIN = DDR_PLL_FRACIN;
  RCC_Pll2InitStruct.POSTDIV1 = DDR_PLL_POSTDIV1;
  RCC_Pll2InitStruct.POSTDIV2 = DDR_PLL_POSTDIV2;
  RCC_Pll2InitStruct.PLLState = DDR_PLL_STATE;
  RCC_Pll2InitStruct.SSM_Mode = DDR_PLL_SSM_MODE;
  RCC_Pll2InitStruct.SSM_SPREAD = DDR_PLL_SSM_SPREAD;
  RCC_Pll2InitStruct.SSM_DIVVAL = DDR_PLL_SSM_DIVVAL;

  if (HAL_RCCEx_PLL2Config(&RCC_Pll2InitStruct) != HAL_OK)
  {
    return -1;
  }
#endif /* !defined(USE_STM32MP257CXX_EMU) */

  return 0;
}

/**
  * @brief  Configure DDR sub-system
  * @param  None
  * @retval 0 if success, error code else
  */
static int32_t ddr_sysconf_configuration(void)
{
  if (ddr_pll2_configure() != 0)
  {
    return -1;
  }

#if defined(USE_STM32MP257CXX_EMU)
  WRITE_REG(DDRDBG->LP_DISABLE,
            DDRDBG_LP_DISABLE_LPI_XPI_DISABLE | DDRDBG_LP_DISABLE_LPI_DDRC_DISABLE);
  WRITE_REG(DDRDBG->BYPASS_PCLKEN, 0U);

  ddr_delay_us(DDR_DELAY_1_US);

  WRITE_REG(RCC->DDRPHYCCFGR, RCC_DDRPHYCCFGR_DDRPHYCEN);

  WRITE_REG(RCC->DDRCPCFGR, RCC_DDRCPCFGR_DDRCPEN | RCC_DDRCPCFGR_DDRCPLPEN);
  SET_BIT(RCC->DDRITFCFGR, RCC_DDRITFCFGR_DDRRST);

  ddr_delay_us(DDR_DELAY_1_US);
#endif /* USE_STM32MP257CXX_EMU */

#if !defined(USE_STM32MP257CXX_EMU)
  WRITE_REG(DDRDBG->LP_DISABLE,
            DDRDBG_LP_DISABLE_LPI_XPI_DISABLE | DDRDBG_LP_DISABLE_LPI_DDRC_DISABLE);

  WRITE_REG(DDRDBG->BYPASS_PCLKEN, (uint32_t)static_ddr_config.p_uib.pllbypass[0]);

  WRITE_REG(RCC->DDRPHYCCFGR, RCC_DDRPHYCCFGR_DDRPHYCEN);
  SET_BIT(RCC->DDRITFCFGR, RCC_DDRITFCFGR_DDRRST);

  ddr_delay_us(DDR_DELAY_1_US);
#endif /* !defined(USE_STM32MP257CXX_EMU) */

  return 0;
}

/**
  * @brief  Wait refresh update done acknowledge
  * @param  None
  * @retval 0 if success, error code else
  */
static int32_t wait_refresh_update_done_ack(void)
{
  __IO uint32_t timeout;
  uint32_t rfshctl3;
  uint32_t refresh_update_level = DDRC_RFSHCTL3_REFRESH_UPDATE_LEVEL;

  /* Toggle rfshctl3.refresh_update_level */
  rfshctl3 = READ_REG(DDRC->RFSHCTL3);
  if ((rfshctl3 & refresh_update_level) == refresh_update_level)
  {
    CLEAR_BIT(DDRC->RFSHCTL3, refresh_update_level);
  }
  else
  {
    SET_BIT(DDRC->RFSHCTL3, refresh_update_level);
    refresh_update_level = 0U;
  }

  timeout = ddr_timeout_init_us(DDR_TIMEOUT_US_1S);
  do
  {
    rfshctl3 = READ_REG(DDRC->RFSHCTL3);

    timeout--;
    if (ddr_timeout_elapsed(timeout))
    {
      return -1;
    }
  } while ((rfshctl3 & DDRC_RFSHCTL3_REFRESH_UPDATE_LEVEL) ==
           refresh_update_level);

  return 0;
}

/**
  * @brief  Disable refresh
  * @param  None
  * @retval 0 if success, error code else
  */
static int32_t disable_refresh(void)
{
  SET_BIT(DDRC->RFSHCTL3, DDRC_RFSHCTL3_DIS_AUTO_REFRESH);

  if (wait_refresh_update_done_ack() != 0)
  {
    return -1;
  }

  ddr_delay_us(DDR_DELAY_1_US);

  CLEAR_BIT(DDRC->PWRCTL, DDRC_PWRCTL_POWERDOWN_EN |
                          DDRC_PWRCTL_SELFREF_EN);

  ddr_delay_us(DDR_DELAY_1_US);

  /*
   * Manage quasi-dynamic registers modification
   * dfimisc.dfi_init_complete_en : Group 3
   */
  if (set_qd3_update_conditions() != 0)
  {
    return -1;
  }

  ddr_delay_us(DDR_DELAY_1_US);

  CLEAR_BIT(DDRC->DFIMISC, DDRC_DFIMISC_DFI_INIT_COMPLETE_EN);

  ddr_delay_us(DDR_DELAY_1_US);

  if (unset_qd3_update_conditions() != 0)
  {
    return -1;
  }

  return 0;
}

/**
  * @brief  Restore refresh
  * @param  rfshctl3 RFSHCTL3 value to restore
  *         pwrctl PWRCTL value to restore
  * @retval 0 if success, error code else
  */
static int32_t restore_refresh(uint32_t rfshctl3, uint32_t pwrctl)
{
  if ((pwrctl & DDRC_PWRCTL_SELFREF_SW) != 0U)
  {
    CLEAR_BIT(DDRC->PWRCTL, DDRC_PWRCTL_SELFREF_SW);

    ddr_delay_us(DDR_DELAY_1_US);
  }

  if ((rfshctl3 & DDRC_RFSHCTL3_DIS_AUTO_REFRESH) == 0U)
  {
    CLEAR_BIT(DDRC->RFSHCTL3, DDRC_RFSHCTL3_DIS_AUTO_REFRESH);
    if (wait_refresh_update_done_ack() != 0)
    {
      return -1;
    }

    ddr_delay_us(DDR_DELAY_1_US);
  }

  if ((pwrctl & DDRC_PWRCTL_POWERDOWN_EN) != 0U)
  {
    SET_BIT(DDRC->PWRCTL, DDRC_PWRCTL_POWERDOWN_EN);

    ddr_delay_us(DDR_DELAY_1_US);
  }

  if ((pwrctl & DDRC_PWRCTL_SELFREF_EN) != 0U)
  {
    SET_BIT(DDRC->PWRCTL, DDRC_PWRCTL_SELFREF_EN);

    ddr_delay_us(DDR_DELAY_1_US);
  }

  /*
   * Manage quasi-dynamic registers modification
   * dfimisc.dfi_init_complete_en : Group 3
   */
  if (set_qd3_update_conditions() != 0)
  {
    return -1;
  }

  ddr_delay_us(DDR_DELAY_1_US);

  SET_BIT(DDRC->DFIMISC, DDRC_DFIMISC_DFI_INIT_COMPLETE_EN);

  ddr_delay_us(DDR_DELAY_1_US);

  if (unset_qd3_update_conditions() != 0)
  {
    return -1;
  }

  return 0;
}

/* Exported functions ---------------------------------------------------------*/

/** @defgroup DDR_Exported_Functions DDR Exported Functions
  * @{
  */


/**
  * @brief  DDR init sequence, including
  *         - reset/clock/power management (i.e. access to other IPs),
  *         - DDRCTRL and DDRPHY configuration and initialization,
  *         - self-refresh mode setup,
  *         - data/addr tests execution after training.
  * @param  DDR initialisation structure
  * @retval None.
  */
HAL_StatusTypeDef HAL_DDR_Init(DDR_InitTypeDef *iddr)
{
  int32_t iret  = -1;
  uint32_t uret;
  HAL_DDR_SelfRefreshModeTypeDef mode;
  bool cid_filtering = is_ddr_cid_filtering_enabled();

  iddr->self_refresh = false;

  if (iddr->wakeup_from_standby)
  {
    iddr->self_refresh = true;
  }

#ifdef STM32MP_GATHER_DDRCTRL_SETTING_IN_STATIC_ARRAY
  {
    uint32_t *ptr_src;
    uint32_t *ptr_dest;
    uint32_t j;
    uint32_t start = 0U;
    uint32_t end = 0U;

    ptr_src = (uint32_t *)&tab_static_param[0];
    ptr_dest = &static_ddr_config.c_reg.MSTR;
    end += ARRAY_SIZE(ddr_reg_desc);

    for (j = start; j < end; j++)
    {
      *ptr_dest = *ptr_src;
      ptr_dest++;
      ptr_src++;
    }

    ptr_dest = &static_ddr_config.c_timing.RFSHTMG;
    start = end;
    end += ARRAY_SIZE(ddr_timing_desc);

    for (j = start; j < end; j++)
    {
      *ptr_dest = *ptr_src;
      ptr_dest++;
      ptr_src++;
    }

    ptr_dest = &static_ddr_config.c_map.ADDRMAP0;
    start = end;
    end += ARRAY_SIZE(ddr_map_desc);


    for (j = start; j < end; j++)
    {
      *ptr_dest = *ptr_src;
      ptr_dest++;
      ptr_src++;
    }

    ptr_dest = &static_ddr_config.c_perf.SCHED;
    start = end;
    end += ARRAY_SIZE(ddr_perf_desc);


    for (j = start; j < end; j++)
    {
      *ptr_dest = *ptr_src;
      ptr_dest++;
      ptr_src++;
    }

    static_ddr_config.p_uia.phymstrtraininterval[0] =
            tab_static_param[ARRAY_SIZE(tab_static_param) - 1];
  }
#endif /* STM32MP_GATHER_DDRCTRL_SETTING_IN_STATIC_ARRAY */

  if ((static_ddr_config.c_reg.MSTR & DDRC_MSTR_DDR3) != 0U)
  {
    iret = HAL_DDR_MspInit(STM32MP_DDR3);
  }
  else if ((static_ddr_config.c_reg.MSTR & DDRC_MSTR_DDR4) != 0U)
  {
    iret = HAL_DDR_MspInit(STM32MP_DDR4);
  }
  else if ((static_ddr_config.c_reg.MSTR & DDRC_MSTR_LPDDR4) != 0U)
  {
    iret = HAL_DDR_MspInit(STM32MP_LPDDR4);
  }

  if (iret != 0)
  {
    return HAL_ERROR;
  }


  if (iddr->wakeup_from_standby)
  {
    WRITE_REG(RCC->DDRCPCFGR, RCC_DDRCPCFGR_DDRCPEN | RCC_DDRCPCFGR_DDRCPLPEN |
                              RCC_DDRCPCFGR_DDRCPRST);
    SET_BIT(RCC->DDRITFCFGR, RCC_DDRITFCFGR_DDRRST);
    WRITE_REG(RCC->DDRPHYCAPBCFGR, RCC_DDRPHYCAPBCFGR_DDRPHYCAPBEN |
                                   RCC_DDRPHYCAPBCFGR_DDRPHYCAPBLPEN |
                                   RCC_DDRPHYCAPBCFGR_DDRPHYCAPBRST);
    WRITE_REG(RCC->DDRCAPBCFGR, RCC_DDRCAPBCFGR_DDRCAPBEN |
                                RCC_DDRCAPBCFGR_DDRCAPBLPEN |
                                RCC_DDRCAPBCFGR_DDRCAPBRST);

    CLEAR_BIT(RCC->DDRITFCFGR, RCC_DDRITFCFGR_DDRPHYDLP);
    SET_BIT(RCC->DDRPHYCCFGR, RCC_DDRPHYCCFGR_DDRPHYCEN);

    ddr_delay_us(DDR_DELAY_1_US);

    if (cid_filtering)
    {
      ddr_disable_cid_filtering();
    }

    /* Disable IO retention */
    SET_BIT(PWR->CR11, PWR_CR11_DDRRETDIS);

    if (cid_filtering)
    {
      ddr_enable_cid_filtering();
    }

    ddr_delay_us(DDR_DELAY_1_US);

    CLEAR_BIT(RCC->DDRCAPBCFGR, RCC_DDRCAPBCFGR_DDRCAPBRST);
    ddr_delay_us(DDR_DELAY_1_US);

    if (ddr_pll2_configure() != 0)
    {
      return HAL_ERROR;
    }
  }
  else
  {
    ddr_reset();

    if (ddr_sysconf_configuration() != 0)
    {
      return HAL_ERROR;
    }
  }

#if STM32MP_LPDDR4_TYPE
  /*
   * Enable PWRCTL.SELFREF_SW to ensure correct setting of PWRCTL.LPDDR4_SR_ALLOWED.
   * Later disabled in restore_refresh().
   */
  static_ddr_config.c_reg.PWRCTL |= DDRC_PWRCTL_SELFREF_SW;
#endif /* STM32MP_LPDDR4_TYPE */

  if ((set_reg(REG_REG, (uintptr_t)&static_ddr_config.c_reg) != 0) ||
      (set_reg(REG_TIMING, (uintptr_t)&static_ddr_config.c_timing) != 0) ||
      (set_reg(REG_MAP, (uintptr_t)&static_ddr_config.c_map) != 0) ||
      (set_reg(REG_PERF, (uintptr_t)&static_ddr_config.c_perf) != 0))
  {
    return HAL_ERROR;
  }

  if (!iddr->wakeup_from_standby)
  {
    uint32_t ddr_retdis;

    /* Check DDR PHY pads retention */
    if (cid_filtering)
    {
      ddr_disable_cid_filtering();
    }

    ddr_retdis = READ_REG(PWR->CR11) & PWR_CR11_DDRRETDIS;

    if (cid_filtering)
    {
      ddr_enable_cid_filtering();
    }

    if (iddr->self_refresh)
    {
      if (ddr_retdis == PWR_CR11_DDRRETDIS)
      {
        iddr->self_refresh = false;
      }
    }

    if (!iddr->self_refresh)
    {
      if (cid_filtering)
      {
        ddr_disable_cid_filtering();
      }

      SET_BIT(PWR->CR11, PWR_CR11_DDRRETDIS);
      ddr_delay_us(DDR_DELAY_1_US);

      if (cid_filtering)
      {
        ddr_enable_cid_filtering();
      }
    }

    /* DDR core and PHY reset de-assert */
    CLEAR_BIT(RCC->DDRITFCFGR, RCC_DDRITFCFGR_DDRRST);
    ddr_delay_us(DDR_DELAY_1_US);

    if (disable_refresh() != 0)
    {
      return HAL_ERROR;
    }
  }

  if ((set_reg(PHY_UI_BASIC, (uintptr_t)&static_ddr_config.p_uib) != 0) ||
      (set_reg(PHY_UI_ADVANCED, (uintptr_t)&static_ddr_config.p_uia) != 0) ||
      (set_reg(PHY_UI_MODE_REGISTER, (uintptr_t)&static_ddr_config.p_uim) != 0) ||
      (set_reg(PHY_UI_SWIZZLE, (uintptr_t)&static_ddr_config.p_uis) != 0))
  {
    return HAL_ERROR;
  }

  if (iddr->wakeup_from_standby)
  {
    WRITE_REG(RCC->DDRCPCFGR, RCC_DDRCPCFGR_DDRCPEN | RCC_DDRCPCFGR_DDRCPLPEN);
    CLEAR_BIT(RCC->DDRITFCFGR, RCC_DDRITFCFGR_DDRRST);
    CLEAR_BIT(RCC->DDRPHYCAPBCFGR, RCC_DDRPHYCAPBCFGR_DDRPHYCAPBRST);
    WRITE_REG(RCC->DDRCFGR, RCC_DDRCFGR_DDRCFGEN | RCC_DDRCFGR_DDRCFGLPEN);

    /* Initialize DDR by skipping training and disabling result saving */
    iret = ddrphy_phyinit_sequence(true, false);

    if (iret == 0)
    {
      iret = ddrphy_phyinit_restore_sequence();
    }

    /* Poll on ddrphy_initeng0_phyinlpx.phyinlp3 = 0 */
    if (wait_lp3_mode(false) != 0)
    {
      return HAL_ERROR;
    }
  }
  else
  {
#ifdef STM32MP_DISABLE_SAVE_RETENTION_REGISTERS
    /*
     * Initialize DDR including training but without result saving.
     * In a M33 DDR_Starter context executed in RETRAM, this is mandatory
     * to ensure a good execution without conflict with log buffer.
     */
    iret = ddrphy_phyinit_sequence(false, false);
#else /* STM32MP_DISABLE_SAVE_RETENTION_REGISTERS */
    /* Initialize DDR including training and result saving */
    iret = ddrphy_phyinit_sequence(false, true);
#endif /* STM32MP_DISABLE_SAVE_RETENTION_REGISTERS */
  }

  if (iret != 0)
  {
    return HAL_ERROR;
  }

  if (activate_controller(false) != 0)
  {
    return HAL_ERROR;
  }

  if (iddr->wakeup_from_standby)
  {
    /* SW self refresh entry prequested */
    CLEAR_BIT(DDRC->PWRCTL, DDRC_PWRCTL_SELFREF_SW);

    if (sr_exit_loop() != 0)
    {
      return HAL_ERROR;
    }

    /* Re-enable DFI low-power interface */
    SET_BIT(DDRC->DFILPCFG0, DDRC_DFILPCFG0_DFI_LP_EN_SR);
  }
  else
  {
    /* Set back registers in step 8 to the orginal values if desidered */
    if (restore_refresh(static_ddr_config.c_reg.RFSHCTL3,
                        static_ddr_config.c_reg.PWRCTL) != 0)
    {
      return HAL_ERROR;
    }
  }

  enable_axi_port();

  /* Checking DDR access is only applicable, if current core is TDCID */
  /* according to default RISAF4 configuration */
  uret = 0U;
#if defined(CORE_CA35)
  if (RIMC_CR_TDCID1 == (RIMC->CR&RIMC_CR_TDCID_Msk))
  {
    uret = 1U;
  }
  else
  {
    /* WARNING:   DDR access tests bypassed - A35 core not TDCID ! */
  }
#elif defined(CORE_CM33)
  if (RIMC_CR_TDCID2 == (RIMC->CR&RIMC_CR_TDCID_Msk))
  {
    uret = 1U;
  }
  else
  {
    /* WARNING:   DDR access tests bypassed - M33 core not TDCID ! */
  }
#endif /* defined(CORE_CM33) */

  if (uret == 1U)
  {
    uintptr_t ulret;

    if (iddr->self_refresh)
    {
      ulret = ddr_test_rw_access();
      if (ulret != 0UL)
      {
        return HAL_ERROR;
      }
    }
    else
    {
      ulret = ddr_test_data_bus();
      if (ulret != 0U)
      {
        return HAL_ERROR;
      }

      ulret = ddr_test_addr_bus();
      if (ulret != 0UL)
      {
        return HAL_ERROR;
      }

      ulret = ddr_check_size();
      if (ulret != static_ddr_config.info.size)
      {
        return HAL_ERROR;
      }
    }
  }

  /*
   * Initialization sequence has configured DDR registers with settings.
   * Get Self Refresh (SR) mode stored in settings (pwrctl value), reset
   * sr_mode global variable and set this mode.
   */
  mode = HAL_DDR_SR_ReadMode();
  sr_mode = HAL_DDR_INVALID_MODE;

  return HAL_DDR_SR_SetMode(mode);
}

/**
  * @brief  Enable DDR Self-Refresh, when entering in CSTOP.
  * @param  zq0cr0_zdata IO calibration value.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_DDR_SR_Entry(__unused uint32_t *zq0cr0_zdata)
{
  HAL_StatusTypeDef ret = HAL_ERROR;

  switch (sr_mode)
  {
    case HAL_DDR_SW_SELF_REFRESH_MODE:
      if (sr_ssr_entry() == 0)
      {
        ret = HAL_OK;
      }
      break;
    case HAL_DDR_AUTO_SELF_REFRESH_MODE:
      if (sr_asr_entry() == 0)
      {
        ret = HAL_OK;
      }
      break;
    case HAL_DDR_HW_SELF_REFRESH_MODE:
      if (sr_hsr_entry() == 0)
      {
        ret = HAL_OK;
      }
      break;
    default:
      break;
  }

  return ret;
}

/**
  * @brief  Enable DDR Self-Refresh, when entering in CSTANBY (shutting down).
  * @param  none.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_DDR_STDBY_SR_Entry(void)
{
  HAL_StatusTypeDef ret = HAL_ERROR;

  switch (sr_mode)
  {
    case HAL_DDR_SW_SELF_REFRESH_MODE:
      if (stdby_sr_ssr_entry() == 0)
      {
        ret = HAL_OK;
      }
      break;
    case HAL_DDR_AUTO_SELF_REFRESH_MODE:
      if (stdby_sr_asr_entry() == 0)
      {
        ret = HAL_OK;
      }
      break;
    case HAL_DDR_HW_SELF_REFRESH_MODE:
      if (stdby_sr_hsr_entry() == 0)
      {
        ret = HAL_OK;
      }
      break;
    default:
      break;
  }

  return ret;
}

/**
  * @brief  Remove DDR Self-Refresh on low power cases (when exiting from
  *         CSTOP).
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_DDR_SR_Exit(void)
{
  HAL_StatusTypeDef ret = HAL_ERROR;

  switch (sr_mode)
  {
    case HAL_DDR_SW_SELF_REFRESH_MODE:
      if (sr_ssr_exit() == 0)
      {
        ret = HAL_OK;
      }
      break;
    case HAL_DDR_AUTO_SELF_REFRESH_MODE:
      if (sr_asr_exit() == 0)
      {
        ret = HAL_OK;
      }
      break;
    case HAL_DDR_HW_SELF_REFRESH_MODE:
      if (sr_hsr_exit() == 0)
      {
        ret = HAL_OK;
      }
      break;
    default:
      break;
  }

  return ret;
}

/**
  * @brief  Activate either HW, or SW or automatic self-refresh, in low power
  *         cases (when entering in or exiting from CSTOP).
  * @param  mode Self-refresh mode.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_DDR_SR_SetMode(HAL_DDR_SelfRefreshModeTypeDef mode)
{
  HAL_StatusTypeDef ret = HAL_ERROR;

  if (mode == sr_mode)
  {
    return HAL_OK;
  }

  switch (mode)
  {
    case HAL_DDR_SW_SELF_REFRESH_MODE:
      if (sr_ssr_set() == 0)
      {
        ret = HAL_OK;
      }
      break;
    case HAL_DDR_AUTO_SELF_REFRESH_MODE:
      if (sr_asr_set() == 0)
      {
        ret = HAL_OK;
      }
      break;
    case HAL_DDR_HW_SELF_REFRESH_MODE:
      if (sr_hsr_set() == 0)
      {
        ret = HAL_OK;
      }
      break;
    default:
      break;
  }

  if (ret == HAL_OK)
  {
    sr_mode = mode;
  }

  return ret;
}

/**
  * @brief  Read current self-refresh mode, in low power
  *         cases (when entering in or exiting from CSTOP).
  * @retval Self-refresh mode.
  */
HAL_DDR_SelfRefreshModeTypeDef HAL_DDR_SR_ReadMode(void)
{
  uint32_t pwrctl = READ_REG(DDRC->PWRCTL);

  switch (pwrctl & (DDRC_PWRCTL_EN_DFI_DRAM_CLK_DISABLE |
                    DDRC_PWRCTL_SELFREF_EN))
  {
    case 0U:
      sr_mode = HAL_DDR_SW_SELF_REFRESH_MODE;
      break;
    case DDRC_PWRCTL_EN_DFI_DRAM_CLK_DISABLE:
      sr_mode = HAL_DDR_HW_SELF_REFRESH_MODE;
      break;
    case DDRC_PWRCTL_EN_DFI_DRAM_CLK_DISABLE | DDRC_PWRCTL_SELFREF_EN:
      sr_mode = HAL_DDR_AUTO_SELF_REFRESH_MODE;
      break;
    default:
      sr_mode = HAL_DDR_INVALID_MODE;
      break;
  }

  return sr_mode;
}

/**
  * @brief  Update retention register save area address.
  *         A default value is defined (last RETRAM 2KB).
  *         Must be used before first HAL_DDR_Init call.
  * @param  base new address.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_DDR_SetRetentionAreaBase(uint64_t base)
{
  if(ddrphy_phyinit_setretreglistbase(base) != 0)
  {
    return HAL_ERROR;
  }

  return HAL_OK;
}

/**
  * @brief  Save data in retention register save area.
  *         To be called if low power functions are used.
  *         HAL_DDR_SetRetentionAreaBase must be
  *         previously called if different from default value.
  *         This function must be called AFTER the COLD boot and
  *         the first DDR initialization BUT BEFORE the first low
  *         power sequence / the DDR SELF REFRESH exit.
  * @param  nones.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_DDR_SaveRetentionData(void)
{
  if(ddrphy_phyinit_usercustom_saveretregs() != 0)
  {
    return HAL_ERROR;
  }

  return HAL_OK;
}

/**
  * @}
  */
#endif /* DDRC */
#endif /* HAL_DDR_MODULE_ENABLED */
/**
  * @}
  */

/**
  * @}
  */
