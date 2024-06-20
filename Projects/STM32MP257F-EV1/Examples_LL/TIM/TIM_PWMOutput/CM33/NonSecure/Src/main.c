/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    Examples_LL/TIM/TIM_PWMOutput/CM33/NonSecure/Src/main.c
  * @author  MCD Application Team
  * @brief   This example describes how to use a timer peripheral to generate a
  *          PWM output signal and update PWM duty cycle
  *          using the STM32MP2xx TIM LL API.
  *          Peripheral initialization done using LL unitary services functions.
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
/** @addtogroup STM32MP2xx_LL_Examples
  * @{
  */

/** @addtogroup TIM_PWMOutput
  * @{
  */

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* Number of output compare modes */
#define TIM_DUTY_CYCLES_NB 11

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
IPCC_HandleTypeDef hipcc;

static __IO uint8_t KeyPressed = 1;
#ifdef DEBUG
volatile uint8_t debug = 1;
#endif /* DEBUG */
/* Duty cycles: D = T/P * 100%                                                */
/* where T is the pulse duration and P  the period of the PWM signal          */
static uint32_t aDutyCycle[TIM_DUTY_CYCLES_NB] = {
  0,    /*  0% */
  10,   /* 10% */
  20,   /* 20% */
  30,   /* 30% */
  40,   /* 40% */
  50,   /* 50% */
  60,   /* 60% */
  70,   /* 70% */
  80,   /* 80% */
  90,   /* 90% */
  100,  /* 100% */
};

/* Duty cycle index */
static uint8_t iDutyCycle = 0;

/* Measured duty cycle */
__IO uint32_t uwMeasuredDutyCycle = 0;

/* TIM8 Clock */
static uint32_t TimOutClock = 1;
/* USER CODE BEGIN PV */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

__STATIC_INLINE void Configure_DutyCycle(uint32_t D);
__STATIC_INLINE void  MX_TIMPWMOutput_Init(void);
static void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_IPCC_Init(void);
void   LED_Init(void);
void   LED_DeInit(void);
void   LED_On(void);
void   UserButton_Init(void);
void   UserButton_DeInit(void);
void   MX_TIMPWMOutput_DeInit(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

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
  /* STM32MP2xx HAL library initialization:
       - Systick timer is configured by default as source of time base, but user
             can eventually implement his proper time base source (a general purpose
             timer for example or other time source), keeping in mind that Time base
             duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
             handled in milliseconds basis.
       - Set NVIC Group Priority to 4
       - Low Level Initialization
     */
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/
#ifdef DEBUG
  while(debug);
#endif /* DEBUG */

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  if (IS_DEVELOPER_BOOT_MODE())
  {
	  SystemClock_Config();
  }

  /* Initialize all configured peripherals */
  MX_GPIO_Init();

  /* Initialize LED1 */
  LED_Init();

  if(!IS_DEVELOPER_BOOT_MODE())
  {
    /* IPCC initialization */
    MX_IPCC_Init();

    /*CoproSync Initialization*/
    CoproSync_Init();
  }

  /* Initialize button in EXTI mode */
  UserButton_Init();

  /* Configure the timer in output compare mode */
  MX_TIMPWMOutput_Init();

  /* USER CODE BEGIN 2 */

  /* Infinite loop */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  * @brief GPIO Initialization Function
  * @param None
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
  * @brief  Initialize LED3.
  * @param  None
  * @retval None
  */
void LED_Init(void)
{
  /* Enable the LED3 Clock */
  if (RESMGR_STATUS_ACCESS_OK == ResMgr_Request(RESMGR_RESOURCE_RIF_RCC, RESMGR_RCC_RESOURCE(99)))
  {
	/* Enable GPIO Clock (to be able to program the configuration registers) */
	  LED3_GPIO_CLK_ENABLE();
  }
  /* Acquire GPIOJ6 using Resource manager */
  if (RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIF_GPIOJ, RESMGR_GPIO_PIN(6)))
  {
    Error_Handler();
  }

  /* Configure IO in output push-pull mode to drive external LED1 */
  LL_GPIO_SetPinMode(LED3_GPIO_PORT, LED3_PIN, LL_GPIO_MODE_OUTPUT);
}


