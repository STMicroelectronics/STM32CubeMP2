/**
  ******************************************************************************
  * @file    rtl8211.h
  * @author  MCD Application Team
  * @brief   This file contains all the functions prototypes for the
  *          rtl8211.c PHY driver.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics.
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
#ifndef RTL8211_H
#define RTL8211_H

#ifdef __cplusplus
 extern "C" {
#endif   
   
/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/** @addtogroup BSP
  * @{
  */ 

/** @addtogroup Component
  * @{
  */
    
/** @defgroup RTL8211
  * @{
  */    
/* Exported constants --------------------------------------------------------*/
/** @defgroup RTL8211_Exported_Constants RTL8211 Exported Constants
  * @{
  */ 
  
/** @defgroup RTL8211_Registers_Mapping RTL8211 Registers Mapping
  * @{
  */ 
#define RTL8211_BMCR                          ((uint16_t)0x0000U)
#define RTL8211_BMSR                          ((uint16_t)0x0001U)
#define RTL8211_PHYID1                        ((uint16_t)0x0002U)
#define RTL8211_PHYID2                        ((uint16_t)0x0003U)
#define RTL8211_ANAR                          ((uint16_t)0x0004U)
#define RTL8211_ANLPAR                        ((uint16_t)0x0005U)
#define RTL8211_ANER                          ((uint16_t)0x0006U)
#define RTL8211_ANNPTR                        ((uint16_t)0x0007U)
#define RTL8211_ANNPRR                        ((uint16_t)0x0008U)
#define RTL8211_GBCR                          ((uint16_t)0x0009U)
#define RTL8211_GBSR                          ((uint16_t)0x000AU)
#define RTL8211_MACR                          ((uint16_t)0x000DU)
#define RTL8211_MAADR                         ((uint16_t)0x000EU)
#define RTL8211_GBESR                         ((uint16_t)0x000FU)
#define RTL8211_INER_PA42                     ((uint16_t)0x0012U)
#define RTL8211_PHYCR1_PA43                   ((uint16_t)0x0018U)
#define RTL8211_PHYCR2_PA43                   ((uint16_t)0x0019U)
#define RTL8211_PHYSR1_PA43                   ((uint16_t)0x001AU)
#define RTL8211_INSR_PA43                     ((uint16_t)0x001DU)
#define RTL8211_PAGSR                         ((uint16_t)0x001FU)
#define RTL8211_PHYCR3_PA44                   ((uint16_t)0x0011U)
#define RTL8211_PHYSCR_PA46                   ((uint16_t)0x0014U)
#define RTL8211_PHYSR2_PA4B                   ((uint16_t)0x0010U)
#define RTL8211_LCR_PD04                      ((uint16_t)0x0010U)
#define RTL8211_EEELCR_PD04                   ((uint16_t)0x0011U)
#define RTL8211_MIICR1_PD08                   ((uint16_t)0x0011U)
#define RTL8211_MIICR2_PD08                   ((uint16_t)0x0015U)
/**
  * @}
  */

/** @defgroup RTL8211_BMCR_Bit_Definition RTL8211 Basic Mode Control Register Bit Definition
  * @{
  */  
#define RTL8211_BMCR_RESET                    ((uint16_t)0x8000U)
#define RTL8211_BMCR_LOOPBACK                 ((uint16_t)0x4000U)
#define RTL8211_BMCR_SPEED_SEL_LSB            ((uint16_t)0x2000U)
#define RTL8211_BMCR_AN_EN                    ((uint16_t)0x1000U)
#define RTL8211_BMCR_POWER_DOWN               ((uint16_t)0x0800U)
#define RTL8211_BMCR_ISOLATE                  ((uint16_t)0x0400U)
#define RTL8211_BMCR_RESTART_AN               ((uint16_t)0x0200U)
#define RTL8211_BMCR_DUPLEX_MODE              ((uint16_t)0x0100U)
#define RTL8211_BMCR_COL_TEST                 ((uint16_t)0x0080U)
#define RTL8211_BMCR_SPEED_SEL_MSB            ((uint16_t)0x0040U)
#define RTL8211_BMCR_UNI_DIRECT_ENABLE        ((uint16_t)0x0020U)
#define RTL8211_BMCR_SPEED_SEL_1000M          ((uint16_t)0x2000U)
#define RTL8211_BMCR_SPEED_SEL_100M           ((uint16_t)0x0040U)
#define RTL8211_BMCR_SPEED_SEL_10M            ((uint16_t)0x0000U)
/**
  * @}
  */

