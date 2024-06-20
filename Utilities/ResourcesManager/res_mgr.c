 /**
  ******************************************************************************
  * @file    res_mgr.c
  * @author  MCD Application Team
  * @brief
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "res_mgr.h"
#if !defined(USE_DEVELOPMENT_MODE)
GPIO_TypeDef *GPIO_base_addrs[] =
{
#if !defined (CORE_CM0PLUS)
  GPIOA,
  GPIOB,
  GPIOC,
  GPIOD,
  GPIOE,
  GPIOF,
  GPIOG,
  GPIOH,
  GPIOI,
  GPIOJ,
  GPIOK,
#endif
  GPIOZ,
};

static ResMgr_Status_t GetBaseAddrRIF_rifsc(ResMgr_data_info_t *rif_info, uint8_t res_num)
{
  uint32_t offset = res_num / 32;
  ResMgr_Status_t  ret =  RESMGR_STATUS_ERROR_NONE;	
  
  /*Check the Parameter*/
  if(!IS_RESMGR_RIFSC_RSC(res_num))
	return RESMGR_STATUS_RES_NUM_ERROR;	

  rif_info->CIDCFGR = (__IO uint32_t *)&(RISC->PER[res_num].CIDCFGR);
  rif_info->SEMCR = (__IO uint32_t *)&RISC->PER[res_num].SEMCR;
  rif_info->priv_base = &RISC->PRIVCFGR[offset];
  rif_info->sec_base = &RISC->SECCFGR[offset];
  rif_info->cid_type = RESMGR_COM_CID_WITH_SEMCR;
  
  return ret;
}
#if !defined (CORE_CM0PLUS)
static ResMgr_Status_t GetBaseAddrRIF_RCC(ResMgr_data_info_t *rif_info, uint8_t res_num)
{
  uint32_t offset = res_num / 32;
  ResMgr_Status_t  ret =  RESMGR_STATUS_ERROR_NONE;	
  /*Check the Parameter*/
  if(!IS_RESMGR_RCC_RSC(res_num))
		return RESMGR_STATUS_RES_NUM_ERROR;

  rif_info->cid_base = (ResMgr_Comp_t *)&RCC->R[res_num];
  rif_info->CIDCFGR = &rif_info->cid_base->CIDCFGR;
  rif_info->SEMCR   = &rif_info->cid_base->SEMCR;
  rif_info->priv_base = &RCC->PRIVCFGR[offset];
  rif_info->sec_base = &RCC->SECCFGR[offset];
  rif_info->cid_type = RESMGR_COM_CID_WITH_SEMCR;
    
  return ret;
}
#else
static ResMgr_Status_t GetBaseAddrRIF_RCC(ResMgr_data_info_t *rif_info, uint8_t res_num)
{
  return RESMGR_STATUS_ACCESS_ERROR;
}
#endif
static ResMgr_Status_t GetBaseAddrRIF_GPIO(ResMgr_data_info_t *rif_info, uint8_t res_num)
{
  GPIO_TypeDef *GPIO_base = GPIO_base_addrs[rif_info->res_type - RESMGR_RESOURCE_RIF_GPIOA];
  ResMgr_Status_t  ret =  RESMGR_STATUS_ERROR_NONE;	
  /*Check the Parameter*/
  if(!IS_RESMGR_GPIO_RSC(res_num))
		return RESMGR_STATUS_RES_NUM_ERROR;	

  rif_info->CIDCFGR = (__IO uint32_t *)(&GPIO_base->CIDCFGR0 + res_num * 2);
  rif_info->SEMCR = (__IO uint32_t *)(&GPIO_base->SEMCR0 + res_num * 2);
  rif_info->priv_base = &GPIO_base->PRIVCFGR;
  rif_info->sec_base = &GPIO_base->SECCFGR;
  rif_info->cid_type = RESMGR_COM_CID_WITH_SEMCR;
  
  return ret;
}
static ResMgr_Status_t GetBaseAddrRIF_PWR_WIO(ResMgr_data_info_t *rif_info, uint8_t res_num)
{
  ResMgr_Status_t  ret =  RESMGR_STATUS_ERROR_NONE;	  	
  /*Check the Parameter*/
  if(!IS_RESMGR_PWR_RSC_WIO(res_num))
		return RESMGR_STATUS_RES_NUM_ERROR;	

  rif_info->cid_base = (ResMgr_Comp_t *)(&PWR->WIO1CIDCFGR + res_num * 2);
  rif_info->CIDCFGR = &rif_info->cid_base->CIDCFGR;
  rif_info->SEMCR   = &rif_info->cid_base->SEMCR;
  rif_info->priv_base = &PWR->WIOPRIVCFGR;
  rif_info->sec_base = &PWR->WIOSECCFGR;
  rif_info->cid_type = RESMGR_COM_CID_WITH_SEMCR;
  
  return ret;
}
#if !defined (CORE_CM0PLUS)
static ResMgr_Status_t GetBaseAddrRIF_FMC(ResMgr_data_info_t *rif_info, uint8_t res_num)
{
  ResMgr_Status_t  ret =  RESMGR_STATUS_ERROR_NONE;		
  /*Check the Parameter*/
  if(!IS_RESMGR_FMC_RSC(res_num))
		return RESMGR_STATUS_RES_NUM_ERROR;

  rif_info->cid_base = (ResMgr_Comp_t *)(&FMC_Common_R->CIDCFGR0 + (res_num * 2));
  rif_info->CIDCFGR = &rif_info->cid_base->CIDCFGR;
  rif_info->SEMCR   = &rif_info->cid_base->SEMCR;
  rif_info->priv_base = &FMC_Common_R->PRIVCFGR;
  rif_info->sec_base = &FMC_Common_R->SECCFGR;
  rif_info->cid_type = RESMGR_COM_CID_WITH_SEMCR;
  
  return ret;
}
#else
static ResMgr_Status_t GetBaseAddrRIF_FMC(ResMgr_data_info_t *rif_info, uint8_t res_num)
{
  return RESMGR_STATUS_ACCESS_ERROR;
}
#endif
static ResMgr_Status_t GetBaseAddrRIF_PWR(ResMgr_data_info_t *rif_info, uint8_t res_num)
{
  ResMgr_Status_t  ret =  RESMGR_STATUS_ERROR_NONE;	
  /*Check the Parameter*/
  if(!IS_RESMGR_PWR_RSC(res_num))
		return RESMGR_STATUS_RES_NUM_ERROR;

  rif_info->cid_base = (ResMgr_Comp_t *)&PWR->RxCIDCFGR[res_num];
  rif_info->CIDCFGR = &rif_info->cid_base->CIDCFGR;
  rif_info->priv_base = &PWR->RPRIVCFGR;
  rif_info->sec_base = &PWR->RSECCFGR;
  rif_info->cid_type = RESMGR_COMP_CID_ONLY;
  
  return ret;
}
static ResMgr_Status_t GetBaseAddrRIF_RTC(ResMgr_data_info_t *rif_info, uint8_t res_num)
{
  ResMgr_Status_t  ret =  RESMGR_STATUS_ERROR_NONE;	
  /*Check the Parameter*/
  if(!IS_RESMGR_RTC_RSC(res_num))
		return RESMGR_STATUS_RES_NUM_ERROR;

  rif_info->cid_base = (ResMgr_Comp_t *)&RTC->RCIDCFGR[res_num];
  rif_info->CIDCFGR = &rif_info->cid_base->CIDCFGR;
  rif_info->priv_base = &RTC->PRIVCR;
  rif_info->sec_base = &RTC->SECCFGR;
  rif_info->cid_type = RESMGR_COMP_CID_ONLY;
  
  return ret;
}
#if !defined (CORE_CM0PLUS)
static ResMgr_Status_t GetBaseAddrRIF_EXTI(ResMgr_data_info_t *rif_info, uint8_t res_num)
{
	
  EXTI_TypeDef *EXTI_base;
  ResMgr_Status_t  ret =  RESMGR_STATUS_ERROR_NONE;	  
  if (rif_info->res_type == RESMGR_RESOURCE_RIF_EXTI1)
  {
    /*Check the Parameter*/
    if(!IS_RESMGR_EXTI1_RSC(res_num))
		 return RESMGR_STATUS_RES_NUM_ERROR;

    EXTI_base = EXTI1;
  }
  else {

	/*Check the Parameter*/
	if(!IS_RESMGR_EXTI2_RSC(res_num))
		 return RESMGR_STATUS_RES_NUM_ERROR;	

	EXTI_base = EXTI2;
  }
  rif_info->cid_base = (ResMgr_Comp_t *)&EXTI_base->ECIDCFGR[res_num];
  rif_info->CIDCFGR = &rif_info->cid_base->CIDCFGR;
  rif_info->priv_base = &EXTI_base->PRIVCFGR1;
  rif_info->sec_base = &EXTI_base->SECCFGR1;
  rif_info->cid_type = RESMGR_COMP_CID_ONLY;
  
  return ret;
}
#else
static ResMgr_Status_t GetBaseAddrRIF_EXTI(ResMgr_data_info_t *rif_info, uint8_t res_num)
{
  return RESMGR_STATUS_ACCESS_ERROR;
}
#endif
#if !defined (CORE_CM0PLUS)
static ResMgr_Status_t GetBaseAddrRIF_DMA(ResMgr_data_info_t *rif_info, uint8_t res_num)
{
  DMA_TypeDef *DMA_base;
  uint32_t offset = 0x050 + 0x80 * res_num;
  DMA_Channel_TypeDef *DMA_channel_base;
  ResMgr_Status_t  ret =  RESMGR_STATUS_ERROR_NONE;	  
  /*Check the Parameter*/
  if(!IS_RESMGR_HPDMA_RSC(res_num))
		return RESMGR_STATUS_RES_NUM_ERROR;		
   

  if (rif_info->res_type == RESMGR_RESOURCE_RIF_HPDMA1)
  {
    DMA_base = HPDMA1;
  }
  else if (rif_info->res_type == RESMGR_RESOURCE_RIF_HPDMA2)
  {
    DMA_base = HPDMA2;
  }
  else
    DMA_base = HPDMA3;
  DMA_channel_base = (DMA_Channel_TypeDef *)((uint32_t)DMA_base + offset);
  rif_info->CIDCFGR = (__IO uint32_t *)&DMA_channel_base->CCIDCFGR;
  rif_info->SEMCR = (__IO uint32_t *)&DMA_channel_base->CSEMCR;  rif_info->priv_base = &DMA_base->PRIVCFGR;
  rif_info->sec_base = &DMA_base->SECCFGR;
  rif_info->cid_type = RESMGR_COM_CID_WITH_SEMCR;
  
  return ret;
}
#else
static ResMgr_Status_t GetBaseAddrRIF_DMA(ResMgr_data_info_t *rif_info, uint8_t res_num)
{
  return RESMGR_STATUS_ACCESS_ERROR;
}
#endif
static ResMgr_Status_t GetBaseAddrRIF(ResMgr_data_info_t *rif_info, uint8_t res_num)
{
  ResMgr_Status_t  ret =  RESMGR_STATUS_ERROR_NONE;	   
  
  switch (rif_info->res_type)
  {
    case RESMGR_RESOURCE_RIFSC:
      ret = GetBaseAddrRIF_rifsc(rif_info, res_num);
      break;
    case RESMGR_RESOURCE_RIF_PWR_WIO:
      ret = GetBaseAddrRIF_PWR_WIO(rif_info, res_num);
      break;
    case RESMGR_RESOURCE_RIF_RCC:
      ret = GetBaseAddrRIF_RCC(rif_info, res_num);
      break;
    case RESMGR_RESOURCE_RIF_GPIOA:
    case RESMGR_RESOURCE_RIF_GPIOB:
    case RESMGR_RESOURCE_RIF_GPIOC:
    case RESMGR_RESOURCE_RIF_GPIOD:
    case RESMGR_RESOURCE_RIF_GPIOE:
    case RESMGR_RESOURCE_RIF_GPIOF:
    case RESMGR_RESOURCE_RIF_GPIOG:
    case RESMGR_RESOURCE_RIF_GPIOH:
    case RESMGR_RESOURCE_RIF_GPIOI:
    case RESMGR_RESOURCE_RIF_GPIOJ:
    case RESMGR_RESOURCE_RIF_GPIOK:
    case RESMGR_RESOURCE_RIF_GPIOZ:
      ret = GetBaseAddrRIF_GPIO(rif_info, res_num);
      break;
    case RESMGR_RESOURCE_RIF_FMC:
      ret = GetBaseAddrRIF_FMC(rif_info, res_num);
      break;
    case RESMGR_RESOURCE_RIF_PWR:
      ret = GetBaseAddrRIF_PWR(rif_info, res_num);
      break;
    case RESMGR_RESOURCE_RIF_RTC:
      ret = GetBaseAddrRIF_RTC(rif_info, res_num);
      break;
    case RESMGR_RESOURCE_RIF_EXTI1:
    case RESMGR_RESOURCE_RIF_EXTI2:
      ret = GetBaseAddrRIF_EXTI(rif_info, res_num);
      break;
    case RESMGR_RESOURCE_RIF_HPDMA1:
    case RESMGR_RESOURCE_RIF_HPDMA2:
    case RESMGR_RESOURCE_RIF_HPDMA3:
      ret = GetBaseAddrRIF_DMA(rif_info, res_num);
      break;
    default:
      ret = RESMGR_STATUS_RES_TYP_ERROR;
      break;
  }
  
  return ret;
}

