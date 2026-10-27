/**
 ******************************************************************************
 * @file   openamp.c
 * @author  MCD Application Team
 * @brief  Code for openamp applications
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2019-2025 STMicroelectronics.
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
#include <string.h>
#include "metal/sys.h"
#include "metal/device.h"
#include "rsc_table.h"
#if defined (OPENAMP_USE_FREERTOS)
#include "cmsis_os.h"
#endif /* defined (OPENAMP_USE_FREERTOS) */
#if defined(OPENAMP_USE_TFM_NS_API) && !defined(CORE_CA35)
#include "tfm_platform_api.h"
#include "tfm_ioctl_cpu_api.h"
#endif /* defined(OPENAMP_USE_TFM_NS_API) && !defined(CORE_CA35) */

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN Define */

/* USER CODE END Define */

#define SHM_DEVICE_NAME         "STM32_SHM"

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */


/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
OPENAMP_MBOX_RxCallback_t openamp_mbox_rx_callback = NULL;

/* USER CODE END PV */

static struct metal_io_region *shm_io = {0};
static struct metal_io_region *rsc_io = {0};
static struct shared_resource_table *rsc_table;
static struct rpmsg_virtio_shm_pool shpool;
static struct rpmsg_virtio_device rvdev;

static metal_phys_addr_t shm_physmap;

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
#ifdef OPENAMP_USE_FREERTOS
/* Callback from IPCC Interrupt Handler: Master Processor informs that there are some free buffers */
static void OPENAMP_MboxRxCallback(IPCC_HandleTypeDef * hipcc,
                                   uint32_t ChannelIndex, IPCC_CHANNELDirTypeDef ChannelDir)
{
  (void)ChannelDir;

  /* USER CODE BEGIN PRE_MAILBOX_CHANNEL1_CALLBACK */

  /* USER CODE END  PRE_MAILBOX_CHANNEL1_CALLBACK */

   /* Inform A35 that we have received the 'buff free' msg */
  HAL_IPCC_NotifyCPU(hipcc, ChannelIndex, IPCC_CHANNEL_DIR_RX);

  if (openamp_mbox_rx_callback != NULL)
  {
    openamp_mbox_rx_callback(ChannelIndex);
  }
  /* USER CODE BEGIN POST_MAILBOX_CHANNEL1_CALLBACK */

  /* USER CODE END  POST_MAILBOX_CHANNEL1_CALLBACK */
}
#endif

void OPENAMP_RegisterMBOXRxCallback(OPENAMP_MBOX_RxCallback_t cb)
{
  openamp_mbox_rx_callback = cb;
}
/* USER CODE END PFP */

#if (OPENAMP_ROLE == RPMSG_REMOTE)
/**
 * check if the remote is ready to start RPMsg communication
 */
static void OPENAMP_Wait_Vdev(struct virtio_device *vdev)
{
  uint8_t status;

  status = vdev->func->get_status(vdev);

  while (1) {
  status = vdev->func->get_status(vdev);
    if (status & VIRTIO_CONFIG_STATUS_DRIVER_OK)
      return;

#ifdef OPENAMP_USE_FREERTOS
      /* Delay to wait before checking again that the remote processor is ready */
      osDelay(1000UL);
#endif
  }
}
#endif


