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
#include <stdio.h>

#include "main.h"
#include "app_freertos.h"
#include "usbpd.h"
#include "cmsis_os2.h"
#include "stm32mp257f_eval_usbpd_pwr.h"

/* Private includes ----------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
COM_InitTypeDef COM_Init;
IPCC_HandleTypeDef hipcc;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void ResMgr_LL_RCC_HPDMA3_EnableClock();
static void MX_IPCC_Init(void);
static void MX_HPDMA3_Init(void);
static void MX_UCPD1_Init(void);

/**
  * @brief  Main program
  * @param  None
  * @retval int
  */
int main(void)
{

  if(IS_DEVELOPER_BOOT_MODE())
  {
    SystemClock_Config();
  }
  else
  {
    SystemCoreClockUpdate();
  }

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Initialize all configured peripherals */
  MX_HPDMA3_Init();

  /* Initialize Display destination */
#if defined(__VALID_OUTPUT_TERMINAL_IO__) && defined (__GNUC__)
  initialise_monitor_handles();
#else
  if(ResMgr_Request(RESMGR_RESOURCE_RIFSC, COM_CM33_RIF_RES_NUM_UART) == RESMGR_STATUS_ACCESS_OK)
  {
    COM_Init.BaudRate                = 115200;
    COM_Init.WordLength              = UART_WORDLENGTH_8B;
    COM_Init.StopBits                = UART_STOPBITS_1;
    COM_Init.Parity                  = UART_PARITY_NONE;
    COM_Init.HwFlowCtl               = UART_HWCONTROL_NONE;
    /* Initialize and select COM1 which is the COM port associated with current Core */
    BSP_COM_Init(COM_VCP_CM33, &COM_Init);
    BSP_COM_SelectLogPort(COM_VCP_CM33);
  }
#endif

  loc_printf("\n\n Starting UCSI project(%s: %s) \r\n", __DATE__, __TIME__);

  if(!IS_DEVELOPER_BOOT_MODE())
  {
     /* IPCC initialization */
     MX_IPCC_Init();
  }
  MAILBOX_SCMI_Init();

  MX_UCPD1_Init();
  /* Call PreOsInit function */
  USBPD_PreInitOs();
  /* UCPD1 interrupt Init */
  NVIC_SetPriority(UCPD1_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),5, 0));
  NVIC_EnableIRQ(UCPD1_IRQn);

  /* Initialize LEDs */
  BSP_LED_Init(LED_ORANGE);
  BSP_LED_On(LED_ORANGE);

  /* Start freeRTOS */
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
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  /* PLL 1 is not to be configured here because M33 only
     application or already configured by an A35 application */
  /* PLL 2 is configured by DDR initialization code */
  /* PLL 3 is configured by GPU initialization code */
  RCC_PLLInitTypeDef RCC_Pll4InitStruct = {0};
  RCC_PLLInitTypeDef RCC_Pll5InitStruct = {0};
  RCC_PLLInitTypeDef RCC_Pll6InitStruct = {0};
  RCC_PLLInitTypeDef RCC_Pll7InitStruct = {0};
  RCC_PLLInitTypeDef RCC_Pll8InitStruct = {0};

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_HSE |
                                     RCC_OSCILLATORTYPE_LSE | RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_OFF;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_OFF;
  RCC_OscInitStruct.LSEDriveValue = RCC_LSEDRIVE_MEDIUMHIGH;

  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /* PLL 1 is not to be configured here because M33 only
     application or already configured by an A35 application */
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

  RCC_Pll5InitStruct.PLLState = RCC_PLL_OFF;
  RCC_Pll6InitStruct.PLLState = RCC_PLL_OFF;
  RCC_Pll7InitStruct.PLLState = RCC_PLL_OFF;
  RCC_Pll8InitStruct.PLLState = RCC_PLL_OFF;

  /* PLL 1 is not to be configured here because M33 only
     application or already configured by an A35 application */
  /* PLL 2 is configured by DDR initialization code */
  /* PLL 3 is configured by GPU initialization code */

  if (HAL_RCCEx_PLL4Config(&RCC_Pll4InitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_RCCEx_PLL5Config(&RCC_Pll5InitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_RCCEx_PLL6Config(&RCC_Pll6InitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_RCCEx_PLL7Config(&RCC_Pll7InitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_RCCEx_PLL8Config(&RCC_Pll8InitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_ICN_HS_MCU  | RCC_CLOCKTYPE_ICN_LS_MCU |
                                RCC_CLOCKTYPE_ICN_SDMMC   | RCC_CLOCKTYPE_ICN_DDR    |
                                RCC_CLOCKTYPE_ICN_DISPLAY | RCC_CLOCKTYPE_ICN_HCL    |
                                RCC_CLOCKTYPE_ICN_NIC     | RCC_CLOCKTYPE_ICN_VID    |
                                RCC_CLOCKTYPE_ICN_APB1    | RCC_CLOCKTYPE_ICN_APB2   |
                                RCC_CLOCKTYPE_ICN_APB3    | RCC_CLOCKTYPE_ICN_APB4   |
                                RCC_CLOCKTYPE_ICN_APBDBG;

  RCC_ClkInitStruct.ICN_HS_MCU.XBAR_ClkSrc  = RCC_XBAR_CLKSRC_PLL4;
  RCC_ClkInitStruct.ICN_HS_MCU.Div          = 3;
  RCC_ClkInitStruct.ICN_SDMMC.XBAR_ClkSrc   = RCC_XBAR_CLKSRC_PLL4;
  RCC_ClkInitStruct.ICN_SDMMC.Div           = 6;
  RCC_ClkInitStruct.ICN_DDR.XBAR_ClkSrc     = RCC_XBAR_CLKSRC_PLL4;
  RCC_ClkInitStruct.ICN_DDR.Div             = 2;
  RCC_ClkInitStruct.ICN_DISPLAY.XBAR_ClkSrc = RCC_XBAR_CLKSRC_PLL4;
  RCC_ClkInitStruct.ICN_DISPLAY.Div         = 3;
  RCC_ClkInitStruct.ICN_HCL.XBAR_ClkSrc     = RCC_XBAR_CLKSRC_PLL4;
  RCC_ClkInitStruct.ICN_HCL.Div             = 4;
  RCC_ClkInitStruct.ICN_NIC.XBAR_ClkSrc     = RCC_XBAR_CLKSRC_PLL4;
  RCC_ClkInitStruct.ICN_NIC.Div             = 3;
  RCC_ClkInitStruct.ICN_VID.XBAR_ClkSrc     = RCC_XBAR_CLKSRC_PLL4;
  RCC_ClkInitStruct.ICN_VID.Div             = 2;
  RCC_ClkInitStruct.ICN_LSMCU_Div           = RCC_LSMCU_DIV2;
  RCC_ClkInitStruct.APB1_Div                = RCC_APB1_DIV1;
  RCC_ClkInitStruct.APB2_Div                = RCC_APB2_DIV1;
  RCC_ClkInitStruct.APB3_Div                = RCC_APB3_DIV1;
  RCC_ClkInitStruct.APB4_Div                = RCC_APB4_DIV1;
  RCC_ClkInitStruct.APBDBG_Div              = RCC_APBDBG_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, 0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief HPDMA3 Deinitialization Function
  * @param None
  * @retval None
  */
void MX_HPDMA3_Deinit(void)
{
  /* HPDMA3 interrupt Init */
  NVIC_DisableIRQ(HPDMA3_Channel0_IRQn);
  NVIC_DisableIRQ(HPDMA3_Channel2_IRQn);
  NVIC_DisableIRQ(HPDMA3_Channel3_IRQn);
}

/**
  * @brief HPDMA3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_HPDMA3_Init(void)
{
  /* Peripheral clock enable */
  ResMgr_LL_RCC_HPDMA3_EnableClock();

  /* HPDMA3 interrupt Init */
  NVIC_SetPriority(HPDMA3_Channel0_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),5, 0));
  NVIC_EnableIRQ(HPDMA3_Channel0_IRQn);
  NVIC_SetPriority(HPDMA3_Channel2_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),5, 0));
  NVIC_EnableIRQ(HPDMA3_Channel2_IRQn);
  NVIC_SetPriority(HPDMA3_Channel3_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),5, 0));
  NVIC_EnableIRQ(HPDMA3_Channel3_IRQn);
}

