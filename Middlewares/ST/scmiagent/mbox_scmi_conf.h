/**
  ******************************************************************************
  * @file    mbox_scmi_conf.h
  * @author  MCD Application Team
  * @brief   SCMI mailbox configuration file
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MBOX_SCMI_CONF_H
#define __MBOX_SCMI_CONF_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32mp2xx_hal.h"

/* Private includes ----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
typedef unsigned int phys_addr_t;     /* Physical address type */

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/

/* Private defines -----------------------------------------------------------*/
#define MBOX_SCMI_IPCC_TIMEOUT_CHAN_FREE_MS   1000
#define MBOX_SCMI_IPCC_TIMEOUT_CALLBACK_MS    1000

#define MBOX_SCMI_IPCC_CHANNEL                IPCC_CHANNEL_16

#if defined(STM32MP21xxxx)
  #define VOLTD_SCMI_ADC            3
  /* VOLTD_SCMI_UCPD is not needed yet for MP21 so not defined */
#elif defined(STM32MP23xxxx)
  #define VOLTD_SCMI_ADC            7
  #define VOLTD_SCMI_UCPD           5
#elif defined(STM32MP25xxxx)
  #define VOLTD_SCMI_ADC            7
  #define VOLTD_SCMI_UCPD           5
#else
  #error "SCMI Domain-Id not defined"
#endif



#if defined (__ICCARM__)
/* For IAR, the .icf file should contain the following lines:
 *
 * define symbol __SCMI_SHM_region_start__  = BASE_ADDRESS; (0x0A042000)     (for MP25 & MP23)
 * OR
 * define symbol __SCMI_SHM_region_start__  = BASE_ADDRESS; (0x0A062000)     (for MP21)
 * define symbol __SCMI_SHM_region_size__   = MEM_SIZE;     (0x1000)
 *
 * export symbol __SCMI_SHM_region_start__;
 * export symbol __SCMI_SHM_region_size__;
 */
extern const uint32_t     __SCMI_SHM_region_start__;
extern const uint32_t     __SCMI_SHM_region_size__;

#define SCMI_SHM_START_ADDRESS     (phys_addr_t)(&__SCMI_SHM_region_start__)
#define SMT_SHARED_MEMORY_SIZE     (size_t)(&__SCMI_SHM_region_size__)

#elif defined(__CC_ARM)
/* For MDK-ARM, the scatter file .sct should contain the following lines:
 *
 * LR_IROM1 ....
 * {
 *   ...
 *   __SCMI_SHMEM__ 0x0A042000  EMPTY 0x1000 {} ; Shared Memory area used by SCMI  (for MP25 & MP23)
 *   OR
 *   __SCMI_SHMEM__ 0x0A062000  EMPTY 0x1000 {} ; Shared Memory area used by SCMI  (for MP21)
 * }
 */
extern unsigned int Image$$__SCMI_SHMEM__$$Base;
extern unsigned int Image$$__SCMI_SHMEM__$$ZI$$Length;

#define SCMI_SHM_START_ADDRESS    (phys_addr_t)(&Image$$__SCMI_SHMEM__$$Base)
#define SMT_SHARED_MEMORY_SIZE    (size_t)(&Image$$__SCMI_SHMEM__$$ZI$$Length)

#else   /* __ICCARM__ , __CC_ARM */
/* For GCC add the following content to the .ld file:
 *
 * MEMORY
 * {
 *   ...
 *   SCMI_SHMEM    (rw)    : ORIGIN = 0x0A042000,	LENGTH = 0x1000 (for MP25 & MP23)
 *   OR
 *   SCMI_SHMEM    (rw)    : ORIGIN = 0x0A062000,	LENGTH = 0x1000 (for MP21)
 * }
 *
 * __SCMI_SHM_region_start__  = ORIGIN(SCMI_SHMEM);
 * __SCMI_SHM_region_end__    = ORIGIN(SCMI_SHMEM)+LENGTH(SCMI_SHMEM);
 */

extern unsigned int __SCMI_SHM_region_start__;  /* defined by linker script */
extern unsigned int __SCMI_SHM_region_end__;    /* defined by linker script */

#define SCMI_SHM_START_ADDRESS       (phys_addr_t)(&__SCMI_SHM_region_start__)
#define SMT_SHARED_MEMORY_SIZE       (128)

/* Actual size of memory reserved for SCMI_SHM is 4096 Bytes (refer to linker file)
 * But as of now only 128*2 Bytes are in use (128 for TX & 128 for RX)
 * That is the reason SMT_SHARED_MEMORY_SIZE is being harcoded as 128 Bytes
 * Below commented SMT_SHARED_MEMORY_SIZE macro can be used when
 * full 4KB SCMI_SHM will be used */
/*
#define SMT_SHARED_MEMORY_SIZE       (size_t)( (size_t)(&__SCMI_SHM_region_end__) - (size_t)(&__SCMI_SHM_region_start__) )
*/

#endif  /* __ICCARM__ , __CC_ARM */



#ifdef __cplusplus
}
#endif
#endif /* __MBOX_SCMI_CONF_H */

