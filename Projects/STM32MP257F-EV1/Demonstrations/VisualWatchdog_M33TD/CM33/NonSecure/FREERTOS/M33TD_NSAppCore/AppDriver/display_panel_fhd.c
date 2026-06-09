/**
  ******************************************************************************
  * @file    display_panel_fhd.c
  * @author  MCD Application Team
  * @brief   ETML FHD panel configuration.
  *
  * @details
  * This file provides a reference panel configuration and basic display
  * peripheral bring-up. Copy into your project and adapt GPIOs, timing, and
  * assets to match your board and panel.
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
#include "splash_anim_13fps_1024x600_001.h"
#include "splash_anim_13fps_1024x600_002.h"
#include "splash_anim_13fps_1024x600_003.h"
#include "splash_anim_13fps_1024x600_004.h"
#include "splash_anim_13fps_1024x600_005.h"
#include "splash_anim_13fps_1024x600_006.h"
#include "splash_anim_13fps_1024x600_007.h"
#include "splash_anim_13fps_1024x600_008.h"
#include "splash_anim_13fps_1024x600_009.h"
#include "splash_anim_13fps_1024x600_010.h"
#include "splash_anim_13fps_1024x600_011.h"
#include "splash_anim_13fps_1024x600_012.h"
#include "splash_anim_13fps_1024x600_013.h"
#include "splash_anim_13fps_1024x600_014.h"
#include "splash_anim_13fps_1024x600_015.h"
#include "splash_anim_13fps_1024x600_016.h"
#include "splash_anim_13fps_1024x600_017.h"
#include "splash_anim_13fps_1024x600_018.h"
#include "splash_anim_13fps_1024x600_019.h"
#include "splash_anim_13fps_1024x600_020.h"
#include "splash_anim_13fps_1024x600_021.h"
#include "splash_anim_13fps_1024x600_022.h"
#include "splash_anim_13fps_1024x600_023.h"
#include "splash_anim_13fps_1024x600_024.h"
#include "splash_anim_13fps_1024x600_025.h"
#include "splash_anim_13fps_1024x600_026.h"
#else
#include "splash_bmp_1024x600.h"
#endif

/* Private variables ---------------------------------------------------------*/
#if defined(SPLASH_ANIMATION_ENABLED)
static const unsigned char *const display_fhd_animation_frames[] =
{
  splash_anim_13fps_1024x600_001,
  splash_anim_13fps_1024x600_002,
  splash_anim_13fps_1024x600_003,
  splash_anim_13fps_1024x600_004,
  splash_anim_13fps_1024x600_005,
  splash_anim_13fps_1024x600_006,
  splash_anim_13fps_1024x600_007,
  splash_anim_13fps_1024x600_008,
  splash_anim_13fps_1024x600_009,
  splash_anim_13fps_1024x600_010,
  splash_anim_13fps_1024x600_011,
  splash_anim_13fps_1024x600_012,
  splash_anim_13fps_1024x600_013,
  splash_anim_13fps_1024x600_014,
  splash_anim_13fps_1024x600_015,
  splash_anim_13fps_1024x600_016,
  splash_anim_13fps_1024x600_017,
  splash_anim_13fps_1024x600_018,
  splash_anim_13fps_1024x600_019,
  splash_anim_13fps_1024x600_020,
  splash_anim_13fps_1024x600_021,
  splash_anim_13fps_1024x600_022,
  splash_anim_13fps_1024x600_023,
  splash_anim_13fps_1024x600_024,
  splash_anim_13fps_1024x600_025,
  splash_anim_13fps_1024x600_026
};
#endif

/* Exported variables --------------------------------------------------------*/
/**
  * @brief LVDS/LTDC handle/config used during panel bring-up.
  */
extern LVDS_HandleTypeDef hlvds_F;
extern LTDC_HandleTypeDef hLtdcHandler;

/* Private function prototypes -----------------------------------------------*/
static void display_fhd_panel_init(void);
static void display_fhd_panel_power_on(void);
static void display_fhd_panel_power_off(void);

/**
  * @brief Display panel configuration instance.
  */
const display_panel_config_t display_panel_fhd =
{
  .type = DISPLAY_PANEL_FHD,
  .name = "ETML FHD",
  .width = 1024,
  .height = 600,
  .init = display_fhd_panel_init,
  .power_on = display_fhd_panel_power_on,
  .power_off = display_fhd_panel_power_off,
#if defined(SPLASH_ANIMATION_ENABLED)
	.animation_frames = (const unsigned char **)display_fhd_animation_frames,
	.animation_frame_count = (int)(sizeof(display_fhd_animation_frames) / sizeof(display_fhd_animation_frames[0])),
	.animation_fps = 13,
#else
	.splash_bmp = st_bmp_logo,
#endif
};

/**
  * @brief Pointer to the active display panel configuration.
  */
const display_panel_config_t *display_active_panel = &display_panel_fhd;

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Initialize the display panel.
  * @retval None
  */
static void display_fhd_panel_init(void)
{
	  /* --------------------------------------------------------------------------
	   * Hardware layer initialization of LVDS and LTDC to display content on Panel
	   * -------------------------------------------------------------------------- */

	  if(ResMgr_Request(RESMGR_RESOURCE_RIFSC, STM32MP25_RIFSC_LVDS_ID) != RESMGR_STATUS_ACCESS_OK)
	  {
	  	 Error_Handler();
	  }

	  if(ResMgr_Request(RESMGR_RESOURCE_RIFSC, STM32MP25_RIFSC_LTDC_CMN_ID) != RESMGR_STATUS_ACCESS_OK)
	  {
	    Error_Handler();
	  }

	  if(ResMgr_Request(RESMGR_RESOURCE_RIFSC, STM32MP25_RIFSC_LTDC_ROT_ID) != RESMGR_STATUS_ACCESS_OK)
	  {
	    Error_Handler();
	  }

	  if(ResMgr_Request(RESMGR_RESOURCE_RIFSC, STM32MP25_RIFSC_LTDC_L2_ID) != RESMGR_STATUS_ACCESS_OK)
	  {
	    Error_Handler();
	  }

	  MX_LVDS_Init();

	  /* Configure the LTDC */
	  LTDC_ColorTypeDef Color = {0x00, 0x00, 0x00};
	  MX_LTDC_Init(Color, LTDC_PIXEL_FORMAT_RGB565);

	  HAL_LVDS_Start(&hlvds_F);

	  /* Release all display and related hardware resources */
	  ResMgr_Release(RESMGR_RESOURCE_RIFSC, STM32MP25_RIFSC_LTDC_CMN_ID);
	  ResMgr_Release(RESMGR_RESOURCE_RIFSC, STM32MP25_RIFSC_LVDS_ID);
	  ResMgr_Release(RESMGR_RESOURCE_RIFSC, STM32MP25_RIFSC_LTDC_ROT_ID);

}

/**
  * @brief  Power ON sequence for the display panel.
  * @retval None
  */
static void display_fhd_panel_power_on(void)
{
	/* Optional power on if required*/
}

/**
  * @brief  Power OFF sequence for the display panel.
  * @retval None
  */
static void display_fhd_panel_power_off(void)
{
	HAL_LVDS_DeInit(&hlvds_F);
	HAL_LTDC_DeInit(&hLtdcHandler);
}

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
