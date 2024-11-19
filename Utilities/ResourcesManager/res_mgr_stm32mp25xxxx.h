/**
  ******************************************************************************
  * @file           : res_mgr_stm32mp25xxxx.h
  * @brief          : Header for res_mgr.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef RES_MGR_STM32MP25XXXX_H
#define RES_MGR_STM32MP25XXXX_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/


/* Exported macros -----------------------------------------------------------*/

/* RIFSC Peripheral ID selection macro*/

#define STM32MP25_RIFSC_TIM1_ID           0
#define STM32MP25_RIFSC_TIM2_ID           1
#define STM32MP25_RIFSC_TIM3_ID           2
#define STM32MP25_RIFSC_TIM4_ID           3
#define STM32MP25_RIFSC_TIM5_ID           4
#define STM32MP25_RIFSC_TIM6_ID           5
#define STM32MP25_RIFSC_TIM7_ID           6
#define STM32MP25_RIFSC_TIM8_ID           7
#define STM32MP25_RIFSC_TIM10_ID          8
#define STM32MP25_RIFSC_TIM11_ID          9
#define STM32MP25_RIFSC_TIM12_ID          10
#define STM32MP25_RIFSC_TIM13_ID          11
#define STM32MP25_RIFSC_TIM14_ID          12
#define STM32MP25_RIFSC_TIM15_ID          13
#define STM32MP25_RIFSC_TIM16_ID          14
#define STM32MP25_RIFSC_TIM17_ID          15
#define STM32MP25_RIFSC_TIM20_ID          16
#define STM32MP25_RIFSC_LPTIM1_ID         17
#define STM32MP25_RIFSC_LPTIM2_ID         18
#define STM32MP25_RIFSC_LPTIM3_ID         19
#define STM32MP25_RIFSC_LPTIM4_ID         20
#define STM32MP25_RIFSC_LPTIM5_ID         21
#define STM32MP25_RIFSC_SPI1_ID           22
#define STM32MP25_RIFSC_SPI2_ID           23
#define STM32MP25_RIFSC_SPI3_ID           24
#define STM32MP25_RIFSC_SPI4_ID           25
#define STM32MP25_RIFSC_SPI5_ID           26
#define STM32MP25_RIFSC_SPI6_ID           27
#define STM32MP25_RIFSC_SPI7_ID         28
#define STM32MP25_RIFSC_SPI8_ID           29
#define STM32MP25_RIFSC_SPDIFRX_ID          30
#define STM32MP25_RIFSC_USART1_ID         31
#define STM32MP25_RIFSC_USART2_ID         32
#define STM32MP25_RIFSC_USART3_ID         33
#define STM32MP25_RIFSC_UART4_ID          34
#define STM32MP25_RIFSC_UART5_ID          35
#define STM32MP25_RIFSC_USART6_ID         36
#define STM32MP25_RIFSC_UART7_ID          37
#define STM32MP25_RIFSC_UART8_ID          38
#define STM32MP25_RIFSC_UART9_ID          39
#define STM32MP25_RIFSC_LPUART1_ID        40
#define STM32MP25_RIFSC_I2C1_ID           41
#define STM32MP25_RIFSC_I2C2_ID           42
#define STM32MP25_RIFSC_I2C3_ID           43
#define STM32MP25_RIFSC_I2C4_ID           44
#define STM32MP25_RIFSC_I2C5_ID           45
#define STM32MP25_RIFSC_I2C6_ID           46
#define STM32MP25_RIFSC_I2C7_ID           47
#define STM32MP25_RIFSC_I2C8_ID           48
#define STM32MP25_RIFSC_SAI1_ID           49
#define STM32MP25_RIFSC_SAI2_ID           50
#define STM32MP25_RIFSC_SAI3_ID           51
#define STM32MP25_RIFSC_SAI4_ID           52
#define STM32MP25_RIFSC_MDF1_ID           54
#define STM32MP25_RIFSC_ADF1_ID           55
#if !defined(STM32MP251Cxx) || !defined(STM32MP251Fxx) || !defined(STM32MP251Axx) || !defined(STM32MP251Dxx)
#define STM32MP25_RIFSC_FDCAN_ID          56
#endif
#define STM32MP25_RIFSC_HDP_ID            57
#define STM32MP25_RIFSC_ADC12_ID          58
#define STM32MP25_RIFSC_ADC3_ID           59
#define STM32MP25_RIFSC_ETH1_ID         60
#if !defined(STM32MP251Cxx) || !defined(STM32MP251Fxx) || !defined(STM32MP251Axx) || !defined(STM32MP251Dxx)
#define STM32MP25_RIFSC_ETH2_ID           61
#endif
#define STM32MP25_RIFSC_USBH_ID           63
#define STM32MP25_RIFSC_USB3DR_ID         66
#define STM32MP25_RIFSC_COMBOPHY_ID       67
#define STM32MP25_RIFSC_PCIE_ID           68
#define STM32MP25_RIFSC_UCPD1_ID          69
#if defined(STM32MP257Axx) || defined(STM32MP257Dxx) || defined(STM32MP257Cxx) || defined(STM32MP257Fxx)
#define STM32MP25_RIFSC_ETHSW_DEIP_ID         70
#define STM32MP25_RIFSC_ETHSW_ACM_CFG_ID      71
#define STM32MP25_RIFSC_ETHSW_ACM_MSGBUF_ID   72
#endif
#define STM32MP25_RIFSC_STGEN_ID            73
#define STM32MP25_RIFSC_OCTOSPI1_ID         74
#define STM32MP25_RIFSC_OCTOSPI2_ID       75
#define STM32MP25_RIFSC_SDMMC1_ID         76
#define STM32MP25_RIFSC_SDMMC2_ID         77
#define STM32MP25_RIFSC_SDMMC3_ID         78
#if defined(STM32MP255Axx) || defined(STM32MP255Dxx) || defined(STM32MP255Cxx) || defined(STM32MP255Fxx) \
 || defined(STM32MP257Axx) || defined(STM32MP257Dxx) || defined(STM32MP257Cxx) || defined(STM32MP257Fxx)
