/**
  ******************************************************************************
  * @file    logger_output_driver.c
  * @brief   Logger output driver implementation.
  ******************************************************************************
  */

#include "logger_output_driver.h"
#include "main.h"

static COM_InitTypeDef loggerComInit;

void logger_output_init(void)
{
  if (ResMgr_Request(RESMGR_RESOURCE_RIFSC, RESMGR_RIFSC_UART4_ID) == RESMGR_STATUS_ACCESS_OK)
  {
    loggerComInit.BaudRate = 115200;
    loggerComInit.WordLength = UART_WORDLENGTH_8B;
    loggerComInit.StopBits = UART_STOPBITS_1;
    loggerComInit.Parity = UART_PARITY_NONE;
    loggerComInit.HwFlowCtl = UART_HWCONTROL_NONE;
    BSP_COM_Init(COM_VCP_CM33, &loggerComInit);
    BSP_COM_SelectLogPort(COM_VCP_CM33);
  }
}