/**
  ********************************************************************************
  * @file    SPI/SPI_FullDuplex_ComDMA_LinkedList/Src/main.c
  * @author  MCD Application Team
  * @brief   This sample code shows how to use STM32MP2xx SPI HAL API to transmit
  *          and receive a data buffer with a communication process based on
  *          DMA transfer.
  *          The communication is done using 2 Boards.
  ********************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ********************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "copro_sync.h"
#include "res_mgr.h"

/** @addtogroup STM32MP2xx_HAL_Examples
  * @{
  */

/** @addtogroup SPI_FullDuplex_ComDMA_LinkedList
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
enum {
  TRANSFER_WAIT,
  TRANSFER_COMPLETE,
  TRANSFER_ERROR
};

/* Private macro -------------------------------------------------------------*/
#ifdef ICACHE_DCACHE_USE
#define ICACHE_DCACHE_ENABLE
#endif /* ICACHE_DCACHE_ENABLE */

#define SPI3_TX_BUFFER_COUNT   (3U)
#define SPI3_RX_BUFFER_COUNT   (SPI3_TX_BUFFER_COUNT)
/* Uncomment this line to build the Master board firmware.*/
#define MASTER_BOARD

/* Private variables ---------------------------------------------------------*/
/* IPCC handler */
IPCC_HandleTypeDef   hipcc1;

/* SPI handler */
SPI_HandleTypeDef    SpiHandle;

/* LL DMA handle for SPI3 TX (HPDMA3 Channel 2) */
DMA_HandleTypeDef    hpdma_spi3_tx;

/* LL DMA handle for SPI3 RX (HPDMA3 Channel 3) */
DMA_HandleTypeDef    hpdma_spi3_rx;

/* Linked-list queues and nodes */
ALIGN_32BYTES(DMA_QListTypeDef SPI3_Tx_Queue);
ALIGN_32BYTES(DMA_NodeTypeDef  SPI3_Tx_Nodes[SPI3_TX_BUFFER_COUNT]);

ALIGN_32BYTES(DMA_QListTypeDef SPI3_Rx_Queue);
ALIGN_32BYTES(DMA_NodeTypeDef  SPI3_Rx_Nodes[SPI3_RX_BUFFER_COUNT]);

/* TX buffers (aligned). Size is deduced from the string initializer.*/
ALIGN_32BYTES(static uint8_t aTxBuffer0[]) =
  "0*****SPI3 LINKED LIST DMA multi-buffer *****0";
ALIGN_32BYTES(static uint8_t aTxBuffer1[]) =
  "0*****SPI3 LINKED LIST DMA multi-buffer *****0";
ALIGN_32BYTES(static uint8_t aTxBuffer2[]) =
  "0*****SPI3 LINKED LIST DMA multi-buffer *****0";

static uint8_t *const aTxBuffers[SPI3_TX_BUFFER_COUNT] =
{
  aTxBuffer0,
  aTxBuffer1,
  aTxBuffer2,
};

static const uint32_t aTxBufferSizes[SPI3_TX_BUFFER_COUNT] =
{
  (uint32_t)(sizeof(aTxBuffer0) - 1u),
  (uint32_t)(sizeof(aTxBuffer1) - 1u),
  (uint32_t)(sizeof(aTxBuffer2) - 1u),
};

/* RX buffers (aligned). */
ALIGN_32BYTES(static uint8_t aRxBuffer0[sizeof(aTxBuffer0) - 1u]);
ALIGN_32BYTES(static uint8_t aRxBuffer1[sizeof(aTxBuffer1) - 1u]);
ALIGN_32BYTES(static uint8_t aRxBuffer2[sizeof(aTxBuffer2) - 1u]);

static uint8_t *const aRxBuffers[SPI3_RX_BUFFER_COUNT] =
{
  aRxBuffer0,
  aRxBuffer1,
  aRxBuffer2,
};

static const uint32_t aRxBufferSizes[SPI3_RX_BUFFER_COUNT] =
{
  (uint32_t)sizeof(aRxBuffer0),
  (uint32_t)sizeof(aRxBuffer1),
  (uint32_t)sizeof(aRxBuffer2),
};

/* transfer state */
__IO uint32_t wTransferState = TRANSFER_WAIT;

__IO uint32_t txComplete = 0;
__IO uint32_t rxComplete = 0;

#ifdef DEBUG
volatile uint8_t debug = 1;
#endif /* DEBUG */

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void MX_DMA_Init(void);
static void MX_SPI3_Init(void);
static void MX_IPCC_Init(void);
#ifdef ICACHE_DCACHE_ENABLE
static void MX_ICACHE_Init(void);
static void MX_DCACHE_Init(void);
static void MPU_Config(void);
DCACHE_HandleTypeDef hdcache = {0};

#define DCACHE_LINE_SIZE_BYTES (32U)
#define DCACHE_SIZE_ALIGN_UP(size_) \
  (((uint32_t)(size_) + (DCACHE_LINE_SIZE_BYTES - 1U)) & ~(DCACHE_LINE_SIZE_BYTES - 1U))
