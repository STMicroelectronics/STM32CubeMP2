/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ADC/ADC_RegularConversion_Polling/Src/main.c
  * @author  MCD Application Team
  * @brief   This example describes how to use Polling mode to convert data.
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
#include "copro_sync.h"
#include <stdio.h>

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
#define LED_TOGGLE_DELAY_MS  (500)                      /* Time delay in mili-seconds to toggle the LED */
#define PRINT_DELAY_MS       (LED_TOGGLE_DELAY_MS * 2)  /* Time delay in mili-seconds to print
                                                         * the results or required information,
                                                         * it should be in multiple of LED_TOGGLE_DELAY_MS */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

#ifdef DEBUG
volatile int debug = 1;
#endif

/* ADC handler declaration */
ADC_HandleTypeDef         AdcHandle;

/* ADC channel configuration structure declaration */
ADC_ChannelConfTypeDef    sConfig;

/* IPCC handler declaration */
IPCC_HandleTypeDef        hipcc;

/* TIM5 handler declaration */
TIM_HandleTypeDef         htim5;

/* Converted value declaration */
__IO uint32_t                    uwConvertedValue;
/* Input voltage declaration */
__IO int32_t                     uwInputVoltage;

static __IO int32_t              g_tim5_int_flag = 0;
static __IO int32_t              g_shutdown_flag = 0;
static IPCC_HandleTypeDef const *hipcc_handle       ;
static uint32_t                  ipcc_ch_id         ;
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);

static void MX_IPCC_Init(void);
static void MX_ADC2_Init(void);
static void MX_ADC2_DeInit(void);
static void MX_TIM5_Init(void);
static void MX_TIM5_DeInit(void);
static void Shutdown_Routine(void);
void        HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);


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
  uint32_t status      = 0;
  g_tim5_int_flag      = 0;
  g_shutdown_flag      = 0;

#ifdef DEBUG
  while(debug==1);
#endif

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

#if defined(__VALID_OUTPUT_TERMINAL_IO__) && defined (__GNUC__)
  initialise_monitor_handles();
#elif defined(__VALID_OUTPUT_UART_IO__)
  COM_InitTypeDef COM_Conf;

  COM_Conf.BaudRate   = 115200;
  COM_Conf.HwFlowCtl  = COM_HWCONTROL_NONE;
  COM_Conf.Parity     = COM_PARITY_NONE;
  COM_Conf.StopBits   = COM_STOPBITS_1;
  COM_Conf.WordLength = COM_WORDLENGTH_8B;

  BSP_COM_Init(COM_VCP_CM33, &COM_Conf);
#endif  /* __VALID_OUTPUT_TERMINAL_IO__ or __VALID_OUTPUT_UART_IO__ */

  /* Configure the system clock for dev mode*/
  if(IS_DEVELOPER_BOOT_MODE())
  {
    SystemClock_Config();

    HAL_PWR_EnableBkUpAccess();
    status = BSP_PMIC_Init();
    if (status)
    {
      printf("error :  BSP PMIC init fail\r\n");
      Error_Handler();
    }

    status = BSP_PMIC_Power_Mode_Init();
    if (status)
    {
      printf("error :  BSP PMIC Mode init fail\r\n");
      Error_Handler();
    }
    else
    {
      /*LED_1 is by default on, switching it off*/
      BSP_LED_Init(LED1);
      BSP_LED_Off(LED1);
    }
  }

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Configure LED3*/
  BSP_LED_Init(LED3);

  if(!IS_DEVELOPER_BOOT_MODE())
  {
    /* IPCC initialization */
    MX_IPCC_Init();
    /*Corpo Sync Initialization*/
    CoproSync_Init();
    /* Initialize MAILBOX with IPCC peripheral */
    MAILBOX_SCMI_Init();
  }

  /* Initialize all configured peripherals */
  MX_TIM5_Init();
  MX_ADC2_Init();

  /* Infinite Loop */
  while (1)
  {
    if( 1 == g_tim5_int_flag )
    {
      if (HAL_ADC_PollForConversion(&AdcHandle, 10) != HAL_OK)
      {
        Error_Handler();
      }

      /* Read the converted value */
      uwConvertedValue = HAL_ADC_GetValue(&AdcHandle);
      /* Convert the result from 12 bit value to the voltage dimension (mV unit) */
      /* Vref = 1.8 V */
      uwInputVoltage = ((uwConvertedValue * 3600) / 0xFFF) - 1800;
      printf("uwConvertedValue = %lu,    uwInputVoltage = %ld    (ADC_DifferentialMode)\r\n", uwConvertedValue, uwInputVoltage);
      BSP_LED_Toggle(LED3);

      g_tim5_int_flag = 0;
    }

    if( 1 == g_shutdown_flag )
    {
      Shutdown_Routine();
      g_shutdown_flag = 0;
    }
  }
  /* USER CODE END 3 */
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
    * @brief  Initialize ADC peripheral
    * @param  None
    * @retval None
    */
