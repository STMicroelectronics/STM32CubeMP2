/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usbpd_devices_conf.h
  * @author  MCD Application Team
  * @brief   This file contains the device define.
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
/* USER CODE END Header */

#ifndef USBPD_DEVICE_CONF_H
#define USBPD_DEVICE_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32mp2xx_hal.h"
#include "stm32mp2xx_ll_bus.h"
#include "stm32mp2xx_ll_dma.h"
#include "stm32mp2xx_ll_gpio.h"
#include "stm32mp2xx_ll_rcc.h"
#include "stm32mp2xx_ll_ucpd.h"
#include "stm32mp2xx_ll_pwr.h"
#include "stm32mp2xx_ll_tim.h"
#include "usbpd_pwr_user.h"
#include "usbpd_pwr_if.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void ResMgr_LL_RCC_HPDMA3_EnableClock();
void ResMgr_LL_RCC_GPIOG_EnableClock();
/* Private functions ---------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* -----------------------------------------------------------------------------
      usbpd_hw.c
-------------------------------------------------------------------------------*/

/* defined used to configure function : USBPD_HW_GetUSPDInstance */
#define UCPD_INSTANCE0 UCPD1

/* defined used to configure function : USBPD_HW_Init_DMARxInstance,USBPD_HW_DeInit_DMARxInstance */
#define UCPDDMA_INSTANCE0_CLOCKENABLE_RX    do {                                     \
                                                 ResMgr_LL_RCC_HPDMA3_EnableClock();      \
                                               } while(0)
#define UCPDDMA_INSTANCE0_DMA_RX            HPDMA3

#define UCPDDMA_INSTANCE0_REQUEST_RX        LL_HPDMA_REQUEST_UCPD_RX

#define UCPDDMA_INSTANCE0_LL_CHANNEL_RX     LL_DMA_CHANNEL_3

#define UCPDDMA_INSTANCE0_CHANNEL_RX        HPDMA3_Channel3

/* defined used to configure function : USBPD_HW_Init_DMATxInstance, USBPD_HW_DeInit_DMATxInstance */
#define UCPDDMA_INSTANCE0_CLOCKENABLE_TX    do {                                     \
                                                 ResMgr_LL_RCC_HPDMA3_EnableClock();      \
                                               } while(0)
#define UCPDDMA_INSTANCE0_DMA_TX            HPDMA3

#define UCPDDMA_INSTANCE0_REQUEST_TX        LL_HPDMA_REQUEST_UCPD_TX

#define UCPDDMA_INSTANCE0_LL_CHANNEL_TX     LL_DMA_CHANNEL_2

#define UCPDDMA_INSTANCE0_CHANNEL_TX        HPDMA3_Channel2

/* defined used to configure  USBPD_HW_SetFRSSignalling */
#define UCPDFRS_INSTANCE0_FRSCC1
#define UCPDFRS_INSTANCE0_FRSCC2

#define UCPD_INSTANCE0_ENABLEIRQ       do {                                                                                     \
                                            NVIC_SetPriority(UCPD1_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),5, 0)); \
                                            NVIC_EnableIRQ(UCPD1_IRQn);                                                         \
                                          } while(0)

/* -----------------------------------------------------------------------------
      Definitions for timer service feature
-------------------------------------------------------------------------------*/

#define TIMX                           TIM2

#define TIMX_CLK_ENABLE                do {                                                        \
                                            __IO uint32_t tmpreg;                                  \
                                            SET_BIT(RCC->TIM2CFGR, RCC_TIM2CFGR_TIM2EN);           \
                                            /* Delay after an RCC peripheral clock enabling */     \
                                            tmpreg = READ_BIT(RCC->TIM2CFGR, RCC_TIM2CFGR_TIM2EN); \
                                            tmpreg = READ_BIT(RCC->TIM2CFGR, RCC_TIM2CFGR_TIM2EN); \
                                            UNUSED(tmpreg);                                        \
                                          } while(0)
#define TIMX_CLK_DISABLE               LL_RCC_TIM2_DisableClock()
#define TIMX_CLK_FREQ                  HAL_RCCEx_GetTimerCLKFreq(RCC_PERIPHCLK_TIM2)
#define TIMX_IRQ                       TIM2_UP_IRQn
#define TIMX_CHANNEL_CH1               LL_TIM_CHANNEL_CH1
#define TIMX_CHANNEL_CH2               LL_TIM_CHANNEL_CH2
#define TIMX_CHANNEL_CH3               LL_TIM_CHANNEL_CH3
#define TIMX_CHANNEL_CH4               LL_TIM_CHANNEL_CH4
#define TIMX_CHANNEL1_SETEVENT         do {                                                                    \
                                            LL_TIM_OC_SetCompareCH1(TIMX, (TimeUs + TIMX->CNT) % TIM_MAX_TIME);\
                                            LL_TIM_ClearFlag_CC1(TIMX);                                        \
                                         } while(0)
#define TIMX_CHANNEL2_SETEVENT         do {                                                                    \
                                            LL_TIM_OC_SetCompareCH2(TIMX, (TimeUs + TIMX->CNT) % TIM_MAX_TIME);\
                                            LL_TIM_ClearFlag_CC2(TIMX);                                        \
                                         } while(0)
#define TIMX_CHANNEL3_SETEVENT         do {                                                                    \
                                            LL_TIM_OC_SetCompareCH3(TIMX, (TimeUs + TIMX->CNT) % TIM_MAX_TIME);\
                                            LL_TIM_ClearFlag_CC3(TIMX);                                        \
                                         } while(0)
#define TIMX_CHANNEL4_SETEVENT         do {                                                                    \
                                            LL_TIM_OC_SetCompareCH4(TIMX, (TimeUs + TIMX->CNT) % TIM_MAX_TIME);\
                                            LL_TIM_ClearFlag_CC4(TIMX);                                        \
                                         } while(0)
#define TIMX_CHANNEL1_GETFLAG          LL_TIM_IsActiveFlag_CC1
#define TIMX_CHANNEL2_GETFLAG          LL_TIM_IsActiveFlag_CC2
#define TIMX_CHANNEL3_GETFLAG          LL_TIM_IsActiveFlag_CC3
#define TIMX_CHANNEL4_GETFLAG          LL_TIM_IsActiveFlag_CC4

#ifdef __cplusplus
}
#endif

#endif /* USBPD_DEVICE_CONF_H */
