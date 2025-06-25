/**
  ******************************************************************************
  * @file           : res_mgr.h
  * @brief          : Header for res_mgr.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef RES_MGR_H
#define RES_MGR_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stdio.h"
#include "string.h"
#include "stm32mp2xx_hal.h"
#if defined(STM32MP25xxxx)
#include "res_mgr_stm32mp25xxxx.h"
#elif defined(STM32MP21xxxx)
#include "res_mgr_stm32mp21xxxx.h"
#elif defined(STM32MP23xxxx)
#include "res_mgr_stm32mp23xxxx.h"
#else
#error "Please select appropriate SoC Preprocessor..."
#endif

/* Exported macros -----------------------------------------------------------*/
#if !defined(TRUE)
#define TRUE 1
#endif /* TRUE */

#if !defined(FALSE)
#define FALSE 0
#endif /* FALSE */

#define RESMGR_CIDCFGR_CFEN_Pos      (0U)
#define RESMGR_CIDCFGR_CFEN_Msk      (0x1U << RESMGR_CIDCFGR_CFEN_Pos)        /*!< 0x00000001 */
#define RESMGR_CIDCFGR_CFEN          RESMGR_CIDCFGR_CFEN_Msk

#define RESMGR_CIDCFGR_SEMEN_Pos     (1U)
#define RESMGR_CIDCFGR_SEMEN_Msk     (0x1U << RESMGR_CIDCFGR_SEMEN_Pos)       /*!< 0x00000002 */
#define RESMGR_CIDCFGR_SEMEN         RESMGR_CIDCFGR_SEMEN_Msk

#define RESMGR_CIDCFGR_SCID_Pos      (4U)
#define RESMGR_CIDCFGR_SCID_Msk      (0x7U << RESMGR_CIDCFGR_SCID_Pos)        /*!< 0x00000070 */
#define RESMGR_CIDCFGR_SCID          RESMGR_CIDCFGR_SCID_Msk
#define RESMGR_CIDCFGR_SCID0         (0x0U << RESMGR_CIDCFGR_SCID_Pos)        /*!< 0x00000000 */
#define RESMGR_CIDCFGR_SCID1         (0x1U << RESMGR_CIDCFGR_SCID_Pos)        /*!< 0x00000010 */
#define RESMGR_CIDCFGR_SCID2         (0x2U << RESMGR_CIDCFGR_SCID_Pos)        /*!< 0x00000020 */
#define RESMGR_CIDCFGR_SCID3         (0x3U << RESMGR_CIDCFGR_SCID_Pos)        /*!< 0x00000030 */
#define RESMGR_CIDCFGR_SCID4         (0x4U << RESMGR_CIDCFGR_SCID_Pos)        /*!< 0x00000040 */
#define RESMGR_CIDCFGR_SCID5         (0x5U << RESMGR_CIDCFGR_SCID_Pos)        /*!< 0x00000050 */
#define RESMGR_CIDCFGR_SCID6         (0x6U << RESMGR_CIDCFGR_SCID_Pos)        /*!< 0x00000060 */
#define RESMGR_CIDCFGR_SCID7         (0x7U << RESMGR_CIDCFGR_SCID_Pos)        /*!< 0x00000070 */

