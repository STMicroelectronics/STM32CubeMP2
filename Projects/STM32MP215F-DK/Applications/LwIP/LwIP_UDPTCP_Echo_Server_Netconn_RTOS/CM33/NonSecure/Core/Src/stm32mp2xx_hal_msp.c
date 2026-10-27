/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file         stm32mp2xx_hal_msp.c
 * @brief        This file provides code for the MSP Initialization
 *               and de-Initialization codes.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "res_mgr.h"
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN Define */

/* USER CODE END Define */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN Macro */

/* USER CODE END Macro */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* External functions --------------------------------------------------------*/
/* USER CODE BEGIN ExternalFunctions */

/* USER CODE END ExternalFunctions */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * Initializes the Global MSP.
 */
void HAL_MspInit(void)
{
  /* USER CODE BEGIN MspInit 0 */
  /* Acquire TIM6 using Resource manager */
  if (ResMgr_Request(RESMGR_RESOURCE_RIFSC, STM32MP21_RIFSC_TIM6_ID) != RESMGR_STATUS_ACCESS_OK)
  {
    Error_Handler();
  }
  /* USER CODE END MspInit 0 */
}

/**
 * DeInitializes the Global MSP.
 */
void HAL_MspDeInit(void)
{
  /* USER CODE BEGIN MspInit 0 */
  /* Release TIM6 using Resource manager */
  if (ResMgr_Release(RESMGR_RESOURCE_RIFSC, STM32MP21_RIFSC_TIM6_ID) != RESMGR_STATUS_ACCESS_OK)
  {
    Error_Handler();
  }
  /* USER CODE END MspInit 0 */
}

/**
 * @brief  Initializes the ETH MSP.
 * @param  heth: pointer to a ETH_HandleTypeDef structure that contains
 *         the configuration information for ETHERNET module
 * @retval None
 */

