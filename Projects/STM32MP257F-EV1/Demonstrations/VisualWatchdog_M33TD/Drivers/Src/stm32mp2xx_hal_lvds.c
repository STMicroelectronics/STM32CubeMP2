/**
  ******************************************************************************
  * @file    stm32mp2xx_hal_lvds.c
  * @author  MCD Application Team
  * @brief   LVDS HAL module driver
  *          This file provides firmware functions to manage the following
  *          functionalities of the Digital Camera Interface (lvds) peripheral:
  *           + Initialization and de-initialization functions
  *           + IO operation functions
  *           + Peripheral Control functions
  *           + Peripheral State and Error functions
  *
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
  @verbatim
  ==============================================================================
                        ##### How to use this driver #####
  ==============================================================================
  [..]
    The LVDS HAL driver can be used as follows:

    (#) Declare a LVDS_HandleTypeDef handle structure, for example: DSI_HandleTypeDef  hlvds;

    (#) Initialize the LVDS low level resources by implementing the HAL_DSI_MspInit() API:
        (##) Enable the DSI interface clock

  (#) Declare the LVDS_PLLInitTypeDef handle structure, for example LVDS_PLLInitTypeDef PLLInit;

  (#) Declare the LVDS_CfgTypeDef handle structure, for example LVDS_CfgTypeDef cfg;

  (#) Initialize the LVDS Host Peripheral and LVDS PHY, with the required PHY PLL parameters, input data
      mapping and the lvds configuration respectively by calling HAL_LVDS_Init() API which calls
      HAL_LVDS_MspInit().

    *** Configuration ***
    =========================
    [..]
      (#) Use HAL_LVDS_ConfigMode() function to configure the modes of LVDS, Single pixel and Dual Pixel mode and
          respective channel distribution for both LINKS by calling HAL_LVDS_SetChannelDistribution().

      (#) The LVDS Host can be started/stopped using respectively functions HAL_LVDS_Start() and HAL_LVDS_Stop().

    (#) To configure the host data mapping, use function HAL_LVDS_ConfigDataMapping() API and HAL_LVDS_SetPattern() API
        for standard data mapping and custom mapping respectively.

    (#) To configure the Lane Polarity, by using function HAL_LVDS_SetLanePolarity().

    (#) For monitoring and measuring the equipment, PRBS Generator can be started/stopped and configured using
      respectively functions HAL_LVDS_BistStart(), HAL_LVDS_BistStop() and HAL_LVDS_BistConfig() API's.

    (#) To control LVDS state you can use the following function: HAL_LVDS_GetState().

  *** LVDS HAL driver macros list ***
  =============================================
  [..]
     Below the list of most used macros in LVDS HAL driver.

    (+) __HAL_LVDS_ENABLE: Enable the LVDS Host.
    (+) __HAL_LVDS_DISABLE: Disable the LVDS Host.
    (+) __HAL_LVDS_PLL_ENABLE: Enable the LVDS PLL.
    (+) __HAL_LVDS_PLL_DISABLE: Disable the LVDS PLL.

  @endverbatim
  *
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32mp2xx_hal.h"

/** @addtogroup STM32MP2xx_HAL_Driver
  * @{
  */

#ifdef HAL_LVDS_MODULE_ENABLED

/** @addtogroup LVDS
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/** @addtogroup LVDS_Private_Constants LVDS Private Constants
  * @{
  */
#define LVDS_TIMEOUT_VALUE            ((uint32_t)1000U)  /* 1s */
/**
  * @}
  */

/* Private macro -------------------------------------------------------------*/
/** @addtogroup LVDS_Private_Macros LVDS Private Macros
  * @{
  */
#define SET_MAP(__MAP__, __FIELD__) (__MAP__) << (__FIELD__)
/**
  * @}
  */

/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/** @defgroup LVDS_Private_Functions LVDS Private Functions
  * @{
  */
HAL_StatusTypeDef LVDS_PHY_StartMst(LVDS_HandleTypeDef *hlvds);
void LVDS_PHY_SetPLL(LVDS_HandleTypeDef *hlvds, const LVDS_PLLInitTypeDef *PLLInit);
HAL_StatusTypeDef LVDS_PHY_StartSlv(LVDS_HandleTypeDef *hlvds);
HAL_StatusTypeDef LVDS_PHY_SetPreEmphasisAmplitude(LVDS_HandleTypeDef *hlvds, const LVDS_PLLInitTypeDef *PLLInit,
                uint32_t amplitude);
HAL_StatusTypeDef LVDS_PHY_SetPreEmphasisTime(LVDS_HandleTypeDef *hlvds, const LVDS_PLLInitTypeDef *PLLInit,
               uint32_t time);
HAL_StatusTypeDef LVDS_PHY_SetRXLoopback(LVDS_HandleTypeDef *hlvds, const LVDS_PLLInitTypeDef *PLLInit,
            uint32_t loopback);
HAL_StatusTypeDef LVDS_PHY_SetCurrentMode(LVDS_HandleTypeDef *hlvds, const LVDS_PLLInitTypeDef *PLLInit,
             uint32_t reg1, uint32_t reg2);
HAL_StatusTypeDef LVDS_PHY_SetFlipedge(LVDS_HandleTypeDef *hlvds, const LVDS_PLLInitTypeDef *PLLInit,
            uint32_t flipedge);
HAL_StatusTypeDef LVDS_PHY_SetSpare(LVDS_HandleTypeDef *hlvds, const LVDS_PLLInitTypeDef *PLLInit,
             uint32_t spare1, uint32_t spare2);
HAL_StatusTypeDef LVDS_PHY_SetBias(LVDS_HandleTypeDef *hlvds, const LVDS_PLLInitTypeDef *PLLInit,
            uint32_t bias1, uint32_t bias2,
            uint32_t bias3, uint32_t bias4, uint32_t bias5);
HAL_StatusTypeDef LVDS_PHY_SetPLLTest(LVDS_HandleTypeDef *hlvds, const LVDS_PLLInitTypeDef *PLLInit,
             uint32_t test);
HAL_StatusTypeDef LVDS_PHY_SetPLLConfig(LVDS_HandleTypeDef *hlvds, const LVDS_PLLInitTypeDef *PLLInit,
             uint32_t config);
HAL_StatusTypeDef LVDS_PHY_SetPLLCP(LVDS_HandleTypeDef *hlvds, const LVDS_PLLInitTypeDef *PLLInit, uint32_t cp);
HAL_StatusTypeDef LVDS_PHY_SetPLLLDO(LVDS_HandleTypeDef *hlvds, const LVDS_PLLInitTypeDef *PLLInit,
            uint32_t config);
HAL_StatusTypeDef LVDS_PHY_SetPLLTWG(LVDS_HandleTypeDef *hlvds, const LVDS_PLLInitTypeDef *PLLInit,
            uint32_t twg1, uint32_t twg2);
HAL_StatusTypeDef LVDS_PHY_SetPLLSD(LVDS_HandleTypeDef *hlvds, const LVDS_PLLInitTypeDef *PLLInit, uint32_t sd1,
             uint32_t sd2);
HAL_StatusTypeDef LVDS_PHY_SetPLLMode(LVDS_HandleTypeDef *hlvds, const LVDS_PLLInitTypeDef *PLLInit,
             uint32_t mode1, uint32_t mode2);
HAL_StatusTypeDef LVDS_PHY_EnableDataLanes(LVDS_HandleTypeDef *hlvds, const LVDS_PLLInitTypeDef *PLLInit,
              uint32_t config);
HAL_StatusTypeDef LVDS_PHY_SetDebug(LVDS_HandleTypeDef *hlvds, const LVDS_PLLInitTypeDef *PLLInit,
             uint32_t debug);
HAL_StatusTypeDef LVDS_PHY_SetMonitorSerializer(LVDS_HandleTypeDef *hlvds, const LVDS_PLLInitTypeDef *PLLInit,
               uint32_t serializer);
HAL_StatusTypeDef LVDS_PHY_SetMonitorSerializerClk(LVDS_HandleTypeDef *hlvds, const LVDS_PLLInitTypeDef *PLLInit,
                uint32_t serializer_clk);
HAL_StatusTypeDef LVDS_PHY_SetMonitorPLLReferenceOK(LVDS_HandleTypeDef *hlvds,
                 const LVDS_PLLInitTypeDef *PLLInit, uint32_t ref);
HAL_StatusTypeDef LVDS_PHY_SetMonitorPLLLDOOutputVoltage(LVDS_HandleTypeDef *hlvds,
                const LVDS_PLLInitTypeDef *PLLInit,  uint32_t voltage);
HAL_StatusTypeDef LVDS_PHY_SetMonitorBandgapOutputVoltage(LVDS_HandleTypeDef *hlvds,
                 const LVDS_PLLInitTypeDef *PLLInit, uint32_t voltage);
HAL_StatusTypeDef LVDS_PHY_SetMonitorBandgapLoop(LVDS_HandleTypeDef *hlvds, const LVDS_PLLInitTypeDef *PLLInit,
              uint32_t bandgap_loop);
HAL_StatusTypeDef LVDS_PHY_SetMonitorBandgapCurrent(LVDS_HandleTypeDef *hlvds,
                 const LVDS_PLLInitTypeDef *PLLInit, uint32_t bandgap);
HAL_StatusTypeDef LVDS_PHY_SetImpedance(LVDS_HandleTypeDef *hlvds, const LVDS_PLLInitTypeDef *PLLInit,
             uint32_t impedance);
HAL_StatusTypeDef LVDS_PHY_SetMonitorPLLLock(LVDS_HandleTypeDef *hlvds, const LVDS_PLLInitTypeDef *PLLInit,
              uint32_t lock);
HAL_StatusTypeDef core(LVDS_HandleTypeDef *hlvds);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  PHY Phase Locked Loop instance configuration
  * @param  hlvds pointer to a LVDS_HandleTypeDef structure that contains information
  *       for the LVDS
  * @param  PLLInit pointer to LVDS_PLLInitTypeDef structure that contains information
  *       for the internal PLL instance (for single instance either MST PHY PLL or SLV PHY PLL)
  * @retval None
  */