#define RESMGR_CIDCFGR_SEMWL_Pos     (16U)
#define RESMGR_CIDCFGR_SEMWL_Msk     (0xFFU << RESMGR_CIDCFGR_SEMWL_Pos)      /*!< 0x00FF0000 */
#define RESMGR_CIDCFGR_SEMWL         RESMGR_CIDCFGR_SEMWL_Msk
#define RESMGR_CIDCFGR_SEMWLC0       (0x1U << RESMGR_CIDCFGR_SEMWL_Pos)       /*!< 0x00010000 */
#define RESMGR_CIDCFGR_SEMWLC1       (0x2U << RESMGR_CIDCFGR_SEMWL_Pos)       /*!< 0x00020000 */
#define RESMGR_CIDCFGR_SEMWLC2       (0x4U << RESMGR_CIDCFGR_SEMWL_Pos)       /*!< 0x00040000 */
#define RESMGR_CIDCFGR_SEMWLC3       (0x8U << RESMGR_CIDCFGR_SEMWL_Pos)       /*!< 0x00080000 */
#define RESMGR_CIDCFGR_SEMWLC4       (0x10U << RESMGR_CIDCFGR_SEMWL_Pos)      /*!< 0x00100000 */
#define RESMGR_CIDCFGR_SEMWLC5       (0x20U << RESMGR_CIDCFGR_SEMWL_Pos)      /*!< 0x00200000 */
#define RESMGR_CIDCFGR_SEMWLC6       (0x40U << RESMGR_CIDCFGR_SEMWL_Pos)      /*!< 0x00400000 */
#define RESMGR_CIDCFGR_SEMWLC7       (0x80U << RESMGR_CIDCFGR_SEMWL_Pos)      /*!< 0x00800000 */


#define RESMGR_SEMCR_SEMMUTEX_Pos    (0U)
#define RESMGR_SEMCR_SEMMUTEX_Msk    (0x1U << RESMGR_SEMCR_SEMMUTEX_Pos)      /*!< 0x00000001 */
#define RESMGR_SEMCR_SEMMUTEX        RESMGR_SEMCR_SEMMUTEX_Msk

#define RESMGR_SEMCR_SEMCID_Pos      (4U)
#define RESMGR_SEMCR_SEMCID_Msk      (0x7U << RESMGR_SEMCR_SEMCID_Pos)        /*!< 0x00000070 */
#define RESMGR_SEMCR_SEMCID          RESMGR_SEMCR_SEMCID_Msk
#define RESMGR_SEMCR_SEMCID0         (0x0U << RESMGR_SEMCR_SEMCID_Pos)        /*!< 0x00000000 */
#define RESMGR_SEMCR_SEMCID1         (0x1U << RESMGR_SEMCR_SEMCID_Pos)        /*!< 0x00000010 */
#define RESMGR_SEMCR_SEMCID2         (0x2U << RESMGR_SEMCR_SEMCID_Pos)        /*!< 0x00000020 */
#define RESMGR_SEMCR_SEMCID3         (0x3U << RESMGR_SEMCR_SEMCID_Pos)        /*!< 0x00000030 */
#define RESMGR_SEMCR_SEMCID4         (0x4U << RESMGR_SEMCR_SEMCID_Pos)        /*!< 0x00000040 */
#define RESMGR_SEMCR_SEMCID5         (0x5U << RESMGR_SEMCR_SEMCID_Pos)        /*!< 0x00000050 */
#define RESMGR_SEMCR_SEMCID6         (0x6U << RESMGR_SEMCR_SEMCID_Pos)        /*!< 0x00000060 */
#define RESMGR_SEMCR_SEMCID7         (0x7U << RESMGR_SEMCR_SEMCID_Pos)        /*!< 0x00000070 */

#define CPU_SECURE                      1
#define CPU_UNSECURE                    0

#define PRIVILEGED              0x01u   /* Privileged access */
#define UNPRIVILEGED            0x00u   /* Unprivileged access */

#define THREAD_MODE_PRIVILEGED      0x00   /* Thread mode has privileged access */
#define THREAD_MODE_UNPRIVILEGED    0x01   /* Thread mode has unprivileged access */


