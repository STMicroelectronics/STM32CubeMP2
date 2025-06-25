/**
  ******************************************************************************
  * @file    main.c
  * @author  MCD Application Team
  * @brief   Main program body (non-secure)
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "stdio.h"
#include "cmsis_os2.h"
#include "app_freertos.h"
#include "low_power_display_task.h"
#include "remoteproc_task.h"
#include "stLogo.h"

/** @addtogroup STM32MP2xx_Application
  * @{
  */

/** @addtogroup SPE_NSCubeProjects
  * @{
  */

/* Private variables ---------------------------------------------------------*/
/** @brief Days of the week strings. */
const char* daysOfWeek[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

/* Callbacks prototypes ------------------------------------------------------*/
extern void HAL_I2C_MspPostInit(I2C_HandleTypeDef* hi2c);
/* Global variables ----------------------------------------------------------*/
RTC_HandleTypeDef RtcHandle; /**< RTC handle for timekeeping. */
I2C_HandleTypeDef I2cHandle; /**< I2C handle for communication. */
COM_InitTypeDef COM_Init; /**< COM port initialization structure. */
EXTI_HandleTypeDef hexti; /**< EXTI handle for external interrupts. */
extern osEventFlagsId_t CA35EventFlag; /**< Event flag for CA35 communication. */
extern osMessageQueueId_t display_update_msg_queue; /**< Message queue for display updates. */

#define MASTER_BOARD
#define I2C_ADDRESS        		0x30F
#define BUS_I2Cx_FREQUENCY   	0x1094102CU
/* External function prototypes ----------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/**
  * @brief  Error handler for critical failures.
  * @retval None
  */
void Error_Handler(void);

/**
  * @brief  Initialize I2C8 peripheral.
  * @retval None
  */
static void MX_I2C8_Init(void);

/**
  * @brief  Initialize EXTI2 for interrupt handling.
  * @retval None
  */
static void MX_EXTI2_Init(void);

/**
  * @brief  Callback for EXTI2 falling edge interrupt.
  * @retval None
  */
static void Exti2FallingCb(void);

/**
  * @brief  Initialize the low-power display.
  * @retval None
  */
static void LowPowerDisplayInit(void);

/**
  * @brief  Configure the system clock.
  * @retval None
  */
void SystemClock_Config(void);

/**
  * @brief  Main program entry point.
  * @retval None
  */
int main(void)
{
#ifdef DEBUG
    volatile uint32_t debug = 1;
    while (debug);
#endif

    if (IS_DEVELOPER_BOOT_MODE()) {
        SystemClock_Config();
    } else {
        SystemCoreClockUpdate();
    }

    HAL_Init();

    /* Initialize LED */
    BSP_LED_Init(LED3);
    BSP_LED_On(LED3);

    /* Initialize Display destination */
    if (ResMgr_Request(RESMGR_RESOURCE_RIFSC, RESMGR_RIFSC_UART5_ID) == RESMGR_STATUS_ACCESS_OK) {
        COM_Init.BaudRate = 115200;
        COM_Init.WordLength = UART_WORDLENGTH_8B;
        COM_Init.StopBits = UART_STOPBITS_1;
        COM_Init.Parity = UART_PARITY_NONE;
        COM_Init.HwFlowCtl = UART_HWCONTROL_NONE;
        BSP_COM_Init(COM_VCP_CM33, &COM_Init);
        BSP_COM_SelectLogPort(COM_VCP_CM33);
    }
    printf("[NS] [INF] Non-Secure system starting...\r\n");

    /* Initialize Low Power I2C Display Module */
    MX_I2C8_Init();
    LowPowerDisplayInit();
    ssd1306_Fill(White);
    ssd1306_DrawBitmap(0, 0, stLogo, 128, 64, Black);
    ssd1306_UpdateScreen();

    printf("\033[1;34m[NS] [INF] STM32Cube FW version: v%ld.%ld.%ld-rc%ld \033[0m\r\n",
           ((HAL_GetHalVersion() >> 24) & 0x000000FF),  // Main version
           ((HAL_GetHalVersion() >> 16) & 0x000000FF),  // Sub1 version
           ((HAL_GetHalVersion() >> 8) & 0x000000FF),   // Sub2 version
           (HAL_GetHalVersion() & 0x000000FF));         // RC version

    /* Initialize the RTC handle */
    RtcHandle.Instance = RTC;

    /* Explicit splash screen delay */
    HAL_Delay(2000);

    /* Configure EXTI2 (connected to PG.8 pin) in interrupt mode */
    MX_EXTI2_Init();

    osKernelInitialize();
    MX_FREERTOS_Init();
    osKernelStart();

    /* Infinite loop */
    while (1) {
    }
}

/**
  * @brief  Configure the system clock.
  * @retval None
  */
void SystemClock_Config(void)
{
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
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, 0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief  Error handler for critical failures.
  * @retval None
  */
void Error_Handler(void)
{
    while (1) {
        BSP_LED_Toggle(LED3);
        HAL_Delay(100);
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  static uint32_t rtcTick = 0;
  if (htim->Instance == TIM6) {
  HAL_IncTick();
  if(rtcTick == 60000) // 1 minute = 60 seconds
  {
      DisplayMessage msg = { .type = DISPLAY_MSG_RTC_UPDATE, .data = 0 };
      LowPowerDisplay_PostMsg(&msg);
    rtcTick = 0;
  }else
  {
    rtcTick++;
  }
  }
}

/**
  * @brief  Initialize I2C8 peripheral.
  * @retval None
  */
static void MX_I2C8_Init(void)
{

  /* USER CODE BEGIN I2C8_Init 0 */

  /* USER CODE END I2C8_Init 0 */

  /* USER CODE BEGIN I2C8_Init 1 */

  /* USER CODE END I2C8_Init 1 */


  /* Acquire I2C8 using Resource manager */
  if (RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIFSC, STM32MP25_RIFSC_I2C8_ID))
  {
    Error_Handler();
  }

  /**I2C8 GPIO Configuration
  PZ3      ------> I2C8_SDA
  PZ4      ------> I2C8_SCL
  */

  /* Acquire GPIOZ3 using Resource manager */
  if (RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIF_GPIOZ, RESMGR_GPIO_PIN(3)))
  {
    Error_Handler();
  }
  /* Acquire GPIOZ4 using Resource manager */
  if (RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIF_GPIOZ, RESMGR_GPIO_PIN(4)))
  {
    Error_Handler();
  }

  /* Enable GPIOs power supplies */
  if (RESMGR_STATUS_ACCESS_OK == ResMgr_Request(RESMGR_RESOURCE_RIF_RCC, RESMGR_RCC_RESOURCE(101)))
  {
    __HAL_RCC_GPIOZ_CLK_ENABLE();
  }


  I2cHandle.Instance = I2C8;
  I2cHandle.Init.Timing = BUS_I2Cx_FREQUENCY;
  I2cHandle.Init.OwnAddress1 = 0;
  I2cHandle.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  I2cHandle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  I2cHandle.Init.OwnAddress2 = 0;
  I2cHandle.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  I2cHandle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  I2cHandle.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

  if (HAL_I2C_Init(&I2cHandle) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&I2cHandle, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&I2cHandle, 0) != HAL_OK)
  {
    Error_Handler();
  }

  /** I2C Fast mode Plus enable
  */
  if (HAL_I2CEx_ConfigFastModePlus(&I2cHandle, I2C_FASTMODEPLUS_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }
  HAL_I2C_MspPostInit(&I2cHandle);
}

