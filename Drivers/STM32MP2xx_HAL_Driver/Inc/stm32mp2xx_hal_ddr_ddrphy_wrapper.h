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

#ifndef DDRPHY_WRAPPER_H
#define DDRPHY_WRAPPER_H

static inline int64_t fmodll(int64_t x, int64_t y)
{
  return x - ((x / y) * y);
}

static inline int32_t fmodi(int32_t x, int32_t y)
{
  return (int)fmodll((int64_t)x, (int64_t)y);
}

#endif /* DDRPHY_WRAPPER_H */
