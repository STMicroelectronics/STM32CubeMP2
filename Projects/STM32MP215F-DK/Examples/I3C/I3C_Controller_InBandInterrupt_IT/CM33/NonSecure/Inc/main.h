/**
  ******************************************************************************
  * @file    main.h
  * @author  MCD Application Team
  * @brief   Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32mp2xx_hal.h"

/* USER CODE BEGIN Includes */
#include "stm32mp215f_disco.h"
#include "stm32mp215f_disco_stpmic2l.h"

/* USER CODE END Includes */
/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
/* Target descriptor */
typedef struct
{
  char *        TARGET_NAME;          /*!< Marketing Target reference */
  uint32_t      TARGET_ID;            /*!< Target Identifier on the Bus */
  uint64_t      TARGET_BCR_DCR_PID;   /*!< Concatenation value of PID, BCR and DCR of the target */
  uint8_t       STATIC_ADDR;          /*!< Static Address of the target, value found in the datasheet of the device */
  uint8_t       DYNAMIC_ADDR;         /*!< Dynamic Address of the target preset by software/application */
} TargetDesc_TypeDef;

/* BCR descriptor */
typedef struct
{
  uint8_t      DCR;     /*!< Device Caracteristics Register */
  uint8_t      BCR;     /*!< Bus Caracteristics Register */
  uint64_t     PID;     /*!< Provisioned Identifier */
} PAYLOAD_DESC_t;
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
#define COUNTOF(__BUFFER__)   (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */
/* Private defines ------------------------------------------------------------*/
#define DEFAULT_IRQ_PRIO      1U

/* ########################## Assert Selection ############################## */
/**
  * @brief Uncomment the line below to expanse the "assert_param" macro in the 
  *        HAL drivers code
  */
/* #define USE_FULL_ASSERT    1U */

/* USER CODE BEGIN Private defines */
/* Define Target Identifier */
#define DEVICE_ID1      1U
#define DEVICE_ID2      2U
/* USER CODE END Private defines */

#ifdef __cplusplus
  extern "C" {
#endif
#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

