/**
  ******************************************************************************
  * @file    stpmic2_reg.h
  * @author  MCD Application Team
  * @brief   Header of stpmic2_reg.c
  *
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STPMIC2_REG_H
#define STPMIC2_REG_H

#include <cmsis_compiler.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/** @addtogroup BSP
  * @{
  */

/** @addtogroup Components
  * @{
  */

/** @addtogroup STPMIC2
  * @{
  */

/** @defgroup STPMIC2_Exported_Types
  * @{
  */

/**
  * @}
  */

/** @defgroup STPMIC2_Exported_Constants STPMIC2 Exported Constants
  * @{
  */
#define BIT(_x)  (1<<(_x))
#define STM32_PMIC_NUM_IRQ_REGS       4

#define GENMASK_32(h, l) \
  (((0xFFFFFFFF) << (l)) & (0xFFFFFFFF >> (32 - 1 - (h))))


/**
  * @brief  STPMIC2 Registers address
  */
/*
 * Status Registers
 */
#define PRODUCT_ID           0x00
#define VERSION_SR           0x01
#define TURN_ON_SR           0x02
#define TURN_OFF_SR          0x03
#define RESTART_SR           0x04
#define OCP_SR1              0x05
#define OCP_SR2              0x06
#define EN_SR1               0x07
#define EN_SR2               0x08
#define FS_CNT_SR1           0x09
#define FS_CNT_SR2           0x0A
#define FS_CNT_SR3           0x0B
#define MODE_SR              0x0C

/*
 * Control Registers
 */
#define MAIN_CR              0x10
#define VINLOW_CR            0x11
#define PKEY_LKP_CR          0x12
#define WDG_CR               0x13
#define WDG_TMR_CR           0x14
#define WDG_TMR_SR           0x15
#define FS_OCP_CR1           0x16
#define FS_OCP_CR2           0x17
#define PADS_PULL_CR         0x18
#define BUCKS_PD_CR1         0x19
#define BUCKS_PD_CR2         0x1A
#define LDOS_PD_CR1          0x1B
#define LDOS_PD_CR2          0x1C
#define BUCKS_MRST_CR        0x1D
#define LDOS_MRST_CR         0x1E

#define STANDBY_PWRCTRL_SEL_0 0x00  /* 00: No source (PMIC operates in RUN state) */
#define STANDBY_PWRCTRL_SEL_1 0x20  /* 01: PWRCTRL1 control source */
#define STANDBY_PWRCTRL_SEL_2 0x40  /* 10: PWRCTRL2 control source */
#define STANDBY_PWRCTRL_SEL_3 0x60  /* 11: PWRCTRL3 control source */
/*
 * Buck CR
 */
#define BUCK1_MAIN_CR1       0x20
#define BUCK1_MAIN_CR2       0x21
#define BUCK1_ALT_CR1        0x22
#define BUCK1_ALT_CR2        0x23
#define BUCK1_PWRCTRL_CR     0x24
#define BUCK2_MAIN_CR1       0x25
#define BUCK2_MAIN_CR2       0x26
#define BUCK2_ALT_CR1        0x27
#define BUCK2_ALT_CR2        0x28
#define BUCK2_PWRCTRL_CR     0x29
#define BUCK3_MAIN_CR1       0x2A
#define BUCK3_MAIN_CR2       0x2B
#define BUCK3_ALT_CR1        0x2C
#define BUCK3_ALT_CR2        0x2D
#define BUCK3_PWRCTRL_CR     0x2E
#define BUCK4_MAIN_CR1       0x2F
#define BUCK4_MAIN_CR2       0x30
#define BUCK4_ALT_CR1        0x31
#define BUCK4_ALT_CR2        0x32
#define BUCK4_PWRCTRL_CR     0x33
#define BUCK5_MAIN_CR1       0x34
#define BUCK5_MAIN_CR2       0x35
#define BUCK5_ALT_CR1        0x36
#define BUCK5_ALT_CR2        0x37
#define BUCK5_PWRCTRL_CR     0x38
#define BUCK6_MAIN_CR1       0x39
#define BUCK6_MAIN_CR2       0x3A
#define BUCK6_ALT_CR1        0x3B
#define BUCK6_ALT_CR2        0x3C
#define BUCK6_PWRCTRL_CR     0x3D
#define BUCK7_MAIN_CR1       0x3E
#define BUCK7_MAIN_CR2       0x3F
#define BUCK7_ALT_CR1        0x40
#define BUCK7_ALT_CR2        0x41
#define BUCK7_PWRCTRL_CR     0x42

