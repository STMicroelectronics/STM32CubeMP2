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

#ifndef DDRPHY_PHYINIT_H
#define DDRPHY_PHYINIT_H

#include <stdbool.h>

#include "stm32mp2xx_hal_ddr_ddrphy_phyinit_usercustom.h"

/* Global Structures : instantiated in ddrphy_globals.c */
extern bool retention_enable;

extern user_input_basic_t userinputbasic;
extern user_input_advanced_t userinputadvanced;
extern user_input_mode_register_t userinputmoderegister;
extern user_input_swizzle_t userinputswizzle;

extern pmu_smb_ddr_1d_t mb_ddr_1d[NB_PS];
extern pmu_smb_ddr_1d_t shdw_ddr_1d[NB_PS];

/* Function definitions */
int32_t ddrphy_phyinit_softsetmb(int32_t ps, softsetmb_field field, int32_t value);
void ddrphy_phyinit_initstruct(void);
void *ddrphy_get_ui_basic_base(void);
int32_t ddrphy_get_ui_basic_pllbypass_0(void);
void *ddrphy_get_ui_advanced_base(void);
void *ddrphy_get_ui_mode_register_base(void);
void *ddrphy_get_ui_swizzle_base(void);

#endif /* DDRPHY_PHYINIT_H */
