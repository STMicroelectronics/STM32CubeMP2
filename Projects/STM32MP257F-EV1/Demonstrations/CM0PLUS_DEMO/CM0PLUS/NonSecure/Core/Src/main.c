/**
  ******************************************************************************
  * @file    main.c
  * @author  MCD Application Team
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "stdlib.h"

/** @addtogroup STM32MP2xx_HAL_Examples
  * @{
  */

/** @addtogroup Templates
  * @{
  */
/* Global variables ---------------------------------------------------------*/
COM_InitTypeDef COM_Init;
/* External function prototypes ----------------------------------------------*/
#if defined(__VALID_OUTPUT_TERMINAL_IO__) && defined (__GNUC__)
void initialise_monitor_handles(void);
#endif /* __VALID_OUTPUT_TERMINAL_IO__ && __GNUC__ */
extern uint32_t __IPC_SHM_region_start__;  /* defined by linker script */
extern uint32_t __IPC_SHM_region_length__;    /* defined by linker script */
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
IPCC_HandleTypeDef hipcc;
static char *ipc_shm_addr;
static uint8_t ipc_shm_size;
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint32_t count = 0;
static uint32_t delay_s = 5; /* 5s */
static uint32_t wakeup = 0;
/* Private function prototypes -----------------------------------------------*/
void MX_IPCC_Init();
void MX_IPCC_DeInit();
void IPCC_channel1_callback(IPCC_HandleTypeDef * hipcc, uint32_t ChannelIndex, IPCC_CHANNELDirTypeDef ChannelDir);

/**
  * @brief  Main program
  * @retval None
  */
int main(void)
{
  /* Low Power Workaround Initialization:
   * 
   * The M0+ microcontroller may crash when power domains D1 or D2 enter or exit
   * low power states (Standby 1 and LPLV-Stop 2).
   * To prevent this issue, the M0+ is placed in a Wait For Interrupt (WFI)
   * state for less than 800 us during the transitions of D1/D2 into or out of
   * low power states.
   * 
   * Requirement: The main CPU (either CPU1 or CPU2) must send an interrupt just
   * before entering low power using EXTI2 (line 59 for CPU2 and line 60 for
   * CPU1) to trigger the workaround.
   */
  lowpower_wa_Init();
  /* STM32MP2xx HAL library initialization:
       - Secure Systick timer is configured by default as source of time base,
         but user can eventually implement his proper time base source (a general
         purpose timer for example or other time source), keeping in mind that
         Time base duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined
         and handled in milliseconds basis.
       - Low Level Initialization
     */
  HAL_Init();

  /* Initialize Display destination */
#if defined(__VALID_OUTPUT_TERMINAL_IO__) && defined (__GNUC__)
  initialise_monitor_handles();
#else
  if(ResMgr_Request(RESMGR_RESOURCE_RIFSC, STM32MP25_RIFSC_LPUART1_ID) == RESMGR_STATUS_ACCESS_OK)
  {
    COM_Init.BaudRate                = 115200;
    COM_Init.WordLength              = UART_WORDLENGTH_8B;
    COM_Init.StopBits                = UART_STOPBITS_1;
    COM_Init.Parity                  = UART_PARITY_NONE;
    COM_Init.HwFlowCtl               = UART_HWCONTROL_NONE;
    /* Initialize and select COM1 which is the COM port associated with current Core */
    BSP_COM_Init(COM_CM0PLUS, &COM_Init);
    BSP_COM_SelectLogPort(COM_CM0PLUS);
  }
#endif

  printf("\n\r**** CM0PLUS Example application ****\r\n");

  /* IPCC initialization */
  MX_IPCC_Init();

  /* Add your CM0PLUS application code here */
  while (1)
  {
    count++;
    printf("(%ld) Hello from CM0+ \n\r", count);

    if (wakeup)
    {
      /* Reset A35 wakeup request */
      wakeup = 0;

      printf("(%ld) A35 wakeup request in %lds\n\r", count, delay_s);

      /* Convert delay in ms */
      HAL_Delay(1000*delay_s);

      printf("(%ld) Send A35 wakeup request\n\r", count);

      /* Notify A35 */
      HAL_IPCC_NotifyCPU(&hipcc, IPCC_CHANNEL_1, IPCC_CHANNEL_DIR_TX);

      printf("(%ld) A35 wakeup request done !\n\r", count);
    }
    HAL_Delay(2000);
  }
}

/**
  * @brief IPPC Initialization Function
  * @param  None
  * @retval None
  */
void MX_IPCC_Init(void)
{
  hipcc.Instance = IPCC2;
  if (HAL_IPCC_Init(&hipcc) != HAL_OK)
  {
     Error_Handler();
  }
  /* IPCC interrupt Init */
  HAL_NVIC_SetPriority(IPCC2_RX_IRQn, DEFAULT_IRQ_PRIO, 0);
  HAL_NVIC_EnableIRQ(IPCC2_RX_IRQn);

  if (HAL_IPCC_ActivateNotification(&hipcc, IPCC_CHANNEL_1, IPCC_CHANNEL_DIR_RX,
      IPCC_channel1_callback) != HAL_OK) {
    Error_Handler();
  }

  ipc_shm_addr = (char *)(&__IPC_SHM_region_start__);
  ipc_shm_size = (uint8_t)(__IPC_SHM_region_length__);
}

void MX_IPCC_DeInit(void)
{
  if (HAL_IPCC_DeActivateNotification(&hipcc, IPCC_CHANNEL_1, IPCC_CHANNEL_DIR_RX) != HAL_OK) {
	Error_Handler();
  }
  /* IPCC interrupt DeInit */
  HAL_NVIC_DisableIRQ(IPCC2_RX_IRQn);

  hipcc.Instance = IPCC2;
  if (HAL_IPCC_DeInit(&hipcc) != HAL_OK)
  {
    Error_Handler();
  }
}

/* Callback from IPCC Interrupt Handler: Master Processor informs that there are some free buffers */
void IPCC_channel1_callback(IPCC_HandleTypeDef * hipcc,
         uint32_t ChannelIndex, IPCC_CHANNELDirTypeDef ChannelDir)
{
  /* USER CODE BEGIN PRE_MAILBOX_CHANNEL1_CALLBACK */

  /* USER CODE END  PRE_MAILBOX_CHANNEL1_CALLBACK */

  /* Notify A35 */
  HAL_IPCC_NotifyCPU(hipcc, IPCC_CHANNEL_1, IPCC_CHANNEL_DIR_RX);

  /* Read delay value */
  sscanf(ipc_shm_addr,"%ld", &delay_s);

  /* Make sure delay is correct */
  if (delay_s > 0) {
    /* Allow A35 wakeup */
    wakeup = 1;
  }

  /* USER CODE BEGIN POST_MAILBOX_CHANNEL1_CALLBACK */

  /* USER CODE END  POST_MAILBOX_CHANNEL1_CALLBACK */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
  printf("%s\n\r",__func__);
  while(1)
  {
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