typedef enum
{
  RESMGR_RESOURCE_RIFSC,
  RESMGR_RESOURCE_RIF_PWR_WIO,
  RESMGR_RESOURCE_RIF_RCC,
#if defined (GPIOA)
  RESMGR_RESOURCE_RIF_GPIOA,
#endif /* GPIOA */
#if defined (GPIOB)
  RESMGR_RESOURCE_RIF_GPIOB,
#endif /* GPIOB */
#if defined (GPIOC)
  RESMGR_RESOURCE_RIF_GPIOC,
#endif /* GPIOC */
#if defined (GPIOD)
  RESMGR_RESOURCE_RIF_GPIOD,
#endif /* GPIOD */
#if defined (GPIOE)
  RESMGR_RESOURCE_RIF_GPIOE,
#endif /* GPIOE */
#if defined (GPIOF)
  RESMGR_RESOURCE_RIF_GPIOF,
#endif /* GPIOF */
#if defined (GPIOG)
  RESMGR_RESOURCE_RIF_GPIOG,
#endif /* GPIOG */
#if defined (GPIOH)
  RESMGR_RESOURCE_RIF_GPIOH,
#endif /* GPIOH */
#if defined (GPIOI)
  RESMGR_RESOURCE_RIF_GPIOI,
#endif /* GPIOI */
#if defined (GPIOJ)
  RESMGR_RESOURCE_RIF_GPIOJ,
#endif /* GPIOJ */
#if defined (GPIOK)
  RESMGR_RESOURCE_RIF_GPIOK,
#endif /* GPIOK */
#if defined (GPIOZ)
  RESMGR_RESOURCE_RIF_GPIOZ,
#endif /* GPIOZ */
  RESMGR_RESOURCE_RIF_FMC,
  RESMGR_RESOURCE_RIF_PWR,
  RESMGR_RESOURCE_RIF_RTC,
  RESMGR_RESOURCE_RIF_EXTI1,
  RESMGR_RESOURCE_RIF_EXTI2,
  RESMGR_RESOURCE_RIF_HPDMA1,
  RESMGR_RESOURCE_RIF_HPDMA2,
  RESMGR_RESOURCE_RIF_HPDMA3,
  RESMGR_RESOURCE_MAX,
} ResMgr_Res_Type_t;


/* Internal types ------------------------------------------------------------*/
typedef enum
{
  RESMGR_COMP_CID_ONLY,
  RESMGR_COM_CID_WITH_SEMCR
} ResMgr_CID_type_t;
typedef struct
{
  __IO uint32_t CIDCFGR;
  __IO uint32_t SEMCR;
} ResMgr_Comp_t;
typedef struct
{
  ResMgr_Comp_t *cid_base;
  __IO uint32_t *priv_base;
  __IO uint32_t *sec_base;
  __IO uint32_t *CIDCFGR;
  __IO uint32_t *SEMCR;
  ResMgr_CID_type_t cid_type;
  ResMgr_Res_Type_t res_type;
  uint32_t max_res_num;
} ResMgr_data_info_t;

/* Exported types ------------------------------------------------------------*/
#define RESMGR_PWR_RESOURCE(x)       (x)
#define RESMGR_RCC_RESOURCE(x)       (x)
#define RESMGR_GPIO_PIN(x)           (x)
#define RESMGR_PWR_RESOURCE_WIO(x)   (x-1)
#define RESMGR_HPDMA_CHANNEL(x)      (x)
#define RESMGR_EXTI_RESOURCE(x)      (x)
#define RESMGR_RTC_RESOURCE(x)       (x)


#define RESMGR_RTC_PROT_BITS_POS(__RES_NUM__)       (__RES_NUM__ < 3 ? __RES_NUM__ : (__RES_NUM__ + 9))

/* Returns the poistion of least significant set bit in x */
#define RESMGR_LSB_SET_BIT_POS(__RES__)         ( ((unsigned int)__RES__ \
                                                   != 0) ?  __builtin_ctz((unsigned int)__RES__) : 32 )

#define RESMGR_RIFSC_PWR_RSC_NB     NON_SHAREABLE_RESOURCE_NB
#define RESMGR_RIFSC_PWR_WIO_RSC_NB     SHAREABLE_RESOURCE_NB
/* Maximum Resource for each GPIO Port */
#define RESMGR_RIFSC_GPIO_RSC_NB        15
#define RESMGR_RIFSC_HPDMA_RSC_NB       15
#define RESMGR_RIFSC_RCC_RSC_NB         113
#define RESMGR_RIFSC_EXTI1_RSC_NB       EXTI1_LINE_NB
#define RESMGR_RIFSC_EXTI2_RSC_NB       EXTI2_LINE_NB
#define RESMGR_RIFSC_FMC_RSC_NB         5
#define RESMGR_RIFSC_RTC_RSC_NB         5