int MX_OPENAMP_Init(int RPMsgRole, IPCC_HandleTypeDef * hipcc)
{
  int status = 0;
  struct metal_device *device = NULL;
  struct metal_init_params metal_params = METAL_INIT_DEFAULTS;
  void* rsc_tab_addr = NULL;
  int rsc_size = 0;
#if defined(OPENAMP_USE_TFM_NS_API) && !defined(CORE_CA35)
  enum tfm_platform_err_t status_tfm;
#endif /* defined(OPENAMP_USE_TFM_NS_API) && !defined(CORE_CA35) */

  /* USER CODE BEGIN MAILBOX_Init */

  /* USER CODE END MAIL_BOX_Init */

  /* mailbox initialization */
  #if defined (OPENAMP_USE_FREERTOS)
  status = MAILBOX_Init(hipcc, OPENAMP_MboxRxCallback);
  #else
  status = MAILBOX_Init(hipcc, NULL);
  #endif
  if (status != 0)
  {
    return status;
  }
  /* Libmetal initialization */

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

  shm_io = metal_device_io_region(device, 0);
  if (shm_io == NULL)
  {
    return -1;
  }

  /* USER CODE BEGIN POST_LIB_METAL_INIT */

  /* USER CODE END POST_LIB_METAL_INIT*/

  /* Resource table Initialization */
  resource_table_init(RPMsgRole, &rsc_tab_addr, &rsc_size);

  rsc_table = (struct shared_resource_table *)rsc_tab_addr;
  if (!rsc_table)
  {
    return -1;
  }
#if defined(OPENAMP_USE_TFM_NS_API) && !defined(CORE_CA35)
#define TF_M_CA35_CPU_ID         0
/*
 * Store the resource table address and length in backup registers to provide
 * resource table information to the Cortex A
 */
  status_tfm =  tfm_platform_cpu_set_rsc_tab(TF_M_CA35_CPU_ID, (uint32_t)rsc_tab_addr, rsc_size);
  if(status_tfm != TFM_PLATFORM_ERR_SUCCESS)
  {
    OPENAMP_log_info("OpenAMP", "Not able to store resource table in backup register( %d)\n",
                     status_tfm);
    return -1;
  }
#endif /* defined(OPENAMP_USE_TFM_NS_API) && !defined(CORE_CA35) */

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

int MX_OPENAMP_RPMsg_Init(int RPMsgRole, rpmsg_ns_bind_cb ns_bind_cb, IPCC_HandleTypeDef * hipcc)
{
  struct fw_rsc_vdev_vring *vring_rsc = NULL;
  struct virtio_device *vdev = NULL;
  int status = 0;

  if (rsc_table == NULL)
  {
    return -1;
  }

  /* USER CODE BEGIN MAILBOX_Init */

   /* USER CODE END PRE_VIRTIO_INIT */
  vdev = rproc_virtio_create_vdev(RPMsgRole, VDEV_ID, &rsc_table->vdev,
                                  rsc_io, hipcc, MAILBOX_Notify, NULL);
  if (vdev == NULL)
  {
    return -1;
  }

#if (OPENAMP_ROLE == RPMSG_REMOTE)
  OPENAMP_log_info("OpenAMP", "waiting remote processor initialisation...");
  OPENAMP_Wait_Vdev(vdev);
  OPENAMP_log_info("OpenAMP", " Ready for RPMsg communication");
#endif

  /* USER CODE BEGIN  POST_VIRTIO_INIT */

  /* USER CODE END POST_VIRTIO_INIT */
  vring_rsc = &rsc_table->vring0;

  if(RPMsgRole == RPMSG_HOST && vring_rsc->da == -1) {
	  vring_rsc->da = VRING_RX_ADDRESS;
  }

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

  if(RPMsgRole == RPMSG_HOST && vring_rsc->da == -1) {
    vring_rsc->da = VRING_TX_ADDRESS;
  }

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

  MAILBOX_DeInit(hipcc);

  /* USER CODE BEGIN POST_OPENAMP_DEINIT */

  /* USER CODE END POST_OPENAMP_DEINIT */
}

void OPENAMP_init_ept(struct rpmsg_endpoint *ept)
{
  if (ept == NULL)
  {
    return;
  }

  (void)memset(ept, 0, sizeof(*ept));
  ept->addr = RPMSG_ADDR_ANY;
  ept->dest_addr = RPMSG_ADDR_ANY;
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

  ret = rpmsg_create_ept(ept, &rvdev.rdev, name, src, dest, cb, unbind_cb);

  /* USER CODE BEGIN POST_EP_CREATE */

  /* USER CODE END POST_EP_CREATE */
  return ret;
}

int OPENAMP_check_for_message(void)
{
  /* USER CODE BEGIN MSG_CHECK */

  /* USER CODE END MSG_CHECK */
#if defined (OPENAMP_USE_FREERTOS)
  return rproc_virtio_notified(rvdev.vdev, RSC_NOTIFY_ID_ANY);
#else
  return MAILBOX_Poll(rvdev.vdev);
#endif
}

void OPENAMP_Wait_EndPointready(struct rpmsg_endpoint *rp_ept)
{
  if (rp_ept == NULL)
  {
    return;
  }

  while (!is_rpmsg_ept_ready(rp_ept))
  {
    (void)OPENAMP_check_for_message();
  }
}


#if (OPENAMP_ROLE == RPMSG_REMOTE)
void OPENAMP_Wait_Remote_Ready(void)
{
  /* USER CODE BEGIN MSG_CHECK */

  /* USER CODE END MSG_CHECK */
  OPENAMP_Wait_Vdev(rvdev.vdev);
}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
