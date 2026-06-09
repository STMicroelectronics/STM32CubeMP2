/**
 ******************************************************************************
 * @file   openamp.h
 * @brief  Header for openamp applications
 * @author  MCD Application Team
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
#ifndef __openamp_H
#define __openamp_H
#ifdef __cplusplus
 extern "C" {
#endif

#include "openamp/open_amp.h"
#include "openamp_conf.h"


typedef void (*OPENAMP_MBOX_RxCallback_t)(uint32_t vring_id);

void OPENAMP_RegisterMBOXRxCallback(OPENAMP_MBOX_RxCallback_t cb);


#define OPENAMP_send  rpmsg_send
#define OPENAMP_destroy_ept rpmsg_destroy_ept

/* Initialize the openamp framework*/
int MX_OPENAMP_Init(int RPMsgRole, rpmsg_ns_bind_cb ns_bind_cb, IPCC_HandleTypeDef * hipcc);

/* Deinitialize the openamp framework*/
void OPENAMP_DeInit(IPCC_HandleTypeDef * hipcc);

/* Initialize the endpoint struct*/
void OPENAMP_init_ept(struct rpmsg_endpoint *ept);

/* Create and register the endpoint */
int OPENAMP_create_endpoint(struct rpmsg_endpoint *ept, const char *name,
                            uint32_t dest, rpmsg_ept_cb cb,
                            rpmsg_ns_unbind_cb unbind_cb);

int OPENAMP_create_fixed_endpoint(struct rpmsg_endpoint *ept, const char *name,
                                  uint32_t src, uint32_t dest, rpmsg_ept_cb cb,
                                  rpmsg_ns_unbind_cb unbind_cb);
/* Check for new rpmsg reception */
int OPENAMP_check_for_message(void);

#ifdef __cplusplus
}
#endif
#endif /*__openamp_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
