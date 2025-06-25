/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    mbox_scmi.h
  * @author  MCD Application Team
  * @brief   SCMI Request thru mailbox header file
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
#ifndef __MBOX_SCMI_H
#define __MBOX_SCMI_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/


/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "mbox_scmi_conf.h"
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
int  MAILBOX_SCMI_Init   (void);
void MAILBOX_SCMI_DeInit (void);
int  MAILBOX_SCMI_Req    (void);
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#if defined(NO_PRINTF)
  #define loc_printf(...)
#else
  #define loc_printf(mess, ...)   printf(mess, ##__VA_ARGS__)
#endif

/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

#ifdef __cplusplus
}
#endif
#endif /* __MBOX_SCMI_H */
