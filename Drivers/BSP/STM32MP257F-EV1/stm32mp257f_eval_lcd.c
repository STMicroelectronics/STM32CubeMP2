/**
  ******************************************************************************
  * @file    stm32mp257f_eval_lcd.c
  * @author  MCD Application Team
  * @brief   This file includes the driver for Liquid Crystal Display (LCD) module
  *          mounted on STM32MP257F-EV board.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023-2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  @verbatim
  1. How To use this driver:
  --------------------------
     - This driver provides basic UI API's to draw Rectangle , Bitmap , Draw ASCII character, Horizontal line
        Vertical Line along with configuration functions.

  2. Driver description:
  ---------------------
    + Initialization steps:
       o Set Active Layer by BSP_LCD_SelectLayer(), LTDC controller support upto 3 layer
       o Synchronize the Pixel Format between utility and driver using  BSP_LCD_GetLTDCPixelFormat()

    + Options
       o Configure and enable the color keying functionality using the
         BSP_LCD_SetColorKeying() function.
       o Modify in the fly the transparency and/or the frame buffer address
         using the following functions:
         - BSP_LCD_SetTransparency()
         - BSP_LCD_SetLayerAddress()
  @endverbatim
  */

/* Dependencies
- stm32mp257f_eval.c
- Components/Common/lcd.c
- fonts.h
- font24.c
- font20.c
- font16.c
- font12.c
- font8.c"
EndDependencies */

/* Includes ------------------------------------------------------------------*/
#include "stm32mp257f_eval_lcd.h"


/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32MP257F_EV1
  * @{
  */

/** @addtogroup LCD
  * @{
  */

/** @defgroup LCD_Private_TypesDefinitions STM32MP257F_EV1 LCD Private Types Definitions
  * @{
  */
const LCD_UTILS_Drv_t LCD_Driver =
{
  BSP_LCD_DrawBitmap,
  BSP_LCD_FillRGBRect,
  BSP_LCD_DrawHLine,
  BSP_LCD_DrawVLine,
  BSP_LCD_FillRect,
  BSP_LCD_ReadPixel,
  BSP_LCD_WritePixel,
  BSP_LCD_GetXSize,
  BSP_LCD_GetYSize,
  BSP_LCD_SelectLayer,
  BSP_LCD_GetPixelFormat

};
/**
  * @}
  */
/** @defgroup LCD_Private_Defines LCD Private Defines
  * @{
  */
#define POLY_X(Z)              ((int32_t)((Points + Z)->X))
#define POLY_Y(Z)              ((int32_t)((Points + Z)->Y))
#define PIXEL_PER_LINE         (1024*2)
#define ALPHA_VALUE             255
/**
  * @}
  */

/** @defgroup LCD_Private_Macros LCD Private Macros
  * @{
  */
#define ABS(X)  ((X) > 0 ? (X) : -(X))
/**
  * @}
  */

/** @defgroup LCD_Private_Variables LCD Private Variables
  * @{
  */
LTDC_HandleTypeDef  hLtdcHandler;

/* Default LCD configuration with LCD Layer 1 */
static uint32_t            ActiveLayer = 0;
static LCD_DrawPropTypeDef DrawProp[MAX_LAYER_NUMBER];
/**
  * @}
  */

/** @defgroup LCD_Private_FunctionPrototypes LCD Private Function Prototypes
  * @{
  */

/**
  * @}
  */

/** @defgroup LCD_Exported_Functions LCD Exported Functions
  * @{
  */

/**
  * @brief  Gets the LCD X size.
  * @param  Instance LCD Instance
  * @retval XSize pointer Used LCD X size
  */
int32_t BSP_LCD_GetXSize(uint32_t Instance, uint32_t *XSize)
{

  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= LCD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    *XSize = hLtdcHandler.LayerCfg[ActiveLayer].ImageWidth;
  }

  return ret;
}

/**
  * @brief  Gets the LCD Y size.
  * @param  Instance LCD Instance
  * @retval YSize pointer Used LCD Y size
  */