/** @defgroup RTL8211_BMSR_Bit_Definition RTL8211 Basic Mode Status Register Bit Definition
  * @{
  */   
#define RTL8211_BMSR_100BT4                   ((uint16_t)0x8000U)
#define RTL8211_BMSR_100BTX_FD                ((uint16_t)0x4000U)
#define RTL8211_BMSR_100BTX_HD                ((uint16_t)0x2000U)
#define RTL8211_BMSR_10BT_FD                  ((uint16_t)0x1000U)
#define RTL8211_BMSR_10BT_HD                  ((uint16_t)0x0800U)
#define RTL8211_BMSR_10BT2_FD                 ((uint16_t)0x0400U)
#define RTL8211_BMSR_10BT2_HD                 ((uint16_t)0x0200U)
#define RTL8211_BMSR_EXTENDED_STATUS          ((uint16_t)0x0100U)
#define RTL8211_BMSR_UNI_DIRECT_STATUS        ((uint16_t)0x0080U)
#define RTL8211_BMSR_PREAMBLE_SUPPR           ((uint16_t)0x0040U)
#define RTL8211_BMSR_AN_COMPLETE              ((uint16_t)0x0020U)
#define RTL8211_BMSR_REMOTE_FAULT             ((uint16_t)0x0010U)
#define RTL8211_BMSR_AN_CAPABLE               ((uint16_t)0x0008U)
#define RTL8211_BMSR_LINK_STATUS              ((uint16_t)0x0004U)
#define RTL8211_BMSR_JABBER_DETECT            ((uint16_t)0x0002U)
#define RTL8211_BMSR_EXTENDED_CAPABLE         ((uint16_t)0x0001U)
/**
  * @}
  */

/** @defgroup RTL8211_PHYI1R_Bit_Definition RTL8211 PHY identifier 1 Register Bit Definition
  * @{
  */
#define RTL8211_PHYID1_OUI_MSB                ((uint16_t)0xFFFFU)
#define RTL8211_PHYID1_OUI_MSB_DEFAULT        ((uint16_t)0x001CU)
/**
  * @}
  */

/** @defgroup RTL8211_PHYID2_Bit_Definition RTL8211 PHY identifier 2 Register Bit Definition
  * @{
  */

#define RTL8211_PHYID2_OUI_LSB                ((uint16_t)0xFC00U)
#define RTL8211_PHYID2_OUI_LSB_DEFAULT        ((uint16_t)0xC800U)
#define RTL8211_PHYID2_MODEL_NUM              ((uint16_t)0x03F0U)
#define RTL8211_PHYID2_MODEL_NUM_DEFAULT      ((uint16_t)0x0110U)
#define RTL8211_PHYID2_REVISION_NUM           ((uint16_t)0x000FU)
#define RTL8211_PHYID2_REVISION_NUM_DEFAULT   ((uint16_t)0x0006U)
/**
  * @}
  */

/** @defgroup RTL8211_ANAR_Bit_Definition RTL8211 Auto-Negotiation Advertisement register Bit Definition
  * @{
  */
