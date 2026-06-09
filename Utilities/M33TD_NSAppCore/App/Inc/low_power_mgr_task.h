/**
  ******************************************************************************
  * @file    low_power_mgr_task.h
  * @author  MCD Application Team
  * @brief   Header file for Low Power Manager Task implementation.
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

#ifndef LOW_POWER_MGR_TASK_H
#define LOW_POWER_MGR_TASK_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stddef.h>
#include <stdint.h>

#include "nsappcore_config.h"

struct rpmsg_endpoint;

/**
  * @brief  Message string to disable low-power entry through RPMsg/OpenAMP.
  */
#define LOW_POWER_MGR_CMD_LIMIT_PM_DISABLED   "LIMIT_PM_DISABLED"

/**
  * @brief  Message string to limit low-power mode to STOP2 through RPMsg/OpenAMP.
  */
#define LOW_POWER_MGR_CMD_LIMIT_PM_STOP2       "LIMIT_PM_STOP2"

/**
  * @brief  Message string to limit low-power mode to LP_STOP2 through RPMsg/OpenAMP.
  */
#define LOW_POWER_MGR_CMD_LIMIT_PM_LP_STOP2    "LIMIT_PM_LP_STOP2"

/**
  * @brief  Message string to limit low-power mode to LPLV_STOP2 through RPMsg/OpenAMP.
  */
#define LOW_POWER_MGR_CMD_LIMIT_PM_LPLV_STOP2  "LIMIT_PM_LPLV_STOP2"

/**
  * @brief  Message string to restore the default standby policy through RPMsg/OpenAMP.
  */
#define LOW_POWER_MGR_CMD_LIMIT_PM_STANDBY     "LIMIT_PM_STANDBY"

/**
  * @brief  Supported low power modes ordered from shallowest to deepest.
  */
typedef enum {
    LOW_POWER_MGR_MODE_STOP2 = 0,    /*!< STOP2 low power mode */
    LOW_POWER_MGR_MODE_LP_STOP2,     /*!< Low-power STOP2 mode */
    LOW_POWER_MGR_MODE_LPLV_STOP2,   /*!< Low-power low-voltage STOP2 mode */
    LOW_POWER_MGR_MODE_STANDBY1,     /*!< STANDBY1 low power mode */
    LOW_POWER_MGR_MODE_STANDBY2,     /*!< STANDBY2 low power mode */
    LOW_POWER_MGR_MODE_NONE          /*!< Low power entry disabled */
} LowPowerMgrMode_t;

/**
  * @brief  Opaque handle for a registered low power agent.
  */
typedef uint8_t LowPowerMgrAgentHandle_t;

/**
  * @brief  Invalid low power agent handle value.
  */
#define LOW_POWER_MGR_AGENT_HANDLE_INVALID ((LowPowerMgrAgentHandle_t)0U)

/**
  * @brief  Low Power Manager event types.
  */
typedef enum {
    LOW_POWER_MGR_EVENT_SUSPEND = 0,          /*!< External: request low power suspend sequence */
    LOW_POWER_MGR_EVENT_SHUTDOWN,             /*!< External: request one-way shutdown sequence */
    LOW_POWER_MGR_EVENT_PREPARE_SLEEP,        /*!< Internal: run PREPARING phase */
    LOW_POWER_MGR_EVENT_ENTER_SLEEP,          /*!< Internal: run ENTERING_SLEEP phase */
    LOW_POWER_MGR_EVENT_RESUME                /*!< Internal: run RESUMING phase */
} LowPowerMgrEvent_t;

/**
  * @brief  Low Power Manager notif types.
  */
typedef enum {
    LOW_POWER_MGR_NOTIF_PREPARE_STOP = 0,    /*!< Notif: prepare for STOP-class modes */
    LOW_POWER_MGR_NOTIF_PREPARE_STANDBY,     /*!< Notif: prepare for STANDBY-class modes */
    LOW_POWER_MGR_NOTIF_RESUME_STOP,         /*!< Notif: resume from STOP-class modes */
    LOW_POWER_MGR_NOTIF_RESUME_STANDBY       /*!< Notif: resume from STANDBY-class modes */
} LowPowerMgrNotif_t;