/*
 * LDO CR
 */
#define LDO1_MAIN_CR         0x4C
#define LDO1_ALT_CR          0x4D
#define LDO1_PWRCTRL_CR      0x4E
#define LDO2_MAIN_CR         0x4F
#define LDO2_ALT_CR          0x50
#define LDO2_PWRCTRL_CR      0x51
#define LDO3_MAIN_CR         0x52
#define LDO3_ALT_CR          0x53
#define LDO3_PWRCTRL_CR      0x54
#define LDO4_MAIN_CR         0x55
#define LDO4_ALT_CR          0x56
#define LDO4_PWRCTRL_CR      0x57
#define LDO5_MAIN_CR         0x58
#define LDO5_ALT_CR          0x59
#define LDO5_PWRCTRL_CR      0x5A
#define LDO6_MAIN_CR         0x5B
#define LDO6_ALT_CR          0x5C
#define LDO6_PWRCTRL_CR      0x5D
#define LDO7_MAIN_CR         0x5E
#define LDO7_ALT_CR          0x5F
#define LDO7_PWRCTRL_CR      0x60
#define LDO8_MAIN_CR         0x61
#define LDO8_ALT_CR          0x62
#define LDO8_PWRCTRL_CR      0x63
#define REFDDR_MAIN_CR       0x64
#define REFDDR_ALT_CR        0x65
#define REFDDR_PWRCTRL_CR    0x66

/* MAIN_CR bits definition */

#define MAIN_CR_EN              0x01  /* MAIN control register enable bit */
#define MAIN_CR_BYPASS          0x40  /* Select bypass mode operation */
#define MAIN_CR_SNK_SRC         0x80  /* select sink/source mode operation */
#define MAIN_CR_INPUT_SRC_VOUT4 0x80  /* VOUT4 as inputsource */
/* MAIN control register2 select regulation mode bit: BUCKx operates in high power mode (HP) */
#define MAIN_CR_PREG_MODE0      0x00
/* MAIN control register2 select regulation mode bit: BUCKx operates in low power mode (LP) */
#define MAIN_CR_PREG_MODE1      0x02
/* MAIN control register2 select regulation mode bit: BUCKx operates in Forced PWM mode (CCM) */
#define MAIN_CR_PREG_MODE2      0x04

#define ALT_CR_EN              0x01  /* ALTERNATE control register enable bit */
#define ALT_CR_BYPASS          0x40  /* Select bypass mode operation */
#define ALT_CR_INPUT_SRC_VOUT4 0x80  /* VOUT4 as inputsource */

#define PWRCTRL_CR_EN          0x01  /* PWRCTRL_CR enable bit */
#define PWRCTRL_CR_RST         0x02  /* PWRCTRL_CR reset enable bit */
#define PWRCTRL_CR_SEL0        0x00  /* PWRCTRL_CR no control source bit */
#define PWRCTRL_CR_SEL1        0x04  /* PWRCTRL_CR PWRCTRL1 control source bit */
#define PWRCTRL_CR_SEL2        0x08  /* PWRCTRL_CR PWRCTRL2 control source bit */
#define PWRCTRL_CR_SEL3        0x0C  /* PWRCTRL_CR PWRCTRL3 control source bit */
/* PWRCTRL_CR LDOx control/reset source shift delay from high to Low level bit: no delay */
#define PWRCTRL_CR_DLY_L0      0x00
/* PWRCTRL_CR LDOx control/reset source shift delay from high to Low level bit: 1.5 ms delay */
#define PWRCTRL_CR_DLY_L1      0x10
/* PWRCTRL_CR LDOx control/reset source shift delay from high to Low level bit: 3 ms delay */
#define PWRCTRL_CR_DLY_L2      0x20
/* PWRCTRL_CR LDOx control/reset source shift delay from high to Low level bit: 6 ms delay */
#define PWRCTRL_CR_DLY_L3      0x30
/* PWRCTRL_CR LDOx control/reset source shift delay from low to High level bit: no delay */
#define PWRCTRL_CR_DLY_H0      0x00
/* PWRCTRL_CR LDOx control/reset source shift delay from low to High level bit: 1.5 ms delay */
#define PWRCTRL_CR_DLY_H1      0x40
/* PWRCTRL_CR LDOx control/reset source shift delay from low to High level bit: 3 ms delay */
#define PWRCTRL_CR_DLY_H2      0x80
/* PWRCTRL_CR LDOx control/reset source shift delay from low to High level bit: 6 ms delay */
#define PWRCTRL_CR_DLY_H3      0xC0

