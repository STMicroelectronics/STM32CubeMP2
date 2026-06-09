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

#include <stdlib.h>

#include "stm32mp2xx_hal_ddr_ddrphy_phyinit.h"

/*
 * Reads PhyInit inputs structures and sets relevant message block
 * parameters.
 *
 * This function sets Message Block parameters based on user_input_basic and
 * user_input_advanced. user changes in these files takes precedence
 * over this function call.
 *
 * MessageBlock fields set ::
 *
 *  - dramtype
 *  - pstate
 *  - dramfreq
 *  - pllbypassen
 *  - dfifreqratio
 *  - phyodtimpedance
 *  - phydrvimpedance
 *  - bpznresval
 *  - enableddqscha (LPDDR4)
 *  - cspresentcha (LPDDR4)
 *  - enableddqsChb (LPDDR4)
 *  - cspresentchb (LPDDR4)
 *  - enableddqs (DDR3/DDR4)
 *  - phycfg (DDR3/DDR4)
 *  - x16present (DDR4)
 *
 * \return 0 on success.
 */
int32_t ddrphy_phyinit_calcmb(void)
{
  int32_t myps;
  int32_t nad0 = userinputbasic.numactivedbytedfi0;
  int32_t nad1 = 0;
  uint16_t mr4 __unused;

  VERBOSE("%s Start\n", __func__);

#if STM32MP_LPDDR4_TYPE
  nad1 = userinputbasic.numactivedbytedfi1;
#endif /* STM32MP_LPDDR4_TYPE */

  /* A few checks to make sure valid programming */
  if ((nad0 <= 0) || (nad1 < 0) || (userinputbasic.numdbyte <= 0))
  {
    ERROR("%s numactivedbytedfi0, numactivedbytedfi1, numdbyte out of range.\n", __func__);
    return -1;
  }

  if ((nad0 + nad1) > userinputbasic.numdbyte)
  {
    ERROR("%s numactivedbytedfi0+numactivedbytedfi1 is larger than numdbyte\n", __func__);
    return -1;
  }

  if ((userinputbasic.dfi1exists == 0) && (nad1 != 0))
  {
    ERROR("%s dfi1exists==0 but numdbytedfi1 != 0\n", __func__);
    return -1;
  }

#if STM32MP_DDR4_TYPE
  /* OR all mr4 masked values, to help check in next loop */
  mr4 = 0;
  for (myps = 0; myps < userinputbasic.numpstates; myps++)
  {
    mr4 |= mb_ddr_1d[myps].mr4 & 0x1C0U;
  }
#endif /* STM32MP_DDR4_TYPE */

  /* 1D message block defaults */
  for (myps = 0; myps < userinputbasic.numpstates; myps++)
  {
    uint16_t disableddbyte __unused;
    int32_t dbyte __unused;
    int32_t ret;

#if STM32MP_DDR4_TYPE
    if (mr4 != 0x0U)
    {
      ERROR("%s Setting DRAM CAL mode is not supported by the PHY.\n", __func__);
      ERROR("Memory controller may set CAL mode after PHY has entered mission\n");
      ERROR("mode. Please check value programmed in mb_ddr_1d[*].mr4\n");
      ERROR("and unset A8:6\n");
      return -1;
    }
#endif /* STM32MP_DDR4_TYPE */

#if STM32MP_DDR3_TYPE
#ifdef USE_STM32MP257CXX_EMU
    if ((userinputbasic.dimmtype == (int32_t)DDR_DIMMTYPE_UDIMM) ||
        (userinputbasic.dimmtype == (int32_t)DDR_DIMMTYPE_NODIMM))
#else
    if (userinputbasic.dimmtype == (int32_t)DDR_DIMMTYPE_NODIMM)
#endif /* USE_STM32MP257CXX_EMU */
    {
      ret = ddrphy_phyinit_softsetmb(myps, SSMB_DRAMTYPE, 0x1);
      if (ret != 0)
      {
        return ret;
      }
    }
#elif STM32MP_DDR4_TYPE
#ifdef USE_STM32MP257CXX_EMU
    if ((userinputbasic.dimmtype == (int32_t)DDR_DIMMTYPE_UDIMM) ||
        (userinputbasic.dimmtype == (int32_t)DDR_DIMMTYPE_NODIMM))
#else
    if (userinputbasic.dimmtype == (int32_t)DDR_DIMMTYPE_NODIMM)
#endif /* USE_STM32MP257CXX_EMU */
    {
      ret = ddrphy_phyinit_softsetmb(myps, SSMB_DRAMTYPE, 0x2);
      if (ret != 0)
      {
        return ret;
      }
    }
#endif /* STM32MP_DDR4_TYPE */

    ret = ddrphy_phyinit_softsetmb(myps, SSMB_PSTATE, myps);
    if (ret != 0)
    {
      return ret;
    }

    ret = ddrphy_phyinit_softsetmb(myps, SSMB_DRAMFREQ, userinputbasic.frequency[myps] * 2);
    if (ret != 0)
    {
      return ret;
    }

    ret = ddrphy_phyinit_softsetmb(myps, SSMB_PLLBYPASSEN, userinputbasic.pllbypass[myps]);
    if (ret != 0)
    {
      return ret;
    }

    if (userinputbasic.dfifreqratio[myps] == 1)
    {
      ret = ddrphy_phyinit_softsetmb(myps, SSMB_DFIFREQRATIO, 0x2);
      if (ret != 0)
      {
        return ret;
      }
    }

    ret = ddrphy_phyinit_softsetmb(myps, SSMB_PHYODTIMPEDANCE, 0);
    if (ret != 0)
    {
      return ret;
    }

    ret = ddrphy_phyinit_softsetmb(myps, SSMB_PHYDRVIMPEDANCE, 0);
    if (ret != 0)
    {
      return ret;
    }

    ret = ddrphy_phyinit_softsetmb(myps, SSMB_BPZNRESVAL, 0);
    if (ret != 0)
    {
      return ret;
    }

#if STM32MP_DDR3_TYPE || STM32MP_DDR4_TYPE
    ret = ddrphy_phyinit_softsetmb(myps, SSMB_ENABLEDDQS, nad0 * 8);
    if (ret != 0)
    {
      return ret;
    }

    disableddbyte = 0x0U;

    for (dbyte = 0; (dbyte < userinputbasic.numdbyte) && (dbyte < 8); dbyte++)
    {
      if (ddrphy_phyinit_isdbytedisabled(dbyte))
      {
        disableddbyte |= 0x1U << dbyte;
      }
    }

    ret = ddrphy_phyinit_softsetmb(myps, SSMB_DISABLEDDBYTE, (int32_t)disableddbyte);
    if (ret != 0)
    {
      return ret;
    }

#if STM32MP_DDR3_TYPE
    ret = ddrphy_phyinit_softsetmb(myps, SSMB_PHYCFG, userinputadvanced.is2ttiming[myps]);
    if (ret != 0)
    {      return ret;
    }
#else
    ret = ddrphy_phyinit_softsetmb(myps, SSMB_PHYCFG,
                 ((mb_ddr_1d[myps].mr3 & 0x8U) != 0U) ? 0 : userinputadvanced.is2ttiming[myps]);
    if (ret != 0)
    {
      return ret;
    }

    ret = ddrphy_phyinit_softsetmb(myps, SSMB_X16PRESENT,
                 (0x10 == userinputbasic.dramdatawidth) ? (int32_t)mb_ddr_1d[myps].cspresent : 0x0);
    if (ret != 0)
    {
      return ret;
    }
#endif /* STM32MP_DDR3_TYPE */
#elif STM32MP_LPDDR4_TYPE
    ret = ddrphy_phyinit_softsetmb(myps, SSMB_ENABLEDDQSCHA, nad0 * 8);
    if (ret != 0)
    {
      return ret;
    }

    ret = ddrphy_phyinit_softsetmb(myps, SSMB_CSPRESENTCHA,
                 (2 == userinputbasic.numrank_dfi0) ? 0x3 : userinputbasic.numrank_dfi0);
    if (ret != 0)
    {
      return ret;
    }

    ret = ddrphy_phyinit_softsetmb(myps, SSMB_ENABLEDDQSCHB, nad1 * 8);
    if (ret != 0)
    {
      return ret;
    }

    ret = ddrphy_phyinit_softsetmb(myps, SSMB_CSPRESENTCHB,
                 (2 == userinputbasic.numrank_dfi1) ? 0x3 : userinputbasic.numrank_dfi1);
    if (ret != 0)
    {
      return ret;
    }
#endif /* STM32MP_LPDDR4_TYPE */
  } /* myps */

  VERBOSE("%s End\n", __func__);

  return 0;
}
