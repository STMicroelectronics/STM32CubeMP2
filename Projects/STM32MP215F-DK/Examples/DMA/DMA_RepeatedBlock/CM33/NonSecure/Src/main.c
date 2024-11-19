/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    DMA/DMA_RepeatedBlock/CM33/NonSecure/Src/main.c
  * @author  MCD Application Team
  * @brief   This example describes how to configure and use the DMA HAL API to perform repeated
  *          block transactions.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* DMA flags declaration */
__IO uint32_t TransferCompleteDetected = 0U, TransferErrorDetected = 0U;

/* Source buffers definition */
static const uint32_t aSRC_Const_Buffer[SRC_BUFFER_SIZE] =
{
  0xAAAAAAAA, 0xAAAAAAAA, 0xAAAAAAAA, 0xAAAAAAAA,

  0x01020304, 0x05060708, 0x090A0B0C, 0x0D0E0F10,
  0x11121314, 0x15161718, 0x191A1B1C, 0x1D1E1F20,

  0xBBBBBBBB, 0xBBBBBBBB, 0xBBBBBBBB, 0xBBBBBBBB,

  0x21222324, 0x25262728, 0x292A2B2C, 0x2D2E2F30,
  0x31323334, 0x35363738, 0x393A3B3C, 0x3D3E3F40,

  0xCCCCCCCC, 0xCCCCCCCC, 0xCCCCCCCC, 0xCCCCCCCC,

  0x41424344, 0x45464748, 0x494A4B4C, 0x4D4E4F50,
  0x51525354, 0x55565758, 0x595A5B5C, 0x5D5E5F60,

  0xDDDDDDDD, 0xDDDDDDDD, 0xDDDDDDDD, 0xDDDDDDDD,

  0x61626364, 0x65666768, 0x696A6B6C, 0x6D6E6F70,
  0x71727374, 0x75767778, 0x797A7B7C, 0x7D7E7F80
};

static const uint32_t aIncrementGather_ExpectedResult_Buffer[GATHER_BUFFER_SIZE] =
{
  0xAAAAAAAA, 0xAAAAAAAA, 0xAAAAAAAA, 0xAAAAAAAA,
  0xBBBBBBBB, 0xBBBBBBBB, 0xBBBBBBBB, 0xBBBBBBBB,
  0xCCCCCCCC, 0xCCCCCCCC, 0xCCCCCCCC, 0xCCCCCCCC,
  0xDDDDDDDD, 0xDDDDDDDD, 0xDDDDDDDD, 0xDDDDDDDD
};

static const uint32_t aDecrementGather_ExpectedResult_Buffer[GATHER_BUFFER_SIZE] =
{
  0xDDDDDDDD, 0xDDDDDDDD, 0xDDDDDDDD, 0xDDDDDDDD,
  0xCCCCCCCC, 0xCCCCCCCC, 0xCCCCCCCC, 0xCCCCCCCC,
  0xBBBBBBBB, 0xBBBBBBBB, 0xBBBBBBBB, 0xBBBBBBBB,
  0xAAAAAAAA, 0xAAAAAAAA, 0xAAAAAAAA, 0xAAAAAAAA
};

static const uint32_t aDecrementScatter_ExpectedResult_Buffer[SRC_BUFFER_SIZE] =
{
  0xDDDDDDDD, 0xDDDDDDDD, 0xDDDDDDDD, 0xDDDDDDDD,

  0x01020304, 0x05060708, 0x090A0B0C, 0x0D0E0F10,
  0x11121314, 0x15161718, 0x191A1B1C, 0x1D1E1F20,

  0xCCCCCCCC, 0xCCCCCCCC, 0xCCCCCCCC, 0xCCCCCCCC,

  0x21222324, 0x25262728, 0x292A2B2C, 0x2D2E2F30,
  0x31323334, 0x35363738, 0x393A3B3C, 0x3D3E3F40,

  0xBBBBBBBB, 0xBBBBBBBB, 0xBBBBBBBB, 0xBBBBBBBB,

  0x41424344, 0x45464748, 0x494A4B4C, 0x4D4E4F50,
  0x51525354, 0x55565758, 0x595A5B5C, 0x5D5E5F60,

  0xAAAAAAAA, 0xAAAAAAAA, 0xAAAAAAAA, 0xAAAAAAAA,

  0x61626364, 0x65666768, 0x696A6B6C, 0x6D6E6F70,
  0x71727374, 0x75767778, 0x797A7B7C, 0x7D7E7F80
};