typedef enum
{
  RESMGR_STATUS_ACCESS_OK              = 0x00U,
  RESMGR_STATUS_NPRIV_ACCESS_ERROR     = 0x01U,
  RESMGR_STATUS_NSEC_ACCESS_ERROR      = 0x02U,
  RESMGR_STATUS_SCID_ACCESS_ERROR      = 0x03U,
  RESMGR_STATUS_CID_WL_ACCESS_ERROR    = 0x04U,
  RESMGR_STATUS_SEM_ACCESS_ERROR       = 0x05U,
  RESMGR_STATUS_ACCESS_ERROR           = 0x06U,
  RESMGR_STATUS_ERROR_NONE             = 0x07U,
  RESMGR_STATUS_RES_TYP_ERROR          = 0x08U,
  RESMGR_STATUS_RES_NUM_ERROR          = 0x09U
} ResMgr_Status_t;

typedef enum
{
  RESMGR_SEM_STATUS_TAKEN            = 0x00U,
  RESMGR_SEM_STATUS_RELEASED         = 0x01U,
  RESMGR_SEM_STATUS_ERROR            = 0x02U
} ResMgr_Sem_Status_t;


/* Private macros ------------------------------------------------------------*/
/** @defgroup RESMGR_Private_Macros RESMGR Private Macros
  * @{
  */
#define IS_RESMGR_RES_TYPE(__TYPE__)     ((ResMgr_Res_Type_t )(__TYPE__) < (ResMgr_Res_Type_t) RESMGR_RESOURCE_MAX)

#define IS_RESMGR_RIFSC_RSC(__NUM__)     ((__NUM__) <= RESMGR_RIFSC_ID_MAX)

#define IS_RESMGR_PWR_RSC(__NUM__)       ((__NUM__) < RESMGR_RIFSC_PWR_RSC_NB)

#define IS_RESMGR_PWR_RSC_WIO(__NUM__)   ((__NUM__) < RESMGR_RIFSC_PWR_WIO_RSC_NB)

#define IS_RESMGR_RCC_RSC(__NUM__)       ((__NUM__) <= RESMGR_RIFSC_RCC_RSC_NB)

#define IS_RESMGR_GPIO_RSC(__NUM__)      ((__NUM__) <= RESMGR_RIFSC_GPIO_RSC_NB)

#define IS_RESMGR_EXTI1_RSC(__NUM__)     ((__NUM__) <  RESMGR_RIFSC_EXTI1_RSC_NB)

#define IS_RESMGR_EXTI2_RSC(__NUM__)     ((__NUM__) < RESMGR_RIFSC_EXTI2_RSC_NB)

#define IS_RESMGR_FMC_RSC(__NUM__)       ((__NUM__) <= RESMGR_RIFSC_FMC_RSC_NB)

#define IS_RESMGR_HPDMA_RSC(__NUM__)     ((__NUM__) <= RESMGR_RIFSC_HPDMA_RSC_NB)

#define IS_RESMGR_RTC_RSC(__NUM__)       ((__NUM__) <= RESMGR_RIFSC_RTC_RSC_NB)

#define IS_RESMGR_GPIO_PIN(__PIN__)      ((((uint32_t)(__PIN__) & RESMGR_GPIO_PIN_MASK) != 0x00U) &&\
                                          (((uint32_t)(__PIN__) & ~RESMGR_GPIO_PIN_MASK) == 0x00U))

/* Exported functions ------------------------------------------------------- */
ResMgr_Status_t ResMgr_Request(ResMgr_Res_Type_t res_type, uint8_t res_num);
ResMgr_Status_t ResMgr_Release(ResMgr_Res_Type_t res_type, uint8_t res_num);

ResMgr_Status_t ResMgr_GPIO_Request(ResMgr_Res_Type_t res_type, uint16_t pin);
ResMgr_Status_t ResMgr_GPIO_Release(ResMgr_Res_Type_t res_type, uint16_t pin);

#ifdef __cplusplus
}
#endif

#endif /* RES_MGR_H */