void HAL_ETH_MspInit(ETH_HandleTypeDef *heth)
{
/* Ethernet MSP init:
		TX_EN   --------------> PA13
		TXD0    --------------> PA15
		TXD1    --------------> PC1
		RXD0    --------------> PF1
		RXD1    --------------> PC2
		CRS_DV  --------------> PA11
		MDC     --------------> PF0
		MDIO    --------------> PF2
		NRST 	--------------> PH11
		REFCLK  ------------->  PA12
		CLK		--------------> PF8
 */

  GPIO_InitTypeDef GPIO_InitStructure = {0};

  /* Acquire ETH1 using Resource manager */
  if (ResMgr_Request(RESMGR_RESOURCE_RIFSC, STM32MP21_RIFSC_ETH1_ID) != RESMGR_STATUS_ACCESS_OK)
  {
    Error_Handler();
  }

  /* Acquire GPIOA13 ETH1_TX_EN using Resource manager */
  if (ResMgr_Request(RESMGR_RESOURCE_RIF_GPIOA, RESMGR_GPIO_PIN(13))  != RESMGR_STATUS_ACCESS_OK){
    Error_Handler();
  }

  /* Acquire GPIOA15 ETH1_TXD0 using Resource manager */
  if (ResMgr_Request(RESMGR_RESOURCE_RIF_GPIOA, RESMGR_GPIO_PIN(15))  != RESMGR_STATUS_ACCESS_OK){
    Error_Handler();
  }

  /* Acquire GPIOC1 ETH1_TXD1 using Resource manager */
  if (ResMgr_Request(RESMGR_RESOURCE_RIF_GPIOC, RESMGR_GPIO_PIN(1)) != RESMGR_STATUS_ACCESS_OK){
    Error_Handler();
  }

  /* Acquire GPIOF1 ETH1_RXD0 using Resource manager */
  if (ResMgr_Request(RESMGR_RESOURCE_RIF_GPIOF, RESMGR_GPIO_PIN(1)) != RESMGR_STATUS_ACCESS_OK){
    Error_Handler();
  }

  /* Acquire GPIOC2 ETH1_RXD1 using Resource manager */
  if (ResMgr_Request(RESMGR_RESOURCE_RIF_GPIOC, RESMGR_GPIO_PIN(2)) != RESMGR_STATUS_ACCESS_OK){
    Error_Handler();
  }

  /* Acquire GPIOA11 ETH1_CRS_DV using Resource manager */
  if (ResMgr_Request(RESMGR_RESOURCE_RIF_GPIOA, RESMGR_GPIO_PIN(11)) != RESMGR_STATUS_ACCESS_OK){
    Error_Handler();
  }

  /* Acquire GPIOF0 ETH1_MDC using Resource manager */
  if (ResMgr_Request(RESMGR_RESOURCE_RIF_GPIOF, RESMGR_GPIO_PIN(0)) != RESMGR_STATUS_ACCESS_OK){
    Error_Handler();
  }

  /* Acquire GPIOF2 ETH1_MDIO using Resource manager */
  if (ResMgr_Request(RESMGR_RESOURCE_RIF_GPIOF, RESMGR_GPIO_PIN(2)) != RESMGR_STATUS_ACCESS_OK){
    Error_Handler();
  }

  /* Acquire GPIOH11 ETH1_NRST using Resource manager */
  if (ResMgr_Request(RESMGR_RESOURCE_RIF_GPIOH, RESMGR_GPIO_PIN(11)) != RESMGR_STATUS_ACCESS_OK){
    Error_Handler();
  }

  /* Acquire GPIOA12 ETH1_MDINT using Resource manager */
  if (ResMgr_Request(RESMGR_RESOURCE_RIF_GPIOA, RESMGR_GPIO_PIN(12))  != RESMGR_STATUS_ACCESS_OK){
    Error_Handler();
  }

  /* Acquire GPIOF8 ETH1_CLK using Resource manager */
  if (ResMgr_Request(RESMGR_RESOURCE_RIF_GPIOF, RESMGR_GPIO_PIN(8)) != RESMGR_STATUS_ACCESS_OK){
    Error_Handler();
  }

  /* Enable Ethernet clocks */
  __HAL_RCC_ETH1_CLK_ENABLE();
  __HAL_RCC_ETH1MAC_CLK_ENABLE();
  __HAL_RCC_ETH1TX_CLK_ENABLE();
  __HAL_RCC_ETH1RX_CLK_ENABLE();

  /* Select Ethernet 50MHz internal clock from RCC */
  HAL_SYSCFG_ETH1RefClockSelect(SYSCFG_ETH_RCC_REF_CLK);

#if defined(USE_DEVELOPMENT_MODE)
  /* Enable GPIOA clock */
  if (ResMgr_Request(RESMGR_RESOURCE_RIF_RCC, RESMGR_RCC_RESOURCE(90)) == RESMGR_STATUS_ACCESS_OK){
    __HAL_RCC_GPIOA_CLK_ENABLE();
  }

  /* Enable GPIOC clock */
  if (ResMgr_Request(RESMGR_RESOURCE_RIF_RCC, RESMGR_RCC_RESOURCE(92)) == RESMGR_STATUS_ACCESS_OK){
    __HAL_RCC_GPIOC_CLK_ENABLE();
  }

  /* Enable GPIOF clock */
  if (ResMgr_Request(RESMGR_RESOURCE_RIF_RCC, RESMGR_RCC_RESOURCE(95)) == RESMGR_STATUS_ACCESS_OK){
    __HAL_RCC_GPIOF_CLK_ENABLE();
  }

  /* Enable GPIOH clock */
  if (ResMgr_Request(RESMGR_RESOURCE_RIF_RCC, RESMGR_RCC_RESOURCE(97)) == RESMGR_STATUS_ACCESS_OK){
    __HAL_RCC_GPIOH_CLK_ENABLE();
  }
#endif

  /* Configure PA13 - 1. ETH1_TX_EN */
  GPIO_InitStructure.Pin = GPIO_PIN_13;
  GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStructure.Pull = GPIO_NOPULL;
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_MEDIUM;
  GPIO_InitStructure.Alternate = GPIO_AF10_ETH1;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Configure PA15 - 2. ETH1_TXD0 */
  GPIO_InitStructure.Pin = GPIO_PIN_15;
  GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStructure.Pull = GPIO_NOPULL;
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_MEDIUM;
  GPIO_InitStructure.Alternate = GPIO_AF10_ETH1;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Configure PC1 - 3. ETH1_TXD1 */
  GPIO_InitStructure.Pin = GPIO_PIN_1;
  GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStructure.Pull = GPIO_NOPULL;
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_MEDIUM;
  GPIO_InitStructure.Alternate = GPIO_AF10_ETH1;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);

  /* Configure PF1 - 4. ETH1_RXD0 */
  GPIO_InitStructure.Pin = GPIO_PIN_1;
  GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStructure.Pull = GPIO_NOPULL;
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStructure.Alternate = GPIO_AF10_ETH1;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStructure);

  /* Configure PC2 - 5. ETH1_RXD1 */
  GPIO_InitStructure.Pin = GPIO_PIN_2;
  GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStructure.Pull = GPIO_NOPULL;
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStructure.Alternate = GPIO_AF10_ETH1;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);

  /* Configure PA11 - 6. ETH1_CRS_DV */
  GPIO_InitStructure.Pin = GPIO_PIN_11;
  GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStructure.Pull = GPIO_NOPULL;
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStructure.Alternate = GPIO_AF10_ETH1;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Configure PF0 - 7. ETH1_MDC */
  GPIO_InitStructure.Pin = GPIO_PIN_0;
  GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStructure.Pull = GPIO_NOPULL;
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_MEDIUM;
  GPIO_InitStructure.Alternate = GPIO_AF10_ETH1;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStructure);

  /* Configure PF2 - 8. ETH1_MDIO */
  GPIO_InitStructure.Pin =  GPIO_PIN_2;
  GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStructure.Pull = GPIO_NOPULL;
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStructure.Alternate = GPIO_AF10_ETH1;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStructure);

  /* Configure PF8 - 11. ETH1_CLK */
  GPIO_InitStructure.Pin = GPIO_PIN_8;
  GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStructure.Pull = GPIO_NOPULL;
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_MEDIUM;
  GPIO_InitStructure.Alternate = GPIO_AF9_ETH1;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStructure);

  /* ------------- GPIO-only Pins -------------- */
  /* Configure 9. ETH1_NRST pin */
  GPIO_InitStructure.Pin = GPIO_PIN_11;
  GPIO_InitStructure.Pull = GPIO_NOPULL ;
  GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
  HAL_GPIO_Init(GPIOH, &GPIO_InitStructure);
  HAL_GPIO_WritePin(GPIOH, GPIO_PIN_11, GPIO_PIN_SET);
  HAL_Delay(100);

  /* Enable the Ethernet global Interrupt */
  HAL_NVIC_SetPriority(ETH1_SBD_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(ETH1_SBD_IRQn);
}

