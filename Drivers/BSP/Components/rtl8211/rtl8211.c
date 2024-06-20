/**
 ******************************************************************************
 * @file    rtl8211.c
 * @author  MCD Application Team
 * @brief   This file provides a set of functions needed to manage the RTL8211
 *          PHY devices.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "rtl8211.h"
#include "stdio.h"

/** @addtogroup BSP
 * @{
 */

/** @addtogroup Component
 * @{
 */

/** @defgroup RTL8211
 * @{
 */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/** @defgroup RTL8211_Private_Defines RTL8211 Private Defines
 * @{
 */

#define RTL8211_INIT_TO        ((uint32_t)5000U)
#define RTL8211_SW_RESET_TO    ((uint32_t)500U)
#define RTL8211_MAX_DEV_ADDR   ((uint32_t)31U)

/**
 * @}
 */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/** @defgroup RTL8211_Private_Functions RTL8211 Private Functions
 * @{
 */

/**
 * @brief  Initialize the rtl8211 and configure the needed hardware resources
 * @param  pObj: device object rtl8211_Object_t.
 * @retval RTL8211_STATUS_OK  if OK
 *         RTL8211_STATUS_ADDRESS_ERROR if cannot find device address
 *         RTL8211_STATUS_READ_ERROR if cannot read register
 *         RTL8211_STATUS_WRITE_ERROR if cannot write to register
 *         RTL8211_STATUS_RESET_TIMEOUT if cannot perform a software reset
 */
int32_t RTL8211_Init(rtl8211_Object_t *pObj)
{
  uint32_t tickstart = 0, regvalue = 0, addr = 0;
  int32_t status = RTL8211_STATUS_ERROR;

  if(pObj->Is_Initialized == 0)
  {
    if(pObj->IO.Init != 0)
    {
      /* GPIO and Clocks initialization */
      pObj->IO.Init();
    }

    /* for later check */
    pObj->DevAddr = RTL8211_MAX_DEV_ADDR + 1;

    /* Get the device address from special mode register */
    for(addr = 1; addr <= RTL8211_MAX_DEV_ADDR; addr ++)
    {
      if(pObj->IO.ReadReg(addr, RTL8211_PHYID1, &regvalue) < 0)
      {
        status = RTL8211_STATUS_READ_ERROR;
        /* Can't read from this device address
            continue with next address */
        continue;
      }

      if(regvalue != 0xFFFF)
      {
        pObj->DevAddr = addr;
        status = RTL8211_STATUS_OK;
        break;
      }
    }

    if(pObj->DevAddr > RTL8211_MAX_DEV_ADDR)
    {
      status = RTL8211_STATUS_ADDRESS_ERROR;
    }

    /* if device address is matched */
    if(status == RTL8211_STATUS_OK)
    {
      /* set a software reset  */
      if(pObj->IO.WriteReg(pObj->DevAddr, RTL8211_BMCR, RTL8211_BMCR_RESET) >= 0)
      {
        /* get software reset status */
        if(pObj->IO.ReadReg(pObj->DevAddr, RTL8211_BMCR, &regvalue) >= 0)
        {
          tickstart = pObj->IO.GetTick();

          /* wait until software reset is done or timeout occured  */
          while(regvalue & RTL8211_BMCR_RESET)
          {
            if((pObj->IO.GetTick() - tickstart) <= RTL8211_SW_RESET_TO)
            {
              if(pObj->IO.ReadReg(pObj->DevAddr, RTL8211_BMCR, &regvalue) < 0)
              {
                status = RTL8211_STATUS_READ_ERROR;
                break;
              }
            }
            else
            {
              status = RTL8211_STATUS_RESET_TIMEOUT;
              break;
            }
          }
        }
        else
        {
          status = RTL8211_STATUS_READ_ERROR;
        }
      }
      else
      {
        status = RTL8211_STATUS_WRITE_ERROR;
      }
    }
  }

  if(status == RTL8211_STATUS_OK)
  {

    tickstart =  pObj->IO.GetTick();

    /* Wait for 2s to perform initialization (Required minima 30ms) */
    while((pObj->IO.GetTick() - tickstart) <= RTL8211_INIT_TO)
    {
    }
    pObj->Is_Initialized = 1;
  }

#if defined(ENABLE_RTL8211F_TXDELAY) && (ENABLE_RTL8211F_TXDELAY==1)
  /* Select page 0xd08 */
  pObj->IO.WriteReg(pObj->DevAddr, RTL8211_PAGSR, 0xd08);
  /* Set PHY Tx Delay */
  pObj->IO.WriteReg(pObj->DevAddr, RTL8211_MIICR1_PD08, RTL8211_MIICR1_TXDLY_ENABLE);
  /* Restore Default Page */
  pObj->IO.WriteReg(pObj->DevAddr, RTL8211_PAGSR, 0);
#endif
#if defined(ENABLE_RTL8211F_RXDELAY) && (ENABLE_RTL8211F_RXDELAY==1)
  /* Select page 0xd08 */
  pObj->IO.WriteReg(pObj->DevAddr, RTL8211_PAGSR, 0xd08);
  /* Set PHY Rx Delay */
  pObj->IO.WriteReg(pObj->DevAddr, RTL8211_MIICR2_PD08, RTL8211_MIICR2_RXDLY_ENABLE);
  /* Restore Default Page */
  pObj->IO.WriteReg(pObj->DevAddr, RTL8211_PAGSR, 0);
#endif

#if defined(DISABLE_RTL8211F_EEE) && (DISABLE_RTL8211F_EEE==1)
  /* Disable EEE (Energy-Efficient Ethernet) feature */
  /* Select page a4b */
  pObj->IO.WriteReg(pObj->DevAddr, RTL8211_PAGSR, 0xa4b);
  pObj->IO.WriteReg(pObj->DevAddr, 0x11, 0x1110);
  /* Select page 0 */
  pObj->IO.WriteReg(pObj->DevAddr, RTL8211_PAGSR, 0);
  /* Set address mode and MMD Device=7 */
  pObj->IO.WriteReg(pObj->DevAddr, RTL8211_MACR, 7);
  /*  Set address value */
  pObj->IO.WriteReg(pObj->DevAddr, RTL8211_MAADR, 0x3c);
  /* Set data mode and MMD Device=7 */
  pObj->IO.WriteReg(pObj->DevAddr, RTL8211_MACR, 0x4007);
  /*  Set data */
  pObj->IO.WriteReg(pObj->DevAddr, RTL8211_MAADR, 0x0);
#endif

  return status;
}

