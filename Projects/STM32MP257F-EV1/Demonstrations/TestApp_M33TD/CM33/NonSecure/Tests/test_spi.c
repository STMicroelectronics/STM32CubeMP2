/**
  ******************************************************************************
  * @file    test_spi.c
  * @author  MCD Application Team
  * @brief   SPI test routines.
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

  /* Includes ------------------------------------------------------------------*/
/**
  * @brief Includes required for SPI test routines.
  */
#include "test_spi.h"
#include "app_freertos.h"

#ifdef ENABLE_SPI_TEST  // Conditionally compile the SPI test code

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define BUFFERSIZE                       (COUNTOF(aSpiTxBuffer) - 1)

#define COUNTOF(__BUFFER__)   (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))

enum {
  TRANSFER_WAIT,
  TRANSFER_COMPLETE,
  TRANSFER_ERROR
};

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* SPI handler declaration */
SPI_HandleTypeDef SpiHandle;

/* Buffer used for transmission */
const uint8_t aSpiTxBuffer[] = "****SPI - Two Boards communication based on DMA **** SPI Message ********* SPI Message *********";

/* Buffer used for reception */
#define BUFFER_ALIGNED_SIZE (((BUFFERSIZE+31)/32)*32)
ALIGN_32BYTES(uint8_t aSpiRxBuffer[BUFFER_ALIGNED_SIZE]);

/* transfer state */
__IO uint32_t wTransferState = TRANSFER_WAIT;

/* USER CODE BEGIN PV */
static DMA_HandleTypeDef hdma_tx;
static DMA_HandleTypeDef hdma_rx;
/* USER CODE END PV */

/* Exported functions --------------------------------------------------------*/
static uint16_t Buffercmp(uint8_t *pBuffer1, uint8_t *pBuffer2, uint16_t BufferLength);
/**
  * @brief  Initialize the SPI peripheral and configure the test environment.
  * @retval TEST_STATUS: TEST_OK if success, TEST_FAIL otherwise.
  */
TEST_STATUS Test_SPI_Init(void)
{
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
	      return TEST_FAIL; // Replaced Error_Handler()
	    }

	    __HAL_LINKDMA(&SpiHandle, hdmatx, hdma_tx);

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
	      return TEST_FAIL; // Replaced Error_Handler()
	    }

	    __HAL_LINKDMA(&SpiHandle, hdmarx, hdma_rx);

	    /* SPI3 interrupt Init */
	    HAL_NVIC_SetPriority(SPI3_IRQn, DEFAULT_IRQ_PRIO, 0);
	    HAL_NVIC_EnableIRQ(SPI3_IRQn);

	    /* NVIC configuration for DMA transfer complete interrupt */
	    HAL_NVIC_SetPriority(HPDMA3_Channel2_IRQn, 0x00, 0);
	    HAL_NVIC_EnableIRQ(HPDMA3_Channel2_IRQn);

	    /* NVIC configuration for DMA transfer complete interrupt */
	    HAL_NVIC_SetPriority(HPDMA3_Channel3_IRQn, 0x00, 0);
	    HAL_NVIC_EnableIRQ(HPDMA3_Channel3_IRQn);

	  /*##-1- Configure the SPI peripheral #######################################*/
	  /* Set the SPI parameters */
	  SpiHandle.Instance               = SPI3;
	  SpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
	  SpiHandle.Init.Direction         = SPI_DIRECTION_2LINES;
	  SpiHandle.Init.CLKPhase          = SPI_PHASE_1EDGE;
	  SpiHandle.Init.CLKPolarity       = SPI_POLARITY_LOW;
	  SpiHandle.Init.DataSize          = SPI_DATASIZE_8BIT;
	  SpiHandle.Init.FirstBit          = SPI_FIRSTBIT_MSB;
	  SpiHandle.Init.TIMode            = SPI_TIMODE_DISABLE;
	  SpiHandle.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
	  SpiHandle.Init.FifoThreshold     = SPI_FIFO_THRESHOLD_01DATA;
	  SpiHandle.Init.CRCPolynomial     = 7;
	  SpiHandle.Init.CRCLength         = SPI_CRC_LENGTH_8BIT;
	  SpiHandle.Init.NSS               = SPI_NSS_SOFT;
	  SpiHandle.Init.NSSPMode          = SPI_NSS_PULSE_DISABLE;
	  SpiHandle.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_ENABLE;  /* Recommended setting to avoid glitches */

	  SpiHandle.Init.Mode = SPI_MODE_MASTER;


	  if(HAL_SPI_Init(&SpiHandle) != HAL_OK)
	  {
		/* Initialization Error */
		return TEST_FAIL; // Replaced Error_Handler()
	  }
		GPIO_InitTypeDef GPIO_InitStruct;
			  /**SPI3 GPIO Configuration
		 PB10     ------> SPI3_MISO
		 PB8      ------> SPI3_MOSI
		 PB7      ------> SPI3_SCK
		 PB1      ------> SPI3_NSS
	   */
	    GPIO_InitStruct.Pin = GPIO_PIN_10 | GPIO_PIN_8 | GPIO_PIN_7 | GPIO_PIN_1;
	    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	    GPIO_InitStruct.Alternate = GPIO_AF1_SPI_I2S3;
	    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    return TEST_OK;
}

