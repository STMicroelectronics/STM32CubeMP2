/**
  ******************************************************************************
  * @file    visual_wdg_overlay.h
  * @author  MCD Application Team
  * @brief   Header file for Visual Watchdog overlay feature.
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

#ifndef VISUAL_WDG_OVERLAY_H
#define VISUAL_WDG_OVERLAY_H

#include <stdint.h>

/* Exported types ------------------------------------------------------------*/
/**
  * @brief Visual watchdog overlay operating states.
  */
typedef enum
{
  VISUAL_WDG_OVERLAY_STATUS_RUNNING = 1,
  VISUAL_WDG_OVERLAY_STATUS_REBOOTING = 2,
} VisualWdgOverlayStatus_t;

/* Exported functions --------------------------------------------------------*/
/**
  * @brief  Update the visual watchdog status.
  * @param  status New overlay status to display.
  * @retval None
  */
void VisualWdgOverlay_SetStatus(VisualWdgOverlayStatus_t status);

/**
  * @brief  Show the overlay using the provided framebuffers.
  * @param  active_fb Pointer to the active framebuffer pointer.
  * @param  decode_fb Pointer to the decode framebuffer pointer.
  * @retval None
  */
void VisualWdgOverlay_Show(uint8_t **active_fb, uint8_t **decode_fb);

/**
  * @brief  Advance and render the overlay animation.
  * @param  active_fb Pointer to the active framebuffer pointer.
  * @param  decode_fb Pointer to the decode framebuffer pointer.
  * @retval None
  */
void VisualWdgOverlay_Update(uint8_t **active_fb, uint8_t **decode_fb);

/**
  * @brief  Hide the overlay and stop any running animation.
  * @retval None
  */
void VisualWdgOverlay_Hide(void);

#endif /* VISUAL_WDG_OVERLAY_H */

/**
  * @}
  */
