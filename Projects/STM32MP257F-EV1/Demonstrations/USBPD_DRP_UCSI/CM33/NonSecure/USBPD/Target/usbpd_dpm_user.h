/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usbpd_dpm_user.h
  * @author  MCD Application Team
  * @brief   Header file for usbpd_dpm_user.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

#ifndef __USBPD_DPM_USER_H_
#define __USBPD_DPM_USER_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* USER CODE BEGIN Include */

/* USER CODE END Include */

/** @addtogroup STM32_USBPD_APPLICATION
  * @{
  */

/** @addtogroup STM32_USBPD_APPLICATION_DPM_USER
  * @{
  */

/* Exported typedef ----------------------------------------------------------*/
typedef struct
{
  uint32_t PE_DataSwap                                    : 1U;  /*!< support data swap                                     */
  uint32_t PE_VconnSwap                                   : 1U;  /*!< support VCONN swap                                    */
  uint32_t PE_DR_Swap_To_DFP                              : 1U;  /*!< If supported, DR Swap to DFP can be accepted or not by the user else directly rejected */
  uint32_t PE_DR_Swap_To_UFP                              : 1U;  /*!< If supported, DR Swap to UFP can be accepted or not by the user else directly rejected */
  uint32_t Reserved1                                      :28U;  /*!< Reserved bits */
  USBPD_SNKPowerRequest_TypeDef DPM_SNKRequestedPower;          /*!< Requested Power by the sink board                     */
#if _MANU_INFO
  USBPD_MIDB_TypeDef  DPM_ManuInfoPort;                         /*!< Manufacturer information used for the port            */
  uint16_t            ReservedManu;                             /*!< Reserved bits to match with Manufacturer information            */
#else
  uint32_t            ReservedManu[7];                          /*!< Reserved bits to match with Manufacturer information            */
#endif /* _MANU_INFO */
  USBPD_SCEDB_TypeDef DPM_SRCExtendedCapa;                      /*!< SRC Extended Capability                               */
  USBPD_SKEDB_TypeDef DPM_SNKExtendedCapa;                      /*!< SNK Extended Capability                                */
  uint8_t             ReservedSnkCapa[3];                       /*!< Reserved bits to match with SnkCapaExt information     */
  uint32_t PWR_AccessoryDetection                         : 1U; /*!< It enables or disables powered accessory detection */
  uint32_t PWR_AccessoryTransition                        : 1U; /*!< It enables or disables transition from Powered.accessory to Try.SNK */
  USBPD_CORE_PDO_ExtPowered_TypeDef PWR_UnconstrainedPower: 1U; /*!< UUT has an external power source available that is sufficient to adequately power the system while charging external devices or the UUT primary function is to charge external devices. */
  CAD_SNK_Source_Current_Adv_Typedef PWR_RpResistorValue  : 2U; /*!< RP resistor value based on @ref CAD_SNK_Source_Current_Adv_Typedef */
  USBPD_CORE_PDO_USBCommCapable_TypeDef USB_Support       : 1U; /*!< USB_Comms_Capable, is the UUT capable of enumerating as a USB host or device? */
  uint32_t USB_Device                                     : 1U; /*!< Type_C_Can_Act_As_Device, Indicates whether the UUT can communicate with USB 2.0 or USB 3.1 as a device or as the Upstream Facing Port of a hub. */
  uint32_t USB_Host                                       : 1U; /*!<  Type_C_Can_Act_As_Host, Indicates whether the UUT can communicate with USB 2.0 or USB 3.1 as a host or as the Downstream Facing Port of a hub */
  USBPD_CORE_PDO_USBSuspendSupport_TypeDef USB_SuspendSupport: 1U; /*!<  USB Suspend support values in PDO definition (Source) */
  uint32_t CAD_tDRP                                       :30U;  /*!< The period that DRP shall complete a Source to Sink and back advertisement */
  uint32_t CAD_dcSRC_DRP                                  :30U;  /*!< The percent of time that a DRP shall advertise Source during tDRP (in %) */
  uint32_t Reserved2                                      :31U;  /*!< reserved bits */
} USBPD_USER_SettingsTypeDef;

