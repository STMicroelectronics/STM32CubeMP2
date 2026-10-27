/* USER CODE BEGIN Header */
/**
 ******************************************************************************
  * File Name          : ethernetif.h
  * Description        : This file provides initialization code for LWIP
  *                      middleWare.
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

#ifndef __ETHERNETIF_H__
#define __ETHERNETIF_H__

#include "lwip/err.h"
#include "lwip/netif.h"
#include "stm32mp2xx_hal.h"
#include "rtl8211.h"

/* Within 'USER CODE' section, code will be kept by default at each generation */
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  Per-instance Ethernet interface context.
  *
  * Contains the resources for one Ethernet MAC instance. A pointer to
  * this context is stored in netif->state so lwIP/HAL callbacks can
  * access the correct hardware context.
  */
typedef struct eth_if_ctx
{
  ETH_HandleTypeDef          heth;          /*!< HAL ETH handle for this MAC           */
  ETH_TxPacketConfigTypeDef  TxConfig;      /*!< TX packet configuration               */
  ETH_DMADescTypeDef        *DMARxDscrTab;  /*!< RX descriptor table                   */
  ETH_DMADescTypeDef        *DMATxDscrTab;  /*!< TX descriptor table                   */
  rtl8211_Object_t           phy;           /*!< PHY driver object                     */
  rtl8211_IOCtx_t            phy_io;        /*!< PHY bus I/O context                   */
  struct netif              *netif;         /*!< Back-pointer to the associated netif  */
  uint32_t                   link_timer;    /*!< Link check periodic timer             */
  uint32_t                   DHCPfineTimer; /*!< DHCP periodic timer                   */
  uint8_t                    DHCP_state;    /*!< DHCP state machine current state      */
} eth_if_ctx_t;

/* Ethernet interface contexts: ETH1 (TCP) and ETH2 (UDP) */
extern eth_if_ctx_t eth_ctx1;
extern eth_if_ctx_t eth_ctx2;

/* Exported functions ------------------------------------------------------- */
err_t ethernetif_init(struct netif *netif);

void ethernetif_input(struct netif *netif);
void ethernet_link_check_state(struct netif *netif);


void Error_Handler(void);
u32_t sys_jiffies(void);
u32_t sys_now(void);

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
#endif
