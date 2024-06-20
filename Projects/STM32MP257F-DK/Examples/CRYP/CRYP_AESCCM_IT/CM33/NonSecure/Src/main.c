/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    CRYP/CRYP_AESCCM_IT/CM33/NonSecure/Src/main.c
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
/* CRYP handler declaration */
CRYP_HandleTypeDef hcryp; /* CRYP handler declaration */

IPCC_HandleTypeDef   hipcc;

__IO uint32_t CrypCompleteDetected = 0; /* CRYP complete detection */

/* AES Key  */
uint32_t AES192Key[6] =   {0xF9677EA3,0x01BAA45D,0x41A33918,0x81D8CDB1, 0x296E6C88, 0x09114C80,};

/* B0 block */
uint32_t BlockB0[4] = {0x7B2F50A1,0xBA5F9AA4,0xC477A82B,0x00000010};

/* B1 block*/
uint32_t BlockB1[4] = {0x000E7578,0x561AD597,0xBD0EAF3F,0xE77DF111};

/* Plaintext */
uint32_t Plaintext[4] = {0xEC5D99F5,0x6EE67628,0x5DF80935,0x0726067E};

/* Expected CCM Ciphertext with AESKey192 */
uint32_t Ciphertext[4] = {0xAD8B6E0D,0x08040B0B,0xCDAC71EE,0xDCD486AF};

/* Expected CCM TAG */
uint32_t ExpectedTAG[4] = {0xEF8DFABB,0x29F6FE68,0xD70AA868,0x26BB30DB};


/* Used for storing the encrypted text */
uint32_t Encryptedtext[4]={0};

/* Used for storing the decrypted text */
uint32_t Decryptedtext[4]={0};

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
static void MX_IPCC_Init(void);
static void MX_CRYP_DeInit(void);
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

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  if (IS_DEVELOPER_BOOT_MODE())
  {
	SystemClock_Config();
  }

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

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
  MX_GPIO_Init();
  MX_CRYP_Init();

  /* USER CODE BEGIN 2 */

  /*##-1- AES CCM Encryption & TAG generation in interrupt mode ##############*/

  /* Encryption, result in  Encryptedtext buffer */
  HAL_CRYP_Encrypt_IT(&hcryp, Plaintext, 4, Encryptedtext);

  /*Wait until output transfer complete*/
  while(CrypCompleteDetected == 0)
  {  }

  /* Reset Output Transfer Complete Detect */
  CrypCompleteDetected = 0;

  /*Compare results with expected buffer*/
  if(memcmp(Encryptedtext, Ciphertext, 16) != 0)
  {
    /* Not expected result, wrong on Encryptedtext buffer: Turn LED3 on */
    Error_Handler();
  }

  /* Generate the authentication TAG */
  HAL_CRYPEx_AESCCM_GenerateAuthTAG(&hcryp,TAG, TIMEOUT_VALUE);

  /*Compare results with expected TAG buffer*/
  if(memcmp(TAG, ExpectedTAG, 16) != 0)
  {
    /* Not expected result, wrong on TAG buffer: Turn LED3 on */
    Error_Handler();
  }

  /*##-2- AES CCM Decryption & TAG generation in interrupt mode ################*/

  /* Decryption, result in  Decryptedtext buffer */
  HAL_CRYP_Decrypt_IT(&hcryp,Ciphertext , 4, Decryptedtext);

  /*Wait until output transfer complete*/
  while(CrypCompleteDetected == 0)
  {  }

  /* Reset Output Transfer Complete Detect */
  CrypCompleteDetected = 0;

  /*Compare results with expected buffer*/
  if(memcmp(Decryptedtext, Plaintext, 16) != 0)
  {
    /* Not expected result, wrong on DecryptedText buffer: Turn LED3 on */
    Error_Handler();
  }

  /* Generate the authentication TAG */
  HAL_CRYPEx_AESCCM_GenerateAuthTAG(&hcryp,TAG, TIMEOUT_VALUE);

  /*Compare results with expected buffer*/
  if(memcmp(TAG, ExpectedTAG, 16) != 0)
  {
    /* Not expected result, wrong on TAG buffer: Turn LED3 on */
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

  /* Acquire CRYP2 using Resource manager */
  if (RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIFSC, RESMGR_RIFSC_CRYP2_ID))
  {
  Error_Handler();
  }

  /*## Set the CRYP parameters and initialize the CRYP IP  ###################*/

  hcryp.Instance        = CRYP2;
  hcryp.Init.DataType   = CRYP_DATATYPE_32B;
  hcryp.Init.KeySize    = CRYP_KEYSIZE_192B;
  hcryp.Init.pKey       = AES192Key;
  hcryp.Init.Algorithm  = CRYP_AES_CCM;
  hcryp.Init.Header     = BlockB1;
  hcryp.Init.HeaderSize = 4;
  hcryp.Init.B0         = BlockB0;
  if (HAL_CRYP_Init(&hcryp) != HAL_OK)
  {
  	Error_Handler();
  }

}

static void MX_CRYP_DeInit(void)
{
  /* USER CODE BEGIN MX_CRYP_DeInit */

  /* USER CODE END MX_CRYP_DeInit */

  /* Deinitialize the CRYP peripheral */
  HAL_CRYP_DeInit(&hcryp);

  /* Release CRYP2 using Resource manager */
  (void) ResMgr_Release(RESMGR_RESOURCE_RIFSC, RESMGR_RIFSC_CRYP2_ID);

}

/**
  * @brief  Output FIFO transfer completed callbacks.
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @retval 1 if output FIFO transfer completed
  */
 void HAL_CRYP_OutCpltCallback(CRYP_HandleTypeDef *hcryp)
 {
   CrypCompleteDetected = 1;
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
   /* Deinitialize the CRYP peripheral  and release the CRYP resource*/
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
