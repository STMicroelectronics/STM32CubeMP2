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
/* USER CODE BEGIN Includes */
#if !REMOTE_PROC_AUTO_START && ENABLE_BTN_MONITOR_TASK
#include "btn_monitor_task.h"
#endif
#include "remoteproc_task.h"
/* USER CODE END Includes */

/** @addtogroup STM32MP2xx_Application
  * @{
  */

/** @addtogroup SPE_NSCubeProjects
  * @{
  */
/* Callbacks prototypes */
/* Global variables ----------------------------------------------------------*/
I3C_HandleTypeDef hi3c1;

/**
  * @brief Index of Frame 1 for I3C context buffers.
  */
#define I3C_IDX_FRAME_1         0U

/**
  * @brief Maximum SSD1306 payload size for one page write.
  */
#define DISPLAY_I3C_MAX_PAYLOAD 128U

/**
  * @brief Number of control words needed for a single private I3C transfer.
  */
#define DISPLAY_I3C_CTRL_WORDS  1U

/**
  * @brief Buffer used by HAL to compute control data for the Private Communication.
  */
uint32_t aControlBuffer[0xFF];

/**
  * @brief Persistent descriptor payload buffer for SSD1306 I3C writes.
  */
static uint8_t aDisplayTxPayload[DISPLAY_I3C_MAX_PAYLOAD + 1U];

/**
  * @brief Persistent transfer frame buffer used by HAL_I3C_AddDescToFrame.
  */
static uint8_t aDisplayTxFrame[DISPLAY_I3C_MAX_PAYLOAD + 1U];

/**
  * @brief Context buffer related to Frame context, contains buffer values for communication.
  */
I3C_XferTypeDef aContextBuffers[1] = {
  {{aControlBuffer, 1}, {NULL, 0}, {NULL, 0}, {NULL, 0}}
};

/**
  * @brief Descriptor for private data transmit.
  */
I3C_PrivateTypeDef aPrivateDescriptor[1] = {
  {SSD1306_I2C_ADDR, {NULL, 0}, {NULL, 0}, HAL_I3C_DIRECTION_WRITE}
};

/* External function prototypes ----------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_I3C_WriteCommand(uint8_t byte);
void MX_I3C_WriteData(uint8_t *buffer, size_t buff_size);
void MX_I3C1_Init(void);
void MX_I3C1_DeInit(void);
void HAL_I3C_MspPostInit(I3C_HandleTypeDef *hi3c);
static void MX_I3C_ResetTransferContext(size_t transfer_size);

/* USER CODE BEGIN PFP */
#if !REMOTE_PROC_AUTO_START && ENABLE_BTN_MONITOR_TASK
static void BtnShortPressCallback(ButtonEventType_t event, void *context);
#endif
/* USER CODE END PFP */

/**
  * @brief  Main program entry point.
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

  osKernelInitialize();  /* Initialize the CMSIS-RTOS2 kernel. */

  /* USER CODE BEGIN AppInit */
  /* Initialize application tasks and modules */
  NSCoreApp_Init();
#if !REMOTE_PROC_AUTO_START && ENABLE_BTN_MONITOR_TASK
  (void)BtnMonitorTask_RegisterListener(BUTTON_EVENT_SHORT_PRESS, BtnShortPressCallback, NULL);
#endif
  /* Add other task/module initializations here */
  /* USER CODE END AppInit */

  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    /* Idle loop: place background processing here */
  }
  /* USER CODE END 3 */
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

/**
  * @brief I3C1 Initialization Function
  * @param None
  * @retval None
  */
