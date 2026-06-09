/**
  ******************************************************************************
  * @file    visual_wdg_overlay.c
  * @author  MCD Application Team
  * @brief   Visual Watchdog overlay implementation.
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
  * @addtogroup VisualWdgOverlay
  * @{
  */

/* Includes ------------------------------------------------------------------*/
#include "visual_wdg_overlay.h"

#if defined(OVERLAY_FEATURE_ENABLED)

#include "display_task.h"
#include "display_utils.h"
#include "main.h"
#include "top_msg_green_off.h"
#include "top_msg_green_on.h"
#include "top_msg_red_off.h"
#include "top_msg_red_on.h"

#include <stdbool.h>
#include <stddef.h>

/* Private defines -----------------------------------------------------------*/
#define VISUAL_WDG_OVERLAY_BAR_X          116
#define VISUAL_WDG_OVERLAY_BAR_Y          0
#define VISUAL_WDG_OVERLAY_BAR_WIDTH      248
#define VISUAL_WDG_OVERLAY_BAR_HEIGHT     16
#define VISUAL_WDG_OVERLAY_ANIM_PERIOD_MS 1000U

/* Private types -------------------------------------------------------------*/
/**
  * @brief Runtime state for the visual watchdog overlay.
  */
typedef struct
{
  VisualWdgOverlayStatus_t status;
  uint32_t frame_index;
  bool visible;
  bool animation_running;
  osTimerId_t anim_timer;
  uint8_t **active_fb;
  uint8_t **decode_fb;
} VisualWdgOverlayState_t;

/* Private variables ---------------------------------------------------------*/
/** @brief Animation frames used while the watchdog state is healthy. */
static const unsigned char *const visual_wdg_running_frames[] =
{
  top_msg_green_on,
  top_msg_green_off,
};

/** @brief Animation frames used while the watchdog indicates rebooting. */
static const unsigned char *const visual_wdg_rebooting_frames[] =
{
  top_msg_red_on,
  top_msg_red_off,
};

/** @brief Overlay state instance. */
static VisualWdgOverlayState_t visualWdgOverlayState;

/* Private function prototypes -----------------------------------------------*/
static uint32_t VisualWdgOverlay_GetCurrentFrameCount(void);
static const unsigned char *VisualWdgOverlay_GetCurrentFrame(uint32_t *frame_count);
static void VisualWdgOverlay_UpdateAnimationState(void);
static void VisualWdgOverlay_Render(void);
static void VisualWdgOverlay_AnimationTimerCallback(void *argument);

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Get the number of frames for the current watchdog status.
  * @return Frame count associated with the active status.
  */
static uint32_t VisualWdgOverlay_GetCurrentFrameCount(void)
{
  if (visualWdgOverlayState.status == VISUAL_WDG_OVERLAY_STATUS_REBOOTING)
  {
    return (uint32_t)(sizeof(visual_wdg_rebooting_frames) /
                      sizeof(visual_wdg_rebooting_frames[0]));
  }

  return (uint32_t)(sizeof(visual_wdg_running_frames) /
                    sizeof(visual_wdg_running_frames[0]));
}

/**
  * @brief  Get the bitmap for the current animation frame.
  * @param  frame_count Output pointer receiving the active animation frame count.
  * @return Pointer to the selected frame bitmap.
  */
static const unsigned char *VisualWdgOverlay_GetCurrentFrame(uint32_t *frame_count)
{
  *frame_count = VisualWdgOverlay_GetCurrentFrameCount();

  if (visualWdgOverlayState.status == VISUAL_WDG_OVERLAY_STATUS_REBOOTING)
  {
    return visual_wdg_rebooting_frames[visualWdgOverlayState.frame_index % (*frame_count)];
  }

  return visual_wdg_running_frames[visualWdgOverlayState.frame_index % (*frame_count)];
}

/**
  * @brief  Start or stop the overlay animation timer according to state.
  * @retval None
  */
static void VisualWdgOverlay_UpdateAnimationState(void)
{
  const uint32_t frame_count = VisualWdgOverlay_GetCurrentFrameCount();

  if (visualWdgOverlayState.anim_timer == NULL)
  {
    visualWdgOverlayState.anim_timer = osTimerNew(VisualWdgOverlay_AnimationTimerCallback,
                                                  osTimerPeriodic,
                                                  NULL,
                                                  NULL);
  }

  if (visualWdgOverlayState.anim_timer == NULL)
  {
    visualWdgOverlayState.animation_running = false;
    return;
  }

  if (visualWdgOverlayState.animation_running)
  {
    (void)osTimerStop(visualWdgOverlayState.anim_timer);
  }

  if (visualWdgOverlayState.visible && (frame_count > 1U))
  {
    visualWdgOverlayState.animation_running = true;
    (void)osTimerStart(visualWdgOverlayState.anim_timer,
                       VISUAL_WDG_OVERLAY_ANIM_PERIOD_MS);
  }
  else
  {
    visualWdgOverlayState.animation_running = false;
  }
}

/**
  * @brief  Render the current overlay frame into the LTDC layer.
  * @retval None
  */