/**
  * @brief  Run the SPI test routine.
  * @retval TEST_STATUS: TEST_OK if success, TEST_FAIL otherwise.
  */
TEST_STATUS Test_SPI_Run(void)
{

		osDelay(3000);

	  /*##-2- Start the Full Duplex Communication process ########################*/
	  /* While the SPI in TransmitReceive process, user can transmit data through
	     "aSpiTxBuffer" buffer & receive data through "aSpiRxBuffer" */
	  if(HAL_SPI_TransmitReceive_DMA(&SpiHandle, (uint8_t*)aSpiTxBuffer, (uint8_t *)aSpiRxBuffer, BUFFERSIZE) != HAL_OK)
	  {
	    /* Transfer error in transmission process */
	    return TEST_FAIL; // Replaced Error_Handler()
	  }

	  /*##-3- Wait for the end of the transfer ###################################*/
	  /*  Before starting a new communication transfer, you must wait the callback call
	      to get the transfer complete confirmation or an error detection.
	      For simplicity reasons, this example is just waiting till the end of the
	      transfer, but application may perform other tasks while transfer operation
	      is ongoing. */
		uint32_t startTime = osKernelGetTickCount(); // Get the current tick count
		while (wTransferState == TRANSFER_WAIT)
		{
			if ((osKernelGetTickCount() - startTime) >= 1000) // Timeout: 1 second
			{
				return TEST_FAIL;
			}
			osDelay(1); // Allow other tasks to run
		}

	  switch(wTransferState)
	  {
	    case TRANSFER_COMPLETE :
	      /*##-4- Compare the sent and received buffers ##############################*/
	      if(Buffercmp((uint8_t*)aSpiTxBuffer, (uint8_t*)aSpiRxBuffer, BUFFERSIZE))
	      {
	        /* Processing Error */
	        return TEST_FAIL; // Replaced Error_Handler()
	      }
	      break;
	    default :
	      return TEST_FAIL; // Replaced Error_Handler()
	      break;
	  }

    return TEST_OK;
}

/**
  * @brief  De-initialize the SPI peripheral and release resources.
  * @retval TEST_STATUS: TEST_OK if success, TEST_FAIL otherwise.
  */
TEST_STATUS Test_SPI_DeInit(void)
{
	HAL_SPI_DeInit(&SpiHandle);

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
    HAL_DMA_DeInit(SpiHandle.hdmatx);
    HAL_DMA_DeInit(SpiHandle.hdmarx);

    /* SPI3 interrupt DeInit */
    HAL_NVIC_DisableIRQ(SPI3_IRQn);

    return TEST_OK;
}

/**
  * @brief  Compares two buffers.
  * @param  pBuffer1, pBuffer2: buffers to be compared.
  * @param  BufferLength: buffer's length
  * @retval 0  : pBuffer1 identical to pBuffer2
  *         >0 : pBuffer1 differs from pBuffer2
  */
static uint16_t Buffercmp(uint8_t *pBuffer1, uint8_t *pBuffer2, uint16_t BufferLength)
{
  while (BufferLength--)
  {
    if ((*pBuffer1) != *pBuffer2)
    {
      return BufferLength;
    }
    pBuffer1++;
    pBuffer2++;
  }

  return 0;
}

/**
  * @brief  TxRx Transfer completed callback.
  * @param  hspi: SPI handle
  * @note   This example shows a simple way to report end of DMA TxRx transfer, and
  *         you can add your own implementation.
  * @retval None
  */
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
  wTransferState = TRANSFER_COMPLETE;
}

