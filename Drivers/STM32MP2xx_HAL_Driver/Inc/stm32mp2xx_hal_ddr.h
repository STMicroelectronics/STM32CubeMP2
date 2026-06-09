/**
  ******************************************************************************
  * @file    stm32mp2xx_hal_ddr.h
  * @author  MCD Application Team
  * @brief   Header file of DDR HAL module.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STM32MP2xx_HAL_DDR_H
#define STM32MP2xx_HAL_DDR_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include "stm32mp2xx_hal_def.h"

#if defined (DDRC)

/** @addtogroup STM32MP2xx_HAL_Driver
  * @{
  */

/** @addtogroup DDR
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup DDR_Exported_Types DDR Exported Types
  * @{
  */

/* Maximum number of PHY pstates used by the driver */
#define HAL_DDR_MAX_PSTATE 1

/* Maximum Number of swizzling parameters */
#define HAL_DDR_MAX_SWIZZLE_PARAM 44

/**
  * @brief  HAL DDR type definition
  */
typedef enum
{
  STM32MP_DDR3,
  STM32MP_DDR4,
  STM32MP_LPDDR4,
} ddr_type;

/**
  * @brief  HAL DDR Self Refresh modes definition
  */
typedef enum
{
  HAL_DDR_SW_SELF_REFRESH_MODE = 0x0U,   /*!< DDR Software Self Refresh Mode */
  HAL_DDR_AUTO_SELF_REFRESH_MODE = 0x1U, /*!< DDR Automatic Self Refresh Mode */
  HAL_DDR_HW_SELF_REFRESH_MODE = 0x2U,   /*!< DDR Hardware Self Refresh Mode */
  HAL_DDR_INVALID_MODE = 0x3U,           /*!< DDR Invalid Self Refresh Mode */
} HAL_DDR_SelfRefreshModeTypeDef;

/**
  * @brief  HAL DDR settings definition
  */
typedef struct
{
  const char *name;
  uint32_t speed; /* in kHZ */
  uint64_t size; /* Memory size in byte = col * row * width */
} HAL_DDR_InfoTypeDef;

typedef struct
{
  uint32_t MSTR;
  uint32_t MRCTRL0;
  uint32_t MRCTRL1;
  uint32_t MRCTRL2;
  uint32_t DERATEEN;
  uint32_t DERATEINT;
  uint32_t DERATECTL;
  uint32_t PWRCTL;
  uint32_t PWRTMG;
  uint32_t HWLPCTL;
  uint32_t RFSHCTL0;
  uint32_t RFSHCTL1;
  uint32_t RFSHCTL3;
  uint32_t CRCPARCTL0;
  uint32_t CRCPARCTL1;
  uint32_t INIT0;
  uint32_t INIT1;
  uint32_t INIT2;
  uint32_t INIT3;
  uint32_t INIT4;
  uint32_t INIT5;
  uint32_t INIT6;
  uint32_t INIT7;
  uint32_t DIMMCTL;
  uint32_t RANKCTL;
  uint32_t RANKCTL1;
  uint32_t ZQCTL0;
  uint32_t ZQCTL1;
  uint32_t ZQCTL2;
  uint32_t DFITMG0;
  uint32_t DFITMG1;
  uint32_t DFILPCFG0;
  uint32_t DFILPCFG1;
  uint32_t DFIUPD0;
  uint32_t DFIUPD1;
  uint32_t DFIUPD2;
  uint32_t DFIMISC;
  uint32_t DFITMG2;
  uint32_t DFITMG3;
  uint32_t DBICTL;
  uint32_t DFIPHYMSTR;
  uint32_t DBG0;
  uint32_t DBG1;
  uint32_t DBGCMD;
  uint32_t SWCTL;
  uint32_t SWCTLSTATIC;
  uint32_t POISONCFG;
  uint32_t PCCFG;
} HAL_DDR_RegTypeDef;

typedef struct
{
  uint32_t RFSHTMG;
  uint32_t RFSHTMG1;
  uint32_t DRAMTMG0;
  uint32_t DRAMTMG1;
  uint32_t DRAMTMG2;
  uint32_t DRAMTMG3;
  uint32_t DRAMTMG4;
  uint32_t DRAMTMG5;
  uint32_t DRAMTMG6;
  uint32_t DRAMTMG7;
  uint32_t DRAMTMG8;
  uint32_t DRAMTMG9;
  uint32_t DRAMTMG10;
  uint32_t DRAMTMG11;
  uint32_t DRAMTMG12;
  uint32_t DRAMTMG13;
  uint32_t DRAMTMG14;
  uint32_t DRAMTMG15;
  uint32_t ODTCFG;
  uint32_t ODTMAP;
} HAL_DDR_TimingTypeDef;

