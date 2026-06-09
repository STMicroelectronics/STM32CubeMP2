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

#include <string.h>

#include "stm32mp2xx_hal_ddr_ddrphy_phyinit.h"

/*
 * Set messageBlock variable only if not set by user
 *
 * This function is used by ddrphy_phyinit_calcmb() to set calculated
 * messageBlock variables only when the user has not directly programmed them.
 *
 * @param[in]   ps      integer between 0-3. Specifies the PState for which the messageBlock field should be set.
 * @param[in]   field   messageBlock field to be programed.
 * @param[in]   value   field value
 *
 * @return 0 on success.
 * On error  returns the following values based on error:
 * - -1 : message block field specified by the input \c field string is not
 * found in the message block data structure.
 * - -2 : when dramtype does not support 2D training but a 2D training field is
 * programmed.
 */
int32_t ddrphy_phyinit_softsetmb(int32_t ps, softsetmb_field field, int32_t value)
{
  int32_t ret = 0;

  switch (field)
  {
    case SSMB_PSTATE:
      if (shdw_ddr_1d[ps].pstate == 0U)
      {
        mb_ddr_1d[ps].pstate = (uint8_t)value;
      }
      break;
    case SSMB_PLLBYPASSEN:
      if (shdw_ddr_1d[ps].pllbypassen == 0U)
      {
        mb_ddr_1d[ps].pllbypassen = (uint8_t)value;
      }
      break;
    case SSMB_DRAMFREQ:
      if (shdw_ddr_1d[ps].dramfreq == 0U)
      {
        mb_ddr_1d[ps].dramfreq = (uint16_t)value;
      }
      break;
    case SSMB_DFIFREQRATIO:
      if (shdw_ddr_1d[ps].dfifreqratio == 0U)
      {
        mb_ddr_1d[ps].dfifreqratio = (uint8_t)value;
      }
      break;
    case SSMB_BPZNRESVAL:
      if (shdw_ddr_1d[ps].bpznresval == 0U)
      {
        mb_ddr_1d[ps].bpznresval = (uint8_t)value;
      }
      break;
    case SSMB_PHYODTIMPEDANCE:
      if (shdw_ddr_1d[ps].phyodtimpedance == 0U)
      {
        mb_ddr_1d[ps].phyodtimpedance = (uint8_t)value;
      }
      break;
    case SSMB_PHYDRVIMPEDANCE:
      if (shdw_ddr_1d[ps].phydrvimpedance == 0U)
      {
        mb_ddr_1d[ps].phydrvimpedance = (uint8_t)value;
      }
      break;
#if STM32MP_DDR3_TYPE || STM32MP_DDR4_TYPE
    case SSMB_DRAMTYPE:
      if (shdw_ddr_1d[ps].dramtype == 0U)
      {
        mb_ddr_1d[ps].dramtype = (uint8_t)value;
      }
      break;
    case SSMB_DISABLEDDBYTE:
      if (shdw_ddr_1d[ps].disableddbyte == 0U)
      {
        mb_ddr_1d[ps].disableddbyte = (uint8_t)value;
      }
      break;
    case SSMB_ENABLEDDQS:
      if (shdw_ddr_1d[ps].enableddqs == 0U)
      {
       mb_ddr_1d[ps].enableddqs = (uint8_t)value;
      }
      break;
    case SSMB_PHYCFG:
      if (shdw_ddr_1d[ps].phycfg == 0U)
      {
        mb_ddr_1d[ps].phycfg = (uint8_t)value;
      }
      break;
#endif /* STM32MP_DDR3_TYPE || STM32MP_DDR4_TYPE */
#if STM32MP_DDR4_TYPE
    case SSMB_X16PRESENT:
      if (shdw_ddr_1d[ps].x16present == 0U)
      {
        mb_ddr_1d[ps].x16present = (uint8_t)value;
      }
      break;
#endif /* STM32MP_DDR4_TYPE */
#if STM32MP_LPDDR4_TYPE
    case SSMB_ENABLEDDQSCHA:
      if (shdw_ddr_1d[ps].enableddqscha == 0U)
      {
        mb_ddr_1d[ps].enableddqscha = (uint8_t)value;
      }
      break;
    case SSMB_CSPRESENTCHA:
      if (shdw_ddr_1d[ps].cspresentcha == 0U)
      {
        mb_ddr_1d[ps].cspresentcha = (uint8_t)value;
      }
      break;
    case SSMB_ENABLEDDQSCHB:
      if (shdw_ddr_1d[ps].enableddqschb == 0U)
      {
        mb_ddr_1d[ps].enableddqschb = (uint8_t)value;
      }
      break;
    case SSMB_CSPRESENTCHB:
      if (shdw_ddr_1d[ps].cspresentchb == 0U)
      {
        mb_ddr_1d[ps].cspresentchb = (uint8_t)value;
      }
      break;
#endif /* STM32MP_LPDDR4_TYPE */
    default:
      ERROR("%s unknown message block field name '%s'\n", __func__, field);
      ret = -1;
      break;
  }

  return ret;
}