#define STM32MP25_RIFSC_GPU_ID            79
#endif
#define STM32MP25_RIFSC_LTDC_CMN_ID       80
#if defined(STM32MP255Axx) || defined(STM32MP255Dxx) || defined(STM32MP255Cxx) || defined(STM32MP255Fxx) \
 || defined(STM32MP257Axx) || defined(STM32MP257Dxx) || defined(STM32MP257Cxx) || defined(STM32MP257Fxx)
#define STM32MP25_RIFSC_DSI_CMN_ID        81
#define STM32MP25_RIFSC_LVDS_ID           84
#endif
#define STM32MP25_RIFSC_CSI_ID            86
#define STM32MP25_RIFSC_DCMIPP_ID         87
#define STM32MP25_RIFSC_DCMI_PSSI_ID        88
#if defined(STM32MP255Axx) || defined(STM32MP255Dxx) || defined(STM32MP255Cxx) || defined(STM32MP255Fxx) \
 || defined(STM32MP257Axx) || defined(STM32MP257Dxx) || defined(STM32MP257Cxx) || defined(STM32MP257Fxx)
#define STM32MP25_RIFSC_VDEC_ID           89
#define STM32MP25_RIFSC_VENC_ID           90
#endif
#define STM32MP25_RIFSC_RNG_ID            92
#define STM32MP25_RIFSC_PKA_ID            93
#define STM32MP25_RIFSC_SAES_ID           94
#define STM32MP25_RIFSC_HASH_ID           95
#if defined(STM32MP251Cxx) || defined(STM32MP251Fxx) || defined(STM32MP253Cxx) || defined(STM32MP253Fxx) \
 || defined(STM32MP255Cxx) || defined(STM32MP255Fxx) || defined(STM32MP257Cxx) || defined(STM32MP257Fxx)
