/**
  ******************************************************************************
  * @file    stpmic2l_reg.h
  * @author  MCD Application Team
  * @brief   Header of stpmic2l_reg.c
  *
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STPMIC2L_REG_H
#define STPMIC2L_REG_H

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

/** @addtogroup STPMIC2L
  * @{
  */

/** @defgroup STPMIC2L_Exported_Types
  * @{
  */

/**
  * @}
  */

/** @defgroup STPMIC2L_Exported_Constants STPMIC2L Exported Constants
  * @{
  */
#define BIT(_x)  (1<<(_x))
#define STM32_PMIC_NUM_IRQ_REGS       4

#define GENMASK_32(h, l) \
 (((0xFFFFFFFF) << (l)) & (0xFFFFFFFF >> (32 - 1 - (h))))


/**
  * @brief  STPMIC2L Registers address
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
 #define GPO_SR               0x0D

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
 #define BUCKS_PD_CR          0x19
 #define STPMIC2L_RESERVED_1  0x1A
 #define LDOS_PD_CR           0x1B
 #define GPO_MRST_CR          0x1C
 #define BUCKS_MRST_CR        0x1D
 #define LDOS_MRST_CR         0x1E


#define STANDBY_PWRCTRL_SEL_0 0x00  /* 00: No source (PMIC operates in RUN state) */
#define STANDBY_PWRCTRL_SEL_1 0x20  /* 01: PWRCTRL1 control source */
#define STANDBY_PWRCTRL_SEL_2 0x40  /* 10: PWRCTRL2 control source */
#define STANDBY_PWRCTRL_SEL_3 0x60  /* 11: PWRCTRL3 control source */

 /*
  * Buck CR
  */
 #define BUCK1_MAIN_CR1            0x20
 #define BUCK1_MAIN_CR2            0x21
 #define BUCK1_ALT_CR1             0x22
 #define BUCK1_ALT_CR2             0x23
 #define BUCK1_PWRCTRL_CR          0x24
 #define BUCK2_MAIN_CR1            0x25
 #define BUCK2_MAIN_CR2            0x26
 #define BUCK2_ALT_CR1             0x27
 #define BUCK2_ALT_CR2             0x28
 #define BUCK2_PWRCTRL_CR          0x29
 #define BUCK3_MAIN_CR1            0x2A
 #define BUCK3_MAIN_CR2            0x2B
 #define BUCK3_ALT_CR1             0x2C
 #define BUCK3_ALT_CR2             0x2D
 #define BUCK3_PWRCTRL_CR          0x2E
 #define STPMIC2L_RESERVED_2       0x2F
 #define STPMIC2L_RESERVED_3       0x30
 #define STPMIC2L_RESERVED_4       0x31
 #define STPMIC2L_RESERVED_5       0x32
 #define STPMIC2L_RESERVED_6       0x33
 #define STPMIC2L_RESERVED_7       0x34
 #define STPMIC2L_RESERVED_8       0x35
 #define STPMIC2L_RESERVED_9       0x36
 #define STPMIC2L_RESERVED_10      0x37
 #define STPMIC2L_RESERVED_11      0x38
 #define STPMIC2L_RESERVED_12      0x39
 #define STPMIC2L_RESERVED_13      0x3A
 #define STPMIC2L_RESERVED_14      0x3B
 #define STPMIC2L_RESERVED_15      0x3C
 #define STPMIC2L_RESERVED_16      0x3D
 #define STPMIC2L_RESERVED_17      0x3E
 #define STPMIC2L_RESERVED_18      0x3F
 #define STPMIC2L_RESERVED_19      0x40
 #define STPMIC2L_RESERVED_20      0x41
 #define STPMIC2L_RESERVED_21      0x42
 #define GPO1_MAIN_CR              0X43
 #define GPO1_ALT_CR               0X44
 #define GPO1_PWRCTRL_CR           0X45
 #define GPO2_MAIN_CR              0X46
 #define GPO2_ALT_CR               0X47
 #define GPO2_PWRCTRL_CR           0X48
 

 /*
  * LDO CR
  */
 #define LDO1_MAIN_CR              0x4C
 #define LDO1_ALT_CR               0x4D
 #define LDO1_PWRCTRL_CR           0x4E
 #define LDO2_MAIN_CR              0x4F
 #define LDO2_ALT_CR               0x50
 #define LDO2_PWRCTRL_CR           0x51
 #define LDO3_MAIN_CR              0x52
 #define LDO3_ALT_CR               0x53
 #define LDO3_PWRCTRL_CR           0x54
 #define LDO4_MAIN_CR              0x55
 #define LDO4_ALT_CR               0x56
 #define LDO4_PWRCTRL_CR           0x57
 #define LDO5_MAIN_CR              0x58
 #define LDO5_ALT_CR               0x59
 #define LDO5_PWRCTRL_CR           0x5A
 #define LDO6_MAIN_CR              0x5B
 #define LDO6_ALT_CR               0x5C
 #define LDO6_PWRCTRL_CR           0x5D
 #define LDO7_MAIN_CR              0x5E
 #define LDO7_ALT_CR               0x5F
 #define LDO7_PWRCTRL_CR           0x60
 #define STPMIC2L_RESERVED_22      0x61
 #define STPMIC2L_RESERVED_23      0x62
 #define STPMIC2L_RESERVED_24      0x63
 #define STPMIC2L_RESERVED_25      0x64
 #define STPMIC2L_RESERVED_26      0x65
 #define STPMIC2L_RESERVED_27      0x66
 #define GPO3_MAIN_CR              0x67
 #define GPO3_ALT_CR               0x68
 #define GPO3_PWRCTRL_CR           0x69
 #define GPO4_MAIN_CR              0x6A
 #define GPO4_ALT_CR               0x6B
 #define GPO4_PWRCTRL_CR           0x6C
 #define GPO5_MAIN_CR              0x6D
 #define GPO5_ALT_CR               0x6E
 #define GPO5_PWRCTRL_CR           0x6F

 /* MAIN_CR bits definition */