uint8_t SecureState;
uint32_t GetCoreMode(void)
{
#if defined (CORE_CM33)
#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
  SecureState = CPU_SECURE;
#else
  SecureState = CPU_UNSECURE;
#endif
  if((__get_CONTROL() & 0x01) == THREAD_MODE_PRIVILEGED)
        return THREAD_MODE_PRIVILEGED;
  else
        return THREAD_MODE_UNPRIVILEGED;
#else
  return 0;
#endif
}
static uint32_t ReadBit(volatile uint32_t *reg_address, uint32_t mask)
{
  volatile uint32_t tmp;

  /* read all bits to Mask */
  tmp = READ_BIT(*reg_address, mask);
  return tmp;
}
static uint32_t ResMgr_GetPrivStatus(ResMgr_data_info_t *res_data, uint8_t res_num)
{
  uint32_t pos;

  if(res_data->res_type == RESMGR_RESOURCE_RIF_RTC)
  {
	  pos = RESMGR_RTC_PROT_BITS_POS(res_num);
  } else
  {
	  pos = res_num % 32;
  }

  return (ReadBit(res_data->priv_base, 1 << pos) != 0);
}
static uint32_t ResMgr_GetSecStatus(ResMgr_data_info_t *res_data, uint8_t res_num)
{
  uint32_t pos;

  if(res_data->res_type == RESMGR_RESOURCE_RIF_RTC)
  {
	  pos = RESMGR_RTC_PROT_BITS_POS(res_num);
  } else
  {
	  pos = res_num % 32;
  }

  return (ReadBit(res_data->sec_base, 1 << pos) != 0);
}
static uint32_t ResMgr_Is_Sem_Avail(ResMgr_data_info_t *resmgr_data)
{

  return !(*(resmgr_data->SEMCR) & RESMGR_SEMCR_SEMMUTEX);
}