/**
 * @brief  De-Initialize the RTL8211 and it's hardware resources
 * @param  pObj: device object rtl8211_Object_t.
 * @retval None
 */
int32_t RTL8211_DeInit(rtl8211_Object_t *pObj)
{
  if(pObj->Is_Initialized)
  {
    if(pObj->IO.DeInit != 0)
    {
      if(pObj->IO.DeInit() < 0)
      {
        return RTL8211_STATUS_ERROR;
      }
    }

    pObj->Is_Initialized = 0;
  }

  return RTL8211_STATUS_OK;
}

/**
 * @brief  Register IO functions to component object
 * @param  pObj: device object  of RTL8211_Object_t.
 * @param  ioctx: holds device IO functions.
 * @retval RTL8211_STATUS_OK  if OK
 *         RTL8211_STATUS_ERROR if missing mandatory function
 */
int32_t  RTL8211_RegisterBusIO(rtl8211_Object_t *pObj, rtl8211_IOCtx_t *ioctx)
{
  if(!pObj || !ioctx->ReadReg || !ioctx->WriteReg || !ioctx->GetTick)
  {
    return RTL8211_STATUS_ERROR;
  }

  pObj->IO.Init = ioctx->Init;
  pObj->IO.DeInit = ioctx->DeInit;
  pObj->IO.ReadReg = ioctx->ReadReg;
  pObj->IO.WriteReg = ioctx->WriteReg;
  pObj->IO.GetTick = ioctx->GetTick;

  return RTL8211_STATUS_OK;
}

/**
 * @brief  Get the link state of RTL8211 device.
 * @param  pObj: Pointer to device object.
 * @param  pLinkState: Pointer to link state
 * @retval RTL8211_STATUS_LINK_DOWN  if link is down
 *         RTL8211_STATUS_AUTONEGO_NOTDONE if Auto nego not completed
 *         RTL8211_STATUS_1000MBITS_FULLDUPLEX if 1000Mb/s FD
 *         RTL8211_STATUS_1000MBITS_HALFDUPLEX if 1000Mb/s HD
 *         RTL8211_STATUS_100MBITS_FULLDUPLEX if 100Mb/s FD
 *         RTL8211_STATUS_100MBITS_HALFDUPLEX if 100Mb/s HD
 *         RTL8211_STATUS_10MBITS_FULLDUPLEX  if 10Mb/s FD
 *         RTL8211_STATUS_10MBITS_HALFDUPLEX  if 10Mb/s HD
 *         RTL8211_STATUS_READ_ERROR if cannot read register
 *         RTL8211_STATUS_WRITE_ERROR if cannot write to register
 */