/**
  * @brief  Deinitialize LED3.
  * @param  None
  * @retval None
  */
void LED_DeInit(void)
{
  /* Release GPIOJ6 using Resource manager */
  (void) ResMgr_Release(RESMGR_RESOURCE_RIF_GPIOJ, RESMGR_GPIO_PIN(6));
}

/**
  * @brief  Turn-on LED3.
  * @param  None
  * @retval None
  */
void LED_On(void)
{
  /* Turn LED1 on */
  LL_GPIO_SetOutputPin(LED3_GPIO_PORT, LED3_PIN);
}

void LED_Off(void)
{
  /* Turn LED1 on */
  LL_GPIO_ResetOutputPin(LED3_GPIO_PORT, LED3_PIN);
}

/**
  * @brief  Configures User push-button in EXTI Line Mode.
  * @param  None
  * @retval None
  */
void UserButton_Init()
{
  LL_EXTI_InitTypeDef exti_initstruct;

  /* Enable GPIOG clock */
  if (RESMGR_STATUS_ACCESS_OK == ResMgr_Request(RESMGR_RESOURCE_RIF_RCC, RESMGR_RCC_RESOURCE(96)))
  {
	/* Enable GPIO Clock (to be able to program the configuration registers) */
	USER2_BUTTON_GPIO_CLK_ENABLE();
  }
  /* Acquire GPIOG8 using Resource manager */
  if (RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIF_GPIOG, RESMGR_GPIO_PIN(8)))
  {
    Error_Handler();
  }

  /* Configure PG.8 pin as input floating */
  /* -1- GPIO Config */
  /* Configure IO */
  LL_GPIO_SetPinMode(USER2_BUTTON_GPIO_PORT, USER2_BUTTON_PIN, LL_GPIO_MODE_INPUT);
  LL_GPIO_SetPinPull(USER2_BUTTON_GPIO_PORT, USER2_BUTTON_PIN, LL_GPIO_PULL_NO);

  /*-3- Enable a falling trigger EXTI line Interrupt */
  /* Set fields of initialization structure */
  exti_initstruct.Line_0_31   = USER2_BUTTON_EXTI_LINE;
  exti_initstruct.Line_32_63  = LL_EXTI_LINE_NONE;
  exti_initstruct.Line_64_95  = LL_EXTI_LINE_NONE;
  exti_initstruct.LineCommand = ENABLE;
  exti_initstruct.Mode        = LL_EXTI_MODE_IT;
  exti_initstruct.Trigger     = LL_EXTI_TRIGGER_FALLING;

  /* Initialize EXTI according to parameters defined in initialization structure. */
  LL_EXTI_Init(EXTI2, &exti_initstruct);

  /* -2- Connect External Line to the GPIO*/
  LL_EXTI_SetEXTISource(EXTI2, LL_EXTI_EXTICR_PORTG, LL_EXTI_EXTI_LINE8);

  /*-4- Configure NVIC for EXTI15_10_IRQn */
  NVIC_EnableIRQ(USER2_BUTTON_EXTI_IRQn);
  NVIC_SetPriority(USER2_BUTTON_EXTI_IRQn,0);
}

/**
  * @brief  Deinitialize User Button.
  * @param  None
  * @retval None
  */
void UserButton_DeInit(void)
{
  /* Release GPIOG8 using Resource manager */
  (void) ResMgr_Release(RESMGR_RESOURCE_RIF_GPIOG, RESMGR_GPIO_PIN(8));

  /* Disable NVIC for EXTI15_10_IRQn */
  NVIC_DisableIRQ(USER2_BUTTON_EXTI_IRQn);
}

/**
  * @brief  Configures the timer to generate a PWM signal on the OC1 output.
  * @note   Peripheral configuration is minimal configuration from reset values.
  *         Thus, some useless LL unitary functions calls below are provided as
  *         commented examples - setting is default configuration from reset.
  * @param  None
  * @retval None
  */
