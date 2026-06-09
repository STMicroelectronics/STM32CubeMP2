/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : ethernetif.c
  * Description        : This file provides code for the configuration
  *                      of the ethernetif.c MiddleWare.
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "lwip/opt.h"
#include "lwip/timeouts.h"
#include "netif/ethernet.h"
#include "netif/etharp.h"
#include "lwip/ethip6.h"
#include "ethernetif.h"
#include "rtl8211.h"
#include <string.h>
#include "cmsis_os.h"
#include "lwip/tcpip.h"

/* Within 'USER CODE' section, code will be kept by default at each generation */
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* Private define ------------------------------------------------------------*/
/* The time to block waiting for input. */
#define TIME_WAITING_FOR_INPUT ( osWaitForever )
/* USER CODE BEGIN OS_THREAD_STACK_SIZE_WITH_RTOS */
/* Stack size of the interface thread */
#define INTERFACE_THREAD_STACK_SIZE ( 1024 )
/* USER CODE END OS_THREAD_STACK_SIZE_WITH_RTOS */
/* Network interface name */
#define IFNAME0 's'
#define IFNAME1 't'

/* ETH Setting  */
#define ETH_DMA_TRANSMIT_TIMEOUT               ( 20U )
#define ETH_TX_BUFFER_MAX             ((ETH_TX_DESC_CNT) * 2U)
/* ETH_RX_BUFFER_SIZE parameter is defined in lwipopts.h */
#define  ETH_PHY_STATUS_ERROR                 ((int32_t)-1)
#define  ETH_PHY_STATUS_OK                    ((int32_t) 0)

#define  ETH_PHY_STATUS_LINK_ERROR            ((int32_t) 0)
#define  ETH_PHY_STATUS_LINK_DOWN             ((int32_t) 1)

#define  ETH_PHY_STATUS_100MBITS_FULLDUPLEX   ((int32_t) 2)
#define  ETH_PHY_STATUS_100MBITS_HALFDUPLEX   ((int32_t) 3)
#define  ETH_PHY_STATUS_10MBITS_FULLDUPLEX    ((int32_t) 4)
#define  ETH_PHY_STATUS_10MBITS_HALFDUPLEX    ((int32_t) 5)
#define  ETH_PHY_STATUS_AUTONEGO_NOT_DONE     ((int32_t) 6)
#if defined(ETH_PHY_1000MBITS_SUPPORTED)
#define  ETH_PHY_STATUS_1000MBITS_FULLDUPLEX  ((int32_t) 7)
#define  ETH_PHY_STATUS_1000MBITS_HALFDUPLEX  ((int32_t) 8)
#endif
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/* Private variables ---------------------------------------------------------*/
/*
@Note: This interface is implemented to operate in zero-copy mode only:
        - Rx buffers will be allocated from LwIP stack memory heap,
          then passed to ETH HAL driver.
        - Tx buffers will be allocated from LwIP stack memory heap,
          then passed to ETH HAL driver.

@Notes:
  1.a. ETH DMA Rx descriptors must be contiguous, the default count is 4,
       to customize it please redefine ETH_RX_DESC_CNT in ETH GUI (Rx Descriptor Length)
       so that updated value will be generated in stm32xxxx_hal_conf.h
  1.b. ETH DMA Tx descriptors must be contiguous, the default count is 4,
       to customize it please redefine ETH_TX_DESC_CNT in ETH GUI (Tx Descriptor Length)
       so that updated value will be generated in stm32xxxx_hal_conf.h

  2.a. Rx Buffers number must be between ETH_RX_DESC_CNT and 2*ETH_RX_DESC_CNT
  2.b. Rx Buffers must have the same size: ETH_RX_BUFFER_SIZE, this value must
       passed to ETH DMA in the init field (heth.Init.RxBuffLen)
  2.c  The RX Ruffers addresses and sizes must be properly defined to be aligned
       to L1-CACHE line size (32 bytes).
*/

/* Data Type Definitions */
typedef enum
{
  RX_ALLOC_OK       = 0x00,
  RX_ALLOC_ERROR    = 0x01
} RxAllocStatusTypeDef;

typedef struct
{
  struct pbuf_custom pbuf_custom;
  uint8_t buff[(ETH_RX_BUFFER_SIZE + 31) & ~31] __ALIGNED(32);
} RxBuff_t;