#define MAIN_CR_EN                0x01 
#define MAIN_CR_SNK_SRC           0x80 
#define MAIN_CR_PREG_AUTO_MODE    0x00 
#define MAIN_CR_PREG_FORCED_MODE  0x04 

#define ALT_CR_EN                 0x01  

#define PWRCTRL_CR_EN             0x01  
#define PWRCTRL_CR_RST            0x02  
#define PWRCTRL_CR_SEL0           0x00  
#define PWRCTRL_CR_SEL1           0x04  
#define PWRCTRL_CR_SEL2           0x08
#define PWRCTRL_CR_SEL3           0x0C 
#define PWRCTRL_CR_DLY_L0         0x00  
#define PWRCTRL_CR_DLY_L1         0x10  
#define PWRCTRL_CR_DLY_L2         0x20  
#define PWRCTRL_CR_DLY_L3         0x30  
#define PWRCTRL_CR_DLY_H0         0x00  
#define PWRCTRL_CR_DLY_H1         0x40  
#define PWRCTRL_CR_DLY_H2         0x80  
#define PWRCTRL_CR_DLY_H3         0xC0  

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
#define NVM_SR                0x8E 
#define NVM_CR                0x8F 
#define NVM_MAIN_CTRL_SHR1    0x90 
#define NVM_MAIN_CTRL_SHR2    0x91 
#define NVM_RANK_SHR1         0x92 
#define NVM_RANK_SHR2         0x93 
#define NVM_FREE_SHR1         0x94 
#define NVM_FREE_SHR2         0x95 
#define NVM_RANK_SHR5         0x96 
#define NVM_RANK_SHR6         0x97 
#define NVM_RANK_SHR7         0x98 
#define NVM_RANK_SHR8         0x99 
#define NVM_BUCK_MODE_SHR1    0x9A 
#define NVM_FREE_SH3          0x9B 
#define NVM_BUCK1_VOUT_SHR    0x9C 
#define NVM_BUCK2_VOUT_SHR    0x9D 
#define NVM_BUCK3_VOUT_SHR    0x9E
#define NVM_MAIN_CTRL_SHR3    0x9F 
#define NVM_RANK_SHR9         0xA0 
#define NVM_RANK_SHR10        0xA1
#define NVM_RANK_SHR11        0xA2
#define NVM_LDO2_SHR          0xA3 
#define NVM_LDO3_SHR          0xA4 
#define NVM_LDO5_SHR          0xA5 
#define NVM_LDO6_SHR          0xA6
#define NVM_LDO7_SHR          0xA7
#define NVM_FREE_SHR4         0xA8 
#define NVM_PD_SHR1           0xA9 
#define NVM_FREE_SHR5         0xAA 
#define NVM_PD_SHR3           0xAB 
#define NVM_BUCKS_IOUT_SHR1   0xAC 
#define NVM_BUCKS_IOUT_SHR2   0xAD 
#define NVM_LDOS_IOUT_SHR     0xAE 
#define NVM_FS_OCP_SHR1       0xAF 
#define NVM_FS_OCP_SHR2       0xB0 
#define NVM_FS_SHR1           0xB1 
#define NVM_FS_SHR2           0xB2 
#define NVM_FS_SHR3           0xB3 
#define NVM_BUCK_AUTOCAL_SHR  0xB4 
#define NVM_I2C_ADDR_SHR      0xB5 
#define NVM_USER_SHR1         0xB6 
#define NVM_USER_SHR2         0xB7 
#define STPMIC2L_RESERVED_28  0xB8 
#define NVM_MAIN_CTRL_SHR4    0xB9 

 /*
  * BUCKS_MRST_CR: Mask reset buck control register bits definition
  */
 #define BUCK1_MRST           BIT(0)
 #define BUCK2_MRST           BIT(1)
 #define BUCK3_MRST           BIT(2)
 /*  STPMIC2L_RESERVED 1 */
 /*  STPMIC2L_RESERVED 2 */
 /*  STPMIC2L_RESERVED 3 */
 /*  STPMIC2L_RESERVED 4 */
 /*  STPMIC2L_RESERVED 5 */

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
 /*  STPMIC2L_RESERVED 1 */

 /*
  * LDOx_MAIN_CR
  */
 #define LDO_VOLT_SHIFT       1
 #define LDO3_SNK_SRC         BIT(7)


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

 #define BUCK1_PD_FAST       BIT(1)
 #define BUCK2_PD_FAST       BIT(3)
 #define BUCK3_PD_FAST       BIT(5)

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

 /* IRQ definitions */
 #define IT_PONKEY_F         0
 #define IT_PONKEY_R         1
 #define IT_VINLOW_FA        4
 #define IT_VINLOW_RI        5
 #define IT_THW_FA           8
 #define IT_THW_RI           9
 #define IT_BUCK1_OCP        16
 #define IT_BUCK2_OCP        17
 #define IT_BUCK3_OCP        18
 #define IT_LDO1_OCP         24
 #define IT_LDO2_OCP         25
 #define IT_LDO3_OCP         26
 #define IT_LDO4_OCP         27
 #define IT_LDO5_OCP         28
 #define IT_LDO6_OCP         29
 #define IT_LDO7_OCP         30


 /* Main PMIC Control Register
  * MAIN_CONTROL_REG
  * */

 #define PWRCTRL3_POLARITY_HIGH      BIT(4)
 #define PWRCTRL2_POLARITY_HIGH      BIT(3)
 #define PWRCTRL1_POLARITY_HIGH      BIT(2)
 #define RESTART_REQUEST_ENABLED     BIT(1)
 #define SOFTWARE_SWITCH_OFF_ENABLED BIT(0)

 /* Main PMIC PADS Control Register */
 #define PWRCTRL3_MASK               0x3
 #define PWRCTRL3_SHIFT              6
 #define PWRCTRL2_MASK               0x3
 #define PWRCTRL2_SHIFT              4
 #define PWRCTRL1_MASK               0x3
 #define PWRCTRL1_SHIFT              2
 #define PONKEY_MASK                 0x3
 #define PONKEY_SHIFT                0
 #define PADS_PULL_REG_MASK          0xFF

 /* Main PMIC VINLOW Control Register */
 #define VINLOW_HYST_MASK            0x3
 #define VINLOW_HYST_SHIFT           4
 #define VINLOW_RISE_MASK            0x7
 #define VINLOW_RISE_SHIFT           1
 #define VINLOW_ENABLED              0x01
 #define VINLOW_CTRL_REG_MASK        0xFF

 /* IRQ masks */
 /* Interrupt Mask for Register 1  */

 /*  STPMIC2L_RESERVED 1 */
 /*  STPMIC2L_RESERVED 2 */
 #define IT_VINLOW_R_MASK          BIT(5)
 #define IT_VINLOW_F_MASK          BIT(4)
 /*  STPMIC2L_RESERVED 3 */
 /*  STPMIC2L_RESERVED 4 */
 #define IT_PONKEY_R_MASK          BIT(1)
 #define IT_PONKEY_F_MASK          BIT(0)


 /* Interrupt Mask for Register 2 */

 /*  STPMIC2L_RESERVED 1 */
 /*  STPMIC2L_RESERVED 2 */
 /*  STPMIC2L_RESERVED 3 */
 /*  STPMIC2L_RESERVED 4 */
 /*  STPMIC2L_RESERVED 5 */
 /*  STPMIC2L_RESERVED 6 */
 #define IT_THW_R_MASK             BIT(1)
 #define IT_THW_F_MASK             BIT(0)


 /* Interrupt Mask for Register 3  */

 /*  STPMIC2L_RESERVED 1 */
 /*  STPMIC2L_RESERVED 2 */
 /*  STPMIC2L_RESERVED 3 */
 /*  STPMIC2L_RESERVED 4 */
 /*  STPMIC2L_RESERVED 5 */
 #define IT_BUCK3_OCP_MASK          BIT(2)
 #define IT_BUCK2_OCP_MASK          BIT(1)
 #define IT_BUCK1_OCP_MASK          BIT(0)


 /* Interrupt Mask for Register 4  */

 /*  STPMIC2L_RESERVED 1 */
 #define IT_LDO7_OCP_MASK            BIT(7)
 #define IT_LDO6_OCP_MASK            BIT(6)
 #define IT_LDO5_OCP_MASK            BIT(4)
 #define IT_LDO4_OCP_MASK            BIT(3)
 #define IT_LDO3_OCP_MASK            BIT(2)
 #define IT_LDO2_OCP_MASK            BIT(1)
 #define IT_LDO1_OCP_MASK            BIT(0)

 #define PMIC_VERSION_ID             0x01

 #define STPMIC_I2C_ADDRESS          (0x33 << 1)