int32_t BSP_LCD_GetYSize(uint32_t Instance, uint32_t *YSize)
{

  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= LCD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    *YSize = hLtdcHandler.LayerCfg[ActiveLayer].ImageHeight;
  }

  return ret;
}

/**
  * @brief  Set the LCD X size.
  * @param  Instance    LCD Instance
  * @param  imageWidthPixels : image width in pixels unit
  * @retval None
  */
int32_t BSP_LCD_SetXSize(uint32_t Instance, uint32_t imageWidthPixels)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= LCD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    hLtdcHandler.LayerCfg[ActiveLayer].ImageWidth = imageWidthPixels;
  }
  return ret;
}

/**
  * @brief  Set the LCD Y size.
  * @param  Instance    LCD Instance
  * @param  imageHeightPixels : image height in lines unit
  * @retval None
  */
int32_t BSP_LCD_SetYSize(uint32_t Instance, uint32_t imageHeightPixels)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= LCD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    hLtdcHandler.LayerCfg[ActiveLayer].ImageHeight = imageHeightPixels;
  }
  return ret;

}


/**
  * @brief  Selects the LCD Layer.
  * @param  Instance    LCD Instance
  * @param  LayerIndex: Layer foreground or background
  * @retval None
  */
int32_t BSP_LCD_SelectLayer(uint32_t Instance, uint32_t LayerIndex)
{
  uint32_t ret = BSP_ERROR_NONE;

  if (Instance >= LCD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    ActiveLayer = LayerIndex;
  }
  return ret;
}

/**
  * @brief  Sets an LCD Layer visible
  * @param  Instance    LCD Instance
  * @param  LayerIndex: Visible Layer
  * @param  State: New state of the specified layer
  *          This parameter can be one of the following values:
  *            @arg  ENABLE
  *            @arg  DISABLE
  * @retval None
  */
int32_t BSP_LCD_SetLayerVisible(uint32_t Instance, uint32_t LayerIndex, FunctionalState State)
{
  uint32_t ret = BSP_ERROR_NONE;

  if (Instance >= LCD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if (State == ENABLE)
    {
      __HAL_LTDC_LAYER_ENABLE(&hLtdcHandler, LayerIndex);
    }
    else
    {
      __HAL_LTDC_LAYER_DISABLE(&hLtdcHandler, LayerIndex);
    }
    __HAL_LTDC_RELOAD_IMMEDIATE_CONFIG(&hLtdcHandler);
  }
  return ret;
}

/**
  * @brief  Sets an LCD Layer visible without reloading.
  * @param  Instance    LCD Instance
  * @param  LayerIndex: Visible Layer
  * @param  State: New state of the specified layer
  *          This parameter can be one of the following values:
  *            @arg  ENABLE
  *            @arg  DISABLE
  * @retval None
  */
int32_t BSP_LCD_SetLayerVisible_NoReload(uint32_t Instance, uint32_t LayerIndex, FunctionalState State)
{
  uint32_t ret = BSP_ERROR_NONE;

  if (Instance >= LCD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if (State == ENABLE)
    {
      __HAL_LTDC_LAYER_ENABLE(&hLtdcHandler, LayerIndex);
    }
    else
    {
      __HAL_LTDC_LAYER_DISABLE(&hLtdcHandler, LayerIndex);
    }
  }
  return ret;
  /* Do not Sets the Reload  */
}

/**
  * @brief  Gets the LCD Active LCD Pixel Format.
  * @param  Instance    LCD Instance
  * @param  PixelFormat Active LCD Pixel Format
  * @retval BSP status
  */
int32_t BSP_LCD_GetPixelFormat(uint32_t Instance, uint32_t *PixelFormat)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= LCD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    uint32_t LCDPixelFormat;
    BSP_LCD_GetLCDPixelFormat(Instance, hLtdcHandler.LayerCfg[ActiveLayer].PixelFormat, &LCDPixelFormat);
    *PixelFormat = LCDPixelFormat;
  }

  return ret;
}
/**
  * @brief  Gets the LCD Active LTDC Pixel Format.
  * @param  Instance    LCD Instance
  * @param  PixelFormat Active LCD Pixel Format
  * @param  LTDCPixelFormat Pointer to Active LTDC Pixel Format
  * @retval BSP status
  */
