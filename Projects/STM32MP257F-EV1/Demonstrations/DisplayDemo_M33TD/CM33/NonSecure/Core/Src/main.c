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
#include "ssd1306_conf.h"
#include "nscoreapp_init.h"

#if !REMOTE_PROC_AUTO_START && ENABLE_BTN_MONITOR_TASK
#include "btn_monitor_task.h"
#endif
#include "remoteproc_task.h"

/** @addtogroup STM32MP2xx_Application
  * @{
  */

/** @addtogroup SPE_NSCubeProjects
  * @{
  */

/* Callbacks prototypes ------------------------------------------------------*/
extern void HAL_I2C_MspPostInit(I2C_HandleTypeDef *hi2c);

/* Global variables ----------------------------------------------------------*/
I2C_HandleTypeDef I2cHandle;

#define MASTER_BOARD
#define I2C_ADDRESS        		0x30F
#define BUS_I2Cx_FREQUENCY   	0x1094102CU
/* External function prototypes ----------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
#if !REMOTE_PROC_AUTO_START && ENABLE_BTN_MONITOR_TASK
static void BtnShortPressCallback(ButtonEventType_t event, void *context);
#endif


/**
  * @brief  Main program
  * @retval None
  */
int main(void)
{
  /* USER CODE BEGIN Init */
#if FAULT_EXCEPTION_ENABLE
  /* Initialize fault and any pre-HAL hooks */
  FAULT_Init();
#endif
  /* USER CODE END Init */

#ifdef DEBUG
  volatile uint32_t debug = 1;
  while (debug);
#endif

  if (IS_DEVELOPER_BOOT_MODE())
  {
    SystemClock_Config();
  }
  else
  {
    SystemCoreClockUpdate();
  }

  HAL_Init();

  osKernelInitialize();  /* Call init function for FreeRTOS objects */

  /* USER CODE BEGIN AppInit */
  /* Initialize application tasks and modules */
  NSCoreApp_Init();
#if !REMOTE_PROC_AUTO_START && ENABLE_BTN_MONITOR_TASK
  (void)BtnMonitorTask_RegisterListener(BUTTON_EVENT_SHORT_PRESS, BtnShortPressCallback, NULL);
#endif
  /* USER CODE END AppInit */

  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* Idle loop: place background processing here */
  }
  /* USER CODE END WHILE */
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
  BSP_LED_Init(LED3);

  while (1)
  {
    BSP_LED_Toggle(LED3);
    HAL_Delay(100);
  }
}

void App_ErrorHandler(void)
{
  Error_Handler();
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM6)
  {
    HAL_IncTick();
  }
}

/**
  * @brief  Initialize I2C8 peripheral.
  * @retval None
  */
void MX_I2C8_Init(void)
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

#if !REMOTE_PROC_AUTO_START && ENABLE_BTN_MONITOR_TASK
/**
  * @brief  Handle the short-press event used to start the remote processor.
  * @param  event Button event reported by the button monitor task.
  * @param  context User context associated with the registered listener.
  * @retval None
  */
static void BtnShortPressCallback(ButtonEventType_t event, void *context)
{
  (void)context;

  if (event != BUTTON_EVENT_SHORT_PRESS)
  {
    return;
  }

  if (RemoteProcTask_GetState() != REMOTEPROC_STATE_OFFLINE)
  {
    return;
  }

  if (RemoteProcTask_PostEvent(REMOTEPROC_EVENT_START, 0U) == HAL_OK)
  {
    (void)BtnMonitorTask_UnregisterListener(BUTTON_EVENT_SHORT_PRESS);
  }
}
#endif

/**
  * @brief  Write a command byte to the SSD1306 display over I2C.
  * @param  byte Command byte.
  * @retval None
  */
void MX_I2C_WriteCommand(uint8_t byte)
{
  (void)HAL_I2C_Mem_Write(&SSD1306_I2C_PORT, SSD1306_I2C_ADDR, 0x00U, 1U, &byte, 1U, HAL_MAX_DELAY);
}

/**
  * @brief  Write a data buffer to the SSD1306 display over I2C.
  * @param  buffer Pointer to the data buffer.
  * @param  buff_size Number of payload bytes.
  * @retval None
  */
void MX_I2C_WriteData(uint8_t *buffer, size_t buff_size)
{
  (void)HAL_I2C_Mem_Write(&SSD1306_I2C_PORT, SSD1306_I2C_ADDR, 0x40U, 1U, buffer, buff_size, HAL_MAX_DELAY);
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
