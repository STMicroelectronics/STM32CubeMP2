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

/**
  * @brief Visual watchdog overlay operating states.
  */
typedef enum
{
  VISUAL_WDG_OVERLAY_STATUS_RUNNING = 1,
  VISUAL_WDG_OVERLAY_STATUS_REBOOTING = 2,
} VisualWdgOverlayStatus_t;

void VisualWdgOverlay_SetStatus(VisualWdgOverlayStatus_t status);
void VisualWdgOverlay_Show(uint8_t **active_fb, uint8_t **decode_fb);
void VisualWdgOverlay_Update(uint8_t **active_fb, uint8_t **decode_fb);
void VisualWdgOverlay_Hide(void);

#endif /* VISUAL_WDG_OVERLAY_H */

/**
  * @}
  */