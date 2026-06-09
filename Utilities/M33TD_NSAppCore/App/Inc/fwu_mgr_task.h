/**
  ******************************************************************************
  * @file    fwu_mgr_task.h
  * @author  MPU Embedded Software
  * @brief   Firmware Update Manager task public API.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#ifndef FWU_MGR_TASK_H
#define FWU_MGR_TASK_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "nsappcore_config.h"
#include <devicetree.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "psa/update.h"

struct rpmsg_endpoint;

/**
  * @brief  FWU Manager command identifiers (must match Linux side).
  */
typedef enum
{
  FWU_MGR_CMD_INIT    = 0U, /**< Initialize cached FWU state. */
  FWU_MGR_CMD_LIST    = 1U, /**< Return the number of supported FWU components. */
  FWU_MGR_CMD_INFO    = 2U, /**< Query FWU version/state information for one or all components. */
  FWU_MGR_CMD_CANCEL  = 3U, /**< Clear pending update state for one or all components. */
  FWU_MGR_CMD_INSTALL = 4U, /**< Mark one or all components for installation. */
  FWU_MGR_CMD_REBOOT  = 5U, /**< Apply pending images, request A35 shutdown, then reboot the platform. */
  FWU_MGR_CMD_ACCEPT  = 6U, /**< Accept the currently booted FWU trial images. */
  FWU_MGR_CMD_REJECT  = 7U, /**< Reject the currently booted FWU trial images. */
} FwuMgrCommandId_t;

/** Special FWU component identifier values. */
#define FWU_COMPONENT_ID_ALL      (0xFFFFFFFFUL)
#define FWU_COMPONENT_ID_UNKNOWN  (0xFFFFFFFEUL)

#if TFM_CONFIG_FWU_STORAGE_ACCESS
/**
  * @brief  Base address of the FWU staging shared memory.
  * @note   The staging layout is sourced from TF-M generated devicetree data.
  */
#define SHARED_MEM_BASE           ((uintptr_t)DT_REG_ADDR(DT_NODELABEL(fwu_shmem)))
/** @brief Size of the secure + non-secure FWU staging slot. */
#define SHARED_TFM_S_NS_SIZE      ((size_t)DT_REG_SIZE(DT_NODELABEL(tfm_primary_partition)))
/** @brief Size of the DDR firmware FWU staging slot. */
#define SHARED_DDR_FW_SIZE        ((size_t)DT_REG_SIZE(DT_NODELABEL(ddr_fw_primary_partition)))

#if (FWU_COMPONENT_NUMBER == 3)
/** @brief Size of the CA35 firmware FWU staging slot in the 3-component profile. */
#define SHARED_CA35_FW_SIZE       ((size_t)DT_REG_SIZE(DT_NODELABEL(ca35_fw_primary_partition)))
#endif

/** @brief Offset of the TF-M secure + non-secure staging slot. */
#define SHARED_TFM_S_NS_OFFSET    (0x0UL)
/** @brief Offset of the DDR firmware staging slot. */
#define SHARED_DDR_FW_OFFSET      ((uintptr_t)SHARED_TFM_S_NS_SIZE)

#if (FWU_COMPONENT_NUMBER == 3)
/** @brief Offset of the CA35 firmware staging slot in the 3-component profile. */
#define SHARED_CA35_FW_OFFSET     ((uintptr_t)(SHARED_DDR_FW_OFFSET + SHARED_DDR_FW_SIZE))
#endif

/** @brief Runtime address of the TF-M secure + non-secure staging slot. */
#define SHARED_TFM_S_NS           (SHARED_MEM_BASE + SHARED_TFM_S_NS_OFFSET)
/** @brief Runtime address of the DDR firmware staging slot. */
#define SHARED_DDR_FW             (SHARED_MEM_BASE + SHARED_DDR_FW_OFFSET)

#if (FWU_COMPONENT_NUMBER == 3)
/** @brief Runtime address of the CA35 firmware staging slot in the 3-component profile. */
#define SHARED_CA35_FW            (SHARED_MEM_BASE + SHARED_CA35_FW_OFFSET)
#endif