int32_t RTL8211_GetLinkState(rtl8211_Object_t *pObj)
{
  uint32_t readval = 0;

  /* Read Status register  */
  if(pObj->IO.ReadReg(pObj->DevAddr, RTL8211_BMSR, &readval) < 0)
  {
    return RTL8211_STATUS_READ_ERROR;
  }

  /* Read Status register again */
  if(pObj->IO.ReadReg(pObj->DevAddr, RTL8211_BMSR, &readval) < 0)
  {
    return RTL8211_STATUS_READ_ERROR;
  }

  if((readval & RTL8211_BMSR_LINK_STATUS) == 0)
  {
    /* Return Link Down status */
    return RTL8211_STATUS_LINK_DOWN;
  }

  /* Check Auto negotiaition */
  if(pObj->IO.ReadReg(pObj->DevAddr, RTL8211_BMCR, &readval) < 0)
  {
    return RTL8211_STATUS_READ_ERROR;
  }

  if((readval & RTL8211_BMCR_AN_EN) != RTL8211_BMCR_AN_EN)
  {
    if(((readval & RTL8211_BMCR_SPEED_SEL_MSB) == RTL8211_BMCR_SPEED_SEL_MSB) && ((readval & RTL8211_BMCR_DUPLEX_MODE) == RTL8211_BMCR_DUPLEX_MODE))
    {
      return RTL8211_STATUS_1000MBITS_FULLDUPLEX;
    }
    else if ((readval & RTL8211_BMCR_SPEED_SEL_MSB) == RTL8211_BMCR_SPEED_SEL_MSB)
    {
      return RTL8211_STATUS_1000MBITS_HALFDUPLEX;
    }
    else if(((readval & RTL8211_BMCR_SPEED_SEL_LSB) == RTL8211_BMCR_SPEED_SEL_LSB) && ((readval & RTL8211_BMCR_DUPLEX_MODE) == RTL8211_BMCR_DUPLEX_MODE))
    {
      return RTL8211_STATUS_100MBITS_FULLDUPLEX;
    }
    else if ((readval & RTL8211_BMCR_SPEED_SEL_LSB) == RTL8211_BMCR_SPEED_SEL_LSB)
    {
      return RTL8211_STATUS_100MBITS_HALFDUPLEX;
    }
    else if ((readval & RTL8211_BMCR_DUPLEX_MODE) == RTL8211_BMCR_DUPLEX_MODE)
    {
      return RTL8211_STATUS_10MBITS_FULLDUPLEX;
    }
    else
    {
      return RTL8211_STATUS_10MBITS_HALFDUPLEX;
    }
  }
  else /* Auto Nego enabled */
  {

    if(pObj->IO.ReadReg(pObj->DevAddr, RTL8211_GBCR, &readval) < 0)
    {
      return RTL8211_STATUS_READ_ERROR;
    }

    if((readval & RTL8211_GBCR_1000BT_FD) == RTL8211_GBCR_1000BT_FD)
    {
      return RTL8211_STATUS_1000MBITS_FULLDUPLEX;
    }


    if(pObj->IO.ReadReg(pObj->DevAddr, RTL8211_ANAR, &readval) < 0)
    {
      return RTL8211_STATUS_READ_ERROR;
    }

    if((readval & RTL8211_ANAR_100BTX_FD) == RTL8211_ANAR_100BTX_FD)
    {
      return RTL8211_STATUS_100MBITS_FULLDUPLEX;
    }
    else if ((readval & RTL8211_ANAR_100BTX_HD) == RTL8211_ANAR_100BTX_HD)
    {
      return RTL8211_STATUS_100MBITS_HALFDUPLEX;
    }
    else if ((readval & RTL8211_ANAR_10BT_FD) == RTL8211_ANAR_10BT_FD)
    {
      return RTL8211_STATUS_10MBITS_FULLDUPLEX;
    }
    else
    {
      return RTL8211_STATUS_10MBITS_HALFDUPLEX;
    }
  }
}

