/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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
#include "tcp_echoclient.h"

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

EXTI_HandleTypeDef hexti;

struct netif gnetif;
extern ETH_HandleTypeDef heth;
extern ETH_TxPacketConfigTypeDef TxConfig;
extern ETH_DMADescTypeDef DMARxDscrTab[ETH_RX_DESC_CNT]; /* Ethernet Rx DMA Descriptors */
extern ETH_DMADescTypeDef DMATxDscrTab[ETH_TX_DESC_CNT]; /* Ethernet Tx DMA Descriptors */

/* USER CODE END PV */
/* Private function prototypes -----------------------------------------------*/
static void Netif_Config(void);
static void SystemClock_Config(void);
void UART_Config(void);
static void MX_GPIO_Init(void);
static void MX_ETH_Init(void);

#if defined(INSTRUCTION_CACHE_ENABLE) && (INSTRUCTION_CACHE_ENABLE == 1U)
static void MX_ICACHE_Init(void);
#endif
#if defined(DATA_CACHE_ENABLE) && (DATA_CACHE_ENABLE == 1U)
static void MX_DCACHE_Init(void);
#endif

/* USER CODE BEGIN PFP */
static void MX_EXTI2_Init(void);
static void Exti2FallingCb(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  Setup the network interface
 * @param  None
 * @retval None
 */
static void Netif_Config(void)
{
  ip_addr_t ipaddr;
  ip_addr_t netmask;
  ip_addr_t gw;

#if LWIP_DHCP
  /* If DHCP is enabled, set IP address, netmask, and gateway to zero */
  ip_addr_set_zero_ip4(&ipaddr);
  ip_addr_set_zero_ip4(&netmask);
  ip_addr_set_zero_ip4(&gw);
#else
  /* If DHCP is not enabled, use static IP address settings */
  /* IP address default setting */
  IP4_ADDR(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
  IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
  IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
  printf("DHCP is not enabled. Using static IP settings:\r\n");
  printf("IP Address: %s\r\n", ip4addr_ntoa(&ipaddr));
  printf("Netmask: %s\r\n", ip4addr_ntoa(&netmask));
  printf("Gateway: %s\r\n", ip4addr_ntoa(&gw));
#endif /* LWIP_DHCP */

  /* Add the network interface to the netif list */
  netif_add(&gnetif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &ethernet_input);

  /* Register the default network interface */
  netif_set_default(&gnetif);

  /* Update the link status of the network interface */
  ethernet_link_status_updated(&gnetif);

#if LWIP_NETIF_LINK_CALLBACK
  /* Set the link callback function to update the link status */
  netif_set_link_callback(&gnetif, ethernet_link_status_updated);
#endif /* LWIP_NETIF_LINK_CALLBACK */
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
  while(debug);
#endif /* DEBUG */
  uint32_t status;

  /* Configure the Instruction CACHE */
#if defined(INSTRUCTION_CACHE_ENABLE) && (INSTRUCTION_CACHE_ENABLE == 1U)
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

    status = BSP_PMIC_Init();
    if (status)
    {
      Error_Handler();
    }

    status = BSP_PMIC_Power_Mode_Init();
    if (status)
    {
      Error_Handler();
    }
  }
  else
  {
    SystemCoreClockUpdate();
  }

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  BSP_LED_Init(LED3);

  /* Configure USER2 push-button in interrupt mode */
  BSP_PB_Init(BUTTON_USER2, BUTTON_MODE_EXTI);

  /* Initialize all configured peripherals */
  MX_GPIO_Init();

  /* Configure EXTI2 (connected to PF.7 pin) in interrupt mode */
  MX_EXTI2_Init();

  UART_Config();
  MX_ETH_Init();

  printf("TCP Echo Client Started\r\n");

  /* Initialize the LwIP stack */
  lwip_init();

  /* Configure the Network interface */
  Netif_Config();

  /* TCP client connect */
  tcp_echoclient_connect();

  /* Infinite loop */
  while (1)
  {
    /* Read a received packet from the Ethernet buffers and send it
       to the lwIP for handling */
    ethernetif_input(&gnetif);

	/* Handle timeouts */
    sys_check_timeouts();

#if LWIP_NETIF_LINK_CALLBACK
    Ethernet_Link_Periodic_Handle(&gnetif);
#endif

#if LWIP_DHCP
    DHCP_Periodic_Handle(&gnetif);
#endif
  }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
static void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };

	/* PLL 2 is configured by DDR initialization code */
	/* PLL 3 is configured by GPU initialization code */
	RCC_PLLInitTypeDef RCC_Pll4InitStruct = { 0 };
	RCC_PLLInitTypeDef RCC_Pll5InitStruct = { 0 };
	RCC_PLLInitTypeDef RCC_Pll6InitStruct = { 0 };
	RCC_PLLInitTypeDef RCC_Pll7InitStruct = { 0 };
	RCC_PLLInitTypeDef RCC_Pll8InitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };
	RCC_PeriphCLKInitTypeDef ETH_CLKConfig = {0};

	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI
			| RCC_OSCILLATORTYPE_HSE |
			RCC_OSCILLATORTYPE_LSE | RCC_OSCILLATORTYPE_LSI;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.LSEState = RCC_LSE_OFF;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.LSIState = RCC_LSI_OFF;
	RCC_OscInitStruct.LSEDriveValue = RCC_LSEDRIVE_MEDIUMHIGH;

	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

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

	/* 532MHz */
	RCC_Pll5InitStruct.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_Pll5InitStruct.PLLMode = 0;
	RCC_Pll5InitStruct.FBDIV = 133;
	RCC_Pll5InitStruct.FREFDIV = 5;
	RCC_Pll5InitStruct.FRACIN = 0;
	RCC_Pll5InitStruct.POSTDIV1 = 1;
	RCC_Pll5InitStruct.POSTDIV2 = 2;
	RCC_Pll5InitStruct.PLLState = RCC_PLL_ON;

	/* 500MHz */
	RCC_Pll6InitStruct.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_Pll6InitStruct.PLLMode = 0;
	RCC_Pll6InitStruct.FBDIV = 25;
	RCC_Pll6InitStruct.FREFDIV = 1;
	RCC_Pll6InitStruct.FRACIN = 0;
	RCC_Pll6InitStruct.POSTDIV1 = 1;
	RCC_Pll6InitStruct.POSTDIV2 = 2;
	RCC_Pll6InitStruct.PLLState = RCC_PLL_ON;

	/* 835.512MHz */
	RCC_Pll7InitStruct.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_Pll7InitStruct.PLLMode = 0;
	RCC_Pll7InitStruct.FBDIV = 167;
	RCC_Pll7InitStruct.FREFDIV = 8;
	RCC_Pll7InitStruct.FRACIN = 1717047;
	RCC_Pll7InitStruct.POSTDIV1 = 1;
	RCC_Pll7InitStruct.POSTDIV2 = 1;
	RCC_Pll7InitStruct.PLLState = RCC_PLL_ON;

	/* 594MHz */
	RCC_Pll8InitStruct.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_Pll8InitStruct.PLLMode = 0;
	RCC_Pll8InitStruct.FBDIV = 297;
	RCC_Pll8InitStruct.FREFDIV = 10;
	RCC_Pll8InitStruct.FRACIN = 0;
	RCC_Pll8InitStruct.POSTDIV1 = 1;
	RCC_Pll8InitStruct.POSTDIV2 = 2;
	RCC_Pll8InitStruct.PLLState = RCC_PLL_ON;

	/* PLL 2 is configured by DDR initialization code */
	/* PLL 3 is configured by GPU initialization code */

	if (HAL_RCCEx_PLL4Config(&RCC_Pll4InitStruct) != HAL_OK) {
		Error_Handler();
	}

	if (HAL_RCCEx_PLL5Config(&RCC_Pll5InitStruct) != HAL_OK) {
		Error_Handler();
	}

	if (HAL_RCCEx_PLL6Config(&RCC_Pll6InitStruct) != HAL_OK) {
		Error_Handler();
	}

	if (HAL_RCCEx_PLL7Config(&RCC_Pll7InitStruct) != HAL_OK) {
		Error_Handler();
	}

	if (HAL_RCCEx_PLL8Config(&RCC_Pll8InitStruct) != HAL_OK) {
		Error_Handler();
	}

	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_ICN_HS_MCU
			| RCC_CLOCKTYPE_ICN_LS_MCU |
			RCC_CLOCKTYPE_ICN_SDMMC | RCC_CLOCKTYPE_ICN_DDR |
			RCC_CLOCKTYPE_ICN_DISPLAY | RCC_CLOCKTYPE_ICN_HCL |
			RCC_CLOCKTYPE_ICN_NIC | RCC_CLOCKTYPE_ICN_VID |
			RCC_CLOCKTYPE_ICN_APB1 | RCC_CLOCKTYPE_ICN_APB2 |
			RCC_CLOCKTYPE_ICN_APB3 | RCC_CLOCKTYPE_ICN_APB4 |
			RCC_CLOCKTYPE_ICN_APBDBG;

	RCC_ClkInitStruct.ICN_HS_MCU.XBAR_ClkSrc = RCC_XBAR_CLKSRC_PLL4;
	RCC_ClkInitStruct.ICN_HS_MCU.Div = 4;
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

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, 0) != HAL_OK) {
		Error_Handler();
	}
	  /* Set periph xbar to PLL6 and divide by 4 => final expected freq is 50 MHz */
	  ETH_CLKConfig.XBAR_Channel = RCC_PERIPHCLK_ETH1;
	  ETH_CLKConfig.XBAR_ClkSrc = RCC_XBAR_CLKSRC_PLL6;
	  ETH_CLKConfig.Div = 10;

	  if (HAL_RCCEx_PeriphCLKConfig(&ETH_CLKConfig) != HAL_OK)
	  {
	    Error_Handler();
	  }

	  ETH_CLKConfig.XBAR_Channel = RCC_PERIPHCLK_ETH1PTP_ETH2PTP;
	  ETH_CLKConfig.XBAR_ClkSrc = RCC_XBAR_CLKSRC_PLL4;
	  ETH_CLKConfig.Div = 6;

	  if (HAL_RCCEx_PeriphCLKConfig(&ETH_CLKConfig) != HAL_OK)
	  {
	    Error_Handler();
	  }
}