/* Destination increment gather buffer */
static uint32_t aDST_IncrementGather_Buffer[GATHER_BUFFER_SIZE];
/* Destination decrement gather buffer */
static uint32_t aDST_DecrementGather_Buffer[GATHER_BUFFER_SIZE];

/* Destination increment scatter buffer */
static uint32_t aDST_IncrementScatter_Buffer[SCATTER_BUFFER_SIZE] =
{
  0x00000000, 0x00000000, 0x00000000, 0x00000000,

  0x01020304, 0x05060708, 0x090A0B0C, 0x0D0E0F10,
  0x11121314, 0x15161718, 0x191A1B1C, 0x1D1E1F20,

  0x00000000, 0x00000000, 0x00000000, 0x00000000,

  0x21222324, 0x25262728, 0x292A2B2C, 0x2D2E2F30,
  0x31323334, 0x35363738, 0x393A3B3C, 0x3D3E3F40,

  0x00000000, 0x00000000, 0x00000000, 0x00000000,

  0x41424344, 0x45464748, 0x494A4B4C, 0x4D4E4F50,
  0x51525354, 0x55565758, 0x595A5B5C, 0x5D5E5F60,

  0x00000000, 0x00000000, 0x00000000, 0x00000000,

  0x61626364, 0x65666768, 0x696A6B6C, 0x6D6E6F70,
  0x71727374, 0x75767778, 0x797A7B7C, 0x7D7E7F80
};
/* Destination decrement scatter buffer */
static uint32_t aDST_DecrementScatter_Buffer[SCATTER_BUFFER_SIZE] =
{
  0x00000000, 0x00000000, 0x00000000, 0x00000000,

  0x01020304, 0x05060708, 0x090A0B0C, 0x0D0E0F10,
  0x11121314, 0x15161718, 0x191A1B1C, 0x1D1E1F20,

  0x00000000, 0x00000000, 0x00000000, 0x00000000,

  0x21222324, 0x25262728, 0x292A2B2C, 0x2D2E2F30,
  0x31323334, 0x35363738, 0x393A3B3C, 0x3D3E3F40,

  0x00000000, 0x00000000, 0x00000000, 0x00000000,

  0x41424344, 0x45464748, 0x494A4B4C, 0x4D4E4F50,
  0x51525354, 0x55565758, 0x595A5B5C, 0x5D5E5F60,

  0x00000000, 0x00000000, 0x00000000, 0x00000000,

  0x61626364, 0x65666768, 0x696A6B6C, 0x6D6E6F70,
  0x71727374, 0x75767778, 0x797A7B7C, 0x7D7E7F80
};

#ifdef DEBUG
  volatile uint8_t debug = 1;
#endif

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
DMA_HandleTypeDef handle_HPDMA3_Channel12;
IPCC_HandleTypeDef hipcc;
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void MX_HPDMA3_Init(void);
static void MX_HPDMA3_DeInit(void);
static void MX_IPCC_Init(void);

void SystemClock_Config(void);
void Error_Handler(void);

static void DMA_Increment_ScatterOperation(void);
static void DMA_Decrement_GatherOperation(void);
static void DMA_Decrement_ScatterOperation(void);

static void TransferComplete(DMA_HandleTypeDef *hdma);
static void TransferError(DMA_HandleTypeDef *hdma);

