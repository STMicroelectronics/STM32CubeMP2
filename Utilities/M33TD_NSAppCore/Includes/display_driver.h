/**
  ******************************************************************************
  * @file    display_driver.h
  * @author  MCD Application Team
  * @brief   Abstract interface for Display control.
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

#ifndef DISPLAY_DRIVER_H
#define DISPLAY_DRIVER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/**
  * @brief Display driver type.
  */
typedef enum
{
  DISPLAY_TYPE_NONE = 0,
  DISPLAY_TYPE_LOW_POWER,
  DISPLAY_TYPE_SYSTEM
} DisplayType_t;

/**
  * @brief Display panel state.
  */
typedef enum
{
  DISPLAY_PANEL_OFFLINE = 0,
  DISPLAY_PANEL_ONLINE,
  DISPLAY_PANEL_ERROR
} DisplayPanelState_t;

/**
  * @brief Display status.
  */
typedef struct
{
  DisplayPanelState_t panel_state;
  bool resources_acquired;
  uint8_t brightness;
} DisplayStatus_t;

/**
  * @brief Display driver interface.
  *
  * Project provides one concrete driver by defining the global instance:
  * `DisplayDriverTypeDef display_driver;`
  */
typedef struct
{
  int  (*init)(void);
  void (*deinit)(void);
  void (*show_splash)(const void *param);
  void (*hide_splash)(const void *param);
#if defined(OVERLAY_FEATURE_ENABLED)
  void (*show_overlay)(const void *param);
  void (*hide_overlay)(const void *param);
  /* Generic non-lifecycle overlay updates are routed through this hook.
   * For DISPLAY_CMD_UPDATE_OVERLAY the caller passes a DisplayOverlayUpdateType_t
   * value encoded in the opaque param.
   */
  void (*update_overlay)(const void *param);
#endif
  DisplayType_t (*get_type)(void);
  DisplayStatus_t (*get_status)(void);
#if defined(SPLASH_ANIMATION_ENABLED)
  void (*handle_animation_frame)(void);
#endif
} DisplayDriverTypeDef;

/**
  * @brief Global display driver instance.
  * @note  Project must define this symbol in exactly one C file.
  */
extern DisplayDriverTypeDef display_driver;

#ifdef __cplusplus
}
#endif

/**
  * @}
  */

#endif /* DISPLAY_DRIVER_H */