/*
 * INTERRUPT CR
 */
#define INT_PENDING_R1       0x70
#define INT_PENDING_R2       0x71
#define INT_PENDING_R3       0x72
#define INT_PENDING_R4       0x73
#define INT_CLEAR_R1         0x74
#define INT_CLEAR_R2         0x75
#define INT_CLEAR_R3         0x76
#define INT_CLEAR_R4         0x77
#define INT_MASK_R1          0x78
#define INT_MASK_R2          0x79
#define INT_MASK_R3          0x7A
#define INT_MASK_R4          0x7B
#define INT_SRC_R1           0x7C
#define INT_SRC_R2           0x7D
#define INT_SRC_R3           0x7E
#define INT_SRC_R4           0x7F
#define INT_DBG_LATCH_R1     0x80
#define INT_DBG_LATCH_R2     0x81
#define INT_DBG_LATCH_R3     0x82
#define INT_DBG_LATCH_R4     0x83

/*
 * NVM registers
 */
/* R     - - NVM_ERROR NVM_PWRDOWN NVM_INIT_DONE NVM_READ_FAIL NVM_WRITE_FAIL NVM_BUSY */
#define NVM_SR                0x8E
/* R/W   - - - - - - NVM_CMD[1:0] */
#define NVM_CR                0x8F
/* R/W   VINOK_HYST[1:0] VINOK_RISE[1:0] NVM_WDG_TMR_SET[1:0] NVM_WDG_EN AUTO_TURN_ON */
#define NVM_MAIN_CTRL_SHR1    0x90
/* R/W   RANK_DLY[1:0] RST_DLY[1:0] NVM_PKEY_LKP_OFF NVM_PKEY_LKP_FSLS NVM_PKEY_LKP_TMR[1:0] */
#define NVM_MAIN_CTRL_SHR2    0x91
/* R/W   - - BUCK2_RANK[2:0] BUCK1_RANK[2:0] */
#define NVM_RANK_SHR1         0x92
/* R/W   - - BUCK4_RANK[2:0] BUCK3_RANK[2:0] */
#define NVM_RANK_SHR2         0x93
/* R/W   - - BUCK6_RANK[2:0] BUCK5_RANK[2:0] */
#define NVM_RANK_SHR3         0x94
/* R/W   - - REFDDR_RANK[2:0] BUCK7_RANK[2:0] */
#define NVM_RANK_SHR4         0x95
/* R/W   - - LDO2_RANK[2:0] LDO1_RANK[2:0] */
#define NVM_RANK_SHR5         0x96
/* R/W   - - LDO4_RANK[2:0] LDO3_RANK[2:0] */
#define NVM_RANK_SHR6         0x97
/* R/W   - - LDO6_RANK[2:0] LDO5_RANK[2:0] */
#define NVM_RANK_SHR7         0x98
/* R/W   - - LDO8_RANK[2:0] LDO7_RANK[2:0] */
#define NVM_RANK_SHR8         0x99
/* R/W   BUCK4_PREG_MODE[1:0] BUCK3_PREG_MODE[1:0] BUCK2_PREG_MODE[1:0] BUCK1_PREG_MODE[1:0] */
#define NVM_BUCK_MODE_SHR1    0x9A
/* R/W   - - BUCK7_PREG_MODE[1:0] BUCK6_PREG_MODE[1:0] BUCK5_PREG_MODE[1:0] */
#define NVM_BUCK_MODE_SHR2    0x9B
/* R/W   - VOUT[6:0] */
#define NVM_BUCK1_VOUT_SHR    0x9C
/* R/W   - VOUT[6:0] */
#define NVM_BUCK2_VOUT_SHR    0x9D
/* R/W   - VOUT[6:0] */
#define NVM_BUCK3_VOUT_SHR    0x9E
/* R/W   - VOUT[6:0] */
#define NVM_BUCK4_VOUT_SHR    0x9F
/* R/W   - VOUT[6:0] */
#define NVM_BUCK5_VOUT_SHR    0xA0
/* R/W   - VOUT[6:0] */
#define NVM_BUCK6_VOUT_SHR    0xA1
/* R/W   - VOUT[6:0] */
#define NVM_BUCK7_VOUT_SHR    0xA2
/* R/W   - BYPASS VOUT[4:0] - */
#define NVM_LDO2_SHR          0xA3
/* R/W   SNK_SRC BYPASS VOUT[4:0] - */
#define NVM_LDO3_SHR          0xA4
/* R/W   - BYPASS VOUT[4:0] - */
#define NVM_LDO5_SHR          0xA5
/* R/W   - BYPASS VOUT[4:0] - */
#define NVM_LDO6_SHR          0xA6
/* R/W   - BYPASS VOUT[4:0] - */
#define NVM_LDO7_SHR          0xA7
/* R/W   - BYPASS VOUT[4:0] - */
#define NVM_LDO8_SHR          0xA8
/* R/W   BUCK4_PD[1:0] BUCK3_PD[1:0] BUCK2_PD[1:0] BUCK1_PD[1:0] */
#define NVM_PD_SHR1           0xA9
/* R/W   REFDDR_PD - BUCK7_PD[1:0] BUCK6_PD[1:0] BUCK5_PD[1:0] */
#define NVM_PD_SHR2           0xAA
/* R/W   LDO8_PD LDO7_PD LDO6_PD LDO5_PD LDO4_PD LDO3_PD LDO2_PD LDO1_PD */
#define NVM_PD_SHR            0xAB
/* R/W   BUCK4_ILIM[1:0] BUCK3_ILIM[1:0] BUCK2_ILIM[1:0] BUCK1_ILIM[1:0] */
#define NVM_BUCKS_IOUT_SHR1   0xAC
/* R/W   HICCUP_DLY[1:0] BUCK7_ILIM[1:0] BUCK6_ILIM[1:0] BUCK5_ILIM[1:0] */
#define NVM_BUCKS_IOUT_SHR2   0xAD
/* R/W   LDO7_ILIM[1:0] LDO6_ILIM[1:0] LDO5_ILIM[1:0] LDO2_ILIM[1:0] */
#define NVM_LDOS_IOUT_SHR     0xAE
/* R/W   NVM_FS_OCP_REFDDR NVM_FS_OCP_BUCK7 NVM_FS_OCP_BUCK6 NVM_FS_OCP_BUCK5
         NVM_FS_OCP_BUCK4 NVM_FS_OCP_BUCK3 NVM_FS_OCP_BUCK2 NVM_FS_OCP_BUCK1 */