void LVDS_PHY_SetPLL(LVDS_HandleTypeDef *hlvds, const LVDS_PLLInitTypeDef *PLLInit)
{
  uint32_t en_sd;
  uint32_t en_twg;
  uint32_t fpfd;
  uint32_t twg_threshold;
  uint32_t twg_step;
  uint32_t sd_code;

  /* Check PHY PLL parameters */
  assert_param(IS_LVDS_PHY_MST_SLV(PLLInit->ISMASTER));
  assert_param(IS_LVDS_PLL_MODE(PLLInit->PLLMODE));
  assert_param(IS_LVDS_PLL_BDIV(PLLInit->PLLBDIV));
  assert_param(IS_LVDS_PLL_MDIV(PLLInit->PLLMDIV));
  assert_param(IS_LVDS_PLL_NDIV(PLLInit->PLLNDIV));
  assert_param(IS_LVDS_PLL_FREF(PLLInit->PLLFREF));

  fpfd = (PLLInit->PLLFREF / PLLInit->PLLNDIV);

  if (PLLInit->PLLMODE == LVDS_PLL_MODE_INT)
  {
    assert_param(PLLInit->FRACIN == (uint32_t)0);
    assert_param(PLLInit->DOWN_SPREAD == (uint32_t)0);

    en_sd = 0;
    en_twg = 0;
    /*Initialize the required parameter to control the Integer Operation mode*/
    sd_code = 0;
    twg_threshold = 0;
    twg_step = 0;
  }
  else if (PLLInit->PLLMODE == LVDS_PLL_MODE_FRAC)
  {
    en_twg = 0;
    if (PLLInit->ISMASTER == (uint8_t)1)
    {
      en_sd = LVDS_PMPLLCR1_MST_PLL_SD_ENABLE;
    }
    else
    {
      en_sd = 0;
    }
    /*Compute and initialize the required parameter to control fractional operation mode */
    sd_code = ((PLLInit->FRACIN * (uint32_t)131072) / (uint32_t)1000) + (uint32_t)4194304;
    twg_threshold = 0;
    twg_step = 0;
  }
  else
  {
    if (PLLInit->ISMASTER == (uint8_t)1)
    {
      en_twg = LVDS_PMPLLCR1_MST_PLL_TWG_ENABLE;
      en_sd = LVDS_PMPLLCR1_MST_PLL_SD_ENABLE;
    }
    else
    {
      en_sd = 0;
      en_twg = 0;
    }
    /*Compute and initialize the required parameter to control spread spectrum clocking operation mode */
    twg_threshold = (((uint32_t)4 * PLLInit->MODDEP * PLLInit->PLLMDIV / (uint32_t)100) * (uint32_t)8388608 \
                     / (uint32_t)256) / (uint32_t)1000;
    twg_step = twg_threshold / (fpfd / ((uint32_t)4 * PLLInit->MODFREQ));
    sd_code = ((PLLInit->FRACIN * (uint32_t)131072) / (uint32_t)1000) + (uint32_t)4194304;
  }
  if (PLLInit->ISMASTER == (uint8_t)1)
  {


    /*Set the input divider setting for input Reference clock of PLL */
    hlvds->Instance->PMPLLCR2 = (PLLInit->PLLNDIV << LVDS_PMPLLCR2_MST_PLL_INPUT_DIV_Pos);

    /*Set the bit clock divider */
    hlvds->Instance->PMPLLCR2 |= (PLLInit->PLLBDIV << LVDS_PMPLLCR2_MST_PLL_BIT_DIV_Pos);

    /*Set Multi-phase feedback divider setting */
    hlvds->Instance->PMPLLSDCR1 = (PLLInit->PLLMDIV << LVDS_PMPLLSDCR1_MST_PLL_SD_INT_RATIO_Pos);


    /*Set the divider setting for test clock */
    hlvds->Instance->PMPLLTESTCR  = (PLLInit->TESTDIV << LVDS_PMPLLTESTCR_MST_PLL_TEST_DIV_SETTINGS_Pos);

    /*Configuring Spread spectrum clock generator engine blocks by
     *  @Enable / Disable Sigma Delta Modulator
     *  @Enable / Disable Triangular Wave Generator
     *  */

    CLEAR_REG(hlvds->Instance->PMPLLCR1);
    hlvds->Instance->PMPLLCR1 = en_sd;
    hlvds->Instance->PMPLLCR1 |= en_twg;

    /*Set the Master PHY PLL TWG Threshold value */
    hlvds->Instance->PMPLLTWGCR2 = (twg_threshold << LVDS_PMPLLTWGCR2_MST_PLL_TWG_THRESHOLD_Pos);

    /*Set the Master PHY PLL TWG Step value */
    hlvds->Instance->PMPLLTWGCR1 = (twg_step << LVDS_PMPLLTWGCR1_MST_PLL_TWG_STEP_Pos);

    /*Set the Master PHY PLL TWG Down Spread value */
    hlvds->Instance->PMPLLTWGCR1 |= (PLLInit->DOWN_SPREAD << LVDS_PMPLLTWGCR1_MST_PLL_TWG_DOWN_SPREAD_Pos);

    /*Set the Master PHY PLL SD Input Code Bypass value */
    hlvds->Instance->PMPLLSDCR2 = (sd_code << LVDS_PMPLLSDCR2_MST_PLL_SD_INPUT_CODE_BYPASS_Pos);

  }
  else
  {

    /*Set the input divider setting for input Reference clock of PLL */
    hlvds->Instance->PSPLLCR2 = (PLLInit->PLLNDIV << LVDS_PSPLLCR2_SLV_PLL_INPUT_DIV_Pos);

    /*Set the bit clock divider */
    hlvds->Instance->PSPLLCR2 |= (PLLInit->PLLBDIV << LVDS_PSPLLCR2_SLV_PLL_BIT_DIV_Pos);

    /*Set Multi-phase feedback divider setting */
    hlvds->Instance->PSPLLSDCR1 = (PLLInit->PLLMDIV << LVDS_PSPLLSDCR1_SLV_PLL_SD_INT_RATIO_Pos);

    /*Set the divider setting for test clock */
    hlvds->Instance->PSPLLTESTCR  = (PLLInit->TESTDIV << LVDS_PSPLLTESTCR_SLV_PLL_TEST_DIV_SETTINGS_Pos);

    /*Configuring Spread spectrum clock generator engine blocks by
     *  @Enable / Disable Sigma Delta Modulator
     *  @Enable / Disable Triangular Wave Generator
     *  */
    CLEAR_REG(hlvds->Instance->PSPLLCR1);
    hlvds->Instance->PSPLLCR1 = (en_sd << LVDS_PSPLLCR1_SLV_PLL_SD_ENABLE_Pos);
    hlvds->Instance->PSPLLCR1 |= (en_twg << LVDS_PSPLLCR1_SLV_PLL_TWG_ENABLE_Pos);

    /*Set the Slave PHY PLL TWG Threshold value */
    hlvds->Instance->PSPLLTWGCR2 = (twg_threshold << LVDS_PSPLLTWGCR2_SLV_PLL_TWG_THRESHOLD_Pos);

    /*Set the Slave PHY PLL TWG Step value */
    hlvds->Instance->PSPLLTWGCR1 = (twg_step << LVDS_PSPLLTWGCR1_SLV_PLL_TWG_STEP_Pos);

    /*Set the Slave PHY PLL TWG Down Spread value */
    hlvds->Instance->PSPLLTWGCR1 |= (PLLInit->DOWN_SPREAD << LVDS_PSPLLTWGCR1_SLV_PLL_TWG_DOWN_SPREAD_Pos);

    /*Set the Slave PHY PLL SD Input Code Bypass value */
    hlvds->Instance->PSPLLSDCR2 = (sd_code << LVDS_PSPLLSDCR2_SLV_PLL_SD_INPUT_CODE_BYPASS_Pos);
  }
}

/**
  * @brief start the Master PHY
  * @param hlvds pointer to a LVDS_HandleTypeDef structure that contains information
  *       for the LVDS
  * @return HAL status
  */
HAL_StatusTypeDef LVDS_PHY_StartMst(LVDS_HandleTypeDef *hlvds)
{
  uint32_t tickstart;
  volatile HAL_StatusTypeDef lock;

  /* PLL lock timing control for the monitor unmask after startup (pll_en) */
  /* Adjust the value so that the masking window is opened at start-up     */
  hlvds->Instance->PMMPLCR = ((uint32_t)(0x200 - 0x160) << LVDS_PMMPLCR_MST_MON_PLL_LOCK_UNMASK_TUNE_Pos);

  /* LVDS Bias enabled on both PHY */
  hlvds->Instance->PMBCR2 = LVDS_PMBCR2_MST_BIAS_EN;

  /*Enable the LS_Clock, BIT_Cock, and Dual_Pixel_Clock*/
  hlvds->Instance->PMGCR |= (LVDS_PMGCR_MST_DP_CLK_OUT_ENABLE | LVDS_PMGCR_MST_LS_CLK_OUT_ENABLE \
                             | LVDS_PMGCR_MST_BIT_CLK_OUT_ENABLE);

  /*Enable the Test clock divide Master PHY*/;
  hlvds->Instance->PMPLLTESTCR |= LVDS_PMPLLTESTCR_MST_PLL_TEST_DIV_EN ;

  /*Enable the Master PHY PLL Dividers*/
  hlvds->Instance->PMPLLCR1 |= LVDS_PMPLLCR1_MST_PLL_DIVIDERS_ENABLE ;

  /*Set the Serial data transmission OK on Master PHY*/
  hlvds->Instance->PMSCR = LVDS_PMSCR_MST_SER_DATA_OK;

  /*Enable the Master PHY  PLL*/
  hlvds->Instance->PMPLLCR1 |= LVDS_PMPLLCR1_MST_PLL_ENABLE ;

  /* Get tick */
  tickstart = HAL_GetTick();

  /* Wait for PLL lock (using the new status register)
  * Wait until the regulator is ready
  */
  do
  {
    /* Check for the Timeout */
    if ((HAL_GetTick() - tickstart) > LVDS_TIMEOUT_VALUE)
    {
      return HAL_TIMEOUT;
    }
  } while ((hlvds->Instance->PMSSR1 & (uint32_t)0x7) != (uint32_t)0x7);

  /* Select MST PHY clock as pixel clock for the LDITX instead of FREF */
  hlvds->Instance->WCLKCR = LVDS_WCLKCR_SLV_CLKPIX_SEL;

  hlvds->Instance->PMPLLTESTCR |= LVDS_PMPLLTESTCR_MST_PLL_TEST_CLK_EN_Msk;

  return lock = HAL_OK;
}


/**
  * @brief  start the Slave PHY
  * @param  hlvds pointer to a LVDS_HandleTypeDef structure that contains information
  *       for the LVDS
  * @return HAL status
  */
HAL_StatusTypeDef LVDS_PHY_StartSlv(LVDS_HandleTypeDef *hlvds)
{
  uint32_t tickstart;
  volatile HAL_StatusTypeDef lock;

  /* PLL lock timing control for the monitor unmask after startup (pll_en) */
  /* Adjust the value so that the masking window is opened at start-up     */
  hlvds->Instance->PSMPLCR = ((uint32_t)(0x200 - 0x160) << LVDS_PSMPLCR_SLV_MON_PLL_LOCK_UNMASK_TUNE_Pos);

  /* LVDS Bias enabled on both PHY */
  hlvds->Instance->PSBCR2 = LVDS_PSBCR2_SLV_BIAS_EN;

  /* Enable the LS_Clock, BIT_Cock, and Dual_Pixel_Clock */
  hlvds->Instance->PSGCR |= (LVDS_PSGCR_SLV_DP_CLK_OUT_ENABLE | LVDS_PSGCR_SLV_LS_CLK_OUT_ENABLE \
                             | LVDS_PSGCR_SLV_BIT_CLK_OUT_ENABLE);

  /* Enable the Test clock divide Slave PHY */;
  hlvds->Instance->PSPLLTESTCR |= LVDS_PSPLLTESTCR_SLV_PLL_TEST_DIV_EN;

  /* Enable the Slave PHY PLL Dividers */
  hlvds->Instance->PSPLLCR1 |= LVDS_PSPLLCR1_SLV_PLL_DIVIDERS_ENABLE;

  /* Set the Serial data transmission OK on Slave PHY */
  hlvds->Instance->PSSCR = LVDS_PSSCR_SLV_SER_DATA_OK;

  /* Enable the Slave PHY  PLL */
  hlvds->Instance->PSPLLCR1 |= LVDS_PSPLLCR1_SLV_PLL_ENABLE;

  /* Get tick */
  tickstart = HAL_GetTick();

  /* LVDS_PSMPLSR_SLV_MON_PLL_LOCK_PERMANENT_FAULT should go high to indicate lock mechanism is starting
     Wait for PLL lock
    */
  do
  {
    /* Check for the Timeout */
    if ((HAL_GetTick() - tickstart) > LVDS_TIMEOUT_VALUE)
    {
      return HAL_TIMEOUT;
    }
  } while ((hlvds->Instance->PSSSR1 & (uint32_t)0x7) != (uint32_t)0x7);

  /* Select MST PHY clock as pixel clock for the LDITX instead of FREF  */
  hlvds->Instance->WCLKCR = LVDS_WCLKCR_SLV_CLKPIX_SEL ;

  hlvds->Instance->PSPLLTESTCR |= LVDS_PSPLLTESTCR_SLV_PLL_TEST_CLK_EN;

  return lock = HAL_OK;
}