/**
  * @brief  Initialize the low-power display.
  * @retval None
  */
static void LowPowerDisplayInit(void)
{
    uint8_t init_commands[] = {
        0xAE, // Display OFF
        0x20, 0x00, // Set Memory Addressing Mode, Horizontal Addressing Mode
        0xB0, // Set Page Start Address for Page Addressing Mode
        0xC8, // Set COM Output Scan Direction
        0x00, // Set low column address
        0x10, // Set high column address
        0x40, // Set start line address
        0x81, 0xFF, // Set contrast control, Maximum contrast
        0xA1, // Set segment re-map
        0xA6, // Set normal display
        0xA8, 0x3F, // Set multiplex ratio, 1/64 duty
        0xA4, // Output follows RAM content
        0xD3, 0x00, // Set display offset, No offset
        0xD5, 0xF0, // Set display clock divide ratio/oscillator frequency
        0xD9, 0x22, // Set pre-charge period
        0xDA, 0x12, // Set com pins hardware configuration
        0xDB, 0x20, // Set vcomh, 0.77xVcc
        0x8D, 0x14, // Set DC-DC enable, Enable charge pump
        0xAF // Display ON
    };

    // Send all initialization commands
    for (size_t i = 0; i < sizeof(init_commands); i++) {
         HAL_I2C_Mem_Write(&SSD1306_I2C_PORT, SSD1306_I2C_ADDR, 0x00, 1, &init_commands[i], 1, 1000);
    }

    // Set default values for screen object
    ssd1306_SetCursor(0, 0);
}

/**
  * @brief  Initialize EXTI2 for interrupt handling.
  * @retval None
  */
