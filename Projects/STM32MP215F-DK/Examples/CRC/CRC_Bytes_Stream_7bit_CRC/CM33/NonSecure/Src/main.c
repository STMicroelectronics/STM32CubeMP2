/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    CRC/CRC_Bytes_Stream_7bit_CRC/CM33/NonSecure/Src/main.c
  * @author  MCD Application Team
  * @brief   This sample code shows how to use the STM32MP2xx CRC HAL API
  *          to compute 7-bit CRC codes from buffers of data bytes (8-bit data),
  *          based on a user-defined generating polynomial.
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
#include <stdio.h>
#include "copro_sync.h"


/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
#ifdef DEBUG
volatile int debug = 1;
#endif
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
#define BUFFER_SIZE_5  5  /* CRC7_DATA8_TEST5[] is 5-byte long   */
#define BUFFER_SIZE_17 17 /* CRC7_DATA8_TEST17[] is 17-byte long */
#define BUFFER_SIZE_1  1  /* CRC7_DATA8_TEST1[] is 1-byte long   */
#define BUFFER_SIZE_2  2  /* CRC7_DATA8_TEST2[] is 2-byte long   */

/* User-defined polynomial */
#define CRC_POLYNOMIAL_7B  0x65  /* X^7 + X^6 + X^5 + X^2 + 1,
                                   used in Train Communication Network, IEC 60870-5[17] */
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

CRC_HandleTypeDef hcrc;
IPCC_HandleTypeDef hipcc;

/* Used for storing CRC Value */
__IO uint32_t uwCRCValue = 0;

/* Bytes buffers that will consecutively yield CRCs */
static const uint8_t CRC7_DATA8_TEST5[5]   = {0x12, 0x34, 0xBA, 0x71, 0xAD};
static const uint8_t CRC7_DATA8_TEST17[17] = {0x12, 0x34, 0xBA, 0x71, 0xAD,
                                              0x11, 0x56, 0xDC, 0x88, 0x1B,
                                              0xEE, 0x4D, 0x82, 0x93, 0xA6,
                                              0x7F, 0xC3
                                             };
static const uint8_t CRC7_DATA8_TEST1[1]   = {0x19};
static const uint8_t CRC7_DATA8_TEST2[2]   = {0xAB, 0xCD};

uint32_t * CRC7_DATA8_PTR_TEST1 = (uint32_t *)CRC7_DATA8_TEST1;
uint32_t * CRC7_DATA8_PTR_TEST2 = (uint32_t *)CRC7_DATA8_TEST2;

/* Expected CRC Values */
/* The 7 LSB bits are the 7-bit long CRC */
uint32_t uwExpectedCRCValue_1 = 0x00000057;    /* First byte stream CRC  */
uint32_t uwExpectedCRCValue_2 = 0x0000006E;    /* Second byte stream CRC */
uint32_t uwExpectedCRCValue_3 = 0x0000004B;    /* Third byte stream CRC  */
uint32_t uwExpectedCRCValue_4 = 0x00000026;    /* Fourth byte stream CRC */

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void MX_CRC_Init(void);
static void MX_CRC_DeInit(void);
static void MX_IPCC_Init(void);

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
#ifdef DEBUG
  while(debug==1);
