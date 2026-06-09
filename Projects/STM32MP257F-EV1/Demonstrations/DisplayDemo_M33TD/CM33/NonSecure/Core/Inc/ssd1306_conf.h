/**
 * Private configuration file for the SSD1306 library.
 * This example is configured for STM32F0, I2C and including all fonts.
 */

#ifndef __SSD1306_CONF_H__
#define __SSD1306_CONF_H__

/** @defgroup SSD1306_Config Microcontroller Family Selection
  * @brief Uncomment to select the target STM32 family.
  */
//#define STM32F0
//#define STM32F1
//#define STM32F4
//#define STM32L0
//#define STM32L4
//#define STM32F3
//#define STM32H7
//#define STM32F7

/** @defgroup SSD1306_Bus Bus Selection
  * @brief Select the communication bus for SSD1306.
  *        Only one should be defined.
  */
#define SSD1306_USE_COM    /**< Use custom communication interface (I3C, etc.) */
//#define SSD1306_USE_I2C   /**< Use I2C bus */
//#define SSD1306_USE_SPI   /**< Use SPI bus */

/** @defgroup SSD1306_I3C_I2C_Config Mixed I3C-I2C Configuration
  * @brief Define I2C port and address for backward compatible I3C controller.
  */
#define SSD1306_I2C_PORT        I2cHandle /**< EV1 I2C8 handle */
#define SSD1306_I2C_ADDR        (0x3C << 1) /**< SSD1306 I2C address shifted for HAL I2C APIs */

/** @defgroup SSD1306_SPI_Config SPI Configuration
  * @brief Define SPI port and pins if using SPI bus.
  */
//#define SSD1306_SPI_PORT        hspi1
//#define SSD1306_CS_Port         OLED_CS_GPIO_Port
//#define SSD1306_CS_Pin          OLED_CS_Pin
//#define SSD1306_DC_Port         OLED_DC_GPIO_Port
//#define SSD1306_DC_Pin          OLED_DC_Pin
//#define SSD1306_Reset_Port      OLED_Res_GPIO_Port
//#define SSD1306_Reset_Pin       OLED_Res_Pin

/** @defgroup SSD1306_Display_Config Display Options
  * @brief Optional display configuration macros.
  */
// Mirror the screen if needed
// #define SSD1306_MIRROR_VERT    /**< Mirror display vertically */
// #define SSD1306_MIRROR_HORIZ   /**< Mirror display horizontally */

// Set inverse color if needed
// #define SSD1306_INVERSE_COLOR  /**< Use inverse color mode */

/** @defgroup SSD1306_Fonts Font Inclusion
  * @brief Include only needed fonts.
  */
#define SSD1306_INCLUDE_FONT_6x8    /**< Include 6x8 font */
//#define SSD1306_INCLUDE_FONT_7x10
//#define SSD1306_INCLUDE_FONT_11x18
//#define SSD1306_INCLUDE_FONT_16x26
//#define SSD1306_INCLUDE_FONT_16x24

/** @defgroup SSD1306_Width_Height Display Size
  * @brief Set display width and height if different from default.
  */
// #define SSD1306_WIDTH           130    /**< Set width if needed (default 128) */
// #define SSD1306_HEIGHT          32     /**< Set height if needed (default 64) */

#endif /* __SSD1306_CONF_H__ */