typedef struct
{
  uint32_t ADDRMAP0;
  uint32_t ADDRMAP1;
  uint32_t ADDRMAP2;
  uint32_t ADDRMAP3;
  uint32_t ADDRMAP4;
  uint32_t ADDRMAP5;
  uint32_t ADDRMAP6;
  uint32_t ADDRMAP7;
  uint32_t ADDRMAP8;
  uint32_t ADDRMAP9;
  uint32_t ADDRMAP10;
  uint32_t ADDRMAP11;
} HAL_DDR_MapTypeDef;

typedef struct
{
  uint32_t SCHED;
  uint32_t SCHED1;
  uint32_t PERFHPR1;
  uint32_t PERFLPR1;
  uint32_t PERFWR1;
  uint32_t SCHED3;
  uint32_t SCHED4;
  uint32_t PCFGR_0;
  uint32_t PCFGW_0;
  uint32_t PCTRL_0;
  uint32_t PCFGQOS0_0;
  uint32_t PCFGQOS1_0;
  uint32_t PCFGWQOS0_0;
  uint32_t PCFGWQOS1_0;
#if STM32MP_DDR_DUAL_AXI_PORT
  uint32_t PCFGR_1;
  uint32_t PCFGW_1;
  uint32_t PCTRL_1;
  uint32_t PCFGQOS0_1;
  uint32_t PCFGQOS1_1;
  uint32_t PCFGWQOS0_1;
  uint32_t PCFGWQOS1_1;
#endif /* STM32MP_DDR_DUAL_AXI_PORT */
} HAL_DDR_PerfTypeDef;

typedef struct
{
  int32_t dramtype;
  int32_t dimmtype;
  int32_t lp4xmode;
  int32_t numdbyte;
  int32_t numactivedbytedfi0;
  int32_t numactivedbytedfi1;
  int32_t numanib;
  int32_t numrank_dfi0;
  int32_t numrank_dfi1;
  int32_t dramdatawidth;
  int32_t numpstates;
  int32_t frequency[HAL_DDR_MAX_PSTATE];
  int32_t pllbypass[HAL_DDR_MAX_PSTATE];
  int32_t dfifreqratio[HAL_DDR_MAX_PSTATE];
  int32_t dfi1exists;
  int32_t train2d;
  int32_t hardmacrover;
  int32_t readdbienable[HAL_DDR_MAX_PSTATE];
  int32_t dfimode;
} HAL_DDR_BasicUiTypeDef;

typedef struct
{
  int32_t lp4rxpreamblemode[HAL_DDR_MAX_PSTATE];
  int32_t lp4postambleext[HAL_DDR_MAX_PSTATE];
  int32_t d4rxpreamblelength[HAL_DDR_MAX_PSTATE];
  int32_t d4txpreamblelength[HAL_DDR_MAX_PSTATE];
  int32_t extcalresval;
  int32_t is2ttiming[HAL_DDR_MAX_PSTATE];
  int32_t odtimpedance[HAL_DDR_MAX_PSTATE];
  int32_t tximpedance[HAL_DDR_MAX_PSTATE];
  int32_t atximpedance;
  int32_t memalerten;
  int32_t memalertpuimp;
  int32_t memalertvreflevel;
  int32_t memalertsyncbypass;
  int32_t disdynadrtri[HAL_DDR_MAX_PSTATE];
  int32_t phymstrtraininterval[HAL_DDR_MAX_PSTATE];
  int32_t phymstrmaxreqtoack[HAL_DDR_MAX_PSTATE];
  int32_t wdqsext;
  int32_t calinterval;
  int32_t calonce;
  int32_t lp4rl[HAL_DDR_MAX_PSTATE];
  int32_t lp4wl[HAL_DDR_MAX_PSTATE];
  int32_t lp4wls[HAL_DDR_MAX_PSTATE];
  int32_t lp4dbird[HAL_DDR_MAX_PSTATE];
  int32_t lp4dbiwr[HAL_DDR_MAX_PSTATE];
  int32_t lp4nwr[HAL_DDR_MAX_PSTATE];
  int32_t lp4lowpowerdrv;
  int32_t drambyteswap;
  int32_t rxenbackoff;
  int32_t trainsequencectrl;
  int32_t snpsumctlopt;
  int32_t snpsumctlf0rc5x[HAL_DDR_MAX_PSTATE];
  int32_t txslewrisedq[HAL_DDR_MAX_PSTATE];
  int32_t txslewfalldq[HAL_DDR_MAX_PSTATE];
  int32_t txslewriseac;
  int32_t txslewfallac;
  int32_t disableretraining;
  int32_t disablephyupdate;
  int32_t enablehighclkskewfix;
  int32_t disableunusedaddrlns;
  int32_t phyinitsequencenum;
  int32_t enabledficspolarityfix;
  int32_t phyvref;
  int32_t sequencectrl[HAL_DDR_MAX_PSTATE];
} HAL_DDR_AdvancedUiTypeDef;

