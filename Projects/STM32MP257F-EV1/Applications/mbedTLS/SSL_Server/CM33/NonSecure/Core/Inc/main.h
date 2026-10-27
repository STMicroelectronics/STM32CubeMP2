/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    CM33/NonSecure/Core/Inc/main.h
  * @author  MCD Application Team
  * @brief   Main application header file.
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
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#define USE_DHCP

/* Includes ------------------------------------------------------------------*/
#include "stm32mp2xx_hal.h"
#include "stm32mp2xx_hal_timebase_tim.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "stm32mp257f_eval.h"
#include "stm32mp257f_eval_pmic.h"
#include "stm32mp257f_eval_bus.h"

/* USER CODE END Includes */
#define SERVER_PORT "4433"

#define HTTP_RESPONSE \
    "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n" \
    "<h2>mbed TLS Test Server</h2>\r\n" \
    "<p>Successful connection using: %s</p>\r\n"
#ifdef MBEDTLS_MEMORY_BUFFER_ALLOC_C
#define MAX_MEM_SIZE 80 * 1024
#endif
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
/* USER CODE BEGIN EFP */
void SSL_Server(void *argument);
void Error_Handler(void);
void HAL_DeInitTick(void);
void Success_Handler(void);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define DEFAULT_IRQ_PRIO      1U
/* USER CODE BEGIN Private defines */
extern IPCC_HandleTypeDef hipcc1;
/* USER CODE END Private defines */

#endif /*__MAIN_H */
