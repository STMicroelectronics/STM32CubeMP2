/**
  ******************************************************************************
  * @file    stm32mp2xx_hal_lvds.h
  * @author  MCD Application Team
  * @brief   Header file of LVDS HAL module.
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
#ifndef __STM32MP2xx_HAL_LVDS_H
#define __STM32MP2xx_HAL_LVDS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32mp2xx_hal_def.h"

#if defined (LVDS)
/** @addtogroup STM32MP2xx_HAL_Driver
  * @{
  */

/** @addtogroup LVDS LVDS
  * @brief LVDS HAL module driver
  * @{
  */

/* Exported constants --------------------------------------------------------*/

/** @defgroup LVDS_Exported_Constants LVDS Exported Constants
  * @{
  */

/**
  * @defgroup LVDS_Number_Of_Links LVDS Number Of Links
  * @{
  */
#define LVDS_ONE_LINK   (uint32_t)0x00000000U
#define LVDS_TWO_LINKS    (uint32_t)0x00000001U
/**
  * @}
  */

/**
  * @defgroup LVDS_Number_Of_Displays LVDS Number of displays
  * @{
  */
#define LVDS_ONE_DISPLAY      (uint32_t)0x00000000U
#define LVDS_TWO_DISPLAYS     (uint32_t)0x00000001U
/**
  * @}
  */

/**
  * @defgroup LVDS_Data_Mapping LVDS Data Mapping
  * @{
  */
#define LVDS_DT_MAP_JEIDA     0x00U
#define LVDS_DT_MAP_VESA      0x01U
/**
  * @}
  */

/**
  * @defgroup LVDS_DISPLAY_STATE Enabled or Disabled
  * @{
  */
#define LVDS_DISPLAY_DISABLE    0x00000000U
#define LVDS_DISPLAY_ENABLE     LVDS_CR_LVDSEN
/**
  * @}
  */

/**
  * @defgroup LVDS_HSYNC_Polarity LVDS HSYNC Signal Polarity
  * @{
  */
#define LVDS_HS_POLARITY_NORMAL   0x00000000U
#define LVDS_HS_POLARITY_INVERTED LVDS_CR_HSPOL
/**
  * @}
  */

/**
  * @defgroup LVDS_VSYNC_Active_Polarity LVDS VSYNC Active Signal Polarity
  * @{
  */
#define LVDS_VS_POLARITY_NORMAL   0x00000000U
#define LVDS_VS_POLARITY_INVERTED LVDS_CR_VSPOL
/**
  * @}
  */

/**
  * @defgroup LVDS_DATA_ENABLE_Polarity LVDS DE Signal Polarity
  * @{
  */
#define LVDS_DE_POLARITY_NORMAL   0x00000000U
#define LVDS_DE_POLARITY_INVERTED LVDS_CR_DEPOL
/**
  * @}
  */

/**
  * @defgroup LVDS_Link_Phase LVDS Links Phase
  * @{
  */
#define LVDS_PHASE_EAO        0x00000000U
#define LVDS_PHASE_EBO        LVDS_CR_LKPHA
/**
  * @}
  */

/**
  * @defgroup LVDS_Link_Selection LVDS Link Selection Macro
  * @{
  */
#define LVDS_LINK1_SELECT     0x0U
#define LVDS_LINK2_SELECT     0x1U
/**
  * @}
  */

/**
  * @defgroup LVDS_Color_Coding LVDS Color Coding
  * @{
  */
#define LVDS_RGB666         0x00U
#define LVDS_RGB888         0x01U
/**
  * @}
  */

/**
  * @defgroup LVDS_PRBS LVDS PRBS
  * @{
  */
#define LVDS_PRBS_07        0x00000000U
#define LVDS_PRBS_15        LVDS_CDL2CR_PRBS_0
#define LVDS_PRBS_23        LVDS_CDL2CR_PRBS_1
#define LVDS_PRBS_31        (LVDS_CDL2CR_PRBS_0 | LVDS_CDL2CR_PRBS_1)
/**
  * @}
  */

