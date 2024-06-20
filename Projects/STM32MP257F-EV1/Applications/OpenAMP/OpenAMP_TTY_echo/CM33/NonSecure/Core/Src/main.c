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

/* Private includes ----------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define MAX_BUFFER_SIZE RPMSG_BUFFER_SIZE

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
COM_InitTypeDef COM_Init;
IPCC_HandleTypeDef hipcc;

/* USER CODE BEGIN PV */
VIRT_UART_HandleTypeDef huart0;
VIRT_UART_HandleTypeDef huart1;

__IO FlagStatus VirtUart0RxMsg = RESET;
uint8_t VirtUart0ChannelBuffRx[MAX_BUFFER_SIZE];
uint16_t VirtUart0ChannelRxSize = 0;

__IO FlagStatus VirtUart1RxMsg = RESET;
uint8_t VirtUart1ChannelBuffRx[MAX_BUFFER_SIZE];
uint16_t VirtUart1ChannelRxSize = 0;

/* Exported variables --------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void MX_IPCC_Init(void);

void VIRT_UART0_RxCpltCallback(VIRT_UART_HandleTypeDef *huart);
void VIRT_UART1_RxCpltCallback(VIRT_UART_HandleTypeDef *huart);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval int
  */
int main(void)
{
  unsigned int counter = 0;

  /* STM32MP2xx HAL library initialization:
     - Configure the Systick to generate an interrupt each 1 msec
     - Set NVIC Group Priority to 3
     - Low Level Initialization
  */

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Initialize Display destination */
#if defined(__VALID_OUTPUT_TERMINAL_IO__) && defined (__GNUC__)
  initialise_monitor_handles();
#else
  if(ResMgr_Request(RESMGR_RESOURCE_RIFSC, STM32MP25_RIFSC_UART5_ID) == RESMGR_STATUS_ACCESS_OK)
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

  /* Initialize led */
  BSP_LED_Init(LED3);
  BSP_LED_On(LED3);

  loc_printf ("\r\n Starting OpenAMP application (%s: %s) \r\n", __DATE__, __TIME__);

  /* IPCC initialisation */
  MX_IPCC_Init();

  if (!IS_DEVELOPER_BOOT_MODE())
  {
    /* Copro Sync Initialization */
	CoproSync_Init();
  }

#if defined (RPMSG_LATE_ATTACH)
  /*
   * To enable only if the firmware is loaded by U-Boot
   * Do some works waiting Kick for the Host processor
   */
  while (OPENAMP_check_for_message())
  {
    BSP_LED_Toggle(LED3);
    HAL_Delay(1000);
  }
#endif /* RPMSG_LATE_ATTACH */

  /* Update System clock variable */
  SystemCoreClockUpdate();

  /* OpenAmp initialisation ---------------------------------*/
  MX_OPENAMP_Init(RPMSG_REMOTE, NULL);
  loc_printf("Virtual UART0 OpenAMP-rpmsg channel creation\r\n");

  if (VIRT_UART_Init(&huart0) != VIRT_UART_OK) {
    loc_printf("VIRT_UART_Init UART0 failed.\r\n");
    Error_Handler();
  }

  loc_printf("Virtual UART1 OpenAMP-rpmsg channel creation\r\n");
  if (VIRT_UART_Init(&huart1) != VIRT_UART_OK) {
    loc_printf("VIRT_UART_Init UART1 failed.\r\n");
    Error_Handler();
  }

  /*Need to register callback for message reception by channels*/
  if(VIRT_UART_RegisterCallback(&huart0, VIRT_UART_RXCPLT_CB_ID, VIRT_UART0_RxCpltCallback) != VIRT_UART_OK)
  {
   Error_Handler();
  }
  if(VIRT_UART_RegisterCallback(&huart1, VIRT_UART_RXCPLT_CB_ID, VIRT_UART1_RxCpltCallback) != VIRT_UART_OK)
  {
    Error_Handler();
  }

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

    OPENAMP_check_for_message();

    if (VirtUart0RxMsg)
    {
      VirtUart0RxMsg = RESET;
      VIRT_UART_Transmit(&huart0, VirtUart0ChannelBuffRx, VirtUart0ChannelRxSize);
    }

    if (VirtUart1RxMsg)
    {
      VirtUart1RxMsg = RESET;
      VIRT_UART_Transmit(&huart1, VirtUart1ChannelBuffRx, VirtUart1ChannelRxSize);
    }

    if(counter++ == 200000)
    {
        BSP_LED_Toggle(LED3);
        counter = 0;
    }
  }
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
  /* USER CODE END 3 */

}

/**
  * @brief IPPC Initialization Function
  * @param  None
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

void VIRT_UART0_RxCpltCallback(VIRT_UART_HandleTypeDef *huart)
{

    loc_printf("Msg received on VIRTUAL UART0 channel:  %s \n\r", (char *) huart->pRxBuffPtr);

    /* copy received msg in a variable to sent it back to master processor in main infinite loop*/
    VirtUart0ChannelRxSize = huart->RxXferSize < MAX_BUFFER_SIZE? huart->RxXferSize : MAX_BUFFER_SIZE-1;
    memcpy(VirtUart0ChannelBuffRx, huart->pRxBuffPtr, VirtUart0ChannelRxSize);
    VirtUart0RxMsg = SET;
}

void VIRT_UART1_RxCpltCallback(VIRT_UART_HandleTypeDef *huart)
{

    loc_printf("Msg received on VIRTUAL UART1 channel:  %s \n\r", (char *) huart->pRxBuffPtr);

    /* copy received msg in a variable to sent it back to master processor in main infinite loop*/
    VirtUart1ChannelRxSize = huart->RxXferSize < MAX_BUFFER_SIZE? huart->RxXferSize : MAX_BUFFER_SIZE-1;
    memcpy(VirtUart1ChannelBuffRx, huart->pRxBuffPtr, VirtUart1ChannelRxSize);
    VirtUart1RxMsg = SET;
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
  VIRT_UART_DeInit(&huart0);
  VIRT_UART_DeInit(&huart1);
  BSP_COM_DeInit(COM_VCP_CM33);

  OPENAMP_DeInit();

  /* Deinitialize the LED3 */
  BSP_LED_DeInit(LED3);

  /* When ready, notify the remote processor that we can be shut down */
  HAL_IPCC_NotifyCPU(hipcc, ChannelIndex, IPCC_CHANNEL_DIR_RX);

  /* Wait for complete shutdown */
  while(1);
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
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  while (1)
  {
  }
}
#endif /* USE_FULL_ASSERT */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
  while(1)
  {
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