/**
  * @brief 	GPIO Initialization Function
  * @param 	None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/**
  * @brief  Configures EXTI line 7 (connected to PF.7 pin) in interrupt mode
  * @param  None
  * @retval None
  */
static void MX_EXTI2_Init(void)
{
  GPIO_InitTypeDef   GPIO_InitStruct;
  EXTI_ConfigTypeDef EXTI_ConfigStructure;

  /* Enable GPIOC clock */
  if (RESMGR_STATUS_ACCESS_OK == ResMgr_Request(RESMGR_RESOURCE_RIF_RCC, RESMGR_RCC_RESOURCE(95)))
  {
	  BUTTON_USER2_GPIO_CLK_ENABLE();
  }

  /* Acquire GPIOF7 using Resource manager */
  if (RESMGR_STATUS_ACCESS_OK != ResMgr_Request(BUTTON_USER2_RIF_RES_TYP_GPIO, BUTTON_USER2_RIF_RES_NUM_GPIO))
  {
	  Error_Handler();
  }

  /* Configure PF.7 pin as input floating */
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Pin = BUTTON_USER2_PIN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(BUTTON_USER2_GPIO_PORT, &GPIO_InitStruct);

  /* Set configuration except Interrupt and Event mask of Exti line 7*/
  EXTI_ConfigStructure.Line = BUTTON_USER2_EXTI_LINE;
  EXTI_ConfigStructure.Trigger = EXTI_TRIGGER_FALLING;
  EXTI_ConfigStructure.GPIOSel = EXTI_GPIOF;
  EXTI_ConfigStructure.Mode = EXTI_MODE_INTERRUPT;
  HAL_EXTI_SetConfigLine(&hexti, &EXTI_ConfigStructure);

  /* Register callback to treat Exti interrupts in user Exti2FallingCb function */
  HAL_EXTI_RegisterCallback(&hexti, HAL_EXTI_FALLING_CB_ID, Exti2FallingCb);

  /* Enable and set line 7 Interrupt to the lowest priority */
  HAL_NVIC_SetPriority(BUTTON_USER2_EXTI_IRQn, DEFAULT_IRQ_PRIO, 0);
  HAL_NVIC_EnableIRQ(BUTTON_USER2_EXTI_IRQn);
}


