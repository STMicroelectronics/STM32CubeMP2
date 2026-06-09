/**
  ******************************************************************************
  * @file    display_driver_system.c
  * @author  MCD Application Team
  * @brief   Display driver (system type) implementation.
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

/**
  * @addtogroup DisplayDriver
  * @{
  */

/**
  * @addtogroup DisplayDriverSystem
  * @{
  */

/* Includes ------------------------------------------------------------------*/
#include "display_driver.h"
#include "display_task.h"
#include "main.h"
#if defined(OVERLAY_FEATURE_ENABLED)
#include "visual_wdg_overlay.h"
#endif

/* Private defines -----------------------------------------------------------*/
/* System policy: drive LTDC and animation, keep resources acquired. */

#ifndef LCD_FB_START_ADDRESS
#error "LCD_FB_START_ADDRESS must be defined by the project (framebuffer base address)."
#endif

/* Private variables ---------------------------------------------------------*/
static DisplayStatus_t display_status = {
    .panel_state = DISPLAY_PANEL_OFFLINE,
    .resources_acquired = false,
    .brightness = 100U,
};

#if defined(SPLASH_ANIMATION_ENABLED)
static osTimerId_t animation_timer;
static bool animation_running;
static uint32_t anim_index;
#endif

#if defined(OVERLAY_FEATURE_ENABLED)
static bool overlay_visible;
#endif

static uint8_t *active_fb;
static uint8_t *decode_fb;
static uint32_t fb_size;

/* Private function prototypes -----------------------------------------------*/
static void DisplayDriver_InitFramebuffers(void);

#if defined(SPLASH_ANIMATION_ENABLED)
static uint32_t DisplayDriver_GetAnimationPeriodMs(void);
static void DisplayDriver_AnimationTimerCallback(void *argument);
#endif

static int DisplayDriver_SystemInit(void);
static void DisplayDriver_SystemDeinit(void);
static void DisplayDriver_SystemShowSplash(const void *param);
static void DisplayDriver_SystemHideSplash(const void *param);
#if defined(OVERLAY_FEATURE_ENABLED)
static void DisplayDriver_SystemShowOverlay(const void *param);
static void DisplayDriver_SystemHideOverlay(const void *param);
static void DisplayDriver_SystemUpdateOverlay(const void *param);
#endif
static DisplayType_t DisplayDriver_SystemGetType(void);
static DisplayStatus_t DisplayDriver_SystemGetStatus(void);

#if defined(SPLASH_ANIMATION_ENABLED)
static void DisplayDriver_SystemHandleAnimationFrame(void);
#endif

/* Exported driver instance --------------------------------------------------*/
/**
  * @brief Display driver instance (system type).
  */
DisplayDriverTypeDef display_driver =
{
  .init = DisplayDriver_SystemInit,
  .deinit = DisplayDriver_SystemDeinit,
  .show_splash = DisplayDriver_SystemShowSplash,
  .hide_splash = DisplayDriver_SystemHideSplash,
#if defined(OVERLAY_FEATURE_ENABLED)
  .show_overlay = DisplayDriver_SystemShowOverlay,
  .hide_overlay = DisplayDriver_SystemHideOverlay,
  .update_overlay = DisplayDriver_SystemUpdateOverlay,
#endif
  .get_type = DisplayDriver_SystemGetType,
  .get_status = DisplayDriver_SystemGetStatus,
#if defined(SPLASH_ANIMATION_ENABLED)
  .handle_animation_frame = DisplayDriver_SystemHandleAnimationFrame,
#endif
};

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Initialize framebuffer pointers based on the active panel.
  */
static void DisplayDriver_InitFramebuffers(void)
{
  if (display_active_panel == 0)
  {
    active_fb = (uint8_t *)LCD_FB_START_ADDRESS;
    decode_fb = (uint8_t *)LCD_FB_START_ADDRESS;
    fb_size = 0U;
    return;
  }

  fb_size = (uint32_t)display_active_panel->width * (uint32_t)display_active_panel->height;
  active_fb = (uint8_t *)LCD_FB_START_ADDRESS;
  decode_fb = (uint8_t *)((uintptr_t)LCD_FB_START_ADDRESS + (uintptr_t)fb_size);
}