__STATIC_INLINE void  MX_TIMPWMOutput_Init(void)
{
  uint32_t hclk_clock_frequency = 0;

  /*************************/
  /* GPIO AF configuration */
  /*************************/

  /* Enable the peripheral clock of GPIOs */
  if (RESMGR_STATUS_ACCESS_OK == ResMgr_Request(RESMGR_RESOURCE_RIF_RCC, RESMGR_RCC_RESOURCE(99)))
  {
    LL_RCC_GPIOJ_EnableClock();
  }

  /* Acquire GPIOJ5 using Resource manager */
  if (RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIF_GPIOJ, RESMGR_GPIO_PIN(5)))
  {
    Error_Handler();
  }

  /* GPIO TIM8_CH1 configuration */
  LL_GPIO_SetPinMode(GPIOJ, LL_GPIO_PIN_5, LL_GPIO_MODE_ALTERNATE);
  LL_GPIO_SetPinPull(GPIOJ, LL_GPIO_PIN_5, LL_GPIO_PULL_DOWN);
  LL_GPIO_SetPinSpeed(GPIOJ, LL_GPIO_PIN_5, LL_GPIO_SPEED_FREQ_HIGH);
  LL_GPIO_SetAFPin_0_7(GPIOJ, LL_GPIO_PIN_5, LL_GPIO_AF_8);

  /***********************************************/
  /* Configure the NVIC to handle TIM8 interrupt */
  /***********************************************/
  NVIC_SetPriority(TIM8_CC_IRQn, 0);
  NVIC_EnableIRQ(TIM8_CC_IRQn);

  /******************************/
  /* Peripheral clocks enabling */
  /******************************/
  /* Enable the timer peripheral clock */

  /* Acquire TIM8 using Resource manager */
  if (RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIFSC, STM32MP25_RIFSC_TIM8_ID))
  {
    Error_Handler();
  }

  LL_RCC_TIM8_EnableClock();

  /***************************/
  /* Time base configuration */
  /***************************/
  /* Set counter mode */
  /* Reset value is LL_TIM_COUNTERMODE_UP */
  //LL_TIM_SetCounterMode(TIM8, LL_TIM_COUNTERMODE_UP);

  /* Set the pre-scaler value to have TIM8 counter clock equal to 10 kHz */
  hclk_clock_frequency = HAL_RCCEx_GetTimerCLKFreq(RCC_PERIPHCLK_TIM8);
  LL_TIM_SetPrescaler(TIM8, __LL_TIM_CALC_PSC(hclk_clock_frequency, 10000));
//  LL_TIM_SetPrescaler(TIM8, hclk_clock_frequency);


  /* Enable TIM8_ARR register preload. Writing to or reading from the         */
  /* auto-reload register accesses the preload register. The content of the   */
  /* preload register are transferred into the shadow register at each update */
  /* event (UEV).                                                             */
  LL_TIM_EnableARRPreload(TIM8);

  /* Set the auto-reload value to have a counter frequency of 100 Hz */
  /* TIM8CLK = HCLK frequency / (APB prescaler & multiplier)               */
  TimOutClock = hclk_clock_frequency/1;
  LL_TIM_SetAutoReload(TIM8, __LL_TIM_CALC_ARR(TimOutClock, LL_TIM_GetPrescaler(TIM8), 100));

  /*********************************/
  /* Output waveform configuration */
  /*********************************/
  /* Set output mode */
  /* Reset value is LL_TIM_OCMODE_FROZEN */
  LL_TIM_OC_SetMode(TIM8, LL_TIM_CHANNEL_CH1, LL_TIM_OCMODE_PWM1);

  /* Set output channel polarity */
  /* Reset value is LL_TIM_OCPOLARITY_HIGH */
  //LL_TIM_OC_SetPolarity(TIM8, LL_TIM_CHANNEL_CH3, LL_TIM_OCPOLARITY_HIGH);

  /* Set compare value to half of the counter period (50% duty cycle ) */
  LL_TIM_OC_SetCompareCH1(TIM8, ( (LL_TIM_GetAutoReload(TIM8) + 1 ) / 2));

  /* Enable TIM8_CCR3 register preload. Read/Write operations access the      */
  /* preload register. TIM8_CCR3 preload value is loaded in the active        */
  /* at each update event.                                                    */
  LL_TIM_OC_EnablePreload(TIM8, LL_TIM_CHANNEL_CH1);

  LL_TIM_EnableAllOutputs(TIM8);

  /**************************/
  /* TIM8 interrupts set-up */
  /**************************/
  /* Enable the capture/compare interrupt for channel 1*/
  LL_TIM_EnableIT_CC1(TIM8);

  /**********************************/
  /* Start output signal generation */
  /**********************************/
  /* Enable output channel 1 */
  LL_TIM_CC_EnableChannel(TIM8, LL_TIM_CHANNEL_CH1);

  /* Enable counter */
  LL_TIM_EnableCounter(TIM8);

  /* Force update generation */
  LL_TIM_GenerateEvent_UPDATE(TIM8);
}


