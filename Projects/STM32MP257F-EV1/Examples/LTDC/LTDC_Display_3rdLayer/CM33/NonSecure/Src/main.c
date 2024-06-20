/**
  ******************************************************************************
  * @file    main.c
  * @author  MCD Application Team
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
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
#include <stdio.h>
#include <string.h>

#if defined(FULLSCREEN)
#include "stlogoBIG.h"
#else
#include "stlogoSMALL.h"
#endif

extern uint32_t __extflash_region_init_base;
extern LTDC_HandleTypeDef   hLtdcHandler;

#ifdef __TERMINAL_IO__
extern void initialise_monitor_handles( void );
#endif

/* Private includes ----------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#if defined(DATA_CACHE_ENABLE) && (DATA_CACHE_ENABLE == 1U)
DCACHE_HandleTypeDef hdcache;
#endif

IPCC_HandleTypeDef   hipcc;

size_t size1 = LCD_WIDTH*LCD_HEIGHT*2;

/* Private function prototypes -----------------------------------------------*/
static void LTDC_Config(void);
static void MX_IPCC_Init(void);
#if defined(DATA_CACHE_ENABLE) && (DATA_CACHE_ENABLE == 1U)
static void MX_DCACHE_Init(void);
#endif

void SystemClock_Config(void);
void Display_DemoDescription(void);
void Display_TimeoutStart(void);

/**
  * @brief  Main program
  * @param  None
  * @retval int
  */
int main(void)
{
  /* STM32MP2xx HAL library initialization:
     - Configure the Systick to generate an interrupt each 1 msec
     - Set NVIC Group Priority to 3
     - Low Level Initialization
  */

#if defined(DEBUG)
  volatile int debug = 1;
  while(debug);
#endif

  /* Configure the Data CACHE */
#if defined(DATA_CACHE_ENABLE) && (DATA_CACHE_ENABLE == 1U)
  MX_DCACHE_Init();
#endif
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  if (IS_DEVELOPER_BOOT_MODE())
  {
	  SystemClock_Config();
  }

  /* Initialize the BSP LED */
  BSP_LED_Init(LED3);

  if(!IS_DEVELOPER_BOOT_MODE())
  {
    /* IPCC initialization */
    MX_IPCC_Init();
    /*Corpo Sync Initialization*/
    CoproSync_Init();
  }

  /* Configure the LTDC */
  LTDC_Config();


  /* Initialize the LCD utility layer */
  UTIL_LCD_SetFuncDriver(&LCD_Driver);
  UTIL_LCD_SetLayer(LTDC_LAYER_3);

  Display_DemoDescription();

  Display_TimeoutStart();

  /* Disable the LTDC 3rd Layer*/
  LTDC_LAYER(&hLtdcHandler, LTDC_LAYER_3)->CR &= ~LTDC_LxCR_LEN;

  /* Clear the FrameBuffer Address */
  LTDC_LAYER(&hLtdcHandler, LTDC_LAYER_3)->CFBAR = 0x0;

  /* Set the Immediate Reload type */
  LTDC_LAYER(&hLtdcHandler, LTDC_LAYER_3)->RCR = LTDC_LxRCR_IMR;

  /*wait for 120ms*/
  HAL_Delay(120);


  while(1)
  {



  };

}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
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
  * @brief Display Timeout function
  * @param None
  * @retval None
  */
void Display_TimeoutStart(void)
{
  uint32_t y_size;
  char str[3];
  size_t size = sizeof(str);
  uint32_t layerTimout = 20;
#if defined(FULLSCREEN)
  uint16_t timoutX = 130;
#else
  uint16_t timoutX = 120;
#endif

  /* Get LCD width */
  BSP_LCD_GetYSize(0, &y_size);
#if defined(FULLSCREEN)
  UTIL_LCD_SetFont(&Font24);
#else
  UTIL_LCD_SetFont(&Font16);
#endif
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_BLUE);
  UTIL_LCD_DisplayStringAt(0, y_size / 2, (uint8_t *)"CM33 Controlled LTDC 3rd Layer is being Displayed", CENTER_MODE);
  UTIL_LCD_DisplayStringAt(0, y_size / 2 + 30, (uint8_t *)"Layer will be disappeared after 20 sec : ", CENTER_MODE);
#if defined(FULLSCREEN)
  UTIL_LCD_SetFont(&Font20);
#else
  UTIL_LCD_SetFont(&Font12);
#endif

  sprintf((char *)str, "%ld", layerTimout);
  UTIL_LCD_DisplayStringAt(timoutX, y_size / 2 + 30, (uint8_t *)str, RIGHT_MODE);
  do
  {
	  /* Delay of 1 second */
	  HAL_Delay(1000);
	  layerTimout--;

	  /* Display Timeout Seconds */
	  memset(str, 0, size);
	  UTIL_LCD_DisplayStringAt(timoutX, y_size / 2 + 30, (uint8_t *)"   ", RIGHT_MODE);
	  sprintf((char *)str, "%ld", layerTimout);
	  UTIL_LCD_DisplayStringAt(timoutX, y_size / 2 + 30, (uint8_t *)str, RIGHT_MODE);

  } while (layerTimout != 0);

  HAL_Delay(1000);

}

/**
  * @brief Display Demo Description function
  * @param None
  * @retval None
  */
