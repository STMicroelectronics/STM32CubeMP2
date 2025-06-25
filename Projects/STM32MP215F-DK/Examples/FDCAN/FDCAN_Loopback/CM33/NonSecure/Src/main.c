/**
  ******************************************************************************
  * @file    FDCAN/FDCAN_Loopback/Src/main.c
  * @author  MCD Application Team
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "copro_sync.h"

/** @addtogroup STM32MP2xx_HAL_Examples
  * @{
  */

/** @addtogroup FDCAN_Loopback
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
EXTI_HandleTypeDef hexti;
FDCAN_HandleTypeDef hfdcan;
FDCAN_FilterTypeDef sFilterConfig;
FDCAN_TxHeaderTypeDef TxHeader;
FDCAN_RxHeaderTypeDef RxHeader;
IPCC_HandleTypeDef   hipcc;
uint8_t TxData0[] = {0x10, 0x32, 0x54, 0x76, 0x98, 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66};
uint8_t TxData1[] = {0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};
uint8_t TxData2[] = {0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00};
uint8_t RxData[12];
__IO uint8_t UserButtonClickEvent = RESET;

#ifdef DEBUG
volatile uint8_t debug = 1;
#endif /* DEBUG */

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_IPCC_Init(void);
static void MX_FDCAN_Init(void);
static void MX_FDCAN_DeInit(void);
static uint32_t BufferCmp8b(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  The application entry point.
  * @param  None
  * @retval int
  */
int main(void)
{
  /* MCU Configuration--------------------------------------------------------*/

  /* STM32MP2xx HAL library initialization:
       - Systick timer is configured by default as source of time base, but user
             can eventually implement his proper time base source (a general purpose
             timer for example or other time source), keeping in mind that Time base
             duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
             handled in milliseconds basis.
       - Set NVIC Group Priority to 4
       - Low Level Initialization
     */
#ifdef DEBUG
  while(debug);
#endif /* DEBUG */

  uint32_t status;

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

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


  /* Configure LED3 */
  BSP_LED_Init(LED3);

  if(!IS_DEVELOPER_BOOT_MODE())
  {
    /* IPCC initialization */
    MX_IPCC_Init();
    /*Corpo Sync Initialization*/
    CoproSync_Init();
  }

  /* Initialize all configured peripherals */
  BSP_PB_Init(BUTTON_USER2, BUTTON_MODE_EXTI);

  /* Initialize all configured peripherals */
  MX_GPIO_Init();

  MX_FDCAN_Init();


  /* Infinite loop */
  while (1)
  {
    /* Toggle LED3 */
    BSP_LED_Toggle(LED3);
    HAL_Delay(500);
  }

}

/**
  * @brief  System Clock Configuration
  * @param  None
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

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, 0) != HAL_OK) {
		Error_Handler();
	}

}

/**
  * @brief 	FDCAN Initialization Function
  * @param 	None
  * @retval None
  */
static void MX_FDCAN_Init(void)
{
	/* Acquire FDCAN using Resource manager */
	if (RESMGR_STATUS_ACCESS_OK == ResMgr_Request(RESMGR_RESOURCE_RIFSC, STM32MP21_RIFSC_FDCAN_ID))
	{
		/* Enable FDCANx clock */
		FDCANx_CLK_ENABLE();
	}

	/* Acquire GPIOC3 using Resource manager */
	if (RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIF_GPIOC, RESMGR_GPIO_PIN(3)))
	{
		Error_Handler();
	}

	/* Acquire GPIOA5 using Resource manager */
	if (RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIF_GPIOA, RESMGR_GPIO_PIN(5)))
	{
		Error_Handler();
	}

	/* Enable GPIOs power supplies */
	if (RESMGR_STATUS_ACCESS_OK == ResMgr_Request(RESMGR_RESOURCE_RIF_RCC, RESMGR_RCC_RESOURCE(96)))
	{
		FDCANx_TX_GPIO_CLK_ENABLE();
	}
	/* Initializes the FDCAN peripheral in loopback mode */
	hfdcan.Instance = FDCANx;
	hfdcan.Init.FrameFormat = FDCAN_FRAME_FD_BRS;
	hfdcan.Init.Mode = FDCAN_MODE_EXTERNAL_LOOPBACK;
	hfdcan.Init.AutoRetransmission = ENABLE;
	hfdcan.Init.TransmitPause = DISABLE;
	hfdcan.Init.ProtocolException = ENABLE;
	/* Bit time configuration:
		************************
		Bit time parameter         | Nominal      |  Data
		---------------------------|--------------|----------------
		fdcan_ker_ck               | 20 MHz       | 20 MHz
		Time_quantum (tq)          | 50 ns        | 50 ns
		Synchronization_segment    | 1 tq         | 1 tq
		Propagation_segment        | 23 tq        | 1 tq
		Phase_segment_1            | 8 tq         | 4 tq
		Phase_segment_2            | 8 tq         | 4 tq
		Synchronization_Jump_width | 8 tq         | 4 tq
		Bit_length                 | 40 tq = 2 µs | 10 tq = 0.5 µs
		Bit_rate                   | 0.5 MBit/s   | 2 MBit/s
	*/
	hfdcan.Init.NominalPrescaler = 0x1; /* tq = NominalPrescaler x (1/fdcan_ker_ck) */
	hfdcan.Init.NominalSyncJumpWidth = 0x8;
	hfdcan.Init.NominalTimeSeg1 = 0x1F; /* NominalTimeSeg1 = Propagation_segment + Phase_segment_1 */
	hfdcan.Init.NominalTimeSeg2 = 0x8;
	hfdcan.Init.DataPrescaler = 0x1;
	hfdcan.Init.DataSyncJumpWidth = 0x4;
	hfdcan.Init.DataTimeSeg1 = 0x5; /* DataTimeSeg1 = Propagation_segment + Phase_segment_1 */
	hfdcan.Init.DataTimeSeg2 = 0x4;
	hfdcan.Init.MessageRAMOffset = 0;
	hfdcan.Init.StdFiltersNbr = 1;
	hfdcan.Init.ExtFiltersNbr = 1;
	hfdcan.Init.RxFifo0ElmtsNbr = 1;
	hfdcan.Init.RxFifo0ElmtSize = FDCAN_DATA_BYTES_12;
	hfdcan.Init.RxFifo1ElmtsNbr = 2;
	hfdcan.Init.RxFifo1ElmtSize = FDCAN_DATA_BYTES_12;
	hfdcan.Init.RxBuffersNbr = 1;
	hfdcan.Init.RxBufferSize = FDCAN_DATA_BYTES_12;
	hfdcan.Init.TxEventsNbr = 2;
	hfdcan.Init.TxBuffersNbr = 1;
	hfdcan.Init.TxFifoQueueElmtsNbr = 2;
	hfdcan.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
	hfdcan.Init.TxElmtSize = FDCAN_DATA_BYTES_12;
	HAL_FDCAN_Init(&hfdcan);

	/* Configure standard ID reception filter to Rx buffer 0 */
	sFilterConfig.IdType = FDCAN_STANDARD_ID;
	sFilterConfig.FilterIndex = 0;
	sFilterConfig.FilterType = FDCAN_FILTER_DUAL;
	sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXBUFFER;
	sFilterConfig.FilterID1 = 0x111;
	sFilterConfig.FilterID2 = 0x555;
	sFilterConfig.RxBufferIndex = 0;
	HAL_FDCAN_ConfigFilter(&hfdcan, &sFilterConfig);

	/* Configure extended ID reception filter to Rx FIFO 1 */
	sFilterConfig.IdType = FDCAN_EXTENDED_ID;
	sFilterConfig.FilterIndex = 0;
	sFilterConfig.FilterType = FDCAN_FILTER_RANGE_NO_EIDM;
	sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO1;
	sFilterConfig.FilterID1 = 0x1111111;
	sFilterConfig.FilterID2 = 0x2222222;
	HAL_FDCAN_ConfigFilter(&hfdcan, &sFilterConfig);

	/* Configure Tx buffer message */
	TxHeader.Identifier = 0x111;
	TxHeader.IdType = FDCAN_STANDARD_ID;
	TxHeader.TxFrameType = FDCAN_DATA_FRAME;
	TxHeader.DataLength = FDCAN_DLC_BYTES_12;
	TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
	TxHeader.BitRateSwitch = FDCAN_BRS_ON;
	TxHeader.FDFormat = FDCAN_FD_CAN;
	TxHeader.TxEventFifoControl = FDCAN_STORE_TX_EVENTS;
	TxHeader.MessageMarker = 0x52;
	HAL_FDCAN_AddMessageToTxBuffer(&hfdcan, &TxHeader, TxData0, FDCAN_TX_BUFFER0);

	/* Start the FDCAN module */
	HAL_FDCAN_Start(&hfdcan);

	/* Send Tx buffer message */
	HAL_FDCAN_EnableTxBufferRequest(&hfdcan, FDCAN_TX_BUFFER0);

	/* Add message to Tx FIFO */
	TxHeader.Identifier = 0x1111112;
	TxHeader.IdType = FDCAN_EXTENDED_ID;
	TxHeader.TxFrameType = FDCAN_DATA_FRAME;
	TxHeader.DataLength = FDCAN_DLC_BYTES_12;
	TxHeader.ErrorStateIndicator = FDCAN_ESI_PASSIVE;
	TxHeader.BitRateSwitch = FDCAN_BRS_ON;
	TxHeader.FDFormat = FDCAN_FD_CAN;
	TxHeader.TxEventFifoControl = FDCAN_STORE_TX_EVENTS;
	TxHeader.MessageMarker = 0xCC;
	HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan, &TxHeader, TxData1);

	/* Add second message to Tx FIFO */
	TxHeader.Identifier = 0x2222222;
	TxHeader.IdType = FDCAN_EXTENDED_ID;
	TxHeader.TxFrameType = FDCAN_DATA_FRAME;
	TxHeader.DataLength = FDCAN_DLC_BYTES_12;
	TxHeader.ErrorStateIndicator = FDCAN_ESI_PASSIVE;
	TxHeader.BitRateSwitch = FDCAN_BRS_OFF;
	TxHeader.FDFormat = FDCAN_FD_CAN;
	TxHeader.TxEventFifoControl = FDCAN_STORE_TX_EVENTS;
	TxHeader.MessageMarker = 0xDD;
	HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan, &TxHeader, TxData2);

	/* Wait for event on Tamper push button to start data comparison */
	while ((UserButtonClickEvent) == RESET){;}

	/* Retrieve message from Rx buffer 0 */
	HAL_FDCAN_GetRxMessage(&hfdcan, FDCAN_RX_BUFFER0, &RxHeader, RxData);

	/* Compare payload to expected data */
	if (BufferCmp8b(TxData0, RxData, 12) != 0)
	{
		Error_Handler();
	}

	/* Retrieve message from Rx FIFO 1 */
	HAL_FDCAN_GetRxMessage(&hfdcan, FDCAN_RX_FIFO1, &RxHeader, RxData);

	/* Compare payload to expected data */
	if (BufferCmp8b(TxData1, RxData, 12) != 0)
	{
		Error_Handler();
	}

	/* Retrieve next message from Rx FIFO 1 */
	HAL_FDCAN_GetRxMessage(&hfdcan, FDCAN_RX_FIFO1, &RxHeader, RxData);

	/* Compare payload to expected data */
	if (BufferCmp8b(TxData2, RxData, 12) != 0)
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