static void MX_ADC2_Init(void)
{
	/* Enable ADC regulator */
	if(ResMgr_Request(RESMGR_RESOURCE_RIF_PWR, RESMGR_PWR_RESOURCE(0)) == RESMGR_STATUS_ACCESS_OK)
	{
	  loc_printf("Enable ADC regulator directly\r\n");
	  HAL_PWREx_EnableSupply(PWR_PVM_A);
	}
	else
	{
	  int ret = 0;
	  loc_printf("Enable ADC regulator with SCMI\r\n");
	  ret = scmi_voltage_domain_enable(&scmi_channel, VOLTD_SCMI_ADC);
	  if (ret)
	    loc_printf("Failed to enable VOLTD_SCMI_ADC (error code %d)\r\n", ret);
	}

	/*##-1- Enable peripherals and GPIO Clocks #################################*/
	/* Acquire ADC2 using Resource manager */
	if (RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIFSC, STM32MP21_RIFSC_ADC2_ID))
	{
	Error_Handler();
	}

	/* Acquire GPIOF11 using Resource manager */
	if (RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIF_GPIOF, RESMGR_GPIO_PIN(11)))
	{
	Error_Handler();
	}

	/* Enable GPIOs power supplies */
	if (RESMGR_STATUS_ACCESS_OK == ResMgr_Request(RESMGR_RESOURCE_RIF_RCC, RESMGR_RCC_RESOURCE(101)))
	{
	__HAL_RCC_GPIOF_CLK_ENABLE();
	}

  /* USER CODE END 2 */
	/*##-1- Configure the ADC peripheral #######################################*/
	AdcHandle.Instance                      = ADC2;
	AdcHandle.Init.ClockPrescaler           = ADC_CLOCK_CK_KER_ADC2_DIV2;    /* Asynchronous clock mode, input ADC clock divided by 2*/
	AdcHandle.Init.Resolution               = ADC_RESOLUTION_12B;            /* 12-bit resolution for converted data */
	AdcHandle.Init.ScanConvMode             = DISABLE;                       /* Sequencer disabled (ADC conversion on only 1 channel: channel set on rank 1) */
	AdcHandle.Init.EOCSelection             = ADC_EOC_SINGLE_CONV;           /* EOC flag picked-up to indicate conversion end */
	AdcHandle.Init.LowPowerAutoWait         = DISABLE;                       /* Auto-delayed conversion feature disabled */
	AdcHandle.Init.ContinuousConvMode       = ENABLE;                        /* Continuous mode enabled (automatic conversion restart after each conversion) */
	AdcHandle.Init.NbrOfConversion          = 1;                             /* Parameter discarded because sequencer is disabled */
	AdcHandle.Init.DiscontinuousConvMode    = DISABLE;                       /* Parameter discarded because sequencer is disabled */
	AdcHandle.Init.NbrOfDiscConversion      = 1;                             /* Parameter discarded because sequencer is disabled */
	AdcHandle.Init.ExternalTrigConv         = ADC_SOFTWARE_START;            /* Software start to trig the 1st conversion manually, without external event */
	AdcHandle.Init.ExternalTrigConvEdge     = ADC_EXTERNALTRIGCONVEDGE_NONE; /* Parameter discarded because software trigger chosen */
	AdcHandle.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DR;         /* DR register used as output (DMA mode disabled) */
	AdcHandle.Init.LeftBitShift             = ADC_LEFTBITSHIFT_NONE;         /* Left shift of final results */
	AdcHandle.Init.Overrun                  = ADC_OVR_DATA_OVERWRITTEN;      /* DR register is overwritten with the last conversion result in case of overrun */
	AdcHandle.Init.OversamplingMode         = DISABLE;                       /* Oversampling disable */

	/* Initialize ADC peripheral according to the passed parameters */
	if (HAL_ADC_Init(&AdcHandle) != HAL_OK)
	{
	  Error_Handler();
	}

	/* ### - 2 - Channel configuration ######################################## */
	sConfig.Channel                = ADC_CHANNEL_6;              /* Sampled channel number */
	sConfig.Rank                   = ADC_REGULAR_RANK_1;         /* Rank of sampled channel number ADCx_CHANNEL */
	sConfig.SamplingTime           = ADC_SAMPLETIME_7CYCLES_5;   /* Sampling time (number of clock cycles unit) */
	sConfig.SingleDiff             = ADC_DIFFERENTIAL_ENDED;     /* Differential input channel */
	sConfig.OffsetNumber           = ADC_OFFSET_NONE;            /* No offset subtraction */
	sConfig.Offset                 = 0;                          /* Parameter discarded because offset correction is disabled */
	sConfig.OffsetRightShift       = DISABLE;                    /* No Right Offset Shift */
	sConfig.OffsetSignedSaturation = DISABLE;                    /* No Signed Saturation */
	if (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK)
	{
	  Error_Handler();
	}

	/* ### - 3 - Start calibration ############################################ */
	/* Run the ADC calibration in differential-ended mode */
	if (HAL_ADCEx_Calibration_Start(&AdcHandle, ADC_DIFFERENTIAL_ENDED) != HAL_OK)
	{
	  Error_Handler();
	}

	/* ### - 4 - Start conversion ############################################# */
	if (HAL_ADC_Start(&AdcHandle) != HAL_OK)
	{
	  Error_Handler();
	}
  }

