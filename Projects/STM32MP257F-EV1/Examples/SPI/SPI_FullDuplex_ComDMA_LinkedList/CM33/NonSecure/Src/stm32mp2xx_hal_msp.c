/**
  ******************************************************************************
  * @file    stm32mp2xx_hal_msp.c
  * @author  MCD Application Team
  * @brief   This file provides code for the MSP Initialization 
  *                      and de-Initialization codes.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "res_mgr.h"

/* Linked-list DMA TX handle is owned/configured by main.c */
extern DMA_HandleTypeDef hpdma_spi3_tx;

/* Linked-list DMA RX handle is owned/configured by main.c */
extern DMA_HandleTypeDef hpdma_spi3_rx;

extern void Error_Handler(void);

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */
/**
  * Initializes the Global MSP.
  */
void HAL_MspInit(void)
{
  /* USER CODE BEGIN MspInit 0 */

  /* USER CODE END MspInit 0 */

  /* USER CODE BEGIN MspInit 1 */

  /* USER CODE END MspInit 1 */
}

void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
{
  if(SPI3 == hspi->Instance)
  {
    /* USER CODE BEGIN SPI3_MspInit 0 */
    /* USER CODE END SPI3_MspInit 0 */

    if (IS_DEVELOPER_BOOT_MODE())
    {
      HAL_PWREx_EnableSupply(PWR_PVM_VDDIO4);
    }

    __HAL_RCC_SPI3_CLK_ENABLE();
    __HAL_RCC_SPI3_FORCE_RESET();
    __HAL_RCC_SPI3_RELEASE_RESET();

    /* Linked-List DMA:
     * - HPDMA3 Channel2 (TX) and Channel3 (RX) are used in Linked-List mode.
     * - DMA channels are initialized/configured in main.c via HAL_DMAEx_List_* APIs.
     * - It only links the handles and enable IRQs here.
     */
    __HAL_LINKDMA(hspi, hdmatx, hpdma_spi3_tx);
    __HAL_LINKDMA(hspi, hdmarx, hpdma_spi3_rx);

    /* SPI3 interrupt Init */
    HAL_NVIC_SetPriority(SPI3_IRQn, DEFAULT_IRQ_PRIO, 0);
    HAL_NVIC_EnableIRQ(SPI3_IRQn);

    /* NVIC configuration for DMA transfer complete interrupt */
    HAL_NVIC_SetPriority(HPDMA3_Channel2_IRQn, DEFAULT_IRQ_PRIO, 0);
    HAL_NVIC_EnableIRQ(HPDMA3_Channel2_IRQn);

    HAL_NVIC_SetPriority(HPDMA3_Channel3_IRQn, DEFAULT_IRQ_PRIO, 0);
    HAL_NVIC_EnableIRQ(HPDMA3_Channel3_IRQn);
    /* USER CODE BEGIN SPI3_MspInit 1 */
    /* USER CODE END SPI3_MspInit 1 */
  }

}

void HAL_SPI_MspPostInit(SPI_HandleTypeDef* hspi)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  /* USER CODE BEGIN SPI_MspPostInit 0 */

  /* USER CODE END SPI_MspPostInit 0 */
  if (SPI3 == hspi->Instance)
  {
    /* GPIOB clock gate access: on some boot flows A35 may have already enabled
     * GPIO clocks. In that case ResMgr can deny the RCC resource, but we can
     * still proceed safely if the clock is already ON.
     */
    int32_t rcc_gpiob = (int32_t)ResMgr_Request(RESMGR_RESOURCE_RIF_RCC, RESMGR_RCC_RESOURCE(91));
    if (rcc_gpiob == (int32_t)RESMGR_STATUS_ACCESS_OK)
    {
      __HAL_RCC_GPIOB_CLK_ENABLE();
    }
    else
    {
      if (!__HAL_RCC_GPIOB_IS_CLK_ENABLED())
      {
        Error_Handler();
      }
    }

    GPIO_InitStruct.Pin       = GPIO_PIN_10 | GPIO_PIN_8 | GPIO_PIN_7 | GPIO_PIN_1;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF1_SPI_I2S3;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  }
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi)
{
  /* USER CODE BEGIN SPI3_MspDeInit 0 */

  /* USER CODE END SPI3_MspDeInit 0 */
  if(SPI3 == hspi->Instance)
  {
    /* Peripheral clock disable */
    __HAL_RCC_SPI3_CLK_DISABLE();

    /**SPI3 GPIO Configuration
		PB10     ------> SPI3_MISO
		PB8      ------> SPI3_MOSI
		PB7      ------> SPI3_SCK
		PB1      ------> SPI3_NSS
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_10 | GPIO_PIN_8 | GPIO_PIN_7 | GPIO_PIN_1);

    /* SPI3 DMA DeInit */
    if (hspi->hdmatx != NULL)
    {
      HAL_DMA_DeInit(hspi->hdmatx);
    }

    if (hspi->hdmarx != NULL)
    {
      HAL_DMA_DeInit(hspi->hdmarx);
    }

    /* SPI3 interrupt DeInit */
    HAL_NVIC_DisableIRQ(SPI3_IRQn);

    /* DMA interrupts DeInit */
    HAL_NVIC_DisableIRQ(HPDMA3_Channel2_IRQn);
    HAL_NVIC_DisableIRQ(HPDMA3_Channel3_IRQn);

    /* USER CODE BEGIN SPI3_MspDeInit 1 */
    /* USER CODE END SPI3_MspDeInit 1 */
  }

}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/**
  * @}
  */

/**
  * @}
  */