static uint16_t Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength);

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
uint8_t status;
  /* USER CODE END 1 */

  #ifdef DEBUG
   while(debug == 1);
  #endif

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock in dev mode*/
  if(IS_DEVELOPER_BOOT_MODE())
  {
    SystemClock_Config();
  }

  /* USER CODE BEGIN SysInit */
  status = BSP_PMIC_Init();
  if (status)
  {
    printf("error :  BSP PMIC init fail\r\n");
  }

  status = BSP_PMIC_Power_Mode_Init();
  if (status)
  {
    printf("error :  BSP PMIC Mode init fail\r\n");
  }
  /* USER CODE END SysInit */

  /* Initialize LED3 */
   BSP_LED_Init(LED3);

   if(!IS_DEVELOPER_BOOT_MODE())
   {
     /* IPCC initialization */
     MX_IPCC_Init();
     /*CoproSync Initialization*/
     CoproSync_Init();
   }
  /* Initialize all configured peripherals */
   MX_HPDMA3_Init();

  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Wait for end of transmission or an error occurred */
    while ((TransferCompleteDetected == 0) && (TransferErrorDetected == 0U));

    /* Check DMA error */
    if (TransferErrorDetected == 1U)
    {
      Error_Handler();
    }

    /* Check destination buffer */
    if (Buffercmp((uint8_t*)aDST_IncrementGather_Buffer, (uint8_t*)aIncrementGather_ExpectedResult_Buffer, GATHER_BUFFER_SIZE) != 0U)
    {
      Error_Handler();
    }

    /* Reset DMA flags */
    TransferCompleteDetected = 0U;
    TransferErrorDetected    = 0U;


    /*##-2- Incremented scatter DMA transaction #################################*/
    DMA_Increment_ScatterOperation();

    /* Wait for end of transmission or an error occurred */
    while ((TransferCompleteDetected == 0) && (TransferErrorDetected == 0U));

    /* Check DMA error */
    if (TransferErrorDetected == 1U)
    {
      Error_Handler();
    }

    /* Check destination buffer */
    if (Buffercmp((uint8_t*)aDST_IncrementGather_Buffer, (uint8_t*)aSRC_Const_Buffer, GATHER_BUFFER_SIZE) != 0U)
    {
      Error_Handler();
    }

    /* Reset DMA flags */
    TransferCompleteDetected = 0U;
    TransferErrorDetected    = 0U;


    /*##-3- Decremented gather DMA transaction #################################*/
    DMA_Decrement_GatherOperation();

    /* Wait for end of transmission or an error occurred */
    while ((TransferCompleteDetected == 0) && (TransferErrorDetected == 0U));

    /* Check DMA error */
    if (TransferErrorDetected == 1U)
    {
      Error_Handler();
    }

    /* Check destination buffer */
    if (Buffercmp((uint8_t*)aDST_DecrementGather_Buffer, (uint8_t*)aDecrementGather_ExpectedResult_Buffer, GATHER_BUFFER_SIZE) != 0U)
    {
      Error_Handler();
    }

    /* Reset DMA flags */
    TransferCompleteDetected = 0U;
    TransferErrorDetected    = 0U;


    /*##-4- Decremented scatter DMA transaction #################################*/
    DMA_Decrement_ScatterOperation();

    /* Wait for end of transmission or an error occurred */
    while ((TransferCompleteDetected == 0) && (TransferErrorDetected == 0U));

    /* Check DMA error */
    if (TransferErrorDetected == 1U)
    {
      Error_Handler();
    }

    /* Check destination buffer */
    if (Buffercmp((uint8_t*)aDST_DecrementScatter_Buffer, (uint8_t*)aDecrementScatter_ExpectedResult_Buffer, GATHER_BUFFER_SIZE) != 0U)
    {
      Error_Handler();
    }

    /* Reset DMA flags */
    TransferCompleteDetected = 0U;
    TransferErrorDetected    = 0U;

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
void SystemClock_Config(void) {
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
  * @brief HPDMA3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_HPDMA3_Init(void)
{

  /* USER CODE BEGIN HPDMA3_Init 0 */

  /* USER CODE END HPDMA3_Init 0 */

  DMA_RepeatBlockConfTypeDef RepeatBlockConfig = {0};

  /* HPDMA3 clock enable */
  if(ResMgr_Request(RESMGR_RESOURCE_RIF_RCC, RESMGR_RCC_RESOURCE(85)) == RESMGR_STATUS_ACCESS_OK)
  {
    __HAL_RCC_HPDMA3_CLK_ENABLE();
  }

  /* Acquire HPDMA3 Channel 12 using Resource manager */
  if (RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIF_HPDMA3, RESMGR_HPDMA_CHANNEL(12)))
  {
    Error_Handler();
  }

  /* HPDMA3 interrupt Init */
  HAL_NVIC_SetPriority(HPDMA3_Channel12_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(HPDMA3_Channel12_IRQn);

  /* USER CODE BEGIN HPDMA3_Init 1 */

  /* USER CODE END HPDMA3_Init 1 */
  handle_HPDMA3_Channel12.Instance = HPDMA3_Channel12;
  handle_HPDMA3_Channel12.Init.Request = DMA_REQUEST_SW;
  handle_HPDMA3_Channel12.Init.BlkHWRequest = DMA_BREQ_SINGLE_BURST;
  handle_HPDMA3_Channel12.Init.Direction = DMA_MEMORY_TO_MEMORY;
  handle_HPDMA3_Channel12.Init.SrcInc = DMA_SINC_INCREMENTED;
  handle_HPDMA3_Channel12.Init.DestInc = DMA_DINC_INCREMENTED;
  handle_HPDMA3_Channel12.Init.SrcDataWidth = DMA_SRC_DATAWIDTH_WORD;
  handle_HPDMA3_Channel12.Init.DestDataWidth = DMA_DEST_DATAWIDTH_WORD;
  handle_HPDMA3_Channel12.Init.Priority = DMA_LOW_PRIORITY_HIGH_WEIGHT;
  handle_HPDMA3_Channel12.Init.SrcBurstLength = 1;
  handle_HPDMA3_Channel12.Init.DestBurstLength = 1;
  handle_HPDMA3_Channel12.Init.TransferAllocatedPort = DMA_SRC_ALLOCATED_PORT1|DMA_DEST_ALLOCATED_PORT1;
  handle_HPDMA3_Channel12.Init.TransferEventMode = DMA_TCEM_BLOCK_TRANSFER;
  handle_HPDMA3_Channel12.Init.Mode = DMA_NORMAL;
  if (HAL_DMA_Init(&handle_HPDMA3_Channel12) != HAL_OK)
  {
    Error_Handler();
  }
  RepeatBlockConfig.RepeatCount = 4;
  RepeatBlockConfig.SrcAddrOffset = 0;
  RepeatBlockConfig.DestAddrOffset = 0;
  RepeatBlockConfig.BlkSrcAddrOffset = 32;
  RepeatBlockConfig.BlkDestAddrOffset = 0;
  if (HAL_DMAEx_ConfigRepeatBlock(&handle_HPDMA3_Channel12, &RepeatBlockConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if(IS_DEVELOPER_BOOT_MODE())
  {
    if (HAL_DMA_ConfigChannelAttributes(&handle_HPDMA3_Channel12, (DMA_CHANNEL_PRIV | DMA_CHANNEL_SEC | DMA_CHANNEL_DEST_SEC | DMA_CHANNEL_SRC_SEC)) != HAL_OK)
    {
      Error_Handler();
    }
  }

 /* Select Callbacks functions called after Half Transfer Complete, Transfer Complete and Transfer Error */
  HAL_DMA_RegisterCallback(&handle_HPDMA3_Channel12, HAL_DMA_XFER_CPLT_CB_ID, TransferComplete);
  HAL_DMA_RegisterCallback(&handle_HPDMA3_Channel12, HAL_DMA_XFER_ERROR_CB_ID, TransferError);

  /* Configure the source, destination and buffer size DMA fields and Start DMA Channel/Stream transfer */
  /* Enable All the DMA interrupts */
  if (HAL_DMA_Start_IT(&handle_HPDMA3_Channel12, (uint32_t)&aSRC_Const_Buffer, (uint32_t)&aDST_IncrementGather_Buffer, GATHER_BLOCK_SIZE) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN HPDMA3_Init 2 */

  /* USER CODE END HPDMA3_Init 2 */

}

/**
  * @brief HPDMA3 DeInitialization Function
  * @param None
  * @retval None
  */
static void MX_HPDMA3_DeInit(void)
{
  /* HPDMA3 interrupt DeInit */
  HAL_NVIC_DisableIRQ(HPDMA3_Channel12_IRQn);

  /* Deinitialize the DMA peripheral */
  HAL_DMA_DeInit(&handle_HPDMA3_Channel12);

  /* Release HPDMA3 Channel 13 using Resource manager */
  ResMgr_Release(RESMGR_RESOURCE_RIF_HPDMA3, RESMGR_HPDMA_CHANNEL(12));

  /* Release RCC HPDMA3 resource using Resource manager */
  ResMgr_Release(RESMGR_RESOURCE_RIF_RCC, RESMGR_RCC_RESOURCE(85));

  /* USER CODE BEGIN HPDMA3_DeInit 0 */

  /* USER CODE END HPDMA3_DeInit 0 */
}


/**
  * @brief  Increment scatter DMA transmit.
  * @param  None
  * @retval None
  */
static void DMA_Increment_ScatterOperation(void)
{

  /* USER CODE BEGIN DMA_Increment_ScatterOperation 0 */

  /* USER CODE END DMA_Increment_ScatterOperation 0 */

  DMA_RepeatBlockConfTypeDef RepeatBlockConfig = {0};

  /* Peripheral clock enable */
  if(!__HAL_RCC_HPDMA3_IS_CLK_ENABLED())
  {
    __HAL_RCC_HPDMA3_CLK_ENABLE();
  }

  /* HPDMA3 interrupt Init */
    HAL_NVIC_SetPriority(HPDMA3_Channel12_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(HPDMA3_Channel12_IRQn);

  /* USER CODE BEGIN DMA_Increment_ScatterOperation 1 */

  /* USER CODE END DMA_Increment_ScatterOperation 1 */
  handle_HPDMA3_Channel12.Instance = HPDMA3_Channel12;
  handle_HPDMA3_Channel12.Init.Request = DMA_REQUEST_SW;
  handle_HPDMA3_Channel12.Init.BlkHWRequest = DMA_BREQ_SINGLE_BURST;
  handle_HPDMA3_Channel12.Init.Direction = DMA_MEMORY_TO_MEMORY;
  handle_HPDMA3_Channel12.Init.SrcInc = DMA_SINC_INCREMENTED;
  handle_HPDMA3_Channel12.Init.DestInc = DMA_DINC_INCREMENTED;
  handle_HPDMA3_Channel12.Init.SrcDataWidth = DMA_SRC_DATAWIDTH_WORD;
  handle_HPDMA3_Channel12.Init.DestDataWidth = DMA_DEST_DATAWIDTH_WORD;
  handle_HPDMA3_Channel12.Init.Priority = DMA_LOW_PRIORITY_HIGH_WEIGHT;
  handle_HPDMA3_Channel12.Init.SrcBurstLength = 1;
  handle_HPDMA3_Channel12.Init.DestBurstLength = 1;
  handle_HPDMA3_Channel12.Init.TransferAllocatedPort = DMA_SRC_ALLOCATED_PORT0|DMA_DEST_ALLOCATED_PORT1;
  handle_HPDMA3_Channel12.Init.Mode = DMA_NORMAL;
  if (HAL_DMA_Init(&handle_HPDMA3_Channel12) != HAL_OK)
  {
    Error_Handler();
  }
  RepeatBlockConfig.RepeatCount = SCATTER_BLOCK_NUM;
  RepeatBlockConfig.SrcAddrOffset = 0;
  RepeatBlockConfig.DestAddrOffset = 0;
  RepeatBlockConfig.BlkSrcAddrOffset = 0;
  RepeatBlockConfig.BlkDestAddrOffset = SCATTER_BLOCK_OFFSET;
  if (HAL_DMAEx_ConfigRepeatBlock(&handle_HPDMA3_Channel12, &RepeatBlockConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /* Select Callbacks functions called after Half Transfer Complete, Transfer Complete and Transfer Error */
  HAL_DMA_RegisterCallback(&handle_HPDMA3_Channel12, HAL_DMA_XFER_CPLT_CB_ID, TransferComplete);
  HAL_DMA_RegisterCallback(&handle_HPDMA3_Channel12, HAL_DMA_XFER_ERROR_CB_ID, TransferError);

  /* Configure the source, destination and buffer size DMA fields and Start DMA Channel/Stream transfer */
  /* Enable All the DMA interrupts */
  if (HAL_DMA_Start_IT(&handle_HPDMA3_Channel12, (uint32_t)&aDST_IncrementGather_Buffer, (uint32_t)&aDST_IncrementScatter_Buffer, SCATTER_BLOCK_SIZE) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN DMA_Increment_ScatterOperation 2 */

  /* USER CODE END DMA_Increment_ScatterOperation 2 */

}

/**
  * @brief  Decrement gather DMA transmit.
  * @param  None
  * @retval None
  */
static void DMA_Decrement_GatherOperation(void)
{

  /* USER CODE BEGIN DMA_Decrement_GatherOperation 0 */

  /* USER CODE END DMA_Decrement_GatherOperation 0 */

  DMA_RepeatBlockConfTypeDef RepeatBlockConfig = {0};

  /* Peripheral clock enable */
  if(!__HAL_RCC_HPDMA3_IS_CLK_ENABLED())
  {
    __HAL_RCC_HPDMA3_CLK_ENABLE();
  }

  /* HPDMA3 interrupt Init */
    HAL_NVIC_SetPriority(HPDMA3_Channel12_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(HPDMA3_Channel12_IRQn);

  /* USER CODE BEGIN DMA_Decrement_GatherOperation 1 */

  /* USER CODE END DMA_Decrement_GatherOperation 1 */
  handle_HPDMA3_Channel12.Instance = HPDMA3_Channel12;
  handle_HPDMA3_Channel12.Init.Request = DMA_REQUEST_SW;
  handle_HPDMA3_Channel12.Init.BlkHWRequest = DMA_BREQ_SINGLE_BURST;
  handle_HPDMA3_Channel12.Init.Direction = DMA_MEMORY_TO_MEMORY;
  handle_HPDMA3_Channel12.Init.SrcInc = DMA_SINC_INCREMENTED;
  handle_HPDMA3_Channel12.Init.DestInc = DMA_DINC_INCREMENTED;
  handle_HPDMA3_Channel12.Init.SrcDataWidth = DMA_SRC_DATAWIDTH_WORD;
  handle_HPDMA3_Channel12.Init.DestDataWidth = DMA_DEST_DATAWIDTH_WORD;
  handle_HPDMA3_Channel12.Init.Priority = DMA_LOW_PRIORITY_HIGH_WEIGHT;
  handle_HPDMA3_Channel12.Init.SrcBurstLength = 1;
  handle_HPDMA3_Channel12.Init.DestBurstLength = 1;
  handle_HPDMA3_Channel12.Init.TransferAllocatedPort = DMA_SRC_ALLOCATED_PORT0|DMA_DEST_ALLOCATED_PORT1;
  handle_HPDMA3_Channel12.Init.Mode = DMA_NORMAL;
  if (HAL_DMA_Init(&handle_HPDMA3_Channel12) != HAL_OK)
  {
    Error_Handler();
  }
  RepeatBlockConfig.RepeatCount = GATHER_BLOCK_NUM;
  RepeatBlockConfig.SrcAddrOffset = 0;
  RepeatBlockConfig.DestAddrOffset = 0;
  RepeatBlockConfig.BlkSrcAddrOffset = GATHER_BLOCK_OFFSET;
  RepeatBlockConfig.BlkDestAddrOffset = (-(GATHER_BLOCK_SIZE * 2U));
  if (HAL_DMAEx_ConfigRepeatBlock(&handle_HPDMA3_Channel12, &RepeatBlockConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /* Select Callbacks functions called after Half Transfer Complete, Transfer Complete and Transfer Error */
  HAL_DMA_RegisterCallback(&handle_HPDMA3_Channel12, HAL_DMA_XFER_CPLT_CB_ID, TransferComplete);
  HAL_DMA_RegisterCallback(&handle_HPDMA3_Channel12, HAL_DMA_XFER_ERROR_CB_ID, TransferError);

  /* Configure the source, destination and buffer size DMA fields and Start DMA Channel/Stream transfer */
  /* Enable All the DMA interrupts */
  if (HAL_DMA_Start_IT(&handle_HPDMA3_Channel12, (uint32_t)&aSRC_Const_Buffer, (uint32_t)&aDST_DecrementGather_Buffer[12], GATHER_BLOCK_SIZE) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN DMA_Decrement_GatherOperation 2 */

  /* USER CODE END DMA_Decrement_GatherOperation 2 */

}

/**
  * @brief  Decrement scatter DMA transmit.
  * @param  None
  * @retval None
  */
static void DMA_Decrement_ScatterOperation(void)
{

  /* USER CODE BEGIN DMA_Decrement_ScatterOperation 0 */

  /* USER CODE END DMA_Decrement_ScatterOperation 0 */

  DMA_RepeatBlockConfTypeDef RepeatBlockConfig = {0};

  /* Peripheral clock enable */
  if(!__HAL_RCC_HPDMA3_IS_CLK_ENABLED())
  {
    __HAL_RCC_HPDMA3_CLK_ENABLE();
  }

  /* HPDMA3 interrupt Init */
    HAL_NVIC_SetPriority(HPDMA3_Channel12_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(HPDMA3_Channel12_IRQn);

  /* USER CODE BEGIN DMA_Decrement_ScatterOperation 1 */

  /* USER CODE END DMA_Decrement_ScatterOperation 1 */
  handle_HPDMA3_Channel12.Instance = HPDMA3_Channel12;
  handle_HPDMA3_Channel12.Init.Request = DMA_REQUEST_SW;
  handle_HPDMA3_Channel12.Init.BlkHWRequest = DMA_BREQ_SINGLE_BURST;
  handle_HPDMA3_Channel12.Init.Direction = DMA_MEMORY_TO_MEMORY;
  handle_HPDMA3_Channel12.Init.SrcInc = DMA_SINC_INCREMENTED;
  handle_HPDMA3_Channel12.Init.DestInc = DMA_DINC_INCREMENTED;
  handle_HPDMA3_Channel12.Init.SrcDataWidth = DMA_SRC_DATAWIDTH_WORD;
  handle_HPDMA3_Channel12.Init.DestDataWidth = DMA_DEST_DATAWIDTH_WORD;
  handle_HPDMA3_Channel12.Init.Priority = DMA_LOW_PRIORITY_HIGH_WEIGHT;
  handle_HPDMA3_Channel12.Init.SrcBurstLength = 1;
  handle_HPDMA3_Channel12.Init.DestBurstLength = 1;
  handle_HPDMA3_Channel12.Init.TransferAllocatedPort = DMA_SRC_ALLOCATED_PORT0|DMA_DEST_ALLOCATED_PORT1;
  handle_HPDMA3_Channel12.Init.Mode = DMA_NORMAL;
  if (HAL_DMA_Init(&handle_HPDMA3_Channel12) != HAL_OK)
  {
    Error_Handler();
  }
  RepeatBlockConfig.RepeatCount = SCATTER_BLOCK_NUM;
  RepeatBlockConfig.SrcAddrOffset = 0;
  RepeatBlockConfig.DestAddrOffset = 0;
  RepeatBlockConfig.BlkSrcAddrOffset = 0;
  RepeatBlockConfig.BlkDestAddrOffset = (-(SCATTER_BLOCK_SIZE * 4U));
  if (HAL_DMAEx_ConfigRepeatBlock(&handle_HPDMA3_Channel12, &RepeatBlockConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /* Select Callbacks functions called after Half Transfer Complete, Transfer Complete and Transfer Error */
  HAL_DMA_RegisterCallback(&handle_HPDMA3_Channel12, HAL_DMA_XFER_CPLT_CB_ID, TransferComplete);
  HAL_DMA_RegisterCallback(&handle_HPDMA3_Channel12, HAL_DMA_XFER_ERROR_CB_ID, TransferError);

  /* Configure the source, destination and buffer size DMA fields and Start DMA Channel/Stream transfer */
  /* Enable All the DMA interrupts */
  if (HAL_DMA_Start_IT(&handle_HPDMA3_Channel12, (uint32_t)&aDST_IncrementGather_Buffer, (uint32_t)&aDST_DecrementScatter_Buffer[36], SCATTER_BLOCK_SIZE) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN DMA_Decrement_ScatterOperation 2 */

  /* USER CODE END DMA_Decrement_ScatterOperation 2 */

}

/**
  * @brief  DMA transfer complete callback
  * @note   This function is executed when the transfer complete interrupt
  *         is generated
  * @retval None
  */
static void TransferComplete(DMA_HandleTypeDef *hdma)
{
  TransferCompleteDetected = 1U;
}

/**
  * @brief  DMA transfer error callback
  * @note   This function is executed when the transfer error interrupt
  *         is generated during DMA transfer
  * @retval None
  */
static void TransferError(DMA_HandleTypeDef *hdma)
{
  TransferErrorDetected = 1U;
}

/**
  * @brief  Compares two buffers.
  * @param  pBuffer1, pBuffer2: buffers to be compared.
  * @param  BufferLength: buffer's length
  * @retval 0   : pBuffer1 identical to pBuffer2
  *         > 0 : pBuffer1 differs from pBuffer2
  */
static uint16_t Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength)
{
  while (BufferLength--)
  {
    if ((*pBuffer1) != *pBuffer2)
    {
      return BufferLength;
    }

    pBuffer1++;
    pBuffer2++;
  }

  return 0;
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
  /* Deinitialize the HPDMA peripheral */
  MX_HPDMA3_DeInit();

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
  /* Turn LED3 on */
  BSP_LED_On(LED3);
  /* User can add his own implementation to report the HAL error return state */
  while (1)
  {
    HAL_Delay(1000);
  }
  /* USER CODE BEGIN Error_Handler_Debug */

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

/**
  * @}
  */

/**
  * @}
  */
