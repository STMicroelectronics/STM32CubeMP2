/*
  ******************************************************************************
  * @file    display_panel_rocktech.c
  * @author  MCD Application Team
  * @brief   Rocktech RGB panel configuration (RK043FN48H).
  *
  * @details
  * This file provides a reference Rocktech RGB panel configuration and basic
  * LTDC bring-up.
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
  * @addtogroup DisplayPanel
  * @{
  */

/* Includes ------------------------------------------------------------------*/
#include "display_panel_config.h"
#include "main.h"

/* Splash assets -------------------------------------------------------------*/
#if defined(SPLASH_ANIMATION_ENABLED)
#include "splash_anim_13fps_480x272_001.h"
#include "splash_anim_13fps_480x272_002.h"
#include "splash_anim_13fps_480x272_003.h"
#include "splash_anim_13fps_480x272_004.h"
#include "splash_anim_13fps_480x272_005.h"
#include "splash_anim_13fps_480x272_006.h"
#include "splash_anim_13fps_480x272_007.h"
#include "splash_anim_13fps_480x272_008.h"
#include "splash_anim_13fps_480x272_009.h"
#include "splash_anim_13fps_480x272_010.h"
#include "splash_anim_13fps_480x272_011.h"
#include "splash_anim_13fps_480x272_012.h"
#include "splash_anim_13fps_480x272_013.h"
#include "splash_anim_13fps_480x272_014.h"
#include "splash_anim_13fps_480x272_015.h"
#include "splash_anim_13fps_480x272_016.h"
#include "splash_anim_13fps_480x272_017.h"
#include "splash_anim_13fps_480x272_018.h"
#include "splash_anim_13fps_480x272_019.h"
#include "splash_anim_13fps_480x272_020.h"
#include "splash_anim_13fps_480x272_021.h"
#include "splash_anim_13fps_480x272_022.h"
#include "splash_anim_13fps_480x272_023.h"
#include "splash_anim_13fps_480x272_024.h"
#include "splash_anim_13fps_480x272_025.h"
#include "splash_anim_13fps_480x272_026.h"
#else
#include "splash_bmp_480x272.h"
#endif

/* Private variables ---------------------------------------------------------*/
#if defined(SPLASH_ANIMATION_ENABLED)
static const unsigned char *const display_rocktech_animation_frames[] =
{
  splash_anim_13fps_480x272_001,
  splash_anim_13fps_480x272_002,
  splash_anim_13fps_480x272_003,
  splash_anim_13fps_480x272_004,
  splash_anim_13fps_480x272_005,
  splash_anim_13fps_480x272_006,
  splash_anim_13fps_480x272_007,
  splash_anim_13fps_480x272_008,
  splash_anim_13fps_480x272_009,
  splash_anim_13fps_480x272_010,
  splash_anim_13fps_480x272_011,
  splash_anim_13fps_480x272_012,
  splash_anim_13fps_480x272_013,
  splash_anim_13fps_480x272_014,
  splash_anim_13fps_480x272_015,
  splash_anim_13fps_480x272_016,
  splash_anim_13fps_480x272_017,
  splash_anim_13fps_480x272_018,
  splash_anim_13fps_480x272_019,
  splash_anim_13fps_480x272_020,
  splash_anim_13fps_480x272_021,
  splash_anim_13fps_480x272_022,
  splash_anim_13fps_480x272_023,
  splash_anim_13fps_480x272_024,
  splash_anim_13fps_480x272_025,
  splash_anim_13fps_480x272_026
};
#endif

/* Exported variables --------------------------------------------------------*/
/**
  * @brief LTDC handle/config used during panel bring-up.
  */
extern LTDC_HandleTypeDef hLtdcHandler;

/* Private function prototypes -----------------------------------------------*/
/**
  * @brief  Initialize the Rocktech RGB panel and LTDC.
  *
  * @note This function performs the full panel bring-up sequence.
  */
static void display_rocktech_panel_init(void);

/**
  * @brief  Optional power-on hook for the Rocktech panel.
  */
static void display_rocktech_panel_power_on(void);

/**
  * @brief  Power-off hook for the Rocktech panel.
  */
static void display_rocktech_panel_power_off(void);

/**
  * @brief Rocktech RGB panel configuration descriptor.
  */
const display_panel_config_t display_panel_rocktech =
{
  .type = DISPLAY_PANEL_ROCKTECH,
  .name = "Rocktech RK043FN48H",
  .width = 480,
  .height = 272,
  .init = display_rocktech_panel_init,
  .power_on = display_rocktech_panel_power_on,
  .power_off = display_rocktech_panel_power_off,
#if defined(SPLASH_ANIMATION_ENABLED)
  .animation_frames = (const unsigned char **)display_rocktech_animation_frames,
  .animation_frame_count = (int)(sizeof(display_rocktech_animation_frames) / sizeof(display_rocktech_animation_frames[0])),
  .animation_fps = 13,
#else
  .splash_bmp = st_bmp_logo,
#endif
};

/**
  * @brief Pointer to the currently active display panel configuration.
  */
const display_panel_config_t *display_active_panel = &display_panel_rocktech;

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Panel initialization entry point.
  * @retval None
  */
static void display_rocktech_panel_init(void)
{
  /* --------------------------------------------------------------------------
   * Hardware layer initialization of LTDC to display content on RGB panel
   * -------------------------------------------------------------------------- */

  if (ResMgr_Request(RESMGR_RESOURCE_RIFSC, STM32MP21_RIFSC_LTDC_CMN_ID) != RESMGR_STATUS_ACCESS_OK)
  {
    Error_Handler();
  }

  if (ResMgr_Request(RESMGR_RESOURCE_RIFSC, STM32MP21_RIFSC_LTDC_L3_ID) != RESMGR_STATUS_ACCESS_OK)
  {
    Error_Handler();
  }

  /* Configure the LTDC */
  LTDC_ColorTypeDef Color = {0x00, 0x00, 0x00};
  MX_LTDC_Init(Color, LTDC_PIXEL_FORMAT_RGB565);

  /* Release all display and related hardware resources */
  /* Keep LTDC layer resource acquired while display is in use. */
  ResMgr_Release(RESMGR_RESOURCE_RIFSC, STM32MP21_RIFSC_LTDC_CMN_ID);
  
}

/**
  * @brief  Panel power-on hook.
  * @retval None
  */
static void display_rocktech_panel_power_on(void)
{
	/* Optional power on if required*/
}

/**
  * @brief  Panel power-off hook.
  * @retval None
  */
static void display_rocktech_panel_power_off(void)
{
  HAL_LTDC_DeInit(&hLtdcHandler);
}

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