/**
  * @brief UCPD1 Deinitialization Function
  * @param None
  * @retval None
  */
void MX_UCPD1_Deinit(void)
{
  int ret = 0;

  NVIC_DisableIRQ(UCPD1_IRQn);

  LL_DMA_DeInit(HPDMA3, LL_DMA_CHANNEL_0);
  LL_DMA_DeInit(HPDMA3, LL_DMA_CHANNEL_2);
  LL_DMA_DeInit(HPDMA3, LL_DMA_CHANNEL_3);
  ResMgr_Release(RESMGR_RESOURCE_RIF_RCC, RESMGR_RCC_RESOURCE(35));
  ResMgr_Release(RESMGR_RESOURCE_RIF_PWR, RESMGR_PWR_RESOURCE(0));

  /* Disable UCPD regulator */
  if(ResMgr_Request(RESMGR_RESOURCE_RIF_PWR, RESMGR_PWR_RESOURCE(0)) == RESMGR_STATUS_ACCESS_OK)
  {
    PWR->CR1 |= PWR_CR1_UCPDSV; /* Disconnect UCPD from supply */
  }
  else
  {
    ret = scmi_voltage_domain_disable(&scmi_channel, VOLTD_SCMI_UCPD);
    if (ret)
      loc_printf("Failed to disable VOLTD_SCMI_UCPD (error code %d)\r\n", ret);
  }

  /* Peripheral clock disable */
  LL_RCC_UCPD1_DisableClock();
}