/**
  * @defgroup LVDS_Channel_Distribution_values LVDS channel Distribution values
  * @{
  */
#define LVDS_CLOCK_LANE_MST     0x0UL
#define LVDS_DATA_LANE_0      0x1UL
#define LVDS_DATA_LANE_1      0x2UL
#define LVDS_DATA_LANE_2      0x3UL
#define LVDS_DATA_LANE_3      0x4UL
#define LVDS_CLOCK_LANE_SLV     0x5UL
#define LVDS_DATA_LANE_4      0x6UL
#define LVDS_DATA_LANE_5      0x7UL
#define LVDS_DATA_LANE_6      0x8UL
#define LVDS_DATA_LANE_7      0x9UL
#define LVDS_PRBS         0xAU
/**
  * @}
  */

/**
  * @defgroup HAL_LVDS_MAP LVDS MAP Type value defines
  * @{
  */

#define HAL_LVDS_MAP_R_0        (uint32_t)0x00000000U
#define HAL_LVDS_MAP_R_1        (uint32_t)0x00000001U
#define HAL_LVDS_MAP_R_2        (uint32_t)0x00000002U
#define HAL_LVDS_MAP_R_3        (uint32_t)0x00000003U
#define HAL_LVDS_MAP_R_4        (uint32_t)0x00000004U
#define HAL_LVDS_MAP_R_5        (uint32_t)0x00000005U
#define HAL_LVDS_MAP_R_6        (uint32_t)0x00000006U
#define HAL_LVDS_MAP_R_7        (uint32_t)0x00000007U
#define HAL_LVDS_MAP_G_0        (uint32_t)0x00000008U
#define HAL_LVDS_MAP_G_1        (uint32_t)0x00000009U
#define HAL_LVDS_MAP_G_2        (uint32_t)0x0000000AU
#define HAL_LVDS_MAP_G_3        (uint32_t)0x0000000BU
#define HAL_LVDS_MAP_G_4        (uint32_t)0x0000000CU
#define HAL_LVDS_MAP_G_5        (uint32_t)0x0000000DU
#define HAL_LVDS_MAP_G_6        (uint32_t)0x0000000EU
#define HAL_LVDS_MAP_G_7        (uint32_t)0x0000000FU
#define HAL_LVDS_MAP_B_0        (uint32_t)0x00000010U
#define HAL_LVDS_MAP_B_1        (uint32_t)0x00000011U
#define HAL_LVDS_MAP_B_2        (uint32_t)0x00000012U
#define HAL_LVDS_MAP_B_3        (uint32_t)0x00000013U
#define HAL_LVDS_MAP_B_4        (uint32_t)0x00000014U
#define HAL_LVDS_MAP_B_5        (uint32_t)0x00000015U
#define HAL_LVDS_MAP_B_6        (uint32_t)0x00000016U
#define HAL_LVDS_MAP_B_7        (uint32_t)0x00000017U
#define HAL_LVDS_MAP_H_S        (uint32_t)0x00000018U
#define HAL_LVDS_MAP_V_S        (uint32_t)0x00000019U
#define HAL_LVDS_MAP_D_E        (uint32_t)0x0000001AU
#define HAL_LVDS_MAP_C_E        (uint32_t)0x0000001BU
#define HAL_LVDS_MAP_C_I          (uint32_t)0x0000001CU
#define HAL_LVDS_MAP_TOG          (uint32_t)0x0000001DU
#define HAL_LVDS_MAP_ONE          (uint32_t)0x0000001EU
#define HAL_LVDS_MAP_ZERO         (uint32_t)0x0000001FU
/**
  * @}
  */

/**
  * @defgroup PLL functional mode
  * @{
  */
#define LVDS_PLL_TWG_ENABLE    (uint8_t)0x01U
#define LVDS_PLL_SD_ENABLE     (uint8_t)0x10U
/**
  * @}
  */

