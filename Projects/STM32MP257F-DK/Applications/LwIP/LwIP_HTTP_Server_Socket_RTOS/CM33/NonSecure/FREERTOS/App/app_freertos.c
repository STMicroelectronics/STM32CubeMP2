/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_azure_rtos.c
  * @author  MCD Application Team
  * @brief   app_azure_rtos application implementation file
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
#include "httpserver_socket.h"
#include "copro_sync.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
void DHCP_Thread(void * argument);
void ethernet_link_thread(void* argument);
static void Netif_Config(void);
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
static osThreadId_t StartThreadHandle;
static const osThreadAttr_t StartThread_attributes = {
  .name = "Start Thread",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 1024 * 2
};
static osThreadId_t ethernet_link_thread_handle;
static const osThreadAttr_t ethernet_link_thread_attributes = {
  .name = "Link Thread",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 1024 * 2
};
static osThreadId_t DHCPThreadHandle;
static const osThreadAttr_t DHCPThread_attributes = {
  .name = "DHCP Thread",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 1024 * 2
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
static void StartThread(void *argument);
static void shutdown_thread_entry(void *argument);
static void MX_FREERTOS_DeInit(void);
/* USER CODE END PFP */

  /**
  * @brief  Define the initial system.
  * @param  first_unused_memory : Pointer to the first unused memory
  * @retval None
  */
void MX_FREERTOS_Init(void) {
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
  /* creation of Task */
	StartThreadHandle = osThreadNew(StartThread, NULL, &StartThread_attributes);
  if (NULL == StartThreadHandle)
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
  * @brief  Start Thread
  * @param  argument not used
  * @retval None
  */
static void StartThread(void *argument)
{
  /* Create tcp_ip stack thread */
  tcpip_init(NULL, NULL);

  /* Initialize the LwIP stack */
  Netif_Config();

  /* Initialize webserver demo */
  http_server_socket_init();

  for( ;; )
  {
    /* Delete the Init Thread */
    osThreadTerminate(StartThreadHandle);
  }
}

/**
  * @brief  Initializes the lwIP stack
  * @param  None
  * @retval None
  */

static void Netif_Config(void)
{
  ip_addr_t ipaddr;
  ip_addr_t netmask;
  ip_addr_t gw;

#if LWIP_DHCP
  ip_addr_set_zero_ip4(&ipaddr);
  ip_addr_set_zero_ip4(&netmask);
  ip_addr_set_zero_ip4(&gw);
#else
  IP_ADDR4(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
  IP_ADDR4(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
  IP_ADDR4(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
#endif /* LWIP_DHCP */

  /* add the network interface */
  netif_add(&gnetif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &tcpip_input);

  /*  Registers the default network interface. */
  netif_set_default(&gnetif);

  ethernet_link_status_updated(&gnetif);

#if LWIP_NETIF_LINK_CALLBACK
  netif_set_link_callback(&gnetif, ethernet_link_status_updated);

  ethernet_link_thread_handle = osThreadNew(ethernet_link_thread, &gnetif, &ethernet_link_thread_attributes);
  if (NULL == ethernet_link_thread_handle)
  {
    Error_Handler();
  }
#endif

#if LWIP_DHCP
  /* Start DHCPClient */
  DHCPThreadHandle = osThreadNew(DHCP_Thread, &gnetif, &DHCPThread_attributes);
  if (NULL == DHCPThreadHandle)
  {
    Error_Handler();
  }
#endif
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
  (void) osThreadTerminate(StartThreadHandle);
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
