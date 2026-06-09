/**
 ******************************************************************************
 * @file   openamp.c
 * @author  MCD Application Team
 * @brief  Code for openamp applications
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

/*
 * Channel direction and usage:
 *
 *  ========   <-- new msg ---=============--------<------   =======
 * ||      ||                || CHANNEL 1 ||                ||     ||
 * ||  A35 ||  ------->-------=============--- buf free-->  || M33 ||
 * ||      ||                                               ||     ||
 * ||master||  <-- buf free---=============--------<------  ||slave||
 * ||      ||                || CHANNEL 2 ||                ||     ||
 *  ========   ------->-------=============----new msg -->   =======
 */

#include "openamp.h"
#include "rsc_table.h"
#include "metal/sys.h"
#include "metal/device.h"
#include "stm32mp2xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN Define */

/* USER CODE END Define */

#define SHM_DEVICE_NAME         "STM32_SHM"

/* Backup register to store the resource table memory region */
#define OPENAMP_RSCTAB_ADDR_BUP_IDX 121
#define OPENAMP_RSCTAB_LGTH_BUP_IDX 122

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */


/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */
static OPENAMP_MBOX_RxCallback_t openamp_mbox_rx_callback = NULL;

static struct metal_io_region *shm_io = {0};
static struct metal_io_region *rsc_io = {0};
static struct shared_resource_table *rsc_table;
static struct rpmsg_virtio_shm_pool shpool;
static struct rpmsg_virtio_device rvdev;

static metal_phys_addr_t shm_physmap;

static unsigned int msg_buff_avail = 0;
static unsigned int msg_buff_used = 0;

#if (METAL_MAX_DEVICE_REGIONS == 1)
#error METAL_MAX_DEVICE_REGIONS should be higher than 1
#endif


struct metal_device shm_device = {
  .name = SHM_DEVICE_NAME,
  .num_regions = 2,
  .regions = {
      {.virt = NULL}, /* shared memory */
      {.virt = NULL}, /* rsc_table memory */
  },
  .node = { NULL },
  .irq_num = 0,
  .irq_info = NULL
};

/* Private functions ---------------------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

static int OPENAMP_MBOX_Init(IPCC_HandleTypeDef *hipcc);
static void OPENAMP_MBOX_DeInit(IPCC_HandleTypeDef *hipcc);
static int OPENAMP_MBOX_Notify(void *priv, uint32_t id);
static void OPENAMP_MBOX_Poll(void);

/* Callback from IPCC Interrupt Handler: Master Processor informs that there are some free buffers */
static void IPCC_buff_used_callback(IPCC_HandleTypeDef * hipcc,
                                   uint32_t ChannelIndex, IPCC_CHANNELDirTypeDef ChannelDir)
{
  /* USER CODE BEGIN PRE_MAILBOX_CHANNEL1_CALLBACK */

  /* USER CODE END  PRE_MAILBOX_CHANNEL1_CALLBACK */

  /* Inform A35 that we have received the 'buff free' msg */
  HAL_IPCC_NotifyCPU(hipcc, ChannelIndex, IPCC_CHANNEL_DIR_RX);

  msg_buff_used = 1;
  if (openamp_mbox_rx_callback != NULL)
  {
    openamp_mbox_rx_callback(VRING0_ID);
  }
  /* USER CODE BEGIN POST_MAILBOX_CHANNEL1_CALLBACK */

  /* USER CODE END  POST_MAILBOX_CHANNEL1_CALLBACK */
}

/* Callback from IPCC Interrupt Handler: new message received from Master Processor */
static void IPCC_buff_avail_callback(IPCC_HandleTypeDef * hipcc,
                                     uint32_t ChannelIndex, IPCC_CHANNELDirTypeDef ChannelDir)
{
  /* USER CODE BEGIN PRE_MAILBOX_CHANNEL2_CALLBACK */

  /* USER CODE END  PRE_MAILBOX_CHANNEL2_CALLBACK */

  /* Inform A35 that we have received the 'buff free' msg */
  HAL_IPCC_NotifyCPU(hipcc, ChannelIndex, IPCC_CHANNEL_DIR_RX);

  msg_buff_avail = 1;
  if (openamp_mbox_rx_callback != NULL)
  {
    openamp_mbox_rx_callback(VRING1_ID);
  }
  /* USER CODE BEGIN POST_MAILBOX_CHANNEL2_CALLBACK */

  /* USER CODE END  POST_MAILBOX_CHANNEL2_CALLBACK */
}

