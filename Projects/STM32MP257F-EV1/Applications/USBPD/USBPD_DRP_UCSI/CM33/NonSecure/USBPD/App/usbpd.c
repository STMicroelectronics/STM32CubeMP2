/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usbpd.c
  * @author  MCD Application Team
  * @brief   This file contains the device define.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "usbpd.h"

/* USER CODE BEGIN 0 */
/* USER CODE END 0 */

/* USER CODE BEGIN 1 */
/* USER CODE END 1 */

/* Private variables ---------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/* USER CODE BEGIN 2 */
/* USER CODE END 2 */
unsigned int USBPD_PreInitOs(void)
{
  /* Global Init of USBPD HW */
  USBPD_HW_IF_GlobalHwInit();
 /* Initialize the Device Policy Manager */
  if (USBPD_OK != USBPD_DPM_InitCore())
  {
    return USBPD_ERROR;
  }

  return USBPD_OK;
}

/* USBPD init function */
unsigned int MX_USBPD_Init(void *memory_ptr)
{
  unsigned int result = USBPD_OK;

  /* Initialize the DPM application */
  if (USBPD_OK != USBPD_DPM_UserInit((void*)memory_ptr))
  {
    return USBPD_ERROR;
  }

  /* USER CODE BEGIN 3 */
  /* USER CODE END 3 */

#if defined(USBPD_THREADX)
  if (USBPD_OK != USBPD_DPM_InitOS((void*)memory_ptr))
#else
  if (USBPD_OK != USBPD_DPM_InitOS())
#endif /* USBPD_THREADX */

  {
    return USBPD_ERROR;
  }
  return result;
}

/* USBPD Deinit function */
void MX_USBPD_DeInit(void)
{
  USBPD_DPM_DeInitOS();
  USBPD_DPM_DeInitCore();
}

/* USER CODE BEGIN 4 */
/* USER CODE END 4 */

/**
  * @}
  */

/**
  * @}
  */