#endif /* ICACHE_DCACHE_ENABLE */
static HAL_StatusTypeDef SPI3_Transmit_LinkedList_Config(void);
static HAL_StatusTypeDef SPI3_Receive_LinkedList_Config(void);
static void DMA_TxTransferComplete(DMA_HandleTypeDef *hdma);
static void DMA_TxTransferError(DMA_HandleTypeDef *hdma);
static void DMA_RxTransferComplete(DMA_HandleTypeDef *hdma);
static void DMA_RxTransferError(DMA_HandleTypeDef *hdma);
static void SPI3_FullDuplexConfig_DMA(uint32_t total_size);
static void SPI3_CloseCommunication_DMA_LinkedList(void);
#ifdef MASTER_BOARD
static void SPI3_StartCommunication_DMA_LinkedList(void);
#endif
static uint16_t Buffercmp(const uint8_t *pBuffer1, const uint8_t *pBuffer2, uint16_t buffer_length);

/* Private functions ---------------------------------------------------------*/


/**
  * @brief  The application entry point.
  * @param  None
  * @retval int
  */
int main(void)
{
  /* MCU Configuration--------------------------------------------------------*/

  /* STM32MP2xx HAL library initialization:
       - Systick timer is configured by default as source of time base, but user
             can eventually implement his proper time base source (a general purpose
             timer for example or other time source), keeping in mind that Time base
             duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
             handled in milliseconds basis.
       - Set NVIC Group Priority to 4
       - Low Level Initialization
     */
#ifdef DEBUG
  while (debug);
#endif /* DEBUG */

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

#ifdef ICACHE_DCACHE_ENABLE
  MPU_Config();
  MX_DCACHE_Init();
  MX_ICACHE_Init();
#endif /* ICACHE_DCACHE_ENABLE */

#if defined(__VALID_OUTPUT_TERMINAL_IO__) && defined (__GNUC__)
  initialise_monitor_handles();
#elif defined(__VALID_OUTPUT_UART_IO__)
  COM_InitTypeDef COM_Conf;
  COM_Conf.BaudRate   = 115200;
  COM_Conf.HwFlowCtl  = COM_HWCONTROL_NONE;
  COM_Conf.Parity     = COM_PARITY_NONE;
  COM_Conf.StopBits   = COM_STOPBITS_1;
  COM_Conf.WordLength = COM_WORDLENGTH_8B;
  BSP_COM_Init(COM_VCP_CM33, &COM_Conf);
#endif  /* __VALID_OUTPUT_TERMINAL_IO__ or __VALID_OUTPUT_UART_IO__ */

  if (IS_DEVELOPER_BOOT_MODE())
  {
    /* Configure the system clock */
    SystemClock_Config();
  }

#ifdef MASTER_BOARD
  printf("\r\nLog : SPI3 Linked-List TX/RX example running (on SPI Master Board)\r\n");
#else
  printf("\r\nLog : SPI3 Linked-List TX/RX example running (on SPI Slave Board)\r\n");
#endif /* MASTER_BOARD */

  /* Configure LED3 */
  BSP_LED_Init(LED3);

  if (!IS_DEVELOPER_BOOT_MODE())
  {
    /* IPCC initialization */
    MX_IPCC_Init();

    /*Corpo Sync Initialization*/
    CoproSync_Init();
  }

  /* Initialize all configured peripherals */
  MX_DMA_Init();
  MX_SPI3_Init();

  /* Configure SPI3 TX Linked-List on HPDMA3 Channel 2 */
  if (SPI3_Transmit_LinkedList_Config() != HAL_OK)
  {
    Error_Handler();
  }
  /* Configure SPI3 RX Linked-List on HPDMA3 Channel 3 */
  if (SPI3_Receive_LinkedList_Config() != HAL_OK)
  {
    Error_Handler();
  }

  /* Link Linked-List queue to hpdma_spi3_tx */
  if (HAL_DMAEx_List_LinkQ(&hpdma_spi3_tx, &SPI3_Tx_Queue) != HAL_OK)
  {
    Error_Handler();
  }

  /* Link Linked-List queue to hpdma_spi3_rx */
  if (HAL_DMAEx_List_LinkQ(&hpdma_spi3_rx, &SPI3_Rx_Queue) != HAL_OK)
  {
    Error_Handler();
  }

  /* Register DMA callbacks */
  HAL_DMA_RegisterCallback(&hpdma_spi3_tx, HAL_DMA_XFER_CPLT_CB_ID,  DMA_TxTransferComplete);
  HAL_DMA_RegisterCallback(&hpdma_spi3_tx, HAL_DMA_XFER_ERROR_CB_ID, DMA_TxTransferError);

  HAL_DMA_RegisterCallback(&hpdma_spi3_rx, HAL_DMA_XFER_CPLT_CB_ID,  DMA_RxTransferComplete);
  HAL_DMA_RegisterCallback(&hpdma_spi3_rx, HAL_DMA_XFER_ERROR_CB_ID, DMA_RxTransferError);

#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
  if (IS_DEVELOPER_BOOT_MODE())
  {
    HAL_DMA_ConfigChannelAttributes(SpiHandle.hdmatx,
                                   (DMA_CHANNEL_PRIV | DMA_CHANNEL_SEC |
                                    DMA_CHANNEL_DEST_NSEC | DMA_CHANNEL_SRC_SEC |
                                    DMA_CHANNEL_CID_STATIC_2));

    HAL_DMA_ConfigChannelAttributes(SpiHandle.hdmarx,
                                   (DMA_CHANNEL_PRIV | DMA_CHANNEL_SEC |
                                    DMA_CHANNEL_DEST_SEC | DMA_CHANNEL_SRC_NSEC |
                                    DMA_CHANNEL_CID_STATIC_2));
  }
#endif /* __ARM_FEATURE_CMSE */

#ifdef ICACHE_DCACHE_ENABLE
  /* Clean cache for TX buffers before DMA transfer */
  for (uint32_t i = 0U; i < SPI3_TX_BUFFER_COUNT; i++)
  {
    HAL_DCACHE_CleanByAddr(&hdcache,
                           (uint32_t *)aTxBuffers[i],
                           (int32_t)DCACHE_SIZE_ALIGN_UP(aTxBufferSizes[i]));
  }

  /* Invalidate RX buffers before DMA writes into them */
  for (uint32_t i = 0U; i < SPI3_RX_BUFFER_COUNT; i++)
  {
    HAL_DCACHE_InvalidateByAddr(&hdcache,
                                (uint32_t *)aRxBuffers[i],
                                (int32_t)DCACHE_SIZE_ALIGN_UP(aRxBufferSizes[i]));
  }

  __DSB();
#endif /* ICACHE_DCACHE_ENABLE */

  /* Total number of bytes to be shifted out on MOSI */
  uint32_t total_size = 0U;
  for (uint32_t i = 0U; i < SPI3_TX_BUFFER_COUNT; i++)
  {
    total_size += aTxBufferSizes[i];
  }

#ifdef MASTER_BOARD
  printf("Log : Press USER2 button to start communication\r\n");

  /* Configure User push-button */
  BSP_PB_Init(BUTTON_USER2, BUTTON_MODE_GPIO);

  /* Wait for User push-button press */
  while (BSP_PB_GetState(BUTTON_USER2) != GPIO_PIN_SET)
  {
  }

  /* Delay to avoid that possible signal rebound is taken as button release */
  HAL_Delay(50);

  /* Wait for User push-button release */
  while (BSP_PB_GetState(BUTTON_USER2) != GPIO_PIN_RESET)
  {
  }
#endif /* MASTER_BOARD */

  printf("Log : Starting SPI3 TX/RX Linked-List transfer\r\n");

#ifdef ICACHE_DCACHE_ENABLE
  /* HPDMA linked-list engine fetches descriptors from memory.
   * With DCache enabled, the node/queue structures must be cleaned so DMA sees
   * the latest descriptor content (links, addresses, sizes, etc.).
   */
  HAL_DCACHE_CleanByAddr(&hdcache,
                         (uint32_t *)&SPI3_Tx_Queue,
                         (int32_t)DCACHE_SIZE_ALIGN_UP(sizeof(SPI3_Tx_Queue)));
  HAL_DCACHE_CleanByAddr(&hdcache,
                         (uint32_t *)&SPI3_Tx_Nodes[0],
                         (int32_t)DCACHE_SIZE_ALIGN_UP(sizeof(SPI3_Tx_Nodes)));

  HAL_DCACHE_CleanByAddr(&hdcache,
                         (uint32_t *)&SPI3_Rx_Queue,
                         (int32_t)DCACHE_SIZE_ALIGN_UP(sizeof(SPI3_Rx_Queue)));
  HAL_DCACHE_CleanByAddr(&hdcache,
                         (uint32_t *)&SPI3_Rx_Nodes[0],
                         (int32_t)DCACHE_SIZE_ALIGN_UP(sizeof(SPI3_Rx_Nodes)));

  __DSB();
#endif /* ICACHE_DCACHE_ENABLE */

  /* Configures SPI3 Total Size and enables TX/RX DMA requests */
  SPI3_FullDuplexConfig_DMA(total_size);

  /* Start linked-list DMA transfers (RX first, then TX) */
  if (HAL_DMAEx_List_Start_IT(&hpdma_spi3_rx) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_DMAEx_List_Start_IT(&hpdma_spi3_tx) != HAL_OK)
  {
    Error_Handler();
  }

#ifdef MASTER_BOARD
  /* Only the master starts the transfer engine (generates SCK). The slave just waits
   * for the master clocking to drive SPI and trigger DMA requests.
   */
  SPI3_StartCommunication_DMA_LinkedList();
#endif

  /* Wait for the end of the transfer */
  while (wTransferState == TRANSFER_WAIT)
  {
  }

  switch (wTransferState)
  {
    case TRANSFER_COMPLETE:
      printf("Log : SPI3 LL TX/RX transfer complete\r\n");

      /* Close communication */
      SPI3_CloseCommunication_DMA_LinkedList();

#ifdef ICACHE_DCACHE_ENABLE
      /* Invalidate RX buffers after DMA so CPU reads fresh data */
  for (uint32_t i = 0U; i < SPI3_RX_BUFFER_COUNT; i++)
      {
    HAL_DCACHE_InvalidateByAddr(&hdcache,
            (uint32_t *)aRxBuffers[i],
            (int32_t)DCACHE_SIZE_ALIGN_UP(aRxBufferSizes[i]));
      }

  __DSB();
#endif /* ICACHE_DCACHE_ENABLE */

      BSP_LED_On(LED3);

      /* Compare TX and RX buffers */
      for (uint32_t i = 0U; i < SPI3_TX_BUFFER_COUNT; i++)
      {
        if (Buffercmp((const uint8_t*)aTxBuffers[i], (const uint8_t*)aRxBuffers[i], (uint16_t)aTxBufferSizes[i]) != 0U)
        {
          printf("Error : SPI example failed (TX-RX data mismatch)\r\n");
          SPI3_CloseCommunication_DMA_LinkedList();
          Error_Handler();
        }
      }

      printf("Log : SPI example working fine\r\n");
      break;
    default:
      SPI3_CloseCommunication_DMA_LinkedList();
      Error_Handler();
      break;
  }
  /* Infinite loop */
  while (1)
  {
    BSP_LED_Toggle(LED3);
    HAL_Delay(500);
  }
}