/**
  * @defgroup LVDS_PLL_MODE_INT Integer mode : SD disable & TWG disable
  * @{
  */
#define LVDS_PLL_MODE_INT      (uint8_t)0x00U
/**
  * @}
  */

/**
  * @defgroup LVDS_PLL_MODE_FRAC Fractional mode : SD enable & TWG disable
  * @{
  */
#define LVDS_PLL_MODE_FRAC     LVDS_PLL_SD_ENABLE
/**
  * @}
  */

/**
  * @defgroup LVDS_PLL_MODE_SSC Spread Spectrum Clocking mode : SD enable & TWG enable
  * @{
  */
#define LVDS_PLL_MODE_SSC      (LVDS_PLL_SD_ENABLE | LVDS_PLL_TWG_ENABLE)
/**
  * @}
  */

/**
  * @defgroup HAL_LVDS_ERROR_CODE LVDS Error Codes
  * @{
  */

#define HAL_LVDS_ERROR_NONE     (uint8_t)0x00U
/**
  * @}
  */

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/

/** @defgroup LVDS_Exported_Macros LVDS Exported Macros
  * @{
  */

/**
  * @brief Enables the LVDS host.
  * @param __HANDLE__ LVDS host.
  * @retval None.
  */
#define __HAL_LVDS_ENABLE(__HANDLE__) do {                \
                                              __IO uint32_t tmpreg = 0x00U;                 \
                                              SET_BIT((__HANDLE__)->Instance->CR, LVDS_CR_LVDSEN);      \
                                              tmpreg = READ_BIT((__HANDLE__)->Instance->CR, LVDS_CR_LVDSEN);  \
                                              UNUSED(tmpreg);                         \
                                         } while(0U)

/**
  * @brief Disables the LVDS host.
  * @param __HANDLE__ LVDS host.
  * @retval None.
  */
#define __HAL_LVDS_DISABLE(__HANDLE__) do {               \
                                                __IO uint32_t tmpreg = 0x00U;                 \
                                                CLEAR_BIT((__HANDLE__)->Instance->CR, LVDS_CR_LVDSEN);      \
                                                tmpreg = READ_BIT((__HANDLE__)->Instance->CR, LVDS_CR_LVDSEN);  \
                                                UNUSED(tmpreg);                         \
                                          } while(0U)

/**
  * @brief Enables the LVDS PLL.
  * @param __HANDLE__ LVDS PLL.
  * @retval None.
  */
#define __HAL_LVDS_PLL_ENABLE(__HANDLE__) do {                        \
                                                  __IO uint32_t tmpreg = 0x00U;             \
                                                  SET_BIT((__HANDLE__)->Instance->PMPLLCR1, \
                                                  LVDS_PMPLLCR1_MST_PLL_ENABLE);            \
                                                  tmpreg = READ_BIT((__HANDLE__)->Instance->PMPLLCR1, \
                                                  LVDS_PMPLLCR1_MST_PLL_ENABLE);                      \
                                                  UNUSED(tmpreg);                                     \
                                             } while(0U)

/**
  * @brief Enables the LVDS PLL.
  * @param __HANDLE__ LVDS PLL.
  * @retval None.
  */
#define __HAL_LVDS_PLL_DISABLE(__HANDLE__) do {                       \
                                                    __IO uint32_t tmpreg = 0x00U;               \
                                                    CLEAR_BIT((__HANDLE__)->Instance->PMPLLCR1, \
                                                    LVDS_PMPLLCR1_MST_PLL_ENABLE);              \
                                                    tmpreg = READ_BIT((__HANDLE__)->Instance->PMPLLCR1, \
                                                    LVDS_PMPLLCR1_MST_PLL_ENABLE);                      \
                                                    UNUSED(tmpreg);                                     \
                                              } while(0U)