/**
  * @brief  Deinitialize TIMPWMOutput.
  * @param  None
  * @retval None
  */
void MX_TIMPWMOutput_DeInit(void)
{
  /* Release GPIOJ5 using Resource manager */
  (void) ResMgr_Release(RESMGR_RESOURCE_RIF_GPIOJ, RESMGR_GPIO_PIN(5));

  /***********************************************/
  /* Disable the NVIC which handled TIM8 interrupt */
  /***********************************************/
  NVIC_DisableIRQ(TIM8_CC_IRQn);

  /* Release TIM8 using Resource manager */
  (void) ResMgr_Release(RESMGR_RESOURCE_RIFSC, STM32MP25_RIFSC_TIM8_ID);
}

/**
  * @brief  Changes the duty cycle of the PWM signal.
  *         D = (T/P)*100
  *           where T is the pulse duration and P is the PWM signal period
  * @param  D Duty cycle
  * @retval None
  */
__STATIC_INLINE void Configure_DutyCycle(uint32_t D)
{
  uint32_t P;    /* Pulse duration */
  uint32_t T;    /* PWM signal period */

  /* PWM signal period is determined by the value of the auto-reload register */
  T = LL_TIM_GetAutoReload(TIM8) + 1;

  /* Pulse duration is determined by the value of the compare register.       */
  /* Its value is calculated in order to match the requested duty cycle.      */
  P = (D*T)/100;
  LL_TIM_OC_SetCompareCH1(TIM8, P);
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
  /* Deinitialize the TIM peripheral */
  MX_TIMPWMOutput_DeInit();

  /* Deinitialize the User Button */
  UserButton_DeInit();

  /* Deinitialize the LED */
  LED_DeInit();

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
  /* Error if LED3 is Turned ON */
  LED_On();

  while(1);
  /* USER CODE END Error_Handler_Debug */
}

/******************************************************************************/
/*   USER IRQ HANDLER TREATMENT                                               */
/******************************************************************************/
/**
  * @brief  Function to manage IRQ Handler
  * @param  None
  * @retval None
  */
void UserButton_Callback(void)
{
  /* Set new duty cycle */
  iDutyCycle = (iDutyCycle + 1) % TIM_DUTY_CYCLES_NB;

  /* Change PWM signal duty cycle */
  Configure_DutyCycle(aDutyCycle[iDutyCycle]);
}

/**
  * @brief  Timer capture/compare interrupt processing
  * @param  None
  * @retval None
  */
void TimerCaptureCompare_Callback(void)
{
  uint32_t CNT, ARR;
  CNT = LL_TIM_GetCounter(TIM8);
  ARR = LL_TIM_GetAutoReload(TIM8);

  if (LL_TIM_OC_GetCompareCH1(TIM8) > ARR )
  {
    /* If capture/compare setting is greater than autoreload, there is a counter overflow and counter restarts from 0.
       Need to add full period to counter value (ARR+1)  */
    CNT = CNT + ARR + 1;
  }
  uwMeasuredDutyCycle = (CNT * 100) / ( ARR + 1 );
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
