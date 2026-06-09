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
#include "cmsis_os2.h"
#include "app_freertos.h"
#include "copro_sync.h"

/* Private includes ----------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
TIM_HandleTypeDef *htim ;
/* Private define ------------------------------------------------------------*/
#ifdef ICACHE_DCACHE_USE
#define ICACHE_DCACHE_ENABLE
#endif
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
IPCC_HandleTypeDef  hipcc1;

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void MX_IPCC_Init(void);
#ifdef ICACHE_DCACHE_ENABLE
static void MX_ICACHE_Init(void);
static void MX_DCACHE_Init(void);
static void MPU_Config(void);
DCACHE_HandleTypeDef hdcache = {0};
#endif

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


  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

#ifdef ICACHE_DCACHE_ENABLE
  MPU_Config();
  MX_DCACHE_Init();
  MX_ICACHE_Init();
#endif

   /* Configure the system clock */
  if(IS_DEVELOPER_BOOT_MODE())
  {
	SystemClock_Config();
  }
  else
  {
    SystemCoreClockUpdate();
  }

  /* Initialize LEDs */
  if(BSP_LED_Init(LED3) != BSP_ERROR_NONE)
  {
	  Error_Handler();
  }

  if(!IS_DEVELOPER_BOOT_MODE())
  {
    /* IPCC initialization */
    MX_IPCC_Init();
    /*Corpo Sync Initialization*/
    CoproSync_Init();
  }

  osKernelInitialize();  /* Call init function for freertos objects (in freertos.c) */
  MX_FREERTOS_Init();

  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  while (1)
  {
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
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM6) {
    HAL_IncTick();
  }
}


/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */

/**
  * @brief  Initialize the IPCC Peripheral
  * @retval None
  */
static void MX_IPCC_Init(void)
{

  hipcc1.Instance = IPCC1;
  if (HAL_IPCC_Init(&hipcc1) != HAL_OK)
  {
     Error_Handler();
  }
  /* IPCC interrupt Init */
  HAL_NVIC_SetPriority(IPCC1_RX_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY+ DEFAULT_IRQ_PRIO, 0);
  HAL_NVIC_EnableIRQ(IPCC1_RX_IRQn);
}

#ifdef ICACHE_DCACHE_ENABLE
/**
  * @brief Instruction Cache Initialization Function
  * @param None
  * @retval None
  */
static void MX_ICACHE_Init(void)
{

  if(HAL_ICACHE_DeInit() != HAL_OK)
  {
    Error_Handler();
  }
  ICACHE_RegionConfigTypeDef pRegionConfig = {0};
  pRegionConfig.TrafficRoute    = ICACHE_MASTER2_PORT;
  pRegionConfig.OutputBurstType = ICACHE_OUTPUT_BURST_INCR;
  pRegionConfig.Size            = ICACHE_REGIONSIZE_2MB;
  pRegionConfig.BaseAddress     = 0x00000000;
  pRegionConfig.RemapAddress    = 0x80000000;

  if (HAL_ICACHE_EnableRemapRegion(ICACHE_REGION_0, &pRegionConfig) != HAL_OK)
  {
  	 Error_Handler();
  }

  if (HAL_ICACHE_Enable() != HAL_OK)
  {
	  Error_Handler();
  }
}

/**
  * @brief Data Cache Initialization Function
  * @param None
  * @retval None
  */
static void MX_DCACHE_Init(void)
{

  hdcache.Instance = DCACHE;
  hdcache.Init.ReadBurstType = DCACHE_READ_BURST_WRAP;

  if (HAL_DCACHE_Init(&hdcache) != HAL_OK)
  {
     Error_Handler();
  }

}

/**
  * @brief  Configure the MPU attributes
  * @param  None
  * @retval None
  */
static void MPU_Config(void)
{

   MPU_Region_InitTypeDef MPU_InitStruct;
   MPU_Attributes_InitTypeDef MPU_Attributes_InitStruct ;

   HAL_MPU_Disable();

   /* write back, read and write allocate */
   MPU_Attributes_InitStruct.Attributes = INNER_OUTER(MPU_WRITE_BACK | MPU_NON_TRANSIENT | MPU_RW_ALLOCATE);
   MPU_Attributes_InitStruct.Number = MPU_ATTRIBUTES_NUMBER0;
   HAL_MPU_ConfigMemoryAttributes(&MPU_Attributes_InitStruct);

   /* ICACHE */
   MPU_InitStruct.Enable           = MPU_REGION_ENABLE;
   MPU_InitStruct.Number           = MPU_REGION_NUMBER0;
   MPU_InitStruct.AttributesIndex  = MPU_ATTRIBUTES_NUMBER0;
   MPU_InitStruct.BaseAddress      = 0x00000000;
   MPU_InitStruct.LimitAddress     = 0x00010000;
   MPU_InitStruct.AccessPermission = MPU_REGION_ALL_RO;
   MPU_InitStruct.DisableExec      = MPU_INSTRUCTION_ACCESS_ENABLE;
   MPU_InitStruct.IsShareable      = MPU_ACCESS_NOT_SHAREABLE;
   HAL_MPU_ConfigRegion(&MPU_InitStruct);

   /* DCACHE */
   MPU_InitStruct.AccessPermission = MPU_REGION_ALL_RW;
   MPU_InitStruct.Enable = MPU_REGION_ENABLE;
   MPU_InitStruct.AttributesIndex = MPU_ATTRIBUTES_NUMBER0;
   MPU_InitStruct.DisableExec= MPU_INSTRUCTION_ACCESS_DISABLE;
   MPU_InitStruct.IsShareable = MPU_ACCESS_INNER_SHAREABLE;
   MPU_InitStruct.BaseAddress =  0x80A00000;
   MPU_InitStruct.LimitAddress = 0x80DFFC00;
   MPU_InitStruct.Number = MPU_REGION_NUMBER1;
   HAL_MPU_ConfigRegion(&MPU_InitStruct);

   /* Data section(IPC_SHMEM_1 & VIRTIO_SHMEM)- NON CACHEABLE */
   MPU_Attributes_InitStruct.Attributes = INNER_OUTER(MPU_NOT_CACHEABLE);
   MPU_Attributes_InitStruct.Number = MPU_ATTRIBUTES_NUMBER1;
   HAL_MPU_ConfigMemoryAttributes(&MPU_Attributes_InitStruct);

   MPU_InitStruct.AccessPermission = MPU_REGION_ALL_RW;
   MPU_InitStruct.Enable = MPU_REGION_ENABLE;
   MPU_InitStruct.AttributesIndex = MPU_ATTRIBUTES_NUMBER1;
   MPU_InitStruct.DisableExec= MPU_INSTRUCTION_ACCESS_DISABLE;
   MPU_InitStruct.IsShareable = MPU_ACCESS_INNER_SHAREABLE;
   MPU_InitStruct.BaseAddress =  0x81200000;
   MPU_InitStruct.LimitAddress = 0x812FFFFF;
   MPU_InitStruct.Number = MPU_REGION_NUMBER2;
   HAL_MPU_ConfigRegion(&MPU_InitStruct);

   HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}
#endif

void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  __disable_irq();
  BSP_LED_Off(LED3);

  BSP_LED_On(LED3);

  while (1);
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
