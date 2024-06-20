/**
  ******************************************************************************
  * @file    stm32mp257f_disco_camera.c
  * @author  MCD Application Team
  * @brief   This file provides a set of firmware functions to communicate
  *          with camera devices available on STM32MP257F-EV1 board (MB1936)
  *          from STMicroelectronics.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* File Info: ------------------------------------------------------------------
                                   User NOTES
1. How to use this driver:
--------------------------
   - This driver is used to drive the camera.
   - The OV5640 component driver MUST be included with this driver.

2. Driver description:
---------------------
     o Initialize the camera instance using the BSP_CAMERA_Init() function with the required
       Resolution and Pixel format where:
       - Instance: Is the physical camera interface and is always 0 on this board.
       - Resolution: The camera resolution
       - PixelFormat: The camera Pixel format

     o DeInitialize the camera instance using the BSP_CAMERA_Init() . This
       function will firstly stop the camera to insure the data transfer complete.

     o Get the camera instance capabilities using the BSP_CAMERA_GetCapabilities().
       This function must be called after the BSP_CAMERA_Init() to get the right
       sensor capabilities

     o Start the camera using the CAMERA_Start() function by specifying the capture Mode:
       - CAMERA_MODE_CONTINUOUS: For continuous capture
       - CAMERA_MODE_SNAPSHOT  : For on shot capture

     o Suspend, resume or stop the camera capture using the following functions:
      - BSP_CAMERA_Suspend()
      - BSP_CAMERA_Resume()
      - BSP_CAMERA_Stop()

     o Call BSP_CAMERA_SetResolution()/BSP_CAMERA_GetResolution() to set/get the camera resolution
       Resolution: - CAMERA_R160x120
                   - CAMERA_R320x240
                   - CAMERA_R480x272
                   - CAMERA_R640x480
                   - CAMERA_R800x480

     o Call BSP_CAMERA_SetPixelFormat()/BSP_CAMERA_GetPixelFormat() to set/get the camera pixel format
       PixelFormat: - CAMERA_PF_RGB565
                    - CAMERA_PF_RGB888
                    - CAMERA_PF_YUV422

     o Call BSP_CAMERA_SetLightMode()/BSP_CAMERA_GetLightMode() to set/get the camera light mode
       LightMode: - CAMERA_LIGHT_AUTO
                  - CAMERA_LIGHT_SUNNY
                  - CAMERA_LIGHT_OFFICE
                  - CAMERA_LIGHT_HOME
                  - CAMERA_LIGHT_CLOUDY

     o Call BSP_CAMERA_SetColorEffect()/BSP_CAMERA_GetColorEffect() to set/get the camera color effects
       Effect: - CAMERA_COLOR_EFFECT_NONE
               - CAMERA_COLOR_EFFECT_BLUE
               - CAMERA_COLOR_EFFECT_RED
               - CAMERA_COLOR_EFFECT_GREEN
               - CAMERA_COLOR_EFFECT_BW
               - CAMERA_COLOR_EFFECT_SEPIA
               - CAMERA_COLOR_EFFECT_NEGATIVE

     o Call BSP_CAMERA_SetBrightness()/BSP_CAMERA_GetBrightness() to set/get the camera Brightness
       Brightness is value between -4(Level 4 negative) and 4(Level 4 positive).

     o Call BSP_CAMERA_SetSaturation()/BSP_CAMERA_GetSaturation() to set/get the camera Saturation
       Saturation is value between -4(Level 4 negative) and 4(Level 4 positive).

     o Call BSP_CAMERA_SetContrast()/BSP_CAMERA_GetContrast() to set/get the camera Contrast
       Contrast is value between -4(Level 4 negative) and 4(Level 4 positive).

     o Call BSP_CAMERA_SetHueDegree()/BSP_CAMERA_GetHueDegree() to set/get the camera Hue Degree
       HueDegree is value between -4(180 degree negative) and 4(150 degree positive).

     o Call BSP_CAMERA_SetMirrorFlip()/BSP_CAMERA_GetMirrorFlip() to set/get the camera mirror and flip
       MirrorFlip could be any combination of: - CAMERA_MIRRORFLIP_NONE
                                               - CAMERA_MIRRORFLIP_FLIP
                                               - CAMERA_MIRRORFLIP_MIRROR
       Note that This feature is only supported with S5K5CAG sensor.

     o Call BSP_CAMERA_SetZoom()/BSP_CAMERA_GetZoom() to set/get the camera zooming
       Zoom is supported only with S5K5CAG sensor could be any value of:
       - CAMERA_ZOOM_x8 for CAMERA_R160x120 resolution only
       - CAMERA_ZOOM_x4 For all resolutions except CAMERA_R640x480 and CAMERA_R800x480
       - CAMERA_ZOOM_x2 For all resolutions except CAMERA_R800x480
       - CAMERA_ZOOM_x1 For all resolutions

     o Call BSP_CAMERA_EnableNightMode() to enable night mode. This feature is only supported
       with S5K5CAG sensor

     o Call BSP_CAMERA_DisableNightMode() to disable night mode. This feature is only supported
       with S5K5CAG sensor

    o Error, line event, vsync event and frame event are handled through dedicated weak
      callbacks that can be override at application level: BSP_CAMERA_LineEventCallback(),
      BSP_CAMERA_FrameEventCallback(), BSP_CAMERA_VsyncEventCallback(), BSP_CAMERA_ErrorCallback()

  Known Limitations:
  ------------------
   1- CAMERA_PF_RGB888 resolution is not supported with S5K5CAG sensor.
   2- The following feature are only supported through S5K5CAG sensor:
      o LightMode setting
      o Saturation setting
      o HueDegree setting
      o Mirror/Flip setting
      o Zoom setting
      o NightMode enable/disable
------------------------------------------------------------------------------*/

