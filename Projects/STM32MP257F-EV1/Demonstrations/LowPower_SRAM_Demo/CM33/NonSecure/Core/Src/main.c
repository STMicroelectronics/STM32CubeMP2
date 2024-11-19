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
EXTI_HandleTypeDef hexti;
/* USER CODE BEGIN PV */
VIRT_UART_HandleTypeDef huart0;

__IO FlagStatus VirtUart0RxMsg = RESET;
uint8_t VirtUart0ChannelBuffRx[MAX_BUFFER_SIZE];
uint16_t VirtUart0ChannelRxSize = 0;

/* RCC Peripheral clock configuration backup variable*/
RCC_PeriphCLKInitTypeDef  PeriphClk;


uint16_t Shutdown_Req = 0;
__IO FlagStatus AlarmStatus = RESET;
#define MSG_STOP "*stop"
//#define MSG_STANDBY "*standby"
#define MSG_DELAY "*delay"

#define MSG_DELAYED_STOP "*delayedstop"

RTC_HandleTypeDef RtcHandle;
/* Exported variables --------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void MX_IPCC_Init(void);

void VIRT_UART0_RxCpltCallback(VIRT_UART_HandleTypeDef *huart);
void Check_Delay(uint8_t *BuffRx, uint16_t BuffSize);
void Check_Delayed_Sleep(uint8_t *BuffRx, uint16_t BuffSize);
void PWR_EnterSTOPMode(uint32_t Regulator, uint8_t STOPEntry);
static void MX_GPIO_Init(void);
static void MX_IPCC_Init(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
static void MX_EXTI2_Init(void);
static void MX_EXTI2_DeInit(void);
static void Exti2FallingCb(void);
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval int
  */
