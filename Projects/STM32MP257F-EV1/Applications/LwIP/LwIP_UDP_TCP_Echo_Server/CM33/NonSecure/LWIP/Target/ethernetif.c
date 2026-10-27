/**
  ******************************************************************************
  * @file    LwIP/LwIP_UDP_Echo_Client/Src/ethernetif.c
  * @author  MCD Application Team
  * @brief   This file implements Ethernet network interface drivers for lwIP
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
#include "stm32mp2xx_hal.h"
#include "lwip/opt.h"
#include "lwip/timeouts.h"
#include "lwip/netif.h"
#include "netif/etharp.h"
#include "ethernetif.h"
#include "rtl8211.h"
#include <string.h>

/* Private typedef -----------------------------------------------------------*/
#if defined(INSTRUCTION_CACHE_ENABLE) && (INSTRUCTION_CACHE_ENABLE == 1U)
extern DCACHE_HandleTypeDef hdcache;
#endif

/* Private define ------------------------------------------------------------*/
/* Network interface name */
#define IFNAME0 's'
#define IFNAME1 't'

#define ETH_DMA_TRANSMIT_TIMEOUT                (20U)
#define ETH_RX_BUFFER_SIZE                       1536
#define ETH_RX_BUFFER_CNT                       ((ETH_RX_DESC_CNT) * 4U)
#define ETH_TX_BUFFER_MAX                       ((ETH_TX_DESC_CNT) * 2U)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/*
@Note: This interface is implemented to operate in zero-copy mode only:
        - Rx Buffers will be allocated from LwIP stack memory heap,
          then passed to ETH HAL driver.
        - Tx Buffers will be allocated from LwIP stack memory heap,
          then passed to ETH HAL driver.

@Notes:
  1.a. ETH DMA Rx descriptors must be contiguous, the default count is 4,
       to customize it please redefine ETH_RX_DESC_CNT in ETH GUI (Rx Descriptor Length)
       so that updated value will be generated in stm32xxxx_hal_conf.h
  1.b. ETH DMA Tx descriptors must be contiguous, the default count is 4,
       to customize it please redefine ETH_TX_DESC_CNT in ETH GUI (Tx Descriptor Length)
       so that updated value will be generated in stm32xxxx_hal_conf.h

  2.a. Rx Buffers number: ETH_RX_BUFFER_CNT must be greater than ETH_RX_DESC_CNT.
  2.b. Rx Buffers must have the same size: ETH_RX_BUFFER_SIZE, this value must
       passed to ETH DMA in the init field (heth.Init.RxBuffLen)
*/
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

/* Ethernet Rx/Tx DMA Descriptors */
static ETH_DMADescTypeDef DMARxDscrTab1[ETH_RX_DESC_CNT] __attribute__((section(".RxDescripSection"))); /* ETH1 Rx */
static ETH_DMADescTypeDef DMATxDscrTab1[ETH_TX_DESC_CNT] __attribute__((section(".TxDescripSection"))); /* ETH1 Tx */
static ETH_DMADescTypeDef DMARxDscrTab2[ETH_RX_DESC_CNT] __attribute__((section(".RxDescripSection"))); /* ETH2 Rx */
static ETH_DMADescTypeDef DMATxDscrTab2[ETH_TX_DESC_CNT] __attribute__((section(".TxDescripSection"))); /* ETH2 Tx */

/* Memory Pool Declaration */
LWIP_MEMPOOL_DECLARE(RX_POOL, ETH_RX_BUFFER_CNT, sizeof(RxBuff_t), "Zero-copy RX PBUF pool");

LWIP_MEMPOOL_DECLARE(LWIP_HEAP_RAM, 1, MEM_SIZE, "LwIP heap ram");

__attribute__((section(".Rx_PoolSection"))) extern u8_t memp_memory_RX_POOL_base[];
__attribute__((section(".LwIP_Heap_RAM_Section"))) extern u8_t memp_memory_LWIP_HEAP_RAM_base[];

/* Variable Definitions */
static uint8_t RxAllocStatus;

/* Ethernet interface contexts */
eth_if_ctx_t eth_ctx1 = { .DMARxDscrTab = DMARxDscrTab1, .DMATxDscrTab = DMATxDscrTab1 };
eth_if_ctx_t eth_ctx2 = { .DMARxDscrTab = DMARxDscrTab2, .DMATxDscrTab = DMATxDscrTab2 };

/* Private function prototypes -----------------------------------------------*/
u32_t sys_now(void);