/* Memory Pool Declaration */
#define ETH_RX_BUFFER_CNT             ((ETH_RX_DESC_CNT) * 2U)

/* Memory Pool Declaration */
LWIP_MEMPOOL_DECLARE(RX_POOL, ETH_RX_BUFFER_CNT, sizeof(RxBuff_t), "Zero-copy RX PBUF pool");

#if defined ( __ICCARM__ ) /*!< IAR Compiler */
#pragma location = 0x30000400
extern u8_t memp_memory_RX_POOL_base[];

#elif defined ( __CC_ARM )  /* MDK ARM Compiler */
__attribute__((section(".Rx_PoolSection"))) extern u8_t memp_memory_RX_POOL_base[];

#elif defined ( __GNUC__ ) /* GNU Compiler */
__attribute__((section(".Rx_PoolSection"))) extern u8_t memp_memory_RX_POOL_base[];

#endif

/* Variable Definitions */
static uint8_t RxAllocStatus;


/* USER CODE BEGIN 2 */

/* USER CODE END 2 */

osSemaphoreId_t RxPktSemaphore = NULL;   /* Semaphore to signal incoming packets */
osSemaphoreId_t TxPktSemaphore = NULL;   /* Semaphore to signal transmit packet complete */

/* Global Ethernet handle */
extern ETH_HandleTypeDef heth;
extern ETH_TxPacketConfigTypeDef TxConfig;
ETH_MACFilterConfigTypeDef FilterConfig;

/* Private function prototypes -----------------------------------------------*/

static void driver_hardware_initialize(void);
typedef void * 	eth_phy_handle_t;
static int32_t eth_phy_init(void);
static int32_t eth_phy_get_link_state(void);
static int32_t rtl8211_io_init(void);
static int32_t rtl8211_io_deinit (void);
static int32_t rtl8211_io_read_reg(uint32_t DevAddr, uint32_t RegAddr, uint32_t *pRegVal);
static int32_t rtl8211_io_write_reg(uint32_t DevAddr, uint32_t RegAddr, uint32_t RegVal);
static int32_t rtl8211_io_get_tick(void);

/* RTL8211 IO context object */
static rtl8211_IOCtx_t  RTL8211_IOCtx = { rtl8211_io_init,
                                          rtl8211_io_deinit,
                                          rtl8211_io_write_reg,
                                          rtl8211_io_read_reg,
                                          rtl8211_io_get_tick
                                        };
/* RTL8211 main object */
static rtl8211_Object_t RTL8211;

/* USER CODE BEGIN 3 */

/* USER CODE END 3 */

/* Private functions ---------------------------------------------------------*/
void pbuf_free_custom(struct pbuf *p);

/**
  * @brief  Ethernet Rx Transfer completed callback
  * @param  handlerEth: ETH handler
  * @retval None
  */
void HAL_ETH_RxCpltCallback(ETH_HandleTypeDef *handlerEth)
{
  osSemaphoreRelease(RxPktSemaphore);
}
/**
  * @brief  Ethernet Tx Transfer completed callback
  * @param  handlerEth: ETH handler
  * @retval None
  */
void HAL_ETH_TxCpltCallback(ETH_HandleTypeDef *handlerEth)
{
  osSemaphoreRelease(TxPktSemaphore);
}
/**
  * @brief  Ethernet DMA transfer error callback
  * @param  handlerEth: ETH handler
  * @retval None
  */