#define RTL8211_ANAR_NEXT_PAGE                ((uint16_t)0x8000U)
#define RTL8211_ANAR_REMOTE_FAULT             ((uint16_t)0x2000U)
#define RTL8211_ANAR_ASYM_PAUSE               ((uint16_t)0x0800U)
#define RTL8211_ANAR_PAUSE                    ((uint16_t)0x0400U)
#define RTL8211_ANAR_100BT4                   ((uint16_t)0x0200U)
#define RTL8211_ANAR_100BTX_FD                ((uint16_t)0x0100U)
#define RTL8211_ANAR_100BTX_HD                ((uint16_t)0x0080U)
#define RTL8211_ANAR_10BT_FD                  ((uint16_t)0x0040U)
#define RTL8211_ANAR_10BT_HD                  ((uint16_t)0x0020U)
#define RTL8211_ANAR_SELECTOR                 ((uint16_t)0x001FU)
#define RTL8211_ANAR_SELECTOR_DEFAULT         ((uint16_t)0x0001U)
/**
  * @}
  */

/** @defgroup RTL8211_ANLPAR_Bit_Definition RTL8211 Auto-Negotiation Link Partner Ability register Bit Definition
  * @{
  */
#define RTL8211_ANLPAR_NEXT_PAGE              ((uint16_t)0x8000U)
#define RTL8211_ANLPAR_ACK                    ((uint16_t)0x4000U)
#define RTL8211_ANLPAR_REMOTE_FAULT           ((uint16_t)0x2000U)
#define RTL8211_ANLPAR_ABILITY_FIELD          ((uint16_t)0x0FE0U)
#define RTL8211_ANLPAR_ABILITY_FIELD_DEFAULT  ((uint16_t)0x0000U)
#define RTL8211_ANLPAR_SELECTOR               ((uint16_t)0x001FU)
#define RTL8211_ANLPAR_SELECTOR_DEFAULT       ((uint16_t)0x0000U)
/**
  * @}
  */

/** @defgroup RTL8211_ANER_Bit_Definition RTL8211 Auto-Negotiation Expansion register Bit Definition
  * @{
  */
#define RTL8211_ANER_RX_NP_LOC_ABLE           ((uint16_t)0x0040U)
#define RTL8211_ANER_RX_NP_LOC                ((uint16_t)0x0020U)
#define RTL8211_ANER_PAR_DETECT_FAULT         ((uint16_t)0x0010U)
#define RTL8211_ANER_LP_NEXT_PAGE_ABLE        ((uint16_t)0x0008U)
#define RTL8211_ANER_NEXT_PAGE_ABLE           ((uint16_t)0x0004U)
#define RTL8211_ANER_PAGE_RECEIVED            ((uint16_t)0x0002U)
#define RTL8211_ANER_LP_AN_ABLE               ((uint16_t)0x0001U)
/**
  * @}
  */

/** @defgroup RTL8211_ANNPTR_Bit_Definition RTL8211 Auto-Negotiation Next Page Transmit register Bit Definition
  * @{
  */
#define RTL8211_ANNPTR_NEXT_PAGE              ((uint16_t)0x8000U)
#define RTL8211_ANNPTR_MSG_PAGE               ((uint16_t)0x2000U)
#define RTL8211_ANNPTR_ACK2                   ((uint16_t)0x1000U)
#define RTL8211_ANNPTR_TOGGLE                 ((uint16_t)0x0800U)
#define RTL8211_ANNPTR_MESSAGE                ((uint16_t)0x07FFU)
#define RTL8211_ANNPTR_MESSAGE_DEFAULT        ((uint16_t)0x0001U)
/**
  * @}
  */

/** @defgroup RTL8211_ANNPRR_Bit_Definition RTL8211 Auto-Negotiation Next Page Receive register Bit Definition
  * @{
  */
#define RTL8211_ANNPRR_NEXT_PAGE              ((uint16_t)0x8000U)
#define RTL8211_ANNPRR_MSG_PAGE               ((uint16_t)0x2000U)
#define RTL8211_ANNPRR_ACK2                   ((uint16_t)0x1000U)
#define RTL8211_ANNPRR_TOGGLE                 ((uint16_t)0x0800U)
#define RTL8211_ANNPRR_MESSAGE                ((uint16_t)0x07FFU)
#define RTL8211_ANNPRR_MESSAGE_DEFAULT        ((uint16_t)0x0001U)
/**
  * @}
  */