/* PHY IO function callbacks */
static int32_t ETH1_PHY_IO_Init(void);
static int32_t ETH2_PHY_IO_Init(void);
static int32_t ETH_PHY_IO_DeInit(void);
static int32_t ETH1_PHY_IO_ReadReg(uint32_t DevAddr, uint32_t RegAddr, uint32_t *pRegVal);
static int32_t ETH2_PHY_IO_ReadReg(uint32_t DevAddr, uint32_t RegAddr, uint32_t *pRegVal);
static int32_t ETH1_PHY_IO_WriteReg(uint32_t DevAddr, uint32_t RegAddr, uint32_t RegVal);
static int32_t ETH2_PHY_IO_WriteReg(uint32_t DevAddr, uint32_t RegAddr, uint32_t RegVal);
static int32_t ETH_PHY_IO_GetTick(void);
void HAL_ETH_RxAllocateCallback(uint8_t **buff);

/* Private functions ---------------------------------------------------------*/
void pbuf_free_custom(struct pbuf *p);
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
  eth_if_ctx_t *ctx = (eth_if_ctx_t *)netif->state;

  /* set MAC hardware address length */
  netif->hwaddr_len = ETH_HWADDR_LEN;

  /* set MAC hardware address */
  netif->hwaddr[0] =  ctx->heth.Init.MACAddr[0];
  netif->hwaddr[1] =  ctx->heth.Init.MACAddr[1];
  netif->hwaddr[2] =  ctx->heth.Init.MACAddr[2];
  netif->hwaddr[3] =  ctx->heth.Init.MACAddr[3];
  netif->hwaddr[4] =  ctx->heth.Init.MACAddr[4];
  netif->hwaddr[5] =  ctx->heth.Init.MACAddr[5];

  /* maximum transfer unit */
  netif->mtu = ETH_MAX_PAYLOAD;

  /* Accept broadcast address and ARP traffic */
  /* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
  netif->flags |= NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;

  HAL_ETH_RegisterRxAllocateCallback(&ctx->heth, HAL_ETH_RxAllocateCallback);

  /* Bind the PHY bus-IO set that targets THIS instance's heth. */
  if (ctx->heth.Instance == ETH1)
  {
    ctx->phy_io.Init     = ETH1_PHY_IO_Init;
    ctx->phy_io.ReadReg  = ETH1_PHY_IO_ReadReg;
    ctx->phy_io.WriteReg = ETH1_PHY_IO_WriteReg;
  }
  else
  {
    ctx->phy_io.Init     = ETH2_PHY_IO_Init;
    ctx->phy_io.ReadReg  = ETH2_PHY_IO_ReadReg;
    ctx->phy_io.WriteReg = ETH2_PHY_IO_WriteReg;
  }
  ctx->phy_io.DeInit  = ETH_PHY_IO_DeInit;
  ctx->phy_io.GetTick = ETH_PHY_IO_GetTick;

  /* Set PHY IO functions */
  RTL8211_RegisterBusIO(&ctx->phy, &ctx->phy_io);

  /* Initialize the RTL8211 ETH PHY */
  RTL8211_Init(&ctx->phy);

  ethernet_link_check_state(netif);
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
  eth_if_ctx_t *ctx = (eth_if_ctx_t *)netif->state;
  uint32_t i = 0U;
  struct pbuf *q = NULL;
  err_t errval = ERR_OK;
  ETH_BufferTypeDef Txbuffer[ETH_TX_DESC_CNT] = {0};

  memset(Txbuffer, 0 , ETH_TX_DESC_CNT * sizeof(ETH_BufferTypeDef));

  for(q = p; q != NULL; q = q->next)
  {
    if(i >= ETH_TX_DESC_CNT)
    {
      return ERR_IF;
    }

    Txbuffer[i].buffer = q->payload;
    Txbuffer[i].len = q->len;

    if(i > 0)
    {
      Txbuffer[i-1].next = &Txbuffer[i];
    }

    if(q->next == NULL)
    {
      Txbuffer[i].next = NULL;
    }

    i++;
  }

  ctx->TxConfig.Length = p->tot_len;
  ctx->TxConfig.TxBuffer = Txbuffer;
  ctx->TxConfig.pData = p;

  /* Keep the pbuf alive while the DMA owns it; HAL_ETH_TxFreeCallback() will
     release this reference once the descriptor is reclaimed. */
  pbuf_ref(p);

  /* Reclaim TX descriptors whose transmission already completed (this also
     frees their pbufs via HAL_ETH_TxFreeCallback). Without this, the
     ETH_TX_DESC_CNT descriptors are never released and transmission silently
     stops after a handful of frames. */
  HAL_ETH_ReleaseTxPacket(&ctx->heth);

  if (HAL_ETH_Transmit_IT(&ctx->heth, &ctx->TxConfig) != HAL_OK)
  {
    /* TX ring is full: reclaim completed descriptors and try once more. */
    HAL_ETH_ReleaseTxPacket(&ctx->heth);

    if (HAL_ETH_Transmit_IT(&ctx->heth, &ctx->TxConfig) != HAL_OK)
    {
      /* Still no room: drop this packet and undo our extra reference. */
      pbuf_free(p);
      errval = ERR_IF;
    }
  }

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
  eth_if_ctx_t *ctx = (eth_if_ctx_t *)netif->state;
  struct pbuf *p = NULL;

  if(RxAllocStatus == RX_ALLOC_OK)
  {
    HAL_ETH_ReadData(&ctx->heth, (void **)&p);
  }
  return p;
}