void HAL_ETH_ErrorCallback(ETH_HandleTypeDef *handlerEth)
{
//  if((HAL_ETH_GetDMAError(handlerEth) & ETH_DMAC0SR_RBU) == ETH_DMAC0SR_RBU)
//  {
//     osSemaphoreRelease(RxPktSemaphore);
//  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

static void driver_hardware_initialize(void)
{
  ETH_DMAConfigTypeDef dmaDefaultConf;
#if defined(ETH_DMASBMR_BLEN4) /* ETH AXI support*/
  dmaDefaultConf.DMAArbitration = ETH_DMAARBITRATION_TX;
#else
  dmaDefaultConf.DMAArbitration = ETH_DMAARBITRATION_RX1_TX1;
#endif
  dmaDefaultConf.AddressAlignedBeats = ENABLE;
  dmaDefaultConf.BurstMode = ETH_BURSTLENGTH_FIXED;
  dmaDefaultConf.TxDMABurstLength = ETH_TXDMABURSTLENGTH_32BEAT;
  dmaDefaultConf.RxDMABurstLength = ETH_RXDMABURSTLENGTH_32BEAT;
  dmaDefaultConf.FlushRxPacket = DISABLE;
#ifndef STM32_ETH_HAL_LEGACY
  dmaDefaultConf.PBLx8Mode = DISABLE;
#if defined(ETH_DMASBMR_BLEN4) /* ETH AXI support*/
  dmaDefaultConf.RxOSRLimit = ETH_RX_OSR_LIMIT_3;
  dmaDefaultConf.TxOSRLimit = ETH_TX_OSR_LIMIT_3;
  dmaDefaultConf.AXIBLENMaxSize = ETH_BLEN_MAX_SIZE_16;
#else
  dmaDefaultConf.RebuildINCRxBurst = DISABLE;
#endif
  dmaDefaultConf.SecondPacketOperate = ENABLE;
  dmaDefaultConf.TCPSegmentation = DISABLE;
  dmaDefaultConf.MaximumSegmentSize = 536;
#endif
#ifdef STM32_ETH_HAL_LEGACY
  dmaDefaultConf.DropTCPIPChecksumErrorFrame = ENABLE;
  dmaDefaultConf.ReceiveStoreForward =  DISABLE;
  dmaDefaultConf.TransmitStoreForward =  ENABLE;
  dmaDefaultConf.TransmitThresholdControl =  ENABLE;
  dmaDefaultConf.ForwardErrorFrames =  DISABLE;
  dmaDefaultConf.ReceiveThresholdControl =  DISABLE;
  dmaDefaultConf.SecondFrameOperate =  DISABLE;
  dmaDefaultConf.EnhancedDescriptorFormat =  DISABLE;
  dmaDefaultConf.DescriptorSkipLength =  DISABLE;
#endif
  /* enable OSF bit to enhance throughput */
  HAL_ETH_SetDMAConfig(&heth, &dmaDefaultConf);
#ifdef STM32_ETH_PROMISCUOUS_ENABLE
  FilterConfig.PromiscuousMode = ENABLE;
#else
  FilterConfig.PromiscuousMode = DISABLE;
#endif
  FilterConfig.HashUnicast = DISABLE;
  FilterConfig.HashMulticast = DISABLE;
  FilterConfig.DestAddrInverseFiltering = DISABLE;
  FilterConfig.PassAllMulticast = DISABLE;
  FilterConfig.BroadcastFilter = ENABLE;
  FilterConfig.SrcAddrInverseFiltering = DISABLE;
  FilterConfig.SrcAddrFiltering = DISABLE;
  FilterConfig.HachOrPerfectFilter = DISABLE;
  FilterConfig.ReceiveAllMode = DISABLE;
  FilterConfig.ControlPacketsFilter = 0x00;
//  FilterConfig.PassAllMulticast = ENABLE;
//  HAL_ETH_SetMACFilterConfig(&heth, &FilterConfig);

  /* Set Tx packet config common parameters */
  memset(&TxConfig, 0, sizeof(ETH_TxPacketConfigTypeDef));
  TxConfig.Attributes = ETH_TX_PACKETS_FEATURES_CSUM ;
  TxConfig.CRCPadCtrl = ETH_CRC_PAD_DISABLE;
}

/*******************************************************************************
                       LL Driver Interface ( LwIP stack --> ETH)
*******************************************************************************/
/**
 * @brief In this function, the hardware should be initialized.
 * Called from ethernetif_init().
 *
 * @param netif the already initialized lwip network interface structure
 *        for this ethernetif
 */
static void low_level_init(struct netif *netif)
{
  /* USER CODE BEGIN OS_THREAD_ATTR_CMSIS_RTOS_V2 */
  osThreadAttr_t attributes;
  ETH_MACConfigTypeDef MACConf;
  /* USER CODE END OS_THREAD_ATTR_CMSIS_RTOS_V2 */
  uint32_t duplex, speed = 0;
  int32_t PHYLinkState = 0;
  FunctionalState portselect = DISABLE;
  /* Start ETH HAL Init */
  driver_hardware_initialize();
  /* End ETH HAL Init */

#if LWIP_ARP || LWIP_ETHERNET

  /* set MAC hardware address length */
  netif->hwaddr_len = ETH_HWADDR_LEN;

  /* set MAC hardware address */
  netif->hwaddr[0] =  heth.Init.MACAddr[0];
  netif->hwaddr[1] =  heth.Init.MACAddr[1];
  netif->hwaddr[2] =  heth.Init.MACAddr[2];
  netif->hwaddr[3] =  heth.Init.MACAddr[3];
  netif->hwaddr[4] =  heth.Init.MACAddr[4];
  netif->hwaddr[5] =  heth.Init.MACAddr[5];

  /* maximum transfer unit */
  netif->mtu = ETH_MAX_PAYLOAD;

  /* Accept broadcast address and ARP traffic */
  /* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
  #if LWIP_ARP
    netif->flags |= NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;
  #else
    netif->flags |= NETIF_FLAG_BROADCAST;
  #endif /* LWIP_ARP */

  /* Initialize the RX POOL */
  LWIP_MEMPOOL_INIT(RX_POOL);

  /* Set Tx packet config common parameters */
  memset(&TxConfig, 0, sizeof(ETH_TxPacketConfigTypeDef));
  TxConfig.Attributes = ETH_TX_PACKETS_FEATURES_CSUM ;
  TxConfig.CRCPadCtrl = ETH_CRC_PAD_DISABLE;

  /* create a binary semaphore used for informing ethernetif of frame reception */
  RxPktSemaphore = osSemaphoreNew(1, 1, NULL);
  osSemaphoreAcquire(RxPktSemaphore, 0);

  /* create a binary semaphore used for informing ethernetif of frame transmission */
  TxPktSemaphore = osSemaphoreNew(1, 1, NULL);
  osSemaphoreAcquire(TxPktSemaphore, 0);

  /* create the task that handles the ETH_MAC */
  /* USER CODE BEGIN OS_THREAD_NEW_CMSIS_RTOS_V2 */
  memset(&attributes, 0x0, sizeof(osThreadAttr_t));
  attributes.name = "EthIf";
  attributes.stack_size = INTERFACE_THREAD_STACK_SIZE;
  attributes.priority = osPriorityRealtime;
  osThreadNew(ethernetif_input, netif, &attributes);
  /* USER CODE END OS_THREAD_NEW_CMSIS_RTOS_V2 */

  /* USER CODE BEGIN PHY_PRE_CONFIG */

  /* USER CODE END PHY_PRE_CONFIG */
  /* Set PHY IO functions */
  if (eth_phy_init() != ETH_PHY_STATUS_OK)
  {
    return;
  }

  PHYLinkState = eth_phy_get_link_state();

  /* Get link state */
  if(PHYLinkState <= ETH_PHY_STATUS_LINK_DOWN)
  {
    netif_set_link_down(netif);
    netif_set_down(netif);
  }
  else
  {
    switch (PHYLinkState)
    {
#if defined(ETH_PHY_1000MBITS_SUPPORTED)
    case ETH_PHY_STATUS_1000MBITS_FULLDUPLEX:
      duplex = ETH_FULLDUPLEX_MODE;
      speed = ETH_SPEED_1000M;
      portselect = DISABLE;
      break;
    case ETH_PHY_STATUS_1000MBITS_HALFDUPLEX:
      duplex = ETH_HALFDUPLEX_MODE;
      speed = ETH_SPEED_1000M;
      portselect = DISABLE;
      break;
#endif
    case ETH_PHY_STATUS_100MBITS_FULLDUPLEX:
      duplex = ETH_FULLDUPLEX_MODE;
      speed = ETH_SPEED_100M;
      portselect = ENABLE;
      break;
    case ETH_PHY_STATUS_100MBITS_HALFDUPLEX:
      duplex = ETH_HALFDUPLEX_MODE;
      speed = ETH_SPEED_100M;
      portselect = ENABLE;
      break;
    case ETH_PHY_STATUS_10MBITS_FULLDUPLEX:
      duplex = ETH_FULLDUPLEX_MODE;
      speed = ETH_SPEED_10M;
      portselect = ENABLE;
      break;
    case ETH_PHY_STATUS_10MBITS_HALFDUPLEX:
      duplex = ETH_HALFDUPLEX_MODE;
      speed = ETH_SPEED_10M;
      portselect = ENABLE;
      break;
    default:
      duplex = ETH_FULLDUPLEX_MODE;
      speed = ETH_SPEED_100M;
      portselect = ENABLE;
      break;
    }

    /* Get MAC Config MAC */
    HAL_ETH_GetMACConfig(&heth, &MACConf);
    MACConf.DuplexMode = duplex;
    MACConf.Speed = speed;
    MACConf.PortSelect = portselect;
    HAL_ETH_SetMACConfig(&heth, &MACConf);
    HAL_ETH_Start_IT(&heth);
//    netif_set_up(netif);
//    netif_set_link_up(netif);
  }
  /* USER CODE BEGIN PHY_POST_CONFIG */

  /* USER CODE END PHY_POST_CONFIG */
#endif /* LWIP_ARP || LWIP_ETHERNET */

  /* USER CODE BEGIN LOW_LEVEL_INIT */

  /* USER CODE END LOW_LEVEL_INIT */
}

/**
 * @brief This function should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
 * @return ERR_OK if the packet could be sent
 *         an err_t value if the packet couldn't be sent
 *
 * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
 *       strange results. You might consider waiting for space in the DMA queue
 *       to become available since the stack doesn't retry to send a packet
 *       dropped because of memory failure (except for the TCP timers).
 */

static err_t low_level_output(struct netif *netif, struct pbuf *p)
{
  uint32_t i = 0U;
  struct pbuf *q = NULL;
  err_t errval = ERR_OK;
  ETH_BufferTypeDef Txbuffer[ETH_TX_DESC_CNT] = {0};

  memset(Txbuffer, 0 , ETH_TX_DESC_CNT*sizeof(ETH_BufferTypeDef));

  for(q = p; q != NULL; q = q->next)
  {
    if(i >= ETH_TX_DESC_CNT)
      return ERR_IF;

    Txbuffer[i].buffer = q->payload;
    Txbuffer[i].len = q->len;

    if(i>0)
    {
      Txbuffer[i-1].next = &Txbuffer[i];
    }

    if(q->next == NULL)
    {
      Txbuffer[i].next = NULL;
    }

    i++;
  }

  TxConfig.ChecksumCtrl = ETH_CHECKSUM_IPHDR_PAYLOAD_INSERT_PHDR_CALC;
  TxConfig.Length = p->tot_len;
  TxConfig.TxBuffer = Txbuffer;
  TxConfig.pData = p;

  pbuf_ref(p);

  HAL_ETH_Transmit_IT(&heth, &TxConfig);
  while(osSemaphoreAcquire(TxPktSemaphore, TIME_WAITING_FOR_INPUT) != osOK)
  {
  }

  HAL_ETH_ReleaseTxPacket(&heth);

  return errval;
}

/**
 * @brief Should allocate a pbuf and transfer the bytes of the incoming
 * packet from the interface into the pbuf.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return a pbuf filled with the received packet (including MAC header)
 *         NULL on memory error
   */
static struct pbuf * low_level_input(struct netif *netif)
{
  struct pbuf *p = NULL;

  if(RxAllocStatus == RX_ALLOC_OK)
  {
    HAL_ETH_ReadData(&heth, (void **)&p);
  }

  return p;
}

/**
 * @brief This function should be called when a packet is ready to be read
 * from the interface. It uses the function low_level_input() that
 * should handle the actual reception of bytes from the network
 * interface. Then the type of the received packet is determined and
 * the appropriate input function is called.
 *
 * @param netif the lwip network interface structure for this ethernetif
 */
void ethernetif_input(void* argument)
{
  struct pbuf *p = NULL;
  struct netif *netif = (struct netif *) argument;

  for( ;; )
  {
    if (osSemaphoreAcquire(RxPktSemaphore, TIME_WAITING_FOR_INPUT) == osOK)
    {
      do
      {
        p = low_level_input( netif );
        if (p != NULL)
        {
          if (netif->input( p, netif) != ERR_OK )
          {
            pbuf_free(p);
          }
        }
      } while(p!=NULL);
    }
  }
}

#if !LWIP_ARP
/**
 * This function has to be completed by user in case of ARP OFF.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if ...
 */
static err_t low_level_output_arp_off(struct netif *netif, struct pbuf *q, const ip4_addr_t *ipaddr)
{
  err_t errval;
  errval = ERR_OK;

/* USER CODE BEGIN 5 */

/* USER CODE END 5 */

  return errval;

}
#endif /* LWIP_ARP */

/**
 * @brief Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 * This function should be passed as a parameter to netif_add().
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 *         any other err_t on error
 */
err_t ethernetif_init(struct netif *netif)
{
  LWIP_ASSERT("netif != NULL", (netif != NULL));

#if LWIP_NETIF_HOSTNAME
  /* Initialize interface hostname */
  netif->hostname = "lwip";
#endif /* LWIP_NETIF_HOSTNAME */

  /*
   * Initialize the snmp variables and counters inside the struct netif.
   * The last argument should be replaced with your link speed, in units
   * of bits per second.
   */
  // MIB2_INIT_NETIF(netif, snmp_ifType_ethernet_csmacd, LINK_SPEED_OF_YOUR_NETIF_IN_BPS);

  netif->name[0] = IFNAME0;
  netif->name[1] = IFNAME1;
  /* We directly use etharp_output() here to save a function call.
   * You can instead declare your own function an call etharp_output()
   * from it if you have to do some checks before sending (e.g. if link
   * is available...) */

#if LWIP_IPV4
#if LWIP_ARP || LWIP_ETHERNET
#if LWIP_ARP
  netif->output = etharp_output;
#else
  /* The user should write its own code in low_level_output_arp_off function */
  netif->output = low_level_output_arp_off;
#endif /* LWIP_ARP */
#endif /* LWIP_ARP || LWIP_ETHERNET */
#endif /* LWIP_IPV4 */

#if LWIP_IPV6
  netif->output_ip6 = ethip6_output;
#endif /* LWIP_IPV6 */

  netif->linkoutput = low_level_output;

  /* initialize the hardware */
  low_level_init(netif);

  return ERR_OK;
}

/**
  * @brief  Custom Rx pbuf free callback
  * @param  pbuf: pbuf to be freed
  * @retval None
  */
void pbuf_free_custom(struct pbuf *p)
{
  struct pbuf_custom* custom_pbuf = (struct pbuf_custom*)p;
  LWIP_MEMPOOL_FREE(RX_POOL, custom_pbuf);

  /* If the Rx Buffer Pool was exhausted, signal the ethernetif_input task to
   * call HAL_ETH_GetRxDataBuffer to rebuild the Rx descriptors. */

  if (RxAllocStatus == RX_ALLOC_ERROR)
  {
    RxAllocStatus = RX_ALLOC_OK;
    osSemaphoreRelease(RxPktSemaphore);
  }
}

/* USER CODE BEGIN 6 */

/**
* @brief  Returns the current time in milliseconds
*         when LWIP_TIMERS == 1 and NO_SYS == 1
* @param  None
* @retval Current Time value
*/
u32_t sys_jiffies(void)
{
  return HAL_GetTick();
}

/**
* @brief  Returns the current time in milliseconds
*         when LWIP_TIMERS == 1 and NO_SYS == 1
* @param  None
* @retval Current Time value
*/
u32_t sys_now(void)
{
  return HAL_GetTick();
}

/* USER CODE END 6 */

/**
  * @brief  Check the ETH link state then update ETH driver and netif link accordingly.
  * @retval None
  */
void ethernet_link_thread(void* argument)
{
  ETH_MACConfigTypeDef MACConf = {0};
  int32_t PHYLinkState = 0;
  uint32_t linkchanged = 0U, speed = 0U, duplex = 0U;
  FunctionalState portselect = DISABLE;

  struct netif *netif = (struct netif *) argument;
/* USER CODE BEGIN ETH link init */

/* USER CODE END ETH link init */

  for(;;)
  {
		PHYLinkState = eth_phy_get_link_state();

		if(netif_is_link_up(netif) && (PHYLinkState <= ETH_PHY_STATUS_LINK_DOWN))
		{
			HAL_ETH_Stop_IT(&heth);
			netif_set_down(netif);
			netif_set_link_down(netif);
		}
		else if(!netif_is_link_up(netif) && (PHYLinkState > ETH_PHY_STATUS_LINK_DOWN))
		{
			switch (PHYLinkState)
			{
	#if defined(ETH_PHY_1000MBITS_SUPPORTED)
			case ETH_PHY_STATUS_1000MBITS_FULLDUPLEX:
				duplex = ETH_FULLDUPLEX_MODE;
				speed = ETH_SPEED_1000M;
        portselect = DISABLE;
				linkchanged = 1;
				break;
			case ETH_PHY_STATUS_1000MBITS_HALFDUPLEX:
				duplex = ETH_HALFDUPLEX_MODE;
				speed = ETH_SPEED_1000M;
        portselect = DISABLE;
				linkchanged = 1;
				break;
	#endif
			case ETH_PHY_STATUS_100MBITS_FULLDUPLEX:
				duplex = ETH_FULLDUPLEX_MODE;
				speed = ETH_SPEED_100M;
        portselect = ENABLE;
				linkchanged = 1;
				break;
			case ETH_PHY_STATUS_100MBITS_HALFDUPLEX:
				duplex = ETH_HALFDUPLEX_MODE;
				speed = ETH_SPEED_100M;
        portselect = ENABLE;
				linkchanged = 1;
				break;
			case ETH_PHY_STATUS_10MBITS_FULLDUPLEX:
				duplex = ETH_FULLDUPLEX_MODE;
				speed = ETH_SPEED_10M;
        portselect = ENABLE;
				linkchanged = 1;
				break;
			case ETH_PHY_STATUS_10MBITS_HALFDUPLEX:
				duplex = ETH_HALFDUPLEX_MODE;
				speed = ETH_SPEED_10M;
        portselect = ENABLE;
				linkchanged = 1;
				break;
			default:
				duplex = ETH_FULLDUPLEX_MODE;
				speed = ETH_SPEED_100M;
        portselect = ENABLE;
				linkchanged = 1;
				break;
			}

			if(linkchanged)
			{
				/* Get MAC Config MAC */
				HAL_ETH_GetMACConfig(&heth, &MACConf);
				MACConf.DuplexMode = duplex;
				MACConf.Speed = speed;
				MACConf.PortSelect = portselect;
				HAL_ETH_SetMACConfig(&heth, &MACConf);
				HAL_ETH_Start_IT(&heth);
				netif_set_up(netif);
				netif_set_link_up(netif);
			}
		}

/* USER CODE BEGIN ETH link Thread core code for User BSP */

/* USER CODE END ETH link Thread core code for User BSP */

    osDelay(100);
  }
}

void HAL_ETH_RxAllocateCallback(uint8_t **buff)
{
/* USER CODE BEGIN HAL ETH RxAllocateCallback */
  struct pbuf_custom *p = LWIP_MEMPOOL_ALLOC(RX_POOL);
  if (p)
  {
    /* Get the buff from the struct pbuf address. */
//    SCB_InvalidateDCache_by_Addr((void *)(p->pbuf.payload), (int32_t)(p->pbuf.len))
    *buff = (uint8_t *)p + offsetof(RxBuff_t, buff);
    p->custom_free_function = pbuf_free_custom;
    /* Initialize the struct pbuf.
    * This must be performed whenever a buffer's allocated because it may be
    * changed by lwIP or the app, e.g., pbuf_free decrements ref. */
    pbuf_alloced_custom(PBUF_RAW, 0, PBUF_REF, p, *buff, ETH_RX_BUFFER_SIZE);
  }
  else
  {
    RxAllocStatus = RX_ALLOC_ERROR;
    *buff = NULL;
  }
/* USER CODE END HAL ETH RxAllocateCallback */
}

void HAL_ETH_RxLinkCallback(void **pStart, void **pEnd, uint8_t *buff, uint16_t Length)
{
/* USER CODE BEGIN HAL ETH RxLinkCallback */

  struct pbuf **ppStart = (struct pbuf **)pStart;
  struct pbuf **ppEnd = (struct pbuf **)pEnd;
  struct pbuf *p = NULL;

  /* Get the struct pbuf from the buff address. */
  p = (struct pbuf *)(buff - offsetof(RxBuff_t, buff));
  p->next = NULL;
  p->tot_len = 0;
  p->len = Length;

  /* Chain the buffer. */
  if (!*ppStart)
  {
    /* The first buffer of the packet. */
    *ppStart = p;
  }
  else
  {
    /* Chain the buffer to the end of the packet. */
    (*ppEnd)->next = p;
  }
  *ppEnd  = p;

  /* Update the total length of all the buffers of the chain. Each pbuf in the chain should have its tot_len
   * set to its own length, plus the length of all the following pbufs in the chain. */
  for (p = *ppStart; p != NULL; p = p->next)
  {
    p->tot_len += Length;
  }

  /* Invalidate data cache because Rx DMA's writing to physical memory makes it stale. */
//  SCB_InvalidateDCache_by_Addr((uint32_t *)buff, Length);

/* USER CODE END HAL ETH RxLinkCallback */
}

void HAL_ETH_TxFreeCallback(uint32_t * buff)
{
/* USER CODE BEGIN HAL ETH TxFreeCallback */

  pbuf_free((struct pbuf *)buff);

/* USER CODE END HAL ETH TxFreeCallback */
}

/*******************************************************************************
                       PHI IO Functions
*******************************************************************************/
/**
  * @brief  Initialize the PHY interface
  * @param  none
  * @retval ETH_PHY_STATUS_OK on success, ETH_PHY_STATUS_ERROR otherwise
  */

static int32_t eth_phy_init(void)
{
    int32_t ret = ETH_PHY_STATUS_ERROR;
    /* Set PHY IO functions */

    RTL8211_RegisterBusIO(&RTL8211, &RTL8211_IOCtx);
    /* Initialize the RTL8211 ETH PHY */

    if (RTL8211_Init(&RTL8211) == RTL8211_STATUS_OK)
    {
        ret = ETH_PHY_STATUS_OK;
    }

    return ret;
}

/**
  * @brief  get the Phy link state.
  * @param  none
  * @retval the link status.
  */

static int32_t eth_phy_get_link_state(void)
{
    int32_t  linkstate = RTL8211_GetLinkState(&RTL8211);

    return linkstate;
}

/**
  * @brief  Initialize the PHY MDIO interface
  * @param  None
  * @retval 0 if OK, -1 if ERROR
  */

static int32_t rtl8211_io_init(void)
{
  /* We assume that MDIO GPIO configuration is already done
     in the ETH_MspInit() else it should be done here
  */

  /* Configure the MDIO Clock */
  HAL_ETH_SetMDIOClockRange(&heth);

  return ETH_PHY_STATUS_OK;
}

/**
  * @brief  De-Initialize the MDIO interface
  * @param  None
  * @retval 0 if OK, -1 if ERROR
  */
static int32_t rtl8211_io_deinit (void)
{
    return ETH_PHY_STATUS_OK;
}

/**
  * @brief  Read a PHY register through the MDIO interface.
  * @param  DevAddr: PHY port address
  * @param  RegAddr: PHY register address
  * @param  pRegVal: pointer to hold the register value
  * @retval 0 if OK -1 if Error
  */
static int32_t rtl8211_io_read_reg(uint32_t DevAddr, uint32_t RegAddr, uint32_t *pRegVal)
{
  if(HAL_ETH_ReadPHYRegister(&heth, DevAddr, RegAddr, pRegVal) != HAL_OK)
  {
    return ETH_PHY_STATUS_ERROR;
  }

  return ETH_PHY_STATUS_OK;
}

static int32_t rtl8211_io_write_reg(uint32_t DevAddr, uint32_t RegAddr, uint32_t RegVal)
{
  if(HAL_ETH_WritePHYRegister(&heth, DevAddr, RegAddr, RegVal) != HAL_OK)
  {
    return ETH_PHY_STATUS_ERROR;
  }

  return ETH_PHY_STATUS_OK;
}

/**
  * @brief  Get the time in millisecons used for internal PHY driver process.
  * @retval Time value
  */
static int32_t rtl8211_io_get_tick(void)
{
  return HAL_GetTick();
}

/* USER CODE BEGIN 8 */

/* USER CODE END 8 */

