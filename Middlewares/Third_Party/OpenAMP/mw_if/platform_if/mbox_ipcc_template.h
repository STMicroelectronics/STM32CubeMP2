/**
 ******************************************************************************
 * @file    mbox_ipcc.h
 * @author  MCD Application Team
 * @brief   Header for mbox_ipcc.c module
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2019-2026 STMicroelectronics.
 * All rights reserved.
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *     opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */

#ifndef MBOX_IPCC_H_
#define MBOX_IPCC_H_

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */
#include "openamp/open_amp.h"
#if defined(STM32MP157Cxx)
#include "stm32mp1xx_hal.h"
#elif defined(STM32MP25xxxx) || defined(STM32MP2)
#include "stm32mp2xx_hal.h"
#endif

/* Exported types ------------------------------------------------------------*/
typedef void (*MAILBOX_ChannelCallback_t)(IPCC_HandleTypeDef *hipcc,
                                          uint32_t ChannelIndex,
                                          IPCC_CHANNELDirTypeDef ChannelDir);
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN Private defines */

/* USER CODE END  Private defines */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

int MAILBOX_Notify(void *priv, uint32_t id);
int MAILBOX_Init(IPCC_HandleTypeDef *hipcc, MAILBOX_ChannelCallback_t cb);
void MAILBOX_DeInit(IPCC_HandleTypeDef *hipcc);
int MAILBOX_Poll(struct virtio_device *vdev);

#endif /* MBOX_IPCC_H_ */
