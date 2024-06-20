/**
  ******************************************************************************
  * @file    stm32mp257f_eval_lcd.h
  * @author  MCD Application Team
  * @brief   This file contains the common defines and functions prototypes for
  *          the stm32mp257f_eval_lcd.c driver.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2020-2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32MP257F_EVAL_LCD_H
#define __STM32MP257F_EVAL_LCD_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* Include LCD component Driver */
#include "stm32mp257f_eval.h"
#include "../Components/Common/lcd.h"

/** @addtogroup BSP
  * @{
  */
typedef struct
{
  unsigned char blue;
  unsigned char green;
  unsigned char red;
  unsigned char alpha;
} Pixel_t;

/** @addtogroup STM32MP257F_EV1
  * @{
  */

/** @addtogroup LCD
  * @{
  */

/** @defgroup LCD_Exported_Types LCD Exported Types
  * @{
  */
typedef struct
{
  uint32_t TextColor;
  uint32_t BackColor;
} LCD_DrawPropTypeDef;

/**
  * @}
  */

/** @defgroup LCD_Exported_Constants LCD Exported Constants
  * @{
  */
#define MAX_LAYER_NUMBER       ((uint32_t)3U)

#define LCD_LayerCfgTypeDef    LTDC_LayerCfgTypeDef

#define LTDC_ACTIVE_LAYER      ((uint32_t)2) /* Layer 3 */

#define LCD_INSTANCES_NBR          1U

#define MAX_PIXEL_SIZE             (1024 * 600)
/**
  * @brief  LCD status structure definition
  */
#define LCD_OK                 ((uint8_t)0x00U)
#define LCD_ERROR              ((uint8_t)0x01U)
#define LCD_TIMEOUT            ((uint8_t)0x02U)

/**
  * @brief  LCD color
  */
#define LCD_COLOR_BLUE          ((uint32_t)0xFF0000FFU)
#define LCD_COLOR_GREEN         ((uint32_t)0xFF00FF00U)
#define LCD_COLOR_RED           ((uint32_t)0xFFFF0000U)
#define LCD_COLOR_CYAN          ((uint32_t)0xFF00FFFFU)
#define LCD_COLOR_MAGENTA       ((uint32_t)0xFFFF00FFU)
#define LCD_COLOR_YELLOW        ((uint32_t)0xFFFFFF00U)
#define LCD_COLOR_LIGHTBLUE     ((uint32_t)0xFF8080FFU)
#define LCD_COLOR_LIGHTGREEN    ((uint32_t)0xFF80FF80U)
#define LCD_COLOR_LIGHTRED      ((uint32_t)0xFFFF8080U)
#define LCD_COLOR_LIGHTCYAN     ((uint32_t)0xFF80FFFFU)
#define LCD_COLOR_LIGHTMAGENTA  ((uint32_t)0xFFFF80FFU)
#define LCD_COLOR_LIGHTYELLOW   ((uint32_t)0xFFFFFF80U)
#define LCD_COLOR_DARKBLUE      ((uint32_t)0xFF000080U)
#define LCD_COLOR_DARKGREEN     ((uint32_t)0xFF008000U)
#define LCD_COLOR_DARKRED       ((uint32_t)0xFF800000U)
#define LCD_COLOR_DARKCYAN      ((uint32_t)0xFF008080U)
#define LCD_COLOR_DARKMAGENTA   ((uint32_t)0xFF800080U)
#define LCD_COLOR_DARKYELLOW    ((uint32_t)0xFF808000U)
#define LCD_COLOR_WHITE         ((uint32_t)0xFFFFFFFFU)
#define LCD_COLOR_LIGHTGRAY     ((uint32_t)0xFFD3D3D3U)
#define LCD_COLOR_GRAY          ((uint32_t)0xFF808080U)
#define LCD_COLOR_DARKGRAY      ((uint32_t)0xFF404040U)
#define LCD_COLOR_BLACK         ((uint32_t)0xFF000000U)
#define LCD_COLOR_BROWN         ((uint32_t)0xFFA52A2AU)
#define LCD_COLOR_ORANGE        ((uint32_t)0xFFFFA500U)
#define LCD_COLOR_TRANSPARENT   ((uint32_t)0xFF000000U)

/**
  * @brief  LCD Reload Types
  */
#define LCD_RELOAD_IMMEDIATE               ((uint32_t)LTDC_SRCR_IMR)
#define LCD_RELOAD_VERTICAL_BLANKING       ((uint32_t)LTDC_SRCR_VBR)

/**
  * @}
  */

/** @defgroup DMA2D_Input_Color_Mode DMA2D Input Color Mode
  * @{
  */
#define CM_ARGB8888        0x00000000U  /*!< ARGB8888 color mode */
#define CM_RGB888          0x00000001U  /*!< RGB888 color mode   */
#define CM_RGB565          0x00000002U  /*!< RGB565 color mode   */
#define CM_ARGB1555        0x00000003U  /*!< ARGB1555 color mode */
#define CM_ARGB4444        0x00000004U  /*!< ARGB4444 color mode */
#define CM_L8              0x00000005U  /*!< L8 color mode       */
#define CM_AL44            0x00000006U  /*!< AL44 color mode     */
#define CM_AL88            0x00000007U  /*!< AL88 color mode     */
#define CM_L4              0x00000008U  /*!< L4 color mode       */
#define CM_A8              0x00000009U  /*!< A8 color mode       */
#define CM_A4              0x0000000AU  /*!< A4 color mode       */
/**
  * @}
  */

