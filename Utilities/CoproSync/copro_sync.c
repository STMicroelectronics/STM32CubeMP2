/**
  ******************************************************************************
  * @file    copro_sync.c
  * @author  MCD Application Team
  * @date    08-October-2018
  * @brief   This file provides services to handle synchronization between the
  *          processors.
  *
  ******************************************************************************
  *
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "copro_sync.h"

/** @addtogroup Utilities
  * @{
  */


/** @defgroup COPRO_SYNC_Private_Types
* @{
*/


/**
* @}
*/


/** @defgroup COPRO_SYNC_Private_Defines
* @{
*/


/**
* @}
*/


/** @defgroup COPRO_SYNC_Private_Macros
* @{
*/
#define COPRO_SYNC_SHUTDOWN_CHANNEL  IPCC_CHANNEL_3

/**
* @}
*/


/** @defgroup COPRO_SYNC_Private_Variables
* @{
*/
extern IPCC_HandleTypeDef hipcc;

/**
* @}
*/


/** @defgroup COPRO_SYNC_Private_FunctionPrototypes
* @{
*/


/**
* @}
*/


/** @defgroup COPRO_SYNC_Private_Functions
* @{
*/

/**
  * @brief  Initializes the Copro Sync utility
  * @param  None
  * @retval Return status
  */
CoproSync_Status_t CoproSync_Init(void)
{
  CoproSync_Status_t ret = COPROSYNC_OK;

  if (HAL_IPCC_ActivateNotification(&hipcc, COPRO_SYNC_SHUTDOWN_CHANNEL, IPCC_CHANNEL_DIR_RX,
          CoproSync_ShutdownCb) != HAL_OK)
    ret = COPROSYNC_ERROR;

  return ret;
}

/**
  * @brief  DeInitializes the Copro Sync utility
  * @param  None
  * @retval Return status
  */
CoproSync_Status_t CoproSync_DeInit(void)
{
  CoproSync_Status_t ret = COPROSYNC_OK;

  if (HAL_IPCC_DeActivateNotification(&hipcc, COPRO_SYNC_SHUTDOWN_CHANNEL, IPCC_CHANNEL_DIR_RX) != HAL_OK)
    ret = COPROSYNC_ERROR;

  return ret;
}

/**
  * @brief  Callback from IPCC Interrupt Handler: Remote Processor asks local processor to shutdown
  * @param  hipcc IPCC handle
  * @param  ChannelIndex Channel number
  * @param  ChannelDir Channel direction
  * @retval None
  */
__weak void CoproSync_ShutdownCb(IPCC_HandleTypeDef * hipcc, uint32_t ChannelIndex, IPCC_CHANNELDirTypeDef ChannelDir)
{
  /* When ready, notify the remote processor that we can be shut down */
  HAL_IPCC_NotifyCPU(hipcc, ChannelIndex, IPCC_CHANNEL_DIR_RX);

}

/**
  * @}
  */