void Display_DemoDescription(void)
{

  uint32_t x_size;
  uint32_t y_size;
  uint16_t logo_x;

  /* Get LCD width and Height */
  BSP_LCD_GetXSize(0, &x_size);
  BSP_LCD_GetYSize(0, &y_size);

  /* Initialize the LCD */
#if defined(FULLSCREEN)
  UTIL_LCD_SetFont(&Font24);
#else
  UTIL_LCD_SetFont(&Font20);
#endif
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);


  /* Set the LCD Text Color */
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_DARKBLUE);

  /* Display LCD messages */
  UTIL_LCD_DisplayStringAt(0, 10, (uint8_t *)"STM32MP257F-EV Display Demo", CENTER_MODE);
  UTIL_LCD_DisplayStringAt(0, 35, (uint8_t *)"Driver's Example", CENTER_MODE);

#if defined(FULLSCREEN)
  logo_x = (x_size - 240)/2;
#else
  logo_x = (x_size - 180)/2;
#endif

  /* Draw Bitmap */
  UTIL_LCD_DrawBitmap( logo_x , 70, (uint8_t *)stlogo);

#if defined(FULLSCREEN)
  UTIL_LCD_SetFont(&Font24);
#else
  UTIL_LCD_SetFont(&Font16);
#endif
  UTIL_LCD_DisplayStringAt(0, y_size - 20, (uint8_t *)"Copyright (c) STMicroelectronics 2024", CENTER_MODE);

#if defined(FULLSCREEN)
  UTIL_LCD_SetFont(&Font24);
#else
  UTIL_LCD_SetFont(&Font16);
#endif
  UTIL_LCD_FillRect(0, y_size / 2 - 15, x_size, 100, UTIL_LCD_COLOR_BLUE);


}

/**
  * @brief LTDC Configuration Function
  * @param None
  * @retval None
  */
static void LTDC_Config(void)
{

  hLtdcHandler.Instance = LTDC;

  /*LTDC init and configuration*/
   LTDC_LayerCfgTypeDef pLayerCfg;

   int32_t width = LCD_WIDTH;
   int32_t height = LCD_HEIGHT;

   memset(&pLayerCfg, 0, sizeof(LTDC_LayerCfgTypeDef));

   BSP_LCD_GetLTDCPixelFormat(0, LCD_PIXEL_FORMAT_RGB565, &pLayerCfg.PixelFormat);

   if(ResMgr_Request(RESMGR_RESOURCE_RIFSC,RESMGR_RIFSC_LTDC_L2_ID) != RESMGR_STATUS_ACCESS_OK)
   {
	   Error_Handler();
   }
   BSP_LCD_SelectLayer(0, LTDC_LAYER_3);

   /* Layer 3 Init */
#if defined(FULLSCREEN)
   pLayerCfg.WindowX0 = 0;
   pLayerCfg.WindowY0 = 0;
   pLayerCfg.WindowX1 =  width;
   pLayerCfg.WindowY1 = height;
#else
   pLayerCfg.WindowX0 = 152;
   pLayerCfg.WindowY0 = 60;
   pLayerCfg.WindowX1 = pLayerCfg.WindowX0 +  width;
   pLayerCfg.WindowY1 = pLayerCfg.WindowY0 + height;
#endif
   pLayerCfg.Alpha = 0xf0; /* Sets the Opacity of Layer */
   pLayerCfg.Alpha0 = 0;
   pLayerCfg.Backcolor.Blue = 0xff;
   pLayerCfg.Backcolor.Green = 0xff;
   pLayerCfg.Backcolor.Red = 0xff;
   pLayerCfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;
   pLayerCfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;
   pLayerCfg.ImageWidth = width;
   pLayerCfg.ImageHeight = height;
   pLayerCfg.HorMirrorEn = 0;
   pLayerCfg.VertMirrorEn = 0;
   pLayerCfg.FBStartAdress = (uint32_t)LCD_FB_START_ADDRESS;

   HAL_LTDC_ConfigLayer_NoReload(&hLtdcHandler, &pLayerCfg, LTDC_LAYER_3);

   /* Reset the FrameBuffer with WHITE COLOR pixels*/
   memset(LCD_FB_START_ADDRESS, 0xff, size1);

   /* Set the Immediate Reload type */
   LTDC_LAYER(&hLtdcHandler, LTDC_LAYER_3)->RCR = LTDC_LxRCR_IMR;

}

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
#endif


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

  /* Disable the LTDC 3rd Layer*/
  LTDC_LAYER(&hLtdcHandler, LTDC_LAYER_3)->CR &= ~LTDC_LxCR_LEN;

  /* Clear the FrameBuffer Address */
  LTDC_LAYER(&hLtdcHandler, LTDC_LAYER_3)->CFBAR = 0x0;

  /* Set the Immediate Reload type */
  LTDC_LAYER(&hLtdcHandler, LTDC_LAYER_3)->RCR = LTDC_LxRCR_IMR;

#if defined(DATA_CACHE_ENABLE) && (DATA_CACHE_ENABLE == 1U)
  HAL_DCACHE_Invalidate(&hdcache);
  HAL_DCACHE_DeInit(&hdcache);
#endif

  /* When ready, notify the remote processor that we can be shut down */
  HAL_IPCC_NotifyCPU(hipcc, ChannelIndex, IPCC_CHANNEL_DIR_RX);

  /* Wait for complete shutdown */
  while(1);
}


/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
  /* Turn LED3 on */
  BSP_LED_On(LED3);
  while (1)
  {
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
  /* Prevent unused argument(s) compilation warning */
  UNUSED(file);
  UNUSED(line);

  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif /* USE_FULL_ASSERT */