/** @defgroup RTL8211_GBCR_Bit_Definition RTL8211 1000Base-T control register Bit Definition
  * @{
  */
#define RTL8211_GBCR_TEST_MODE                ((uint16_t)0xE000U)
#define RTL8211_GBCR_MS_MAN_CONF_EN           ((uint16_t)0x1000U)
#define RTL8211_GBCR_MS_MAN_CONF_VAL          ((uint16_t)0x0800U)
#define RTL8211_GBCR_PORT_TYPE                ((uint16_t)0x0400U)
#define RTL8211_GBCR_1000BT_FD                ((uint16_t)0x0200U)
/**
  * @}
  */

/** @defgroup RTL8211_GBSR_SR_Bit_Definition RTL8211 1000Base-T status register Bit Definition
  * @{
  */
#define RTL8211_GBSR_MS_CONF_FAULT            ((uint16_t)0x8000U)
#define RTL8211_GBSR_MS_CONF_RES              ((uint16_t)0x4000U)
#define RTL8211_GBSR_LOCAL_RECEIVER_STATUS    ((uint16_t)0x2000U)
#define RTL8211_GBSR_REMOTE_RECEIVER_STATUS   ((uint16_t)0x1000U)
#define RTL8211_GBSR_LP_1000BT_FD             ((uint16_t)0x0800U)
#define RTL8211_GBSR_LP_1000BT_HD             ((uint16_t)0x0400U)
#define RTL8211_GBSR_IDLE_ERR_COUNT           ((uint16_t)0x00FFU)
/**
  * @}
  */

/** @defgroup RTL8211_MACR_Bit_Definition RTL8211 MMD Access Control register Bit Definition
  * @{
  */
#define RTL8211_MACR_FUNCT                    ((uint16_t)0xC000U)
#define RTL8211_MACR_DEVAD                    ((uint16_t)0x001FU)
/**
  * @}
  */

 /** @defgroup RTL8211_MAADR_Bit_Definition RTL8211 MMD Access Address Data register Bit Definition
   * @{
   */
 #define RTL8211_MAADR_ADD_DATA               ((uint16_t)0xFFFFU)
 #define RTL8211_MAADR_ADD_DATA_DEFAULT       ((uint16_t)0x0000U)
 /**
   * @}
   */

/** @defgroup RTL8211_GBSER_Bit_Definition RTL8211 1000Base-T Extended Status register Bit Definition
  * @{
  */
#define RTL8211_GBESR_1000BX_FD               ((uint16_t)0x8000U)
#define RTL8211_GBESR_1000BX_HD               ((uint16_t)0x4000U)
#define RTL8211_GBESR_1000BT_FD               ((uint16_t)0x2000U)
#define RTL8211_GBESR_1000BT_HD               ((uint16_t)0x1000U)
/**
  * @}
  */

/** @defgroup RTL8211_INER_Bit_Definition RTL8211 Interrupt Enable register Bit Definition
  * @{
  */
#define RTL8211_INER_JABBER                   ((uint16_t)0x4000U)
#define RTL8211_INER_ALDPS_STATE              ((uint16_t)0x2000U)
#define RTL8211_INER_PME                      ((uint16_t)0x0080U)
#define RTL8211_INER_PHY_REG_ACCESS           ((uint16_t)0x0020U)
#define RTL8211_INER_LINK_STATUS_CHANGE       ((uint16_t)0x0010U)
#define RTL8211_INER_AN_COMPLETE              ((uint16_t)0x0008U)
#define RTL8211_INER_PAGE_RECEIVED            ((uint16_t)0x0004U)
#define RTL8211_INER_AN_ERROR                 ((uint16_t)0x0001U)
/**
  * @}
  */

/** @defgroup RTL8211_PHYCR1_Bit_Definition RTL8211 PHY Specific Control register 1 Bit Definition
  * @{
  */