#if defined(SPLASH_ANIMATION_ENABLED)
/**
  * @brief  Get animation period for the active panel.
  * @return Period in milliseconds.
  */
static uint32_t DisplayDriver_GetAnimationPeriodMs(void)
{
  if ((display_active_panel == 0) || (display_active_panel->animation_fps == 0U))
  {
    return 1000U;
  }

  return (1000U / display_active_panel->animation_fps);
}

/**
  * @brief  RTOS timer callback used to request animation frame decode.
  * @param  argument Unused.
  */
static void DisplayDriver_AnimationTimerCallback(void *argument)
{
  (void)argument;

  const DisplayTaskCommand_t cmd = { .type = DISPLAY_CMD_DECODE_ANIMATION_FRAME, .param = 0U };
  (void)DisplayTask_PostCommand(&cmd);
}
#endif

/**
  * @brief  Initialize the system display driver.
  * @retval 0 on success, negative value on failure.
  */
static int DisplayDriver_SystemInit(void)
{
  if ((display_active_panel == 0) || (display_active_panel->init == 0))
  {
    display_status.panel_state = DISPLAY_PANEL_ERROR;
    display_status.resources_acquired = false;
    return -1;
  }

  display_active_panel->init();
  if (display_active_panel->power_on != 0)
  {
    display_active_panel->power_on();
  }

  DisplayDriver_InitFramebuffers();

#if defined(SPLASH_ANIMATION_ENABLED)
  anim_index = 0U;
  animation_running = false;
#endif
#if defined(OVERLAY_FEATURE_ENABLED)
  overlay_visible = false;
#endif
  display_status.panel_state = DISPLAY_PANEL_ONLINE;
  display_status.resources_acquired = true;
  return 0;
}

/**
  * @brief  Deinitialize the system display driver.
  */
static void DisplayDriver_SystemDeinit(void)
{
#if defined(SPLASH_ANIMATION_ENABLED)
  if (animation_running && (animation_timer != NULL))
  {
    (void)osTimerStop(animation_timer);
  }
  animation_running = false;
#endif

  if ((display_active_panel != 0) && (display_active_panel->power_off != 0))
  {
    display_active_panel->power_off();
  }

  display_status.panel_state = DISPLAY_PANEL_OFFLINE;
  display_status.resources_acquired = false;
}

/**
  * @brief  Show the splash screen.
  */
static void DisplayDriver_SystemShowSplash(const void *param)
{
  (void)param;

#if defined(SPLASH_ANIMATION_ENABLED)
  anim_index = 0U;
#endif

  if ((display_active_panel == 0) || (active_fb == 0) || (fb_size == 0U))
  {
    return;
  }

#if defined(SPLASH_ANIMATION_ENABLED)
  /* Always decode the first frame as the splash image. */
  if ((display_active_panel->animation_frames == 0) || (display_active_panel->animation_frame_count == 0U))
  {
    return;
  }

  (void)DisplayUtils_DecodeSplashBMP(display_active_panel->animation_frames[0], decode_fb,
                                    (int)display_active_panel->width, (int)display_active_panel->height);

  /* Configure layer (uses internal palette captured during decode). */
  DisplayUtils_LTDC_ConfigLayer(decode_fb, 0, 0, (int)display_active_panel->width, (int)display_active_panel->height);

  /* Flip to make decoded buffer visible, and update palette/CLUT. */
  DisplayUtils_FlipBuffers(&active_fb, &decode_fb, LTDC_LAYER_3, true);
#else
  if (display_active_panel->splash_bmp == 0)
  {
    return;
  }

  (void)DisplayUtils_DecodeSplashBMP(display_active_panel->splash_bmp, active_fb,
                                    (int)display_active_panel->width, (int)display_active_panel->height);

  /* Configure layer for the splash buffer (also loads CLUT). */
  DisplayUtils_LTDC_ConfigLayer(active_fb, 0, 0, (int)display_active_panel->width, (int)display_active_panel->height);
#endif

#if defined(SPLASH_ANIMATION_ENABLED)
  if (animation_timer == NULL)
  {
    animation_timer = osTimerNew(DisplayDriver_AnimationTimerCallback, osTimerPeriodic, NULL, NULL);
  }

  if (animation_timer != NULL)
  {
    animation_running = true;
    (void)osTimerStart(animation_timer, DisplayDriver_GetAnimationPeriodMs());
  }
  else
  {
    animation_running = false;
  }
#endif
}