typedef struct
{
  int32_t mr0[HAL_DDR_MAX_PSTATE];
  int32_t mr1[HAL_DDR_MAX_PSTATE];
  int32_t mr2[HAL_DDR_MAX_PSTATE];
  int32_t mr3[HAL_DDR_MAX_PSTATE];
  int32_t mr4[HAL_DDR_MAX_PSTATE];
  int32_t mr5[HAL_DDR_MAX_PSTATE];
  int32_t mr6[HAL_DDR_MAX_PSTATE];
  int32_t mr11[HAL_DDR_MAX_PSTATE];
  int32_t mr12[HAL_DDR_MAX_PSTATE];
  int32_t mr13[HAL_DDR_MAX_PSTATE];
  int32_t mr14[HAL_DDR_MAX_PSTATE];
  int32_t mr22[HAL_DDR_MAX_PSTATE];
} HAL_DDR_ModeRegisterUiTypeDef;

typedef struct
{
  int32_t swizzle[HAL_DDR_MAX_SWIZZLE_PARAM];
} HAL_DDR_SwizzleUiTypeDef;

typedef struct
{
  HAL_DDR_InfoTypeDef           info;
  HAL_DDR_RegTypeDef            c_reg;
  HAL_DDR_TimingTypeDef         c_timing;
  HAL_DDR_MapTypeDef            c_map;
  HAL_DDR_PerfTypeDef           c_perf;
  HAL_DDR_BasicUiTypeDef        p_uib;
  HAL_DDR_AdvancedUiTypeDef     p_uia;
  HAL_DDR_ModeRegisterUiTypeDef p_uim;
  HAL_DDR_SwizzleUiTypeDef      p_uis;
} HAL_DDR_ConfigTypeDef;

/**
  * @brief  DDR Initialization Structure definition
  */
typedef struct
{
  bool wakeup_from_standby; /*!< [input]
                                 Specifies if the system is waking up from the
                                 standby mode. The application is in charge to
                                 get this information. It will impact the self
                                 refresh state and the related steps within
                                 the initialization sequence. */

  bool self_refresh;        /*!< [output]
                                 Specifies the DDR self refresh status.
                                 Self Refresh enabled if true. */

  uint32_t zdata;           /*!< [input]
                                 Specifies the ZQ calibration restored value
                                 if any. Only used by the initialization
                                 sequence if wakeup_from_standby is true. The
                                 application is in charge of saving/restoring
                                 it during low power transitions. Value is
                                 obtaines when calling HAL_DDR_SR_Entry. */

  bool clear_bkp;           /*!< [output]
                                 Specifies if backup should be cleared after
                                 DDR initialization (DDR lost content case).
                                 Clear requested if true. */
} DDR_InitTypeDef;

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup DDR_Exported_Constants DDR Exported Constants
  * @{
  */

/**
  * @}
  */

/* Exported macros -----------------------------------------------------------*/
/** @defgroup DDR_Exported_Macros DDR Exported Macros
  * @{
  */

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @addtogroup DDR_Exported_Functions
  * @{
  */

HAL_StatusTypeDef HAL_DDR_Init(DDR_InitTypeDef *iddr);
HAL_StatusTypeDef HAL_DDR_SR_Entry(uint32_t *zq0cr0_zdata);
HAL_StatusTypeDef HAL_DDR_STDBY_SR_Entry(void);
HAL_StatusTypeDef HAL_DDR_SR_Exit(void);
HAL_StatusTypeDef HAL_DDR_SR_SetMode(HAL_DDR_SelfRefreshModeTypeDef mode);
HAL_DDR_SelfRefreshModeTypeDef HAL_DDR_SR_ReadMode(void);
HAL_StatusTypeDef HAL_DDR_SetRetentionAreaBase(uint64_t base);
HAL_StatusTypeDef HAL_DDR_SaveRetentionData(void);

/**
  * @}
  */

/* Private types -------------------------------------------------------------*/
/** @defgroup DDR_Private_Types DDR Private Types
  * @{
  */

/**
  * @}
  */

/* Private variables ---------------------------------------------------------*/
/** @defgroup DDR_Private_Variables DDR Private Variables
  * @{
  */

/**
  * @}
  */

/* Private constants ---------------------------------------------------------*/
/** @defgroup DDR_Private_Constants DDR Private Constants
  * @{
  */

/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/** @defgroup DDR_Private_Macros DDR Private Macros
  * @{
  */

/**
  * @}
  */
/* Private functions ---------------------------------------------------------*/
/** @defgroup DDR_Private_Functions DDR Private Functions
  * @{
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#endif /* DDRC */

#ifdef __cplusplus
}
#endif

#endif /* STM32MP2xxHAL_DDR_H */