void MX_I3C1_Init(void)
{

  /* USER CODE BEGIN I3C1_Init 0 */

  /* USER CODE END I3C1_Init 0 */

  I3C_FifoConfTypeDef sFifoConfig = {0};
  I3C_CtrlConfTypeDef sCtrlConfig = {0};

  /* USER CODE BEGIN I3C1_Init 1 */

  /* USER CODE END I3C1_Init 1 */

  /* Acquire I3C1 using Resource manager */
  if (RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIFSC, RESMGR_RIFSC_I3C2_ID))
  {
    Error_Handler();
  }
  /* Acquire GPIOA2 using Resource manager */
  if (RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIF_GPIOC, RESMGR_GPIO_PIN(12)))
  {
    Error_Handler();
  }

  /* Acquire GPIOG13 using Resource manager */
  if (RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIF_GPIOG, RESMGR_GPIO_PIN(0)))
  {
    Error_Handler();
  }

  /* Enable GPIOs power supplies */
  if (RESMGR_STATUS_ACCESS_OK == ResMgr_Request(RESMGR_RESOURCE_RIF_RCC, RESMGR_RCC_RESOURCE(101)))
  {
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
  }

  hi3c1.Instance = I3C2;
    hi3c1.Mode = HAL_I3C_MODE_CONTROLLER;
    hi3c1.Init.CtrlBusCharacteristic.SDAHoldTime = HAL_I3C_SDA_HOLD_TIME_0_5;
    hi3c1.Init.CtrlBusCharacteristic.WaitTime = HAL_I3C_OWN_ACTIVITY_STATE_0;
    hi3c1.Init.CtrlBusCharacteristic.SCLPPLowDuration = 0x2f;
    hi3c1.Init.CtrlBusCharacteristic.SCLI3CHighDuration = 0x02;
    hi3c1.Init.CtrlBusCharacteristic.SCLODLowDuration = 0x7f;
    hi3c1.Init.CtrlBusCharacteristic.SCLI2CHighDuration = 0x1f;
    hi3c1.Init.CtrlBusCharacteristic.BusFreeDuration = 0x4a;
    hi3c1.Init.CtrlBusCharacteristic.BusIdleDuration = 0x3e;
  if (HAL_I3C_Init(&hi3c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure FIFO
  */
  sFifoConfig.RxFifoThreshold = HAL_I3C_RXFIFO_THRESHOLD_1_4;
  sFifoConfig.TxFifoThreshold = HAL_I3C_TXFIFO_THRESHOLD_1_4;
  sFifoConfig.ControlFifo = HAL_I3C_CONTROLFIFO_DISABLE;
  sFifoConfig.StatusFifo = HAL_I3C_STATUSFIFO_DISABLE;
  if (HAL_I3C_SetConfigFifo(&hi3c1, &sFifoConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure controller
  */
  sCtrlConfig.DynamicAddr = 0;
  sCtrlConfig.StallTime = 0x00;
  sCtrlConfig.HotJoinAllowed = DISABLE;
  sCtrlConfig.ACKStallState = DISABLE;
  sCtrlConfig.CCCStallState = DISABLE;
  sCtrlConfig.TxStallState = DISABLE;
  sCtrlConfig.RxStallState = DISABLE;
  sCtrlConfig.HighKeeperSDA = DISABLE;
  if (HAL_I3C_Ctrl_Config(&hi3c1, &sCtrlConfig) != HAL_OK)
  {
    Error_Handler();
  }

  HAL_I3C_MspPostInit(&hi3c1);
  /* USER CODE BEGIN I3C1_Init 2 */

  /* USER CODE END I3C1_Init 2 */

}

#if !REMOTE_PROC_AUTO_START
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
  * @brief  I3C1 DeInitialization Function
  * @param  None
  * @retval None
  */
void MX_I3C1_DeInit(void)
{
  /* Deinitialize the I3C peripheral */
  HAL_I3C_DeInit(&hi3c1);

  /* Release GPIOZ3 using Resource manager */
  ResMgr_Release(RESMGR_RESOURCE_RIF_GPIOG, RESMGR_GPIO_PIN(0));

  /* Release GPIOZ4 using Resource manager */
  ResMgr_Release(RESMGR_RESOURCE_RIF_GPIOC, RESMGR_GPIO_PIN(12));

  /* Release I3C1 using Resource manager */
  ResMgr_Release(RESMGR_RESOURCE_RIFSC, RESMGR_RIFSC_I3C2_ID);

}

/**
  * @brief  Reset the transfer context consumed in place by the HAL I3C API.
  * @param  transfer_size Number of bytes in the generated transfer frame.
  * @retval None
  */
static void MX_I3C_ResetTransferContext(size_t transfer_size)
{
  aContextBuffers[I3C_IDX_FRAME_1].CtrlBuf.pBuffer = aControlBuffer;
  aContextBuffers[I3C_IDX_FRAME_1].CtrlBuf.Size = DISPLAY_I3C_CTRL_WORDS;
  aContextBuffers[I3C_IDX_FRAME_1].StatusBuf.pBuffer = NULL;
  aContextBuffers[I3C_IDX_FRAME_1].StatusBuf.Size = 0U;
  aContextBuffers[I3C_IDX_FRAME_1].TxBuf.pBuffer = aDisplayTxFrame;
  aContextBuffers[I3C_IDX_FRAME_1].TxBuf.Size = (uint32_t)transfer_size;
  aContextBuffers[I3C_IDX_FRAME_1].RxBuf.pBuffer = NULL;
  aContextBuffers[I3C_IDX_FRAME_1].RxBuf.Size = 0U;
}
/**
  * @brief  Write a command byte to SSD1306 using I3C.
  * @param  byte Command byte to send.
  * @retval None
  */
void MX_I3C_WriteCommand(uint8_t byte) {
    aDisplayTxPayload[0] = 0x00;
    aDisplayTxPayload[1] = byte;
    MX_I3C_ResetTransferContext(2U);
    aPrivateDescriptor[I3C_IDX_FRAME_1].TxBuf.pBuffer = aDisplayTxPayload;
    aPrivateDescriptor[I3C_IDX_FRAME_1].TxBuf.Size = 2;
    HAL_I3C_AddDescToFrame(&SSD1306_I2C_PORT, NULL, &aPrivateDescriptor[I3C_IDX_FRAME_1], &aContextBuffers[I3C_IDX_FRAME_1], aContextBuffers[I3C_IDX_FRAME_1].CtrlBuf.Size, I2C_PRIVATE_WITHOUT_ARB_STOP);
    HAL_I3C_Ctrl_Transmit(&SSD1306_I2C_PORT, &aContextBuffers[I3C_IDX_FRAME_1], 1000);
}

/**
  * @brief Write data buffer to SSD1306 using I3C.
  * @param buffer Pointer to data buffer.
  * @param buff_size Size of data buffer.
  *
  * This function sends a data buffer to the SSD1306 display using the I3C controller
  * in backward-compatible I2C mode. The first byte is a control byte (0x40) indicating data.
  */
void MX_I3C_WriteData(uint8_t* buffer, size_t buff_size) {
    if (buff_size > DISPLAY_I3C_MAX_PAYLOAD) {
      Error_Handler();
    }

    aDisplayTxPayload[0] = 0x40;
    memcpy(&aDisplayTxPayload[1], buffer, buff_size);
    MX_I3C_ResetTransferContext(buff_size + 1U);
    aPrivateDescriptor[I3C_IDX_FRAME_1].TxBuf.pBuffer = aDisplayTxPayload;
    aPrivateDescriptor[I3C_IDX_FRAME_1].TxBuf.Size = buff_size + 1;
    HAL_I3C_AddDescToFrame(&SSD1306_I2C_PORT, NULL, &aPrivateDescriptor[I3C_IDX_FRAME_1], &aContextBuffers[I3C_IDX_FRAME_1], aContextBuffers[I3C_IDX_FRAME_1].CtrlBuf.Size, I2C_PRIVATE_WITHOUT_ARB_STOP);
    HAL_I3C_Ctrl_Transmit(&SSD1306_I2C_PORT, &aContextBuffers[I3C_IDX_FRAME_1], HAL_MAX_DELAY);
}

/**
  * @brief  Period elapsed callback for TIM6.
  * @param  htim TIM handle pointer.
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM6) {
        HAL_IncTick();
    }
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
    while (1) {
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