#define __HAL_LVDS_PLL_LOCKED(__HANDLE__) (!(((__HANDLE__)->Instance->PMSSR1) & LVDS_PMSSR1_MST_DRV_A0_SPARE_RD & 1))
#define __HAL_LVDS_PLL_LOCKED_BIS(__HANDLE__) ((__HANDLE__)->Instance->PMPLLSR & LVDS_PMPLLSR_MST_PLL_LOCK)

/**
  * @}
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup LVDS_Exported_Types LVDS Exported Types
  * @{
  */
/**
  * @brief LVDS Init Structure definition
  */
typedef struct
{
  uint32_t DataMapping;           /*!< Data Mapping
                        This parameter can be any value of @ref LVDS_Data_Mapping */

  uint32_t ColorCoding;           /*!< Color coding for LTDC interface
                        This parameter can be any value of @ref LVDS_Color_Coding */

} LVDS_InitTypeDef;


/**
  * @brief LVDS PLL Clock structure definition
  */
typedef struct
{
  uint8_t  ISMASTER;                 /*!< PHY PLL instance configuration with Master or Slave
                              This parameter can be any value of @ref IS_LVDS_PHY_MST_SLV*/

  uint8_t  PLLMODE;                  /*!< PLL Operational Mode
                              This parameter can be any value of @ref IS_LVDS_PLL_MODE */

  uint32_t PLLBDIV;              /*!< PLL Output Division Factor
                              This parameter must be a value between 0x002 and 0   */


  uint32_t PLLMDIV;                  /* Feedback divider */

  uint32_t PLLNDIV;            /*!< PLL Input Division Factor
                              This parameter can be any value of @ref IS_LVDS_PLL_NDIV */

  uint32_t PLLFREF;                /* PLL reference frequency should be < 25MHz */

  uint32_t TESTDIV;                  /* PLL Test Clock Divider */

  uint32_t MODDEP;                   /* Modulation Depth */

  uint32_t MODFREQ;                  /* Modulation Frequency */

  uint32_t FRACIN;                   /* Fractional value of average feedback multiplication ratio MDIV*/

  uint32_t DOWN_SPREAD;              /* Triangular Wave Gnerator pattern spreading
                                This parameter value can be either 0 - Center Spread  or 1 - Down Spread*/

} LVDS_PLLInitTypeDef;

/**
  * @brief  LVDS configuration
  */
typedef struct
{
  uint32_t NumberOfLinks;                /*!< Number of links
                        It is either 1 Link (4DL) or 2 (8DL)
                        This parameter can be any value of @ref LVDS_Number_Of_Links */

  uint32_t NumberOfDisplays;              /*!< Number of displays
                        It is either 1 or 2  with same content on both
                        This parameter can be any value of @ref LVDS_Number_Of_Displays */

  uint32_t HSPolarity;                    /*!< HSYNC pin polarity
                                                This parameter can be any value of @ref LVDS_HSYNC_Polarity         */

  uint32_t VSPolarity;                    /*!< VSYNC pin polarity
                                                This parameter can be any value of @ref LVDS_VSYNC_Active_Polarity  */

  uint32_t DEPolarity;                    /*!< Data Enable pin polarity
                                                This parameter can be any value of @ref LVDS_DATA_ENABLE_Polarity   */

  uint32_t LinkPhase;           /*!< Link phase
                          This parameter can be any value of @ref LVDS_Link_Phase           */
} LVDS_CfgTypeDef;

/**
  * @brief  HAL LVDS States Structure Definition
  */
typedef enum
{
  HAL_LVDS_STATE_RESET   = 0x00000000U,
  HAL_LVDS_STATE_READY   = 0x00000001U,
  HAL_LVDS_STATE_ERROR   = 0x00000002U,
  HAL_LVDS_STATE_BUSY    = 0x00000003U,
  HAL_LVDS_STATE_TIMEOUT = 0x00000004U

} HAL_LVDS_StateTypeDef;

/**
  * @brief  HAL LVDS handle structures definition
  */