#define NVM_FS_OCP_SHR1       0xAF
/* R/W   NVM_FS_OCP_LDO8 NVM_FS_OCP_LDO7 NVM_FS_OCP_LDO6 NVM_FS_OCP_LDO5 NVM_FS_OCP_LDO4
         NVM_FS_OCP_LDO3 NVM_FS_OCP_LDO2 NVM_FS_OCP_LDO1 */
#define NVM_FS_OCP_SHR2       0xB0
/* R/W   VIN_FLT_CNT_MAX[3:0] PKEY_FLT_CNT_MAX[3:0] */
#define NVM_FS_SHR1           0xB1
/* R/W   TSHDN_FLT_CNT_MAX[3:0] OCP_FLT_CNT_MAX[3:0] */
#define NVM_FS_SHR2           0xB2
/* R/W   - FAIL_SAFE_LOCK_DIS RST_FLT_CNT_TMR[1:0] WDG_FLT_CNT_MAX[3:0] */
#define NVM_FS_SHR3           0xB3
/* R/W   - NVM_BUCK_AUTOCAL_CFG[6:0] */
#define NVM_BUCK_AUTOCAL_SHR  0xB4
/* R/W   LOCK_NVM I2C_ADDR[6:0] */
#define NVM_I2C_ADDR_SHR      0xB5
/* R/W   NVM_USER1[7:0] */
#define NVM_USER_SHR1         0xB6
/* R/W   NVM_USER2[7:0] */
#define NVM_USER_SHR2         0xB7
/* R/W   NVM_BUCK4_COMP[1:0] NVM_BUCK3_COMP[1:0] NVM_BUCK2_COMP[1:0] NVM_BUCK1_COMP[1:0] */
#define NVM_BUCK_COMP_SHR1    0xB8
/* R/W   - - NVM_BUCK7_COMP[1:0] NVM_BUCK6_COMP[1:0] NVM_BUCK5_COMP[1:0] */
#define NVM_BUCK_COMP_SHR2    0xB9