typedef struct
{
  uint32_t XID;               /*!< Value provided by the USB-IF assigned to the product   */
  uint16_t VID;               /*!< Vendor ID (assigned by the USB-IF)                     */
  uint16_t PID;               /*!< Product ID (assigned by the manufacturer)              */
} USBPD_IdSettingsTypeDef;

/** @brief  USBPD PDO Selection method enum definition
  *
  */
typedef enum
{
  PDO_SEL_METHOD_MAX_PWR,
  PDO_SEL_METHOD_MIN_PWR,
  PDO_SEL_METHOD_MAX_VOLT,
  PDO_SEL_METHOD_MIN_VOLT,
  PDO_SEL_METHOD_MAX_CUR,
  PDO_SEL_METHOD_MIN_CUR
} USBPD_DPM_PDO_SelectionMethodTypeDef;

typedef enum {
  DPM_USER_EVENT_TIMER,         /* TIMER EVENT */
  DPM_USER_EVENT_GUI,           /* GUI EVENT */
  DPM_USER_EVENT_NONE,          /* NO EVENT */
} DPM_USER_EVENT;
/* USER CODE BEGIN Typedef */
/**
  * @brief  USBPD DPM handle Structure definition
  * @{
  */
typedef struct
{
  uint32_t                      DPM_ListOfRcvSRCPDO[USBPD_MAX_NB_PDO];   /*!< The list of received Source Power Data Objects from Port partner
                                                                              (when Port partner is a Source or a DRP port).                       */
  uint32_t                      DPM_NumberOfRcvSRCPDO;                   /*!< The number of received Source Power Data Objects from port Partner
                                                                              (when Port partner is a Source or a DRP port).
                                                                              This parameter must be set to a value lower than USBPD_MAX_NB_PDO    */
  uint32_t                      DPM_ListOfRcvSNKPDO[USBPD_MAX_NB_PDO];   /*!< The list of received Sink Power Data Objects from Port partner
                                                                              (when Port partner is a Sink or a DRP port).                         */
  uint32_t                      DPM_NumberOfRcvSNKPDO;                   /*!< The number of received Sink Power Data Objects from port Partner
                                                                              (when Port partner is a Sink or a DRP port).
                                                                              This parameter must be set to a value lower than USBPD_MAX_NB_PDO    */
  uint32_t                      DPM_RDOPosition;                         /*!< RDO Position of requested DO in Source list of capabilities          */
  uint32_t                      DPM_RequestedVoltage;                    /*!< Value of requested voltage                                           */
  uint32_t                      DPM_RequestedCurrent;                    /*!< Value of requested current                                           */
#if defined(USBPD_REV30_SUPPORT)
  int16_t                       DPM_MeasuredCurrent;                     /*!< Value of measured current                                            */
#endif /* USBPD_REV30_SUPPORT */
  uint32_t                      DPM_RDOPositionPrevious;                 /*!< RDO Position of previous requested DO in Source list of capabilities */
  uint32_t                      DPM_RequestDOMsg;                        /*!< Request Power Data Object message to be sent                         */
  uint32_t                      DPM_RequestDOMsgPrevious;                /*!< Previous Request Power Data Object message to be sent                */
  uint32_t                      DPM_RcvRequestDOMsg;                     /*!< Received request Power Data Object message from the port Partner     */
  volatile uint32_t             DPM_ErrorCode;                           /*!< USB PD Error code                                                    */
  volatile uint8_t              DPM_IsConnected;                         /*!< USB PD connection state                                              */
#if defined(_VCONN_SUPPORT)
  uint16_t                      DPM_CablePDCapable:1;                    /*!< Flag to keep information that Cable may be PD capable                */
  uint16_t                      DPM_CableResetOnGoing:1;                 /*!< Flag to manage a cable reset on going                                */
  uint16_t                      DPM_Reserved:14;                         /*!< Reserved bytes                                                       */
#else
  uint16_t                      DPM_Reserved:16;                         /*!< Reserved bytes                                                       */
#endif /* _VCONN_SUPPORT */
  USBPD_PPSSDB_TypeDef          DPM_RcvPPSStatus;                        /*!< PPS Status received by port partner                                  */
#if defined(USBPD_REV30_SUPPORT)

  uint8_t                       FlagSendGetSrcCapaExtended;
  volatile uint16_t             DPM_TimerSRCExtendedCapa;                /*!< timer to request the extended capa                                   */
  USBPD_SKEDB_TypeDef           DPM_RcvSNKExtendedCapa;                  /*!< SNK Extended Capability received by port partner                     */
#if _STATUS
  USBPD_SDB_TypeDef             DPM_RcvStatus;                           /*!< Status received by port partner                                      */
#endif /* _STATUS */
#if _SRC_CAPA_EXT
  USBPD_SCEDB_TypeDef           DPM_RcvSRCExtendedCapa;                  /*!< SRC Extended Capability received by port partner                     */
#endif /* _SRC_CAPA_EXT */
#if _MANU_INFO
  USBPD_GMIDB_TypeDef           DPM_GetManufacturerInfo;                 /*!< Get Manufacturer Info                                                */
#endif /* _MANU_INFO */
#if _BATTERY
  USBPD_GBSDB_TypeDef           DPM_GetBatteryStatus;                    /*!< Get Battery status                                                   */
  USBPD_GBCDB_TypeDef           DPM_GetBatteryCapability;                /*!< Get Battery Capability                                               */
  USBPD_BSDO_TypeDef            DPM_BatteryStatus;                       /*!< Battery status                                                       */
#endif /* _BATTERY */
#if _ALERT
  volatile uint16_t             DPM_TimerAlert;                          /*!< Timer used to monitor current and trig an ALERT                      */
  USBPD_ADO_TypeDef             DPM_SendAlert;                           /*!< Save the Alert sent to port partner                                  */
  USBPD_ADO_TypeDef             DPM_RcvAlert;                            /*!< Save the Alert received by port partner                              */
#endif /* _ALERT */
#endif /* USBPD_REV30_SUPPORT */
#ifdef _VCONN_SUPPORT
  USBPD_DiscoveryIdentity_TypeDef VDM_DiscoCableIdentify;                /*!< VDM Cable Discovery Identify                                         */
#endif /* _VCONN_SUPPORT */
#ifdef _VDM
  USBPD_DiscoveryIdentity_TypeDef   VDM_DiscoIdentify;                   /*!< VDM Discovery Identify                                               */
  USBPD_SVIDPortPartnerInfo_TypeDef VDM_SVIDPortPartner;                 /*!< VDM SVID list                                                        */
  USBPD_ModeInfo_TypeDef            VDM_ModesPortPartner;                /*!< VDM Modes list                                                       */
#endif /* _VDM */
#if defined(_FWUPDATE_RESPONDER)
  uint16_t                      DPM_PDFU_Data_NextBlockIndex;            /*!< Next PDFU Data Block Index                                           */
  uint16_t                      DPM_PDFU_Data_ReceivedBlockIndex;        /*!< Latest Received PDFU Data Block Index                                */
  uint16_t                      DPM_PDFU_Data_Size;                      /*!< Latest PDFU Data Size                                                */
  uint8_t                       DPM_PDFU_WaitTime;                       /*!< Value to keep the latest waittime sent to initiator                  */
#endif /* _FWUPDATE_RESPONDER */
} USBPD_HandleTypeDef;
/* USER CODE END Typedef */