int32_t BSP_LCD_GetLTDCPixelFormat(uint32_t Instance, uint32_t PixelFormat, uint32_t *LTDCPixelFormat)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= LCD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {

    switch (PixelFormat)
    {
      case LCD_PIXEL_FORMAT_RGB565:
        *LTDCPixelFormat = LTDC_PIXEL_FORMAT_RGB565;
        break;
      case LCD_PIXEL_FORMAT_ARGB8888:
        *LTDCPixelFormat = LTDC_PIXEL_FORMAT_ARGB8888;
        break;
      default:
        *LTDCPixelFormat = LTDC_PIXEL_FORMAT_RGB565;
        break;
    }
  }

  return ret;
}

/**
  * @brief  Gets the LCD Active LCD Pixel Format.
  * @param  Instance    LCD Instance
  * @param  PixelFormat Active LTDC Pixel Format
  * @param  LCDPixelFormat Pointer to Active LCD Pixel Format
  * @retval BSP status
  */
int32_t BSP_LCD_GetLCDPixelFormat(uint32_t Instance, uint32_t PixelFormat, uint32_t *LCDPixelFormat)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= LCD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {

    switch (PixelFormat)
    {
      case LTDC_PIXEL_FORMAT_RGB565:
        *LCDPixelFormat = LCD_PIXEL_FORMAT_RGB565;
        break;
      case LTDC_PIXEL_FORMAT_ARGB8888:
        *LCDPixelFormat = LCD_PIXEL_FORMAT_ARGB8888;
        break;
      default:
        *LCDPixelFormat = LCD_PIXEL_FORMAT_RGB565;
        break;
    }
  }

  return ret;
}
/**
  * @brief  Configures the transparency.
  * @param  Instance    LCD Instance
  * @param  LayerIndex: Layer foreground or background.
  * @param  Transparency: Transparency
  *           This parameter must be a number between Min_Data = 0x00 and Max_Data = 0xFF
  * @retval None
  */
int32_t BSP_LCD_SetTransparency(uint32_t Instance, uint32_t LayerIndex, uint8_t Transparency)
{
  uint32_t ret = BSP_ERROR_NONE;

  if (Instance >= LCD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if (HAL_LTDC_SetAlpha(&hLtdcHandler, Transparency, LayerIndex) != HAL_OK)
    {
      return LCD_ERROR;
    }
  }
  return ret;
}

/**
  * @brief  Configures the transparency without reloading.
  * @param  Instance    LCD Instance
  * @param  LayerIndex: Layer foreground or background.
  * @param  Transparency: Transparency
  *           This parameter must be a number between Min_Data = 0x00 and Max_Data = 0xFF
  * @retval None
  */
int32_t BSP_LCD_SetTransparency_NoReload(uint32_t Instance, uint32_t LayerIndex, uint8_t Transparency)
{
  uint32_t ret = BSP_ERROR_NONE;

  if (Instance >= LCD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if (HAL_LTDC_SetAlpha_NoReload(&hLtdcHandler, Transparency, LayerIndex) != HAL_OK)
    {
      return LCD_ERROR;
    }
  }
  return ret;
}

/**
  * @brief  Sets an LCD layer frame buffer address.
  * @param  Instance    LCD Instance
  * @param  LayerIndex: Layer foreground or background
  * @param  Address: New LCD frame buffer value
  * @retval None
  */
int32_t BSP_LCD_SetLayerAddress(uint32_t Instance, uint32_t LayerIndex, uint32_t Address)
{
  uint32_t ret = BSP_ERROR_NONE;

  if (Instance >= LCD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if (HAL_LTDC_SetAddress(&hLtdcHandler, Address, LayerIndex) != HAL_OK)
    {
      return LCD_ERROR;
    }
  }
  return ret;
}