#define RTL8211_PHYCR1_PHYAD0_ENABLE          ((uint16_t)0x2000U)
#define RTL8211_PHYCR1_XTAL_OFF               ((uint16_t)0x1000U)
#define RTL8211_PHYCR1_MDI_MODE_MANUAL        ((uint16_t)0x0200U)
#define RTL8211_PHYCR1_MDI_MODE               ((uint16_t)0x0100U)
#define RTL8211_PHYCR1_TX_CRS_ENABLE          ((uint16_t)0x0800U)
#define RTL8211_PHYCR1_PHYAD_NZERO_DETECT     ((uint16_t)0x0400U)
#define RTL8211_PHYCR1_PREAMBLE_CHECK_ENABLE  ((uint16_t)0x0010U)
#define RTL8211_PHYCR1_JABBER_DETECT_ENABLE   ((uint16_t)0x0008U)
#define RTL8211_PHYCR1_ALDPS_ENABLE           ((uint16_t)0x0004U)
#define RTL8211_PHYCR1_ALDPS_PLLOFF_ENABLE    ((uint16_t)0x0001U)
/**
  * @}
  */

/** @defgroup RTL8211_PHYCR2_Bit_Definition RTL8211 PHY Specific Control register 2 Bit Definition
  * @{
  */
#define RTL8211_PHYCR2_CLKOUT_CAPABILITY      ((uint16_t)0x3000U)
#define RTL8211_PHYCR2_CLKOUT_FREQ_SEL        ((uint16_t)0x8000U)
#define RTL8211_PHYCR2_CLKOUT_SSC_ENABLE      ((uint16_t)0x0080U)
#define RTL8211_PHYCR2_CENTERTAP_SHORT_ENABLE ((uint16_t)0x0040U)
#define RTL8211_PHYCR2_PHY_MODE_EEE_ENABLE    ((uint16_t)0x0020U)
#define RTL8211_PHYCR2_SYSCLK_SSC_ENABLE      ((uint16_t)0x0008U)
#define RTL8211_PHYCR2_RXC_ENABLE             ((uint16_t)0x0002U)
#define RTL8211_PHYCR2_CLKOUT_ENABLE          ((uint16_t)0x0001U)
/**
  * @}
  */

/** @defgroup RTL8211_PHYSR1_Bit_Definition RTL8211 PHY Specific Status register 1 Bit Definition
  * @{
  */
#define RTL8211_PHYSR1_ALDPS_STATE            ((uint16_t)0x4000U)
#define RTL8211_PHYSR1_MDI_PLUG               ((uint16_t)0x2000U)
#define RTL8211_PHYSR1_NWAY_ENABLE            ((uint16_t)0x1000U)
#define RTL8211_PHYSR1_MASTER_MODE            ((uint16_t)0x0800U)
#define RTL8211_PHYSR1_EEE_CAPABILITY         ((uint16_t)0x0100U)
#define RTL8211_PHYSR1_RXFLOW_ENABLE          ((uint16_t)0x0080U)
#define RTL8211_PHYSR1_TXFLOW_ENABLE          ((uint16_t)0x0040U)
#define RTL8211_PHYSR1_SPEED_1000M            ((uint16_t)0x0020U)
#define RTL8211_PHYSR1_SPEED_100M             ((uint16_t)0x0010U)
#define RTL8211_PHYSR1_SPEED_10M              ((uint16_t)0x0000U)
#define RTL8211_PHYSR1_DUPLEX                 ((uint16_t)0x0008U)
#define RTL8211_PHYSR1_LINK_RT                ((uint16_t)0x0004U)
#define RTL8211_PHYSR1_MDI_STATUS             ((uint16_t)0x0002U)
#define RTL8211_PHYSR1_JABBER_RT              ((uint16_t)0x0001U)
/**
  * @}
  */

/** @defgroup RTL8211_INSR_Bit_Definition RTL8211 Interrupt Status register Bit Definition
  * @{
  */