/**
  * @brief  Set the amplitude of pre-emphasis
  * @param  hlvds pointer to a LVDS_HandleTypeDef structure that contains information
  *       for the LVDS
  * @param  PLLInit pointer to LVDS_PLLInitTypeDef structure that contains information
  *       for the internal PLL instance (for single instance either MST PHY PLL or SLV PHY PLL)
  * @param  amplitude
  * @retval HAL Status
  */
HAL_StatusTypeDef LVDS_PHY_SetPreEmphasisAmplitude(LVDS_HandleTypeDef *hlvds, const LVDS_PLLInitTypeDef *PLLInit,
                                                          uint32_t amplitude)
{
  /* Check the LVDS handle allocation */
  if (hlvds == NULL)
  {
    return HAL_ERROR;
  }
  /* Process locked */
  __HAL_LOCK(hlvds);

  if (PLLInit->ISMASTER == (uint8_t)1)
  {
    hlvds->Instance->PMPEACR = amplitude;
  }
  else
  {
    hlvds->Instance->PSPEACR = amplitude;
  }

  /* Release Lock */
  __HAL_UNLOCK(hlvds);

  return HAL_OK;
}

/**
  * @brief  Set the Time Duration for pre-emphasis
  * @param  hlvds pointer to a LVDS_HandleTypeDef structure that contains information
  *       for the LVDS
  * @param  PLLInit pointer to LVDS_PLLInitTypeDef structure that contains information
  *       for the internal PLL instance (for single instance either MST PHY PLL or SLV PHY PLL)
  * @param  time
  * @retval HAL Status
  */
HAL_StatusTypeDef LVDS_PHY_SetPreEmphasisTime(LVDS_HandleTypeDef *hlvds, const LVDS_PLLInitTypeDef *PLLInit,
                                                     uint32_t time)
{
  /* Check the LVDS handle allocation */
  if (hlvds == NULL)
  {
    return HAL_ERROR;
  }
  /* Process locked */
  __HAL_LOCK(hlvds);

  if (PLLInit->ISMASTER == (uint8_t)1)
  {
    hlvds->Instance->PMPETCR = time;
  }
  else
  {
    hlvds->Instance->PSPETCR = time;
  }

  /* Release Lock */
  __HAL_UNLOCK(hlvds);

  return HAL_OK;
}

/**
  * @brief  Rx LoopBack configuration
  * @param  hlvds pointer to a LVDS_HandleTypeDef structure that contains information
  *       for the LVDS
  * @param  PLLInit pointer to LVDS_PLLInitTypeDef structure that contains information
  *       for the internal PLL instance (for single instance either MST PHY PLL or SLV PHY PLL)
  * @param  loopback
  * @retval HAL Status
  */
HAL_StatusTypeDef LVDS_PHY_SetRXLoopback(LVDS_HandleTypeDef *hlvds, const LVDS_PLLInitTypeDef *PLLInit,
                                                uint32_t loopback)
{
  /* Check the LVDS handle allocation */
  if (hlvds == NULL)
  {
    return HAL_ERROR;
  }
  /* Process locked */
  __HAL_LOCK(hlvds);

  if (PLLInit->ISMASTER == (uint8_t)1)
  {
    hlvds->Instance->PMRLPCR = loopback;
  }
  else
  {
    hlvds->Instance->PSRLPCR = loopback;
  }

  /* Release Lock */
  __HAL_UNLOCK(hlvds);

  return HAL_OK;
}

/**
  * @brief  Set the Current Mode Control (CMD) configuration
  * @param  hlvds pointer to a LVDS_HandleTypeDef structure that contains information
  *       for the LVDS
  * @param  PLLInit pointer to LVDS_PLLInitTypeDef structure that contains information
  *       for the internal PLL instance (for single instance either MST PHY PLL or SLV PHY PLL)
  * @param  reg1
  * @param  reg2
  * @retval HAL Status
  */
HAL_StatusTypeDef LVDS_PHY_SetCurrentMode(LVDS_HandleTypeDef *hlvds, const LVDS_PLLInitTypeDef *PLLInit,
                                                 uint32_t reg1, uint32_t reg2)
{
  /* Check the LVDS handle allocation */
  if (hlvds == NULL)
  {
    return HAL_ERROR;
  }
  /* Process locked */
  __HAL_LOCK(hlvds);

  if (PLLInit->ISMASTER == (uint8_t)1)
  {
    hlvds->Instance->PMCMCR1 = reg1;
    hlvds->Instance->PMCMCR2 = reg2;
  }
  else
  {
    hlvds->Instance->PSCMCR1 = reg1;
    hlvds->Instance->PSCMCR2 = reg2;
  }

  /* Release Lock */
  __HAL_UNLOCK(hlvds);

  return HAL_OK;
}

/**
  * @brief  Configure the flips of clock edge
  * @param  hlvds pointer to a LVDS_HandleTypeDef structure that contains information
  *       for the LVDS
  * @param  PLLInit pointer to LVDS_PLLInitTypeDef structure that contains information
  *       for the internal PLL instance (for single instance either MST PHY PLL or SLV PHY PLL)
  * @param  flipedge
  * @retval HAL Status
  */
HAL_StatusTypeDef LVDS_PHY_SetFlipedge(LVDS_HandleTypeDef *hlvds, const LVDS_PLLInitTypeDef *PLLInit,
                                              uint32_t flipedge)
{
  /* Check the LVDS handle allocation */
  if (hlvds == NULL)
  {
    return HAL_ERROR;
  }
  /* Process locked */
  __HAL_LOCK(hlvds);

  if (PLLInit->ISMASTER == (uint8_t)1)
  {
    hlvds->Instance->PMFCR = flipedge;
  }
  else
  {
    hlvds->Instance->PSFCR = flipedge;
  }

  /* Release Lock */
  __HAL_UNLOCK(hlvds);

  return HAL_OK;
}


/**
  * @brief  Set the Spare Inputs Configuration
  * @param  hlvds pointer to a LVDS_HandleTypeDef structure that contains information
  *       for the LVDS
  * @param  PLLInit pointer to LVDS_PLLInitTypeDef structure that contains information
  *       for the internal PLL instance (for single instance either MST PHY PLL or SLV PHY PLL)
  * @param  spare1
  * @param  spare2
  * @retval HAL Status
  */
HAL_StatusTypeDef LVDS_PHY_SetSpare(LVDS_HandleTypeDef *hlvds, const LVDS_PLLInitTypeDef *PLLInit,
                                           uint32_t spare1, uint32_t spare2)
{
  /* Check the LVDS handle allocation */
  if (hlvds == NULL)
  {
    return HAL_ERROR;
  }
  /* Process locked */
  __HAL_LOCK(hlvds);

  if (PLLInit->ISMASTER == (uint8_t)1)
  {
    hlvds->Instance->PMSCR1 = spare1;
    hlvds->Instance->PMSCR2 = spare2;
  }
  else
  {
    hlvds->Instance->PSSCR1 = spare1;
    hlvds->Instance->PSSCR2 = spare2;
  }

  /* Release Lock */
  __HAL_UNLOCK(hlvds);

  return HAL_OK;
}

/**
  * @brief  Set the Bias configuration of LINK
  * @param  hlvds pointer to a LVDS_HandleTypeDef structure that contains information
  *       for the LVDS
  * @param  PLLInit pointer to LVDS_PLLInitTypeDef structure that contains information
  *       for the internal PLL instance (for single instance either MST PHY PLL or SLV PHY PLL)
  * @param  bias1 (ENABLE or DISABLE the local bias for data lanes)
  * @param  bias2 (Global Bias Enable)
  * @param  bias3 (Voltage Mode Driver enable or disable for data lanes)
  * @param  bias4 (VOH tuning of Voltage mode driver)
  * @param  bias5 (VOL tuning of Voltage mode driver)
  * @retval HAL Status
  */
HAL_StatusTypeDef LVDS_PHY_SetBias(LVDS_HandleTypeDef *hlvds, const LVDS_PLLInitTypeDef *PLLInit,
                                          uint32_t bias1, uint32_t bias2,
                                          uint32_t bias3, uint32_t bias4, uint32_t bias5)
{
  /* Check the LVDS handle allocation */
  if (hlvds == NULL)
  {
    return HAL_ERROR;
  }
  /* Process locked */
  __HAL_LOCK(hlvds);

  if (PLLInit->ISMASTER == (uint8_t)1)
  {
    hlvds->Instance->PMBCR1 = bias1;
    hlvds->Instance->PMBCR2 = bias2;
    hlvds->Instance->PMBCR3 = bias3;
    hlvds->Instance->PMBCR4 = bias4;
    hlvds->Instance->PMBCR5 = bias5;
  }
  else
  {
    hlvds->Instance->PSBCR1 = bias1;
    hlvds->Instance->PSBCR2 = bias2;
    hlvds->Instance->PSBCR3 = bias3;
    hlvds->Instance->PSBCR4 = bias4;
    hlvds->Instance->PSBCR5 = bias5;
  }

  /* Release Lock */
  __HAL_UNLOCK(hlvds);

  return HAL_OK;
}

/**
  * @brief  Set impedance for data lanes
  * @param  hlvds pointer to a LVDS_HandleTypeDef structure that contains information
  *       for the LVDS
  * @param  PLLInit pointer to LVDS_PLLInitTypeDef structure that contains information
  *       for the internal PLL instance (for single instance either MST PHY PLL or SLV PHY PLL)
  * @param  impedance
  * @retval HAL Status
  */
HAL_StatusTypeDef LVDS_PHY_SetImpedance(LVDS_HandleTypeDef *hlvds, const LVDS_PLLInitTypeDef *PLLInit,
                                               uint32_t impedance)
{
  /* Check the LVDS handle allocation */
  if (hlvds == NULL)
  {
    return HAL_ERROR;
  }
  /* Process locked */
  __HAL_LOCK(hlvds);

  if (PLLInit->ISMASTER == (uint8_t)1)
  {
    hlvds->Instance->PMICR = impedance;
  }
  else
  {
    hlvds->Instance->PSICR = impedance;
  }

  /* Release Lock */
  __HAL_UNLOCK(hlvds);

  return HAL_OK;
}

/**
  * @brief  set bandgap current monitor configuration
  * @param  hlvds pointer to a LVDS_HandleTypeDef structure that contains information
  *       for the LVDS
  * @param  PLLInit pointer to LVDS_PLLInitTypeDef structure that contains information
  *       for the internal PLL instance (for single instance either MST PHY PLL or SLV PHY PLL)
  * @param  bandgap
  * @retval HAL Status
  */