/**
  * @brief  Sets an LCD layer frame buffer address without reloading.
  * @param  Instance    LCD Instance
  * @param  LayerIndex: Layer foreground or background
  * @param  Address: New LCD frame buffer value
  * @retval None
  */
int32_t BSP_LCD_SetLayerAddress_NoReload(uint32_t Instance, uint32_t LayerIndex, uint32_t Address)
{
  uint32_t ret = BSP_ERROR_NONE;

  if (Instance >= LCD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if (HAL_LTDC_SetAddress_NoReload(&hLtdcHandler, Address, LayerIndex) != HAL_OK)
    {
      return LCD_ERROR;
    }
  }
  return ret;

}

/**
  * @brief  Sets display window.
  * @param  Instance    LCD Instance
  * @param  LayerIndex: Layer index
  * @param  Xpos: LCD X position
  * @param  Ypos: LCD Y position
  * @param  Width: LCD window width
  * @param  Height: LCD window height
  * @retval None
  */
int32_t BSP_LCD_SetLayerWindow(uint32_t Instance, uint16_t LayerIndex, uint16_t Xpos, uint16_t Ypos, uint16_t Width,
                               uint16_t Height)
{
  uint32_t ret = BSP_ERROR_NONE;

  if (Instance >= LCD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Reconfigure the layer size */
    if (HAL_LTDC_SetWindowSize(&hLtdcHandler, Width, Height, LayerIndex) != HAL_OK)
    {
      return LCD_ERROR;
    }

    /* Reconfigure the layer position */
    if (HAL_LTDC_SetWindowPosition(&hLtdcHandler, Xpos, Ypos, LayerIndex) != HAL_OK)
    {
      return LCD_ERROR;
    }
  }
  return ret;
}

/**
  * @brief  Sets display window without reloading.
  * @param  Instance    LCD Instance
  * @param  LayerIndex: Layer index
  * @param  Xpos: LCD X position
  * @param  Ypos: LCD Y position
  * @param  Width: LCD window width
  * @param  Height: LCD window height
  * @retval None
  */
int32_t BSP_LCD_SetLayerWindow_NoReload(uint32_t Instance, uint16_t LayerIndex, uint16_t Xpos, uint16_t Ypos,
                                        uint16_t Width, uint16_t Height)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= LCD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Reconfigure the layer size */
    if (HAL_LTDC_SetWindowSize_NoReload(&hLtdcHandler, Width, Height, LayerIndex) != HAL_OK)
    {
      return LCD_ERROR;
    }

    /* Reconfigure the layer position */
    if (HAL_LTDC_SetWindowPosition_NoReload(&hLtdcHandler, Xpos, Ypos, LayerIndex) != HAL_OK)
    {
      return LCD_ERROR;
    }
  }
  return ret;
}

/**
  * @brief  Configures and sets the color keying.
  * @param  Instance    LCD Instance
  * @param  LayerIndex: Layer foreground or background
  * @param  RGBValue: Color reference
  * @retval None
  */
int32_t BSP_LCD_SetColorKeying(uint32_t Instance, uint32_t LayerIndex, uint32_t RGBValue)
{

  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= LCD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Configure and Enable the color Keying for LCD Layer */
    if (HAL_LTDC_ConfigColorKeying(&hLtdcHandler, RGBValue, LayerIndex) != HAL_OK)
    {
      return LCD_ERROR;
    }

    if (HAL_LTDC_EnableColorKeying(&hLtdcHandler, LayerIndex) != HAL_OK)
    {
      return LCD_ERROR;
    }
  }
  return ret;
}

/**
  * @brief  Configures and sets the color keying without reloading.
  * @param  Instance    LCD Instance
  * @param  LayerIndex: Layer foreground or background
  * @param  RGBValue: Color reference
  * @retval None
  */
int32_t BSP_LCD_SetColorKeying_NoReload(uint32_t Instance, uint32_t LayerIndex, uint32_t RGBValue)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= LCD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Configure and Enable the color Keying for LCD Layer */
    if (HAL_LTDC_ConfigColorKeying_NoReload(&hLtdcHandler, RGBValue, LayerIndex) != HAL_OK)
    {
      return LCD_ERROR;
    }

    if (HAL_LTDC_EnableColorKeying_NoReload(&hLtdcHandler, LayerIndex) != HAL_OK)
    {
      return LCD_ERROR;
    }
  }
  return ret;
}