/**
  * @brief  Configures SPI3 TSIZE and enables TX/RX DMA requests.
  * @param  total_size: Total number of data frames to be transferred.
  * @retval None
  */
static void SPI3_FullDuplexConfig_DMA(uint32_t total_size)
{
  /* SPI TSIZE, RXDMAEN,TXDMAEN must be configured while SPI is disabled. */
  __HAL_SPI_DISABLE(&SpiHandle);

  /* Configure number of data frames to transmit */
  LL_SPI_SetTransferSize(SpiHandle.Instance, total_size);

  /* Enable RX DMA requests */
  LL_SPI_EnableDMAReq_RX(SpiHandle.Instance);

  /* Enable TX DMA requests */
  LL_SPI_EnableDMAReq_TX(SpiHandle.Instance);

  __HAL_SPI_ENABLE(&SpiHandle);
}

/**
  * @brief  Stops HPDMA3 TX/RX channels and disables SPI3 DMA requests.
  * @retval None
  */
static void SPI3_CloseCommunication_DMA_LinkedList(void)
{
  /* Stop/abort DMA channels */
  (void)HAL_DMA_Abort(&hpdma_spi3_tx);
  (void)HAL_DMA_Abort(&hpdma_spi3_rx);

  /* Disable SPI */
  __HAL_SPI_DISABLE(&SpiHandle);

  /* Disable SPI DMA request generation (no SET_BIT/CLEAR_BIT) */
  LL_SPI_DisableDMAReq_TX(SpiHandle.Instance);
  LL_SPI_DisableDMAReq_RX(SpiHandle.Instance);
}