/*
 * BUCKS_MRST_CR: Mask reset buck control register bits definition
 */
#define BUCK1_MRST           BIT(0)
#define BUCK2_MRST           BIT(1)
#define BUCK3_MRST           BIT(2)
#define BUCK4_MRST           BIT(3)
#define BUCK5_MRST           BIT(4)
#define BUCK6_MRST           BIT(5)
#define BUCK7_MRST           BIT(6)
#define REFDDR_MRST          BIT(7)

/*
 * LDOS_MRST_CR: Mask reset LDO control register bits definition
 */
#define LDO1_MRST            BIT(0)
#define LDO2_MRST            BIT(1)
#define LDO3_MRST            BIT(2)
#define LDO4_MRST            BIT(3)
#define LDO5_MRST            BIT(4)
#define LDO6_MRST            BIT(5)
#define LDO7_MRST            BIT(6)
#define LDO8_MRST            BIT(7)

/*
 * LDOx_MAIN_CR
 */
#define LDO_VOLT_SHIFT       1
#define LDO_BYPASS           BIT(6)
#define LDO1_INPUT_SRC       BIT(7)
#define LDO3_SNK_SRC         BIT(7)
#define LDO4_INPUT_SRC_SHIFT 6
#define LDO4_INPUT_SRC_MASK  GENMASK_32(7, 6)

/*
 * PWRCTRL register bit definition
 */
#define PWRCTRL_EN          BIT(0)
#define PWRCTRL_RS          BIT(1)
#define PWRCTRL_SEL_SHIFT   2
#define PWRCTRL_SEL_MASK    GENMASK_32(3, 2)

/*
 * BUCKx_MAIN_CR2
 */
#define PREG_MODE_SHIFT     1
#define PREG_MODE_MASK      GENMASK_32(2, 1)

/*
 * BUCKS_PD_CR1
 */
#define BUCK1_PD_MASK       GENMASK_32(1, 0)
#define BUCK2_PD_MASK       GENMASK_32(3, 2)
#define BUCK3_PD_MASK       GENMASK_32(5, 4)
#define BUCK4_PD_MASK       GENMASK_32(7, 6)

#define BUCK1_PD_FAST       BIT(1)
#define BUCK2_PD_FAST       BIT(3)
#define BUCK3_PD_FAST       BIT(5)
#define BUCK4_PD_FAST       BIT(7)

/*
 * BUCKS_PD_CR2
 */
#define BUCK5_PD_MASK       GENMASK_32(1, 0)
#define BUCK6_PD_MASK       GENMASK_32(3, 2)
#define BUCK7_PD_MASK       GENMASK_32(5, 4)

#define BUCK5_PD_FAST       BIT(1)
#define BUCK6_PD_FAST       BIT(3)
#define BUCK7_PD_FAST       BIT(5)

/*
 * LDOS_PD_CR1
 */