HAL_StatusTypeDef LVDS_PHY_SetMonitorBandgapCurrent(LVDS_HandleTypeDef *hlvds,
                                                           const LVDS_PLLInitTypeDef *PLLInit, uint32_t bandgap)
{
  /* Check the LVDS handle allocation */
  if (hlvds == NULL)
  {
    return HAL_ERROR;
  }
  /* Process locked */
  __HAL_LOCK(hlvds);

  if (PLLInit->ISMASTER == (uint8_t)1)
  {
    hlvds->Instance->PMMBCCR = bandgap;
  }
  else
  {
    hlvds->Instance->PSMBCCR = bandgap;
  }

  /* Release Lock */
  __HAL_UNLOCK(hlvds);

  return HAL_OK;
}

/**
  * @brief  set bandgap loop monitor configuration
  * @param  hlvds pointer to a LVDS_HandleTypeDef structure that contains information
  *       for the LVDS
  * @param  PLLInit pointer to LVDS_PLLInitTypeDef structure that contains information
  *       for the internal PLL instance (for single instance either MST PHY PLL or SLV PHY PLL)
  * @param  bandgap_loop
  * @retval HAL Status
  */
HAL_StatusTypeDef LVDS_PHY_SetMonitorBandgapLoop(LVDS_HandleTypeDef *hlvds, const LVDS_PLLInitTypeDef *PLLInit,
                                                        uint32_t bandgap_loop)
{
  /* Check the LVDS handle allocation */
  if (hlvds == NULL)
  {
    return HAL_ERROR;
  }
  /* Process locked */
  __HAL_LOCK(hlvds);

  if (PLLInit->ISMASTER == (uint8_t)1)
  {
    hlvds->Instance->PMMBLCR = bandgap_loop;
  }
  else
  {
    hlvds->Instance->PSMBLCR = bandgap_loop;
  }

  /* Release Lock */
  __HAL_UNLOCK(hlvds);

  return HAL_OK;
}

/**
  * @brief  set bandgap output voltage monitor configuration
  * @param  hlvds pointer to a LVDS_HandleTypeDef structure that contains information
  *       for the LVDS
  * @param  PLLInit pointer to LVDS_PLLInitTypeDef structure that contains information
  *       for the internal PLL instance (for single instance either MST PHY PLL or SLV PHY PLL)
  * @param  voltage
  * @retval HAL Status
  */
HAL_StatusTypeDef LVDS_PHY_SetMonitorBandgapOutputVoltage(LVDS_HandleTypeDef *hlvds,
                                                                 const LVDS_PLLInitTypeDef *PLLInit, uint32_t voltage)
{
  /* Check the LVDS handle allocation */
  if (hlvds == NULL)
  {
    return HAL_ERROR;
  }
  /* Process locked */
  __HAL_LOCK(hlvds);

  if (PLLInit->ISMASTER == (uint8_t)1)
  {
    hlvds->Instance->PMMBOVCR = voltage;
  }
  else
  {
    hlvds->Instance->PSMBOVCR = voltage;
  }

  /* Release Lock */
  __HAL_UNLOCK(hlvds);

  return HAL_OK;
}

/**
  * @brief  set the LDO output voltage monitor configuration
  * @param  hlvds pointer to a LVDS_HandleTypeDef structure that contains information
  *       for the LVDS
  * @param  PLLInit pointer to LVDS_PLLInitTypeDef structure that contains information
  *       for the internal PLL instance (for single instance either MST PHY PLL or SLV PHY PLL)
  * @param  voltage
  * @retval HAL Status
  */
HAL_StatusTypeDef LVDS_PHY_SetMonitorPLLLDOOutputVoltage(LVDS_HandleTypeDef *hlvds,
                                                                const LVDS_PLLInitTypeDef *PLLInit,  uint32_t voltage)
{
  /* Check the LVDS handle allocation */
  if (hlvds == NULL)
  {
    return HAL_ERROR;
  }
  /* Process locked */
  __HAL_LOCK(hlvds);

  if (PLLInit->ISMASTER == (uint8_t)1)
  {
    hlvds->Instance->PMMPLOVCR = voltage;
  }
  else
  {
    hlvds->Instance->PSMPLOVCR = voltage;
  }

  /* Release Lock */
  __HAL_UNLOCK(hlvds);

  return HAL_OK;
}

/**
  * @brief  set the PLL Lock monitor configuration
  * @param  hlvds pointer to a LVDS_HandleTypeDef structure that contains information
  *       for the LVDS
  * @param  PLLInit pointer to LVDS_PLLInitTypeDef structure that contains information
  *       for the internal PLL instance (for single instance either MST PHY PLL or SLV PHY PLL)
  * @param  lock
  * @retval HAL Status
  */
HAL_StatusTypeDef LVDS_PHY_SetMonitorPLLLock(LVDS_HandleTypeDef *hlvds, const LVDS_PLLInitTypeDef *PLLInit,
                                                    uint32_t lock)
{
  /* Check the LVDS handle allocation */
  if (hlvds == NULL)
  {
    return HAL_ERROR;
  }
  /* Process locked */
  __HAL_LOCK(hlvds);

  if (PLLInit->ISMASTER == (uint8_t)1)
  {
    hlvds->Instance->PMMPLCR = lock;
  }
  else
  {
    hlvds->Instance->PSMPLCR = lock;
  }

  /* Release Lock */
  __HAL_UNLOCK(hlvds);

  return HAL_OK;
}

/**
  * @brief  set the PLL Reference clock monitor configuration
  * @param  hlvds pointer to a LVDS_HandleTypeDef structure that contains information
  *       for the LVDS
  * @param  PLLInit pointer to LVDS_PLLInitTypeDef structure that contains information
  *       for the internal PLL instance (for single instance either MST PHY PLL or SLV PHY PLL)
  * @param  ref
  * @retval HAL Status
  */
HAL_StatusTypeDef LVDS_PHY_SetMonitorPLLReferenceOK(LVDS_HandleTypeDef *hlvds,
                                                           const LVDS_PLLInitTypeDef *PLLInit, uint32_t ref)
{
  /* Check the LVDS handle allocation */
  if (hlvds == NULL)
  {
    return HAL_ERROR;
  }
  /* Process locked */
  __HAL_LOCK(hlvds);

  if (PLLInit->ISMASTER == (uint8_t)1)
  {
    hlvds->Instance->PMMPROCR = ref;
  }
  else
  {
    hlvds->Instance->PSMPROCR = ref;
  }

  /* Release Lock */
  __HAL_UNLOCK(hlvds);

  return HAL_OK;
}

/**
  * @brief  set the PHY serializer clock monitor configuration
  * @param  hlvds pointer to a LVDS_HandleTypeDef structure that contains information
  *       for the LVDS
  * @param  PLLInit pointer to LVDS_PLLInitTypeDef structure that contains information
  *       for the internal PLL instance (for single instance either MST PHY PLL or SLV PHY PLL)
  * @param  serializer_clk
  * @retval HAL Status
  */
HAL_StatusTypeDef LVDS_PHY_SetMonitorSerializerClk(LVDS_HandleTypeDef *hlvds, const LVDS_PLLInitTypeDef *PLLInit,
                                                          uint32_t serializer_clk)
{
  /* Check the LVDS handle allocation */
  if (hlvds == NULL)
  {
    return HAL_ERROR;
  }
  /* Process locked */
  __HAL_LOCK(hlvds);

  if (PLLInit->ISMASTER == (uint8_t)1)
  {
    hlvds->Instance->PMMSCCR = serializer_clk;
  }
  else
  {
    hlvds->Instance->PSMSCCR = serializer_clk;
  }

  /* Release Lock */
  __HAL_UNLOCK(hlvds);

  return HAL_OK;
}

/**
  * @brief  set the PHY serializer monitor configuration
  * @param  hlvds pointer to a LVDS_HandleTypeDef structure that contains information
  *       for the LVDS
  * @param  PLLInit pointer to LVDS_PLLInitTypeDef structure that contains information
  *       for the internal PLL instance (for single instance either MST PHY PLL or SLV PHY PLL)
  * @param  serializer
  * @retval HAL Status
  */
HAL_StatusTypeDef LVDS_PHY_SetMonitorSerializer(LVDS_HandleTypeDef *hlvds, const LVDS_PLLInitTypeDef *PLLInit,
                                                       uint32_t serializer)
{
  /* Check the LVDS handle allocation */
  if (hlvds == NULL)
  {
    return HAL_ERROR;
  }
  /* Process locked */
  __HAL_LOCK(hlvds);

  if (PLLInit->ISMASTER == (uint8_t)1)
  {
    hlvds->Instance->PMMSCR = serializer;
  }
  else
  {
    hlvds->Instance->PSMSCR = serializer;
  }

  /* Release Lock */
  __HAL_UNLOCK(hlvds);

  return HAL_OK;
}

/**
  * @brief  set the PHY debug configuration
  * @param  hlvds pointer to a LVDS_HandleTypeDef structure that contains information
  *       for the LVDS
  * @param  PLLInit pointer to LVDS_PLLInitTypeDef structure that contains information
  *       for the internal PLL instance (for single instance either MST PHY PLL or SLV PHY PLL)
  * @param  debug
  * @retval HAL Status
  */
HAL_StatusTypeDef LVDS_PHY_SetDebug(LVDS_HandleTypeDef *hlvds, const LVDS_PLLInitTypeDef *PLLInit,
                                           uint32_t debug)
{
  /* Check the LVDS handle allocation */
  if (hlvds == NULL)
  {
    return HAL_ERROR;
  }
  /* Process locked */
  __HAL_LOCK(hlvds);

  if (PLLInit->ISMASTER == (uint8_t)1)
  {
    hlvds->Instance->PMDCR = debug;
  }
  else
  {
    hlvds->Instance->PSDCR = debug;
  }

  /* Release Lock */
  __HAL_UNLOCK(hlvds);

  return HAL_OK;
}

/**
  * @brief  set the PHY data lane configuration
  * @param  hlvds pointer to a LVDS_HandleTypeDef structure that contains information
  *       for the LVDS
  * @param  PLLInit pointer to LVDS_PLLInitTypeDef structure that contains information
  *       for the internal PLL instance (for single instance either MST PHY PLL or SLV PHY PLL)
  * @param  config (ENABLE or DISABLE digital data lanes)
  * @retval HAL Status
  */
HAL_StatusTypeDef LVDS_PHY_EnableDataLanes(LVDS_HandleTypeDef *hlvds, const LVDS_PLLInitTypeDef *PLLInit,
                                                  uint32_t config)
{
  /* Check the LVDS handle allocation */
  if (hlvds == NULL)
  {
    return HAL_ERROR;
  }
  /* Process locked */
  __HAL_LOCK(hlvds);

  if (PLLInit->ISMASTER == (uint8_t)1)
  {
    hlvds->Instance->PMCFGCR = config;
  }
  else
  {
    hlvds->Instance->PSCFGCR = config;
  }

  /* Release Lock */
  __HAL_UNLOCK(hlvds);

  return HAL_OK;
}

/**
  * @brief  set the PLL Operational Mode configuration
  * @param  hlvds pointer to a LVDS_HandleTypeDef structure that contains information
  *       for the LVDS
  * @param  PLLInit pointer to LVDS_PLLInitTypeDef structure that contains information
  *       for the internal PLL instance (for single instance either MST PHY PLL or SLV PHY PLL)
  * @param  mode1
  * @param  mode2
  * @retval HAL Status
  */
