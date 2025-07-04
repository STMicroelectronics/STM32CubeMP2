/**
  ******************************************************************************
  * @file    stm32mp215f_disco_conf_template.h
  * @author  MCD Application Team
  * @brief   STM32MP215F_DK board configuration file.
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
#ifndef STM32MP215F_DK_CONF_H
#define STM32MP215F_DK_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32mp2xx_hal.h"

/* COM define */
#define USE_COM_LOG                         0U
#define USE_BSP_COM_FEATURE                 0U

/* LCD controllers defines */
#define USE_LCD_CTRL_OTM8009A               0U
#define USE_LCD_CTRL_ADV7533                0U

#define LCD_LAYER_0_ADDRESS                 0xC0000000U
#define LCD_LAYER_1_ADDRESS                 0xC0200000U

#define USE_DMA2D_TO_FILL_RGB_RECT          0U
/* Camera sensors defines */
#define USE_CAMERA_SENSOR_OV5640            0U
#define USE_CAMERA_SENSOR_OV9655            0U

/* SD interface defines */
#define USE_SD_BUS_WIDE_4B                  0U

/* Audio codecs defines */
#define USE_AUDIO_CODEC_WM8994              0U

/* TS supported features defines */
#define USE_TS_GESTURE                      0U
#define USE_TS_MULTI_TOUCH                  0U

/* Default TS touch number */
#define TS_TOUCH_NBR                        2U

/* IRQ priorities */
#define BSP_BUTTON_USER_IT_PRIORITY         15U
#define BSP_BUTTON_TAMPER_IT_PRIORITY       15U
#define BSP_SDRAM_IT_PRIORITY               15U
#define BSP_CAMERA_IT_PRIORITY              15U
#define BSP_BUTTON_WAKEUP_IT_PRIORITY       15U
#define BSP_AUDIO_OUT_IT_PRIORITY           14U
#define BSP_AUDIO_IN_IT_PRIORITY            15U
#define BSP_SD_IT_PRIORITY                  14U
#define BSP_SD_RX_IT_PRIORITY               14U
#define BSP_SD_TX_IT_PRIORITY               15U
#define BSP_TS_IT_PRIORITY                  15U

#ifdef __cplusplus
}
#endif

#endif /* STM32MP215F_DK_CONF_H */