/**
  * @brief  Resolve a FWU component ID to its staging address.
  * @param  id FWU component identifier.
  * @retval Base address of the staging slot, or `NULL` if unsupported.
  */
#if (FWU_COMPONENT_NUMBER == 3)
#define FWU_COMPONENT_ADDR(id) \
    (((id) == FWU_COMPONENT_ID_TFM_S_NS)      ? (void *)(SHARED_TFM_S_NS) : \
     ((id) == FWU_COMPONENT_ID_DDR_FIRMWARE)  ? (void *)(SHARED_DDR_FW)   : \
     ((id) == FWU_COMPONENT_ID_CA35_FIRMWARE) ? (void *)(SHARED_CA35_FW)  : \
                                              NULL)
#else
#define FWU_COMPONENT_ADDR(id) \
    (((id) == FWU_COMPONENT_ID_TFM_S_NS)      ? (void *)(SHARED_TFM_S_NS) : \
     ((id) == FWU_COMPONENT_ID_DDR_FIRMWARE)  ? (void *)(SHARED_DDR_FW)   : \
                                              NULL)
#endif

/**
  * @brief  Resolve a FWU component ID to its staging size.
  * @param  id FWU component identifier.
  * @retval Size of the staging slot in bytes, or `0U` if unsupported.
  */
#if (FWU_COMPONENT_NUMBER == 3)
#define FWU_COMPONENT_SIZE(id) \
    (((id) == FWU_COMPONENT_ID_TFM_S_NS)      ? (SHARED_TFM_S_NS_SIZE) : \
     ((id) == FWU_COMPONENT_ID_DDR_FIRMWARE)  ? (SHARED_DDR_FW_SIZE)   : \
     ((id) == FWU_COMPONENT_ID_CA35_FIRMWARE) ? (SHARED_CA35_FW_SIZE)  : \
                                              0U)
#else
#define FWU_COMPONENT_SIZE(id) \
    (((id) == FWU_COMPONENT_ID_TFM_S_NS)      ? (SHARED_TFM_S_NS_SIZE) : \
     ((id) == FWU_COMPONENT_ID_DDR_FIRMWARE)  ? (SHARED_DDR_FW_SIZE)   : \
                                              0U)
#endif
#endif /* TFM_CONFIG_FWU_STORAGE_ACCESS */


/**
  * @brief  FWU Manager message payload shared by the task queue and RPMsg transport.
  */
typedef struct
{
  uint32_t command;                    /**< FWU_MGR_CMD_xxx command identifier. */
  uint32_t componentId;                /**< FWU component identifier or FWU_COMPONENT_ID_ALL. */
  psa_status_t status;                 /**< PSA status associated with the command response. */
  struct psa_fwu_image_version_t info; /**< FWU component version information for INFO responses. */
} FwuMgrMessage_t;

/**
  * @brief  Initialize the FWU Manager task and its resources.
  *         Creates the FWU Manager thread with attributes defined in the module.
  * @retval None
  */
void FwuMgrTask_Init(void);

/**
  * @brief  Deinitialize the FWU Manager task and release resources.
  *         Terminates the FWU Manager thread and releases allocated resources.
  * @retval None
  */
void FwuMgrTask_DeInit(void);

/**
  * @brief  Post a FWU message to the FWU Manager task queue.
  * @param  msg: Pointer to the message to post.
  * @retval true if posted successfully, false otherwise.
  */
bool FwuMgrTask_PostCommand(const FwuMgrMessage_t *msg);

/**
  * @brief  RPMsg reception callback for the FWU endpoint.
  * @param  ept:  Local RPMsg endpoint.
  * @param  data: Pointer to received message buffer.
  * @param  len:  Length of received message.
  * @param  src:  Remote endpoint address.
  * @param  priv: User private context (unused).
  * @retval 0 on success, negative error code otherwise.
  */
int FwuMgrTask_RpmsgCallback(struct rpmsg_endpoint *ept,
                             void *data, size_t len,
                             uint32_t src, void *priv);

#ifdef __cplusplus
}
#endif

#endif /* FWU_MGR_TASK_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