#define LDO1_PD             BIT(0)
#define LDO2_PD             BIT(1)
#define LDO3_PD             BIT(2)
#define LDO4_PD             BIT(3)
#define LDO5_PD             BIT(4)
#define LDO6_PD             BIT(5)
#define LDO7_PD             BIT(6)
#define LDO8_PD             BIT(7)

/*
 * LDOS_PD_CR2
 */
#define REFDDR_PD           BIT(0)

/* FS_OCP_CRx
 * 0: OCP hiccup mode (Level 0)
 * 1: OCP fail-safe PMIC turn-off (Level 1)
 */
/*
  * FS_OCP_CR1
  */
#define FS_OCP_BUCK1        BIT(0)
#define FS_OCP_BUCK2        BIT(1)
#define FS_OCP_BUCK3        BIT(2)
#define FS_OCP_BUCK4        BIT(3)
#define FS_OCP_BUCK5        BIT(4)
#define FS_OCP_BUCK6        BIT(5)
#define FS_OCP_BUCK7        BIT(6)
#define FS_OCP_REFDDR       BIT(7)

/*
 * FS_OCP_CR2
 */
#define FS_OCP_LDO1         BIT(0)
#define FS_OCP_LDO2         BIT(1)
#define FS_OCP_LDO3         BIT(2)
#define FS_OCP_LDO4         BIT(3)
#define FS_OCP_LDO5         BIT(4)
#define FS_OCP_LDO6         BIT(5)
#define FS_OCP_LDO7         BIT(6)
#define FS_OCP_LDO8         BIT(7)

/* IRQ definitions */
#define IT_PONKEY_F         0
#define IT_PONKEY_R         1
#define IT_WKP_FA           2
#define IT_WKP_RI           3
#define IT_VINLOW_FA        4
#define IT_VINLOW_RI        5
#define IT_VBUS_FA          6
#define IT_VBUS_RI          7
#define IT_THW_FA           8
#define IT_THW_RI           9
#define IT_BUCK1_OCP        16
#define IT_BUCK2_OCP        17
#define IT_BUCK3_OCP        18
#define IT_BUCK4_OCP        19
#define IT_BUCK5_OCP        20
#define IT_BUCK6_OCP        21
#define IT_BUCK7_OCP        22
#define IT_REFDDR_OCP       23
#define IT_LDO1_OCP         24
#define IT_LDO2_OCP         25
#define IT_LDO3_OCP         26
#define IT_LDO4_OCP         27
#define IT_LDO5_OCP         28
#define IT_LDO6_OCP         29
#define IT_LDO7_OCP         30
#define IT_LDO8_OCP         31

/* Main PMIC Control Register
 * MAIN_CONTROL_REG
 * Address : 0x10
 * */
#define ICC_EVENT_ENABLED           BIT(4)
#define PWRCTRL_POLARITY_HIGH       BIT(3)
#define PWRCTRL_PIN_VALID           BIT(2)
#define RESTART_REQUEST_ENABLED     BIT(1)
#define SOFTWARE_SWITCH_OFF_ENABLED BIT(0)

/* Main PMIC PADS Control Register
 * PADS_PULL_REG
 * Address : 0x11
 * */
#define WAKEUP_DETECTOR_DISABLED    BIT(4)
#define PWRCTRL_PD_ACTIVE           BIT(3)
#define PWRCTRL_PU_ACTIVE           BIT(2)
#define WAKEUP_PD_ACTIVE            BIT(1)
#define PONKEY_PU_ACTIVE            BIT(0)


/* Main PMIC VINLOW Control Register
 * VIN_CONTROL_REGC DMSC
 * Address : 0x15
 * */
#define SWIN_DETECTOR_ENABLED       BIT(7)
#define SWOUT_DETECTOR_ENABLED      BIT(6)
#define VINLOW_HYST_MASK            0x3
#define VINLOW_HYST_SHIFT           4
#define VINLOW_THRESHOLD_MASK       0x7
#define VINLOW_THRESHOLD_SHIFT      1
#define VINLOW_ENABLED              0x01
#define VINLOW_CTRL_REG_MASK        0xFF


/* USB Control Register
 * Address : 0x40
 * */