#define STM32MP25_RIFSC_CRYP1_ID          96
#define STM32MP25_RIFSC_CRYP2_ID          97
#endif
#define STM32MP25_RIFSC_IWDG1_ID          98
#define STM32MP25_RIFSC_IWDG2_ID          99
#define STM32MP25_RIFSC_IWDG3_ID          100
#define STM32MP25_RIFSC_IWDG4_ID          101
#define STM32MP25_RIFSC_IWDG5_ID          102
#define STM32MP25_RIFSC_WWDG1_ID          103
#define STM32MP25_RIFSC_WWDG2_ID          104
#define STM32MP25_RIFSC_VREFBUF_ID        106
#define STM32MP25_RIFSC_DTS_ID            107
#define STM32MP25_RIFSC_RAMCFG_ID         108
#define STM32MP25_RIFSC_CRC_ID            109
#define STM32MP25_RIFSC_SERC_ID         110
#define STM32MP25_RIFSC_OCTOSPIM_ID         111
#define STM32MP25_RIFSC_GICV2M_ID         112
#define STM32MP25_RIFSC_I3C1_ID           114
#define STM32MP25_RIFSC_I3C2_ID           115
#define STM32MP25_RIFSC_I3C3_ID           116
#define STM32MP25_RIFSC_I3C4_ID             117
#define STM32MP25_RIFSC_ICACHE_DCACHE_ID    118
#define STM32MP25_RIFSC_LTDC_L0L1_ID        119
#define STM32MP25_RIFSC_LTDC_L2_ID            120
#define STM32MP25_RIFSC_LTDC_ROT_ID           121
#if defined(STM32MP255Axx) || defined(STM32MP255Dxx) || defined(STM32MP255Cxx) || defined(STM32MP255Fxx) \
 || defined(STM32MP257Axx) || defined(STM32MP257Dxx) || defined(STM32MP257Cxx) || defined(STM32MP257Fxx)
#define STM32MP25_RIFSC_DSI_TRIG_ID           122
#define STM32MP25_RIFSC_DSI_RDFIFO_ID         123
#endif
#define STM32MP25_RIFSC_OTFDEC1_ID            125
#define STM32MP25_RIFSC_OTFDEC2_ID            126
#define STM32MP25_RIFSC_IAC_ID              127