/**
  * @brief  Hide the splash screen.
  */
static void DisplayDriver_SystemHideSplash(const void *param)
{
  (void)param;

#if defined(SPLASH_ANIMATION_ENABLED)
  if (animation_running && (animation_timer != NULL))
  {
    (void)osTimerStop(animation_timer);
  }
  animation_running = false;
#endif

  if (DisplayUtils_IsLtdcClockEnabled())
  {
    DisplayUtils_LTDC_HideLayer();
  }
}

#if defined(OVERLAY_FEATURE_ENABLED)
/**
  * @brief  Show overlay content.
  */
static void DisplayDriver_SystemShowOverlay(const void *param)
{
  (void)param;

  if (!overlay_visible)
  {
    VisualWdgOverlay_Show(&active_fb, &decode_fb);
    overlay_visible = true;
  }
}

/**
  * @brief  Hide overlay content.
  */
static void DisplayDriver_SystemHideOverlay(const void *param)
{
  (void)param;
  VisualWdgOverlay_Hide();
  overlay_visible = false;
}

/**
  * @brief  Update overlay content.
  * @param  status Opaque status pointer provided by application.
  */
static void DisplayDriver_SystemUpdateOverlay(const void *param)
{
  switch ((DisplayOverlayUpdateType_t)(uintptr_t)param)
  {
    case DISPLAY_OVERLAY_UPDATE_REBOOT_EVENT:
      VisualWdgOverlay_SetStatus(VISUAL_WDG_OVERLAY_STATUS_REBOOTING);
      break;
    case DISPLAY_OVERLAY_UPDATE_STATE_SYNC_EVENT:
      VisualWdgOverlay_SetStatus(VISUAL_WDG_OVERLAY_STATUS_RUNNING);
      break;
    case DISPLAY_OVERLAY_UPDATE_FRAME_EVENT:
    default:
      VisualWdgOverlay_Update(&active_fb, &decode_fb);
      break;
  }
}

#endif /* OVERLAY_FEATURE_ENABLED */

/**
  * @brief  Get display driver type.
  * @return Driver type.
  */
static DisplayType_t DisplayDriver_SystemGetType(void)
{
  return DISPLAY_TYPE_SYSTEM;
}

/**
  * @brief  Get display driver status.
  * @return Status structure.
  */
static DisplayStatus_t DisplayDriver_SystemGetStatus(void)
{
  return display_status;
}

#if defined(SPLASH_ANIMATION_ENABLED)
/**
  * @brief  Handle one animation frame decode/flip.
  */
static void DisplayDriver_SystemHandleAnimationFrame(void)
{
  if (!animation_running)
  {
    return;
  }

  if ((display_active_panel == 0) || (display_active_panel->animation_frames == 0) || (display_active_panel->animation_frame_count == 0U))
  {
    return;
  }

  if ((decode_fb == 0) || (fb_size == 0U))
  {
    return;
  }

  (void)DisplayUtils_DecodeSplashBMP(display_active_panel->animation_frames[anim_index], decode_fb,
                                    (int)display_active_panel->width, (int)display_active_panel->height);

  if (DisplayUtils_IsLtdcClockEnabled())
  {
    DisplayUtils_FlipBuffers(&active_fb, &decode_fb, LTDC_LAYER_3, true);
  }
  else
  {
      display_status.panel_state = DISPLAY_PANEL_OFFLINE;
  }

  anim_index = (anim_index + 1U) % display_active_panel->animation_frame_count;
}
#endif

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