static ResMgr_Sem_Status_t ResMgr_Get_SemTake(ResMgr_data_info_t *resmgr_data)
{
  uint32_t scid_mask = RESMGR_CIDCFGR_SCID;
  ResMgr_Sem_Status_t status = RESMGR_SEM_STATUS_ERROR;

  /* Check that cortex M has the semaphore */
  if (ResMgr_Is_Sem_Avail(resmgr_data))
    SET_BIT(*(resmgr_data->SEMCR), RESMGR_SEMCR_SEMMUTEX);


  if ((*(resmgr_data->SEMCR) & scid_mask) == RESMGR_SEMCR_SEMCID2)
    status = RESMGR_SEM_STATUS_TAKEN;
  else
	status = RESMGR_SEM_STATUS_ERROR;

  return status;;
}
static ResMgr_Sem_Status_t ResMgr_SemRel(ResMgr_data_info_t *resmgr_data)
{
  uint32_t scid_mask = RESMGR_CIDCFGR_SCID;

  if (ResMgr_Is_Sem_Avail(resmgr_data))
  {
    return RESMGR_SEM_STATUS_RELEASED;
  }

  CLEAR_BIT(*(resmgr_data->SEMCR), RESMGR_SEMCR_SEMMUTEX);

  /* Ok if another compartment takes the semaphore before the check */
  if (!ResMgr_Is_Sem_Avail(resmgr_data) &&
      (*(resmgr_data->SEMCR) & scid_mask) == RESMGR_SEMCR_SEMCID2)
  {
    return RESMGR_SEM_STATUS_ERROR;
  }

  return RESMGR_SEM_STATUS_RELEASED;
}