static void VisualWdgOverlay_Render(void)
{
  const unsigned char *bitmap;
  uint32_t frame_count;

  if ((visualWdgOverlayState.active_fb == NULL) ||
      (visualWdgOverlayState.decode_fb == NULL) ||
      (*visualWdgOverlayState.active_fb == NULL) ||
      (*visualWdgOverlayState.decode_fb == NULL) ||
      !DisplayUtils_IsLtdcClockEnabled())
  {
    return;
  }

  bitmap = VisualWdgOverlay_GetCurrentFrame(&frame_count);
  (void)frame_count;

  if (DisplayUtils_DecodeSplashBMP(bitmap,
                                   *visualWdgOverlayState.decode_fb,
                                   VISUAL_WDG_OVERLAY_BAR_WIDTH,
                                   VISUAL_WDG_OVERLAY_BAR_HEIGHT) != 0)
  {
    return;
  }

  DisplayUtils_LTDC_ConfigLayer(*visualWdgOverlayState.decode_fb,
                                VISUAL_WDG_OVERLAY_BAR_X,
                                VISUAL_WDG_OVERLAY_BAR_Y,
                                VISUAL_WDG_OVERLAY_BAR_WIDTH,
                                VISUAL_WDG_OVERLAY_BAR_HEIGHT);
  DisplayUtils_FlipBuffers(visualWdgOverlayState.active_fb,
                           visualWdgOverlayState.decode_fb,
                           LTDC_LAYER_3,
                           true);
}

/**
  * @brief  Periodic timer callback used to request overlay animation updates.
  * @param  argument Unused timer argument.
  * @retval None
  */
static void VisualWdgOverlay_AnimationTimerCallback(void *argument)
{
  (void)argument;

  if (!visualWdgOverlayState.visible)
  {
    return;
  }

  {
    const DisplayTaskCommand_t cmd = {
      .type = DISPLAY_CMD_UPDATE_OVERLAY,
      .param = (uint32_t)DISPLAY_OVERLAY_UPDATE_FRAME_EVENT
    };
    (void)DisplayTask_PostCommand(&cmd);
  }
}

/* Exported functions --------------------------------------------------------*/
/**
  * @brief  Update the visual watchdog status.
  * @param  status New overlay status to display.
  * @retval None
  */
void VisualWdgOverlay_SetStatus(VisualWdgOverlayStatus_t status)
{
  if (visualWdgOverlayState.status == status)
  {
    return;
  }

  visualWdgOverlayState.status = status;
  visualWdgOverlayState.frame_index = 0U;

  if (visualWdgOverlayState.visible)
  {
    VisualWdgOverlay_UpdateAnimationState();
    VisualWdgOverlay_Render();
  }
}

/**
  * @brief  Show the overlay using the provided framebuffers.
  * @param  active_fb Pointer to the active framebuffer pointer.
  * @param  decode_fb Pointer to the decode framebuffer pointer.
  * @retval None
  */
void VisualWdgOverlay_Show(uint8_t **active_fb, uint8_t **decode_fb)
{
  if (visualWdgOverlayState.status != VISUAL_WDG_OVERLAY_STATUS_REBOOTING)
  {
    visualWdgOverlayState.status = VISUAL_WDG_OVERLAY_STATUS_RUNNING;
    visualWdgOverlayState.frame_index = 0U;
  }

  visualWdgOverlayState.visible = true;
  visualWdgOverlayState.active_fb = active_fb;
  visualWdgOverlayState.decode_fb = decode_fb;

  VisualWdgOverlay_Render();
  VisualWdgOverlay_UpdateAnimationState();
}

/**
  * @brief  Advance and render the overlay animation.
  * @param  active_fb Pointer to the active framebuffer pointer.
  * @param  decode_fb Pointer to the decode framebuffer pointer.
  * @retval None
  */
void VisualWdgOverlay_Update(uint8_t **active_fb, uint8_t **decode_fb)
{
  uint32_t frame_count;

  if (!visualWdgOverlayState.visible)
  {
    return;
  }

  visualWdgOverlayState.active_fb = active_fb;
  visualWdgOverlayState.decode_fb = decode_fb;

  (void)VisualWdgOverlay_GetCurrentFrame(&frame_count);
  if (frame_count > 1U)
  {
    visualWdgOverlayState.frame_index = (visualWdgOverlayState.frame_index + 1U) % frame_count;
  }

  VisualWdgOverlay_Render();
}

/**
  * @brief  Hide the overlay and stop any running animation.
  * @retval None
  */
void VisualWdgOverlay_Hide(void)
{
  if (visualWdgOverlayState.animation_running && (visualWdgOverlayState.anim_timer != NULL))
  {
    (void)osTimerStop(visualWdgOverlayState.anim_timer);
  }

  visualWdgOverlayState.animation_running = false;
  visualWdgOverlayState.visible = false;
  visualWdgOverlayState.status = VISUAL_WDG_OVERLAY_STATUS_RUNNING;
  visualWdgOverlayState.frame_index = 0U;

  if (DisplayUtils_IsLtdcClockEnabled())
  {
    DisplayUtils_LTDC_HideLayer();
  }
}

#endif /* OVERLAY_FEATURE_ENABLED */

/**
  * @}
  */