#ifdef MASTER_BOARD
/**
  * @brief  Starts the SPI master transfer engine (generates SCK).
  * @param 	None
  * @retval None
  */
static void SPI3_StartCommunication_DMA_LinkedList(void)
{
  LL_SPI_StartMasterTransfer(SpiHandle.Instance);
}
#endif

/**
  * @brief  System Clock Configuration.
  * @param 	None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
  RCC_PLLInitTypeDef RCC_Pll4InitStruct = { 0 };
  RCC_PLLInitTypeDef RCC_Pll5InitStruct = { 0 };
  RCC_PLLInitTypeDef RCC_Pll6InitStruct = { 0 };
  RCC_PLLInitTypeDef RCC_Pll7InitStruct = { 0 };
  RCC_PLLInitTypeDef RCC_Pll8InitStruct = { 0 };
  RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI
                                    | RCC_OSCILLATORTYPE_HSE
                                    | RCC_OSCILLATORTYPE_LSE
                                    | RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_OFF;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_OFF;
  RCC_OscInitStruct.LSEDriveValue = RCC_LSEDRIVE_MEDIUMHIGH;

  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

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

  if (HAL_RCCEx_PLL4Config(&RCC_Pll4InitStruct) != HAL_OK) Error_Handler();
  if (HAL_RCCEx_PLL5Config(&RCC_Pll5InitStruct) != HAL_OK) Error_Handler();
  if (HAL_RCCEx_PLL6Config(&RCC_Pll6InitStruct) != HAL_OK) Error_Handler();
  if (HAL_RCCEx_PLL7Config(&RCC_Pll7InitStruct) != HAL_OK) Error_Handler();
  if (HAL_RCCEx_PLL8Config(&RCC_Pll8InitStruct) != HAL_OK) Error_Handler();

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_ICN_HS_MCU
                               | RCC_CLOCKTYPE_ICN_LS_MCU
                               | RCC_CLOCKTYPE_ICN_SDMMC
                               | RCC_CLOCKTYPE_ICN_DDR
                               | RCC_CLOCKTYPE_ICN_DISPLAY
                               | RCC_CLOCKTYPE_ICN_HCL
                               | RCC_CLOCKTYPE_ICN_NIC
                               | RCC_CLOCKTYPE_ICN_VID
                               | RCC_CLOCKTYPE_ICN_APB1
                               | RCC_CLOCKTYPE_ICN_APB2
                               | RCC_CLOCKTYPE_ICN_APB3
                               | RCC_CLOCKTYPE_ICN_APB4
                               | RCC_CLOCKTYPE_ICN_APBDBG;

  RCC_ClkInitStruct.ICN_HS_MCU.XBAR_ClkSrc = RCC_XBAR_CLKSRC_PLL4;
  RCC_ClkInitStruct.ICN_HS_MCU.Div         = 3;
  RCC_ClkInitStruct.ICN_SDMMC.XBAR_ClkSrc  = RCC_XBAR_CLKSRC_PLL4;
  RCC_ClkInitStruct.ICN_SDMMC.Div          = 6;
  RCC_ClkInitStruct.ICN_DDR.XBAR_ClkSrc    = RCC_XBAR_CLKSRC_PLL4;
  RCC_ClkInitStruct.ICN_DDR.Div            = 2;
  RCC_ClkInitStruct.ICN_DISPLAY.XBAR_ClkSrc= RCC_XBAR_CLKSRC_PLL4;
  RCC_ClkInitStruct.ICN_DISPLAY.Div        = 3;
  RCC_ClkInitStruct.ICN_HCL.XBAR_ClkSrc    = RCC_XBAR_CLKSRC_PLL4;
  RCC_ClkInitStruct.ICN_HCL.Div            = 4;
  RCC_ClkInitStruct.ICN_NIC.XBAR_ClkSrc    = RCC_XBAR_CLKSRC_PLL4;
  RCC_ClkInitStruct.ICN_NIC.Div            = 3;
  RCC_ClkInitStruct.ICN_VID.XBAR_ClkSrc    = RCC_XBAR_CLKSRC_PLL4;
  RCC_ClkInitStruct.ICN_VID.Div            = 2;
  RCC_ClkInitStruct.ICN_LSMCU_Div          = RCC_LSMCU_DIV2;
  RCC_ClkInitStruct.APB1_Div               = RCC_APB1_DIV1;
  RCC_ClkInitStruct.APB2_Div               = RCC_APB2_DIV1;
  RCC_ClkInitStruct.APB3_Div               = RCC_APB3_DIV1;
  RCC_ClkInitStruct.APB4_Div               = RCC_APB4_DIV1;
  RCC_ClkInitStruct.APBDBG_Div             = RCC_APBDBG_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, 0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief  Initializes the DMA controller clock.
  * @note   Only enables global HPDMA3 clock (channels are granted via ResMgr).
  * @retval None
  */