/* Exported define -----------------------------------------------------------*/
/* USER CODE BEGIN Define */

/* USER CODE END Define */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN Constant */

/* USER CODE END Constant */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN Macro */

/* USER CODE END Macro */

/* Exported variables --------------------------------------------------------*/
/* USER CODE BEGIN Private_Variables */

#if !defined(USBPD_DPM_USER_C)
extern USBPD_HandleTypeDef DPM_Ports[USBPD_PORT_COUNT];
#else
USBPD_HandleTypeDef DPM_Ports[USBPD_PORT_COUNT] =
{
  {
    .DPM_Reserved = 0,
#if defined(USBPD_REV30_SUPPORT)
#if defined(USBPDCORE_SNK_CAPA_EXT)
    .DPM_RcvSNKExtendedCapa = {0},                  /*!< SNK Extended Capability received by port partner                     */
#endif /* USBPDCORE_SNK_CAPA_EXT */
#if _SRC_CAPA_EXT
    .FlagSendGetSrcCapaExtended = 0,
    .DPM_TimerSRCExtendedCapa = 0,                  /*!< timer to request the extended capa                                   */
    .DPM_RcvSRCExtendedCapa = {0},                  /*!< SRC Extended Capability received by port partner                     */
#endif /* _SRC_CAPA_EXT */
#if defined(_SNK)||defined(_DRP)
    .DPM_RcvSNKExtendedCapa = {0},                  /*!< SNK Extended Capability received by port partner                     */
#endif /* _DRP || _SNK */
#if _MANU_INFO
    .DPM_GetManufacturerInfo = {0},                 /*!< Get Manufacturer Info                                                */
#endif /* _MANU_INFO */
#if _BATTERY
    .DPM_GetBatteryStatus = {0},                    /*!< Get Battery status                                                   */
    .DPM_GetBatteryCapability = {0},                /*!< Get Battery Capability                                               */
    .DPM_BatteryStatus = {0},                       /*!< Battery status                                                       */
#endif /* _BATTERY */
#if _ALERT
    .DPM_TimerAlert = 0,                            /*!< Timer used to monitor current and trig an ALERT                      */
    .DPM_SendAlert = {0},                           /*!< Save the Alert sent to port partner                                  */
    .DPM_RcvAlert = {0},                            /*!< Save the Alert received by port partner                              */
#endif /* _ALERT */
#endif /* USBPD_REV30_SUPPORT */
#if USBPD_PORT_COUNT >= 2
  },
  {
    .DPM_Reserved = 0,
#if defined(USBPD_REV30_SUPPORT)
#if _SRC_CAPA_EXT
    .FlagSendGetSrcCapaExtended = 0,
    .DPM_TimerSRCExtendedCapa = 0,                  /*!< timer to request the extended capa                                   */
    .DPM_RcvSRCExtendedCapa = {0},                  /*!< SRC Extended Capability received by port partner                     */
#endif /* _SRC_CAPA_EXT */
    .DPM_RcvSNKExtendedCapa = {0},                  /*!< SNK Extended Capability received by port partner                     */
#if _MANU_INFO
    .DPM_GetManufacturerInfo = {0},                 /*!< Get Manufacturer Info                                                */
#endif /* _MANU_INFO */
#if _BATTERY
    .DPM_GetBatteryStatus = {0},                    /*!< Get Battery status                                                   */
    .DPM_GetBatteryCapability = {0},                /*!< Get Battery Capability                                               */
    .DPM_BatteryStatus = {0},                       /*!< Battery status                                                       */
#endif /* _BATTERY */
#if _ALERT
    .DPM_TimerAlert = 0,                            /*!< Timer used to monitor current and trig an ALERT                      */
    .DPM_SendAlert = {0},                           /*!< Save the Alert sent to port partner                                  */
    .DPM_RcvAlert = {0},                            /*!< Save the Alert received by port partner                              */
#endif /* _ALERT */
#endif /* USBPD_REV30_SUPPORT */
  }
#else
  }