/**
  * @brief  SPI error callbacks.
  * @param  hspi: SPI handle
  * @note   This example shows a simple way to report transfer error, and you can
  *         add your own implementation.
  * @retval None
  */
void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
  wTransferState = TRANSFER_ERROR;
}

/**
  * @brief  Acquire SPI resources.
  * @retval TEST_STATUS: TEST_OK if success, TEST_FAIL otherwise.
  */
TEST_STATUS Test_SPI_ResourceAcquire(void)
{
	/* HPDMA3 clock enable */
	if(ResMgr_Request(RESMGR_RESOURCE_RIF_RCC, RESMGR_RCC_RESOURCE(85)) == RESMGR_STATUS_ACCESS_OK)
	{
		__HAL_RCC_HPDMA3_CLK_ENABLE();
	}

	  /* Acquire SPI3 using Resource manager */
	  if (RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIFSC, STM32MP25_RIFSC_SPI3_ID))
	  {
		  return TEST_FAIL;
	  }

	  /* Acquire HPDMA3 Channel 2 using Resource manager */
	  if (RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIF_HPDMA3, RESMGR_HPDMA_CHANNEL(2)))
	  {
		  return TEST_FAIL;
	  }

	  /* Acquire HPDMA3 Channel 3 using Resource manager */
	  if (RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIF_HPDMA3, RESMGR_HPDMA_CHANNEL(3)))
	  {
		  return TEST_FAIL;
	  }

	  /* Acquire GPIOB10 using Resource manager */
	  if (RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIF_GPIOB, RESMGR_GPIO_PIN(10)))
	  {
		  return TEST_FAIL;
	  }

	  /* Acquire GPIOB8 using Resource manager */
	  if (RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIF_GPIOB, RESMGR_GPIO_PIN(8)))
	  {
		  return TEST_FAIL;
	  }

	  /* Acquire GPIOB7 using Resource manager */
	  if (RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIF_GPIOB, RESMGR_GPIO_PIN(7)))
	  {
		  return TEST_FAIL;
	  }

	  /* Acquire GPIOB1 using Resource manager */
	  if (RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIF_GPIOB, RESMGR_GPIO_PIN(1)))
	  {
		  return TEST_FAIL;
	  }

	  /* Enable GPIOs power supplies */
	  if (RESMGR_STATUS_ACCESS_OK == ResMgr_Request(RESMGR_RESOURCE_RIF_RCC, RESMGR_RCC_RESOURCE(91)))
	  {
		  __HAL_RCC_GPIOB_CLK_ENABLE();
	  }


    return TEST_OK;
}

/**
  * @brief  Release SPI resources.
  * @retval TEST_STATUS: TEST_OK if success, TEST_FAIL otherwise.
  */
TEST_STATUS Test_SPI_ResourceRelease(void)
{
	/* Release HPDMA3 Channel 2 using Resource manager */
    ResMgr_Release(RESMGR_RESOURCE_RIF_HPDMA3, RESMGR_HPDMA_CHANNEL(2));

    /* Release HPDMA3 Channel 3 using Resource manager */
	ResMgr_Release(RESMGR_RESOURCE_RIF_HPDMA3, RESMGR_HPDMA_CHANNEL(3));

	/* Release SPI3 using Resource manager */
	ResMgr_Release(RESMGR_RESOURCE_RIFSC, STM32MP25_RIFSC_SPI8_ID);

	/* Release GPIOB10 using Resource manager */
	ResMgr_Release(RESMGR_RESOURCE_RIF_GPIOB, RESMGR_GPIO_PIN(10));

	/* Release GPIOB8 using Resource manager */
	ResMgr_Release(RESMGR_RESOURCE_RIF_GPIOB, RESMGR_GPIO_PIN(8));

	/* Release GPIOB7 using Resource manager */
	ResMgr_Release(RESMGR_RESOURCE_RIF_GPIOB, RESMGR_GPIO_PIN(7));

	/* Release GPIOB1 using Resource manager */
	ResMgr_Release(RESMGR_RESOURCE_RIF_GPIOB, RESMGR_GPIO_PIN(1));


    return TEST_OK;
}

#endif /* ENABLE_SPI_TEST */  // End of conditional compilation

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/