static void MX_DMA_Init(void)
{
  /* HPDMA3 clock enable */
  if (ResMgr_Request(RESMGR_RESOURCE_RIF_RCC, RESMGR_RCC_RESOURCE(85)) == RESMGR_STATUS_ACCESS_OK)
  {
    __HAL_RCC_HPDMA3_CLK_ENABLE();
  }
}

/**
  * @brief  Initializes SPI3 peripheral and requests resources via ResMgr.
  * @param 	None
  * @retval None
  */
static void MX_SPI3_Init(void)
{
  /* Acquire SPI3 using Resource manager */
  if (RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIFSC, STM32MP25_RIFSC_SPI3_ID))
  {
    Error_Handler();
  }

  /* Acquire HPDMA3 Channel 2 using Resource manager */
  if (RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIF_HPDMA3, RESMGR_HPDMA_CHANNEL(2)))
  {
    Error_Handler();
  }

  /* Acquire HPDMA3 Channel 3 using Resource manager */
  if (RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIF_HPDMA3, RESMGR_HPDMA_CHANNEL(3)))
  {
    Error_Handler();
  }

  /* Acquire GPIOB10 using Resource manager */
  if (RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIF_GPIOB, RESMGR_GPIO_PIN(10)))
  {
    Error_Handler();
  }

  /* Acquire GPIOB8 using Resource manager */
  if (RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIF_GPIOB, RESMGR_GPIO_PIN(8)))
  {
    Error_Handler();
  }

  /* Acquire GPIOB7 using Resource manager */
  if (RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIF_GPIOB, RESMGR_GPIO_PIN(7)))
  {
    Error_Handler();
  }

  /* Acquire GPIOB1 using Resource manager */
  if (RESMGR_STATUS_ACCESS_OK != ResMgr_Request(RESMGR_RESOURCE_RIF_GPIOB, RESMGR_GPIO_PIN(1)))
  {
    Error_Handler();
  }

  /**SPI3 GPIO Configuration
	 PB10     ------> SPI3_MISO
	 PB8      ------> SPI3_MOSI
	 PB7      ------> SPI3_SCK
	 PB1      ------> SPI3_NSS
   */

  /* Enable GPIOs power supplies */
  if (RESMGR_STATUS_ACCESS_OK == ResMgr_Request(RESMGR_RESOURCE_RIF_RCC, RESMGR_RCC_RESOURCE(91)))
  {
	  __HAL_RCC_GPIOB_CLK_ENABLE();
  }
  SpiHandle.Instance               = SPI3;
  SpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;  /* tune for printhead */
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
  SpiHandle.Init.NSS               = SPI_NSS_SOFT;  /* or HARD if printhead needs it */
  SpiHandle.Init.NSSPMode          = SPI_NSS_PULSE_DISABLE;
  SpiHandle.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_ENABLE;

#ifdef MASTER_BOARD
  SpiHandle.Init.Mode = SPI_MODE_MASTER;
#else
  SpiHandle.Init.Mode = SPI_MODE_SLAVE;
#endif

  if (HAL_SPI_Init(&SpiHandle) != HAL_OK)
  {
    Error_Handler();
  }

  HAL_SPI_MspPostInit(&SpiHandle);
}

/**
  * @brief  Initializes IPCC.
  * @retval None
  */
static void MX_IPCC_Init(void)
{
  hipcc1.Instance = IPCC1;
  if (HAL_IPCC_Init(&hipcc1) != HAL_OK)
  {
    Error_Handler();
  }
  HAL_NVIC_SetPriority(IPCC1_RX_IRQn, DEFAULT_IRQ_PRIO, 0);
  HAL_NVIC_EnableIRQ(IPCC1_RX_IRQn);
}

/**
  * @brief 	SPI3 DeInitialization Function
  * @param 	None
  * @retval None
  */