/**
  * @brief EXTI line detection callbacks
  * @param None:
  * @retval None
  */
static void Exti2FallingCb(void)
{
  /* send udp echo */
  tcp_echoclient_connect();
}

/**
 * @brief ETH Initialization Function
 * @param None
 * @retval None
 */
void MX_ETH_Init(void)
{
  /* USER CODE BEGIN ETH_Init 0 */

  /* USER CODE END ETH_Init 0 */

  static uint8_t MACAddr[6];

  /* USER CODE BEGIN ETH_Init 1 */

  /* USER CODE END ETH_Init 1 */
  heth.Instance = ETH1;
  MACAddr[0] = 0x00;
  MACAddr[1] = 0x80;
  MACAddr[2] = 0xE1;
  MACAddr[3] = 0x00;
  MACAddr[4] = 0x00;
  MACAddr[5] = 0x00;
  heth.Init.MACAddr = &MACAddr[0];
  heth.Init.MediaInterface = HAL_ETH_RMII_MODE;
  heth.Init.TxDesc = DMATxDscrTab;
  heth.Init.RxDesc = DMARxDscrTab;
  heth.Init.RxBuffLen = 1536;

  /* USER CODE BEGIN MACADDRESS */

  /* USER CODE END MACADDRESS */

  if (HAL_ETH_Init(&heth) != HAL_OK)
  {
    Error_Handler();
  }

  memset(&TxConfig, 0 , sizeof(ETH_TxPacketConfigTypeDef));
  TxConfig.Attributes = ETH_TX_PACKETS_FEATURES_CSUM | ETH_TX_PACKETS_FEATURES_CRCPAD;
  TxConfig.ChecksumCtrl = ETH_CHECKSUM_IPHDR_PAYLOAD_INSERT_PHDR_CALC;
  TxConfig.CRCPadCtrl = ETH_CRC_PAD_INSERT;
  /* USER CODE BEGIN ETH_Init 2 */

  /* USER CODE END ETH_Init 2 */

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

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
#if defined(__VALID_OUTPUT_TERMINAL_IO__) && defined(__GNUC__)
  initialise_monitor_handles();
#elif defined(__VALID_OUTPUT_UART_IO__)
  COM_InitTypeDef COM_Conf;

  COM_Conf.BaudRate = 115200;
  COM_Conf.HwFlowCtl = COM_HWCONTROL_NONE;
  COM_Conf.Parity = COM_PARITY_NONE;
  COM_Conf.StopBits = COM_STOPBITS_1;
  COM_Conf.WordLength = COM_WORDLENGTH_8B;

  BSP_COM_Init(COM_VCP_CM33, &COM_Conf);
#endif /* __VALID_OUTPUT_TERMINAL_IO__ or __VALID_OUTPUT_UART_IO__ */

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

#ifdef  USE_FULL_ASSERT
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
