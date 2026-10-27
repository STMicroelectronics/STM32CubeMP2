/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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
#include "lwip/opt.h"
#include "lwip/init.h"
#include "netif/etharp.h"
#include "lwip/netif.h"
#include "lwip/timeouts.h"
#if LWIP_DHCP
#include "lwip/dhcp.h"
#endif
#include "ethernetif.h"
#include "main.h"
#include "app_ethernet.h"
#include "tcp_echoserver.h"
#include "udp_echoserver.h"
#include "copro_sync.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
#ifdef DEBUG
volatile uint8_t debug = 1;
#endif /* DEBUG */

struct netif gnetif1;
struct netif gnetif2;
extern ETH_HandleTypeDef heth;
extern ETH_TxPacketConfigTypeDef TxConfig;
extern ETH_DMADescTypeDef DMARxDscrTab[ETH_RX_DESC_CNT]; /* Ethernet Rx DMA Descriptors */
extern ETH_DMADescTypeDef DMATxDscrTab[ETH_TX_DESC_CNT]; /* Ethernet Tx DMA Descriptors */

static uint8_t tcp_server_started = 0;
static uint8_t udp_server_started = 0;

#if defined(INSTRUCTION_CACHE_ENABLE) && (INSTRUCTION_CACHE_ENABLE == 1U)
static ICACHE_RegionConfigTypeDef region_config;
#endif
#if defined(DATA_CACHE_ENABLE) && (DATA_CACHE_ENABLE == 1U)
DCACHE_HandleTypeDef hdcache;
#endif

IPCC_HandleTypeDef hipcc1;

/* USER CODE END PV */
/* Private function prototypes -----------------------------------------------*/
static void Netif_Config(void);
static void Netif_Config_If(struct netif *netif, eth_if_ctx_t *ctx,
                            const ip4_addr_t *ip, const ip4_addr_t *nm,
                            const ip4_addr_t *gw, const char *tag);
void SystemClock_Config(void);
void UART_Config(void);
static void MX_ETH1_Init(void);
static void MX_ETH2_Init(void);
static void MX_IPCC_Init(void);
static void MX_ETH_DeInit(void);
#if defined(INSTRUCTION_CACHE_ENABLE) && (INSTRUCTION_CACHE_ENABLE == 1U)
static void MPU_Config(void);
#endif

#if defined(INSTRUCTION_CACHE_ENABLE) && (INSTRUCTION_CACHE_ENABLE == 1U)
static void MX_ICACHE_Init(void);
#endif
#if defined(DATA_CACHE_ENABLE) && (DATA_CACHE_ENABLE == 1U)
static void MX_DCACHE_Init(void);
#endif

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  Add and bring up one Ethernet interface with a static address.
 * @param  netif : the lwIP netif to configure
 * @param  ctx   : the per-instance Ethernet context (stored in netif->state)
 * @param  ip,nm,gw : static IPv4 address, netmask and gateway
 * @param  tag   : label used in the console trace
 * @retval None
 */
static void Netif_Config_If(struct netif *netif, eth_if_ctx_t *ctx,
                            const ip4_addr_t *ip, const ip4_addr_t *nm,
                            const ip4_addr_t *gw, const char *tag)
{
  /* Add the network interface */
  netif_add(netif, ip, nm, gw, ctx, &ethernetif_init, &ethernet_input);

#if LWIP_NETIF_LINK_CALLBACK
  netif_set_link_callback(netif, ethernet_link_status_updated);
#endif /* LWIP_NETIF_LINK_CALLBACK */

  ethernet_link_status_updated(netif);
}

/**
 * @brief  Setup both network interfaces.
 * @param  None
 * @retval None
 */