#define RESMGR_RIFSC_TIM1_ID       STM32MP25_RIFSC_TIM1_ID
#define RESMGR_RIFSC_TIM2_ID         STM32MP25_RIFSC_TIM2_ID
#define RESMGR_RIFSC_TIM3_ID         STM32MP25_RIFSC_TIM3_ID
#define RESMGR_RIFSC_TIM4_ID         STM32MP25_RIFSC_TIM4_ID
#define RESMGR_RIFSC_TIM5_ID         STM32MP25_RIFSC_TIM5_ID
#define RESMGR_RIFSC_TIM6_ID         STM32MP25_RIFSC_TIM6_ID
#define RESMGR_RIFSC_TIM7_ID         STM32MP25_RIFSC_TIM7_ID
#define RESMGR_RIFSC_TIM8_ID         STM32MP25_RIFSC_TIM8_ID
#define RESMGR_RIFSC_TIM10_ID        STM32MP25_RIFSC_TIM10_ID
#define RESMGR_RIFSC_TIM11_ID        STM32MP25_RIFSC_TIM11_ID
#define RESMGR_RIFSC_TIM12_ID        STM32MP25_RIFSC_TIM12_ID
#define RESMGR_RIFSC_TIM13_ID      STM32MP25_RIFSC_TIM13_ID
#define RESMGR_RIFSC_TIM14_ID        STM32MP25_RIFSC_TIM14_ID
#define RESMGR_RIFSC_TIM15_ID        STM32MP25_RIFSC_TIM15_ID
#define RESMGR_RIFSC_TIM16_ID        STM32MP25_RIFSC_TIM16_ID
#define RESMGR_RIFSC_TIM17_ID          STM32MP25_RIFSC_TIM17_ID
#define RESMGR_RIFSC_TIM20_ID        STM32MP25_RIFSC_TIM20_ID
#define RESMGR_RIFSC_LPTIM1_ID       STM32MP25_RIFSC_LPTIM1_ID
#define RESMGR_RIFSC_LPTIM2_ID       STM32MP25_RIFSC_LPTIM2_ID
#define RESMGR_RIFSC_LPTIM3_ID       STM32MP25_RIFSC_LPTIM3_ID
#define RESMGR_RIFSC_LPTIM4_ID       STM32MP25_RIFSC_LPTIM4_ID
#define RESMGR_RIFSC_LPTIM5_ID       STM32MP25_RIFSC_LPTIM5_ID
#define RESMGR_RIFSC_SPI1_ID         STM32MP25_RIFSC_SPI1_ID
#define RESMGR_RIFSC_SPI2_ID         STM32MP25_RIFSC_SPI2_ID
#define RESMGR_RIFSC_SPI3_ID         STM32MP25_RIFSC_SPI3_ID
#define RESMGR_RIFSC_SPI4_ID         STM32MP25_RIFSC_SPI4_ID
#define RESMGR_RIFSC_SPI5_ID         STM32MP25_RIFSC_SPI5_ID
#define RESMGR_RIFSC_SPI6_ID         STM32MP25_RIFSC_SPI6_ID
#define RESMGR_RIFSC_SPI7_ID         STM32MP25_RIFSC_SPI7_ID
#define RESMGR_RIFSC_SPI8_ID         STM32MP25_RIFSC_SPI8_ID
#define RESMGR_RIFSC_SPDIFRX_ID      STM32MP25_RIFSC_SPDIFRX_ID
#define RESMGR_RIFSC_USART1_ID       STM32MP25_RIFSC_USART1_ID
#define RESMGR_RIFSC_USART2_ID       STM32MP25_RIFSC_USART2_ID
#define RESMGR_RIFSC_USART3_ID       STM32MP25_RIFSC_USART3_ID
#define RESMGR_RIFSC_UART4_ID        STM32MP25_RIFSC_UART4_ID
#define RESMGR_RIFSC_UART5_ID        STM32MP25_RIFSC_UART5_ID
#define RESMGR_RIFSC_USART6_ID       STM32MP25_RIFSC_USART6_ID
#define RESMGR_RIFSC_UART7_ID        STM32MP25_RIFSC_UART7_ID
#define RESMGR_RIFSC_UART8_ID        STM32MP25_RIFSC_UART8_ID
#define RESMGR_RIFSC_UART9_ID        STM32MP25_RIFSC_UART9_ID
#define RESMGR_RIFSC_LPUART1_ID      STM32MP25_RIFSC_LPUART1_ID
#define RESMGR_RIFSC_I2C1_ID         STM32MP25_RIFSC_I2C1_ID
#define RESMGR_RIFSC_I2C2_ID           STM32MP25_RIFSC_I2C2_ID
#define RESMGR_RIFSC_I2C3_ID         STM32MP25_RIFSC_I2C3_ID
#define RESMGR_RIFSC_I2C4_ID         STM32MP25_RIFSC_I2C4_ID
#define RESMGR_RIFSC_I2C5_ID         STM32MP25_RIFSC_I2C5_ID
#define RESMGR_RIFSC_I2C6_ID         STM32MP25_RIFSC_I2C6_ID
#define RESMGR_RIFSC_I2C7_ID         STM32MP25_RIFSC_I2C7_ID
#define RESMGR_RIFSC_I2C8_ID         STM32MP25_RIFSC_I2C8_ID
#define RESMGR_RIFSC_SAI1_ID         STM32MP25_RIFSC_SAI1_ID
#define RESMGR_RIFSC_SAI2_ID         STM32MP25_RIFSC_SAI2_ID
#define RESMGR_RIFSC_SAI3_ID         STM32MP25_RIFSC_SAI3_ID
#define RESMGR_RIFSC_SAI4_ID         STM32MP25_RIFSC_SAI4_ID
#define RESMGR_RIFSC_MDF1_ID         STM32MP25_RIFSC_MDF1_ID
#define RESMGR_RIFSC_ADF1_ID         STM32MP25_RIFSC_ADF1_ID
#if !defined(STM32MP251Cxx) || !defined(STM32MP251Fxx) || !defined(STM32MP251Axx) || !defined(STM32MP251Dxx)
#define RESMGR_RIFSC_FDCAN_ID        STM32MP25_RIFSC_FDCAN_ID
#endif
#define RESMGR_RIFSC_HDP_ID          STM32MP25_RIFSC_HDP_ID
#define RESMGR_RIFSC_ADC12_ID        STM32MP25_RIFSC_ADC12_ID
#define RESMGR_RIFSC_ADC3_ID         STM32MP25_RIFSC_ADC3_ID
#define RESMGR_RIFSC_tea_ID        STM32MP25_RIFSC_ETH1_ID
#if !defined(STM32MP251Cxx) || !defined(STM32MP251Fxx) || !defined(STM32MP251Axx) || !defined(STM32MP251Dxx)
#define RESMGR_RIFSC_ETH2_ID         STM32MP25_RIFSC_ETH2_ID
#endif
#define RESMGR_RIFSC_USBH_ID         STM32MP25_RIFSC_USBH_ID
#define RESMGR_RIFSC_USB3DR_ID       STM32MP25_RIFSC_USB3DR_ID
#define RESMGR_RIFSC_COMBOPHY_ID     STM32MP25_RIFSC_COMBOPHY_ID
#define RESMGR_RIFSC_PCIE_ID         STM32MP25_RIFSC_PCIE_ID
#define RESMGR_RIFSC_UCPD1_ID        STM32MP25_RIFSC_UCPD1_ID
#if defined(STM32MP257Axx) || defined(STM32MP257Dxx) || defined(STM32MP257Cxx) || defined(STM32MP257Fxx)
#define RESMGR_RIFSC_ETHSW_DEIP_ID     STM32MP25_RIFSC_ETHSW_DEIP_ID
#define RESMGR_RIFSC_ETHSW_ACM_CFG_ID    STM32MP25_RIFSC_ETHSW_ACM_CFG_ID
#define RESMGR_RIFSC_ETHSW_ACM_MSGBUF_ID STM32MP25_RIFSC_ETHSW_ACM_MSGBUF_ID
#endif
#define RESMGR_RIFSC_STGEN_ID        STM32MP25_RIFSC_STGEN_ID
#define RESMGR_RIFSC_OCTOSPI1_ID       STM32MP25_RIFSC_OCTOSPI1_ID
#define RESMGR_RIFSC_OCTOSPI2_ID       STM32MP25_RIFSC_OCTOSPI2_ID
#define RESMGR_RIFSC_SDMMC1_ID       STM32MP25_RIFSC_SDMMC1_ID
#define RESMGR_RIFSC_SDMMC2_ID       STM32MP25_RIFSC_SDMMC2_ID
#define RESMGR_RIFSC_SDMMC3_ID       STM32MP25_RIFSC_SDMMC3_ID
#if defined(STM32MP255Axx) || defined(STM32MP255Dxx) || defined(STM32MP255Cxx) || defined(STM32MP255Fxx) \
 || defined(STM32MP257Axx) || defined(STM32MP257Dxx) || defined(STM32MP257Cxx) || defined(STM32MP257Fxx)