/**
 * @brief  Set the link state of RTL8211 device.
 * @param  pObj: Pointer to device object.
 * @param  pLinkState: link state can be one of the following
 *         RTL8211_STATUS_1000MBITS_FULLDUPLEX if 1000Mb/s FD
 *         RTL8211_STATUS_1000MBITS_HALFDUPLEX if 1000Mb/s HD
 *         RTL8211_STATUS_100MBITS_FULLDUPLEX if 100Mb/s FD
 *         RTL8211_STATUS_100MBITS_HALFDUPLEX if 100Mb/s HD
 *         RTL8211_STATUS_10MBITS_FULLDUPLEX  if 10Mb/s FD
 *         RTL8211_STATUS_10MBITS_HALFDUPLEX  if 10Mb/s HD
 * @retval RTL8211_STATUS_OK  if OK
 *         RTL8211_STATUS_ERROR  if parameter error
 *         RTL8211_STATUS_READ_ERROR if cannot read register
 *         RTL8211_STATUS_WRITE_ERROR if cannot write to register
 */
int32_t RTL8211_SetLinkState(rtl8211_Object_t *pObj, uint32_t LinkState)
{
  uint32_t bcrvalue,gcrvalue,anarvalue = 0;

  int32_t status = RTL8211_STATUS_OK;

  if(pObj->IO.ReadReg(pObj->DevAddr, RTL8211_BMCR, &bcrvalue)  >= 0 &&
      pObj->IO.ReadReg(pObj->DevAddr, RTL8211_GBCR, &gcrvalue)  >= 0 &&
      pObj->IO.ReadReg(pObj->DevAddr, RTL8211_ANAR, &anarvalue) >= 0)
  {
    /* Disable link config (Auto nego, speed and duplex) */
    bcrvalue &= ~(RTL8211_BMCR_AN_EN | RTL8211_BMCR_SPEED_SEL_LSB | RTL8211_BMCR_SPEED_SEL_MSB | RTL8211_BMCR_DUPLEX_MODE);

    /* AN restart dedicated to take into account following modifications */
    bcrvalue |= RTL8211_BMCR_RESTART_AN;

    /* Default ANAR register configurate with 100base-TX full/half duplex support & 10base-TX full/half duplex support*/
    gcrvalue &= ~(RTL8211_GBCR_1000BT_FD);

    if(LinkState == RTL8211_STATUS_1000MBITS_FULLDUPLEX){
      gcrvalue |= (RTL8211_GBCR_1000BT_FD);
    }
    else if(LinkState == RTL8211_STATUS_1000MBITS_HALFDUPLEX){
    }
    else if(LinkState == RTL8211_STATUS_100MBITS_FULLDUPLEX)
    {
      bcrvalue |= (RTL8211_BMCR_SPEED_SEL_LSB | RTL8211_BMCR_DUPLEX_MODE);
    }
    else if (LinkState == RTL8211_STATUS_100MBITS_HALFDUPLEX)
    {
      bcrvalue |= RTL8211_BMCR_SPEED_SEL_LSB;
      anarvalue &= ~(RTL8211_ANAR_100BTX_FD);
    }
    else if (LinkState == RTL8211_STATUS_10MBITS_FULLDUPLEX)
    {
      bcrvalue |= RTL8211_BMCR_DUPLEX_MODE;
      anarvalue &= ~(RTL8211_ANAR_100BTX_FD | RTL8211_ANAR_100BTX_HD);
    }
    else if (LinkState == RTL8211_STATUS_10MBITS_HALFDUPLEX)
    {
      anarvalue &= ~(RTL8211_ANAR_100BTX_FD | RTL8211_ANAR_100BTX_HD | RTL8211_ANAR_10BT_FD);
    }else{
      /* Wrong link status parameter */
      status = RTL8211_STATUS_ERROR;
    }
  }
  else
  {
    status = RTL8211_STATUS_READ_ERROR;
  }

  if(status == RTL8211_STATUS_OK)
  {
    /* Apply configuration */
    if(pObj->IO.WriteReg(pObj->DevAddr, RTL8211_BMCR, bcrvalue) < 0)
    {
      status = RTL8211_STATUS_WRITE_ERROR;
    }
    if(pObj->IO.WriteReg(pObj->DevAddr, RTL8211_GBCR, gcrvalue) < 0)
    {
      status = RTL8211_STATUS_WRITE_ERROR;
    }
    if(pObj->IO.WriteReg(pObj->DevAddr, RTL8211_ANAR, anarvalue) < 0)
    {
      status = RTL8211_STATUS_WRITE_ERROR;
    }
  }

  return status;
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
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