#define RTL8211_INSR_JABBER                   ((uint16_t)0x4000U)
#define RTL8211_INSR_ALDPS_STATE              ((uint16_t)0x2000U)
#define RTL8211_INSR_PME                      ((uint16_t)0x0080U)
#define RTL8211_INSR_PHY_REG_ACCESS           ((uint16_t)0x0020U)
#define RTL8211_INSR_LINK_STATUS_CHANGE       ((uint16_t)0x0010U)
#define RTL8211_INSR_AN_COMPLETE              ((uint16_t)0x0008U)
#define RTL8211_INSR_PAGE_RECEIVED            ((uint16_t)0x0004U)
#define RTL8211_INSR_AN_ERROR                 ((uint16_t)0x0001U)
/**
  * @}
  */

/** @defgroup RTL8211_PAGESR_Bit_Definition RTL8211 Page Select register Bit Definition
  * @{
  */
#define RTL8211_PAGSR_PAGE_SEL                ((uint16_t)0x0FFFU)
#define RTL8211_PAGSR_PAGE_SEL_DEFAULT        ((uint16_t)0x0A42U)
/**
  * @}
  */

/** @defgroup RTL8211_LCR_Bit_Definition RTL8211 LED Control register Bit Definition
  * @{
  */
#define RTL8211_LCR_LED_MODE                  ((uint16_t)0x8000U)
#define RTL8211_LCR_LED2_ACT                  ((uint16_t)0x4000U)
#define RTL8211_LCR_LED2_LINK_1000            ((uint16_t)0x2000U)
#define RTL8211_LCR_LED2_LINK_100             ((uint16_t)0x0800U)
#define RTL8211_LCR_LED2_LINK_10              ((uint16_t)0x0400U)
#define RTL8211_LCR_LED1_ACT                  ((uint16_t)0x0200U)
#define RTL8211_LCR_LED1_LINK_1000            ((uint16_t)0x0100U)
#define RTL8211_LCR_LED1_LINK_100             ((uint16_t)0x0040U)
#define RTL8211_LCR_LED1_LINK_10              ((uint16_t)0x0020U)
#define RTL8211_LCR_LED0_ACT                  ((uint16_t)0x0010U)
#define RTL8211_LCR_LED0_LINK_1000            ((uint16_t)0x0008U)
#define RTL8211_LCR_LED0_LINK_100             ((uint16_t)0x0002U)
#define RTL8211_LCR_LED0_LINK_10              ((uint16_t)0x0001U)
/**
  * @}
  */

/** @defgroup RTL8211_EEELCR_Bit_Definition RTL8211 EEE LED Control register Bit Definition
  * @{
  */
#define RTL8211_EEELCR_LED2_EEE_ENABLE        ((uint16_t)0x0008U)
#define RTL8211_EEELCR_LED1_EEE_ENABLE        ((uint16_t)0x0004U)
#define RTL8211_EEELCR_LED0_EEE_ENABLE        ((uint16_t)0x0002U)
/**
  * @}
  */

/** @defgroup RTL8211_MIICR1_Bit_Definition RTL8211 MII Control register 1 Bit Definition
  * @{
  */
#define RTL8211_MIICR1_TXDLY_ENABLE           ((uint16_t)0x0100U)
/**
  * @}
  */

/** @defgroup RTL8211_MIICR2_Bit_Definition RTL8211 MII Control register 2 Bit Definition
  * @{
  */
#define RTL8211_MIICR2_RGMII_CRS_ENABLE       ((uint16_t)0x0040U)
#define RTL8211_MIICR2_RXDLY_ENABLE           ((uint16_t)0x0008U)
/**
  * @}
  */

/** @defgroup RTL8211_STATUS RTL8211 Status
  * @{
  */