/**
  * @brief UCPD1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UCPD1_Init(void)
{
  int ret = 0;

  LL_DMA_InitTypeDef DMA_InitStruct = {0};

  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /* Peripheral clock enable */
  LL_RCC_UCPD1_EnableClock();

  /* Enable UCPD regulator */
  if(ResMgr_Request(RESMGR_RESOURCE_RIF_PWR, RESMGR_PWR_RESOURCE(0)) == RESMGR_STATUS_ACCESS_OK)
  {
    loc_printf("Enable UCPD regulator directly\r\n");
    PWR->CR1 |= PWR_CR1_UCPDVMEN;                            /* UCPD voltage monitor enable */
    while ((PWR->CR1 & PWR_CR1_UCPDRDY) != PWR_CR1_UCPDRDY); /* Wait for UCPD ready */
    PWR->CR1 |= PWR_CR1_UCPDSV;                              /* Connect UCPD to supply */
    PWR->CR1 &= ~PWR_CR1_UCPDVMEN;                           /* UCPD voltage monitor disable */
  }
  else
  {
    loc_printf("Enable UCPD regulator with SCMI\r\n");
    ret = scmi_voltage_domain_enable(&scmi_channel, VOLTD_SCMI_UCPD);
    if (ret)
      loc_printf("Failed to enable VOLTD_SCMI_UCPD (error code %d)\r\n", ret);
  }

  if(ResMgr_Request(RESMGR_RESOURCE_RIF_RCC, RESMGR_RCC_RESOURCE(35)) == RESMGR_STATUS_ACCESS_OK) {
	  /* Set clock source for UCPD */
	  PeriphClkInit.XBAR_Channel = RCC_PERIPHCLK_USBTC;
	  PeriphClkInit.XBAR_ClkSrc = RCC_XBAR_CLKSRC_HSI;
	  PeriphClkInit.Div = 4; /* 64MHz / 4 = 16MHz */
	  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
	  {
		while (1);
	  }
  }

  /* UCPD1 DMA Init */

  /* HPDMA3_REQUEST_UCPD1_RX Init */
  DMA_InitStruct.SrcAddress = 0x00000000U;
  DMA_InitStruct.DestAddress = 0x00000000U;
  DMA_InitStruct.Direction = LL_DMA_DIRECTION_PERIPH_TO_MEMORY;
  DMA_InitStruct.BlkHWRequest = LL_DMA_HWREQUEST_SINGLEBURST;
  DMA_InitStruct.DataAlignment = LL_DMA_DATA_ALIGN_ZEROPADD;
  DMA_InitStruct.SrcBurstLength = 1;
  DMA_InitStruct.DestBurstLength = 1;
  DMA_InitStruct.SrcDataWidth = LL_DMA_SRC_DATAWIDTH_BYTE;
  DMA_InitStruct.DestDataWidth = LL_DMA_DEST_DATAWIDTH_BYTE;
  DMA_InitStruct.SrcIncMode = LL_DMA_SRC_FIXED;
  DMA_InitStruct.DestIncMode = LL_DMA_DEST_INCREMENT;
  DMA_InitStruct.Priority = LL_DMA_LOW_PRIORITY_LOW_WEIGHT;
  DMA_InitStruct.BlkDataLength = 0x00000000U;
  DMA_InitStruct.TriggerMode = LL_DMA_TRIGM_BLK_TRANSFER;
  DMA_InitStruct.TriggerPolarity = LL_DMA_TRIG_POLARITY_MASKED;
  DMA_InitStruct.TriggerSelection = 0x00000000U;
  DMA_InitStruct.Request = LL_HPDMA_REQUEST_UCPD_RX;
  DMA_InitStruct.TransferEventMode = LL_DMA_TCEM_BLK_TRANSFER;
  DMA_InitStruct.SrcAllocatedPort = LL_DMA_SRC_ALLOCATED_PORT0;
  DMA_InitStruct.DestAllocatedPort = LL_DMA_DEST_ALLOCATED_PORT0;
  DMA_InitStruct.LinkAllocatedPort = LL_DMA_LINK_ALLOCATED_PORT1;
  DMA_InitStruct.LinkStepMode = LL_DMA_LSM_FULL_EXECUTION;
  DMA_InitStruct.LinkedListBaseAddr = 0x00000000U;
  DMA_InitStruct.LinkedListAddrOffset = 0x00000000U;
  LL_DMA_Init(HPDMA3, LL_DMA_CHANNEL_3, &DMA_InitStruct);

  /* HPDMA3_REQUEST_UCPD1_TX Init */
  DMA_InitStruct.SrcAddress = 0x00000000U;
  DMA_InitStruct.DestAddress = 0x00000000U;
  DMA_InitStruct.Direction = LL_DMA_DIRECTION_MEMORY_TO_PERIPH;
  DMA_InitStruct.BlkHWRequest = LL_DMA_HWREQUEST_SINGLEBURST;
  DMA_InitStruct.DataAlignment = LL_DMA_DATA_ALIGN_ZEROPADD;
  DMA_InitStruct.SrcBurstLength = 1;
  DMA_InitStruct.DestBurstLength = 1;
  DMA_InitStruct.SrcDataWidth = LL_DMA_SRC_DATAWIDTH_BYTE;
  DMA_InitStruct.DestDataWidth = LL_DMA_DEST_DATAWIDTH_BYTE;
  DMA_InitStruct.SrcIncMode = LL_DMA_SRC_INCREMENT;
  DMA_InitStruct.DestIncMode = LL_DMA_DEST_FIXED;
  DMA_InitStruct.Priority = LL_DMA_LOW_PRIORITY_LOW_WEIGHT;
  DMA_InitStruct.BlkDataLength = 0x00000000U;
  DMA_InitStruct.TriggerMode = LL_DMA_TRIGM_BLK_TRANSFER;
  DMA_InitStruct.TriggerPolarity = LL_DMA_TRIG_POLARITY_MASKED;
  DMA_InitStruct.TriggerSelection = 0x00000000U;
  DMA_InitStruct.Request = LL_HPDMA_REQUEST_UCPD_TX;
  DMA_InitStruct.TransferEventMode = LL_DMA_TCEM_BLK_TRANSFER;
  DMA_InitStruct.SrcAllocatedPort = LL_DMA_SRC_ALLOCATED_PORT0;
  DMA_InitStruct.DestAllocatedPort = LL_DMA_DEST_ALLOCATED_PORT0;
  DMA_InitStruct.LinkAllocatedPort = LL_DMA_LINK_ALLOCATED_PORT1;
  DMA_InitStruct.LinkStepMode = LL_DMA_LSM_FULL_EXECUTION;
  DMA_InitStruct.LinkedListBaseAddr = 0x00000000U;
  DMA_InitStruct.LinkedListAddrOffset = 0x00000000U;
  LL_DMA_Init(HPDMA3, LL_DMA_CHANNEL_2, &DMA_InitStruct);

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
    USBPD_DPM_TimerCounter();
  }
}

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

void MX_IPCC_DeInit(void)
{
  /* IPCC interrupt DeInit */
  HAL_NVIC_DisableIRQ(IPCC1_RX_IRQn);

  hipcc.Instance = IPCC1;
  if (HAL_IPCC_DeInit(&hipcc) != HAL_OK)
  {
     Error_Handler();
  }
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  __disable_irq();
  BSP_LED_Off(LED_ORANGE);
  while (1)
  {
    BSP_LED_Toggle(LED_ORANGE);
    HAL_Delay(100);
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