/* Includes ------------------------------------------------------------------*/
#include "stm32mp257f_disco_camera.h"
#include "stm32mp257f_disco_bus.h"


/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32MP257F_DK
  * @{
  */

/** @addtogroup CAMERA
  * @{
  */

/** @defgroup Exported_Variables Exported Variables
  * @{
  */
CAMERA_Ctx_t        CameraCtx[CAMERA_INSTANCES_NBR];

CAMERA_Capabilities_t Cap;

/** @defgroup Private_Variables Private Variables
  * @{
  */
static CAMERA_Drv_t *CameraDrv = NULL;
static void         *CompObj = NULL;
static CAMERA_Capabilities_t *CameraCap;
/**
  * @}
  */

/** @defgroup Private_Functions_Prototypes Private Functions Prototypes
  * @{
  */
#if defined(USE_CAMERA_SENSOR_OV5640)
OV5640_Object_t   OV5640Obj = { 0 };
static int32_t OV5640_Probe(uint32_t Resolution, uint32_t PixelFormat);
#elif defined (USE_CAMERA_SENSOR_IMX219)
IMX219_Object_t   IMX219Obj = { 0 };
static int32_t IMX219_Probe(uint32_t Resolution, uint32_t PixelFormat);
#elif defined (USE_CAMERA_SENSOR_IMX335)
IMX335_Object_t   IMX335Obj = { 0 };
static int32_t IMX335_Probe(uint32_t Resolution, uint32_t PixelFormat);
#endif /* USE_CAMERA_SENSOR_OV5640 */

/**
  * @}
  */

/** @addtogroup CAMERA_Exported_Functions

  * @{
  */
/**
  * @brief  Initializes the camera.
  * @param  Instance    Camera instance.
  * @param  Resolution  Camera sensor requested resolution (x, y) : standard resolution
  *         naming QQVGA, QVGA, VGA ...
  * @param  PixelFormat Capture pixel format
  * @retval BSP status
  */
int32_t BSP_CAMERA_Init(uint32_t Instance, uint32_t Resolution, uint32_t PixelFormat)
{
  int32_t ret;

  if (Instance >= CAMERA_INSTANCES_NBR)
  {
    return BSP_ERROR_WRONG_PARAM;
  }

  if (BSP_CAMERA_HwReset(0) != BSP_ERROR_NONE)
  {
    return BSP_ERROR_BUS_FAILURE;
  }

  /* Read ID of Camera module via I2C */
#if defined(USE_CAMERA_SENSOR_OV5640)
  ret = OV5640_Probe(Resolution, PixelFormat);
#elif defined(USE_CAMERA_SENSOR_IMX219)
  ret = IMX219_Probe(Resolution, PixelFormat);
#elif defined(USE_CAMERA_SENSOR_IMX335)
  ret = IMX335_Probe(Resolution, PixelFormat);
#endif /* USE_CAMERA_SENSOR_OV5640 */
  if (ret != BSP_ERROR_NONE)
  {
    return BSP_ERROR_UNKNOWN_COMPONENT;
  }

  CameraCtx[Instance].Resolution = Resolution;
  CameraCtx[Instance].PixelFormat = PixelFormat;

  /* BSP status */
  return BSP_ERROR_NONE;
}

/**
  * @brief  DeInitializes the camera.
  * @param  Instance Camera instance.
  * @retval BSP status
  */
int32_t BSP_CAMERA_DeInit(uint32_t Instance)
{
  int32_t ret;

  if (Instance >= CAMERA_INSTANCES_NBR)
  {
    return BSP_ERROR_WRONG_PARAM;
  }

  /* First stop the camera to insure all data are transfered */
  ret = BSP_CAMERA_Stop(Instance);
  if (ret != BSP_ERROR_NONE)
  {
    return BSP_ERROR_PERIPH_FAILURE;
  }

  /* Return BSP status */
  return BSP_ERROR_NONE;
}

/**
  * @brief  Starts the camera capture in continuous mode.
  * @param  Instance Camera instance.
  * @param  CaptureMode CAMERA_MODE_CONTINUOUS or CAMERA_MODE_SNAPSHOT
  * @param  pBff     pointer to the camera output buffer
  */
int32_t BSP_CAMERA_Start(uint32_t Instance, uint8_t *pBff, uint32_t Mode)
{
  UNUSED(pBff);
  UNUSED(Mode);

  if (Instance >= CAMERA_INSTANCES_NBR)
  {
    return BSP_ERROR_WRONG_PARAM;
  }

  return BSP_ERROR_FEATURE_NOT_SUPPORTED;
}

/**
  * @brief  Stop the CAMERA capture
  * @param  Instance Camera instance.
  * @retval BSP status
  */
int32_t BSP_CAMERA_Stop(uint32_t Instance)
{
  int32_t ret;

  if (Instance >= CAMERA_INSTANCES_NBR)
  {
    return BSP_ERROR_WRONG_PARAM;
  }

#if defined(USE_CAMERA_SENSOR_OV5640)
  CameraDrv = (CAMERA_Drv_t *) &OV5640_CAMERA_Driver;
  CompObj = &OV5640Obj;

  ret = CameraDrv->DeInit(CompObj);
  if (ret != OV5640_OK)
  {
    return BSP_ERROR_COMPONENT_FAILURE;
  }
  CompObj = NULL;
#elif defined(USE_CAMERA_SENSOR_IMX219)
  CameraDrv = (CAMERA_Drv_t *) &IMX219_CAMERA_Driver;
  CompObj = &IMX219Obj;
  ret = CameraDrv->DeInit(CompObj);
  if (ret != IMX219_OK)
  {
    return BSP_ERROR_COMPONENT_FAILURE;
  }
  CompObj = NULL;
#elif defined(USE_CAMERA_SENSOR_IMX335)
  CameraDrv = (CAMERA_Drv_t *) &IMX335_CAMERA_Driver;
  CompObj = &IMX335Obj;
  ret = CameraDrv->DeInit(CompObj);
  if (ret != IMX335_OK)
  {
    return BSP_ERROR_COMPONENT_FAILURE;
  }
  CompObj = NULL;
#endif /* USE_CAMERA_SENSOR_OV5640 */

  return BSP_ERROR_NONE;
}

/**
  * @brief Suspend the CAMERA capture
  * @param  Instance Camera instance.
  */
int32_t BSP_CAMERA_Suspend(uint32_t Instance)
{
  if (Instance >= CAMERA_INSTANCES_NBR)
  {
    return BSP_ERROR_WRONG_PARAM;
  }

  return BSP_ERROR_FEATURE_NOT_SUPPORTED;
}

/**
  * @brief Resume the CAMERA capture
  * @param  Instance Camera instance.
  */
int32_t BSP_CAMERA_Resume(uint32_t Instance)
{
  if (Instance >= CAMERA_INSTANCES_NBR)
  {
    return BSP_ERROR_WRONG_PARAM;
  }

  return BSP_ERROR_FEATURE_NOT_SUPPORTED;
}

/**
  * @brief  Get the Camera Capabilities.
  * @param  Instance  Camera instance.
  * @param  Capabilities  pointer to camera Capabilities
  * @note   This function should be called after the init. This to get Capabilities
  *         from the right camera sensor(OV5640)
  * @retval Component status
  */
int32_t BSP_CAMERA_GetCapabilities(uint32_t Instance, CAMERA_Capabilities_t *Capabilities)
{
  int32_t ret;

  if (Instance >= CAMERA_INSTANCES_NBR)
  {
    return BSP_ERROR_WRONG_PARAM;
  }

  ret = CameraDrv->GetCapabilities(CompObj, Capabilities);
  if (ret < 0)
  {
    return BSP_ERROR_COMPONENT_FAILURE;
  }

  return BSP_ERROR_NONE;
}

/**
  * @brief  Set the camera pixel format.
  * @param  Instance  Camera instance.
  * @param  PixelFormat pixel format to be configured
  * @retval BSP status
  */
int32_t BSP_CAMERA_SetPixelFormat(uint32_t Instance, uint32_t PixelFormat)
{
  int32_t ret;

  if (Instance >= CAMERA_INSTANCES_NBR)
  {
    return BSP_ERROR_WRONG_PARAM;
  }

  ret = CameraDrv->SetPixelFormat(CompObj, PixelFormat);
  if (ret < 0)
  {
    return BSP_ERROR_COMPONENT_FAILURE;
  }

  CameraCtx[Instance].PixelFormat = PixelFormat;
  return BSP_ERROR_NONE;
}

/**
  * @brief  Get the camera pixel format.
  * @param  Instance  Camera instance.
  * @param  PixelFormat pixel format to be returned
  * @retval BSP status
  */
int32_t BSP_CAMERA_GetPixelFormat(uint32_t Instance, uint32_t *PixelFormat)
{
  if (Instance >= CAMERA_INSTANCES_NBR)
  {
    return BSP_ERROR_WRONG_PARAM;
  }

  *PixelFormat = CameraCtx[Instance].PixelFormat;
  return BSP_ERROR_NONE;
}


/**
  * @brief  Set the camera Resolution.
  * @param  Instance  Camera instance.
  * @param  Resolution Resolution to be configured
  * @retval BSP status
  */
int32_t BSP_CAMERA_SetResolution(uint32_t Instance, uint32_t Resolution)
{
  int32_t ret;

  if (Instance >= CAMERA_INSTANCES_NBR)
  {
    return BSP_ERROR_WRONG_PARAM;
  }

  if (CameraCap->Resolution == 0U)
  {
    return BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }

  ret = CameraDrv->SetResolution(CompObj, Resolution);
  if (ret < 0)
  {
    return BSP_ERROR_COMPONENT_FAILURE;
  }

  CameraCtx[Instance].Resolution = Resolution;
  return BSP_ERROR_NONE;
}

/**
  * @brief  Get the camera Resolution.
  * @param  Instance  Camera instance.
  * @param  Resolution Resolution to be returned
  * @retval BSP status
  */
int32_t BSP_CAMERA_GetResolution(uint32_t Instance, uint32_t *Resolution)
{
  if (Instance >= CAMERA_INSTANCES_NBR)
  {
    return BSP_ERROR_WRONG_PARAM;
  }

  if (CameraCap->Resolution == 0U)
  {
    return BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }

  *Resolution = CameraCtx[Instance].Resolution;
  return BSP_ERROR_NONE;
}

/**
  * @brief  Set the camera Light Mode.
  * @param  Instance  Camera instance.
  * @param  LightMode Light Mode to be configured
  * @retval BSP status
  */
int32_t BSP_CAMERA_SetLightMode(uint32_t Instance, uint32_t LightMode)
{
  int32_t ret;

  if (Instance >= CAMERA_INSTANCES_NBR)
  {
    return BSP_ERROR_WRONG_PARAM;
  }

  if (CameraCap->LightMode == 0U)
  {
    return BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }

  ret = CameraDrv->SetLightMode(CompObj, LightMode);
  if (ret < 0)
  {
    return BSP_ERROR_COMPONENT_FAILURE;
  }

  CameraCtx[Instance].LightMode = LightMode;
  return BSP_ERROR_NONE;
}

/**
  * @brief  Get the camera Light Mode.
  * @param  Instance  Camera instance.
  * @param  LightMode Light Mode to be returned
  * @retval BSP status
  */
int32_t BSP_CAMERA_GetLightMode(uint32_t Instance, uint32_t *LightMode)
{
  if (Instance >= CAMERA_INSTANCES_NBR)
  {
    return BSP_ERROR_WRONG_PARAM;
  }

  if (CameraCap->LightMode == 0U)
  {
    return BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }

  *LightMode = CameraCtx[Instance].LightMode;
  return BSP_ERROR_NONE;
}

/**
  * @brief  Set the camera Special Effect.
  * @param  Instance Camera instance.
  * @param  ColorEffect Effect to be configured
  * @retval BSP status
  */
int32_t BSP_CAMERA_SetColorEffect(uint32_t Instance, uint32_t ColorEffect)
{
  int32_t ret;

  if (Instance >= CAMERA_INSTANCES_NBR)
  {
    return BSP_ERROR_WRONG_PARAM;
  }

  if (CameraCap->ColorEffect == 0U)
  {
    return BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }

  ret = CameraDrv->SetColorEffect(CompObj, ColorEffect);
  if (ret < 0)
  {
    return BSP_ERROR_COMPONENT_FAILURE;
  }

  CameraCtx[Instance].ColorEffect = ColorEffect;
  return BSP_ERROR_NONE;
}

/**
  * @brief  Get the camera Special Effect.
  * @param  Instance Camera instance.
  * @param  ColorEffect Effect to be returned
  * @retval BSP status
  */
int32_t BSP_CAMERA_GetColorEffect(uint32_t Instance, uint32_t *ColorEffect)
{
  if (Instance >= CAMERA_INSTANCES_NBR)
  {
    return BSP_ERROR_WRONG_PARAM;
  }

  if (CameraCap->ColorEffect == 0U)
  {
    return BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }

  *ColorEffect = CameraCtx[Instance].ColorEffect;
  return BSP_ERROR_NONE;
}

/**
  * @brief  Set the camera Brightness Level.
  * @param  Instance   Camera instance.
  * @param  Brightness Brightness Level
  * @retval BSP status
  */
int32_t BSP_CAMERA_SetBrightness(uint32_t Instance, int32_t Brightness)
{
  int32_t ret;

  if ((Instance >= CAMERA_INSTANCES_NBR) || ((Brightness < CAMERA_BRIGHTNESS_MIN)
                                             && (Brightness > CAMERA_BRIGHTNESS_MAX)))
  {
    return BSP_ERROR_WRONG_PARAM;
  }

  if (CameraCap->Brightness == 0U)
  {
    return BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }

  ret = CameraDrv->SetBrightness(CompObj, Brightness);
  if (ret < 0)
  {
    return BSP_ERROR_COMPONENT_FAILURE;
  }

  CameraCtx[Instance].Brightness = Brightness;
  return BSP_ERROR_NONE;
}

/**
  * @brief  Get the camera Brightness Level.
  * @param  Instance Camera instance.
  * @param  Level    Brightness Level
  * @retval BSP status
  */
int32_t BSP_CAMERA_GetBrightness(uint32_t Instance, int32_t *Brightness)
{
  if (Instance >= CAMERA_INSTANCES_NBR)
  {
    return BSP_ERROR_WRONG_PARAM;
  }

  if (CameraCap->Brightness == 0U)
  {
    return BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }

  *Brightness = CameraCtx[Instance].Brightness;
  return BSP_ERROR_NONE;
}

/**
  * @brief  Set the camera Saturation Level.
  * @param  Instance    Camera instance.
  * @param  Saturation  Saturation Level
  * @retval BSP status
  */
int32_t BSP_CAMERA_SetSaturation(uint32_t Instance, int32_t Saturation)
{
  int32_t ret;

  if ((Instance >= CAMERA_INSTANCES_NBR) || ((Saturation < CAMERA_SATURATION_MIN)
                                             && (Saturation > CAMERA_SATURATION_MAX)))
  {
    return BSP_ERROR_WRONG_PARAM;
  }

  if (CameraCap->Saturation == 0U)
  {
    return BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }

  ret = CameraDrv->SetSaturation(CompObj, Saturation);
  if (ret < 0)
  {
    return BSP_ERROR_COMPONENT_FAILURE;
  }

  CameraCtx[Instance].Saturation = Saturation;
  return BSP_ERROR_NONE;
}

/**
  * @brief  Get the camera Saturation Level.
  * @param  Instance    Camera instance.
  * @param  Saturation  Saturation Level
  * @retval BSP status
  */
int32_t BSP_CAMERA_GetSaturation(uint32_t Instance, int32_t *Saturation)
{
  if (Instance >= CAMERA_INSTANCES_NBR)
  {
    return BSP_ERROR_WRONG_PARAM;
  }

  if (CameraCap->Saturation == 0U)
  {
    return BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }

  *Saturation = CameraCtx[Instance].Saturation;
  return BSP_ERROR_NONE;
}

/**
  * @brief  Set the camera Contrast Level.
  * @param  Instance Camera instance.
  * @param  Contrast Contrast Level
  * @retval BSP status
  */
int32_t BSP_CAMERA_SetContrast(uint32_t Instance, int32_t Contrast)
{
  int32_t ret;

  if ((Instance >= CAMERA_INSTANCES_NBR) || ((Contrast < CAMERA_CONTRAST_MIN)
                                             && (Contrast > CAMERA_CONTRAST_MAX)))
  {
    return BSP_ERROR_WRONG_PARAM;
  }

  if (CameraCap->Contrast == 0U)
  {
    return BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }

  ret = CameraDrv->SetContrast(CompObj, Contrast);
  if (ret < 0)
  {
    return BSP_ERROR_COMPONENT_FAILURE;
  }

  CameraCtx[Instance].Contrast = Contrast;
  return BSP_ERROR_NONE;
}

/**
  * @brief  Get the camera Contrast Level.
  * @param  Instance Camera instance.
  * @param  Contrast Contrast Level
  * @retval BSP status
  */
int32_t BSP_CAMERA_GetContrast(uint32_t Instance, int32_t *Contrast)
{
  if (Instance >= CAMERA_INSTANCES_NBR)
  {
    return BSP_ERROR_WRONG_PARAM;
  }

  if (CameraCap->Contrast == 0U)
  {
    return BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }

  *Contrast = CameraCtx[Instance].Contrast;
  return BSP_ERROR_NONE;
}

/**
  * @brief  Set the camera Hue Degree.
  * @param  Instance   Camera instance.
  * @param  HueDegree  Hue Degree
  * @retval BSP status
  */
int32_t BSP_CAMERA_SetHueDegree(uint32_t Instance, int32_t HueDegree)
{
  int32_t ret;

  if ((Instance >= CAMERA_INSTANCES_NBR) || ((HueDegree < CAMERA_HUEDEGREE_MIN)
                                             && (HueDegree > CAMERA_HUEDEGREE_MAX)))
  {
    return BSP_ERROR_WRONG_PARAM;
  }

  if (CameraCap->HueDegree == 0U)
  {
    return BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }

  ret = CameraDrv->SetHueDegree(CompObj, HueDegree);
  if (ret < 0)
  {
    return BSP_ERROR_COMPONENT_FAILURE;
  }

  CameraCtx[Instance].HueDegree = HueDegree;
  return BSP_ERROR_NONE;
}

/**
  * @brief  Get the camera Hue Degree.
  * @param  Instance   Camera instance.
  * @param  HueDegree  Hue Degree
  * @retval BSP status
  */
int32_t BSP_CAMERA_GetHueDegree(uint32_t Instance, int32_t *HueDegree)
{
  if (Instance >= CAMERA_INSTANCES_NBR)
  {
    return BSP_ERROR_WRONG_PARAM;
  }

  if (CameraCap->HueDegree == 0U)
  {
    return BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }

  *HueDegree = CameraCtx[Instance].HueDegree;
  return BSP_ERROR_NONE;
}

/**
  * @brief  Set the camera Mirror/Flip.
  * @param  Instance  Camera instance.
  * @param  MirrorFlip CAMERA_MIRRORFLIP_NONE or any combination of
  *                    CAMERA_MIRRORFLIP_FLIP and CAMERA_MIRRORFLIP_MIRROR
  * @retval BSP status
  */
int32_t BSP_CAMERA_SetMirrorFlip(uint32_t Instance, uint32_t MirrorFlip)
{
  int32_t ret;

  if (Instance >= CAMERA_INSTANCES_NBR)
  {
    return BSP_ERROR_WRONG_PARAM;
  }

  if (CameraCap->MirrorFlip == 0U)
  {
    return BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }

  ret = CameraDrv->MirrorFlipConfig(CompObj, MirrorFlip);
  if (ret < 0)
  {
    return BSP_ERROR_COMPONENT_FAILURE;
  }

  CameraCtx[Instance].MirrorFlip = MirrorFlip;
  return BSP_ERROR_NONE;
}

/**
  * @brief  Get the camera Mirror/Flip.
  * @param  Instance   Camera instance.
  * @param  MirrorFlip Mirror/Flip config
  * @retval BSP status
  */
int32_t BSP_CAMERA_GetMirrorFlip(uint32_t Instance, uint32_t *MirrorFlip)
{
  if (Instance >= CAMERA_INSTANCES_NBR)
  {
    return BSP_ERROR_WRONG_PARAM;
  }

  if (CameraCap->MirrorFlip == 0U)
  {
    return BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }

  *MirrorFlip = CameraCtx[Instance].MirrorFlip;
  return BSP_ERROR_NONE;
}

/**
  * @brief  Set the camera zoom
  * @param  Instance Camera instance.
  * @param  Zoom     Zoom to be configured
  * @retval BSP status
  */
int32_t BSP_CAMERA_SetZoom(uint32_t Instance, uint32_t Zoom)
{
  int32_t ret;

  if (Instance >= CAMERA_INSTANCES_NBR)
  {
    return BSP_ERROR_WRONG_PARAM;
  }

  if (CameraCap->Zoom == 0U)
  {
    return BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }

  ret = CameraDrv->ZoomConfig(CompObj, Zoom);
  if (ret < 0)
  {
    return BSP_ERROR_COMPONENT_FAILURE;
  }

  CameraCtx[Instance].Zoom = Zoom;
  return BSP_ERROR_NONE;
}

/**
  * @brief  Get the camera zoom
  * @param  Instance Camera instance.
  * @param  Zoom     Zoom to be returned
  * @retval BSP status
  */
int32_t BSP_CAMERA_GetZoom(uint32_t Instance, uint32_t *Zoom)
{
  if (Instance >= CAMERA_INSTANCES_NBR)
  {
    return BSP_ERROR_WRONG_PARAM;
  }

  if (CameraCap->Zoom == 0U)
  {
    return BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }

  *Zoom = CameraCtx[Instance].Zoom;
  return BSP_ERROR_NONE;
}

/**
  * @brief  Enable the camera night mode
  * @param  Instance Camera instance.
  * @retval BSP status
  */
int32_t BSP_CAMERA_EnableNightMode(uint32_t Instance)
{
  int32_t ret;

  if (Instance >= CAMERA_INSTANCES_NBR)
  {
    return BSP_ERROR_WRONG_PARAM;
  }

  if (CameraCap->NightMode == 0U)
  {
    return BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }

  ret = CameraDrv->NightModeConfig(CompObj, CAMERA_NIGHT_MODE_SET);
  if (ret < 0)
  {
    return BSP_ERROR_COMPONENT_FAILURE;
  }

  return BSP_ERROR_NONE;
}

/**
  * @brief  Disable the camera night mode
  * @param  Instance Camera instance.
  * @retval BSP status
  */
int32_t BSP_CAMERA_DisableNightMode(uint32_t Instance)
{
  int32_t ret;

  if (Instance >= CAMERA_INSTANCES_NBR)
  {
    return BSP_ERROR_WRONG_PARAM;
  }

  if (CameraCap->NightMode == 0U)
  {
    return BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }

  ret = CameraDrv->NightModeConfig(CompObj, CAMERA_NIGHT_MODE_RESET);
  if (ret < 0)
  {
    return BSP_ERROR_COMPONENT_FAILURE;
  }

  return BSP_ERROR_NONE;
}

/**
  * @brief  CAMERA hardware reset
  * @param  Instance Camera instance.
  * @retval BSP status
  */
int32_t BSP_CAMERA_HwReset(uint32_t Instance)
{
  UNUSED(Instance);

  int32_t ret = BSP_ERROR_NONE;
  /* Camera sensor RESET sequence */
  GPIO_InitTypeDef  gpio_init_structure;

  /*** Configure the GPIOs ***/
  CAMERA_STBYN_GPIO_CLK_ENABLE();
  CAMERA_RST_GPIO_CLK_ENABLE();

  gpio_init_structure.Pin = CAMERA_STBYN_PIN;
  gpio_init_structure.Mode = GPIO_MODE_OUTPUT_PP;
  gpio_init_structure.Pull = GPIO_NOPULL;
  gpio_init_structure.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(CAMERA_STBYN_GPIO_PORT, &gpio_init_structure);

  gpio_init_structure.Pin = CAMERA_RST_PIN;
  HAL_GPIO_Init(CAMERA_RST_GPIO_PORT, &gpio_init_structure);

#if defined(USE_CAMERA_SENSOR_OV5640)
#if defined(USE_CSI_INTERFACE)
  HAL_GPIO_WritePin(CAMERA_STBYN_GPIO_PORT, CAMERA_STBYN_PIN, GPIO_PIN_SET);
  HAL_Delay(5);
  HAL_GPIO_WritePin(CAMERA_RST_GPIO_PORT, CAMERA_RST_PIN, GPIO_PIN_SET);
  HAL_Delay(25);
#else
  HAL_GPIO_WritePin(CAMERA_STBYN_GPIO_PORT, CAMERA_STBYN_PIN, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(CAMERA_STBYN_GPIO_PORT, CAMERA_STBYN_PIN, GPIO_PIN_SET);
  HAL_GPIO_WritePin(CAMERA_RST_GPIO_PORT, CAMERA_RST_PIN, GPIO_PIN_SET);
  HAL_Delay(100);
  HAL_GPIO_WritePin(CAMERA_STBYN_GPIO_PORT, CAMERA_STBYN_PIN, GPIO_PIN_RESET);
  HAL_Delay(100);
  HAL_GPIO_WritePin(CAMERA_STBYN_GPIO_PORT, CAMERA_STBYN_PIN, GPIO_PIN_SET);
  HAL_Delay(100);
  HAL_GPIO_WritePin(CAMERA_RST_GPIO_PORT, CAMERA_RST_PIN, GPIO_PIN_RESET);
  HAL_Delay(100);
#endif /* USE_CSI_INTERFACE */
#elif defined(USE_CAMERA_SENSOR_IMX219)
  HAL_GPIO_WritePin(CAMERA_STBYN_GPIO_PORT, CAMERA_STBYN_PIN, GPIO_PIN_RESET);
  HAL_Delay(100);
  HAL_GPIO_WritePin(CAMERA_RST_GPIO_PORT, CAMERA_RST_PIN, GPIO_PIN_RESET);
  HAL_Delay(100);
  HAL_GPIO_WritePin(CAMERA_RST_GPIO_PORT, CAMERA_RST_PIN, GPIO_PIN_SET);
  HAL_Delay(100);
#elif defined(USE_CAMERA_SENSOR_IMX335)
  HAL_GPIO_WritePin(CAMERA_STBYN_GPIO_PORT, CAMERA_STBYN_PIN, GPIO_PIN_RESET);
  HAL_Delay(100);
  HAL_GPIO_WritePin(CAMERA_RST_GPIO_PORT, CAMERA_RST_PIN, GPIO_PIN_RESET);
  HAL_Delay(100);
  HAL_GPIO_WritePin(CAMERA_STBYN_GPIO_PORT, CAMERA_STBYN_PIN, GPIO_PIN_SET);
  HAL_Delay(100);
  HAL_GPIO_WritePin(CAMERA_RST_GPIO_PORT, CAMERA_RST_PIN, GPIO_PIN_SET);
  HAL_Delay(100);
#endif /* USE_CAMERA_SENSOR_OV5640 */

  return ret;
}

/**
  * @}
  */

/** @defgroup CAMERA_Private_Functions Private Functions
  * @{
  */

/**
  * @brief  Get the capture size in pixels unit.
  * @param  Resolution  the current resolution.
  * @retval capture size in pixels unit.
  */
int32_t GetSize(uint32_t Resolution, uint32_t PixelFormat)
{
  uint32_t size = 0;
  uint32_t pf_div;
  if (PixelFormat == CAMERA_PF_RGB888)
  {
    pf_div = 3; /* each pixel on 3 bytes so 3/4 words */
  }
  else
  {
    pf_div = 2; /* each pixel on 2 bytes so 1/2 words*/
  }
  /* Get capture size */
  switch (Resolution)
  {
    case CAMERA_R160x120:
      size = ((uint32_t)(160 * 120) * pf_div);
      break;
    case CAMERA_R320x240:
      size = ((uint32_t)(320 * 240) * pf_div);
      break;
    case CAMERA_R480x272:
      size = ((uint32_t)(480 * 272) * pf_div);
      break;
    case CAMERA_R640x480:
      size = ((uint32_t)(640 * 480) * pf_div);
      break;
    case CAMERA_R800x480:
      size = ((uint32_t)(800 * 480) * pf_div);
      break;
    default:
      break;
  }

  return (int32_t)size;
}

/**
  * @}
  */

/** @addtogroup _CAMERA_Private_FunctionPrototypes
  * @{
  */
/**
  * @brief  Register Bus IOs if component ID is OK
  * @retval error status
  */
#if defined(USE_CAMERA_SENSOR_OV5640)
static int32_t OV5640_Probe(uint32_t Resolution, uint32_t PixelFormat)
{
  int32_t ret;
  OV5640_IO_t              IOCtx;
  uint32_t                  id;

  /* Only perform the init if the object already exist */
  if (!CompObj)
  {
    /* Configure the I2C driver */
    IOCtx.Address     = CAMERA_OV5640_ADDRESS;
    IOCtx.Init        = BSP_I2C2_Init;
    IOCtx.DeInit      = BSP_I2C2_DeInit;
    IOCtx.ReadReg     = BSP_I2C2_ReadReg16;
    IOCtx.WriteReg    = BSP_I2C2_WriteReg16;
    IOCtx.GetTick     = BSP_GetTick;

    ret = OV5640_RegisterBusIO(&OV5640Obj, &IOCtx);
    if (ret != OV5640_OK)
    {
      return BSP_ERROR_COMPONENT_FAILURE;
    }

    ret = OV5640_ReadID(&OV5640Obj, &id);
    if (ret != OV5640_OK)
    {
      return BSP_ERROR_COMPONENT_FAILURE;
    }

    if (id != OV5640_ID)
    {
      return BSP_ERROR_UNKNOWN_COMPONENT;
    }

    CameraDrv = (CAMERA_Drv_t *) &OV5640_CAMERA_Driver;
    CompObj = &OV5640Obj;
    CameraCap = &Cap;
  }

  ret = CameraDrv->Init(CompObj, Resolution, PixelFormat);
  if (ret != OV5640_OK)
  {
    return BSP_ERROR_COMPONENT_FAILURE;
  }

  ret = CameraDrv->GetCapabilities(CompObj, CameraCap);
  if (ret != OV5640_OK)
  {
    return BSP_ERROR_COMPONENT_FAILURE;
  }

  return BSP_ERROR_NONE;
}
#endif /* USE_CAMERA_SENSOR_OV5640 */

/**
  * @brief  Register Bus IOs if component ID is OK
  * @retval error status
  */
#if defined(USE_CAMERA_SENSOR_IMX219)
static int32_t IMX219_Probe(uint32_t Resolution, uint32_t PixelFormat)
{
  int32_t ret;
  IMX219_IO_t              IOCtx;
  uint32_t                  id;

  /* Only perform the init if the object already exist */
  if (!CompObj)
  {
    /* Configure the I2C driver */
    IOCtx.Address     = CAMERA_IMX219_ADDRESS;
    IOCtx.Init        = BSP_I2C2_Init;
    IOCtx.DeInit      = BSP_I2C2_DeInit;
    IOCtx.ReadReg     = BSP_I2C2_ReadReg16;
    IOCtx.WriteReg    = BSP_I2C2_WriteReg16;
    IOCtx.GetTick     = BSP_GetTick;

    ret = IMX219_RegisterBusIO(&IMX219Obj, &IOCtx);
    if (ret != IMX219_OK)
    {
      return BSP_ERROR_COMPONENT_FAILURE;
    }

    ret = IMX219_ReadID(&IMX219Obj, &id);
    if (ret != IMX219_OK)
    {
      return BSP_ERROR_COMPONENT_FAILURE;
    }

    if (id != IMX219_CHIP_ID)
    {
      return BSP_ERROR_UNKNOWN_COMPONENT;
    }

    CameraDrv = (CAMERA_Drv_t *) &IMX219_CAMERA_Driver;
    CompObj = &IMX219Obj;
    CameraCap = &Cap;
  }

  ret = CameraDrv->Init(CompObj, Resolution, PixelFormat);
  if (ret != IMX219_OK)
  {
    return BSP_ERROR_COMPONENT_FAILURE;
  }

  ret = CameraDrv->GetCapabilities(CompObj, CameraCap);
  if (ret != IMX219_OK)
  {
    return BSP_ERROR_COMPONENT_FAILURE;
  }

  return BSP_ERROR_NONE;
}
#endif /* USE_CAMERA_SENSOR_IMX219 */

#if defined(USE_CAMERA_SENSOR_IMX335)
static int32_t IMX335_Probe(uint32_t Resolution, uint32_t PixelFormat)
{
  int32_t ret;
  IMX335_IO_t              IOCtx;
  uint32_t                  id;

  /* Only perform the init if the object already exist */
  if (!CompObj)
  {
    /* Configure the I2C driver */
    IOCtx.Address     = CAMERA_IMX335_ADDRESS;
    IOCtx.Init        = BSP_I2C2_Init;
    IOCtx.DeInit      = BSP_I2C2_DeInit;
    IOCtx.ReadReg     = BSP_I2C2_ReadReg16;
    IOCtx.WriteReg    = BSP_I2C2_WriteReg16;
    IOCtx.GetTick     = BSP_GetTick;

    ret = IMX335_RegisterBusIO(&IMX335Obj, &IOCtx);
    if (ret != IMX335_OK)
    {
      return BSP_ERROR_COMPONENT_FAILURE;
    }

    ret = IMX335_ReadID(&IMX335Obj, &id);
    if (ret != IMX335_OK)
    {
      return BSP_ERROR_COMPONENT_FAILURE;
    }

    if (id != IMX335_CHIP_ID)
    {
      return BSP_ERROR_UNKNOWN_COMPONENT;
    }

    CameraDrv = (CAMERA_Drv_t *) &IMX335_CAMERA_Driver;
    CompObj = &IMX335Obj;
    CameraCap = &Cap;
  }

  ret = CameraDrv->Init(CompObj, Resolution, PixelFormat);
  if (ret != IMX335_OK)
  {
    return BSP_ERROR_COMPONENT_FAILURE;
  }

  ret = CameraDrv->GetCapabilities(CompObj, CameraCap);
  if (ret != IMX335_OK)
  {
    return BSP_ERROR_COMPONENT_FAILURE;
  }

  return BSP_ERROR_NONE;
}
#endif /* USE_CAMERA_SENSOR_IMX335 */

int32_t BSP_CAMERA_SetColorbarMode(uint32_t Instance, uint32_t Mode)
{
  UNUSED(Instance);

#if defined(USE_CAMERA_SENSOR_OV5640)
  if (OV5640_ColorbarModeConfig(CompObj, Mode) != OV5640_OK)
  {
    return BSP_ERROR_COMPONENT_FAILURE;
  }

  return BSP_ERROR_NONE;
#else
  return BSP_ERROR_FEATURE_NOT_SUPPORTED;
#endif /* USE_CAMERA_SENSOR_OV5640 */
}

int32_t BSP_CAMERA_SetPolarities(uint32_t Instance, uint32_t Pclk, uint32_t Href, uint32_t Vsync)
{
  UNUSED(Instance);

#if defined(USE_CAMERA_SENSOR_OV5640)
  if (OV5640_SetPolarities(CompObj,
                           Pclk ? OV5640_POLARITY_PCLK_HIGH : OV5640_POLARITY_PCLK_LOW,
                           Href ? OV5640_POLARITY_HREF_HIGH : OV5640_POLARITY_HREF_LOW,
                           Vsync ? OV5640_POLARITY_VSYNC_HIGH : OV5640_POLARITY_VSYNC_LOW) != OV5640_OK)
  {
    return BSP_ERROR_COMPONENT_FAILURE;
  }

  return BSP_ERROR_NONE;
#else
  return BSP_ERROR_FEATURE_NOT_SUPPORTED;
#endif /* USE_CAMERA_SENSOR_OV5640 */
}

int32_t BSP_CAMERA_SetSynchroCodes(uint32_t Instance, uint32_t FSC, uint32_t LSC, uint32_t LEC, uint32_t FEC)
{
  UNUSED(Instance);

#if defined(USE_CAMERA_SENSOR_OV5640)
  OV5640_SyncCodes_t SyncCode;

  SyncCode.FrameStartCode = FSC;
  SyncCode.LineStartCode = LSC;
  SyncCode.LineEndCode = LEC;
  SyncCode.FrameEndCode = FEC;

  if (OV5640_EmbeddedSynchroConfig(CompObj, &SyncCode) != OV5640_OK)
  {
    return BSP_ERROR_COMPONENT_FAILURE;
  }

  return BSP_ERROR_NONE;
#else
  return BSP_ERROR_FEATURE_NOT_SUPPORTED;
#endif /* USE_CAMERA_SENSOR_OV5640 */
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
