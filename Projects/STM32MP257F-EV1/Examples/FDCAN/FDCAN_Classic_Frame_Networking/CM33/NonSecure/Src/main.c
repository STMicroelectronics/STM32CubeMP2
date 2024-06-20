/**
  ******************************************************************************
  * @file    main.c
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

/** @addtogroup FDCAN_Classic_Frame_Networking
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define KEY_PRESSED     0x00
#define KEY_NOT_PRESSED 0x01

/* Private variables ---------------------------------------------------------*/
uint8_t ubKeyNumber = 0x0;
FDCAN_HandleTypeDef hfdcan;
FDCAN_RxHeaderTypeDef RxHeader;
uint8_t RxData[8];
FDCAN_TxHeaderTypeDef TxHeader;
uint8_t TxData[8];
IPCC_HandleTypeDef   hipcc;

#ifdef DEBUG
volatile uint8_t debug = 1;
#endif /* DEBUG */

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_IPCC_Init(void);
static void MX_FDCAN_Init(void);
static void MX_FDCAN_DeInit(void);
void LED_Display(uint8_t LedStatus);

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

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  if (IS_DEVELOPER_BOOT_MODE())
  {
	  /* Configure the system clock */
	  SystemClock_Config();
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

  /* Configure Tamper push-button in interrupt mode */
  BSP_PB_Init(BUTTON_USER2, BUTTON_MODE_GPIO);

  /* Initialize all configured peripherals */
  MX_GPIO_Init();

  /* Configure the FDCAN peripheral */
  MX_FDCAN_Init();

  /* Infinite loop */
  while (1)
  {
	while (BSP_PB_GetState(BUTTON_USER2) == KEY_PRESSED)
	{
	  if (ubKeyNumber == 0x4)
	  {
		ubKeyNumber = 0x00;
	  }
	  else
	  {
		LED_Display(++ubKeyNumber);

		/* Set the data to be transmitted */
		TxData[0] = ubKeyNumber;
		TxData[1] = 0xAD;

		/* Start the Transmission process */
		if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan, &TxHeader, TxData) != HAL_OK)
		{
		  /* Transmission request Error */
		  Error_Handler();
		}
		HAL_Delay(10);

		while (BSP_PB_GetState(BUTTON_USER2) != KEY_NOT_PRESSED)
		{
		}
	  }
	}
  }

}

/**
  * @brief  System Clock Configuration
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_PeriphCLKInitTypeDef RCC_PeriphCLKInitStruct= {0};

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
	/* Setup FDCAN CLK to 100 MHz (Freq Max = 133 MHz) */
    RCC_PeriphCLKInitStruct.XBAR_Channel  = RCC_PERIPHCLK_FDCAN;
    RCC_PeriphCLKInitStruct.XBAR_ClkSrc   = RCC_XBAR_CLKSRC_PLL6;
    RCC_PeriphCLKInitStruct.Div           = 5; /* FreqMAX 100MHz*/
    if (HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphCLKInitStruct) != HAL_OK)
    {
	  Error_Handler();
    }

}

/**
  * @brief  Configures the FDCAN.
  * @param  None
  * @retval None
  */
