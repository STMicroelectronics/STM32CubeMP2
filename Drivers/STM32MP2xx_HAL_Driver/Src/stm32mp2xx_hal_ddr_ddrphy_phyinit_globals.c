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

/* Global struct defines */
bool retention_enable;      /*
                             * Retention Enable input parameter, instructs phyinit to issue
                             * register reads during initialization to retention registers.
                             */

user_input_basic_t userinputbasic;
user_input_advanced_t userinputadvanced;
user_input_mode_register_t userinputmoderegister;
user_input_swizzle_t userinputswizzle;

/* Firmware 1D Message Block structures */
pmu_smb_ddr_1d_t mb_ddr_1d[NB_PS];
/* Shadow of 1D message block. Used by PhyInit to track user changes to the data structure */
pmu_smb_ddr_1d_t shdw_ddr_1d[NB_PS];

/*
 * Represent the value stored in Step C into the register with the same name.
 * Defined as a global variable so that implementation of ddrphy_phyinit_progcsrskiptrain()
 * function does not require a PHY read register implementation.
 */
int32_t ardptrinitval[NB_PS];