int main(void)
{

 unsigned int status = 0;
 uint32_t counter = 0;
  /* STM32MP2xx HAL library initialization:
     - Configure the Systick to generate an interrupt each 1 msec
     - Set NVIC Group Priority to 3
     - Low Level Initialization
  */
 EXTI_ConfigTypeDef EXTI_ConfigStructure;
 EXTI_HandleTypeDef hexti60;

 /* Clear the Low Power MPU flags */
  __HAL_PWR_CLEAR_FLAG();

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
	 if (status)
	 {
	      printf("error :  CoproSync_Init fail\r\n");
	 }

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

  /* Initialize all configured peripherals */
    MX_GPIO_Init();

    /* USER CODE BEGIN 2 */
    /* -2- Configure EXTI2 (connected to PG.8 pin) in interrupt mode */
    MX_EXTI2_Init();

  /* Update System clock variable */
  SystemCoreClockUpdate();
  //HAL_PWR_EnterSTOPMode();
  /* OpenAmp initialisation ---------------------------------*/
  MX_OPENAMP_Init(RPMSG_REMOTE, NULL);
  loc_printf("Virtual UART0 OpenAMP-rpmsg channel creation\r\n");

  if (VIRT_UART_Init(&huart0) != VIRT_UART_OK) {
    loc_printf("VIRT_UART_Init UART0 failed.\r\n");
    Error_Handler();
  }

  /*Need to register callback for message reception by channels*/
  if(VIRT_UART_RegisterCallback(&huart0, VIRT_UART_RXCPLT_CB_ID, VIRT_UART0_RxCpltCallback) != VIRT_UART_OK)
  {
   Error_Handler();
  }


  EXTI_ConfigStructure.Line = EXTI1_LINE_60;
  EXTI_ConfigStructure.Mode = EXTI_MODE_INTERRUPT;
  HAL_EXTI_SetConfigLine(&hexti60, &EXTI_ConfigStructure);

  while (1)
  {

    OPENAMP_check_for_message();

    if (VirtUart0RxMsg) {
          VirtUart0RxMsg = RESET;
          Check_Delayed_Sleep(VirtUart0ChannelBuffRx, VirtUart0ChannelRxSize);
          VIRT_UART_Transmit(&huart0, VirtUart0ChannelBuffRx, VirtUart0ChannelRxSize);
    }

    if(counter++ >= 800000)
    {
        BSP_LED_Toggle(LED3);
        counter = 0;
    }
  }

  MX_EXTI2_DeInit();
  return 0;
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

void Check_Delayed_Sleep(uint8_t *BuffRx, uint16_t BuffSize)
{
  FlagStatus Stop_Flag = RESET;
  uint32_t delay = 0;
#if DBG
  char buf[8] = {0} ;
#endif
  if (!strncmp((char *)BuffRx, MSG_DELAYED_STOP, strlen(MSG_DELAYED_STOP)))
  {
     if (BuffSize > strlen(MSG_DELAYED_STOP)) {
       delay = atoi((char *)BuffRx + strlen(MSG_DELAYED_STOP));
       delay *= 1000;
     }

     if (delay == 0)
       delay = 20;
#if DBG
      itoa(delay, buf, 10);
      VIRT_UART_Transmit(&huart0, buf, 4);
#endif
  }
  else if (!strncmp((char *)BuffRx, MSG_DELAY, strlen(MSG_DELAY)))
  {
     if (BuffSize > strlen(MSG_DELAY)) {
       delay = atoi((char *)BuffRx + strlen(MSG_DELAY));
       delay *= 1000;
     }

     if (delay == 0)
       delay = 20;

     log_info("Waiting %d secs before sending the answer message\r\n", delay);
#if DBG
     itoa(delay, buf, 10);
     VIRT_UART_Transmit(&huart0, buf, 4);
#endif
     HAL_Delay(delay);
     return;
  }
  else if (!strncmp((char *)BuffRx, MSG_STOP, strlen(MSG_STOP)))
  {
	 delay = 20;
  }
  else
  {
	  return;
  }
  BSP_LED_On(LED3);
  loc_printf("Waiting %ld msecs before sending the answer message\r\n", delay);
  HAL_Delay(delay); /* wait for ack message to be received */
  BSP_LED_Off(LED3); /* Indication of going to Stop Mode*/
  log_info("Going into CStop mode\r\n");

#if RCC_PERCLK_BKUP
    /* Back up clock context */
   HAL_RCCEx_GetPeriphCLKConfig(&PeriphClk);
#endif //RCC_PERCLK_BKUP
    /* Clear the Low Power MPU flags before going into CSTOP */
   __HAL_PWR_CLEAR_FLAG();

   PWR_EnterSTOPMode(PWR_REGULATOR_LP_ON_LV_OFF, PWR_STOPENTRY_WFI);

    /* Leaving CStop mode */

    /* Test if system was on STOP mode */
   if(__HAL_PWR_GET_FLAG(PWR_FLAG_STOP) == 1U)
   {
      /* System was on STOP mode */
     Stop_Flag = SET;

#if RCC_PERCLK_BKUP
//Backing up peripheral clock raises IAC in optee.
//E/TC:0   stm32_iac_itr:192 IAC exceptions [159:128]: 0x10000000
//E/TC:0   stm32_iac_itr:197 IAC exception ID: 156
//E/TC:0   Panic at core/drivers/firewall/stm32_iac.c:212 <stm32_iac_itr>

     /* Back up clock context */
      /* Restore clocks */
     if (HAL_RCCEx_PeriphCLKConfig(&PeriphClk) != HAL_OK)
     {
       Error_Handler();
     }
#endif //RCC_PERCLK_BKUP
   }

    /* All level of ITs can interrupt */
   __set_BASEPRI(0U);

    /* CRITICAL SECTION ENDS HERE */

   log_info("CStop mode left\r\n");

   if (Stop_Flag == SET)
   {
     log_info("System was on STOP mode\r\n");
   }
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
#if UART1
  VIRT_UART_DeInit(&huart1);
#endif
  BSP_COM_DeInit(COM_VCP_CM33);

  OPENAMP_DeInit();

  /* Deinitialize the LED3 */
  BSP_LED_DeInit(LED3);

  /* When ready, notify the remote processor that we can be shut down */
  HAL_IPCC_NotifyCPU(hipcc, ChannelIndex, IPCC_CHANNEL_DIR_RX);

  /* Wait for complete shutdown */
  while(1);
}

static void MX_GPIO_Init(void)
{
}

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
  //if (RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIF_GPIOA, RESMGR_GPIO_PIN(0)))
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
  HAL_NVIC_SetPriority(EXTI2_8_IRQn, DEFAULT_IRQ_PRIO, 0);
  HAL_NVIC_EnableIRQ(EXTI2_8_IRQn);
}

