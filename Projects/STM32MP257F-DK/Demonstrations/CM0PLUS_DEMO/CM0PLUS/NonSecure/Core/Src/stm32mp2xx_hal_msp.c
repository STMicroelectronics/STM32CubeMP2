/**
  ******************************************************************************
  * @file    stm32mp2xx_hal_msp.c
  * @author  MCD Application Team
  * @brief   HAL MSP module.
  *          This file template is located in the HAL folder and should be copied
  *          to the user folder.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32mp2xx_hal.h"

/** @addtogroup STM32MP2xx_HAL_Driver
  * @{
  */

/** @defgroup HAL_MSP HAL MSP module driver
  * @brief HAL MSP module.
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup HAL_MSP_Private_Functions
  * @{
  */

/**
  * @brief  Initialize the Global MSP.
  * @retval None
  */
/*
void HAL_MspInit(void)
{
}
*/

/**
  * @brief  DeInitialize the Global MSP.
  * @retval None
  */
/*
void HAL_MspDeInit(void)
{
}
*/

/**
  * @brief  Initialize the PPP MSP.
  * @retval None
  */
/*
void HAL_PPP_MspInit(void)
{
}
*/

/**
  * @brief  DeInitialize the PPP MSP.
  * @retval None
  */
/*
void HAL_PPP_MspDeInit(void)
{
}
*/


/**
 * @brief IPCC MSP Initialization
 * This function configures the hardware resources used in this example
 * @param hipcc: IPCC handle pointer
 * @retval None
 */
void HAL_IPCC_MspInit(IPCC_HandleTypeDef* hipcc)
{

	if(hipcc->Instance==IPCC2)
	{
		/* USER CODE BEGIN IPCC_MspInit 0 */

		/* USER CODE END IPCC_MspInit 0 */

		/* USER CODE BEGIN IPCC_MspInit 1 */

		/* USER CODE END IPCC_MspInit 1 */
	}

}

/**
* @brief IPCC MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hipcc: IPCC handle pointer
* @retval None
*/

void HAL_IPCC_MspDeInit(IPCC_HandleTypeDef* hipcc)
{

  if(hipcc->Instance==IPCC2)
  {
  	/* USER CODE BEGIN IPCC_MspDeInit 0 */

  	/* USER CODE END IPCC_MspDeInit 0 */

  	/* USER CODE BEGIN IPCC_MspDeInit 1 */

  	/* USER CODE END IPCC_MspDeInit 1 */
  }

}


/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