/**
  * @brief  Disables the color keying.
  * @param  Instance    LCD Instance
  * @param  LayerIndex: Layer foreground or background
  * @retval None
  */
int32_t BSP_LCD_ResetColorKeying(uint32_t Instance, uint32_t LayerIndex)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= LCD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Disable the color Keying for LCD Layer */
    if (HAL_LTDC_DisableColorKeying(&hLtdcHandler, LayerIndex) != HAL_OK)
    {
      return LCD_ERROR;
    }
  }
  return ret;
}

/**
  * @brief  Disables the color keying without reloading.
  * @param  Instance    LCD Instance
  * @param  LayerIndex: Layer foreground or background
  * @retval None
  */
int32_t BSP_LCD_ResetColorKeying_NoReload(uint32_t Instance, uint32_t LayerIndex)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= LCD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Disable the color Keying for LCD Layer */
    if (HAL_LTDC_DisableColorKeying_NoReload(&hLtdcHandler, LayerIndex) != HAL_OK)
    {
      return LCD_ERROR;
    }
  }
  return ret;
}

/**
  * @brief  Disables the color keying without reloading.
  * @param  Instance    LCD Instance
  * @param  ReloadType: can be one of the following values
  *         - LCD_RELOAD_IMMEDIATE
  *         - LCD_RELOAD_VERTICAL_BLANKING
  * @retval None
  */
int32_t BSP_LCD_Reload(uint32_t Instance, uint32_t ReloadType)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= LCD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if (HAL_LTDC_Reload(&hLtdcHandler, ReloadType) != HAL_OK)
    {
      return LCD_ERROR;
    }
  }
  return ret;
}

/**
  * @brief  Sets the LCD text color.
  * @param  Instance    LCD Instance
  * @param  Color: Text color code ARGB(8-8-8-8)
  * @retval None
  */
int32_t BSP_LCD_SetTextColor(uint32_t Instance, uint32_t Color)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= LCD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    DrawProp[ActiveLayer].TextColor = Color;
  }
  return ret;
}

/**
  * @brief  Gets the LCD text color.
  * @param  Instance    LCD Instance
  * @retval Used text color.
  */
int32_t BSP_LCD_GetTextColor(uint32_t Instance)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= LCD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    return DrawProp[ActiveLayer].TextColor;
  }
  return ret;
}

/**
  * @brief  Sets the LCD background color.
  * @param  Instance    LCD Instance
  * @param  Color: Layer background color code ARGB(8-8-8-8)
  * @retval None
  */
int32_t BSP_LCD_SetBackColor(uint32_t Instance, uint32_t Color)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= LCD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    DrawProp[ActiveLayer].BackColor = Color;
  }
  return ret;
}

/**
  * @brief  Gets the LCD background color.
  * @param  Instance    LCD Instance
  * @retval Used background colour
  */
int32_t BSP_LCD_GetBackColor(uint32_t Instance)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= LCD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    return DrawProp[ActiveLayer].BackColor;
  }
  return ret;
}


/**
  * @brief  Draw a horizontal line on LCD.
  * @param  Instance LCD Instance.
  * @param  Xpos X position.
  * @param  Ypos Y position.
  * @param  pData Pointer to RGB line data
  * @param  Width Rectangle width.
  * @param  Height Rectangle Height.
  * @retval BSP status.
  */