#endif

  uint32_t status;

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock for dev mode*/
  if(IS_DEVELOPER_BOOT_MODE())
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
  }

  /* USER CODE BEGIN 2 */

  /* Initialize all configured peripherals */
  MX_CRC_Init();
  
  /****************************************************************************/
  /*                                                                          */
  /*         CRC computation of a first bytes stream                          */
  /*                                                                          */
  /****************************************************************************/

  /* The 7-bit long CRC of a 5-byte buffer is computed. After peripheral initialization,
     the CRC calculator is initialized with the default value that is 0x7F for
     a 7-bit CRC.

    The computed CRC is stored in uint32_t uwCRCValue. The 7-bit long CRC is made of
    uwCRCValue 7 LSB bits. */

  uwCRCValue = HAL_CRC_Accumulate(&hcrc, (uint32_t *)&CRC7_DATA8_TEST5, BUFFER_SIZE_5);

  /* Compare the CRC value to the expected one */
  if (uwCRCValue != uwExpectedCRCValue_1)
  {
    /* Wrong CRC value: Turn the LED3 On */
    Error_Handler();
  }


  /****************************************************************************/
  /*                                                                          */
  /*         CRC computation of a second bytes stream                         */
  /*                                                                          */
  /****************************************************************************/

  /* The 7-bit long CRC of a 17-byte buffer is computed. The CRC calculator
    is not re-initialized, instead the previously computed CRC is used
    as initial value. */

  uwCRCValue = HAL_CRC_Accumulate(&hcrc, (uint32_t *)&CRC7_DATA8_TEST17, BUFFER_SIZE_17);

  /* Compare the CRC value to the expected one */
  if (uwCRCValue != uwExpectedCRCValue_2)
  {
    /* Wrong CRC value: Turn the LED3 On */
    Error_Handler();
  }


  /****************************************************************************/
  /*                                                                          */
  /*         CRC computation of a single byte                                 */
  /*                                                                          */
  /****************************************************************************/

  /* The 7-bit long CRC of a 1-byte buffer is computed. The CRC calculator
    is not re-initialized, instead the previously computed CRC is used
    as initial value. */

  uwCRCValue = HAL_CRC_Accumulate(&hcrc, (uint32_t *)CRC7_DATA8_PTR_TEST1, BUFFER_SIZE_1);

  /* Compare the CRC value to the expected one */
  if (uwCRCValue != uwExpectedCRCValue_3)
  {
    /* Wrong CRC value: Turn the LED3 On */
    Error_Handler();
  }


  /****************************************************************************/
  /*                                                                          */
  /*         CRC computation of the last bytes stream                         */
  /*                                                                          */
  /****************************************************************************/

  /* The 7-bit long CRC of a 2-byte buffer is computed. The CRC calculator
    is re-initialized with the default value that is 0x7F for a 7-bit CRC.
    This is done with a call to HAL_CRC_Calculate() instead of
    HAL_CRC_Accumulate(). */

  uwCRCValue = HAL_CRC_Calculate(&hcrc, (uint32_t *)CRC7_DATA8_PTR_TEST2, BUFFER_SIZE_2);

  /* Compare the CRC value to the expected one */
  if (uwCRCValue != uwExpectedCRCValue_4)
  {
    /* Wrong CRC value: Turn the LED3 On */
    Error_Handler();
  }
  /* USER CODE END 2 */

  /* Right CRC value: Blink LED3 */

  /* Infinite loop */
  while (1)
  {
    /* Toggle LED3 */
    BSP_LED_Toggle(LED3);

    /* Delay */
    HAL_Delay(500);

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
  * @brief CRC Initialization Function
  * @param None
  * @retval None
  */
static void MX_CRC_Init(void)
{

  /* Acquire CRC using Resource manager */
  if (RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIFSC, RESMGR_RIFSC_CRC_ID))
  {
	  Error_Handler();
  }
  /* USER CODE BEGIN CRC_Init 0 */

  /* USER CODE END CRC_Init 0 */

  /* USER CODE BEGIN CRC_Init 1 */

  /* USER CODE END CRC_Init 1 */
  hcrc.Instance = CRC;
  hcrc.Init.DefaultPolynomialUse = DEFAULT_POLYNOMIAL_DISABLE;
  hcrc.Init.DefaultInitValueUse = DEFAULT_INIT_VALUE_ENABLE;
  hcrc.Init.GeneratingPolynomial = 101;
  hcrc.Init.CRCLength = CRC_POLYLENGTH_7B;
  hcrc.Init.InputDataInversionMode = CRC_INPUTDATA_INVERSION_NONE;
  hcrc.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_DISABLE;
  hcrc.InputDataFormat = CRC_INPUTDATA_FORMAT_BYTES;
  if (HAL_CRC_Init(&hcrc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CRC_Init 2 */

  /* USER CODE END CRC_Init 2 */

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
  * @brief CRC DeInitialization Function
  * @param None
  * @retval None
  */
static void MX_CRC_DeInit(void)
{
	/* Deinitialize the CRC peripheral */
	HAL_CRC_DeInit(&hcrc);
	/* Release CRC using Resource manager */
	(void) ResMgr_Release(RESMGR_RESOURCE_RIFSC, RESMGR_RIFSC_CRC_ID);
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
  /* Deinitialize the CRC peripheral */
  MX_CRC_DeInit();

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
	/* Blink the LED3 */
	while (1)
	{
	  /* Error if LED3 is ON */
	  BSP_LED_On(LED3);
	  while (1)
	  {
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