void MX_SPI3_DeInit(void)
{
  HAL_SPI_DeInit(&SpiHandle);

  /* Release HPDMA3 Channel 2 using Resource manager */
  ResMgr_Release(RESMGR_RESOURCE_RIF_HPDMA3, RESMGR_HPDMA_CHANNEL(2));

  /* Release HPDMA3 Channel 3 using Resource manager */
  ResMgr_Release(RESMGR_RESOURCE_RIF_HPDMA3, RESMGR_HPDMA_CHANNEL(3));

  /* Release SPI3 using Resource manager */
  ResMgr_Release(RESMGR_RESOURCE_RIFSC, STM32MP25_RIFSC_SPI3_ID);

  /* Release GPIOB10 using Resource manager */
  ResMgr_Release(RESMGR_RESOURCE_RIF_GPIOB, RESMGR_GPIO_PIN(10));

  /* Release GPIOB8 using Resource manager */
  ResMgr_Release(RESMGR_RESOURCE_RIF_GPIOB, RESMGR_GPIO_PIN(8));

  /* Release GPIOB7 using Resource manager */
  ResMgr_Release(RESMGR_RESOURCE_RIF_GPIOB, RESMGR_GPIO_PIN(7));

  /* Release GPIOB1 using Resource manager */
  ResMgr_Release(RESMGR_RESOURCE_RIF_GPIOB, RESMGR_GPIO_PIN(1));
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
  /* Deinitialize the SPI peripheral */
  MX_SPI3_DeInit();

  /* Deinitialize the LED3 */
  BSP_LED_DeInit(LED3);

  /* When ready, notify the remote processor that we can be shut down */
  HAL_IPCC_NotifyCPU(hipcc, ChannelIndex, IPCC_CHANNEL_DIR_RX);

  /* Wait for complete shutdown */
  while(1);
}

/**
  * @brief  Builds the SPI3 TX linked-list queue on HPDMA3 Channel 2.
  * @retval HAL status
  */
static HAL_StatusTypeDef SPI3_Transmit_LinkedList_Config(void)
{
  HAL_StatusTypeDef   ret = HAL_OK;
  DMA_NodeConfTypeDef pNodeConfig;

  /* Configure LL for our dedicated channel handle */
  hpdma_spi3_tx.Instance = HPDMA3_Channel2;
  hpdma_spi3_tx.InitLinkedList.Priority          = DMA_HIGH_PRIORITY;
  hpdma_spi3_tx.InitLinkedList.LinkStepMode      = DMA_LSM_FULL_EXECUTION;
  hpdma_spi3_tx.InitLinkedList.LinkAllocatedPort = DMA_LINK_ALLOCATED_PORT1;
  hpdma_spi3_tx.InitLinkedList.TransferEventMode = DMA_TCEM_LAST_LL_ITEM_TRANSFER;
  hpdma_spi3_tx.InitLinkedList.LinkedListMode    = DMA_LINKEDLIST_NORMAL;

  if (HAL_DMAEx_List_Init(&hpdma_spi3_tx) != HAL_OK)
  {
    return HAL_ERROR;
  }

  /* Reset queue */
  HAL_DMAEx_List_ResetQ(&SPI3_Tx_Queue);

  /* Common node config */
  pNodeConfig.NodeType                   = DMA_HPDMA_LINEAR_NODE;
  pNodeConfig.Init.Mode                  = DMA_NORMAL;
  pNodeConfig.Init.Request               = HPDMA_REQUEST_SPI3_TX;
  pNodeConfig.Init.BlkHWRequest          = DMA_BREQ_SINGLE_BURST;
  pNodeConfig.Init.Direction             = DMA_MEMORY_TO_PERIPH;
  pNodeConfig.Init.SrcInc                = DMA_SINC_INCREMENTED;
  pNodeConfig.Init.DestInc               = DMA_DINC_FIXED;
  pNodeConfig.Init.SrcDataWidth          = DMA_SRC_DATAWIDTH_BYTE;
  pNodeConfig.Init.DestDataWidth         = DMA_DEST_DATAWIDTH_BYTE;
  pNodeConfig.Init.SrcBurstLength        = 1;
  pNodeConfig.Init.DestBurstLength       = 1;
  pNodeConfig.Init.TransferAllocatedPort = DMA_SRC_ALLOCATED_PORT0 | DMA_DEST_ALLOCATED_PORT0;
  pNodeConfig.Init.TransferEventMode     = DMA_TCEM_LAST_LL_ITEM_TRANSFER;
  pNodeConfig.TriggerConfig.TriggerPolarity    = DMA_TRIG_POLARITY_MASKED;
  pNodeConfig.DataHandlingConfig.DataExchange  = DMA_EXCHANGE_NONE;
  pNodeConfig.DataHandlingConfig.DataAlignment = DMA_DATA_RIGHTALIGN_ZEROPADDED;

  pNodeConfig.DstAddress = LL_SPI_DMA_GetTxRegAddr(SpiHandle.Instance);

  for (uint32_t i = 0U; i < SPI3_TX_BUFFER_COUNT; i++)
  {
    pNodeConfig.SrcAddress = (uint32_t)aTxBuffers[i];
    pNodeConfig.DataSize   = aTxBufferSizes[i];
    ret |= HAL_DMAEx_List_BuildNode(&pNodeConfig, &SPI3_Tx_Nodes[i]);
    ret |= HAL_DMAEx_List_InsertNode_Tail(&SPI3_Tx_Queue, &SPI3_Tx_Nodes[i]);
  }

  return ret;
}

/**
  * @brief  Builds the SPI3 RX linked-list queue on HPDMA3 Channel 3.
  * @retval HAL status
  */