typedef struct __LVDS_HandleTypeDef
{
  LVDS_TypeDef        *Instance;    /*!< Register base address       */
  LVDS_InitTypeDef      Init;     /*!< LVDS Init parameters    */
  HAL_LockTypeDef       Lock;     /*!< LVDS peripheral status      */
  __IO HAL_LVDS_StateTypeDef  State;      /*!< LVDS communication state    */
  __IO uint32_t       ErrorCode;    /*!< LVDS Error code             */
  uint32_t          ErrorMsk;   /*!< LVDS Error monitoring mask  */
} LVDS_HandleTypeDef;
/**
  * @}
  */


/**********************************************************************************************/
/*                                  LVDS HAL Control / Init definitions                     */
/**********************************************************************************************/

/* Exported Variable -------------------------------*/


/* Exported functions --------------------------------------------------------*/
/** @addtogroup LVDS_Exported_Functions LVDS Exported Functions
  * @{
  */
/** @addtogroup LVDS_Group1 Initialization and Configuration functions
  *  @brief   Initialization and Configuration functions
  * @{
  */
HAL_StatusTypeDef HAL_LVDS_Init(LVDS_HandleTypeDef *hlvds, LVDS_CfgTypeDef *cfg, LVDS_PLLInitTypeDef *PLLInit);
HAL_StatusTypeDef HAL_LVDS_DeInit(LVDS_HandleTypeDef *hlvds);
void HAL_LVDS_MspInit(LVDS_HandleTypeDef *hlvds);
void HAL_LVDS_MspDeInit(LVDS_HandleTypeDef *hlvds);
HAL_StatusTypeDef HAL_LVDS_PHY_Init(LVDS_HandleTypeDef *hlvds, LVDS_PLLInitTypeDef *PLLInit);
HAL_StatusTypeDef HAL_LVDS_PHY_PLLConfig(LVDS_HandleTypeDef *hlvds, const LVDS_PLLInitTypeDef *PLLInit);
/**
  * @}
  */

/** @addtogroup LVDS_Group2 Peripheral Control functions
  *  @brief    Peripheral Control functions
  * @{
  */
HAL_StatusTypeDef HAL_LVDS_Start(LVDS_HandleTypeDef *hlvds);
HAL_StatusTypeDef HAL_LVDS_Stop(LVDS_HandleTypeDef *hlvds);
HAL_StatusTypeDef HAL_LVDS_ConfigMode(LVDS_HandleTypeDef *hlvds, const LVDS_CfgTypeDef *cfg);
HAL_StatusTypeDef HAL_LVDS_ConfigDataMapping(LVDS_HandleTypeDef *hlvds);
HAL_StatusTypeDef HAL_LVDS_SetPattern(LVDS_HandleTypeDef *hlvds, const uint32_t pattern[5][7]);
HAL_StatusTypeDef HAL_LVDS_BistStart(LVDS_HandleTypeDef *hlvds, uint32_t polyname);
HAL_StatusTypeDef HAL_LVDS_BistStop(LVDS_HandleTypeDef *hlvds);
HAL_StatusTypeDef HAL_LVDS_BistConfig(LVDS_HandleTypeDef *hlvds, uint32_t link, uint32_t custom);
HAL_StatusTypeDef HAL_LVDS_SetLanePolarity(LVDS_HandleTypeDef *hlvds, uint32_t link, uint32_t channels);
HAL_StatusTypeDef HAL_LVDS_SetChannelDistribution(LVDS_HandleTypeDef *hlvds, uint32_t link, uint32_t distribution);
/**
  * @}
  */

/** @addtogroup LVDS_Group3 Peripheral State and Errors functions
  *  @brief    Peripheral State and Errors functions
  * @{
  */
HAL_LVDS_StateTypeDef HAL_LVDS_GetState(const LVDS_HandleTypeDef *hlvds);
uint32_t HAL_LVDS_GetError(const LVDS_HandleTypeDef *hlvds);
/**
  * @}
  */

/**
  * @}
  */