#define  RTL8211_STATUS_100MBITS_READ_AGAIN   ((int32_t)-6)
#define  RTL8211_STATUS_READ_ERROR            ((int32_t)-5)
#define  RTL8211_STATUS_WRITE_ERROR           ((int32_t)-4)
#define  RTL8211_STATUS_ADDRESS_ERROR         ((int32_t)-3)
#define  RTL8211_STATUS_RESET_TIMEOUT         ((int32_t)-2)
#define  RTL8211_STATUS_ERROR                 ((int32_t)-1)
#define  RTL8211_STATUS_OK                    ((int32_t) 0)
#define  RTL8211_STATUS_LINK_DOWN             ((int32_t) 1)
#define  RTL8211_STATUS_100MBITS_FULLDUPLEX   ((int32_t) 2)
#define  RTL8211_STATUS_100MBITS_HALFDUPLEX   ((int32_t) 3)
#define  RTL8211_STATUS_10MBITS_FULLDUPLEX    ((int32_t) 4)
#define  RTL8211_STATUS_10MBITS_HALFDUPLEX    ((int32_t) 5)
#define  RTL8211_STATUS_AUTONEGO_NOTDONE      ((int32_t) 6)
#define  RTL8211_STATUS_1000MBITS_FULLDUPLEX  ((int32_t) 7)
#define  RTL8211_STATUS_1000MBITS_HALFDUPLEX  ((int32_t) 8)
/**
  * @}
  */

/* Exported types ------------------------------------------------------------*/ 
/** @defgroup RTL8211_Exported_Types RTL8211 Exported Types
  * @{
  */
typedef int32_t  (*rtl8211_Init_Func) (void);
typedef int32_t  (*rtl8211_DeInit_Func) (void);
typedef int32_t  (*rtl8211_ReadReg_Func) (uint32_t, uint32_t, uint32_t *);
typedef int32_t  (*rtl8211_WriteReg_Func) (uint32_t, uint32_t, uint32_t);
typedef int32_t  (*rtl8211_GetTick_Func) (void);

typedef struct 
{                   
  rtl8211_Init_Func      Init;
  rtl8211_DeInit_Func    DeInit;
  rtl8211_WriteReg_Func  WriteReg;
  rtl8211_ReadReg_Func   ReadReg;
  rtl8211_GetTick_Func   GetTick;
} rtl8211_IOCtx_t;

  
typedef struct 
{
  uint32_t            DevAddr;
  uint32_t            Is_Initialized;
  rtl8211_IOCtx_t     IO;
  void                *pData;
} rtl8211_Object_t;
/**
  * @}
  */ 

/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/** @defgroup RTL8211_Exported_Functions RTL8211 Exported Functions
  * @{
  */
int32_t RTL8211_RegisterBusIO(rtl8211_Object_t *pObj, rtl8211_IOCtx_t *ioctx);
int32_t RTL8211_Init(rtl8211_Object_t *pObj);
int32_t RTL8211_DeInit(rtl8211_Object_t *pObj);
int32_t RTL8211_DisablePowerDownMode(rtl8211_Object_t *pObj);
int32_t RTL8211_EnablePowerDownMode(rtl8211_Object_t *pObj);
int32_t RTL8211_StartAutoNego(rtl8211_Object_t *pObj);
int32_t RTL8211_GetLinkState(rtl8211_Object_t *pObj);
int32_t RTL8211_SetLinkState(rtl8211_Object_t *pObj, uint32_t LinkState);
int32_t RTL8211_EnableLoopbackMode(rtl8211_Object_t *pObj);
int32_t RTL8211_DisableLoopbackMode(rtl8211_Object_t *pObj);
int32_t RTL8211_EnableIT(rtl8211_Object_t *pObj, uint32_t Interrupt);
int32_t RTL8211_DisableIT(rtl8211_Object_t *pObj, uint32_t Interrupt);
int32_t RTL8211_ClearIT(rtl8211_Object_t *pObj, uint32_t Interrupt);
int32_t RTL8211_GetITStatus(rtl8211_Object_t *pObj, uint32_t Interrupt);
/**
  * @}
  */ 

#ifdef __cplusplus
}
#endif
#endif /* RTL8211_H */


/**
  * @}
  */ 

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
