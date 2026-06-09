/**
  ******************************************************************************
  * @file    logger_output_driver.c
  * @author  Application Team
  * @brief   Logger output driver Implementation.
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

/**
  * @addtogroup LoggerOutputDriver
  * @{
  */

/* Includes ------------------------------------------------------------------*/
#include "logger_output_driver.h"
#include "main.h"
#include <stdio.h>

/* Private variables ---------------------------------------------------------*/
COM_InitTypeDef COM_Init;

/**
  * @brief  Initialize the logger output.
  * @retval None
  */
void logger_output_init(void) {

  /* Initialize Uart For Logging*/
  if (ResMgr_Request(RESMGR_RESOURCE_RIFSC, RESMGR_RIFSC_UART5_ID) == RESMGR_STATUS_ACCESS_OK)
  {
    COM_Init.BaudRate = 115200;
    COM_Init.WordLength = UART_WORDLENGTH_8B;
    COM_Init.StopBits = UART_STOPBITS_1;
    COM_Init.Parity = UART_PARITY_NONE;
    COM_Init.HwFlowCtl = UART_HWCONTROL_NONE;
    /* Initialize and select COM1 which is the COM port associated with current Core */
    BSP_COM_Init(COM_VCP_CM33, &COM_Init);
    BSP_COM_SelectLogPort(COM_VCP_CM33);
  }
}

/**
  * @}
  */