HAL_StatusTypeDef LVDS_PHY_SetPLLMode(LVDS_HandleTypeDef *hlvds, const LVDS_PLLInitTypeDef *PLLInit,
                                             uint32_t mode1, uint32_t mode2)
{
  /* Check the LVDS handle allocation */
  if (hlvds == NULL)
  {
    return HAL_ERROR;
  }
  /* Process locked */
  __HAL_LOCK(hlvds);

  if (PLLInit->ISMASTER == (uint8_t)1)
  {
    hlvds->Instance->PMPLLCR1 = mode1;
    hlvds->Instance->PMPLLCR2 = mode2;
  }
  else
  {
    hlvds->Instance->PSPLLCR1 = mode1;
    hlvds->Instance->PSPLLCR2 = mode2;
  }

  /* Release Lock */
  __HAL_UNLOCK(hlvds);

  return HAL_OK;
}

/**
  * @brief  Configure the Sigma Delta block of PLL instance
  * @param  hlvds pointer to a LVDS_HandleTypeDef structure that contains information
  *       for the LVDS
  * @param  PLLInit pointer to LVDS_PLLInitTypeDef structure that contains information
  *       for the internal PLL instance (for single instance either MST PHY PLL or SLV PHY PLL)
  * @param  sd1
  * @param  sd2
  * @retval HAL Status
  */
HAL_StatusTypeDef LVDS_PHY_SetPLLSD(LVDS_HandleTypeDef *hlvds, const LVDS_PLLInitTypeDef *PLLInit, uint32_t sd1,
                                           uint32_t sd2)
{
  /* Check the LVDS handle allocation */
  if (hlvds == NULL)
  {
    return HAL_ERROR;
  }
  /* Process locked */
  __HAL_LOCK(hlvds);

  if (PLLInit->ISMASTER == (uint8_t)1)
  {
    hlvds->Instance->PMPLLSDCR1 = sd1;
    hlvds->Instance->PMPLLSDCR2 = sd2;
  }
  else
  {
    hlvds->Instance->PSPLLSDCR1 = sd1;
    hlvds->Instance->PSPLLSDCR2 = sd2;
  }

  /* Release Lock */
  __HAL_UNLOCK(hlvds);

  return HAL_OK;
}

/**
  * @brief  Configure the Triangular Wave Modulator block of PLL instance
  * @param  hlvds pointer to a LVDS_HandleTypeDef structure that contains information
  *       for the LVDS
  * @param  PLLInit pointer to LVDS_PLLInitTypeDef structure that contains information
  *       for the internal PLL instance (for single instance either MST PHY PLL or SLV PHY PLL)
  * @param  twg1
  * @param  twg2
  * @retval HAL Status
  */
HAL_StatusTypeDef LVDS_PHY_SetPLLTWG(LVDS_HandleTypeDef *hlvds, const LVDS_PLLInitTypeDef *PLLInit,
                                            uint32_t twg1, uint32_t twg2)
{
  /* Check the LVDS handle allocation */
  if (hlvds == NULL)
  {
    return HAL_ERROR;
  }
  /* Process locked */
  __HAL_LOCK(hlvds);

  if (PLLInit->ISMASTER == (uint8_t)1)
  {
    hlvds->Instance->PMPLLTWGCR1 = twg1;
    hlvds->Instance->PMPLLTWGCR2 = twg2;
  }
  else
  {
    hlvds->Instance->PSPLLTWGCR1 = twg1;
    hlvds->Instance->PSPLLTWGCR2 = twg2;
  }

  /* Release Lock */
  __HAL_UNLOCK(hlvds);

  return HAL_OK;
}

/**
  * @brief  Set the PLL LDO control configuration
  * @param  hlvds pointer to a LVDS_HandleTypeDef structure that contains information
  *       for the LVDS
  * @param  PLLInit pointer to LVDS_PLLInitTypeDef structure that contains information
  *       for the internal PLL instance (for single instance either MST PHY PLL or SLV PHY PLL)
  * @param  config
  * @retval HAL Status
  */
HAL_StatusTypeDef LVDS_PHY_SetPLLLDO(LVDS_HandleTypeDef *hlvds, const LVDS_PLLInitTypeDef *PLLInit,
                                            uint32_t config)
{
  /* Check the LVDS handle allocation */
  if (hlvds == NULL)
  {
    return HAL_ERROR;
  }
  /* Process locked */
  __HAL_LOCK(hlvds);

  if (PLLInit->ISMASTER == (uint8_t)1)
  {
    hlvds->Instance->PMPLLLDOCR = config;
  }
  else
  {
    hlvds->Instance->PSPLLLDOCR = config;
  }

  /* Release Lock */
  __HAL_UNLOCK(hlvds);

  return HAL_OK;
}

/**
  * @brief  Set the PLL Charge pump control configuration
  * @param  hlvds pointer to a LVDS_HandleTypeDef structure that contains information
  *       for the LVDS
  * @param  PLLInit pointer to LVDS_PLLInitTypeDef structure that contains information
  *       for the internal PLL instance (for single instance either MST PHY PLL or SLV PHY PLL)
  * @param  cp (charge pump configuration value)
  * @retval HAL Status
  */
HAL_StatusTypeDef LVDS_PHY_SetPLLCP(LVDS_HandleTypeDef *hlvds, const LVDS_PLLInitTypeDef *PLLInit, uint32_t cp)
{
  /* Check the LVDS handle allocation */
  if (hlvds == NULL)
  {
    return HAL_ERROR;
  }
  /* Process locked */
  __HAL_LOCK(hlvds);

  if (PLLInit->ISMASTER == (uint8_t)1)
  {
    hlvds->Instance->PMPLLCPCR = cp;
  }
  else
  {
    hlvds->Instance->PSPLLCPCR = cp;
  }

  /* Release Lock */
  __HAL_UNLOCK(hlvds);

  return HAL_OK;
}

/**
  * @brief  Set the PLL configuration
  * @param  hlvds pointer to a LVDS_HandleTypeDef structure that contains information
  *       for the LVDS
  * @param  PLLInit pointer to LVDS_PLLInitTypeDef structure that contains information
  *       for the internal PLL instance (for single instance either MST PHY PLL or SLV PHY PLL)
  * @param  config
  * @retval HAL Status
  */
HAL_StatusTypeDef LVDS_PHY_SetPLLConfig(LVDS_HandleTypeDef *hlvds, const LVDS_PLLInitTypeDef *PLLInit,
                                               uint32_t config)
{
  /* Check the LVDS handle allocation */
  if (hlvds == NULL)
  {
    return HAL_ERROR;
  }
  /* Process locked */
  __HAL_LOCK(hlvds);

  if (PLLInit->ISMASTER == (uint8_t)1)
  {
    hlvds->Instance->PMPLLCFGCR = config;
  }
  else
  {
    hlvds->Instance->PSPLLCFGCR = config;
  }

  /* Release Lock */
  __HAL_UNLOCK(hlvds);

  return HAL_OK;
}

/**
  * @brief  Set the PLL TEST control configuration
  * @param  hlvds pointer to a LVDS_HandleTypeDef structure that contains information
  *       for the LVDS
  * @param  PLLInit pointer to LVDS_PLLInitTypeDef structure that contains information
  *       for the internal PLL instance (for single instance either MST PHY PLL or SLV PHY PLL)
  * @param  test
  * @retval HAL Status
  */
HAL_StatusTypeDef LVDS_PHY_SetPLLTest(LVDS_HandleTypeDef *hlvds, const LVDS_PLLInitTypeDef *PLLInit,
                                             uint32_t test)
{
  /* Check the LVDS handle allocation */
  if (hlvds == NULL)
  {
    return HAL_ERROR;
  }
  /* Process locked */
  __HAL_LOCK(hlvds);

  if (PLLInit->ISMASTER == (uint8_t)1)
  {
    hlvds->Instance->PMPLLTESTCR = test;
  }
  else
  {
    hlvds->Instance->PSPLLTESTCR = test;
  }

  /* Release Lock */
  __HAL_UNLOCK(hlvds);

  return HAL_OK;
}

/**
  * @brief  Set the core configuration
  * @param  hlvds pointer to a LVDS_HandleTypeDef structure that contains information
  *       for the LVDS
  * @retval HAL Status
  */
HAL_StatusTypeDef core(LVDS_HandleTypeDef *hlvds)
{
  /* Check the LVDS handle allocation */
  if (hlvds == NULL)
  {
    return HAL_ERROR;
  }
  /* Process locked */
  __HAL_LOCK(hlvds);

  /* Release Lock */
  __HAL_UNLOCK(hlvds);

  return HAL_OK;
}

/**
  * @brief  Set the PLL PHY configuration (For Both Links)
  * @param  hlvds pointer to a LVDS_HandleTypeDef structure that contains information
  *       for the LVDS
  * @param  PLLInit pointer to LVDS_PLLInitTypeDef structure that contains information
  *       for the internal PLL instance (for single instance either MST PHY PLL or SLV PHY PLL)
  * @retval HAL Status
  */
