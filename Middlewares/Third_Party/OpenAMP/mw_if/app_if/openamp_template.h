/**
 ******************************************************************************
 * @file    openamp.h
 * @brief   Header for OpenAMP applications
 * @author  MCD Application Team
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2019-2026 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef OPENAMP_H
#define OPENAMP_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Private includes ----------------------------------------------------------*/
#include "openamp/open_amp.h"
#include "openamp_conf.h"
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN Private defines */
typedef void (*OPENAMP_MBOX_RxCallback_t)(uint32_t vring_id);
void OPENAMP_RegisterMBOXRxCallback(OPENAMP_MBOX_RxCallback_t cb);
/* USER CODE END  Private defines */

/* Exported macro ------------------------------------------------------------*/
#define OPENAMP_send           rpmsg_send
#define OPENAMP_destroy_ept    rpmsg_destroy_ept
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/**
 * @brief  Initialize the OpenAMP framework.
 * @param  RPMsgRole: Role of the RPMsg (master or remote)
 * @param  hipcc: Pointer to IPCC handle
 * @retval 0 on success, negative value on error
 */
int MX_OPENAMP_Init(int RPMsgRole, IPCC_HandleTypeDef *hipcc);

/**
 * @brief  Initialize the RPMSG framework.
 * @param  RPMsgRole: Role of the RPMsg (master or remote)
 * @param  ns_bind_cb: Name service bind callback
 * @param  hipcc: Pointer to IPCC handle
 * @retval 0 on success, negative value on error
 */
int MX_OPENAMP_RPMsg_Init(int RPMsgRole, rpmsg_ns_bind_cb ns_bind_cb, IPCC_HandleTypeDef *hipcc);

/**
 * @brief  Deinitialize the OpenAMP framework.
 * @param  hipcc: Pointer to IPCC handle
 */
void OPENAMP_DeInit(IPCC_HandleTypeDef *hipcc);

/**
 * @brief  Initialize the endpoint struct.
 * @param  ept: Pointer to rpmsg_endpoint structure
 */
void OPENAMP_init_ept(struct rpmsg_endpoint *ept);

/**
 * @brief  Create and register the endpoint.
 * @param  ept: Pointer to rpmsg_endpoint structure
 * @param  name: Endpoint name
 * @param  dest: Destination address
 * @param  cb: Endpoint callback
 * @param  unbind_cb: Unbind callback
 * @retval 0 on success, negative value on error
 */
int OPENAMP_create_endpoint(struct rpmsg_endpoint *ept, const char *name,
                            uint32_t dest, rpmsg_ept_cb cb,
                            rpmsg_ns_unbind_cb unbind_cb);

/**
 * @brief  Create and register the endpoint with a fixed address.
 * @param  ept: Pointer to rpmsg_endpoint structure
 * @param  name: Endpoint name
 * @param  src: Source address
 * @param  dest: Destination address
 * @param  cb: Endpoint callback
 * @param  unbind_cb: Unbind callback
 * @retval 0 on success, negative value on error
 */
int OPENAMP_create_fixed_endpoint(struct rpmsg_endpoint *ept, const char *name,
                                  uint32_t src, uint32_t dest, rpmsg_ept_cb cb,
                                  rpmsg_ns_unbind_cb unbind_cb);

/**
 * @brief  Check for new RPMsg reception.
 * @retval 0 if no message, 1 if message received
 */
int OPENAMP_check_for_message(void);

/**
 * @brief  Wait loop on endpoint ready (message destination address is known).
 * @param  rp_ept: Pointer to rpmsg_endpoint structure
 */
void OPENAMP_Wait_EndPointready(struct rpmsg_endpoint *rp_ept);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif
#endif /*__openamp_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