static HAL_StatusTypeDef SPI3_Receive_LinkedList_Config(void)
{
  HAL_StatusTypeDef   ret = HAL_OK;
  DMA_NodeConfTypeDef pNodeConfig;

  hpdma_spi3_rx.Instance = HPDMA3_Channel3;
  hpdma_spi3_rx.InitLinkedList.Priority          = DMA_HIGH_PRIORITY;
  hpdma_spi3_rx.InitLinkedList.LinkStepMode      = DMA_LSM_FULL_EXECUTION;
  hpdma_spi3_rx.InitLinkedList.LinkAllocatedPort = DMA_LINK_ALLOCATED_PORT1;
  hpdma_spi3_rx.InitLinkedList.TransferEventMode = DMA_TCEM_LAST_LL_ITEM_TRANSFER;
  hpdma_spi3_rx.InitLinkedList.LinkedListMode    = DMA_LINKEDLIST_NORMAL;

  if (HAL_DMAEx_List_Init(&hpdma_spi3_rx) != HAL_OK)
  {
    return HAL_ERROR;
  }

  HAL_DMAEx_List_ResetQ(&SPI3_Rx_Queue);

  pNodeConfig.NodeType                   = DMA_HPDMA_LINEAR_NODE;
  pNodeConfig.Init.Mode                  = DMA_NORMAL;
  pNodeConfig.Init.Request               = HPDMA_REQUEST_SPI3_RX;
  pNodeConfig.Init.BlkHWRequest          = DMA_BREQ_SINGLE_BURST;
  pNodeConfig.Init.Direction             = DMA_PERIPH_TO_MEMORY;
  pNodeConfig.Init.SrcInc                = DMA_SINC_FIXED;
  pNodeConfig.Init.DestInc               = DMA_DINC_INCREMENTED;
  pNodeConfig.Init.SrcDataWidth          = DMA_SRC_DATAWIDTH_BYTE;
  pNodeConfig.Init.DestDataWidth         = DMA_DEST_DATAWIDTH_BYTE;
  pNodeConfig.Init.SrcBurstLength        = 1;
  pNodeConfig.Init.DestBurstLength       = 1;
  pNodeConfig.Init.TransferAllocatedPort = DMA_SRC_ALLOCATED_PORT0 | DMA_DEST_ALLOCATED_PORT0;
  pNodeConfig.Init.TransferEventMode     = DMA_TCEM_LAST_LL_ITEM_TRANSFER;
  pNodeConfig.TriggerConfig.TriggerPolarity    = DMA_TRIG_POLARITY_MASKED;
  pNodeConfig.DataHandlingConfig.DataExchange  = DMA_EXCHANGE_NONE;
  pNodeConfig.DataHandlingConfig.DataAlignment = DMA_DATA_RIGHTALIGN_ZEROPADDED;

  pNodeConfig.SrcAddress = LL_SPI_DMA_GetRxRegAddr(SpiHandle.Instance);

  for (uint32_t i = 0U; i < SPI3_RX_BUFFER_COUNT; i++)
  {
    pNodeConfig.DstAddress = (uint32_t)aRxBuffers[i];
    pNodeConfig.DataSize   = aRxBufferSizes[i];
    ret |= HAL_DMAEx_List_BuildNode(&pNodeConfig, &SPI3_Rx_Nodes[i]);
    ret |= HAL_DMAEx_List_InsertNode_Tail(&SPI3_Rx_Queue, &SPI3_Rx_Nodes[i]);
  }

  return ret;
}

/**
  * @brief  DMA TX transfer complete callback.
  * @param  hdma: DMA handle
  * @retval None
  */
static void DMA_TxTransferComplete(DMA_HandleTypeDef *hdma)
{
  if (hdma == &hpdma_spi3_tx)
  {
    txComplete = 1;
    if ((txComplete != 0u) && (rxComplete != 0u))
    {
      wTransferState = TRANSFER_COMPLETE;
    }
  }
}

/**
  * @brief  DMA TX transfer error callback.
  * @param  hdma: DMA handle
  * @retval None
  */
static void DMA_TxTransferError(DMA_HandleTypeDef *hdma)
{
  if (hdma == &hpdma_spi3_tx)
  {
    wTransferState = TRANSFER_ERROR;
  }
}

/**
  * @brief  DMA RX transfer complete callback.
  * @param  hdma: DMA handle
  * @retval None
  */
static void DMA_RxTransferComplete(DMA_HandleTypeDef *hdma)
{
  if (hdma == &hpdma_spi3_rx)
  {
    rxComplete = 1;
    if ((txComplete != 0u) && (rxComplete != 0u))
    {
      wTransferState = TRANSFER_COMPLETE;
    }
  }
}

/**
  * @brief  DMA RX transfer error callback.
  * @param  hdma: DMA handle
  * @retval None
  */
static void DMA_RxTransferError(DMA_HandleTypeDef *hdma)
{
  if (hdma == &hpdma_spi3_rx)
  {
    wTransferState = TRANSFER_ERROR;
  }
}

/**
  * @brief  Compares two buffers.
  * @param  pBuffer1, pBuffer2: buffers to be compared.
  * @param  BufferLength: buffer's length
  * @retval 0  : pBuffer1 identical to pBuffer2
  *         >0 : pBuffer1 differs from pBuffer2
  */