HAL_StatusTypeDef HAL_LVDS_PHY_PLLConfig(LVDS_HandleTypeDef *hlvds, const LVDS_PLLInitTypeDef *PLLInit)
{
  /* Process locked */
  __HAL_LOCK(hlvds);

  /* Check PLLInit parameters */
  assert_param(IS_LVDS_PLL_FREF(PLLInit->PLLFREF));
  assert_param(IS_LVDS_PLL_BDIV(PLLInit->PLLBDIV));
  assert_param(IS_LVDS_PLL_MDIV(PLLInit->PLLMDIV));
  assert_param(IS_LVDS_PLL_NDIV(PLLInit->PLLNDIV));

  /* Power up bias and PLL dividers ******************************************************/

  /*POWER OK, masks the LVDS-PHY functionality*/
  WRITE_REG(hlvds->Instance->PMDCR, LVDS_PMDCR_MST_POWER_OK);
  WRITE_REG(hlvds->Instance->PSDCR, LVDS_PSDCR_SLV_POWER_OK);

  /*Enable the Current Mode driver for MST PHY data lanes*/
  SET_BIT(hlvds->Instance->PMCMCR1, LVDS_PMCMCR1_MST_CM_EN_DL3  \
          | LVDS_PMCMCR1_MST_CM_EN_DL2  \
          | LVDS_PMCMCR1_MST_CM_EN_DL1  \
          | LVDS_PMCMCR1_MST_CM_EN_DL0);

  SET_BIT(hlvds->Instance->PMCMCR2, LVDS_PMCMCR2_MST_CM_EN_DL4);


  /*Enable the Current Mode driver for SLV PHY data lanes*/
  SET_BIT(hlvds->Instance->PSCMCR1, LVDS_PSCMCR1_SLV_CM_EN_DL3  \
          | LVDS_PSCMCR1_SLV_CM_EN_DL2  \
          | LVDS_PSCMCR1_SLV_CM_EN_DL1  \
          | LVDS_PSCMCR1_SLV_CM_EN_DL0);

  SET_BIT(hlvds->Instance->PSCMCR2, LVDS_PSCMCR2_SLV_CM_EN_DL4);

  /*Enable the Voltage Mode driver for MST PHY data lanes*/
  SET_BIT(hlvds->Instance->PMBCR3, LVDS_PMBCR3_MST_VM_EN_DL3  \
          | LVDS_PMBCR3_MST_VM_EN_DL2 \
          | LVDS_PMBCR3_MST_VM_EN_DL1 \
          | LVDS_PMBCR3_MST_VM_EN_DL0 \
          | LVDS_PMBCR3_MST_VM_EN_DL4);

  /*Enable the Voltage Mode driver for SLV PHY data lanes*/
  SET_BIT(hlvds->Instance->PSBCR3, LVDS_PSBCR3_SLV_VM_EN_DL3  \
          | LVDS_PSBCR3_SLV_VM_EN_DL2 \
          | LVDS_PSBCR3_SLV_VM_EN_DL1 \
          | LVDS_PSBCR3_SLV_VM_EN_DL0 \
          | LVDS_PSBCR3_SLV_VM_EN_DL4);

  /*Enable the Local Bias at MST PHY data lanes*/
  SET_BIT(hlvds->Instance->PMBCR1, LVDS_PMBCR1_MST_EN_BIAS_DL3  \
          | LVDS_PMBCR1_MST_EN_BIAS_DL2 \
          | LVDS_PMBCR1_MST_EN_BIAS_DL1 \
          | LVDS_PMBCR1_MST_EN_BIAS_DL0 \
          | LVDS_PMBCR1_MST_EN_BIAS_DL4);
  /*Enable the Local Bias at SLV PHY data lanes*/
  SET_BIT(hlvds->Instance->PSBCR1, LVDS_PSBCR1_SLV_EN_BIAS_DL3  \
          | LVDS_PSBCR1_SLV_EN_BIAS_DL2 \
          | LVDS_PSBCR1_SLV_EN_BIAS_DL1 \
          | LVDS_PSBCR1_SLV_EN_BIAS_DL0 \
          | LVDS_PSBCR1_SLV_EN_BIAS_DL4);

  /*Enable the Data lanes of MST PHY*/
  SET_BIT(hlvds->Instance->PMCFGCR, LVDS_PMCFGCR_MST_EN_DIG_DL);

  /*Enable the Data lanes of SLV PHY*/
  SET_BIT(hlvds->Instance->PSCFGCR, LVDS_PSCFGCR_SLV_EN_DIG_DL);

  /* Release Lock */
  __HAL_UNLOCK(hlvds);

  return HAL_OK;
}

/**
  * @brief  LVDS host Data mapping configuration
  * @param  hlvds pointer to a LVDS_HandleTypeDef structure that contains information
  *       for the LVDS
  * @retval HAL Status
  */
HAL_StatusTypeDef HAL_LVDS_ConfigDataMapping(LVDS_HandleTypeDef *hlvds)
{
  HAL_StatusTypeDef status = HAL_OK;
  /*
     Expected JEIDA-RGB888 data to be sent in LSB format
             bit6 ............................bit0
     CHAN0   [ONE, ONE, ZERO, ZERO, ZERO, ONE, ONE]
     CHAN1   [G2,  R7,  R6,   R5,   R4,   R3,  R2]
     CHAN2   [B3,  B2,  G7,   G6,   G5,   G4,  G3]
     CHAN3   [DE,  VS,  HS,   B7,   B6,   B5,  B4]
     CHAN4   [CE,  B1,  B0,   G1,   G0,   R1,  R0]
    */

  const uint32_t HAL_LVDS_Map_JEIDA_RGB888[5][7] =
  {
    {
      HAL_LVDS_MAP_ONE, HAL_LVDS_MAP_ONE, HAL_LVDS_MAP_ZERO, HAL_LVDS_MAP_ZERO,
      HAL_LVDS_MAP_ZERO, HAL_LVDS_MAP_ONE, HAL_LVDS_MAP_ONE
    },            /* CH0 */
    {
      HAL_LVDS_MAP_G_2, HAL_LVDS_MAP_R_7, HAL_LVDS_MAP_R_6,  HAL_LVDS_MAP_R_5,
      HAL_LVDS_MAP_R_4,  HAL_LVDS_MAP_R_3, HAL_LVDS_MAP_R_2
    },
    {
      HAL_LVDS_MAP_B_3, HAL_LVDS_MAP_B_2, HAL_LVDS_MAP_G_7,  HAL_LVDS_MAP_G_6,
      HAL_LVDS_MAP_G_5,  HAL_LVDS_MAP_G_4, HAL_LVDS_MAP_G_3
    },
    {
      HAL_LVDS_MAP_D_E, HAL_LVDS_MAP_V_S, HAL_LVDS_MAP_H_S,  HAL_LVDS_MAP_B_7,
      HAL_LVDS_MAP_B_6,  HAL_LVDS_MAP_B_5, HAL_LVDS_MAP_B_4
    },
    {
      HAL_LVDS_MAP_C_E, HAL_LVDS_MAP_B_1, HAL_LVDS_MAP_B_0,  HAL_LVDS_MAP_G_1,
      HAL_LVDS_MAP_G_0,  HAL_LVDS_MAP_R_1, HAL_LVDS_MAP_R_0
    }            /* CH4 */
  };

  /*
     Expected VESA-RGB888 data to be sent in LSB format
              bit6 ............................bit0
     CHAN0   [ONE, ONE, ZERO, ZERO, ZERO, ONE, ONE]
     CHAN1   [G0,  R5,  R4,   R3,   R2,   R1,  R0]
     CHAN2   [B1,  B0,  G5,   G4,   G3,   G2,  G1]
     CHAN3   [DE,  VS,  HS,   B5,   B4,   B3,  B2]
     CHAN4   [CE,  B7,  B6,   G7,   G6,   R7,  R6]
  */

  const uint32_t HAL_LVDS_Map_VESA_RGB888[5][7] =
  {
    {
      HAL_LVDS_MAP_ONE, HAL_LVDS_MAP_ONE, HAL_LVDS_MAP_ZERO, HAL_LVDS_MAP_ZERO,
      HAL_LVDS_MAP_ZERO, HAL_LVDS_MAP_ONE, HAL_LVDS_MAP_ONE
    },                   /* CH0 */
    {
      HAL_LVDS_MAP_G_0, HAL_LVDS_MAP_R_5, HAL_LVDS_MAP_R_4,  HAL_LVDS_MAP_R_3,
      HAL_LVDS_MAP_R_2,  HAL_LVDS_MAP_R_1, HAL_LVDS_MAP_R_0
    },                   /* CH1 */
    {
      HAL_LVDS_MAP_B_1, HAL_LVDS_MAP_B_0, HAL_LVDS_MAP_G_5,  HAL_LVDS_MAP_G_4,
      HAL_LVDS_MAP_G_3,  HAL_LVDS_MAP_G_2, HAL_LVDS_MAP_G_1
    },                   /* CH2 */
    {
      HAL_LVDS_MAP_D_E, HAL_LVDS_MAP_V_S, HAL_LVDS_MAP_H_S,  HAL_LVDS_MAP_B_5,
      HAL_LVDS_MAP_B_4,  HAL_LVDS_MAP_B_3, HAL_LVDS_MAP_B_2
    },                   /* CH3 */
    {
      HAL_LVDS_MAP_C_E, HAL_LVDS_MAP_B_7, HAL_LVDS_MAP_B_6,  HAL_LVDS_MAP_G_7,
      HAL_LVDS_MAP_G_6,  HAL_LVDS_MAP_R_7, HAL_LVDS_MAP_R_6
    }                    /* CH4 */
  };

  /*
     Expected JEIDA/VESA-RGB666 data to be sent in LSB format
             bit6 ............................bit0
     CHAN0   [ONE, ONE, ZERO, ZERO, ZERO, ONE, ONE]
     CHAN1   [G2,  R7,  R6,   R5,   R4,   R3,  R2]
     CHAN2   [B3,  B2,  G7,   G6,   G5,   G4,  G3]
     CHAN3   [DE,  VS,  HS,   B7,   B6,   B5,  B4]
     CHAN4   [00,  00,  00,   00,   00,   00,  00]
  */

  const uint32_t HAL_LVDS_Map_RGB666[5][7] =
  {
    {
      HAL_LVDS_MAP_ONE,  HAL_LVDS_MAP_ONE,  HAL_LVDS_MAP_ZERO,  HAL_LVDS_MAP_ZERO,
      HAL_LVDS_MAP_ZERO,  HAL_LVDS_MAP_ONE,  HAL_LVDS_MAP_ONE
    },                  /* CH0 */
    {
      HAL_LVDS_MAP_G_2,  HAL_LVDS_MAP_R_7,  HAL_LVDS_MAP_R_6,   HAL_LVDS_MAP_R_5,
      HAL_LVDS_MAP_R_4,   HAL_LVDS_MAP_R_3,  HAL_LVDS_MAP_R_2
    },
    {
      HAL_LVDS_MAP_B_3,  HAL_LVDS_MAP_B_2,  HAL_LVDS_MAP_G_7,   HAL_LVDS_MAP_G_6,
      HAL_LVDS_MAP_G_5,   HAL_LVDS_MAP_G_4,  HAL_LVDS_MAP_G_3
    },
    {
      HAL_LVDS_MAP_D_E,  HAL_LVDS_MAP_V_S,  HAL_LVDS_MAP_H_S,   HAL_LVDS_MAP_B_7,
      HAL_LVDS_MAP_B_6,   HAL_LVDS_MAP_B_5,  HAL_LVDS_MAP_B_4
    },
    {
      HAL_LVDS_MAP_ZERO, HAL_LVDS_MAP_ZERO, HAL_LVDS_MAP_ZERO,  HAL_LVDS_MAP_ZERO,
      HAL_LVDS_MAP_ZERO,  HAL_LVDS_MAP_ZERO, HAL_LVDS_MAP_ZERO
    }                  /* CH4 */
  };
  /* Set Data Channels for RGB666 color coding */
  if (!(hlvds->Init.ColorCoding == LVDS_RGB888)) /* RGB666 */
  {
    status = HAL_LVDS_SetPattern(hlvds, HAL_LVDS_Map_RGB666);
  }
  if ((hlvds->Init.ColorCoding == LVDS_RGB888) && (hlvds->Init.DataMapping == LVDS_DT_MAP_VESA)) /* VESA-RGB888 */
  {

    status = HAL_LVDS_SetPattern(hlvds, HAL_LVDS_Map_VESA_RGB888);
  }
  if ((hlvds->Init.ColorCoding == LVDS_RGB888) && !((hlvds->Init.DataMapping == LVDS_DT_MAP_VESA))) /* JEIDA-RGB888 */
  {
    status = HAL_LVDS_SetPattern(hlvds, HAL_LVDS_Map_JEIDA_RGB888);
  }
  return status;
}
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/

/** @defgroup LVDS_Exported_Functions LVDS Exported Functions
  * @{
  */

/** @defgroup LVDS_Group1 Initialization and Configuration functions
  *  @brief   Initialization and Configuration functions
  *
@verbatim
 ===============================================================================
                ##### Initialization and Configuration functions #####
 ===============================================================================
    [..]  This section provides functions allowing to:
      (+) Initialize and configure the LVDS
      (+) De-initialize the LVDS

@endverbatim
  * @{
  */