void OPENAMP_RegisterMBOXRxCallback(OPENAMP_MBOX_RxCallback_t cb)
{
  openamp_mbox_rx_callback = cb;
}

static int OPENAMP_MBOX_Init(IPCC_HandleTypeDef *hipcc)
{
  /* USER CODE BEGIN PRE_MAILBOX_INIT */

  /* USER CODE END  PRE_MAILBOX_INIT */

  if (hipcc == NULL)
  {
    return -1;
  }

  if (HAL_IPCC_ActivateNotification(hipcc, IPCC_CHANNEL_1, IPCC_CHANNEL_DIR_RX,
                                    IPCC_buff_used_callback) != HAL_OK)
  {
    OPENAMP_log_err("OpenAMP","%s: ch_1 RX fail", __func__);
    return -1;
  }

  if (HAL_IPCC_ActivateNotification(hipcc, IPCC_CHANNEL_2, IPCC_CHANNEL_DIR_RX,
                                    IPCC_buff_avail_callback) != HAL_OK)
  {
    OPENAMP_log_err("OpenAMP","%s: ch_2 RX fail", __func__);
    return -1;
  }

  /* USER CODE BEGIN POST_MAILBOX_INIT */

  /* USER CODE END  POST_MAILBOX_INIT */
  return 0;
}

/**
  * @brief  DeInitialize MAILBOX with IPCC peripheral
  * @param  hipcc: Pointer to IPCC handle
  * @retval None
  */
static void OPENAMP_MBOX_DeInit(IPCC_HandleTypeDef *hipcc)
{
  /* USER CODE BEGIN PRE_MAILBOX_DEINIT */

  /* USER CODE END  PRE_MAILBOX_DEINIT */

  if (hipcc == NULL)
  {
    return;
  }

  if (HAL_IPCC_DeActivateNotification(hipcc, IPCC_CHANNEL_1, IPCC_CHANNEL_DIR_RX) != HAL_OK)
  {
    OPENAMP_log_err("OpenAMP","%s: ch_1 RX fail", __func__);
  }

  if (HAL_IPCC_DeActivateNotification(hipcc, IPCC_CHANNEL_2, IPCC_CHANNEL_DIR_RX) != HAL_OK)
  {
    OPENAMP_log_err("OpenAMP","%s: ch_2 RX fail", __func__);
  }

  /* USER CODE BEGIN POST_MAILBOX_DEINIT */

  /* USER CODE END  POST_MAILBOX_DEINIT */
}

static int OPENAMP_shmem_init(int RPMsgRole)
{
  int status = 0;
  struct metal_device *device = NULL;
  struct metal_init_params metal_params = METAL_INIT_DEFAULTS;
  void* rsc_tab_addr = NULL;
  int rsc_size = 0;


  /* USER CODE BEGIN PRE_LIB_METAL_INIT */

  /* USER CODE END  PRE_LIB_METAL_INIT */
  status = metal_init(&metal_params);
  if (status != 0)
  {
    return status;
  }

  status = metal_register_generic_device(&shm_device);
  if (status != 0)
  {
    return status;
  }

  status = metal_device_open("generic", SHM_DEVICE_NAME, &device);
  if (status != 0)
  {
    return status;
  }

  shm_physmap = SHM_START_ADDRESS;
  metal_io_init(&device->regions[0], (void *)SHM_START_ADDRESS, &shm_physmap,
                SHM_SIZE, (unsigned int)-1, 0, NULL);

  /* USER CODE BEGIN PRE_SHM_IO_INIT */

  /* USER CODE END PRE_SHM_IO_INIT */
  shm_io = metal_device_io_region(device, 0);
  if (shm_io == NULL)
  {
    return -1;
  }

  /* USER CODE BEGIN POST_SHM_IO_INIT */

  /* USER CODE END POST_SHM_IO_INIT */

  /* Initialize resources table variables */
  resource_table_init(RPMsgRole, &rsc_tab_addr, &rsc_size);
  rsc_table = (struct shared_resource_table *)rsc_tab_addr;
  if (!rsc_table)
  {
    return -1;
  }

/*
 * Store the resource table address and length in backup registers to provide
 * resource table information to the Cortex A
 */
  HAL_RTCEx_BKUPWrite(NULL, OPENAMP_RSCTAB_ADDR_BUP_IDX, (uint32_t)rsc_tab_addr);
  HAL_RTCEx_BKUPWrite(NULL, OPENAMP_RSCTAB_LGTH_BUP_IDX, (uint32_t)rsc_size);

  /* USER CODE BEGIN POST_RSC_TABLE_INIT */

  /* USER CODE END  POST_RSC_TABLE_INIT */

  metal_io_init(&device->regions[1], rsc_table,
               (metal_phys_addr_t *)rsc_table, rsc_size, -1U, 0, NULL);

  /* USER CODE BEGIN POST_METAL_IO_INIT */

  /* USER CODE END  POST_METAL_IO_INIT */
  rsc_io = metal_device_io_region(device, 1);
  if (rsc_io == NULL)
  {
    return -1;
  }

  /* USER CODE BEGIN POST_RSC_IO_INIT */

  /* USER CODE END  POST_RSC_IO_INIT */
  return 0;
}

