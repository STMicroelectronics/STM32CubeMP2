/**
  ******************************************************************************
  * @file    stm32mp257f_eval_conf.h
  * @author  MCD Application Team
  * @brief   STM32MP257F-EVAL board configuration file.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STM32MP257F_EVAL_CONF_H
#define STM32MP257F_EVAL_CONF_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32mp2xx_hal.h"

/* COM usage define */
#define USE_COM_LOG                         1U
#define USE_BSP_COM_FEATURE                 1U

/* LCD controllers defines */
#define LCD_LAYER_0_ADDRESS                 0xD0000000U
#define LCD_LAYER_1_ADDRESS                 0xD0200000U

/* Audio codecs defines */
#define USE_AUDIO_CODEC_WM8994              1U
#define DEFAULT_AUDIO_IN_BUFFER_SIZE        2048U
#define USE_BSP_CPU_CACHE_MAINTENANCE       1U

/* TS supported features defines */
#define TS_TOUCH_NBR                        1U
#define USE_TS3510_TS_CTRL                  1U
#define USE_EXC7200_TS_CTRL                 1U

/* Default EEPROM max trials */
#define EEPROM_MAX_TRIALS                   3000U

/* IRQ priorities */
#define BSP_SRAM_IT_PRIORITY                15U
#define BSP_SDRAM_IT_PRIORITY               15U
#define BSP_IOEXPANDER_IT_PRIORITY          15U
#define BSP_BUTTON_USER_IT_PRIORITY         15U
#define BSP_BUTTON_WAKEUP_IT_PRIORITY       15U
#define BSP_BUTTON_TAMPER_IT_PRIORITY       15U
#define BSP_AUDIO_OUT_IT_PRIORITY           14U
#define BSP_AUDIO_IN_IT_PRIORITY            15U
#define BSP_SD_IT_PRIORITY                  14U
#define BSP_SD_RX_IT_PRIORITY               14U
#define BSP_SD_TX_IT_PRIORITY               15U
#define BSP_TS_IT_PRIORITY                  15U

#ifdef __cplusplus
}
#endif

#endif /* STM32MP257F_EVAL_CONF_H */