static uint32_t ResMgr_Get_CidFiltering(ResMgr_data_info_t *resmgr_data)
{
  return ReadBit(resmgr_data->CIDCFGR, RESMGR_CIDCFGR_CFEN);
}
static uint32_t ResMgr_Get_SemEnable(ResMgr_data_info_t *resmgr_data)
{
  return ReadBit(resmgr_data->CIDCFGR, RISC_PERCIDCFGR_SEMEN);

}
static uint32_t ResMgr_Get_StaticCid(ResMgr_data_info_t *resmgr_data, uint32_t cid)
{
  uint32_t scid = ReadBit(resmgr_data->CIDCFGR, RESMGR_CIDCFGR_SCID);
  return (scid == (cid) ? 1 : 0);

}
static uint32_t ResMgr_Get_SemCidWlist(ResMgr_data_info_t *resmgr_data, uint32_t cid_wl)
{
  return ReadBit(resmgr_data->CIDCFGR, cid_wl);
}

/**
  * @brief  < Add here the function description >
  * @note   < OPTIONAL: add here global note >
  * @param  None
  * @retval None
  */
ResMgr_Status_t ResMgr_Request(ResMgr_Res_Type_t res_type, uint8_t res_num)
{
  ResMgr_Status_t status;
  ResMgr_data_info_t resmgr_data_info;
  ResMgr_data_info_t *resmgr_data = &resmgr_data_info;
  int cpu_mode = GetCoreMode();

  resmgr_data_info.res_type = res_type;

  status =  GetBaseAddrRIF(&resmgr_data_info, res_num);
  if(status != RESMGR_STATUS_ERROR_NONE)
  {
    printf("[ERROR]: %s: Invalid RIF Base Address - Res num = %d\n\r", __func__, res_num);
	  return status;
  }	  

#ifdef STM32MP25XX_SI_CUT1_X
  if (res_type == RESMGR_RESOURCE_RIF_EXTI1 || res_type == RESMGR_RESOURCE_RIF_EXTI2)
  {
    return (RESMGR_STATUS_ACCESS_ERROR);
  }
#endif
  if (ResMgr_GetPrivStatus(resmgr_data, res_num) == PRIVILEGED && cpu_mode == THREAD_MODE_UNPRIVILEGED)
  {
    return RESMGR_STATUS_NPRIV_ACCESS_ERROR;
  }
  if (ResMgr_GetSecStatus(resmgr_data, res_num) == CPU_SECURE && SecureState == CPU_UNSECURE)
  {
    return RESMGR_STATUS_NSEC_ACCESS_ERROR;
  }
  /* Is CID filtering activated ? */
  if (ResMgr_Get_CidFiltering(resmgr_data) != 0)
  {
    /* Is there a semaphore (sharing) activation            */
    /* in other words is Peripheral shared with semaphore ? */
    if ((resmgr_data->cid_type == RESMGR_COMP_CID_ONLY) || ResMgr_Get_SemEnable(resmgr_data) == 0)
    {
      /* Is SCID == CPU2 ? */
      if (ResMgr_Get_StaticCid(resmgr_data, RESMGR_CIDCFGR_SCID2))
      {
        /* SCID == CPU1 : configuration is correct : */
        /* return an OK access response to caller    */
        return (RESMGR_STATUS_ACCESS_OK);
      }
      else
      {
        /* SCID != CPU2 : configuration is not correct : */
        /* return a KO access response to caller         */
        printf("[ERROR]: %s: SCID != CPU2 - Res num = %d\n\r", __func__, res_num);
        return (RESMGR_STATUS_SCID_ACCESS_ERROR);
      }
    } /* of if(semEn == 0) */
    else if(resmgr_data->cid_type == RESMGR_COM_CID_WITH_SEMCR)
    {
      /* Peripheral Sharing with semaphore is activated */
      /* Is CPU2 part of programmed semaphore white list ? */
      if (((ResMgr_Get_SemCidWlist(resmgr_data, RESMGR_CIDCFGR_SEMWLC2) == 0)))
      {
        /* CPU2 is not in white list       */
        printf("[ERROR]: %s: CPU2 is not in SEM white list - Res num = %d\n\r", __func__, res_num);
        /* return a KO access response to caller */
        return (RESMGR_STATUS_CID_WL_ACCESS_ERROR);
      }
      else
      {
        /* CPU2 is in white list ... */
        if (ResMgr_Get_SemTake(resmgr_data) == RESMGR_SEM_STATUS_ERROR)
        {
          /* return(ResMgr_Get_SemTake(res_num, RIF_CID_MPU, ISOLATION_SEM_WAIT_TIMEOUT_VAL)); */
          printf("[ERROR]: %s: SEM for CPU2 is already taken - Res num = %d\n\r", __func__, res_num);
          return RESMGR_STATUS_SEM_ACCESS_ERROR;
        }
      } /* of else CPU1 is in RISUP white list */
    } /* of else Peripheral Sharing with semaphore is activated */

  } /* of if(ResMgr_Get_CidFiltering(res_num) != 0) */
  return (RESMGR_STATUS_ACCESS_OK);
}
/**
  * @brief  < Add here the function description >
  * @note   < OPTIONAL: add here global note >
  * @param  None
  * @retval None
  */
