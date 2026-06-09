/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "app_freertos.h"
#include "cmsis_os2.h"
#include "lwipopts.h"

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

ETH_DMADescTypeDef  __attribute__((section (".TxDecripSection"))) DMATxDscrTab[ETH_TX_DESC_CNT] ; /* Ethernet Tx DMA Descriptors */
ETH_DMADescTypeDef  __attribute__((section (".RxDecripSection"))) DMARxDscrTab[ETH_RX_DESC_CNT] ; /* Ethernet Rx DMA Descriptors */

ETH_TxPacketConfigTypeDef TxConfig;

ETH_HandleTypeDef heth;
#if defined(INSTRUCTION_CACHE_ENABLE) && (INSTRUCTION_CACHE_ENABLE == 1U)
static ICACHE_RegionConfigTypeDef region_config;
#endif
#if defined(DATA_CACHE_ENABLE) && (DATA_CACHE_ENABLE == 1U)
DCACHE_HandleTypeDef hdcache;
#endif

static uint8_t *ext_flash_addr;
extern uint32_t __extflash_region_init_base;

/* USER CODE END PV */
/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void UART_Config(void);

static void MX_ETH_Init(void);

#if (INSTRUCTION_CACHE_ENABLE == 1U) && (DATA_CACHE_ENABLE == 1U)
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
  ext_flash_addr = (uint8_t *)(&__extflash_region_init_base);

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
  if(IS_DEVELOPER_BOOT_MODE())
  {
    SystemClock_Config();
  }
  else
  {
    SystemCoreClockUpdate();
  }

  if(IS_DEVELOPER_BOOT_MODE())
  {
#if (USE_STPMIC2x) && !defined(USE_DDR)
    /* Configure PMIC */
    BSP_PMIC_Init();
#endif
  }

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  BSP_LED_Init(LED3);

  UART_Config();

  printf("\r\nlwIP Example started\r\n");

  MX_ETH_Init();

  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  osKernelInitialize();  /* Call init function for freertos objects (in freertos.c) */
  MX_FREERTOS_Init();

  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
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

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_ICN_HS_MCU  | RCC_CLOCKTYPE_ICN_LS_MCU |
                                RCC_CLOCKTYPE_ICN_SDMMC   | RCC_CLOCKTYPE_ICN_DDR    |
                                RCC_CLOCKTYPE_ICN_DISPLAY | RCC_CLOCKTYPE_ICN_HCL    |
                                RCC_CLOCKTYPE_ICN_NIC     | RCC_CLOCKTYPE_ICN_VID    |
                                RCC_CLOCKTYPE_ICN_APB1    | RCC_CLOCKTYPE_ICN_APB2   |
                                RCC_CLOCKTYPE_ICN_APB3    | RCC_CLOCKTYPE_ICN_APB4   |
                                RCC_CLOCKTYPE_ICN_APBDBG;

  RCC_ClkInitStruct.ICN_HS_MCU.XBAR_ClkSrc  = RCC_XBAR_CLKSRC_PLL4;
  RCC_ClkInitStruct.ICN_HS_MCU.Div          = 3;
  RCC_ClkInitStruct.ICN_SDMMC.XBAR_ClkSrc   = RCC_XBAR_CLKSRC_PLL4;
  RCC_ClkInitStruct.ICN_SDMMC.Div           = 6;
  RCC_ClkInitStruct.ICN_DDR.XBAR_ClkSrc     = RCC_XBAR_CLKSRC_PLL4;
  RCC_ClkInitStruct.ICN_DDR.Div             = 2;
  RCC_ClkInitStruct.ICN_DISPLAY.XBAR_ClkSrc = RCC_XBAR_CLKSRC_PLL4;
  RCC_ClkInitStruct.ICN_DISPLAY.Div         = 3;
  RCC_ClkInitStruct.ICN_HCL.XBAR_ClkSrc     = RCC_XBAR_CLKSRC_PLL4;
  RCC_ClkInitStruct.ICN_HCL.Div             = 4;
  RCC_ClkInitStruct.ICN_NIC.XBAR_ClkSrc     = RCC_XBAR_CLKSRC_PLL4;
  RCC_ClkInitStruct.ICN_NIC.Div             = 3;
  RCC_ClkInitStruct.ICN_VID.XBAR_ClkSrc     = RCC_XBAR_CLKSRC_PLL4;
  RCC_ClkInitStruct.ICN_VID.Div             = 2;
  RCC_ClkInitStruct.ICN_LSMCU_Div           = RCC_LSMCU_DIV2;
  RCC_ClkInitStruct.APB1_Div                = RCC_APB1_DIV1;
  RCC_ClkInitStruct.APB2_Div                = RCC_APB2_DIV1;
  RCC_ClkInitStruct.APB3_Div                = RCC_APB3_DIV1;
  RCC_ClkInitStruct.APB4_Div                = RCC_APB4_DIV1;
  RCC_ClkInitStruct.APBDBG_Div              = RCC_APBDBG_DIV1;
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
  heth.Init.MediaInterface = HAL_ETH_RGMII_MODE;
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
  /* USER CODE BEGIN ETH_Init 2 */

  /* USER CODE END ETH_Init 2 */

}

/* USER CODE BEGIN 4 */

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
  region_config.BaseAddress     = 0x10000000;
  region_config.RemapAddress    = 0x80100600;
  region_config.TrafficRoute    = ICACHE_MASTER2_PORT;
  region_config.OutputBurstType = ICACHE_OUTPUT_BURST_INCR;
  region_config.Size            = ICACHE_REGIONSIZE_2MB;

  /* Enable the remapped region */
  if(HAL_ICACHE_EnableRemapRegion(ICACHE_REGION_0, &region_config) != HAL_OK)
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
  COM_Init.BaudRate   = 115200;
  COM_Init.Parity     = UART_PARITY_NONE;
  COM_Init.StopBits   = UART_STOPBITS_1;
  COM_Init.WordLength = UART_WORDLENGTH_8B;
  COM_Init.HwFlowCtl  = UART_HWCONTROL_NONE;
  BSP_COM_Init(COM1, &COM_Init);
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM6) {
    HAL_IncTick();
  }
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();

  BSP_LED_Off(LED_BLUE);

  while (1)
  {
    BSP_LED_Toggle(LED_RED);
    osDelay(20);
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