/**
  * @brief  Initializes the LVDS according to the specified
  *         parameters in the LVDS_InitTypeDef and create the associated handle.
  * @param  hlvds  pointer to a LVDS_HandleTypeDef structure that contains
  *               the configuration information for the LVDS.
  * @param  PLLInit  pointer to a LVDS_PLLInitTypeDef structure that contains
  *                  the PLL Clock structure definition for the LVDS.
  * @param  cfg pointer to a LVDS_CfgTypeDef structure that contains LVDS
            configuration
  * @retval HAL status
 */
HAL_StatusTypeDef HAL_LVDS_Init(LVDS_HandleTypeDef *hlvds, LVDS_CfgTypeDef *cfg, LVDS_PLLInitTypeDef *PLLInit)
{
  /* Check the LVDS handle allocation */
  if (hlvds == NULL)
  {
    return HAL_ERROR;
  }

  /* Initialize the low level hardware */
  if (hlvds->State == HAL_LVDS_STATE_RESET)
  {
    HAL_LVDS_MspInit(hlvds);
  }
  /* Change LVDS peripheral state */
  hlvds->State = HAL_LVDS_STATE_BUSY;

  if (HAL_LVDS_PHY_Init(hlvds,  PLLInit) != HAL_OK)
  {
    hlvds->State = HAL_LVDS_STATE_ERROR;
  }

  if (HAL_LVDS_ConfigMode(hlvds, cfg) != HAL_OK)
  {
    hlvds->State = HAL_LVDS_STATE_ERROR;
  }

  /****************************** Error management *****************************/

  /* Reset the Error Mask and init the error code */
  hlvds->ErrorMsk = 0U;

  /* Initialize the error code */
  hlvds->ErrorCode = HAL_LVDS_ERROR_NONE;


  if (hlvds->State == HAL_LVDS_STATE_ERROR)
  {
    return HAL_ERROR;
  }

  /* Initialize the LVDS state*/
  hlvds->State = HAL_LVDS_STATE_READY;

  return HAL_OK;
}

/**
  * @brief  Initialize the LVDS PHY (both MST and SLV respectively) and also configure the
  *     respective PLL instance
  * @param  hlvds  pointer to a LVDS_HandleTypeDef structure that contains
  *               the configuration information for the LVDS.
  * @param  PLLInit  pointer to a LVDS_PLLInitTypeDef structure that contains
  *                  the PLL Clock structure definition for the LVDS.
  * @retval HAL status
 */
HAL_StatusTypeDef HAL_LVDS_PHY_Init(LVDS_HandleTypeDef *hlvds, LVDS_PLLInitTypeDef *PLLInit)
{
  HAL_StatusTypeDef status;

  /* Check the LVDS handle allocation */
  if (hlvds == NULL)
  {
    return HAL_ERROR;
  }

  if (PLLInit == NULL)
  {
    return HAL_ERROR;
  }
  /*************************** Enable LVDS PLL ***************************/
  /* Release LVDS PHY Master & Slave from reset */
  WRITE_REG(hlvds->Instance->PMGCR, LVDS_PMGCR_MST_DIVIDERS_RSTN | LVDS_PMGCR_MST_RSTZ);
  WRITE_REG(hlvds->Instance->PSGCR, LVDS_PSGCR_SLV_DIVIDERS_RSTN | LVDS_PSGCR_SLV_RSTZ);


  /*********************** Set the LVDS clock parameters ************************/

  /* Set PLL Slv & Mst configs and timings */
  LVDS_PHY_SetPLL(hlvds, PLLInit);

  status = HAL_LVDS_PHY_PLLConfig(hlvds, PLLInit);
  if (status != HAL_OK)
  {
    return status;
  }

  /*************************** Set the PHY parameters ***************************/

  if (LVDS_PHY_StartMst(hlvds) == HAL_TIMEOUT)
  {
    return HAL_ERROR;
  }

#if 0
 if (LVDS_PHY_StartSlv(hlvds) == HAL_TIMEOUT)
 {
   return HAL_ERROR;
 }

#endif
  return HAL_OK;
}

/**
  * @brief  De-initializes the LVDS peripheral registers to their default reset
  *         values.
  * @param  hlvds  pointer to a LVDS_HandleTypeDef structure that contains
  *               the configuration information for the LVDS.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_LVDS_DeInit(LVDS_HandleTypeDef *hlvds)
{
  /* Check the LVDS handle allocation */
  if (hlvds == NULL)
  {
    return HAL_ERROR;
  }

  /* Change LVDS peripheral state */
  hlvds->State = HAL_LVDS_STATE_BUSY;


  /* Disable LVDS module */
  __HAL_LVDS_DISABLE(hlvds);

  /* Disable D-PHY clocks and digital */
  CLEAR_BIT(hlvds->Instance->PMGCR, LVDS_PMGCR_MST_RSTZ);
  CLEAR_BIT(hlvds->Instance->PMGCR, (LVDS_PMGCR_MST_DP_CLK_OUT_ENABLE |
                                     LVDS_PMGCR_MST_LS_CLK_OUT_ENABLE |
                                     LVDS_PMGCR_MST_BIT_CLK_OUT_ENABLE));

  /* Shutdown the LVDS PLL */
  __HAL_LVDS_PLL_DISABLE(hlvds);

  /* Assert LVDS PHY Master & Slave in reset mode */
  CLEAR_BIT(hlvds->Instance->PMGCR, LVDS_PMGCR_MST_DIVIDERS_RSTN | LVDS_PMGCR_MST_RSTZ);
  CLEAR_BIT(hlvds->Instance->PSGCR, LVDS_PSGCR_SLV_DIVIDERS_RSTN | LVDS_PSGCR_SLV_RSTZ);

  /* DeInit the low level hardware */
  HAL_LVDS_MspDeInit(hlvds);

  /* Reinitialize ErrorCode */
  hlvds->ErrorCode = HAL_LVDS_ERROR_NONE;

  /* Initialize the LVDS state */
  hlvds->State = HAL_LVDS_STATE_RESET;

  /* Release Lock */
  __HAL_UNLOCK(hlvds);

  return HAL_OK;
}

/**
  * @brief  Initializes the LVDS MSP.
  * @param  hlvds  pointer to a LVDS_HandleTypeDef structure that contains
  *               the configuration information for the LVDS.
  * @retval None
  */
__weak void HAL_LVDS_MspInit(LVDS_HandleTypeDef *hlvds)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hlvds);
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_LVDS_MspInit could be implemented in the user file
   */
}

/**
  * @brief  De-initializes the LVDS MSP.
  * @param  hlvds  pointer to a LVDS_HandleTypeDef structure that contains
  *               the configuration information for the LVDS.
  * @retval None
  */
__weak void HAL_LVDS_MspDeInit(LVDS_HandleTypeDef *hlvds)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hlvds);

  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_LVDS_MspDeInit could be implemented in the user file
   */
}

/**
  * @}
  */

/** @defgroup LVDS_Group2 Peripheral Control functions
  *  @brief    Peripheral Control functions
  *
@verbatim
 ===============================================================================
                    ##### Peripheral Control functions #####
 ===============================================================================
    [..]  This section provides functions allowing to:
      (+) Configure the LVDS Mode (Single Pixel mode or Dual Pixel Mode)
      (+) Start the LVDS module
      (+) Stop the LVDS module
      (+) Set the Data Mapping Pattern
      (+) Start the Bist (PRBS Sequence Generator)
      (+) Stop the Bist (PRBS Sequence Generator)
      (+) Configure the Bist (PRBS Sequence Generator)
      (+) Set the LVDS host Lane Polarity
      (+) Set the custom channel distribution for PHY LINK

@endverbatim
  * @{
  */

/**
  * @brief  Configure the LVDS Operating Mode single pixel or dual pixel ,
  *     accordingly set channel distribution, and the link phase
  * @param  hlvds  pointer to a LVDS_HandleTypeDef structure that contains
  *               the configuration information for the LVDS.
  * @param  cfg pointer to a LVDS_CfgTypeDef structure that contains
  *                  the LVDS configuration
  * @retval HAL status
 */
HAL_StatusTypeDef HAL_LVDS_ConfigMode(LVDS_HandleTypeDef *hlvds, const LVDS_CfgTypeDef *cfg)
{
  uint32_t distribution;
  HAL_StatusTypeDef status;

  /* Check function parameters */
  assert_param(IS_LVDS_LINK(cfg->NumberOfLinks));
  assert_param(IS_LVDS_DISPLAY(cfg->NumberOfDisplays));
  assert_param(IS_LVDS_VS_POLARITY(cfg->VSPolarity));
  assert_param(IS_LVDS_HS_POLARITY(cfg->HSPolarity));
  assert_param(IS_LVDS_DE_POLARITY(cfg->DEPolarity));
  assert_param(IS_LVDS_LINK_PHASE(cfg->LinkPhase));


  CLEAR_BIT(hlvds->Instance->CR, LVDS_CR_LKMOD);

  /* Set channel distribution For Master PHY*/
  distribution = (LVDS_DATA_LANE_0 << LVDS_CDL1CR_DISTR0_Pos) + (LVDS_DATA_LANE_1 << LVDS_CDL1CR_DISTR1_Pos) \
                 + (LVDS_DATA_LANE_2 << LVDS_CDL1CR_DISTR2_Pos) + (LVDS_DATA_LANE_3 << LVDS_CDL1CR_DISTR3_Pos)
                 + (LVDS_CLOCK_LANE_MST << LVDS_CDL1CR_DISTR4_Pos);

  if (HAL_LVDS_SetChannelDistribution(hlvds, LVDS_LINK1_SELECT, distribution) != HAL_OK)
  {
    status =  HAL_ERROR;
    return status;
  }


  if (cfg->NumberOfLinks == LVDS_TWO_LINKS)
  {
    /*Enabling dual Link mode */
    SET_BIT(hlvds->Instance->CR, LVDS_CR_LKMOD);

    /* Set channel distribution For Slave PHY*/
    distribution = (LVDS_DATA_LANE_4 << LVDS_CDL2CR_DISTR0_Pos) + (LVDS_DATA_LANE_5 << LVDS_CDL2CR_DISTR1_Pos)
                   + (LVDS_DATA_LANE_6 << LVDS_CDL2CR_DISTR2_Pos) + (LVDS_DATA_LANE_7 << LVDS_CDL2CR_DISTR3_Pos)
                   + (LVDS_CLOCK_LANE_SLV << LVDS_CDL2CR_DISTR4_Pos);

    if (HAL_LVDS_SetChannelDistribution(hlvds, LVDS_LINK2_SELECT, distribution) != HAL_OK)
    {
      status =  HAL_ERROR;
      return status;
    }
  }

  if (cfg->NumberOfDisplays == LVDS_TWO_DISPLAYS)
  {

    /* Set channel distribution For Slave PHY*/
    distribution = (LVDS_DATA_LANE_0 << LVDS_CDL2CR_DISTR0_Pos) + (LVDS_DATA_LANE_1 << LVDS_CDL2CR_DISTR1_Pos)
                   + (LVDS_DATA_LANE_2 << LVDS_CDL2CR_DISTR2_Pos) + (LVDS_DATA_LANE_3 << LVDS_CDL2CR_DISTR3_Pos)
                   + (LVDS_CLOCK_LANE_MST << LVDS_CDL2CR_DISTR4_Pos);

    if (HAL_LVDS_SetChannelDistribution(hlvds, LVDS_LINK2_SELECT, distribution) != HAL_OK)
    {
      status =  HAL_ERROR;
      return status;
    }
  }

  /* Set Data Mapping */
  status = HAL_LVDS_ConfigDataMapping(hlvds);

  /* Set signal polarity */
  CLEAR_BIT(hlvds->Instance->CR, LVDS_CR_VSPOL | LVDS_CR_HSPOL | LVDS_CR_DEPOL);
  SET_BIT(hlvds->Instance->CR, cfg->VSPolarity | cfg->HSPolarity | cfg->DEPolarity);

  /* Set link phase */
  CLEAR_BIT(hlvds->Instance->CR, LVDS_CR_LKPHA);
  if (cfg->NumberOfLinks == LVDS_TWO_LINKS)
  {
    /*Setting Link phase in dual link mode*/
    SET_BIT(hlvds->Instance->CR, cfg->LinkPhase);
  }

  return status;
}