ResMgr_Status_t ResMgr_Release(ResMgr_Res_Type_t res_type, uint8_t res_num)
{
  ResMgr_Status_t status;
  ResMgr_data_info_t resmgr_data_info;
  ResMgr_data_info_t *resmgr_data = &resmgr_data_info;
  int cpu_mode = GetCoreMode();


#ifdef STM32MP25XX_SI_CUT1_X
  if (res_type == RESMGR_RESOURCE_RIF_EXTI1 || res_type == RESMGR_RESOURCE_RIF_EXTI2)
  {
    return (RESMGR_STATUS_ACCESS_ERROR);
  }
#endif

  resmgr_data_info.res_type = res_type;

  status =  GetBaseAddrRIF(&resmgr_data_info, res_num);
  if(status != RESMGR_STATUS_ERROR_NONE)
  {
	  return status;
  }	
  
  if (ResMgr_GetPrivStatus(resmgr_data, res_num) == PRIVILEGED && cpu_mode == THREAD_MODE_UNPRIVILEGED)
  {
    return RESMGR_STATUS_NPRIV_ACCESS_ERROR;
  }
  if (ResMgr_GetSecStatus(resmgr_data, res_num) == CPU_SECURE && SecureState == CPU_UNSECURE)
  {
    return RESMGR_STATUS_NSEC_ACCESS_ERROR;
  }
  /* Is CID filtering activated ? */
  if (ResMgr_Get_CidFiltering(resmgr_data) != 0)
  {
    /* CID filtering is activated in RISUP */
    /* Is there a semaphore (sharing) activation            */
    /* in other words is Peripheral shared with semaphore ? */
    if ((resmgr_data->cid_type == RESMGR_COMP_CID_ONLY) || ResMgr_Get_SemEnable(resmgr_data) == 0)
    {
      /* There is no Semaphore sharing so compartment filtering */
      /* is used with a Static CID (SCID)                       */
      /* Is SCID == CPU2 ? */
      if (ResMgr_Get_StaticCid(resmgr_data, RESMGR_CIDCFGR_SCID2))
      {
        /* SCID == CPU2 : configuration is correct : */
        /* return an OK access response to caller    */
        /* cut1 correction return(RESMGR_STATUS_OK); */
        return (RESMGR_STATUS_ACCESS_OK);
      }
      else
      {
        /* SCID != CPU2 : configuration is not correct : */
        /* return a KO access response to caller         */
        printf("[ERROR]: %s: SCID != CPU2 - Res num = %d\n\r", __func__, res_num);
        return (RESMGR_STATUS_SCID_ACCESS_ERROR);
      }
    } /* of if(semEn == 0) */
    else if(resmgr_data->cid_type == RESMGR_COM_CID_WITH_SEMCR)
    {
      /* Peripheral Sharing with semaphore is activated */
      /* Is CPU2 part of programmed RISUP semaphore white list ? */
      if (((ResMgr_Get_SemCidWlist(resmgr_data, RESMGR_CIDCFGR_SEMWLC2) == 0)))
      {
        /* CPU2 is not in RISUP white list       */
        printf("[ERROR]: %s: CPU2 is not in SEM white list - Res num = %d\n\r", __func__, res_num);
        /* return a KO access response to caller */
        return (RESMGR_STATUS_CID_WL_ACCESS_ERROR);
      }
      else
      {
        /* CPU2 is in RISUP white list ... */
        if (ResMgr_SemRel(resmgr_data) == RESMGR_SEM_STATUS_ERROR)
        {
          /* return(ResMgr_Get_SemTake(res_num, RIF_CID_MPU, ISOLATION_SEM_WAIT_TIMEOUT_VAL)); */
          printf("[ERROR]: %s: SEM for CPU2 is already taken - Res num = %d\n\r", __func__, res_num);
          return RESMGR_STATUS_SEM_ACCESS_ERROR;
        }
      } /* of else CPU2 is in RISUP white list */
    } /* of else Peripheral Sharing with semaphore is activated */

  }
  return (RESMGR_STATUS_ACCESS_OK);
}
#else
/**
  * @brief  < Add here the function description >
  * @note   < OPTIONAL: add here global note >
  * @param  None
  * @retval None
  */
ResMgr_Status_t ResMgr_Request(ResMgr_Res_Type_t res_type, uint8_t res_num)
{
	UNUSED(res_type);
	UNUSED(res_num);

	return RESMGR_STATUS_ACCESS_OK;
}
/**
  * @brief  < Add here the function description >
  * @note   < OPTIONAL: add here global note >
  * @param  None
  * @retval None
  */
ResMgr_Status_t ResMgr_Release(ResMgr_Res_Type_t res_type, uint8_t res_num)
{
	UNUSED(res_type);
	UNUSED(res_num);

	return RESMGR_STATUS_ACCESS_OK;
}
#endif