/**
 * @brief  DeInitializes ETH MSP.
 * @param  heth: pointer to a ETH_HandleTypeDef structure that contains
 *         the configuration information for ETHERNET module
 * @retval None
 */
void HAL_ETH_MspDeInit(ETH_HandleTypeDef *heth)
{
  /* Disable Ethernet clocks */
  __HAL_RCC_ETH1_CLK_DISABLE();
  __HAL_RCC_ETH1MAC_CLK_DISABLE();
  __HAL_RCC_ETH1TX_CLK_DISABLE();
  __HAL_RCC_ETH1RX_CLK_DISABLE();

  /* Disable IRQ */
  HAL_NVIC_DisableIRQ(ETH1_SBD_IRQn);

#if defined(USE_DEVELOPMENT_MODE)
  /* Disable GPIOA clock */
  if (ResMgr_Release(RESMGR_RESOURCE_RIF_RCC, RESMGR_RCC_RESOURCE(90)) == RESMGR_STATUS_ACCESS_OK)
  {
    __HAL_RCC_GPIOA_CLK_DISABLE();
  }
  /* Disable GPIOC clock */
  if (ResMgr_Release(RESMGR_RESOURCE_RIF_RCC, RESMGR_RCC_RESOURCE(92)) == RESMGR_STATUS_ACCESS_OK)
  {
    __HAL_RCC_GPIOC_CLK_DISABLE();
  }
  /* Disable GPIOF clock */
  if (ResMgr_Release(RESMGR_RESOURCE_RIF_RCC, RESMGR_RCC_RESOURCE(95)) == RESMGR_STATUS_ACCESS_OK)
  {
    __HAL_RCC_GPIOF_CLK_DISABLE();
  }
  /* Disable GPIOH clock */
  if (ResMgr_Release(RESMGR_RESOURCE_RIF_RCC, RESMGR_RCC_RESOURCE(97)) == RESMGR_STATUS_ACCESS_OK)
  {
    __HAL_RCC_GPIOH_CLK_DISABLE();
  }
#endif

  /* Release ETH1 using Resource manager */
  if (ResMgr_Release(RESMGR_RESOURCE_RIFSC, STM32MP21_RIFSC_ETH1_ID) != RESMGR_STATUS_ACCESS_OK){
    Error_Handler();
  }

  /* Release GPIOA13 ETH1_TX_EN using Resource manager */
  if (ResMgr_Release(RESMGR_RESOURCE_RIF_GPIOA, RESMGR_GPIO_PIN(13))  != RESMGR_STATUS_ACCESS_OK){
    Error_Handler();
  }

  /* Release GPIOA15 ETH1_TXD0 using Resource manager */
  if (ResMgr_Release(RESMGR_RESOURCE_RIF_GPIOA, RESMGR_GPIO_PIN(15))  != RESMGR_STATUS_ACCESS_OK){
    Error_Handler();
  }

  /* Release GPIOC1 ETH1_TXD1 using Resource manager */
  if (ResMgr_Release(RESMGR_RESOURCE_RIF_GPIOC, RESMGR_GPIO_PIN(1)) != RESMGR_STATUS_ACCESS_OK){
    Error_Handler();
  }

  /* Release GPIOF1 ETH1_RXD0 using Resource manager */
  if (ResMgr_Release(RESMGR_RESOURCE_RIF_GPIOF, RESMGR_GPIO_PIN(1)) != RESMGR_STATUS_ACCESS_OK){
    Error_Handler();
  }

  /* Release GPIOC2 ETH1_RXD1 using Resource manager */
  if (ResMgr_Release(RESMGR_RESOURCE_RIF_GPIOC, RESMGR_GPIO_PIN(2)) != RESMGR_STATUS_ACCESS_OK){
    Error_Handler();
  }

  /* Release GPIOA11 ETH1_CRS_DV using Resource manager */
  if (ResMgr_Release(RESMGR_RESOURCE_RIF_GPIOA, RESMGR_GPIO_PIN(11)) != RESMGR_STATUS_ACCESS_OK){
    Error_Handler();
  }

  /* Release GPIOF0 ETH1_MDC using Resource manager */
  if (ResMgr_Release(RESMGR_RESOURCE_RIF_GPIOF, RESMGR_GPIO_PIN(0)) != RESMGR_STATUS_ACCESS_OK){
    Error_Handler();
  }

  /* Release GPIOF2 ETH1_MDIO using Resource manager */
  if (ResMgr_Release(RESMGR_RESOURCE_RIF_GPIOF, RESMGR_GPIO_PIN(2)) != RESMGR_STATUS_ACCESS_OK){
    Error_Handler();
  }

  /* Release GPIOH11 ETH1_NRST using Resource manager */
  if (ResMgr_Release(RESMGR_RESOURCE_RIF_GPIOH, RESMGR_GPIO_PIN(11)) != RESMGR_STATUS_ACCESS_OK){
    Error_Handler();
  }

  /* Release GPIOA12 ETH1_MDINT using Resource manager */
  if (ResMgr_Release(RESMGR_RESOURCE_RIF_GPIOA, RESMGR_GPIO_PIN(12))  != RESMGR_STATUS_ACCESS_OK){
    Error_Handler();
  }

  /* Release GPIOF8 ETH1_CLK using Resource manager */
  if (ResMgr_Release(RESMGR_RESOURCE_RIF_GPIOF, RESMGR_GPIO_PIN(8)) != RESMGR_STATUS_ACCESS_OK){
    Error_Handler();
  }

}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