/* Private macros ------------------------------------------------------------*/
/** @defgroup LVDS_Private_Macros LVDS Private Macros
  * @{
  */

#define IS_LVDS_PLL_BDIV(__BDIV__)           ((2U <= (__BDIV__)) && ((__BDIV__) <= 1023U))
#define IS_LVDS_PLL_MDIV(__MDIV__)           ((1U <= (__MDIV__)) && ((__MDIV__) <= 1023U))
#define IS_LVDS_PLL_NDIV(__NDIV__)           ((1U <= (__NDIV__)) && ((__NDIV__) <= 1023U))
#define IS_LVDS_PLL_FREF(__FREF__)           ((0U <= (__FREF__)) && ((__FREF__) <= (uint32_t)40000000))
#define IS_LVDS_LINK(__LINK__)             (((__LINK__) == LVDS_ONE_LINK) || \
                                            ((__LINK__) == LVDS_TWO_LINKS))
#define IS_LVDS_DISPLAY(__CHANNEL__)         (((__CHANNEL__) == LVDS_ONE_DISPLAY) || \
                                              ((__CHANNEL__) == LVDS_TWO_DISPLAYS))
#define IS_LVDS_VS_POLARITY(__POLARITY__)    (((__POLARITY__) == LVDS_VS_POLARITY_NORMAL) || \
                                              ((__POLARITY__) == LVDS_VS_POLARITY_INVERTED))
#define IS_LVDS_HS_POLARITY(__POLARITY__)    (((__POLARITY__) == LVDS_HS_POLARITY_NORMAL) || \
                                              ((__POLARITY__) == LVDS_HS_POLARITY_INVERTED))
#define IS_LVDS_DE_POLARITY(__POLARITY__)    (((__POLARITY__) == LVDS_DE_POLARITY_NORMAL) || \
                                              ((__POLARITY__) == LVDS_DE_POLARITY_INVERTED))
#define IS_LVDS_LINK_PHASE(__PHASE__)        (((__PHASE__) == LVDS_PHASE_EAO) || \
                                              ((__PHASE__) == LVDS_PHASE_EBO))
#define IS_LVDS_LINK_SELECTED(__LINK__)        (((__LINK__) == LVDS_LINK1_SELECT) || \
                                                ((__LINK__) == LVDS_LINK2_SELECT))
#define IS_LVDS_CHANNEL(__CHANNEL__)         ((0x00U <= (__CHANNEL__)) && ((__CHANNEL__) <= 0xAAAAAU))
#define IS_PATTERN_SIZE(__PATTERN__)         (((sizeof((__PATTERN__)[5][7])/sizeof((__PATTERN__)[0])) \
                                               == (uint32_t)5) && \
                                              (sizeof((__PATTERN__)[0]) == (uint32_t)7))
#define IS_LVDS_PRBS_POLYNOME(__POLYNOME__)    (((__POLYNOME__) == LVDS_PRBS_07) || \
                                                ((__POLYNOME__) == LVDS_PRBS_15) || \
                                                ((__POLYNOME__) == LVDS_PRBS_23) || \
                                                ((__POLYNOME__) == LVDS_PRBS_31))
#define IS_LVDS_CHANNEL_VALUE(__CHANNEL__)     ((0x0U <= (__CHANNEL__)) && ((__CHANNEL__) <= 0xAU))
#define IS_LVDS_PLL_MODE(__PLLMODE__)        (((__PLLMODE__) == LVDS_PLL_MODE_INT) || \
                                              ((__PLLMODE__) == LVDS_PLL_MODE_FRAC) || \
                                              ((__PLLMODE__) == LVDS_PLL_MODE_SSC))
#define IS_LVDS_PHY_MST_SLV(__PHYMODE__)         (((__PHYMODE__) == (uint8_t)1U) || ((__PHYMODE__) == (uint8_t)0U))

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */


#endif /* LVDS */

#ifdef __cplusplus
}
#endif

#endif /* __STM32MP2xx_HAL_LVDS_H */