#define BOOST_OVP_DISABLED          BIT(7)
#define VBUS_OTG_DETECTION_DISABLED BIT(6)
/* Discharge not implemented */
#define OCP_LIMIT_HIGH              BIT(3)
#define SWIN_SWOUT_ENABLED          BIT(2)
#define USBSW_OTG_SWITCH_ENABLED    BIT(1)


/* IRQ masks */
/* Interrupt Mask for Register 1 (0x50 for latch) */
#define IT_SWOUT_R_MASK             BIT(7)
#define IT_SWOUT_F_MASK             BIT(6)
#define IT_VBUS_OTG_R_MASK          BIT(5)
#define IT_VBUS_OTG_F_MASK          BIT(4)
#define IT_WAKEUP_R_MASK            BIT(3)
#define IT_WAKEUP_F_MASK            BIT(2)
#define IT_PONKEY_R_MASK            BIT(1)
#define IT_PONKEY_F_MASK            BIT(0)

/* Interrupt Mask for Register 2 (0x51 for latch) */
#define IT_OVP_BOOST_MASK           BIT(7)
#define IT_OCP_BOOST_MASK           BIT(6)
#define IT_OCP_SWOUT_MASK           BIT(5)
#define IT_OCP_OTG_MASK             BIT(4)
#define IT_CURLIM_BUCK4_MASK        BIT(3)
#define IT_CURLIM_BUCK3_MASK        BIT(2)
#define IT_CURLIM_BUCK2_MASK        BIT(1)
#define IT_CURLIM_BUCK1_MASK        BIT(0)

/* Interrupt Mask for Register 3 (0x52 for latch) */
#define IT_SHORT_SWOUT_MASK         BIT(7)
#define IT_SHORT_SWOTG_MASK         BIT(6)
#define IT_CURLIM_LDO6_MASK         BIT(5)
#define IT_CURLIM_LDO5_MASK         BIT(4)
#define IT_CURLIM_LDO4_MASK         BIT(3)
#define IT_CURLIM_LDO3_MASK         BIT(2)
#define IT_CURLIM_LDO2_MASK         BIT(1)
#define IT_CURLIM_LDO1_MASK         BIT(0)

/* Interrupt Mask for Register 4 (0x53 for latch) */
#define IT_SWIN_R_MASK              BIT(7)
#define IT_SWIN_F_MASK              BIT(6)
/*  Reserved 1 */
/*  Reserved 2 */
#define IT_VINLOW_R_MASK            BIT(3)
#define IT_VINLOW_F_MASK            BIT(2)
#define IT_TWARN_R_MASK             BIT(1)
#define IT_TWARN_F_MASK             BIT(0)

#define PMIC_VERSION_ID             0x10

#define STPMIC_I2C_ADDRESS          (0x33 << 1)

/**
  * @}
  */

/************** Generic Function  *******************/

typedef int32_t (*STPMIC2_Write_Func)(void *, uint16_t, uint8_t *, uint16_t);
typedef int32_t (*STPMIC2_Read_Func)(void *, uint16_t, uint8_t *, uint16_t);

typedef struct
{
  STPMIC2_Write_Func   WriteReg;
  STPMIC2_Read_Func    ReadReg;
  void                *handle;
} stpmic2_ctx_t;

/*******************************************************************************
  * Register      : Generic - All
  * Address       : Generic - All
  * Bit Group Name: None
  * Permission    : W
  *******************************************************************************/
int32_t stpmic2_write_reg(stpmic2_ctx_t *ctx, uint16_t reg, uint8_t *pdata, uint16_t length);
int32_t stpmic2_read_reg(stpmic2_ctx_t *ctx, uint16_t reg, uint8_t *pdata, uint16_t length);

int32_t stpmic2_register_set(stpmic2_ctx_t *ctx, uint16_t reg, uint8_t value);
int32_t stpmic2_register_get(stpmic2_ctx_t *ctx, uint16_t reg, uint8_t *value);
int32_t stpmic2_register_update(stpmic2_ctx_t *ctx, uint16_t reg, uint8_t value, uint8_t mask);


/**
  * @}
  */
#ifdef __cplusplus
}
#endif

#endif /* STPMIC2_REG_H */
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