/* USER CODE BEGIN 4 */

/**
  * @brief Compares two buffers.
  * @par Input
  *  - pBuffer1, pBuffer2: buffers to be compared.
  *  - BufferLength: buffer's length
  * @par Output
  * None.
  * @retval
  *   0: pBuffer1 identical to pBuffer2
  *   1: pBuffer1 differs from pBuffer2
  */
static uint32_t BufferCmp8b(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength)
{
  while(BufferLength--)
  {
    if(*pBuffer1 != *pBuffer2)
    {
      return 1;
    }

    pBuffer1++;
    pBuffer2++;
  }
  return 0;
}

/* USER CODE END 4 */


/**
  * @brief 	FDCAN Initialization Function
  * @param 	None
  * @retval None
  */
static void MX_FDCAN_DeInit(void)
{
	/* Deinitialize the FDCAN peripheral */
	HAL_FDCAN_DeInit(&hfdcan);

	ResMgr_Release(RESMGR_RESOURCE_RIFSC, STM32MP21_RIFSC_FDCAN_ID);

	/* Release GPIOC3 using Resource manager */
	ResMgr_Release(RESMGR_RESOURCE_RIF_GPIOC, RESMGR_GPIO_PIN(3));

	/* Release GPIOA5 using Resource manager */
	ResMgr_Release(RESMGR_RESOURCE_RIF_GPIOG, RESMGR_GPIO_PIN(5));
}
/**
  * @brief  Initialize the IPCC Peripheral
  * @retval None
  */