#endif /*USBPD_PORT_COUNT >= 2*/
};
#endif /* !USBPD_DPM_USER_C */
/* USER CODE END Private_Variables */

/* Exported functions --------------------------------------------------------*/
/** @addtogroup USBPD_USER_EXPORTED_FUNCTIONS
  * @{
  */
/** @addtogroup USBPD_USER_EXPORTED_FUNCTIONS_GROUP1
  * @{
  */
USBPD_StatusTypeDef USBPD_DPM_UserInit(void *MemoryPtr);
void                USBPD_DPM_WaitForTime(uint32_t Time);
void                USBPD_DPM_UserExecute(void const *argument);
void                USBPD_DPM_UserCableDetection(uint8_t PortNum, USBPD_CAD_EVENT State);
void                USBPD_DPM_UserTimerCounter(uint8_t PortNum);

/**
  * @}
  */

/** @addtogroup USBPD_USER_EXPORTED_FUNCTIONS_GROUP2
  * @{
  */
void                USBPD_DPM_Notification(uint8_t PortNum, USBPD_NotifyEventValue_TypeDef EventVal);
USBPD_StatusTypeDef USBPD_DPM_SetupNewPower(uint8_t PortNum);
void                USBPD_DPM_HardReset(uint8_t PortNum, USBPD_PortPowerRole_TypeDef CurrentRole, USBPD_HR_Status_TypeDef Status);
USBPD_StatusTypeDef USBPD_DPM_EvaluatePowerRoleSwap(uint8_t PortNum);
void                USBPD_DPM_ExtendedMessageReceived(uint8_t PortNum, USBPD_ExtendedMsg_TypeDef MsgType, uint8_t *ptrData, uint16_t DataSize);
void                USBPD_DPM_GetDataInfo(uint8_t PortNum, USBPD_CORE_DataInfoType_TypeDef DataId , uint8_t *Ptr, uint32_t *Size);
void                USBPD_DPM_SetDataInfo(uint8_t PortNum, USBPD_CORE_DataInfoType_TypeDef DataId , uint8_t *Ptr, uint32_t Size);
USBPD_StatusTypeDef USBPD_DPM_EvaluateRequest(uint8_t PortNum, USBPD_CORE_PDO_Type_TypeDef *PtrPowerObject);
void                USBPD_DPM_SNK_EvaluateCapabilities(uint8_t PortNum, uint32_t *PtrRequestData, USBPD_CORE_PDO_Type_TypeDef *PtrPowerObjectType);
uint32_t            USBPD_DPM_SNK_EvaluateMatchWithSRCPDO(uint8_t PortNum, uint32_t SrcPDO, uint32_t* PtrRequestedVoltage, uint32_t* PtrRequestedPower);
void                USBPD_DPM_PowerRoleSwap(uint8_t PortNum, USBPD_PortPowerRole_TypeDef CurrentRole, USBPD_PRS_Status_TypeDef Status);
USBPD_StatusTypeDef USBPD_DPM_EvaluateVconnSwap(uint8_t PortNum);
USBPD_StatusTypeDef USBPD_DPM_PE_VconnPwr(uint8_t PortNum, USBPD_FunctionalState State);
void                USBPD_DPM_EnterErrorRecovery(uint8_t PortNum);
USBPD_StatusTypeDef USBPD_DPM_EvaluateDataRoleSwap(uint8_t PortNum);
USBPD_FunctionalState USBPD_DPM_IsPowerReady(uint8_t PortNum, USBPD_VSAFE_StatusTypeDef Vsafe);