static void MX_EXTI2_Init(void)
{
  GPIO_InitTypeDef   GPIO_InitStruct;
  EXTI_ConfigTypeDef EXTI_ConfigStructure;

  /* Enable GPIOG clock */
  if (RESMGR_STATUS_ACCESS_OK == ResMgr_Request(RESMGR_RESOURCE_RIF_RCC, RESMGR_RCC_RESOURCE(96)))
  {
	__HAL_RCC_GPIOG_CLK_ENABLE();
  }
  /* Acquire GPIOG8 using Resource manager */
  if (RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIF_GPIOG, RESMGR_GPIO_PIN(8)))
  {
	  Error_Handler();
  }

  /* Configure PG.8 pin as input floating */
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Pin = BUTTON_USER2_PIN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(BUTTON_USER2_GPIO_PORT, &GPIO_InitStruct);

  /* Set configuration except Interrupt and Event mask of Exti line 8*/
  EXTI_ConfigStructure.Line = EXTI2_LINE_8;
  EXTI_ConfigStructure.Trigger = EXTI_TRIGGER_FALLING;
  EXTI_ConfigStructure.GPIOSel = EXTI_GPIOG;
  EXTI_ConfigStructure.Mode = EXTI_MODE_INTERRUPT;
  HAL_EXTI_SetConfigLine(&hexti, &EXTI_ConfigStructure);

  /* Register callback to treat Exti interrupts in user Exti2FallingCb function */
  HAL_EXTI_RegisterCallback(&hexti, HAL_EXTI_FALLING_CB_ID, Exti2FallingCb);

  /* Enable and set line 2 Interrupt to the lowest priority */
  HAL_NVIC_SetPriority(EXTI2_8_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + DEFAULT_IRQ_PRIO, 0);
  HAL_NVIC_EnableIRQ(EXTI2_8_IRQn);
}

/**
  * @brief  Configures EXTI line 8 (connected to PG.8 pin) in interrupt mode
  * @param  None
  * @retval None
  */
 void MX_EXTI2_DeInit(void)
 {
   HAL_GPIO_DeInit(BUTTON_USER2_GPIO_PORT, BUTTON_USER2_PIN);
 
   ResMgr_Release(RESMGR_RESOURCE_RIF_GPIOG, RESMGR_GPIO_PIN(8));
 
   HAL_NVIC_DisableIRQ(EXTI2_8_IRQn);
 }
 
/**
  * @brief  Callback for EXTI2 falling edge interrupt.
  * @retval None
  */
static void Exti2FallingCb(void)
{
	HAL_NVIC_DisableIRQ(EXTI2_8_IRQn);
    RemoteProcCmd cmd = { .action = REMOTEPROC_ACTION_START, .cpu_id = 0 };
    RemoteProc_PostCmd(&cmd);
}

/**
  * @brief  Display the current time.
  * @param  showtime : pointer to buffer
  * @retval None
  */
void RTC_TimeShow(char *showDate, char *showTime)
{
  RTC_DateTypeDef sdatestructureget;
  RTC_TimeTypeDef stimestructureget;

  /* Get the RTC current Time */
  HAL_RTC_GetTime(&RtcHandle, &stimestructureget, RTC_FORMAT_BCD);
  /* Get the RTC current Date */
  HAL_RTC_GetDate(&RtcHandle, &sdatestructureget, RTC_FORMAT_BCD);

  /* Display time Format : hh:mm:ss */
  sprintf((char *)showTime, "%02d:%02d:%02d", stimestructureget.Hours, stimestructureget.Minutes, stimestructureget.Seconds);

  /* Display date Format : Day dd/mm/yy */
  sprintf((char *)showDate, "%s %02d/%02d/%02d", daysOfWeek[sdatestructureget.WeekDay] , sdatestructureget.Date, sdatestructureget.Month, sdatestructureget.Year);

}

/**
  * @brief  I2C8 Deinitialization Function
  * @param  None
  * @retval None
  */
void MX_I2C8_DeInit(void)
{

  /* USER CODE BEGIN I2C8_Init 0 */

  /* USER CODE END I2C8_Init 0 */

  /* Deinitialize the I2C peripheral */
  HAL_I2C_DeInit(&I2cHandle);

  /* Release HPDMA3 Channel 2 using Resource manager */
  (void) ResMgr_Release(RESMGR_RESOURCE_RIF_HPDMA3, RESMGR_HPDMA_CHANNEL(2));

  /* Release HPDMA3 Channel 3 using Resource manager */
  (void) ResMgr_Release(RESMGR_RESOURCE_RIF_HPDMA3, RESMGR_HPDMA_CHANNEL(3));

  /* Release I2C8 using Resource manager */
  (void) ResMgr_Release(RESMGR_RESOURCE_RIFSC, STM32MP25_RIFSC_I2C8_ID);

  /* Release GPIOZ3 using Resource manager */
  (void) ResMgr_Release(RESMGR_RESOURCE_RIF_GPIOZ, RESMGR_GPIO_PIN(3));

  /* Release GPIOZ4 using Resource manager */
  (void) ResMgr_Release(RESMGR_RESOURCE_RIF_GPIOZ, RESMGR_GPIO_PIN(4));
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file pointer to the source file name
  * @param  line assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: local_printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
