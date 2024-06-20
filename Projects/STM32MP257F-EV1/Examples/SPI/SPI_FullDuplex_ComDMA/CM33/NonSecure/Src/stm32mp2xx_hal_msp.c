/**
  ******************************************************************************
  * @file    stm32mp2xx_hal_msp.c
  * @author  MCD Application Team
  * @brief   This file provides code for the MSP Initialization 
  *                      and de-Initialization codes.
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
#include "res_mgr.h"

static DMA_HandleTypeDef hdma_tx;
static DMA_HandleTypeDef hdma_rx;

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

    __HAL_RCC_SPI3_CLK_ENABLE();
    __HAL_RCC_SPI3_FORCE_RESET();
    __HAL_RCC_SPI3_RELEASE_RESET();

    /* SPI3 DMA Init */
    /* SPI3_TX Init */
    hdma_tx.Instance = HPDMA3_Channel2;
    hdma_tx.Init.Request = HPDMA_REQUEST_SPI3_TX;
    hdma_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_tx.Init.SrcInc = DMA_SINC_INCREMENTED;
    hdma_tx.Init.DestInc = DMA_DINC_FIXED;
    hdma_tx.Init.SrcDataWidth = DMA_SRC_DATAWIDTH_BYTE;
    hdma_tx.Init.DestDataWidth = DMA_DEST_DATAWIDTH_BYTE;
    hdma_tx.Init.SrcBurstLength = 1;
    hdma_tx.Init.DestBurstLength = 1;
    hdma_tx.Init.Priority = DMA_HIGH_PRIORITY;
    hdma_tx.Init.TransferEventMode = DMA_TCEM_BLOCK_TRANSFER;

    /* Deinitialize the Stream for new transfer */
    HAL_DMA_DeInit(&hdma_tx);

    /* Configure the DMA Stream */
    if (HAL_OK != HAL_DMA_Init(&hdma_tx))
    {
      Error_Handler();
    }

    __HAL_LINKDMA(hspi, hdmatx, hdma_tx);

    /* SPI3_RX Init */
    hdma_rx.Instance = HPDMA3_Channel3;
    hdma_rx.Init.Request = HPDMA_REQUEST_SPI3_RX;
    hdma_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_rx.Init.SrcInc = DMA_SINC_FIXED;
    hdma_rx.Init.DestInc = DMA_DINC_INCREMENTED;
    hdma_rx.Init.SrcDataWidth = DMA_SRC_DATAWIDTH_BYTE;
    hdma_rx.Init.DestDataWidth = DMA_DEST_DATAWIDTH_BYTE;
    hdma_rx.Init.SrcBurstLength = 1;
    hdma_rx.Init.DestBurstLength = 1;
    hdma_rx.Init.Priority = DMA_HIGH_PRIORITY;
    hdma_rx.Init.TransferEventMode = DMA_TCEM_BLOCK_TRANSFER;

    HAL_DMA_DeInit(&hdma_rx);
    if (HAL_OK != HAL_DMA_Init(&hdma_rx))
    {
      Error_Handler();
    }

    __HAL_LINKDMA(hspi, hdmarx, hdma_rx);

    /* SPI3 interrupt Init */
    HAL_NVIC_SetPriority(SPI3_IRQn, DEFAULT_IRQ_PRIO, 0);
    HAL_NVIC_EnableIRQ(SPI3_IRQn);

    /* NVIC configuration for DMA transfer complete interrupt */
    HAL_NVIC_SetPriority(HPDMA3_Channel2_IRQn, 0x00, 0);
    HAL_NVIC_EnableIRQ(HPDMA3_Channel2_IRQn);

    /* NVIC configuration for DMA transfer complete interrupt */
    HAL_NVIC_SetPriority(HPDMA3_Channel3_IRQn, 0x00, 0);
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
	if(SPI3 == hspi->Instance)
	{
	    GPIO_InitStruct.Pin = GPIO_PIN_10 | GPIO_PIN_8 | GPIO_PIN_7 | GPIO_PIN_1;
	    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
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
    HAL_GPIO_DeInit(GPIOZ, GPIO_PIN_10 | GPIO_PIN_8 | GPIO_PIN_7 | GPIO_PIN_1);

    /* SPI3 DMA DeInit */
    HAL_DMA_DeInit(hspi->hdmatx);
    HAL_DMA_DeInit(hspi->hdmarx);

    /* SPI3 interrupt DeInit */
    HAL_NVIC_DisableIRQ(SPI3_IRQn);

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