/**
  * @brief  Start the LVDS module
  * @param  hlvds  pointer to a LVDS_HandleTypeDef structure that contains
  *               the configuration information for the LVDS.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_LVDS_Start(LVDS_HandleTypeDef *hlvds)
{
  /* Process locked */
  __HAL_LOCK(hlvds);

  /* Enable the LVDS host */
  __HAL_LVDS_ENABLE(hlvds);

  /* Process unlocked */
  __HAL_UNLOCK(hlvds);

  return HAL_OK;
}

/**
  * @brief  Stop the LVDS module
  * @param  hlvds  pointer to a LVDS_HandleTypeDef structure that contains
  *               the configuration information for the LVDS.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_LVDS_Stop(LVDS_HandleTypeDef *hlvds)
{
  /* Process locked */
  __HAL_LOCK(hlvds);

  /* Disable the LVDS host */
  __HAL_LVDS_DISABLE(hlvds);

  /* Process unlocked */
  __HAL_UNLOCK(hlvds);

  return HAL_OK;
}

/**
  * @brief  set the Data mapping for LVDS host configuration
  * @param  hlvds  pointer to a LVDS_HandleTypeDef structure that contains
  *               the configuration information for the LVDS.
  * @param  pattern (standard supported Patterns for data mapping either VESA or JEIDA)
  * @retval HAL status
 */
HAL_StatusTypeDef HAL_LVDS_SetPattern(LVDS_HandleTypeDef *hlvds, const uint32_t pattern[5][7])
{
  /* Process locked */
  __HAL_LOCK(hlvds);

  /* Clear the generator */
  CLEAR_REG(hlvds->Instance->DMLCR0);
  CLEAR_REG(hlvds->Instance->DMMCR0);
  CLEAR_REG(hlvds->Instance->DMLCR1);
  CLEAR_REG(hlvds->Instance->DMMCR1);
  CLEAR_REG(hlvds->Instance->DMLCR2);
  CLEAR_REG(hlvds->Instance->DMMCR2);
  CLEAR_REG(hlvds->Instance->DMLCR3);
  CLEAR_REG(hlvds->Instance->DMMCR3);
  CLEAR_REG(hlvds->Instance->DMLCR4);
  CLEAR_REG(hlvds->Instance->DMMCR4);


  /* Set the pattern in Data Mapping */
  WRITE_REG(hlvds->Instance->DMLCR0, ((pattern[0][0]) + (pattern[0][1] << 5) + (pattern[0][2] << 10) \
                                      + (pattern[0][3] << 15)));
  WRITE_REG(hlvds->Instance->DMMCR0, ((pattern[0][4]) + (pattern[0][5] << 5) + (pattern[0][6] << 10)));
  WRITE_REG(hlvds->Instance->DMLCR1, ((pattern[1][0]) + (pattern[1][1] << 5) + (pattern[1][2] << 10) \
                                      + (pattern[1][3] << 15)));
  WRITE_REG(hlvds->Instance->DMMCR1, ((pattern[1][4]) + (pattern[1][5] << 5) + (pattern[1][6] << 10)));
  WRITE_REG(hlvds->Instance->DMLCR2, ((pattern[2][0]) + (pattern[2][1] << 5) + (pattern[2][2] << 10) \
                                      + (pattern[2][3] << 15)));
  WRITE_REG(hlvds->Instance->DMMCR2, ((pattern[2][4]) + (pattern[2][5] << 5) + (pattern[2][6] << 10)));
  WRITE_REG(hlvds->Instance->DMLCR3, ((pattern[3][0]) + (pattern[3][1] << 5) + (pattern[3][2] << 10) \
                                      + (pattern[3][3] << 15)));
  WRITE_REG(hlvds->Instance->DMMCR3, ((pattern[3][4]) + (pattern[3][5] << 5) + (pattern[3][6] << 10)));
  WRITE_REG(hlvds->Instance->DMLCR4, ((pattern[4][0]) + (pattern[4][1] << 5) + (pattern[4][2] << 10) \
                                      + (pattern[4][3] << 15)));
  WRITE_REG(hlvds->Instance->DMMCR4, ((pattern[4][4]) + (pattern[4][5] << 5) + (pattern[4][6] << 10)));

  /* Process unlocked */
  __HAL_UNLOCK(hlvds);

  return HAL_OK;
}

/**
  * @brief  Start the PRBS Generator for given polynomial
  * @param  hlvds pointer to a LVDS_HandleTypeDef structure that contains
  *               the configuration information for the LVDS.
  * @param  polyname
  * @retval HAL status
 */
HAL_StatusTypeDef HAL_LVDS_BistStart(LVDS_HandleTypeDef *hlvds, uint32_t polyname)
{
  /* Process locked */
  __HAL_LOCK(hlvds);

  /* check the parameters */
  assert_param(IS_LVDS_PRBS_POLYNOME(polyname));

  CLEAR_BIT(hlvds->Instance->CDL2CR, LVDS_CDL2CR_PRBS_Msk);
  SET_BIT(hlvds->Instance->CDL2CR, polyname);

  /* Process unlocked */
  __HAL_UNLOCK(hlvds);

  return HAL_OK;
}

/**
  * @brief  Stop the PRBS Generator for given polynomial
  * @param  hlvds pointer to a LVDS_HandleTypeDef structure that contains
  *               the configuration information for the LVDS.
  * @retval HAL status
 */
HAL_StatusTypeDef HAL_LVDS_BistStop(LVDS_HandleTypeDef *hlvds)
{
  /* Process locked */
  __HAL_LOCK(hlvds);

  CLEAR_BIT(hlvds->Instance->CDL2CR, LVDS_CDL2CR_PRBS_Msk);

  /* Process unlocked */
  __HAL_UNLOCK(hlvds);

  return HAL_OK;
}

/**
  * @brief  Set the custom channel distribution for PRBS sequence generator
  * @param  hlvds pointer to a LVDS_HandleTypeDef structure that contains
  *               the configuration information for the LVDS.
  * @param  link
  * @param  custom
  * @retval HAL status
 */
HAL_StatusTypeDef HAL_LVDS_BistConfig(LVDS_HandleTypeDef *hlvds, uint32_t link, uint32_t custom)
{
  return HAL_LVDS_SetChannelDistribution(hlvds, link, custom);
}

/**
  * @brief  configure the polarity of the differential output pair of data lane (fixes the PCB layout issue)
  * @param  hlvds pointer to a LVDS_HandleTypeDef structure that contains
  *               the configuration information for the LVDS.
  * @param  link
  * @param  channels
  * @retval HAL status
 */
HAL_StatusTypeDef HAL_LVDS_SetLanePolarity(LVDS_HandleTypeDef *hlvds, uint32_t link, uint32_t channels)
{
  HAL_StatusTypeDef status = HAL_OK;

  uint32_t lk1pol;
  uint32_t lk2pol;
  uint32_t lk2pol_msk = (LVDS_CR_LK2POL_Msk >> LVDS_CR_LK1POL_Pos);
  uint32_t lk1pol_msk = (LVDS_CR_LK1POL_Msk >> LVDS_CR_LK1POL_Pos);

  /* Process locked */
  __HAL_LOCK(hlvds);

  /* Check function parameters */
  assert_param(IS_LVDS_LINK(link));
  /* uint32_t channels has to be between 0x00U and 0x1FU */
  assert_param(IS_LVDS_CHANNEL(channels));


  /* Computing LK1POL & LK2POL
     channels[9:0] is LVDS_CR.LK2POL + LVDS_CR.LK1POL */
  lk1pol = (channels & lk1pol_msk);
  lk2pol = ((channels & lk2pol_msk) >> 5U);

  /* Set Link polarity */
  CLEAR_BIT(hlvds->Instance->CR, LVDS_CR_LK1POL);
  SET_BIT(hlvds->Instance->CR, (lk1pol << LVDS_CR_LK1POL_Pos));

  CLEAR_BIT(hlvds->Instance->CR, LVDS_CR_LK2POL);
  SET_BIT(hlvds->Instance->CR, (lk2pol << LVDS_CR_LK2POL_Pos));

  /* Process unlocked */
  __HAL_UNLOCK(hlvds);

  return status;
}

/**
  * @brief  Set custom value for a given lane
  * @param  hlvds      pointer to a LVDS_HandleTypeDef structure that contains
  *                    the configuration information for the LVDS.
  * @param  link   Lane to be modified.
  *                    This parameter can be LVDS_LINK1_SELECT or LVDS_LINK2_SELECT
  * @param  distribution     Custom value to modify
  *                    This parameter can be any value of LVDS_Channel_Distribution_values.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_LVDS_SetChannelDistribution(LVDS_HandleTypeDef *hlvds, uint32_t link, uint32_t distribution)
{
  /* Process locked */
  __HAL_LOCK(hlvds);

  /* Check function parameters */
  assert_param(IS_LVDS_LINK_SELECTED(link));
  assert_param(IS_LVDS_CHANNEL(distribution));

  if (link == LVDS_LINK1_SELECT)
  {
    CLEAR_REG(hlvds->Instance->CDL1CR);
    WRITE_REG(hlvds->Instance->CDL1CR, distribution);
  }
  else
  {
    CLEAR_REG(hlvds->Instance->CDL2CR);
    WRITE_REG(hlvds->Instance->CDL2CR, distribution);
  }

  /* Process unlocked */
  __HAL_UNLOCK(hlvds);

  return HAL_OK;
}

/**
  * @}
  */

/** @defgroup LVDS_Group3 Peripheral State and Errors functions
  *  @brief    Peripheral State and Errors functions
  *
@verbatim
 ===============================================================================
                  ##### Peripheral State and Errors functions #####
 ===============================================================================
    [..]
    This subsection provides functions allowing to
      (+) Check the LVDS state.
      (+) Get error code.

@endverbatim
  * @{
  */

/**
  * @brief  Return the LVDS state
  * @param  hlvds  pointer to a LVDS_HandleTypeDef structure that contains
  *               the configuration information for the LVDS.
  * @retval HAL state
  */
HAL_LVDS_StateTypeDef HAL_LVDS_GetState(const LVDS_HandleTypeDef *hlvds)
{
  return hlvds->State;
}

/**
  * @brief  Return the LVDS error code
  * @param  hlvds  pointer to a LVDS_HandleTypeDef structure that contains
  *               the configuration information for the DSI.
  * @retval LVDS Error Code
  */
uint32_t HAL_LVDS_GetError(const LVDS_HandleTypeDef *hlvds)
{
  /* Get the error code */
  return hlvds->ErrorCode;
}

/**
  * @}
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

#endif /* HAL_LVDS_MODULE_ENABLED */
