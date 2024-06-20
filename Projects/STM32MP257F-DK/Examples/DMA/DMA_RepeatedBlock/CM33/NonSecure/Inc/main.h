/**
  ******************************************************************************
  * @file    main.h
  * @author  MCD Application Team
  * @brief   Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32mp2xx_hal.h"
#include "stm32mp257f_disco.h"
#include <string.h>

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
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines ------------------------------------------------------------*/
#define DEFAULT_IRQ_PRIO      1U

/* USER CODE BEGIN Private defines */
/* Source buffers declaration */
#define SRC_BUFFER_SIZE        (48U)  /* In Word */

#define GATHER_BUFFER_SIZE     (16U)  /* In Word */
#define GATHER_BLOCK_SIZE      (16U)  /* In Byte */
#define GATHER_BLOCK_OFFSET    (32U)  /* In Byte */
#define GATHER_BLOCK_NUM        (4U)

#define SCATTER_BUFFER_SIZE    (48U)  /* In Word */
#define SCATTER_BLOCK_SIZE     (16U)  /* In Byte */
#define SCATTER_BLOCK_OFFSET   (32U)  /* In Byte */
#define SCATTER_BLOCK_NUM       (4U)
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