/**
  * @brief This function is the ethernetif_input task, it is processed when a packet
  * is ready to be read from the interface. It uses the function low_level_input()
  * that should handle the actual reception of bytes from the network
  * interface. Then the type of the received packet is determined and
  * the appropriate input function is called.
  *
  * @param netif the lwip network interface structure for this ethernetif
  */
void ethernetif_input(struct netif *netif)
{
  struct pbuf *p = NULL;
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
static int RX_POOL_is_inited;
err_t ethernetif_init(struct netif *netif)
{
  LWIP_ASSERT("netif != NULL", (netif != NULL));

  /* Cache the back-pointer so callbacks can reach the context from the netif. */
  ((eth_if_ctx_t *)netif->state)->netif = netif;

#if LWIP_NETIF_HOSTNAME
  /* Initialize interface hostname */
  netif->hostname = "lwip";
#endif /* LWIP_NETIF_HOSTNAME */

  netif->name[0] = IFNAME0;
  netif->name[1] = IFNAME1;
  /* We directly use etharp_output() here to save a function call.
   * You can instead declare your own function an call etharp_output()
   * from it if you have to do some checks before sending (e.g. if link
   * is available...) */
  netif->output = etharp_output;
  netif->linkoutput = low_level_output;

  /* Initialize the RX POOL */
  if(RX_POOL_is_inited == 0)
  {
    LWIP_MEMPOOL_INIT(RX_POOL);
  }
  RX_POOL_is_inited = 1;

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
  }
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

/*******************************************************************************
                       PHI IO Functions
*******************************************************************************/
/**
  * @brief  Initializes the MDIO interface GPIO and clocks (per instance).
  * @retval 0 if OK, -1 if ERROR
  */
static int32_t ETH1_PHY_IO_Init(void)
{
  /* MDIO GPIO configuration is done in HAL_ETH_MspInit(). */
  HAL_ETH_SetMDIOClockRange(&eth_ctx1.heth);
  return 0;
}

static int32_t ETH2_PHY_IO_Init(void)
{
  HAL_ETH_SetMDIOClockRange(&eth_ctx2.heth);
  return 0;
}

/**
  * @brief  De-Initializes the MDIO interface .
  * @param  None
  * @retval 0 if OK, -1 if ERROR
  */
static int32_t ETH_PHY_IO_DeInit (void)
{
  return 0;
}

/**
  * @brief  Read a PHY register through the MDIO interface (per instance).
  * @retval 0 if OK -1 if Error
  */
static int32_t ETH1_PHY_IO_ReadReg(uint32_t DevAddr, uint32_t RegAddr, uint32_t *pRegVal)
{
  return (HAL_ETH_ReadPHYRegister(&eth_ctx1.heth, DevAddr, RegAddr, pRegVal) != HAL_OK) ? -1 : 0;
}

static int32_t ETH2_PHY_IO_ReadReg(uint32_t DevAddr, uint32_t RegAddr, uint32_t *pRegVal)
{
  return (HAL_ETH_ReadPHYRegister(&eth_ctx2.heth, DevAddr, RegAddr, pRegVal) != HAL_OK) ? -1 : 0;
}

/**
  * @brief  Write a value to a PHY register through the MDIO interface.
  * @param  DevAddr: PHY port address
  * @param  RegAddr: PHY register address
  * @param  RegVal: Value to be written
  * @retval 0 if OK -1 if Error
  */
static int32_t ETH1_PHY_IO_WriteReg(uint32_t DevAddr, uint32_t RegAddr, uint32_t RegVal)
{
  return (HAL_ETH_WritePHYRegister(&eth_ctx1.heth, DevAddr, RegAddr, RegVal) != HAL_OK) ? -1 : 0;
}

static int32_t ETH2_PHY_IO_WriteReg(uint32_t DevAddr, uint32_t RegAddr, uint32_t RegVal)
{
  return (HAL_ETH_WritePHYRegister(&eth_ctx2.heth, DevAddr, RegAddr, RegVal) != HAL_OK) ? -1 : 0;
}

/**
  * @brief  Get the time in milliseconds used for internal PHY driver process.
  * @retval Time value
  */
static int32_t ETH_PHY_IO_GetTick(void)
{
  return (int32_t)HAL_GetTick();
}


/**
  * @brief
  * @retval None
  */
void ethernet_link_check_state(struct netif *netif)
{
  eth_if_ctx_t *ctx = (eth_if_ctx_t *)netif->state;
  ETH_MACConfigTypeDef MACConf = {0};
  int32_t PHYLinkState = 0U;
  uint32_t linkchanged = 0U, speed = 0U, duplex =0U;
  FunctionalState portselect = DISABLE;
  PHYLinkState = RTL8211_GetLinkState(&ctx->phy);

  /* Get link state */
  if(netif_is_link_up(netif) && (PHYLinkState <= RTL8211_STATUS_LINK_DOWN)){
    HAL_ETH_Stop_IT(&ctx->heth);
    netif_set_down(netif);
    netif_set_link_down(netif);
  }
  else
  {
    switch (PHYLinkState)
    {
  #if defined(RTL8211_SUPPORTED)
    case RTL8211_STATUS_1000MBITS_FULLDUPLEX:
    duplex = ETH_FULLDUPLEX_MODE;
    speed = ETH_SPEED_1000M;
    portselect = DISABLE;
    linkchanged = 1;
    break;
    case RTL8211_STATUS_1000MBITS_HALFDUPLEX:
    duplex = ETH_HALFDUPLEX_MODE;
    speed = ETH_SPEED_1000M;
    portselect = DISABLE;
    linkchanged = 1;
    break;
  #endif
    case RTL8211_STATUS_100MBITS_FULLDUPLEX:
        duplex = ETH_FULLDUPLEX_MODE;
        speed = ETH_SPEED_100M;
        portselect = ENABLE;
        linkchanged = 1;
        break;
    case RTL8211_STATUS_100MBITS_HALFDUPLEX:
        duplex = ETH_HALFDUPLEX_MODE;
        speed = ETH_SPEED_100M;
        portselect = ENABLE;
        linkchanged = 1;
        break;
    case RTL8211_STATUS_10MBITS_FULLDUPLEX:
        duplex = ETH_FULLDUPLEX_MODE;
        speed = ETH_SPEED_10M;
        portselect = ENABLE;
        linkchanged = 1;
        break;
    case RTL8211_STATUS_10MBITS_HALFDUPLEX:
        duplex = ETH_HALFDUPLEX_MODE;
        speed = ETH_SPEED_10M;
        portselect = ENABLE;
        linkchanged = 1;
        break;
    default:
        break;
    }

    if(linkchanged)
    {
      HAL_ETH_GetMACConfig(&ctx->heth, &MACConf);
      MACConf.DuplexMode = duplex;
      MACConf.Speed = speed;
      MACConf.PortSelect = portselect;
      HAL_ETH_SetMACConfig(&ctx->heth, &MACConf);
      HAL_ETH_Start_IT(&ctx->heth);
      netif_set_up(netif);
      netif_set_link_up(netif);
    }
  }
}

void HAL_ETH_RxAllocateCallback(uint8_t **buff)
{
/* USER CODE BEGIN HAL ETH RxAllocateCallback */
    struct pbuf_custom *p = LWIP_MEMPOOL_ALLOC(RX_POOL);
    if (p)
    {
      /* Get the buff from the struct pbuf address. */
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

#if defined(INSTRUCTION_CACHE_ENABLE) && (INSTRUCTION_CACHE_ENABLE == 1U)
  /* Invalidate data cache because Rx DMA's writing to physical memory makes it stale. */
  HAL_DCACHE_InvalidateByAddr(&hdcache,(uint32_t *)buff, Length);
#endif
}

void HAL_ETH_TxFreeCallback(uint32_t * buff)
{
/* USER CODE BEGIN HAL ETH TxFreeCallback */

  pbuf_free((struct pbuf *)buff);

/* USER CODE END HAL ETH TxFreeCallback */
}