static void MX_FDCAN_Init(void)
{
  /* Acquire FDCAN using Resource manager */
  if (RESMGR_STATUS_ACCESS_OK == ResMgr_Request(RESMGR_RESOURCE_RIFSC, STM32MP25_RIFSC_FDCAN_ID))
  {
    /* Enable FDCANx clock */
    FDCANx_CLK_ENABLE();
  }

  /* Acquire GPIOG11 using Resource manager */
  if (RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIF_GPIOG, RESMGR_GPIO_PIN(11)))
  {
    Error_Handler();
  }

  /* Acquire GPIOG12 using Resource manager */
  if (RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIF_GPIOG, RESMGR_GPIO_PIN(12)))
  {
    Error_Handler();
  }

  /* Enable GPIOs power supplies */
  if (RESMGR_STATUS_ACCESS_OK == ResMgr_Request(RESMGR_RESOURCE_RIF_RCC, RESMGR_RCC_RESOURCE(96)))
  {
    FDCANx_TX_GPIO_CLK_ENABLE();
  }

  FDCAN_FilterTypeDef sFilterConfig;

  /* Bit time configuration:
    fdcan_ker_ck               = 100 MHz
    Time_quantum (tq)          = 50 ns
    Synchronization_segment    = 1 tq
    Propagation_segment        = 23 tq
    Phase_segment_1            = 8 tq
    Phase_segment_2            = 8 tq
    Synchronization_Jump_width = 8 tq
    Bit_length                 = 40 tq = 2 µs
    Bit_rate                   = 0.5 MBit/s
  */
  hfdcan.Instance = FDCANx;
  hfdcan.Init.FrameFormat = FDCAN_FRAME_CLASSIC;
  hfdcan.Init.Mode = FDCAN_MODE_NORMAL;
  hfdcan.Init.AutoRetransmission = ENABLE;
  hfdcan.Init.TransmitPause = DISABLE;
  hfdcan.Init.ProtocolException = ENABLE;
  hfdcan.Init.NominalPrescaler = 0x5; /* tq = NominalPrescaler x (1/fdcan_ker_ck) */
  hfdcan.Init.NominalSyncJumpWidth = 0x8;
  hfdcan.Init.NominalTimeSeg1 = 0x1F; /* NominalTimeSeg1 = Propagation_segment + Phase_segment_1 */
  hfdcan.Init.NominalTimeSeg2 = 0x8;
  hfdcan.Init.MessageRAMOffset = 0;
  hfdcan.Init.StdFiltersNbr = 1;
  hfdcan.Init.ExtFiltersNbr = 0;
  hfdcan.Init.RxFifo0ElmtsNbr = 1;
  hfdcan.Init.RxFifo0ElmtSize = FDCAN_DATA_BYTES_8;
  hfdcan.Init.RxFifo1ElmtsNbr = 0;
  hfdcan.Init.RxBuffersNbr = 0;
  hfdcan.Init.TxEventsNbr = 0;
  hfdcan.Init.TxBuffersNbr = 0;
  hfdcan.Init.TxFifoQueueElmtsNbr = 1;
  hfdcan.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
  hfdcan.Init.TxElmtSize = FDCAN_DATA_BYTES_8;
  if (HAL_FDCAN_Init(&hfdcan) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /* Configure Rx filter */
  sFilterConfig.IdType = FDCAN_STANDARD_ID;
  sFilterConfig.FilterIndex = 0;
  sFilterConfig.FilterType = FDCAN_FILTER_MASK;
  sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
  sFilterConfig.FilterID1 = 0x321;
  sFilterConfig.FilterID2 = 0x7FF;
  if (HAL_FDCAN_ConfigFilter(&hfdcan, &sFilterConfig) != HAL_OK)
  {
    /* Filter configuration Error */
    Error_Handler();
  }

  /* Configure global filter to reject all non-matching frames */
  HAL_FDCAN_ConfigGlobalFilter(&hfdcan, FDCAN_REJECT, FDCAN_REJECT, FDCAN_REJECT_REMOTE, FDCAN_REJECT_REMOTE);

  /* Start the FDCAN module */
  if (HAL_FDCAN_Start(&hfdcan) != HAL_OK)
  {
    /* Start Error */
    Error_Handler();
  }

  if (HAL_FDCAN_ActivateNotification(&hfdcan, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0) != HAL_OK)
  {
    /* Notification Error */
    Error_Handler();
  }

  /* Prepare Tx Header */
  TxHeader.Identifier = 0x321;
  TxHeader.IdType = FDCAN_STANDARD_ID;
  TxHeader.TxFrameType = FDCAN_DATA_FRAME;
  TxHeader.DataLength = FDCAN_DLC_BYTES_2;
  TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
  TxHeader.BitRateSwitch = FDCAN_BRS_OFF;
  TxHeader.FDFormat = FDCAN_CLASSIC_CAN;
  TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
  TxHeader.MessageMarker = 0;
}

/**
  * @brief  Rx FIFO 0 callback.
  * @param  hfdcan: pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @param  RxFifo0ITs: indicates which Rx FIFO 0 interrupts are signalled.
  *                     This parameter can be any combination of @arg FDCAN_Rx_Fifo0_Interrupts.
  * @retval None
  */
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
  if((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != RESET)
  {
    /* Retrieve Rx messages from RX FIFO0 */
    if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &RxHeader, RxData) != HAL_OK)
    {
  	  /* Reception Error */
	  Error_Handler();
    }

    /* Display LEDx */
    if ((RxHeader.Identifier == 0x321) && (RxHeader.IdType == FDCAN_STANDARD_ID) && (RxHeader.DataLength == FDCAN_DLC_BYTES_2))
    {
      LED_Display(RxData[0]);
      ubKeyNumber = RxData[0];
    }

    if (HAL_FDCAN_ActivateNotification(hfdcan, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0) != HAL_OK)
    {
      /* Notification Error */
      Error_Handler();
    }
  }
}

/**
  * @brief  Turns ON/OFF the dedicated LED.
  * @param  LedStatus: LED number from 0 to 3
  * @retval None
  */
void LED_Display(uint8_t LedStatus)
{
  /* Turn OFF all LEDs */
  BSP_LED_Off(LED3);

  switch(LedStatus)
  {
    case (1):
      /* Turn ON LED3 */
      BSP_LED_On(LED3);
      break;

    case (2):
      /* Turn OFF LED3 */
      BSP_LED_On(LED3);
      break;

    default:
      break;
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

/* USER CODE END 4 */

/**
  * @brief 	FDCAN De-Initialization Function
  * @param 	None
  * @retval None
  */
void MX_FDCAN_DeInit(void)
{
  /* Deinitialize the hfdcan peripheral */
  HAL_FDCAN_DeInit(&hfdcan);

  ResMgr_Release(RESMGR_RESOURCE_RIFSC, STM32MP25_RIFSC_FDCAN_ID);

  /* Release GPIOG11 using Resource manager */
  ResMgr_Release(RESMGR_RESOURCE_RIF_GPIOG, RESMGR_GPIO_PIN(11));

  /* Release GPIOG12 using Resource manager */
  ResMgr_Release(RESMGR_RESOURCE_RIF_GPIOG, RESMGR_GPIO_PIN(12));
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
/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* Error if LED3 is Blinking */
  while (1)
  {
    BSP_LED_Toggle(LED3);
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