static void Netif_Config(void)
{
  ip4_addr_t ip, nm, gw;

  /* ETH1 : TCP echo server. */
  IP4_ADDR(&ip, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
  IP4_ADDR(&nm, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
  IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
  Netif_Config_If(&gnetif1, &eth_ctx1, &ip, &nm, &gw, "ETH1/TCP");

  /* ETH2 : UDP echo server. */
  IP4_ADDR(&ip, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
  IP4_ADDR(&nm, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
  IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
  Netif_Config_If(&gnetif2, &eth_ctx2, &ip, &nm, &gw, "ETH2/UDP");

  /* Default route for client-initiated egress goes via ETH1. */
  netif_set_default(&gnetif1);
}

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */
#ifdef DEBUG
  while (debug);
#endif /* DEBUG */

  /* Configure the Instruction CACHE */
#if defined(INSTRUCTION_CACHE_ENABLE) && (INSTRUCTION_CACHE_ENABLE == 1U)
  MPU_Config();
  MX_ICACHE_Init();
#endif
  /* Configure the Data CACHE */
#if defined(DATA_CACHE_ENABLE) && (DATA_CACHE_ENABLE == 1U)
  MX_DCACHE_Init();
#endif

  /* Reset of all peripherals, Initialize the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  if (IS_DEVELOPER_BOOT_MODE())
  {
    SystemClock_Config();
  }
  else
  {
    SystemCoreClockUpdate();
  }

  if (IS_DEVELOPER_BOOT_MODE())
  {
#if (USE_STPMIC2x) && !defined(USE_DDR)
    /* Configure PMIC */
    BSP_PMIC_Init();
#endif
  }

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  BSP_LED_Init(LED3);

  if (!IS_DEVELOPER_BOOT_MODE())
  {
    MX_IPCC_Init();
    CoproSync_Init();
  }

  /* Initialize all configured peripherals */
  UART_Config();

  /* Initialize the ethernet peripherals */
  MX_ETH1_Init();
  MX_ETH2_Init();

  printf("TCP UDP Echo Server Started\r\n");

  /* Initialize the LwIP stack */
  lwip_init();

  /* Configure both network interfaces */
  Netif_Config();

  /* Infinite loop */
  while (1)
  {
    /* Read a received packet from the Ethernet buffers and send it
       to the lwIP for handling */
    ethernetif_input(&gnetif1);
    ethernetif_input(&gnetif2);

    /* Handle timeouts */
    sys_check_timeouts();

#if LWIP_NETIF_LINK_CALLBACK
    Ethernet_Link_Periodic_Handle(&gnetif1);
    Ethernet_Link_Periodic_Handle(&gnetif2);
#endif

#if LWIP_DHCP
    DHCP_Periodic_Handle(&gnetif1);
    DHCP_Periodic_Handle(&gnetif2);

    /* Start TCP server after DHCP address assigned */
    if (!tcp_server_started)
    {
      eth_if_ctx_t *ctx1 = (eth_if_ctx_t *)gnetif1.state;
      if ((ctx1 != NULL) && (ctx1->DHCP_state == DHCP_ADDRESS_ASSIGNED))
      {
        tcp_echoserver_init(&gnetif1);
        tcp_server_started = 1;
        printf("TCP echo server started on ETH1\r\n");
      }
    }

    /* Start UDP server after DHCP address assigned */
    if (!udp_server_started)
    {
      eth_if_ctx_t *ctx2 = (eth_if_ctx_t *)gnetif2.state;
      if ((ctx2 != NULL) && (ctx2->DHCP_state == DHCP_ADDRESS_ASSIGNED))
      {
        udp_echoserver_init(&gnetif2);
        udp_server_started = 1;
        printf("UDP echo server started on ETH2\r\n");
      }
    }

#else
    /* Start servers with static IP */
    if (!tcp_server_started)
    {
      tcp_echoserver_init(&gnetif1);
      tcp_server_started = 1;
      printf("TCP echo server started on ETH1\r\n");
    }

    if (!udp_server_started)
    {
      udp_echoserver_init(&gnetif2);
      udp_server_started = 1;
      printf("UDP echo server started on ETH2\r\n");
    }
#endif
  }
}

/**
 * @brief ETH1 Initialization Function
 * @param None
 * @retval None
 */
void MX_ETH1_Init(void)
{
  /* USER CODE BEGIN ETH_Init 0 */

  /* USER CODE END ETH_Init 0 */

  ETH_HandleTypeDef *heth = &eth_ctx1.heth;
  static uint8_t MACAddr[6];

  /* USER CODE BEGIN ETH_Init 1 */

  /* USER CODE END ETH_Init 1 */
  heth->Instance = ETH1;
  MACAddr[0] = 0x00;
  MACAddr[1] = 0x80;
  MACAddr[2] = 0xE1;
  MACAddr[3] = 0x00;
  MACAddr[4] = 0x00;
  MACAddr[5] = 0x00;
  heth->Init.MACAddr = &MACAddr[0];
  heth->Init.MediaInterface = HAL_ETH_RGMII_MODE;
  heth->Init.TxDesc = eth_ctx1.DMATxDscrTab;
  heth->Init.RxDesc = eth_ctx1.DMARxDscrTab;
  heth->Init.RxBuffLen = 1536;

  /* USER CODE BEGIN MACADDRESS */

  /* USER CODE END MACADDRESS */

  if (HAL_ETH_Init(heth) != HAL_OK)
  {
    Error_Handler();
  }

  memset(&eth_ctx1.TxConfig, 0, sizeof(ETH_TxPacketConfigTypeDef));
  eth_ctx1.TxConfig.Attributes = ETH_TX_PACKETS_FEATURES_CSUM | ETH_TX_PACKETS_FEATURES_CRCPAD;
  eth_ctx1.TxConfig.ChecksumCtrl = ETH_CHECKSUM_IPHDR_PAYLOAD_INSERT_PHDR_CALC;
  eth_ctx1.TxConfig.CRCPadCtrl = ETH_CRC_PAD_INSERT;
  /* USER CODE BEGIN ETH_Init 2 */

  /* USER CODE END ETH_Init 2 */
}

void MX_ETH2_Init(void)
{
  /* USER CODE BEGIN ETH_Init 0 */

  /* USER CODE END ETH_Init 0 */

  ETH_HandleTypeDef *heth = &eth_ctx2.heth;
  static uint8_t MACAddr[6];

  /* USER CODE BEGIN ETH_Init 1 */

  /* USER CODE END ETH_Init 1 */
  heth->Instance = ETH2;
  MACAddr[0] = 0x00;
  MACAddr[1] = 0x80;
  MACAddr[2] = 0xE1;
  MACAddr[3] = 0x00;
  MACAddr[4] = 0x00;
  MACAddr[5] = 0x01;
  heth->Init.MACAddr = &MACAddr[0];
  heth->Init.MediaInterface = HAL_ETH_RGMII_MODE;
  heth->Init.TxDesc = eth_ctx2.DMATxDscrTab;
  heth->Init.RxDesc = eth_ctx2.DMARxDscrTab;
  heth->Init.RxBuffLen = 1536;

  /* USER CODE BEGIN MACADDRESS */

  /* USER CODE END MACADDRESS */

  if (HAL_ETH_Init(heth) != HAL_OK)
  {
    Error_Handler();
  }

  memset(&eth_ctx2.TxConfig, 0, sizeof(ETH_TxPacketConfigTypeDef));
  eth_ctx2.TxConfig.Attributes = ETH_TX_PACKETS_FEATURES_CSUM | ETH_TX_PACKETS_FEATURES_CRCPAD;
  eth_ctx2.TxConfig.ChecksumCtrl = ETH_CHECKSUM_IPHDR_PAYLOAD_INSERT_PHDR_CALC;
  eth_ctx2.TxConfig.CRCPadCtrl = ETH_CRC_PAD_INSERT;
  /* USER CODE BEGIN ETH_Init 2 */

  /* USER CODE END ETH_Init 2 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
#if !defined(USE_DDR)
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
#endif
  /* PLL 1 is not to be configured here because M33 only
     application or already configured by an A35 application */
  /* PLL 2 is configured by DDR initialization code */
  /* PLL 3 is configured by GPU initialization code */
  RCC_PLLInitTypeDef RCC_Pll4InitStruct = {0};
  RCC_PLLInitTypeDef RCC_Pll5InitStruct = {0};
  RCC_PLLInitTypeDef RCC_Pll6InitStruct = {0};
  RCC_PLLInitTypeDef RCC_Pll7InitStruct = {0};
  RCC_PLLInitTypeDef RCC_Pll8InitStruct = {0};
  RCC_PeriphCLKInitTypeDef ETH_CLKConfig = {0};

#if !defined(USE_DDR)
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_HSE |
                                     RCC_OSCILLATORTYPE_LSE | RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_OFF;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_OFF;
  RCC_OscInitStruct.LSEDriveValue = RCC_LSEDRIVE_MEDIUMHIGH;

  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

#endif /* !USE_DDR */
  /* PLL 1 is not to be configured here because M33 only
     application or already configured by an A35 application */
  /* PLL 2 is configured by DDR initialization code */
  /* PLL 3 is configured by GPU initialization code */

  /* 1200MHz */
  RCC_Pll4InitStruct.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_Pll4InitStruct.PLLMode = 0;
  RCC_Pll4InitStruct.FBDIV = 30;
  RCC_Pll4InitStruct.FREFDIV = 1;
  RCC_Pll4InitStruct.FRACIN = 0;
  RCC_Pll4InitStruct.POSTDIV1 = 1;
  RCC_Pll4InitStruct.POSTDIV2 = 1;
  RCC_Pll4InitStruct.PLLState = RCC_PLL_ON;

  /* 500MHz */
  RCC_Pll6InitStruct.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_Pll6InitStruct.PLLMode = 0;
  RCC_Pll6InitStruct.FBDIV = 25;
  RCC_Pll6InitStruct.FREFDIV = 1;
  RCC_Pll6InitStruct.FRACIN = 0;
  RCC_Pll6InitStruct.POSTDIV1 = 1;
  RCC_Pll6InitStruct.POSTDIV2 = 2;
  RCC_Pll6InitStruct.PLLState = RCC_PLL_ON;

  RCC_Pll7InitStruct.PLLState = RCC_PLL_OFF;
  RCC_Pll8InitStruct.PLLState = RCC_PLL_OFF;

  /* PLL 1 is not to be configured here because M33 only
     application or already configured by an A35 application */
  /* PLL 2 is configured by DDR initialization code */
  /* PLL 3 is configured by GPU initialization code */

  if (HAL_RCCEx_PLL4Config(&RCC_Pll4InitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_RCCEx_PLL5Config(&RCC_Pll5InitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_RCCEx_PLL6Config(&RCC_Pll6InitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_RCCEx_PLL7Config(&RCC_Pll7InitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_RCCEx_PLL8Config(&RCC_Pll8InitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_ICN_HS_MCU | RCC_CLOCKTYPE_ICN_LS_MCU |
                                RCC_CLOCKTYPE_ICN_SDMMC | RCC_CLOCKTYPE_ICN_DDR |
                                RCC_CLOCKTYPE_ICN_DISPLAY | RCC_CLOCKTYPE_ICN_HCL |
                                RCC_CLOCKTYPE_ICN_NIC | RCC_CLOCKTYPE_ICN_VID |
                                RCC_CLOCKTYPE_ICN_APB1 | RCC_CLOCKTYPE_ICN_APB2 |
                                RCC_CLOCKTYPE_ICN_APB3 | RCC_CLOCKTYPE_ICN_APB4 |
                                RCC_CLOCKTYPE_ICN_APBDBG;

  RCC_ClkInitStruct.ICN_HS_MCU.XBAR_ClkSrc = RCC_XBAR_CLKSRC_PLL4;
  RCC_ClkInitStruct.ICN_HS_MCU.Div = 3;
  RCC_ClkInitStruct.ICN_SDMMC.XBAR_ClkSrc = RCC_XBAR_CLKSRC_PLL4;
  RCC_ClkInitStruct.ICN_SDMMC.Div = 6;
  RCC_ClkInitStruct.ICN_DDR.XBAR_ClkSrc = RCC_XBAR_CLKSRC_PLL4;
  RCC_ClkInitStruct.ICN_DDR.Div = 2;
  RCC_ClkInitStruct.ICN_DISPLAY.XBAR_ClkSrc = RCC_XBAR_CLKSRC_PLL4;
  RCC_ClkInitStruct.ICN_DISPLAY.Div = 3;
  RCC_ClkInitStruct.ICN_HCL.XBAR_ClkSrc = RCC_XBAR_CLKSRC_PLL4;
  RCC_ClkInitStruct.ICN_HCL.Div = 4;
  RCC_ClkInitStruct.ICN_NIC.XBAR_ClkSrc = RCC_XBAR_CLKSRC_PLL4;
  RCC_ClkInitStruct.ICN_NIC.Div = 3;
  RCC_ClkInitStruct.ICN_VID.XBAR_ClkSrc = RCC_XBAR_CLKSRC_PLL4;
  RCC_ClkInitStruct.ICN_VID.Div = 2;
  RCC_ClkInitStruct.ICN_LSMCU_Div = RCC_LSMCU_DIV2;
  RCC_ClkInitStruct.APB1_Div = RCC_APB1_DIV1;
  RCC_ClkInitStruct.APB2_Div = RCC_APB2_DIV1;
  RCC_ClkInitStruct.APB3_Div = RCC_APB3_DIV1;
  RCC_ClkInitStruct.APB4_Div = RCC_APB4_DIV1;
  RCC_ClkInitStruct.APBDBG_Div = RCC_APBDBG_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* Set periph xbar to PLL6 and divide by 4 => final expected freq is 125 MHz */
  ETH_CLKConfig.XBAR_Channel = RCC_PERIPHCLK_ETH1_ETHSW;
  ETH_CLKConfig.XBAR_ClkSrc = RCC_XBAR_CLKSRC_PLL6;
  ETH_CLKConfig.Div = 4;

  if (HAL_RCCEx_PeriphCLKConfig(&ETH_CLKConfig) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

static void MX_IPCC_Init(void)
{
  hipcc1.Instance = IPCC1;
  if (HAL_IPCC_Init(&hipcc1) != HAL_OK)
  {
    Error_Handler();
  }
  HAL_NVIC_SetPriority(IPCC1_RX_IRQn, DEFAULT_IRQ_PRIO, 0);
  HAL_NVIC_EnableIRQ(IPCC1_RX_IRQn);
}

static void MX_ETH_DeInit(void)
{
  HAL_ETH_Stop_IT(&eth_ctx1.heth);
  HAL_ETH_DeInit(&eth_ctx1.heth);
  HAL_ETH_Stop_IT(&eth_ctx2.heth);
  HAL_ETH_DeInit(&eth_ctx2.heth);
}

void CoproSync_ShutdownCb(IPCC_HandleTypeDef * hipcc, uint32_t ChannelIndex,
                          IPCC_CHANNELDirTypeDef ChannelDir)
{
  MX_ETH_DeInit();
  BSP_LED_DeInit(LED3);
#if defined(DATA_CACHE_ENABLE) && (DATA_CACHE_ENABLE == 1U)
  HAL_DCACHE_CleanInvalidByAddr(&hdcache, (const uint32_t *)0x80A00000U, 0x003FFC00U);
  HAL_DCACHE_Disable(&hdcache);
  HAL_DCACHE_DeInit(&hdcache);
#endif
#if defined(INSTRUCTION_CACHE_ENABLE) && (INSTRUCTION_CACHE_ENABLE == 1U)
  HAL_ICACHE_DeInit();
#endif
  HAL_IPCC_NotifyCPU(hipcc, ChannelIndex, IPCC_CHANNEL_DIR_RX);
  while (1);
}

/* USER CODE END 4 */
#if defined(INSTRUCTION_CACHE_ENABLE) && (INSTRUCTION_CACHE_ENABLE == 1U)
/**
 * @brief ICACHE Initialization Function
 * @param None
 * @retval None
 */
static void MX_ICACHE_Init(void)
{

  /* USER CODE BEGIN ICACHE_Init 0 */

  /* USER CODE END ICACHE_Init 0 */

  /* USER CODE BEGIN ICACHE_Init 1 */

  /* USER CODE END ICACHE_Init 1 */

  /* Enable instruction cache in 1-way (direct mapped cache) */
  if (HAL_ICACHE_ConfigAssociativityMode(ICACHE_1WAY) != HAL_OK)
  {
    Error_Handler();
  }

  /* Disable Instruction cache prior to enable the remapped region */
  HAL_ICACHE_Disable();

  /* Remap external flash memory to C-Bus */
  region_config.BaseAddress = 0x10000000;
  region_config.RemapAddress = 0x80100600;
  region_config.TrafficRoute = ICACHE_MASTER2_PORT;
  region_config.OutputBurstType = ICACHE_OUTPUT_BURST_INCR;
  region_config.Size = ICACHE_REGIONSIZE_2MB;

  /* Enable the remapped region */
  if (HAL_ICACHE_EnableRemapRegion(ICACHE_REGION_0, &region_config) != HAL_OK)
  {
    Error_Handler();
  }

  /* Enable the Instruction and Data Cache */
  if (HAL_ICACHE_Enable() != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ICACHE_Init 2 */

  /* USER CODE END ICACHE_Init 2 */
}
#endif

#if defined(DATA_CACHE_ENABLE) && (DATA_CACHE_ENABLE == 1U)
/**
 * @brief DCACHE Initialization Function
 * @param None
 * @retval None
 */
static void MX_DCACHE_Init(void)
{

  /* USER CODE BEGIN ICACHE_Init 0 */

  /* USER CODE END ICACHE_Init 0 */

  /* USER CODE BEGIN ICACHE_Init 1 */

  /* USER CODE END ICACHE_Init 1 */

  /** Enable data cache in burst wrap */
  hdcache.Instance = DCACHE;
  hdcache.Init.ReadBurstType = DCACHE_READ_BURST_WRAP;

  if (HAL_DCACHE_Init(&hdcache) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ICACHE_Init 2 */

  /* USER CODE END ICACHE_Init 2 */
}

/**
 * @brief  Configure the MPU attributes
 * @param  None
 * @retval None
 */
static void MPU_Config(void)
{

  MPU_Region_InitTypeDef MPU_InitStruct;
  MPU_Attributes_InitTypeDef MPU_Attributes_InitStruct;

  HAL_MPU_Disable();

  /* write back, read and write allocate */
  MPU_Attributes_InitStruct.Attributes = INNER_OUTER(MPU_WRITE_BACK | MPU_NON_TRANSIENT | MPU_RW_ALLOCATE);
  MPU_Attributes_InitStruct.Number = MPU_ATTRIBUTES_NUMBER0;
  HAL_MPU_ConfigMemoryAttributes(&MPU_Attributes_InitStruct);

  /* ICACHE */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.AttributesIndex = MPU_ATTRIBUTES_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x00000000;
  MPU_InitStruct.LimitAddress = 0x00010000;
  MPU_InitStruct.AccessPermission = MPU_REGION_ALL_RO;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /* DCACHE */
  MPU_InitStruct.AccessPermission = MPU_REGION_ALL_RW;
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.AttributesIndex = MPU_ATTRIBUTES_NUMBER0;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_INNER_SHAREABLE;
  MPU_InitStruct.BaseAddress = 0x80A00000;
  MPU_InitStruct.LimitAddress = 0x811FFFFF;
  MPU_InitStruct.Number = MPU_REGION_NUMBER1;
  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  MPU_Attributes_InitStruct.Attributes = INNER_OUTER(MPU_NOT_CACHEABLE);
  MPU_Attributes_InitStruct.Number = MPU_ATTRIBUTES_NUMBER2;
  HAL_MPU_ConfigMemoryAttributes(&MPU_Attributes_InitStruct);

  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.AttributesIndex = MPU_ATTRIBUTES_NUMBER2;
  MPU_InitStruct.BaseAddress = 0x0A060000;
  MPU_InitStruct.LimitAddress = 0x0A080000;
  MPU_InitStruct.AccessPermission = MPU_REGION_ALL_RW;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_INNER_SHAREABLE;
  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /* 3. Non-cacheable region for IPC/SHMEM (if needed) */
  MPU_Attributes_InitStruct.Attributes = INNER_OUTER(MPU_NOT_CACHEABLE);
  MPU_Attributes_InitStruct.Number = MPU_ATTRIBUTES_NUMBER2;
  HAL_MPU_ConfigMemoryAttributes(&MPU_Attributes_InitStruct);

  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER1;
  MPU_InitStruct.AttributesIndex = MPU_ATTRIBUTES_NUMBER2;
  MPU_InitStruct.BaseAddress = 0x81200000;
  MPU_InitStruct.LimitAddress = 0x812FFFFF;
  MPU_InitStruct.AccessPermission = MPU_REGION_ALL_RW;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_INNER_SHAREABLE;
  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

#endif

void UART_Config(void)
{
  /* UARTx configured as follow:
    - BaudRate = 115200 baud
    - Word Length = 8 Bits
    - One Stop Bit
    - No parity
    - Hardware flow control disabled (RTS and CTS signals)
    - Receive and transmit enabled
   */

  COM_InitTypeDef COM_Init;
  COM_Init.BaudRate = 115200;
  COM_Init.Parity = UART_PARITY_NONE;
  COM_Init.StopBits = UART_STOPBITS_1;
  COM_Init.WordLength = UART_WORDLENGTH_8B;
  COM_Init.HwFlowCtl = UART_HWCONTROL_NONE;
  BSP_COM_Init(COM1, &COM_Init);
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* Error if LED3 is ON */
  BSP_LED_On(LED3);
  while (1)
  {
    HAL_Delay(1000);
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* Infinite loop */
  while (1)
  {
  }
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