#define RESMGR_RIFSC_GPU_ID          STM32MP25_RIFSC_GPU_ID
#endif
#define RESMGR_RIFSC_LTDC_CMN_ID       STM32MP25_RIFSC_LTDC_CMN_ID
#if defined(STM32MP255Axx) || defined(STM32MP255Dxx) || defined(STM32MP255Cxx) || defined(STM32MP255Fxx) \
 || defined(STM32MP257Axx) || defined(STM32MP257Dxx) || defined(STM32MP257Cxx) || defined(STM32MP257Fxx)
#define RESMGR_RIFSC_DSI_CMN_ID      STM32MP25_RIFSC_DSI_CMN_ID
#define RESMGR_RIFSC_LVDS_ID         STM32MP25_RIFSC_LVDS_ID
#endif
#define RESMGR_RIFSC_CSI_ID          STM32MP25_RIFSC_CSI_ID
#define RESMGR_RIFSC_DCMIPP_ID       STM32MP25_RIFSC_DCMIPP_ID
#define RESMGR_RIFSC_DCMI_PSSI_ID      STM32MP25_RIFSC_DCMI_PSSI_ID
#if defined(STM32MP255Axx) || defined(STM32MP255Dxx) || defined(STM32MP255Cxx) || defined(STM32MP255Fxx) \
 || defined(STM32MP257Axx) || defined(STM32MP257Dxx) || defined(STM32MP257Cxx) || defined(STM32MP257Fxx)
