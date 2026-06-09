/**
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

#include "stm32mp2xx_hal_ddr_ddrphy_phyinit.h"

/*
 * This function implements the flow of PhyInit software to initialize the PHY.
 *
 * The execution sequence follows the overview figure provided in the PhyInit App Note.
 *
 * \returns 0 on completion of the sequence, EXIT_FAILURE on error.
 */
int32_t ddrphy_phyinit_sequence(bool skip_training, bool reten)
{
  int32_t ret;
  bool skip_train = skip_training;

  /* Check user input pstate number consistency vs. SW capabilities */
  if (userinputbasic.numpstates > NB_PS)
  {
    return -1;
  }

  VERBOSE("%s Start\n", __func__);

  /* Initialize structures */
  ddrphy_phyinit_initstruct();

  /* Re-calculate Firmware Message Block input based on final user input */
  ret = ddrphy_phyinit_calcmb();
  if (ret != 0)
  {
    return ret;
  }

  /* (A) Bring up VDD, VDDQ, and VAA */
  /* call ddrphy_phyinit_usercustom_a_bringuppower() if needed */

  /* (B) Start Clocks and Reset the PHY */
  /* call ddrphy_phyinit_usercustom_b_startclockresetphy() if needed */

  /* (C) Initialize PHY Configuration */
  ret = ddrphy_phyinit_c_initphyconfig();
  if (ret != 0)
  {
    return ret;
  }
  /*
   * Customize any register write desired; This can include any CSR not covered by PhyInit
   * or user wish to override values calculated in step_C
   */
  ddrphy_phyinit_usercustom_pretrain();

  /* Stop retention register tracking for training firmware related registers */
  ret = ddrphy_phyinit_reginterface(STOPTRACK, 0, 0);
  if (ret != 0)
  {
    return ret;
  }

#ifdef USE_STM32MP257CXX_EMU
  /* No firmware execution (i.e. always skip training) in emulation setup */
  skip_train = true;
#endif /* USE_STM32MP257CXX_EMU */

  if (skip_train) {
    /* Skip running training firmware entirely */
    ddrphy_phyinit_progcsrskiptrain();
#ifndef USE_STM32MP257CXX_EMU
  }
  else
  {
    int32_t pstate;

    /* Run all 1D power states, then 2D P0, to reduce total Imem/Dmem loads. */

    /* (D) Load the IMEM Memory for 1D training */
    ddrphy_phyinit_d_loadimem();

    for (pstate = 0; pstate < userinputbasic.numpstates; pstate++)
    {
      /* (E) Set the PHY input clocks to the desired frequency */
      /* call ddrphy_phyinit_usercustom_e_setdficlk(pstate) if needed */

      /*
       * Note: this routine implies other items such as dfifreqratio, DfiCtlClk
       * are also set properly.
       * Because the clocks are controlled in the SOC, external to the software
       * and PHY, this step intended to be replaced by the user with the necessary
       * SOC operations to achieve the new input frequency to the PHY.
       */

      /* (F) Write the Message Block parameters for the training firmware */
      ret = ddrphy_phyinit_f_loaddmem(pstate);
      if (ret != 0)
      {
        return ret;
      }

      /* (G) Execute the Training Firmware */
      ret = ddrphy_phyinit_g_execfw();
      if (ret != 0)
      {
        return ret;
      }
    }
#endif /* !USE_STM32MP257CXX_EMU */
  }

  /* Start retention register tracking for training firmware related registers */
  ret = ddrphy_phyinit_reginterface(STARTTRACK, 0, 0);
  if (ret != 0)
  {
    return ret;
  }

  /* (I) Load PHY Init Engine Image */
  ddrphy_phyinit_i_loadpieimage(skip_train);

  /*
   * Customize any CSR write desired to override values programmed by firmware or
   * ddrphy_phyinit_i_loadpieimage()
   */
  ddrphy_phyinit_usercustom_posttrain();

  if (reten)
  {
    /* Save value of tracked registers for retention restore sequence. */
    /*
     * Following call has been dissociated from PHY init sequence.
     * A dedicated HAL_DDR_SaveRetentionData() is now exposed in HAL driver API.
     */
    /* ret = ddrphy_phyinit_usercustom_saveretregs(); */
    if (ret != 0)
    {
      return ret;
    }

    retention_enable = reten;
  }

  /* (J) Initialize the PHY to Mission Mode through DFI Initialization */
  /* call ddrphy_phyinit_usercustom_j_entermissionmode();= if needed */

  VERBOSE("%s End\n", __func__);

  return 0;
}