int32_t BSP_LCD_FillRGBRect(uint32_t Instance, uint32_t Xpos, uint32_t Ypos, uint8_t *pData, uint32_t Width,
                            uint32_t Height)
{
  int32_t ret = BSP_ERROR_NONE;
  if (Instance >= LCD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    uint32_t Xaddress;
    uint32_t StartAddress;
    uint32_t i;
    uint32_t j;
    uint32_t LCDPixelFormat;

    BSP_LCD_GetPixelFormat(Instance, &LCDPixelFormat);

    if (LCDPixelFormat == LCD_PIXEL_FORMAT_RGB565)
    {
      /* Set the start address */
      StartAddress = (hLtdcHandler.LayerCfg[ActiveLayer].FBStartAdress);
      StartAddress = StartAddress + (2 * (Ypos * (hLtdcHandler.LayerCfg[ActiveLayer].ImageWidth) + Xpos));

      /* Fill the rectangle */
      for (i = 0; i < Height; i++)
      {
        Xaddress =  StartAddress + (PIXEL_PER_LINE * i);
        for (j = 0; j < Width; j++)
        {
          *(__IO uint16_t *)(Xaddress)  = *(uint16_t *)(pData + (2U * j));
          Xaddress += 2U;
        }
      }
    }
    else if (LCDPixelFormat == LCD_PIXEL_FORMAT_ARGB8888)
    {
      /* Set the start address */
      StartAddress = (hLtdcHandler.LayerCfg[ActiveLayer].FBStartAdress);
      StartAddress = StartAddress + (4 * (Ypos * (hLtdcHandler.LayerCfg[ActiveLayer].ImageWidth) + Xpos));


      /* Fill the rectangle */
      for (i = 0; i < Height; i++)
      {
        Xaddress =  StartAddress + (2 * PIXEL_PER_LINE * i);
        for (j = 0; j < Width; j++)
        {
          *(__IO uint32_t *)(Xaddress)  = *(uint32_t *)(pData + (4U * j));
          Xaddress += 4U;
        }
      }
    }

  }
  return ret;
}

/**
  * @brief  Draws a full rectangle in currently active layer.
  * @param  Instance   LCD Instance
  * @param  Xpos X position
  * @param  Ypos Y position
  * @param  Width Rectangle width
  * @param  Height Rectangle height
  * @param  Color RGB color
  * @retval BSP status
  */
int32_t BSP_LCD_FillRect(uint32_t Instance, uint32_t Xpos, uint32_t Ypos, uint32_t Width, uint32_t Height,
                         uint32_t Color)
{
  int32_t ret = BSP_ERROR_NONE;
  if (Instance >= LCD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    uint32_t ref = Ypos + Height;
    /* Write line */
    while (Ypos < ref)
    {
      ret = BSP_LCD_DrawHLine(Instance, Xpos, Ypos++, Width, Color);
    }
  }
  return ret;
}

/**
  * @brief  Draws an horizontal line in currently active layer.
  * @param  Instance   LCD Instance
  * @param  Xpos  X position
  * @param  Ypos  Y position
  * @param  Length  Line length
  * @param  Color RGB color
  * @retval BSP status
  */
int32_t BSP_LCD_DrawHLine(uint32_t Instance, uint32_t Xpos, uint32_t Ypos, uint32_t Length, uint32_t Color)
{
  int32_t ret = BSP_ERROR_NONE;
  if (Instance >= LCD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    uint16_t xRef = Xpos;
    /* Write line */
    while (Xpos < Length + xRef)
    {
      ret = BSP_LCD_WritePixel(Instance, Xpos++, Ypos, Color);
    }
  }
  return ret;
}

/**
  * @brief  Draws a vertical line in currently active layer.
  * @param  Instance   LCD Instance
  * @param  Xpos  X position
  * @param  Ypos  Y position
  * @param  Length  Line length
  * @param  Color RGB color
  * @retval BSP status
  */
int32_t BSP_LCD_DrawVLine(uint32_t Instance, uint32_t Xpos, uint32_t Ypos, uint32_t Length, uint32_t Color)
{
  int32_t ret = BSP_ERROR_NONE;
  if (Instance >= LCD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    uint16_t yRef = Ypos;

    while (Ypos < Length + yRef)
    {
      ret = BSP_LCD_WritePixel(Instance, Xpos, Ypos++, Color);
    }
  }
  return ret;

}

/**
  * @brief  Reads a LCD pixel.
  * @param  Instance    LCD Instance
  * @param  Xpos X position
  * @param  Ypos Y position
  * @param  Color RGB pixel color
  * @retval BSP status
  */
