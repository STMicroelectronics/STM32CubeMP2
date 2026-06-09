/**
  ******************************************************************************
  * @file    lcd_panel_ssd1306_oled.h
  * @author  MCD Application Team
  * @brief   OLED geometry/constants for SSD1306-compatible panels.
  ******************************************************************************
  */

#ifndef __OLED_PANEL_SSD1306_H
#define __OLED_PANEL_SSD1306_H

#include <stdint.h>

/*
 * SSD1306 OLED panel used by DisplayDemo.
 * Only geometry is required by the current utility stack for this panel.
 */
#define SSD1306_OLED_128X64_WIDTH   ((uint16_t)128)
#define SSD1306_OLED_128X64_HEIGHT  ((uint16_t)64)

#define SSD1306_OLED_64X128_WIDTH   ((uint16_t)64)
#define SSD1306_OLED_64X128_HEIGHT  ((uint16_t)128)

#if defined(LCD_ORIENTATION_PORTRAIT)
#define LCD_WIDTH   SSD1306_OLED_64X128_WIDTH
#define LCD_HEIGHT  SSD1306_OLED_64X128_HEIGHT
#elif defined(LCD_ORIENTATION_LANDSCAPE)
#define LCD_WIDTH   SSD1306_OLED_128X64_WIDTH
#define LCD_HEIGHT  SSD1306_OLED_128X64_HEIGHT
#else
/* Default to the panel's native 128x64 layout when no orientation is set. */
#define LCD_WIDTH   SSD1306_OLED_128X64_WIDTH
#define LCD_HEIGHT  SSD1306_OLED_128X64_HEIGHT
#endif

#endif /* __OLED_PANEL_SSD1306_H */