static void MX_IPCC_Init(void)
{

  hipcc.Instance = IPCC1;
  if (HAL_IPCC_Init(&hipcc) != HAL_OK)
  {
     Error_Handler();
  }
  /* IPCC interrupt Init */
  HAL_NVIC_SetPriority(IPCC1_RX_IRQn, DEFAULT_IRQ_PRIO, 0);
  HAL_NVIC_EnableIRQ(IPCC1_RX_IRQn);
}

/**
  * @brief  Callback from IPCC Interrupt Handler: Remote Processor asks local processor to shutdown
  * @param  hipcc IPCC handle
  * @param  ChannelIndex Channel number
  * @param  ChannelDir Channel direction
  * @retval None
  */
void CoproSync_ShutdownCb(IPCC_HandleTypeDef * hipcc, uint32_t ChannelIndex, IPCC_CHANNELDirTypeDef ChannelDir)
{
  MX_FDCAN_DeInit();

  /* Deinitialize the LED3 */
  BSP_LED_DeInit(LED3);

  /* When ready, notify the remote processor that we can be shut down */
  HAL_IPCC_NotifyCPU(hipcc, ChannelIndex, IPCC_CHANNEL_DIR_RX);

  /* Wait for complete shutdown */
  while(1);
}

void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == BUTTON_USER2_PIN)
	{
		UserButtonClickEvent = SET;
	}
}
/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* Error if LED1 is ON */
  BSP_LED_On(LED1);
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
  Error_Handler();
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