/** @addtogroup LCD_Exported_Functions
  * @{
  */
/* Initialization APIs */
int32_t  BSP_LCD_Init(uint32_t Instance);
int32_t  BSP_LCD_DeInit(uint32_t Instance);

/* LCD specific APIs: Layer control & LCD HW reset */
int32_t     BSP_LCD_SetTransparency(uint32_t Instance, uint32_t LayerIndex, uint8_t Transparency);
int32_t     BSP_LCD_SetTransparency_NoReload(uint32_t Instance, uint32_t LayerIndex, uint8_t Transparency);
int32_t     BSP_LCD_SetLayerAddress(uint32_t Instance, uint32_t LayerIndex, uint32_t Address);
int32_t     BSP_LCD_SetLayerAddress_NoReload(uint32_t Instance, uint32_t LayerIndex, uint32_t Address);
int32_t     BSP_LCD_SetColorKeying(uint32_t Instance, uint32_t LayerIndex, uint32_t RGBValue);
int32_t     BSP_LCD_SetColorKeying_NoReload(uint32_t Instance, uint32_t LayerIndex, uint32_t RGBValue);
int32_t     BSP_LCD_ResetColorKeying(uint32_t Instance, uint32_t LayerIndex);
int32_t     BSP_LCD_ResetColorKeying_NoReload(uint32_t Instance, uint32_t LayerIndex);
int32_t     BSP_LCD_SetLayerWindow(uint32_t Instance, uint16_t LayerIndex, uint16_t Xpos, uint16_t Ypos,
                                   uint16_t Width, uint16_t Height);
int32_t     BSP_LCD_SetLayerWindow_NoReload(uint32_t Instance, uint16_t LayerIndex, uint16_t Xpos, uint16_t Ypos,
                                            uint16_t Width, uint16_t Height);
int32_t     BSP_LCD_SelectLayer(uint32_t Instance, uint32_t LayerIndex);
int32_t     BSP_LCD_GetPixelFormat(uint32_t Instance, uint32_t *PixelFormat);
int32_t     BSP_LCD_GetLTDCPixelFormat(uint32_t Instance, uint32_t PixelFormat, uint32_t *LTDCPixelFormat);
int32_t     BSP_LCD_GetLCDPixelFormat(uint32_t Instance, uint32_t PixelFormat, uint32_t *LCDPixelFormat);
int32_t     BSP_LCD_SetLayerVisible(uint32_t Instance, uint32_t LayerIndex, FunctionalState State);
int32_t     BSP_LCD_SetLayerVisible_NoReload(uint32_t Instance, uint32_t LayerIndex, FunctionalState State);
int32_t     BSP_LCD_Reload(uint32_t Instance, uint32_t ReloadType);

/* LCD generic APIs: Display control */
int32_t     BSP_LCD_GetXSize(uint32_t Instance, uint32_t *XSize);
int32_t     BSP_LCD_GetYSize(uint32_t Instance, uint32_t *YSize);
int32_t     BSP_LCD_SetXSize(uint32_t Instance, uint32_t imageWidthPixels);
int32_t     BSP_LCD_SetYSize(uint32_t Instance, uint32_t imageHeightPixels);

/* LCD generic APIs: Draw operations. This list of APIs is required for
   lcd gfx utilities */
int32_t    BSP_LCD_SetTextColor(uint32_t Instance, uint32_t Color);
int32_t    BSP_LCD_GetTextColor(uint32_t Instance);
int32_t    BSP_LCD_SetBackColor(uint32_t Instance, uint32_t Color);
int32_t    BSP_LCD_GetBackColor(uint32_t Instance);


extern LTDC_HandleTypeDef  hLtdcHandler;
extern const LCD_UTILS_Drv_t LCD_Driver;

/* These functions can be modified in case the current settings
   need to be changed for specific application needs */
int32_t BSP_LCD_ReadPixel(uint32_t Instance, uint32_t Xpos, uint32_t Ypos, uint32_t *Color);
int32_t BSP_LCD_WritePixel(uint32_t Instance, uint32_t Xpos, uint32_t Ypos, uint32_t RGB_Code);
int32_t BSP_LCD_FillRGBRect(uint32_t Instance, uint32_t Xpos, uint32_t Ypos, uint8_t *pData, uint32_t Width,
                            uint32_t Height);
int32_t BSP_LCD_DrawHLine(uint32_t Instance, uint32_t Xpos, uint32_t Ypos, uint32_t Length, uint32_t Color);
int32_t BSP_LCD_DrawVLine(uint32_t Instance, uint32_t Xpos, uint32_t Ypos, uint32_t Length, uint32_t Color);
int32_t BSP_LCD_FillRect(uint32_t Instance, uint32_t Xpos, uint32_t Ypos, uint32_t Width, uint32_t Height,
                         uint32_t Color);
int32_t BSP_LCD_DrawBitmap(uint32_t Instance, uint32_t Xpos, uint32_t Ypos, uint8_t *pBmp);
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

#ifdef __cplusplus
}
#endif

#endif /* __STM32MP257F_EVAL_LCD_H */