int32_t BSP_LCD_ReadPixel(uint32_t Instance, uint32_t Xpos, uint32_t Ypos, uint32_t *Color)
{
  int32_t ret = BSP_ERROR_NONE;
  if (Instance >= LCD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    uint32_t LCDPixelFormat;
    BSP_LCD_GetPixelFormat(Instance, &LCDPixelFormat);

    if (LCDPixelFormat == LCD_PIXEL_FORMAT_RGB565)
    {
      /* Read data value from RAM memory */
      *Color = *(__IO uint16_t *)(hLtdcHandler.LayerCfg[ActiveLayer].FBStartAdress +
                                  (2 * (Ypos * (hLtdcHandler.LayerCfg[ActiveLayer].ImageWidth) + Xpos)));
    }
    else if (LCDPixelFormat == LCD_PIXEL_FORMAT_ARGB8888)
    {
      /* Read data value from RAM memory */
      *Color = *(__IO uint32_t *)(hLtdcHandler.LayerCfg[ActiveLayer].FBStartAdress +
                                  (4 * (Ypos * (hLtdcHandler.LayerCfg[ActiveLayer].ImageWidth) + Xpos)));
    }
  }
  return ret;
}
/**
  * @brief  Draws a pixel on LCD.
  * @param  Instance LCD Instance
  * @param  Xpos X position
  * @param  Ypos Y position
  * @param  RGB_Code Pixel color
  * @retval BSP status
  */
int32_t BSP_LCD_WritePixel(uint32_t Instance, uint32_t Xpos, uint32_t Ypos, uint32_t RGB_Code)
{
  int32_t ret = BSP_ERROR_NONE;
  if (Instance >= LCD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    uint32_t LCDPixelFormat;
    BSP_LCD_GetPixelFormat(Instance, &LCDPixelFormat);
    /* Write data value to all SDRAM memory */
    if (LCDPixelFormat == LCD_PIXEL_FORMAT_RGB565)
    {
      /* RGB565 format */
      *(__IO uint16_t *)(hLtdcHandler.LayerCfg[ActiveLayer].FBStartAdress + \
                         (2 * (Ypos * (hLtdcHandler.LayerCfg[ActiveLayer].ImageWidth) + Xpos))) = (uint16_t)RGB_Code;
    }
    else if (LCDPixelFormat == LCD_PIXEL_FORMAT_ARGB8888)
    {
      /* ARGB8888 format */
      *(__IO uint32_t *)(hLtdcHandler.LayerCfg[ActiveLayer].FBStartAdress + \
                         (4 * (Ypos * (hLtdcHandler.LayerCfg[ActiveLayer].ImageWidth) + Xpos))) = RGB_Code;
    }
  }
  return ret;
}

/**
  * @brief  Draws a bitmap picture loaded in the internal Flash in currently active layer.
  * @param  Instance LCD Instance
  * @param  Xpos Bmp X position in the LCD
  * @param  Ypos Bmp Y position in the LCD
  * @param  pBmp Pointer to Bmp picture address in the internal Flash.
  * @retval BSP status
  */