/**
  * @}
  */

/** @addtogroup USBPD_USER_EXPORTED_FUNCTIONS_GROUP3
  * @{
  */
USBPD_StatusTypeDef USBPD_DPM_RequestHardReset(uint8_t PortNum);
USBPD_StatusTypeDef USBPD_DPM_RequestCableReset(uint8_t PortNum);
USBPD_StatusTypeDef USBPD_DPM_RequestGotoMin(uint8_t PortNum);
USBPD_StatusTypeDef USBPD_DPM_RequestPing(uint8_t PortNum);
USBPD_StatusTypeDef USBPD_DPM_RequestMessageRequest(uint8_t PortNum, uint8_t IndexSrcPDO, uint16_t RequestedVoltage);
USBPD_StatusTypeDef USBPD_DPM_RequestGetSourceCapability(uint8_t PortNum);
USBPD_StatusTypeDef USBPD_DPM_RequestGetSinkCapability(uint8_t PortNum);
USBPD_StatusTypeDef USBPD_DPM_RequestDataRoleSwap(uint8_t PortNum);
USBPD_StatusTypeDef USBPD_DPM_RequestPowerRoleSwap(uint8_t PortNum);
USBPD_StatusTypeDef USBPD_DPM_RequestVconnSwap(uint8_t PortNum);
USBPD_StatusTypeDef USBPD_DPM_RequestSoftReset(uint8_t PortNum, USBPD_SOPType_TypeDef SOPType);
USBPD_StatusTypeDef USBPD_DPM_RequestSourceCapability(uint8_t PortNum);
USBPD_StatusTypeDef USBPD_DPM_RequestVDM_DiscoveryIdentify(uint8_t PortNum, USBPD_SOPType_TypeDef SOPType);
USBPD_StatusTypeDef USBPD_DPM_RequestVDM_DiscoverySVID(uint8_t PortNum, USBPD_SOPType_TypeDef SOPType);
USBPD_StatusTypeDef USBPD_DPM_RequestVDM_DiscoveryMode(uint8_t PortNum, USBPD_SOPType_TypeDef SOPType, uint16_t SVID);
USBPD_StatusTypeDef USBPD_DPM_RequestVDM_EnterMode(uint8_t PortNum, USBPD_SOPType_TypeDef SOPType, uint16_t SVID, uint8_t ModeIndex);
USBPD_StatusTypeDef USBPD_DPM_RequestVDM_ExitMode(uint8_t PortNum, USBPD_SOPType_TypeDef SOPType, uint16_t SVID, uint8_t ModeIndex);
USBPD_StatusTypeDef USBPD_DPM_RequestDisplayPortStatus(uint8_t PortNum, USBPD_SOPType_TypeDef SOPType, uint16_t SVID, uint32_t *pDPStatus);
USBPD_StatusTypeDef USBPD_DPM_RequestDisplayPortConfig(uint8_t PortNum, USBPD_SOPType_TypeDef SOPType, uint16_t SVID, uint32_t *pDPConfig);
USBPD_StatusTypeDef USBPD_DPM_RequestAttention(uint8_t PortNum, USBPD_SOPType_TypeDef SOPType, uint16_t SVID);
USBPD_StatusTypeDef USBPD_DPM_RequestAlert(uint8_t PortNum, USBPD_ADO_TypeDef Alert);
USBPD_StatusTypeDef USBPD_DPM_RequestGetSourceCapabilityExt(uint8_t PortNum);
USBPD_StatusTypeDef USBPD_DPM_RequestGetSinkCapabilityExt(uint8_t PortNum);
USBPD_StatusTypeDef USBPD_DPM_RequestGetManufacturerInfo(uint8_t PortNum, USBPD_SOPType_TypeDef SOPType, uint8_t* pManuInfoData);
USBPD_StatusTypeDef USBPD_DPM_RequestGetStatus(uint8_t PortNum);
USBPD_StatusTypeDef USBPD_DPM_RequestFastRoleSwap(uint8_t PortNum);
USBPD_StatusTypeDef USBPD_DPM_RequestGetPPS_Status(uint8_t PortNum);
USBPD_StatusTypeDef USBPD_DPM_RequestGetCountryCodes(uint8_t PortNum);
USBPD_StatusTypeDef USBPD_DPM_RequestGetCountryInfo(uint8_t PortNum, uint16_t CountryCode);
USBPD_StatusTypeDef USBPD_DPM_RequestGetBatteryCapability(uint8_t PortNum, uint8_t *pBatteryCapRef);
USBPD_StatusTypeDef USBPD_DPM_RequestGetBatteryStatus(uint8_t PortNum, uint8_t *pBatteryStatusRef);
USBPD_StatusTypeDef USBPD_DPM_RequestSecurityRequest(uint8_t PortNum);
#if defined(USBPDCORE_USBDATA) || defined(USBPDCORE_EPR)
uint32_t USBPD_DPM_RequestWhatToDo(uint8_t PortNum, uint32_t Id);
#endif
/* USER CODE BEGIN Function */

/* USER CODE END Function */
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

#ifdef __cplusplus
}
#endif

#endif /* __USBPD_DPM_USER_H_ */