/**
 * check if the remote is ready to start RPMsg communication
 */
static void OPENAMP_Wait_Remote_Ready(struct virtio_device *vdev)
{
	uint8_t status;

  status = vdev->func->get_status(vdev);

	while (1) {
		status = vdev->func->get_status(vdev);
		if (status & VIRTIO_CONFIG_STATUS_DRIVER_OK)
			return;

      /* Delay to wait before checking again that the remote processor is ready */
      osDelay(100);
  }
}

/**
  * @brief  Callback function called by OpenAMP MW to notify message processing
  * @param  VRING id
  * @retval Operation result
  */
static int OPENAMP_MBOX_Notify(void *priv, uint32_t id)
{
  uint32_t channel;
  IPCC_HandleTypeDef *hipcc = (IPCC_HandleTypeDef *)priv;

  if (hipcc == NULL)
  {
    return -1;
  }

  /* USER CODE BEGIN PRE_MAILBOX_NOTIFY */
  /* USER CODE END  PRE_MAILBOX_NOTIFY */
  /* Called after virtqueue processing: time to inform the remote */

  if (id == VRING0_ID)
  {
    channel = IPCC_CHANNEL_1;
    OPENAMP_log_dbg("OpenAMP", "Send msg on ch_1");
  }
  else if (id == VRING1_ID) {
    channel = IPCC_CHANNEL_2;
  }
  else {
    OPENAMP_log_err("OpenAMP", "invalid vring (%d)", (int)id);
    return -1;
  }

  /* Check that the channel is free (otherwise wait until it is) */
  if (HAL_IPCC_GetChannelStatus(hipcc, channel, IPCC_CHANNEL_DIR_TX) == IPCC_CHANNEL_STATUS_OCCUPIED)
  {
    while (HAL_IPCC_GetChannelStatus(hipcc, channel, IPCC_CHANNEL_DIR_TX) == IPCC_CHANNEL_STATUS_OCCUPIED)
      ;
  }

  /* Inform A35 (either new message, or buf free) */
  HAL_IPCC_NotifyCPU(hipcc, channel, IPCC_CHANNEL_DIR_TX);

  /* USER CODE BEGIN POST_MAILBOX_NOTIFY */
  /* USER CODE END  POST_MAILBOX_NOTIFY */

  return 0;
}