#define RESMGR_RIFSC_VDEC_ID         STM32MP25_RIFSC_VDEC_ID
#define RESMGR_RIFSC_VENC_ID         STM32MP25_RIFSC_VENC_ID
#endif
#define RESMGR_RIFSC_RNG_ID          STM32MP25_RIFSC_RNG_ID
#define RESMGR_RIFSC_PKA_ID          STM32MP25_RIFSC_PKA_ID
#define RESMGR_RIFSC_SAES_ID           STM32MP25_RIFSC_SAES_ID
#define RESMGR_RIFSC_HASH_ID         STM32MP25_RIFSC_HASH_ID
#if defined(STM32MP251Cxx) || defined(STM32MP251Fxx) || defined(STM32MP253Cxx) || defined(STM32MP253Fxx) \
 || defined(STM32MP255Cxx) || defined(STM32MP255Fxx) || defined(STM32MP257Cxx) || defined(STM32MP257Fxx)
#define RESMGR_RIFSC_CRYP1_ID        STM32MP25_RIFSC_CRYP1_ID
#define RESMGR_RIFSC_CRYP2_ID        STM32MP25_RIFSC_CRYP2_ID
#endif
#define RESMGR_RIFSC_IWDG1_ID        STM32MP25_RIFSC_IWDG1_ID
#define RESMGR_RIFSC_IWDG2_ID        STM32MP25_RIFSC_IWDG2_ID
#define RESMGR_RIFSC_IWDG3_ID        STM32MP25_RIFSC_IWDG3_ID
#define RESMGR_RIFSC_IWDG4_ID        STM32MP25_RIFSC_IWDG4_ID
#define RESMGR_RIFSC_IWDG5_ID        STM32MP25_RIFSC_IWDG5_ID
#define RESMGR_RIFSC_WWDG1_ID        STM32MP25_RIFSC_WWDG1_ID
#define RESMGR_RIFSC_WWDG2_ID        STM32MP25_RIFSC_WWDG2_ID
#define RESMGR_RIFSC_VREFBUF_ID      STM32MP25_RIFSC_VREFBUF_ID
#define RESMGR_RIFSC_DTS_ID          STM32MP25_RIFSC_DTS_ID
#define RESMGR_RIFSC_RAMCFG_ID       STM32MP25_RIFSC_RAMCFG_ID
#define RESMGR_RIFSC_CRC_ID          STM32MP25_RIFSC_CRC_ID
#define RESMGR_RIFSC_SERC_ID         STM32MP25_RIFSC_SERC_ID
#define RESMGR_RIFSC_OCTOSPIM_ID     STM32MP25_RIFSC_OCTOSPIM_ID
#define RESMGR_RIFSC_GICV2M_ID       STM32MP25_RIFSC_GICV2M_ID
#define RESMGR_RIFSC_I3C1_ID         STM32MP25_RIFSC_I3C1_ID
#define RESMGR_RIFSC_I3C2_ID         STM32MP25_RIFSC_I3C2_ID
#define RESMGR_RIFSC_I3C3_ID         STM32MP25_RIFSC_I3C3_ID
#define RESMGR_RIFSC_I3C4_ID         STM32MP25_RIFSC_I3C4_ID
#define RESMGR_RIFSC_ICACHE_DCACHE_ID    STM32MP25_RIFSC_ICACHE_DCACHE_ID
#define RESMGR_RIFSC_LTDC_L0L1_ID        STM32MP25_RIFSC_LTDC_L0L1_ID
#define RESMGR_RIFSC_LTDC_L2_ID        STM32MP25_RIFSC_LTDC_L2_ID
#define RESMGR_RIFSC_LTDC_ROT_ID       STM32MP25_RIFSC_LTDC_ROT_ID
#if defined(STM32MP255Axx) || defined(STM32MP255Dxx) || defined(STM32MP255Cxx) || defined(STM32MP255Fxx) \
 || defined(STM32MP257Axx) || defined(STM32MP257Dxx) || defined(STM32MP257Cxx) || defined(STM32MP257Fxx)
