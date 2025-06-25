/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    TIM/TIM_PWMOutput/Src/main.c
  * @author  MCD Application Team
  * @brief   This sample code shows how to use STM32MP2xx TIM HAL API to generate
  *          3 signals in PWM.
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
#include <stdio.h>
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
TIM_HandleTypeDef htim8;
IPCC_HandleTypeDef   hipcc;
uint32_t Prescaler;

#ifdef DEBUG
volatile uint8_t debug = 1;
#endif /* DEBUG */

/* USER CODE BEGIN PV */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM8_Init(void);
static void MX_TIM8_DeInit(void);
static void MX_IPCC_Init(void);
void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);
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

 uint32_t status;

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
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

  /* Configure LED3 */
  BSP_LED_Init(LED3);

  if(!IS_DEVELOPER_BOOT_MODE())
  {
    /* IPCC initialization */
    MX_IPCC_Init();

 /*Corpo Sync Initialization*/
    CoproSync_Init();
  }
  /* USER CODE BEGIN SysInit */
  /* Initialize Prescaler */
  /* -----------------------------------------------------------------------
  TIM8 Configuration: generate 4 PWM signals with 4 different duty cycles.

      In this example TIM8 input clock (TIM8CLK) is set to 2 * APB2 clock (PCLK2),
      since APB2 prescaler is equal to 0.
        TIM8CLK = 2 * PCLK2
        PCLK2 = HCLK /2
        => TIM8CLK = HCLK = SystemCoreClock

      To get TIM8 counter clock at 16 MHz, the prescaler is computed as follows:
         Prescaler = (TIM8CLK / TIM8 counter clock) - 1
         Prescaler = ((SystemCoreClock) /16 MHz) - 1

      To get TIM8 output clock at 24 KHz, the period (ARR)) is computed as follows:
         ARR = (TIM8 counter clock / TIM8 output clock) - 1
             = 665

      TIM8 Channel2 duty cycle = (TIM8_CCR2/ TIM8_ARR + 1)* 100 = 50%
      TIM8 Channel3 duty cycle = (TIM8_CCR3/ TIM8_ARR + 1)* 100 = 25%
      TIM8 Channel4 duty cycle = (TIM8_CCR4/ TIM8_ARR + 1)* 100 = 12.5%

      Note:
       SystemCoreClock variable holds HCLK frequency and is defined in system_stm32mp1xx.c file.
       Each time the core clock (HCLK) changes, user had to update SystemCoreClock
       variable value. Otherwise, any configuration based on this variable will be incorrect.
       This variable is updated in three ways:
        1) by calling CMSIS function SystemCoreClockUpdate()
        2) by calling HAL API function HAL_RCC_GetSysClockFreq()
        3) each time HAL_RCC_ClockConfig() is called to configure the system clock frequency
    ----------------------------------------------------------------------- */
  Prescaler = (HAL_RCCEx_GetTimerCLKFreq(RCC_PERIPHCLK_TIM8)/ (16 * 1000000)) - 1;
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM8_Init();

  /* USER CODE BEGIN 2 */

  /*## Start PWM signals generation #######################################*/
    /* Start channel 1 */
    if (HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_2) != HAL_OK)
    {
      /* PWM Generation Error */
      Error_Handler();
    }
    /* Start channel 3 */
    if (HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_3) != HAL_OK)
    {
      /* PWM generation Error */
      Error_Handler();
    }
    /* Start channel 4 */
    if (HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_4) != HAL_OK)
    {
      /* PWM generation Error */
      Error_Handler();
    }

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

/* TIM8 init function */
static void MX_TIM8_Init(void)
{
  TIM_MasterConfigTypeDef sMasterConfig;
  TIM_OC_InitTypeDef sConfigOC;
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig;


  if (RESMGR_STATUS_ACCESS_OK == ResMgr_Request(RESMGR_RESOURCE_RIF_RCC, RESMGR_RCC_RESOURCE(92)))
  {
    __HAL_RCC_GPIOC_CLK_ENABLE();
  }

  /* Acquire GPIOC8 using Resource manager */
  if (RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIF_GPIOC, RESMGR_GPIO_PIN(8)))
  {
	  Error_Handler();
  }

  /* Acquire GPIOC12 using Resource manager */
  if (RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIF_GPIOC, RESMGR_GPIO_PIN(12)))
  {
	  Error_Handler();
  }

  /* Acquire GPIOC10 using Resource manager */
  if (RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIF_GPIOC, RESMGR_GPIO_PIN(10)))
  {
	  Error_Handler();
  }

  htim8.Instance = TIM8;
  htim8.Init.Prescaler = Prescaler;
  htim8.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim8.Init.Period = PERIOD_VALUE;
  htim8.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim8.Init.RepetitionCounter = 0;
  htim8.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim8) != HAL_OK)
  {
     Error_Handler();
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim8, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }

  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = PULSE1_VALUE;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }

  sConfigOC.Pulse = PULSE3_VALUE;
  if (HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }

  sConfigOC.Pulse = PULSE4_VALUE;
  if (HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }

  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.BreakFilter = 0;
  sBreakDeadTimeConfig.BreakAFMode = TIM_BREAK_AFMODE_INPUT;
  sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
  sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
  sBreakDeadTimeConfig.Break2Filter = 0;
  sBreakDeadTimeConfig.Break2AFMode = TIM_BREAK_AFMODE_INPUT;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim8, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }

  HAL_TIM_MspPostInit(&htim8);

}

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
static void MX_GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
}

/*
  * @brief  TIM Deinitialization
  * @param  None
  * @retval None
 */
static void MX_TIM8_DeInit(void)
{
	/* Deinitialize the TIM peripheral */
	HAL_TIM_PWM_DeInit(&htim8);

    /* Release GPIOC8 using Resource manager */
	(void) ResMgr_Release(RESMGR_RESOURCE_RIF_GPIOC, RESMGR_GPIO_PIN(8));

	/* Release GPIOC12 using Resource manager */
	(void) ResMgr_Release(RESMGR_RESOURCE_RIF_GPIOC, RESMGR_GPIO_PIN(12));

	/* Release GPIOC10 using Resource manager */
	(void) ResMgr_Release(RESMGR_RESOURCE_RIF_GPIOC, RESMGR_GPIO_PIN(10));

	/* Release TIM8 using Resource manager */
	(void) ResMgr_Release(RESMGR_RESOURCE_RIFSC, STM32MP21_RIFSC_TIM8_ID);

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
  /* Deinitialize the TIM peripheral and resources*/
  MX_TIM8_DeInit();

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
  Error_Handler();
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