/**
  * @brief  De-Initialize ADC peripheral
  * @param  None
  * @retval None
  */
static void MX_ADC2_DeInit(void)
{
	/* Deinitialize the ADC peripheral */
	HAL_ADC_DeInit(&AdcHandle);

	/* Release ADC2 using Resource manager */
	ResMgr_Release(RESMGR_RESOURCE_RIFSC, STM32MP21_RIFSC_ADC2_ID);
	ResMgr_Release(RESMGR_RESOURCE_RIF_PWR, RESMGR_PWR_RESOURCE(0));

	/* Disable ADC regulator */
	if(ResMgr_Request(RESMGR_RESOURCE_RIF_PWR, RESMGR_PWR_RESOURCE(0)) == RESMGR_STATUS_ACCESS_OK)
	{
	  loc_printf("Disable ADC regulator directly\r\n");
	  HAL_PWREx_DisableSupply(PWR_PVM_A);
	}
	else
	{
	  int ret = 0;
	  loc_printf("Disable ADC regulator with SCMI\r\n");
	  ret = scmi_voltage_domain_disable(&scmi_channel, VOLTD_SCMI_ADC);
	  if (ret)
	    loc_printf("Failed to disable VOLTD_SCMI_ADC (error code %d)\r\n", ret);
	}

	/* Release GPIOF11 using Resource manager */
	ResMgr_Release(RESMGR_RESOURCE_RIF_GPIOF, RESMGR_GPIO_PIN(11));
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

/* TIM5 init function */
static void MX_TIM5_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

  /* Acquire TIM5 using Resource manager */
  if (RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIFSC, STM32MP21_RIFSC_TIM5_ID))
  {
	  Error_Handler();
  }

  htim5.Instance = TIM5;
  htim5.Init.Prescaler = PRESCALER_VALUE;
  htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim5.Init.Period = PERIOD_VALUE;
  htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  if (HAL_TIM_Base_Init(&htim5) != HAL_OK)
  {
	Error_Handler();
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim5, &sClockSourceConfig) != HAL_OK)
  {
	Error_Handler();
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim5, &sMasterConfig) != HAL_OK)
  {
	Error_Handler();
  }

  /*## Start the TIM Base generation in interrupt mode ####################*/
  /* Start Channel1 */
  if (HAL_TIM_Base_Start_IT(&htim5) != HAL_OK)
  {
    /* Starting Error */
    Error_Handler();
  }
}


/**
  * @brief  TIM DeInitialization Function
  * @param  None
  * @retval None
  */
static void MX_TIM5_DeInit(void)
{
	/* Deinitialize the TIM peripheral and Resources*/
	  HAL_TIM_Base_DeInit(&htim5);

	/* Release TIM5 using Resource manager */
    ResMgr_Release(RESMGR_RESOURCE_RIFSC, STM32MP21_RIFSC_TIM5_ID);
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (TIM5 == htim->Instance)
  {
	 g_tim5_int_flag = 1;
  }
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
	  hipcc_handle = hipcc;
	  ipcc_ch_id   = ChannelIndex;

	  g_shutdown_flag = 1;
}

/**
  * @brief  Executes in main after Callback from IPCC Interrupt Handler
  * @param  none
  * @retval None
  */
static void Shutdown_Routine(void)
{
  /* Deinitialize the ADC peripheral */
  MX_ADC2_DeInit();

  /* DeInitialize MAILBOX with IPCC peripheral */
  MAILBOX_SCMI_DeInit();

  /* Deinitialize the TIM peripheral and Resources*/
  MX_TIM5_DeInit();

  /* Deinitialize the LED3 */
  (void)BSP_LED_DeInit(LED3);

  /* When ready, notify the remote processor that we can be shut down */
  (void)HAL_IPCC_NotifyCPU(hipcc_handle, ipcc_ch_id, IPCC_CHANNEL_DIR_RX);

  /* Wait for complete shutdown */
  while(1);
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* Blink the LED3 */
  while (1)
  {
    /* Error if LED3 is ON */
    BSP_LED_On(LED3);
    while (1)
    {
      HAL_Delay(1000);
    }
  }
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