#define RESMGR_RIFSC_DSI_TRIG_ID       STM32MP25_RIFSC_DSI_TRIG_ID
#define RESMGR_RIFSC_DSI_RDFIFO_ID     STM32MP25_RIFSC_DSI_RDFIFO_ID
#endif
#define RESMGR_RIFSC_OTFDEC1_ID        STM32MP25_RIFSC_OTFDEC1_ID
#define RESMGR_RIFSC_OTFDEC2_ID        STM32MP25_RIFSC_OTFDEC2_ID
#define RESMGR_RIFSC_IAC_ID          STM32MP25_RIFSC_IAC_ID

#define RESMGR_RIFSC_RIFSC_ID_MAX   127

/*GPIO Pin Resource Selection Macro*/
#define RESMGR_GPIO_PIN_0                GPIO_PIN_0         /* Pin 0 selected    */
#define RESMGR_GPIO_PIN_1                GPIO_PIN_1         /* Pin 1 selected    */
#define RESMGR_GPIO_PIN_2                GPIO_PIN_2         /* Pin 2 selected    */
#define RESMGR_GPIO_PIN_3                GPIO_PIN_3         /* Pin 3 selected    */
#define RESMGR_GPIO_PIN_4                GPIO_PIN_4         /* Pin 4 selected    */
#define RESMGR_GPIO_PIN_5        GPIO_PIN_5         /* Pin 5 selected    */
#define RESMGR_GPIO_PIN_6                GPIO_PIN_6         /* Pin 6 selected    */
#define RESMGR_GPIO_PIN_7                GPIO_PIN_7         /* Pin 7 selected    */
#define RESMGR_GPIO_PIN_8                GPIO_PIN_8         /* Pin 8 selected    */
#define RESMGR_GPIO_PIN_9                GPIO_PIN_9         /* Pin 9 selected    */
#define RESMGR_GPIO_PIN_10               GPIO_PIN_10        /* Pin 10 selected   */
#define RESMGR_GPIO_PIN_11               GPIO_PIN_11        /* Pin 11 selected   */
#define RESMGR_GPIO_PIN_12               GPIO_PIN_12        /* Pin 12 selected   */
#define RESMGR_GPIO_PIN_13               GPIO_PIN_13        /* Pin 13 selected   */
#define RESMGR_GPIO_PIN_14               GPIO_PIN_14        /* Pin 14 selected   */
#define RESMGR_GPIO_PIN_15               GPIO_PIN_15        /* Pin 15 selected   */

#define RESMGR_GPIO_PIN_MASK             GPIO_PIN_MASK      /* PIN mask for valid assert test */

/*RTC Feature Resource Selection Macros*/
#define RESMGR_RTC_ALARMA_RSC      0
#define RESMGR_RTC_ALARMB_RSC      1
#define RESMGR_RTC_WKUPT_RSC       2
#define RESMGR_RTC_TIMESTAMP_RSC   3
#define RESMGR_RTC_CALIB_RSC       4
#define RESMGR_RTC_INIT_RSC        5


/* Internal types ------------------------------------------------------------*/


/* Exported types ------------------------------------------------------------*/


/* Private macros ------------------------------------------------------------*/
/** @defgroup RESMGR_Private_Macros RESMGR Private Macros
  * @{
  */


/* Exported functions ------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

#endif /* RES_MGR_STM32MP25XXXX_H */