/**
  * @}
  */

/************** Generic Function  *******************/

typedef int32_t (*STPMIC2L_Write_Func)(void *, uint16_t, uint8_t*, uint16_t);
typedef int32_t (*STPMIC2L_Read_Func) (void *, uint16_t, uint8_t*, uint16_t);

typedef struct
{
  STPMIC2L_Write_Func   WriteReg;
  STPMIC2L_Read_Func    ReadReg;
  void                *handle;
} stpmic2l_ctx_t;

/*******************************************************************************
* Register      : Generic - All
* Address       : Generic - All
* Bit Group Name: None
* Permission    : W
*******************************************************************************/
int32_t stpmic2l_write_reg(stpmic2l_ctx_t *ctx, uint16_t reg, uint8_t *pdata, uint16_t length);
int32_t stpmic2l_read_reg(stpmic2l_ctx_t *ctx, uint16_t reg, uint8_t *pdata, uint16_t length);

int32_t stpmic2l_register_set(stpmic2l_ctx_t *ctx, uint16_t reg, uint8_t value);
int32_t stpmic2l_register_get(stpmic2l_ctx_t *ctx, uint16_t reg, uint8_t *value);
int32_t stpmic2l_register_update(stpmic2l_ctx_t *ctx, uint16_t reg, uint8_t value, uint8_t mask);


/**
  * @}
  */
#ifdef __cplusplus
}
#endif

#endif /* STPMIC2L_REG_H */
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
