/**
  ******************************************************************************
  * @file    CM33/NonSecure/LwIP/App/app_ethernet.c
  * @author  MCD Application Team
  * @brief   Ethernet-specific module.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "lwip/opt.h"
#include "main.h"
#if LWIP_DHCP
#include "lwip/dhcp.h"
#endif
#include "ethernetif.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#if LWIP_DHCP
#define DHCP_OFF                   (uint8_t) 0
#define DHCP_START                 (uint8_t) 1
#define DHCP_WAIT_ADDRESS          (uint8_t) 2
#define DHCP_ADDRESS_ASSIGNED      (uint8_t) 3
#define DHCP_TIMEOUT               (uint8_t) 4
#define DHCP_LINK_DOWN             (uint8_t) 5
#define MAX_DHCP_TRIES  100
__IO uint8_t DHCP_state = DHCP_OFF;
#endif
extern struct netif gnetif;
extern ip4_addr_t ipaddr;
extern ip4_addr_t netmask;
extern ip4_addr_t gw;
extern uint8_t IP_ADDRESS[4];
extern uint8_t NETMASK_ADDRESS[4];
extern uint8_t GATEWAY_ADDRESS[4];

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Notify the user about the network interface configuration status.
  * @param  netif: the network interface
  * @retval None
  */
void ethernet_link_status_updated(struct netif *netif)
{
  /* Check if the network interface is up */
  if (netif_is_up(netif))
  {
#if LWIP_DHCP
    /* Update DHCP state machine */
    DHCP_state = DHCP_START;
#else
    /* If DHCP is not enabled, print the static IP address */
    printf("Static IP address: %s\n\r", ip4addr_ntoa(netif_ip4_addr(netif)));

    /* Turn on the blue LED and turn off the green LED */
    BSP_LED_On(LED2);
    BSP_LED_Off(LED3);
#endif /* LWIP_DHCP */
  }
  else
  {
#if LWIP_DHCP
    /* If DHCP is enabled, update the DHCP state machine to indicate link down */
    DHCP_state = DHCP_LINK_DOWN;
#else
    /* If DHCP is not enabled, notify that the network cable is not connected */
    printf("The network cable is not connected \n\r");
    BSP_LED_Off(LED2);
    BSP_LED_On(LED3);
#endif /* LWIP_DHCP */
  }
}

#if LWIP_DHCP
/**
  * @brief  DHCP thread to handle DHCP state machine
  * @param  argument: not used
  * @retval None
  */
void DHCP_Thread(void * argument)
{
  struct netif *netif = NULL;
  if( netif == NULL )
  {
    printf("DHCP_Thread: NULL netif argument, using gnetif\r\n");
    netif = &gnetif;
  }
  ip_addr_t ipaddr;
  ip_addr_t netmask;
  ip_addr_t gw;
  struct dhcp *dhcp;

  for (;;)
  {
    switch (DHCP_state)
    {
    case DHCP_START:
      {
        printf("DHCP Start\r\n");

        ip_addr_set_zero_ip4(&netif->ip_addr);
        ip_addr_set_zero_ip4(&netif->netmask);
        ip_addr_set_zero_ip4(&netif->gw);
        DHCP_state = DHCP_WAIT_ADDRESS;
        dhcp_start(netif);
        printf("Started DHCP client\r\n");
      }
      break;
    case DHCP_WAIT_ADDRESS:
      {
        if (dhcp_supplied_address(netif))
        {
          DHCP_state = DHCP_ADDRESS_ASSIGNED;
          printf("IP address assigned by DHCP server: %s\r\n", ip4addr_ntoa(netif_ip4_addr(netif)));
        }
        else
        {
          dhcp = (struct dhcp *)netif_get_client_data(netif, LWIP_NETIF_CLIENT_DATA_INDEX_DHCP);

          /* DHCP timeout */
          if (dhcp->tries > MAX_DHCP_TRIES)
          {
            DHCP_state = DHCP_TIMEOUT;

              /* Assign Static IP */
            IP4_ADDR(&ipaddr, IP_ADDRESS[0], IP_ADDRESS[1], IP_ADDRESS[2], IP_ADDRESS[3]);
            IP4_ADDR(&netmask, NETMASK_ADDRESS[0], NETMASK_ADDRESS[1] , NETMASK_ADDRESS[2], NETMASK_ADDRESS[3]);
            IP4_ADDR(&gw, GATEWAY_ADDRESS[0], GATEWAY_ADDRESS[1], GATEWAY_ADDRESS[2], GATEWAY_ADDRESS[3]);
            netif_set_addr(netif, ip_2_ip4(&ipaddr), ip_2_ip4(&netmask), ip_2_ip4(&gw));
          }
        }
      }
      break;
  case DHCP_LINK_DOWN:
    {
      DHCP_state = DHCP_OFF;
    }
    break;
    default:
	  break;
    }

    /* Wait 6000 ms */
    osDelay(6000);
  }
}
#endif  /* LWIP_DHCP */


