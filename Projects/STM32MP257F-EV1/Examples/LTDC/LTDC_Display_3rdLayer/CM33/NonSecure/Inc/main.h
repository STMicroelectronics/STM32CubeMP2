/**
  ******************************************************************************
  * @file    main.h
  * @author  MCD Application Team
  * @brief   Header for main.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32mp2xx_hal.h"

/* Private includes ----------------------------------------------------------*/
#include "stm32mp257f_eval_lcd.h"
#include "stm32mp2xx_it.h"
#include "stm32_lcd.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/
#define COUNTOF(__BUFFER__) (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))
/* Exported functions --------------------------------------------------------*/
void Error_Handler(void);

/* Private defines -----------------------------------------------------------*/
#define LCD_FB_START_ADDRESS      (uint32_t *) 0x81300000UL       /* LCD frame buffer located on the DDR */
#define LCD_FB_SIZE               ((uint32_t)  0x400000)          /* Allocated DDR size in Bytes (4MB)*/

#define DEFAULT_IRQ_PRIO      5U

#if defined(FULLSCREEN)
#define LCD_WIDTH    ((uint16_t)1024)          /* LCD PIXEL WIDTH            */
#define LCD_HEIGHT   ((uint16_t)600)          /* LCD PIXEL HEIGHT           */
#else
#define LCD_WIDTH    ((uint16_t)720)          /* LCD PIXEL WIDTH            */
#define LCD_HEIGHT   ((uint16_t)480)          /* LCD PIXEL HEIGHT           */
#endif


#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