/**
  * @brief  Configures EXTI line 8 (connected to PG.8 pin) in interrupt mode
  * @param  None
  * @retval None
  */
static void MX_EXTI2_DeInit(void)
{
	HAL_GPIO_DeInit(BUTTON_USER2_GPIO_PORT, BUTTON_USER2_PIN);

	ResMgr_Release(RESMGR_RESOURCE_RIF_GPIOG, RESMGR_GPIO_PIN(8));

	HAL_NVIC_DisableIRQ(EXTI2_8_IRQn);
}



/**
  * @brief EXTI line detection callbacks
  * @param None:
  * @retval None
  */
static void Exti2FallingCb(void)
{
	BSP_LED_Toggle(LED3);
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
	  BSP_LED_Toggle(LED3);
	 HAL_Delay(300);
	 BSP_LED_Toggle(LED3);
	 	 HAL_Delay(1000);
  }
}

void PWR_EnterSTOPMode(uint32_t Regulator, uint8_t STOPEntry)
{
  /* Check the parameters */
  assert_param(IS_PWR_STOP_MODE_REGULATOR(Regulator));
  assert_param(IS_PWR_STOP_ENTRY(STOPEntry));

/*indicate  which power saving  can be done when cpu is in stop mode*/
  #define LP_LV_Msk (PWR_CPU2CR_LPDS_D2_Msk|PWR_CPU2CR_LVDS_D2_Msk)
  #define REGU_SHIFT PWR_CPU2CR_LPDS_D2_Pos
  __IO uint32_t *regaddr = &PWR->CPU2CR;
  /* Check  parameter */
  assert_param(IS_PWR_STOP_MODE_REGULATOR(Regulator));
  *regaddr = ((*regaddr & (~LP_LV_Msk)) | (Regulator<<REGU_SHIFT));

/*check  request to system standby not set*/
  CLEAR_BIT(PWR->CPU2CR, SYSTEM_STANDBY_REQUEST);

/* Ensure core enters in  CSTOP mode on sleep (core and core-subsystem  clock are gated) */
  /* Set DEEP SLEEP bit of Cortex System Control Register */
  SET_BIT(SCB->SCR,SCB_SCR_SLEEPDEEP_Msk);

  /* Select Stop mode entry --------------------------------------------------*/
  if ((STOPEntry == PWR_STOPENTRY_WFI))
  {
    /* Request Wait For Interrupt */
    __WFI();
  }
  else if (STOPEntry == PWR_STOPENTRY_WFE)
  {
    /* Request Wait For Event */
    __SEV();
    __WFE();
    __WFE();
  }

/*remove any low power indication for regulator when CPU is in stop mode*/
/*not mandatory as low power PWR output signal are not active when system is in RUN mode, but cleaner  to clear these bit inside PWR register*/
  #define LP_LV_Msk (PWR_CPU2CR_LPDS_D2_Msk|PWR_CPU2CR_LVDS_D2_Msk)
  #define REGU_SHIFT PWR_CPU2CR_LPDS_D2_Pos
  MODIFY_REG(PWR->CPU2CR, LP_LV_Msk, PWR_REGULATOR_LP_OFF<<REGU_SHIFT);

/* remove 'clock gating on sleep entry' request  */
  /* clear DEEP SLEEP bit of Cortex System Control Register */
  CLEAR_BIT(SCB->SCR,SCB_SCR_SLEEPDEEP_Msk);
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
