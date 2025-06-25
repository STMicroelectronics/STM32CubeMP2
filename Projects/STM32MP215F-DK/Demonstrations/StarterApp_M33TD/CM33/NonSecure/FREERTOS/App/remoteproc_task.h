/**
  ******************************************************************************
  * @file    remoteproc_task.h
  * @author  MCD Application Team
  * @brief   Header file for remote processor task implementation.
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
#ifndef REMOTEPROC_TASK_H
#define REMOTEPROC_TASK_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "cmsis_os2.h"
#include <stdint.h>
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/**
  * @brief  Enumeration for remote processor actions.
  */
typedef enum {
  REMOTEPROC_ACTION_START, /*!< Start the remote processor */
  REMOTEPROC_ACTION_STOP,  /*!< Stop the remote processor */
  REMOTEPROC_ACTION_CRASH, /*!< Handle a crash of the remote processor */
} RemoteProcAction;

/**
  * @brief  Structure for remote processor commands.
  */
typedef struct {
  RemoteProcAction action; /*!< Action to perform */
  uint32_t cpu_id;         /*!< ID of the remote processor */
} RemoteProcCmd;

/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
/**
  * @brief  Initialize the remote processor task and its resources.
  * @retval None
  */
void RemoteProcTask_Init(void);

/**
  * @brief  De-initialize the remote processor task and release its resources.
  * @retval None
  */
void RemoteProcTask_DeInit(void);

/**
  * @brief  Post a command to the remote processor task.
  * @param  cmd: Pointer to the command structure to enqueue.
  * @retval HAL_OK if the command was successfully enqueued, HAL_ERROR otherwise.
  */
HAL_StatusTypeDef RemoteProc_PostCmd(const RemoteProcCmd *cmd);

/**
  * @brief  Get a copy of the remote processor information structure.
  * @retval A copy of the `cpu_info_res` structure.
  */
struct cpu_info_res RemoteProc_GetCoproInfo(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

#ifdef __cplusplus
}
#endif

#endif /* REMOTEPROC_TASK_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