int MX_OPENAMP_Init(int RPMsgRole, rpmsg_ns_bind_cb ns_bind_cb, IPCC_HandleTypeDef * hipcc)
{
  struct fw_rsc_vdev_vring *vring_rsc = NULL;
  struct virtio_device *vdev = NULL;
  int status = 0;

  if (hipcc == NULL)
  {
    return -1;
  }

  /* USER CODE BEGIN MAILBOX_Init */

  /* USER CODE END MAIL_BOX_Init */

  OPENAMP_MBOX_Init(hipcc);

  /* Libmetal Initilalization */
  status = OPENAMP_shmem_init(RPMsgRole);
  if (status)
  {
    return status;
  }

  /* USER CODE BEGIN  PRE_VIRTIO_INIT */

  /* USER CODE END PRE_VIRTIO_INIT */
  vdev = rproc_virtio_create_vdev(RPMsgRole, VDEV_ID, &rsc_table->vdev, rsc_io,
                                  hipcc, OPENAMP_MBOX_Notify, NULL);
  if (vdev == NULL)
  {
    return -1;
  }

  OPENAMP_log_info("OpenAMP", "waiting remote processor initialisation...");
  OPENAMP_Wait_Remote_Ready(vdev);
  OPENAMP_log_info("OpenAMP", " Ready for RPMsg communication");

  /* USER CODE BEGIN  POST_VIRTIO_INIT */

  /* USER CODE END POST_VIRTIO_INIT */
  vring_rsc = &rsc_table->vring0;
  status = rproc_virtio_init_vring(vdev, 0, vring_rsc->notifyid,
                                   (void *)vring_rsc->da, shm_io,
                                   vring_rsc->num, vring_rsc->align);
  if (status != 0)
  {
    return status;
  }

  /* USER CODE BEGIN  POST_VRING0_INIT */

  /* USER CODE END POST_VRING0_INIT */
  vring_rsc = &rsc_table->vring1;
  status = rproc_virtio_init_vring(vdev, 1, vring_rsc->notifyid,
                                   (void *)vring_rsc->da, shm_io,
                                   vring_rsc->num, vring_rsc->align);
  if (status != 0)
  {
    return status;
  }

  /* USER CODE BEGIN  POST_VRING1_INIT */

  /* USER CODE END POST_VRING1_INIT */

  rpmsg_virtio_init_shm_pool(&shpool, (void *)VRING_BUFF_ADDRESS,
                             (size_t)SHM_SIZE);
  rpmsg_init_vdev(&rvdev, vdev, ns_bind_cb, shm_io, &shpool);

  /* USER CODE BEGIN POST_RPMSG_INIT */

  /* USER CODE END POST_RPMSG_INIT */

  return 0;
}

void OPENAMP_DeInit(IPCC_HandleTypeDef * hipcc)
{

  /* USER CODE BEGIN PRE_OPENAMP_DEINIT */

  /* USER CODE END PRE_OPENAMP_DEINIT */

  rpmsg_deinit_vdev(&rvdev);

  metal_finish();

  OPENAMP_MBOX_DeInit(hipcc);

  /* USER CODE BEGIN POST_OPENAMP_DEINIT */

  /* USER CODE END POST_OPENAMP_DEINIT */
}

static void OPENAMP_MBOX_Poll(void)
{
  if (msg_buff_avail)
  {
    OPENAMP_log_dbg("OpenAMP", "Running virt1 (ch_2 new msg)");
    rproc_virtio_notified(rvdev.vdev, VRING1_ID);
    msg_buff_avail = 0;
  }

  if (msg_buff_used)
  {
    OPENAMP_log_dbg("OpenAMP","Running virt0 (ch_1 buf free)");
    rproc_virtio_notified(rvdev.vdev, VRING0_ID);
    msg_buff_used = 0;
  }
}

int OPENAMP_create_endpoint(struct rpmsg_endpoint *ept, const char *name,
                            uint32_t dest, rpmsg_ept_cb cb,
                            rpmsg_ns_unbind_cb unbind_cb)
{
  int ret = 0;
  /* USER CODE BEGIN PRE_EP_CREATE */

  /* USER CODE END PRE_EP_CREATE */

  ret = rpmsg_create_ept(ept, &rvdev.rdev, name, RPMSG_ADDR_ANY, dest, cb,
              unbind_cb);

  /* USER CODE BEGIN POST_EP_CREATE */

  /* USER CODE END POST_EP_CREATE */
  return ret;
}


int OPENAMP_create_fixed_endpoint(struct rpmsg_endpoint *ept, const char *name,
                                  uint32_t src, uint32_t dest, rpmsg_ept_cb cb,
                                  rpmsg_ns_unbind_cb unbind_cb)
{
  int ret = 0;
  /* USER CODE BEGIN PRE_EP_CREATE */

  /* USER CODE END PRE_EP_CREATE */

  ret = rpmsg_create_ept(ept, &rvdev.rdev, name, src, dest, cb,
              unbind_cb);

  /* USER CODE BEGIN POST_EP_CREATE */

  /* USER CODE END POST_EP_CREATE */
  return ret;
}

int OPENAMP_check_for_message(void)
{
  /* USER CODE BEGIN MSG_CHECK */

  /* USER CODE END MSG_CHECK */
  OPENAMP_MBOX_Poll();

  return 0;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
