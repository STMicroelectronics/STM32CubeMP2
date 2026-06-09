/**
  ******************************************************************************
  * @file    display_demo_overlay.h
  * @author  MCD Application Team
  * @brief   Header file for Display Demo OLED overlay rendering.
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
  * @addtogroup DisplayDemoOverlay
  * @{
  */

#ifndef DISPLAY_DEMO_OVERLAY_H
#define DISPLAY_DEMO_OVERLAY_H

/* Exported functions --------------------------------------------------------*/
/**
  * @brief  Initialize overlay resources.
  * @retval 0 on success, negative value on failure.
  */
int DisplayDemoOverlay_Init(void);

/**
  * @brief  Deinitialize overlay resources.
  * @retval None
  */
void DisplayDemoOverlay_Deinit(void);

/**
  * @brief  Show the overlay and start periodic refresh.
  * @retval None
  */
void DisplayDemoOverlay_Show(void);

/**
  * @brief  Refresh the overlay if it is currently visible.
  * @retval None
  */
void DisplayDemoOverlay_Update(void);

/**
  * @brief  Hide the overlay and clear the OLED framebuffer.
  * @retval None
  */
void DisplayDemoOverlay_Hide(void);

#endif /* DISPLAY_DEMO_OVERLAY_H */

/**
  * @}
  */