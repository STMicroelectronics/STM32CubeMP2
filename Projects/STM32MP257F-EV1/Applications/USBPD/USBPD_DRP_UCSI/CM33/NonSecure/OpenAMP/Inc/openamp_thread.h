/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    openamp_thread.h
  * @author  MCD Application Team
  * @brief   Thread Openamp header file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __OPENAMP_THREAD_H
#define __OPENAMP_THREAD_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
 /* Exported functions prototypes ---------------------------------------------*/
 int Thread_Openamp_Init(osSemaphoreId_t EventSemaphore);
 int Thread_Openamp_DeInit();

 /* USER CODE BEGIN EFP */

 /* USER CODE END EFP */

 /* Private defines -----------------------------------------------------------*/
 /* USER CODE BEGIN PD */

 /* USER CODE END PD */

 /* USER CODE BEGIN 1 */

 /* USER CODE END 1 */

#ifdef __cplusplus
}
#endif
#endif /* __OPENAMP_THREAD_H__ */