/**
  * @brief  Low Power Manager notif listener callback type.
  * @param  notif: Notif type.
  * @param  mode: Active or candidate low power mode associated with the event.
  * @param  context: User context pointer.
  * @retval None
  */
typedef void (*LowPowerMgrNotifListener)(LowPowerMgrNotif_t notif,
                                         LowPowerMgrMode_t mode,
                                         void *context);

/**
  * @brief  Initialize the Low Power Manager Task and its resources.
  * @retval None
  */
void LowPowerMgrTask_Init(void);

/**
  * @brief  De-initialize the Low Power Manager Task and release its resources.
  * @retval None
  */
void LowPowerMgrTask_DeInit(void);

/**
  * @brief  Post an event to the Low Power Manager Task queue.
  * @param  event: Event type.
  * @param  param: Event parameter.
  * @retval HAL_OK if posted, HAL_ERROR otherwise.
  */
HAL_StatusTypeDef LowPowerMgrTask_PostEvent(LowPowerMgrEvent_t event, uint32_t param);

/**
  * @brief  Register a runtime low power agent.
  * @param  label: Optional static label used for logs, may be NULL.
  * @retval Valid agent handle on success, LOW_POWER_MGR_AGENT_HANDLE_INVALID otherwise.
  */
LowPowerMgrAgentHandle_t LowPowerMgrTask_RegisterAgent(const char *label);

/**
  * @brief  Unregister a runtime low power agent.
  * @param  agent: Agent handle returned by LowPowerMgrTask_RegisterAgent().
  * @retval HAL_OK if unregistered, HAL_ERROR otherwise.
  */
HAL_StatusTypeDef LowPowerMgrTask_UnregisterAgent(LowPowerMgrAgentHandle_t agent);

/**
  * @brief  Create or update a low power constraint for an agent.
  * @param  agent: Agent handle returned by LowPowerMgrTask_RegisterAgent().
  * @param  mode: Requested constraint mode, including LOW_POWER_MGR_MODE_NONE.
  *               LOW_POWER_MGR_MODE_STANDBY2 is private to SHUTDOWN flow and rejected.
  * @retval HAL_OK if updated, HAL_ERROR otherwise.
  */
HAL_StatusTypeDef LowPowerMgrTask_SetAgentConstraint(LowPowerMgrAgentHandle_t agent, LowPowerMgrMode_t mode);

/**
  * @brief  Clear the active low power constraint for an agent.
  * @param  agent: Agent handle returned by LowPowerMgrTask_RegisterAgent().
  * @retval HAL_OK if cleared, HAL_ERROR otherwise.
  */
HAL_StatusTypeDef LowPowerMgrTask_ClearAgentConstraint(LowPowerMgrAgentHandle_t agent);

/**
  * @brief  Register a listener for low power notifications.
  * @param  cb: Listener callback.
  * @param  context: User context pointer.
  * @retval HAL_OK if registered, HAL_ERROR otherwise.
  */
HAL_StatusTypeDef LowPowerMgrTask_RegisterListener(LowPowerMgrNotifListener cb, void *context);

/**
  * @brief  Unregister a low power listener.
  * @param  cb: Listener callback to remove.
  * @retval HAL_OK if unregistered, HAL_ERROR otherwise.
  */
HAL_StatusTypeDef LowPowerMgrTask_UnregisterListener(LowPowerMgrNotifListener cb);

/**
  * @brief  RPMsg reception callback for the Low Power Manager endpoint.
  *
  * Expected messages:
   * - LOW_POWER_MGR_CMD_LIMIT_PM_DISABLED
  * - LOW_POWER_MGR_CMD_LIMIT_PM_STOP2
  * - LOW_POWER_MGR_CMD_LIMIT_PM_LP_STOP2
  * - LOW_POWER_MGR_CMD_LIMIT_PM_LPLV_STOP2
   * - LOW_POWER_MGR_CMD_LIMIT_PM_STANDBY
  */
int LowPowerMgrTask_RpmsgCallback(struct rpmsg_endpoint *ept, void *data,
                                  size_t len, uint32_t src, void *priv);

#ifdef __cplusplus
}
#endif

#endif /* LOW_POWER_MGR_TASK_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
