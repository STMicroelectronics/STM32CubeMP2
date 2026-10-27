/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_freertos.c
  * @author  MCD Application Team
  * @brief   FreeRTOS application implementation file.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
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
#include "app_freertos.h"
#include "cmsis_os2.h"
#include "lwip/tcpip.h"
#include "lwip/init.h"
#include "lwip/netif.h"
#if defined ( __CC_ARM )  /* MDK ARM Compiler */
#include "lwip/sio.h"
#endif /* MDK ARM Compiler */
#include "ethernetif.h"
#include "copro_sync.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
void DHCP_Thread(void * argument);
void ethernet_link_thread(void* argument);
/* USER CODE END Includes */
extern struct netif gnetif;
extern ip4_addr_t ipaddr;
extern ip4_addr_t netmask;
extern ip4_addr_t gw;
extern uint8_t IP_ADDRESS[4];
extern uint8_t NETMASK_ADDRESS[4];
extern uint8_t GATEWAY_ADDRESS[4];
extern ETH_HandleTypeDef heth;
#if defined(DATA_CACHE_ENABLE) && (DATA_CACHE_ENABLE == 1U)
extern DCACHE_HandleTypeDef hdcache;
#endif
/* Created in net_sockets.c */
extern osThreadId_t ethernet_link_thread_handle;
extern osThreadId_t DHCPThreadHandle;

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
static osThreadId_t SSL_ServerHandle;
static const osThreadAttr_t SSL_Server_attributes = {
  .name = "Thread One",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 1024 * 8
};

static osSemaphoreId_t ShutDownSemaphore;
static const osSemaphoreAttr_t semAttr_ShutDownSemaphore = {
    .name = "ShutDownSemaphore",
};
static osThreadId_t ShutDownThreadHandle;
static const osThreadAttr_t ShutDown_attributes = {
  .name = "ShutDown Thread",
  .priority = (osPriority_t) osPriorityAboveNormal,
  .stack_size = 2 * 1024
};
static IPCC_HandleTypeDef const *hipcc_handle;
static uint32_t ipcc_ch_id;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

static void shutdown_thread_entry(void *argument);
static void MX_FREERTOS_DeInit(void);
/* USER CODE END PFP */

  /**
  * @brief  Define the initial system.
  * @param  first_unused_memory : Pointer to the first unused memory
  * @retval None
  */
void MX_FREERTOS_Init(void)
{
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  ShutDownSemaphore = osSemaphoreNew(1, 0, &semAttr_ShutDownSemaphore);
  if (NULL == ShutDownSemaphore)
  {
    Error_Handler();
  }
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
  /* Creation of task */
  printf("\r\n Starting Main Thread...\n\r");
  /* Start SSL server task: wait for a client connection and perform the SSL/TLS handshake. */
  SSL_ServerHandle = osThreadNew(SSL_Server, NULL, &SSL_Server_attributes);
  if(SSL_ServerHandle == NULL)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  ShutDownThreadHandle = osThreadNew(shutdown_thread_entry, NULL, &ShutDown_attributes);
  if (NULL == ShutDownThreadHandle)
  {
    Error_Handler();
  }
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}



/**
  * @brief  Shutdown thread: performs the graceful teardown once the remote processor requests it
  * @param  argument not used
  * @retval None
  */
static void shutdown_thread_entry(void *argument)
{
  osSemaphoreAcquire(ShutDownSemaphore, osWaitForever);

  /* Teardown RTOS resources first */
  MX_FREERTOS_DeInit();

  HAL_ETH_Stop_IT(&heth);
  HAL_ETH_DeInit(&heth);

  BSP_LED_DeInit(LED3);

#if defined(DATA_CACHE_ENABLE) && (DATA_CACHE_ENABLE == 1U)
  /* Flush the shared DDR region so the remote processor never reads stale cached data */
  HAL_DCACHE_CleanInvalidByAddr(&hdcache, (const uint32_t *)0x80A00000U, 0x003FFC00U);
  HAL_DCACHE_Disable(&hdcache);
  HAL_DCACHE_DeInit(&hdcache);
#endif
#if defined(INSTRUCTION_CACHE_ENABLE) && (INSTRUCTION_CACHE_ENABLE == 1U)
  HAL_ICACHE_DeInit();
#endif

  /* When ready, notify the remote processor that we can be shut down */
  HAL_IPCC_NotifyCPU(hipcc_handle, ipcc_ch_id, IPCC_CHANNEL_DIR_RX);

  /* Wait for complete shutdown */
  while(1);
}

/**
  * @brief  Deinitialize FreeRTOS resources (threads, semaphores, tick)
  * @param  None
  * @retval None
  */
static void MX_FREERTOS_DeInit(void)
{
  HAL_DeInitTick();
  (void) osSemaphoreDelete(ShutDownSemaphore);
  (void) osThreadTerminate(SSL_ServerHandle);
  (void) osThreadTerminate(ethernet_link_thread_handle);
  (void) osThreadTerminate(DHCPThreadHandle);
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
  hipcc_handle = hipcc;
  ipcc_ch_id = ChannelIndex;
  (void) osSemaphoreRelease(ShutDownSemaphore);
}