static uint16_t Buffercmp(const uint8_t *pBuffer1, const uint8_t *pBuffer2, uint16_t buffer_length)
{
  while (buffer_length--)
  {
    if ((*pBuffer1) != *pBuffer2)
    {
      return buffer_length;
    }
    pBuffer1++;
    pBuffer2++;
  }

  return 0U;
}

#ifdef ICACHE_DCACHE_ENABLE
/**
  * @brief Instruction Cache Initialization Function
  * @param None
  * @retval None
  */
static void MX_ICACHE_Init(void)
{

  if(HAL_ICACHE_DeInit() != HAL_OK)
  {
    Error_Handler();
  }
  ICACHE_RegionConfigTypeDef pRegionConfig = {0};
  pRegionConfig.TrafficRoute    = ICACHE_MASTER2_PORT;
  pRegionConfig.OutputBurstType = ICACHE_OUTPUT_BURST_INCR;
  pRegionConfig.Size            = ICACHE_REGIONSIZE_2MB;
  pRegionConfig.BaseAddress     = 0x00000000;
  pRegionConfig.RemapAddress    = 0x80000000;

  if (HAL_ICACHE_EnableRemapRegion(ICACHE_REGION_0, &pRegionConfig) != HAL_OK)
  {
  	 Error_Handler();
  }

  if (HAL_ICACHE_Enable() != HAL_OK)
  {
	  Error_Handler();
  }
}

/**
  * @brief Data Cache Initialization Function
  * @param None
  * @retval None
  */
static void MX_DCACHE_Init(void)
{

  hdcache.Instance = DCACHE;
  hdcache.Init.ReadBurstType = DCACHE_READ_BURST_WRAP;

  if (HAL_DCACHE_Init(&hdcache) != HAL_OK)
  {
     Error_Handler();
  }

}

/**
  * @brief  Configure the MPU attributes
  * @param  None
  * @retval None
  */
static void MPU_Config(void)
{

   MPU_Region_InitTypeDef MPU_InitStruct;
   MPU_Attributes_InitTypeDef MPU_Attributes_InitStruct ;

   HAL_MPU_Disable();

   /* write back, read and write allocate */
   MPU_Attributes_InitStruct.Attributes = INNER_OUTER(MPU_WRITE_BACK | MPU_NON_TRANSIENT | MPU_RW_ALLOCATE);
   MPU_Attributes_InitStruct.Number = MPU_ATTRIBUTES_NUMBER0;
   HAL_MPU_ConfigMemoryAttributes(&MPU_Attributes_InitStruct);

   /* ICACHE */
   MPU_InitStruct.Enable           = MPU_REGION_ENABLE;
   MPU_InitStruct.Number           = MPU_REGION_NUMBER0;
   MPU_InitStruct.AttributesIndex  = MPU_ATTRIBUTES_NUMBER0;
   MPU_InitStruct.BaseAddress      = 0x00000000;
   MPU_InitStruct.LimitAddress     = 0x00010000;
   MPU_InitStruct.AccessPermission = MPU_REGION_ALL_RO;
   MPU_InitStruct.DisableExec      = MPU_INSTRUCTION_ACCESS_ENABLE;
   MPU_InitStruct.IsShareable      = MPU_ACCESS_NOT_SHAREABLE;
   HAL_MPU_ConfigRegion(&MPU_InitStruct);

   /* DCACHE */
   MPU_InitStruct.AccessPermission = MPU_REGION_ALL_RW;
   MPU_InitStruct.Enable = MPU_REGION_ENABLE;
   MPU_InitStruct.AttributesIndex = MPU_ATTRIBUTES_NUMBER0;
   MPU_InitStruct.DisableExec= MPU_INSTRUCTION_ACCESS_DISABLE;
   MPU_InitStruct.IsShareable = MPU_ACCESS_INNER_SHAREABLE;
   MPU_InitStruct.BaseAddress =  0x80A00000;
   MPU_InitStruct.LimitAddress = 0x80DFFC00;
   MPU_InitStruct.Number = MPU_REGION_NUMBER1;
   HAL_MPU_ConfigRegion(&MPU_InitStruct);

   /* Data section(IPC_SHMEM_1 & VIRTIO_SHMEM)- NON CACHEABLE */
   MPU_Attributes_InitStruct.Attributes = INNER_OUTER(MPU_NOT_CACHEABLE);
   MPU_Attributes_InitStruct.Number = MPU_ATTRIBUTES_NUMBER1;
   HAL_MPU_ConfigMemoryAttributes(&MPU_Attributes_InitStruct);

   MPU_InitStruct.AccessPermission = MPU_REGION_ALL_RW;
   MPU_InitStruct.Enable = MPU_REGION_ENABLE;
   MPU_InitStruct.AttributesIndex = MPU_ATTRIBUTES_NUMBER1;
   MPU_InitStruct.DisableExec= MPU_INSTRUCTION_ACCESS_DISABLE;
   MPU_InitStruct.IsShareable = MPU_ACCESS_INNER_SHAREABLE;
   MPU_InitStruct.BaseAddress =  0x81200000;
   MPU_InitStruct.LimitAddress = 0x812FFFFF;
   MPU_InitStruct.Number = MPU_REGION_NUMBER2;
   HAL_MPU_ConfigRegion(&MPU_InitStruct);

   HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}
#endif /* ICACHE_DCACHE_ENABLE */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  printf("Error : Something went wrong\r\n");
  BSP_LED_On(LED3);
  while (1)
  {
    HAL_Delay(1000);
  }
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
  Error_Handler();
}
#endif /* USE_FULL_ASSERT */
