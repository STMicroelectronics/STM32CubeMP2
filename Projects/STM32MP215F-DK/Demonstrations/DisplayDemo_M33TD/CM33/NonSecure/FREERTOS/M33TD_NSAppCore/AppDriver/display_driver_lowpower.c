/**
  ******************************************************************************
  * @file    display_driver_lowpower.c
  * @author  MCD Application Team
  * @brief   SSD1306-backed low-power display driver.
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
  * @addtogroup DisplayDriver
  * @{
  */

/**
  * @addtogroup DisplayDriverLowPower
  * @{
  */

/* Includes ------------------------------------------------------------------*/
#include "display_driver.h"
#include "display_panel_config.h"
#if defined(OVERLAY_FEATURE_ENABLED)
#include "display_demo_overlay.h"
#include "display_task.h"
#endif
#include "main.h"
#include "ssd1306.h"
#include <stdio.h>

/* Private defines -----------------------------------------------------------*/
#if defined(SPLASH_ANIMATION_ENABLED)
#error "SPLASH_ANIMATION_ENABLED is not supported for the OLED low-power display driver. Use the static splash configuration for DisplayDemo_M33TD."
#endif

/* Private variables ---------------------------------------------------------*/
/** @brief Runtime status of the low-power display driver. */
static DisplayStatus_t displayStatus =
{
  .panel_state = DISPLAY_PANEL_OFFLINE,
  .resources_acquired = false,
  .brightness = 100U,
};

/* Private function prototypes -----------------------------------------------*/
static void DisplayDriver_LowPowerClear(void);
static void DisplayDriver_LowPowerRenderSplash(void);
static int DisplayDriver_LowPowerInit(void);
static void DisplayDriver_LowPowerDeinit(void);
static void DisplayDriver_LowPowerShowSplash(const void *param);
static void DisplayDriver_LowPowerHideSplash(const void *param);
#if defined(OVERLAY_FEATURE_ENABLED)
static void DisplayDriver_LowPowerShowOverlay(const void *param);
static void DisplayDriver_LowPowerHideOverlay(const void *param);
static void DisplayDriver_LowPowerUpdateOverlay(const void *param);
#endif
static DisplayType_t DisplayDriver_LowPowerGetType(void);
static DisplayStatus_t DisplayDriver_LowPowerGetStatus(void);

/* Exported driver instance --------------------------------------------------*/
/**
  * @brief Display driver instance (low-power OLED type).
  */
DisplayDriverTypeDef display_driver =
{
  .init = DisplayDriver_LowPowerInit,
  .deinit = DisplayDriver_LowPowerDeinit,
  .show_splash = DisplayDriver_LowPowerShowSplash,
  .hide_splash = DisplayDriver_LowPowerHideSplash,
#if defined(OVERLAY_FEATURE_ENABLED)
  .show_overlay = DisplayDriver_LowPowerShowOverlay,
  .hide_overlay = DisplayDriver_LowPowerHideOverlay,
  .update_overlay = DisplayDriver_LowPowerUpdateOverlay,
#endif
  .get_type = DisplayDriver_LowPowerGetType,
  .get_status = DisplayDriver_LowPowerGetStatus,
};

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Clear the OLED panel.
  * @retval None
  */
static void DisplayDriver_LowPowerClear(void)
{
  ssd1306_Fill(Black);
  ssd1306_UpdateScreen();
}

/**
  * @brief  Render the configured splash image on the OLED panel.
  * @retval None
  */
static void DisplayDriver_LowPowerRenderSplash(void)
{
#if defined(SPLASH_ANIMATION_ENABLED)
  return;
#else
  if ((display_active_panel == NULL) || (display_active_panel->splash_bmp == NULL))
  {
    return;
  }
  DisplayDriver_LowPowerClear();
  ssd1306_Fill(White);
  ssd1306_DrawBitmap(0U, 0U, display_active_panel->splash_bmp, display_active_panel->width, display_active_panel->height, Black);
  ssd1306_UpdateScreen();
#endif
}

/**
  * @brief  Initialize the low-power display driver.
  * @retval 0 on success, negative value on failure.
  */
static int DisplayDriver_LowPowerInit(void)
{
  if ((display_active_panel == NULL) || (display_active_panel->init == NULL))
  {
    displayStatus.panel_state = DISPLAY_PANEL_ERROR;
    displayStatus.resources_acquired = false;
    return -1;
  }

  display_active_panel->init();
  if (display_active_panel->power_on != NULL)
  {
    display_active_panel->power_on();
  }

#if defined(OVERLAY_FEATURE_ENABLED)
  if (DisplayDemoOverlay_Init() != 0)
  {
    if ((display_active_panel != NULL) && (display_active_panel->power_off != NULL))
    {
      display_active_panel->power_off();
    }

    displayStatus.panel_state = DISPLAY_PANEL_ERROR;
    displayStatus.resources_acquired = false;
    return -1;
  }
#endif

  displayStatus.panel_state = DISPLAY_PANEL_ONLINE;
  displayStatus.resources_acquired = true;
  return 0;
}

/**
  * @brief  Deinitialize the low-power display driver.
  * @retval None
  */
static void DisplayDriver_LowPowerDeinit(void)
{
#if defined(OVERLAY_FEATURE_ENABLED)
  DisplayDemoOverlay_Deinit();
#endif

  if ((display_active_panel != NULL) && (display_active_panel->power_off != NULL))
  {
    display_active_panel->power_off();
  }

  displayStatus.panel_state = DISPLAY_PANEL_OFFLINE;
  displayStatus.resources_acquired = false;
}

/**
  * @brief  Show the splash screen.
  * @param  param Unused command parameter.
  * @retval None
  */
static void DisplayDriver_LowPowerShowSplash(const void *param)
{
  (void)param;
  DisplayDriver_LowPowerRenderSplash();
  osDelay(1000);
  const DisplayTaskCommand_t cmd =
  {
   .type = DISPLAY_CMD_HIDE_SPLASH,
   .param = 0,
  };
  (void)DisplayTask_PostCommand(&cmd);
}

/**
  * @brief  Hide the splash screen.
  * @param  param Unused command parameter.
  * @retval None
  */
static void DisplayDriver_LowPowerHideSplash(const void *param)
{
  (void)param;
  DisplayDriver_LowPowerClear();
}

#if defined(OVERLAY_FEATURE_ENABLED)
/**
  * @brief  Show the overlay content.
  * @param  param Unused command parameter.
  * @retval None
  */
static void DisplayDriver_LowPowerShowOverlay(const void *param)
{
  (void)param;
  DisplayDemoOverlay_Show();
}

/**
  * @brief  Hide the overlay content.
  * @param  param Unused command parameter.
  * @retval None
  */
static void DisplayDriver_LowPowerHideOverlay(const void *param)
{
  (void)param;
  DisplayDemoOverlay_Hide();
}

/**
  * @brief  Update the overlay content.
  * @param  param Unused command parameter.
  * @retval None
  */
static void DisplayDriver_LowPowerUpdateOverlay(const void *param)
{
  (void)param;
  DisplayDemoOverlay_Update();
}
#endif

/**
  * @brief  Get the display driver type.
  * @return Display driver type.
  */
static DisplayType_t DisplayDriver_LowPowerGetType(void)
{
  return DISPLAY_TYPE_LOW_POWER;
}

/**
  * @brief  Get the current low-power display status.
  * @return Display status snapshot.
  */
static DisplayStatus_t DisplayDriver_LowPowerGetStatus(void)
{
  return displayStatus;
}

/**
  * @}
  */

/**
  * @}
  */
