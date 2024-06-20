/**
  ******************************************************************************
  * @file    NonSecure/Src/main.c
  * @author  MCD Application Team
  * @brief   Main program body (non-secure)
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "copro_sync.h"
#include "psa/protected_storage.h"

/** @addtogroup STM32MP2xx_Application
  * @{
  */

/** @addtogroup SPE_NSCubeProjects
  * @{
  */

/* Callbacks prototypes */
/* Global variables ----------------------------------------------------------*/
COM_InitTypeDef COM_Init;
IPCC_HandleTypeDef   hipcc;
/* External function prototypes ----------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#define WRITE_DATA_1               "WRITE_DATA_1"
#define WRITE_DATA_2               "WRITE_DATA_2"
/* Private function prototypes -----------------------------------------------*/
static void Error_Handler(void);
static void MX_IPCC_Init(void);
void SystemClock_Config(void);

/**
  * @brief  Main program
  * @retval None
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

  HAL_Init();


  /* Initialize led */
  BSP_LED_Init(LED3);
  BSP_LED_On(LED3);

  if(!IS_DEVELOPER_BOOT_MODE())
  {
    /* IPCC initialization */
    MX_IPCC_Init();
    /*Corpo Sync Initialization*/
    CoproSync_Init();
  }

  /* Initialize Display destination */
  if(ResMgr_Request(RESMGR_RESOURCE_RIFSC, RESMGR_RIFSC_UART5_ID) == RESMGR_STATUS_ACCESS_OK) {
	  COM_Init.BaudRate                = 115200;
	  COM_Init.WordLength              = UART_WORDLENGTH_8B;
	  COM_Init.StopBits                = UART_STOPBITS_1;
	  COM_Init.Parity                  = UART_PARITY_NONE;
	  COM_Init.HwFlowCtl               = UART_HWCONTROL_NONE;
	  /* Initialize and select COM1 which is the COM port associated with current Core */
	  BSP_COM_Init(COM_VCP_CM33, &COM_Init);
	  BSP_COM_SelectLogPort(COM_VCP_CM33);
  }
  /* Add your non-secure example code here
     */

  printf("**** TFM_Protected_Storage M33 NonSecure ****\r\n");
  printf("\033[1;34mSTM32Cube FW version: v%ld.%ld.%ld \033[0m\r\n",
            ((HAL_GetHalVersion() >> 24) & 0x000000FF),
            ((HAL_GetHalVersion() >> 16) & 0x000000FF),
            ((HAL_GetHalVersion() >> 8) & 0x000000FF));

  psa_status_t status = 0;

  printf("Protected Storage sample started.\r\n");
  printf("PSA Protected Storage API Version %d.%d\r\n",
       PSA_PS_API_VERSION_MAJOR, PSA_PS_API_VERSION_MINOR);

  printf("Writing data to UID1: %s\r\n", WRITE_DATA_1);
  psa_storage_uid_t uid1 = 1;
  psa_storage_create_flags_t uid1_flag = PSA_STORAGE_FLAG_NONE;

  status = psa_ps_set(uid1, sizeof(WRITE_DATA_1), WRITE_DATA_1, uid1_flag);
  if (status != PSA_SUCCESS) {
    printf("Failed to store data! (%ld)\r\n", status);
    Error_Handler();
  }

  /* Get info on UID1 */
  struct psa_storage_info_t uid1_info;

  status = psa_ps_get_info(uid1, &uid1_info);
  if (status != PSA_SUCCESS) {
      printf("Failed to get info! (%ld)\r\n", status);
      Error_Handler();
  }
  printf("Info on data stored in UID1:\r\n");
  printf("- Size: %d\r\n", uid1_info.size);
  printf("- Flags: 0x%2lx\r\n", uid1_info.flags);

  printf("Read and compare data stored in UID1\r\n");
  size_t bytes_read;
  char stored_data[sizeof(WRITE_DATA_1)];

  status = psa_ps_get(uid1, 0, sizeof(WRITE_DATA_1), &stored_data, &bytes_read);
  if (status != PSA_SUCCESS) {
     printf("Failed to get data stored in UID1! (%ld)\r\n", status);
     Error_Handler();
  }
  printf("Data stored in UID1: %s\r\n", stored_data);

  printf("Overwriting data stored in UID1: %s\r\n", WRITE_DATA_2);
  status = psa_ps_set(uid1, sizeof(WRITE_DATA_2), WRITE_DATA_2, uid1_flag);
  if (status != PSA_SUCCESS) {
     printf("Failed to overwrite UID1! (%ld)\r\n", status);
     Error_Handler();
  }

  printf("Writing data to UID2 with overwrite protection: %s\r\n", WRITE_DATA_1);
  psa_storage_uid_t uid2 = 2;
  psa_storage_create_flags_t uid2_flag = PSA_STORAGE_FLAG_WRITE_ONCE;

  status = psa_ps_set(uid2, sizeof(WRITE_DATA_1), WRITE_DATA_1, uid2_flag);
  if (status != PSA_SUCCESS) {
    printf("Failed to set write once flag! (%ld)\r\n", status);
    Error_Handler();
   }

  printf("Attempting to write '%s' to UID2\r\n", WRITE_DATA_2);
  status = psa_ps_set(uid2, sizeof(WRITE_DATA_2), WRITE_DATA_2, uid2_flag);
  if (status != PSA_ERROR_NOT_PERMITTED) {
       printf("Got unexpected status when overwriting! (%ld)\r\n", status);
       Error_Handler();
  }
  printf("Got expected error (PSA_ERROR_NOT_PERMITTED) when writing to protected UID\r\n");

  printf("Removing UID1\r\n");
  status = psa_ps_remove(uid1);
  if (status != PSA_SUCCESS) {
      printf("Failed to remove UID1! (%ld)\r\n", status);
      Error_Handler();
  }
  printf("**** End of application ****\r\n");
  ResMgr_Release(RESMGR_RESOURCE_RIFSC, STM32MP25_RIFSC_UART5_ID);
  while (1)
  {
    BSP_LED_Toggle(LED3);
    HAL_Delay(1000);
  }
}

/**
  * @brief System Clock Configuration
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
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
static void Error_Handler(void)
{
  while(1)
  {
    BSP_LED_Toggle(LED3);
    HAL_Delay(100);
  }
}

/**
  * @brief  Initialize the IPCC Peripheral
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
  HAL_NVIC_SetPriority(IPCC1_RX_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(IPCC1_RX_IRQn);
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
  BSP_COM_DeInit(COM_VCP_CM33);

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
