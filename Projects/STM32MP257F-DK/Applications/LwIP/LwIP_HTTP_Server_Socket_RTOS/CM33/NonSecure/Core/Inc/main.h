/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
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
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32mp2xx_hal.h"
#include "stm32mp2xx_hal_timebase_tim.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "stm32mp257f_disco.h"
#include "stm32mp257f_disco_pmic.h"
#include "stm32mp257f_disco_bus.h"

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
/*Static IP ADDRESS*/
#define IP_ADDR0   			((uint8_t)10U)
#define IP_ADDR1   			((uint8_t)199U)
#define IP_ADDR2   			((uint8_t)84U)
#define IP_ADDR3   			((uint8_t)230U)

/*NETMASK*/
#define NETMASK_ADDR0   ((uint8_t)255U)
#define NETMASK_ADDR1   ((uint8_t)255U)
#define NETMASK_ADDR2   ((uint8_t)255U)
#define NETMASK_ADDR3   ((uint8_t)0U)

/*Gateway Address*/
#define GW_ADDR0   			((uint8_t)10U)
#define GW_ADDR1   			((uint8_t)199U)
#define GW_ADDR2   			((uint8_t)0U)
#define GW_ADDR3   			((uint8_t)1U)

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
/* USER CODE BEGIN EFP */
void Error_Handler(void);
void HAL_DeInitTick(void);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define DEFAULT_IRQ_PRIO      1U
/* USER CODE BEGIN Private defines */
extern IPCC_HandleTypeDef hipcc1;
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

