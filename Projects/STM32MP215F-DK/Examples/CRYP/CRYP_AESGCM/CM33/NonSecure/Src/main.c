/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    CRYP/CRYP_AESGCM/CM33/NonSecure/Src/main.c
  * @author  MCD Application Team
  * @brief   This sample code shows how to use the STM32MP2xx CRYP HAL API
  *          to encrypt/decrypt data and get TAG using AES in GCM Algorithm.
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
/** @addtogroup STM32MP2xx_HAL_Examples
  * @{
  */

/** @addtogroup CRYP_Example
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
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
IPCC_HandleTypeDef   hipcc;
/* CRYP handler declaration */
CRYP_HandleTypeDef hcryp;

/* AES Key  */
uint32_t AES128Key[4] = {0xfeffe992,0x8665731c,0x6d6a8f94,0x67308308};

/* below InitVector is the Initial Counter Block (ICB)composed of IV and counter*/
uint32_t InitVector[4] = {0xcafebabe,0xfacedbad,0xdecaf888 , 0x00000002};

/* Header */
uint32_t HeaderMessage[5] = {0xfeedface, 0xdeadbeef, 0xfeedface, 0xdeadbeef,0xabaddad2};

/* Plaintext */
uint32_t Plaintext[15] = {0xd9313225,0xf88406e5,0xa55909c5,0xaff5269a
                         ,0x86a7a953,0x1534f7da,0x2e4c303d,0x8a318a72
                         ,0x1c3c0c95,0x95680953,0x2fcf0e24,0x49a6b525
                         ,0xb16aedf5,0xaa0de657,0xba637b39};

/* Expected GCM Ciphertext with AESKey128 */
uint32_t Ciphertext[15] = {0x42831ec2,0x21777424,0x4b7221b7,0x84d0d49c
                          ,0xe3aa212f,0x2c02a4e0,0x35c17e23,0x29aca12e
                          ,0x21d514b2,0x5466931c,0x7d8f6a5a,0xac84aa05
                          ,0x1ba30b39,0x6a0aac97,0x3d58e091};
/* Expected GCM TAG */
uint32_t ExpectedTAG[4]={0x5bc94fbc,0x3221a5db,0x94fae95a,0xe7121a47};

/* Used for storing the encrypted text */
uint32_t Encryptedtext[16]={0};

/* Used for storing the decrypted text */
uint32_t Decryptedtext[16]={0};

/* Used for storing the computed MAC (TAG) */
uint32_t TAG[4]={0};

#ifdef DEBUG
volatile uint8_t debug = 1;
#endif /* DEBUG */

/* USER CODE BEGIN PV */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_CRYP_Init(void);
static void MX_CRYP_DeInit(void);
static void MX_IPCC_Init(void);
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
#ifdef DEBUG
  while(debug);
#endif /* DEBUG */
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

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

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Configure LED3 */
  BSP_LED_Init(LED3);

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_CRYP_Init();

  /* USER CODE BEGIN 2 */

  if(!IS_DEVELOPER_BOOT_MODE())
  {
	/* IPCC initialization */
	MX_IPCC_Init();

	/*Corpo Sync Initialization*/
	CoproSync_Init();
  }
  /*## AES GCM Encryption and TAG generation  ################################*/

  /* Encryption, result in  Encryptedtext buffer */
  HAL_CRYP_Encrypt(&hcryp, Plaintext, 15, Encryptedtext, TIMEOUT_VALUE);

  /*Compare results with expected buffer*/
  if(memcmp(Encryptedtext, Ciphertext, 60) != 0)
  {
    /* Not expected result, wrong on Encryptedtext buffer: Turn LED3 ON */
    Error_Handler();
  }

  /* Generate the authentication TAG */
  HAL_CRYPEx_AESGCM_GenerateAuthTAG(&hcryp,TAG, TIMEOUT_VALUE);

  /*Compare results with expected TAG buffer*/
  if(memcmp(TAG, ExpectedTAG, 16) != 0)
  {
    /* Not expected result, wrong on TAG buffer: Turn LED3 ON */
    Error_Handler();
  }

  /*## AES GCM Decryption and TAG generation   ##############################*/

  /* Decryption, result in  Decryptedtext buffer */
  HAL_CRYP_Decrypt(&hcryp,Ciphertext , 15, Decryptedtext, TIMEOUT_VALUE);
  /*Compare results with expected buffer*/
  if(memcmp(Decryptedtext, Plaintext, 60) != 0)
  {
    /* Not expected result, wrong on DecryptedText buffer: Turn LED3 ON */
    Error_Handler();
  }

  /* Generate the authentication TAG */
  HAL_CRYPEx_AESGCM_GenerateAuthTAG(&hcryp,TAG, TIMEOUT_VALUE);

  /*Compare results with expected buffer*/
  if(memcmp(TAG, ExpectedTAG, 16) != 0)
  {
    /* Not expected result, wrong on TAG buffer: Turn LED3 ON */
    Error_Handler();
  }

  /* Right Encryptedtext & Decryptedtext buffer : Blink LED3 */

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
  * @brief CRYP DeInitialization Function
  * @param None
  * @retval None
  */
static void MX_CRYP_DeInit(void)
{
	/* Deinitialize the CRYP peripheral */
	HAL_CRYP_DeInit(&hcryp);

	/* Release CRYP2 using Resource manager */
	ResMgr_Release(RESMGR_RESOURCE_RIFSC, RESMGR_RIFSC_CRYP2_ID);
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

static void MX_CRYP_Init(void)
{
	/* USER CODE BEGIN MX_CRYP_Init */

	/* USER CODE END MX_CRYP_Init */

	/*## Set the CRYP parameters and initialize the CRYP IP  ###################*/

	/* Acquire CRYP2 using Resource manager */

	if (RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIFSC, RESMGR_RIFSC_CRYP2_ID))
	{
		Error_Handler();
	}

	hcryp.Instance        = CRYP2;
	hcryp.Init.DataType   = CRYP_DATATYPE_32B;
	hcryp.Init.KeySize    = CRYP_KEYSIZE_128B;
	hcryp.Init.pKey       = AES128Key;
	hcryp.Init.Algorithm  = CRYP_AES_GCM;
	hcryp.Init.pInitVect  = InitVector;
	hcryp.Init.Header     = HeaderMessage;
	hcryp.Init.HeaderSize = 5;
	if (HAL_CRYP_Init(&hcryp) != HAL_OK)
	{
		Error_Handler();
	}

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
  /* Deinitialize the CRYP peripheral */
  MX_CRYP_DeInit();

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
