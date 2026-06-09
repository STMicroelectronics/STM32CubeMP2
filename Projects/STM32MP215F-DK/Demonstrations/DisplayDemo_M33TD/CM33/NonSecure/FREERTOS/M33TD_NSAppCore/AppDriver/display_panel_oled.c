/**
  ******************************************************************************
  * @file    display_panel_oled.c
  * @author  MCD Application Team
  * @brief   OLED panel descriptor for NSAppCore display task.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  ******************************************************************************
  */

/**
  * @addtogroup DisplayPanel
  * @{
  */

/* Includes ------------------------------------------------------------------*/
#include "display_panel_config.h"
#include "main.h"
#include "ssd1306.h"
#if !defined(SPLASH_ANIMATION_ENABLED)
#include "splash_bmp_128x64.h"
#endif

/* Private variables ---------------------------------------------------------*/
/** @brief SSD1306 communication hooks backed by the project I3C helpers. */
static const SSD1306_Comm_t displayPanelOledComm =
{
  .WriteCommand = MX_I3C_WriteCommand,
  .WriteData = MX_I3C_WriteData,
  .Reset = NULL,
};

/* Private function prototypes -----------------------------------------------*/
static void DisplayPanel_OledInit(void);
static void DisplayPanel_OledPowerOn(void);
static void DisplayPanel_OledPowerOff(void);

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Initialize the OLED panel communication interface.
  * @retval None
  */
static void DisplayPanel_OledInit(void)
{
  MX_I3C1_Init();
  SSD1306_SetCommInterface(&displayPanelOledComm);
}

/**
  * @brief  Power on and initialize the SSD1306 panel.
  * @retval None
  */
static void DisplayPanel_OledPowerOn(void)
{
  static const uint8_t initCommands[] =
  {
    0xAE,       /* Display OFF. */
    0x20, 0x00, /* Set memory addressing mode to horizontal. */
    0xB0,       /* Set page start address for page addressing mode. */
    0xC8,       /* Set COM output scan direction. */
    0x00,       /* Set low column address. */
    0x10,       /* Set high column address. */
    0x40,       /* Set display start line. */
    0x81, 0xFF, /* Set contrast control to maximum. */
    0xA1,       /* Set segment remap. */
    0xA6,       /* Set normal display mode. */
    0xA8, 0x3F, /* Set multiplex ratio to 1/64 duty. */
    0xA4,       /* Resume RAM content display. */
    0xD3, 0x00, /* Set display offset to zero. */
    0xD5, 0xF0, /* Set display clock divide ratio and oscillator frequency. */
    0xD9, 0x22, /* Set pre-charge period. */
    0xDA, 0x12, /* Set COM pins hardware configuration. */
    0xDB, 0x20, /* Set VCOMH deselect level. */
    0x8D, 0x14, /* Enable the internal charge pump. */
    0xAF,       /* Display ON. */
  };

  for (size_t index = 0; index < sizeof(initCommands); ++index)
  {
    ssd1306_WriteCommand(initCommands[index]);
  }

  ssd1306_SetCursor(0U, 0U);
}

/**
  * @brief  Power off the OLED panel and release the I3C transport.
  * @retval None
  */
static void DisplayPanel_OledPowerOff(void)
{
  ssd1306_Fill(Black);
  ssd1306_UpdateScreen();
  ssd1306_SetDisplayOn(0U);
  MX_I3C1_DeInit();
}

/* Exported variables --------------------------------------------------------*/
/**
  * @brief OLED panel configuration descriptor.
  */
static const display_panel_config_t display_panel_oled =
{
  .type = DISPLAY_PANEL_OLED,
  .name = "SSD1306 OLED",
  .width = 128,
  .height = 64,
  .init = DisplayPanel_OledInit,
  .power_on = DisplayPanel_OledPowerOn,
  .power_off = DisplayPanel_OledPowerOff,
#if defined(SPLASH_ANIMATION_ENABLED)
  .animation_frames = NULL,
  .animation_frame_count = 0,
  .animation_fps = 0,
#else
  .splash_bmp = st_bmp_logo_128x64,
#endif
};

/**
  * @brief Pointer to the currently active display panel configuration.
  */
const display_panel_config_t *display_active_panel = &display_panel_oled;

/**
  * @}
  */