int32_t BSP_LCD_DrawBitmap(uint32_t Instance, uint32_t Xpos, uint32_t Ypos, uint8_t *pBmp)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= LCD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    uint32_t index;
    uint32_t width;
    uint32_t height;
    uint32_t bit_pixel;
    uint32_t StartAddress;
    uint32_t xStartAddress;
    uint32_t input_color_mode;

    /* Get bitmap data address offset */
    index = (uint32_t)pBmp[10] + ((uint32_t)pBmp[11] << 8) + ((uint32_t)pBmp[12] << 16)  + ((uint32_t)pBmp[13] << 24);

    /* Read bitmap width */
    width = (uint32_t)pBmp[18] + ((uint32_t)pBmp[19] << 8) + ((uint32_t)pBmp[20] << 16)  + ((uint32_t)pBmp[21] << 24);

    /* Read bitmap height */
    height = (uint32_t)pBmp[22] + ((uint32_t)pBmp[23] << 8) + ((uint32_t)pBmp[24] << 16)  + ((uint32_t)pBmp[25] << 24);

    /* Read bit/pixel */
    bit_pixel = (uint32_t)pBmp[28] + ((uint32_t)pBmp[29] << 8);


    /* Get the layer pixel format */
    if ((bit_pixel / 8U) == 2U)
    {
      input_color_mode = LCD_PIXEL_FORMAT_RGB565;
      /* Set the address */
      StartAddress = (hLtdcHandler.LayerCfg[ActiveLayer].FBStartAdress);
      StartAddress = StartAddress + (2 * (Ypos * (hLtdcHandler.LayerCfg[ActiveLayer].ImageWidth) + Xpos));
    }
    else if ((bit_pixel / 8U) == 4U)
    {
      input_color_mode = LCD_PIXEL_FORMAT_ARGB8888;
      /* Set the address */
      StartAddress = (hLtdcHandler.LayerCfg[ActiveLayer].FBStartAdress);
      StartAddress = StartAddress + (4 * (Ypos * (hLtdcHandler.LayerCfg[ActiveLayer].ImageWidth) + Xpos));
    }


    /*Read the Pixel*/
    uint32_t size =  width * height;
    Pixel_t pixels[MAX_PIXEL_SIZE];
    uint16_t i;

    for (i = 0; i < size; i++)
    {
      if (input_color_mode == LCD_PIXEL_FORMAT_RGB565)
      {
        uint16_t temp_pixel = (pBmp[index + i * 2 + 1] << 8) | pBmp[index + i * 2];
        pixels[i].red = (temp_pixel & 0xF800) >> 8;
        pixels[i].green = (temp_pixel & 0x07E0) >> 3;
        pixels[i].blue = (temp_pixel & 0x001F) << 3;
      }
      else if (input_color_mode == LCD_PIXEL_FORMAT_ARGB8888)
      {
        uint32_t temp_pixel = (pBmp[index + i * 4 + 3] << 24) | (pBmp[index + i * 4 + 2] << 16) |
                              (pBmp[index + i * 4 + 1] << 8) | pBmp[index + i * 4];
        pixels[i].alpha = (temp_pixel & 0xFF000000) >> 24 ;
        pixels[i].red = (temp_pixel & 0x00FF0000) >> 16;
        pixels[i].green = (temp_pixel & 0x0000FF00) >> 8;
        pixels[i].blue = temp_pixel & 0x000000FF;
      }
    }

    /* Write the pixel data to the display */
    if (input_color_mode == LCD_PIXEL_FORMAT_RGB565)
    {
      int16_t y;
      int16_t x;
      for (y = height - 1; y >= 0; y--)
      {
        xStartAddress = StartAddress;
        for (x = 0; x < width; x++)
        {
          uint16_t rgbCode;
          rgbCode = ((pixels[y * width + x].red & 0xF8) << 8) |
                    ((pixels[y * width + x].green & 0xFC) << 3) |
                    (pixels[y * width + x].blue >> 3);


          *(__IO uint16_t *)(xStartAddress) = rgbCode;
          xStartAddress += 2U;
        }
        StartAddress += hLtdcHandler.LayerCfg[ActiveLayer].ImageWidth * 2;
      }
    }
    else if ((input_color_mode == LCD_PIXEL_FORMAT_ARGB8888))
    {
      int32_t y;
      int32_t x;
      for (y = height - 1; y >= 0; y--)
      {
        xStartAddress = StartAddress;
        for (x = 0; x < width; x++)
        {
          uint32_t argbCode;
          argbCode = (pixels[y * width + x].alpha << 24) |
                     (pixels[y * width + x].red << 16) |
                     (pixels[y * width + x].green << 8) |
                     pixels[y * width + x].blue;
          *(__IO uint32_t *)(xStartAddress) = argbCode;
          xStartAddress += 4U;
        }
        StartAddress += hLtdcHandler.LayerCfg[ActiveLayer].ImageWidth * 4;
      }
    }
  }
  return ret;
}

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
