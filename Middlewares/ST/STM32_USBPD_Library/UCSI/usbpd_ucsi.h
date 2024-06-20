/**
  ******************************************************************************
  * @file    usbpd_ucsi.h
  * @author  MCD Application Team
  * @brief   Header file of UCSI driver
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

  This software comes from github.com/microsoft/Windows-driver-samples.
  Original file is under MS-PL License.
  A complete copy of this license can be found alongside this file.
  --
  Copyright (c) Microsoft Corporation. All rights reserved.
  Module Name:
      Ucsi.h
  Abstract:
      UCSI specification structure definitions.
      UCSI version 1.0
  Environment:
    Kernel-mode and user-mode.
  --
  */

#ifndef __USBPD_UCSI_H
#define __USBPD_UCSI_H

#ifdef __cplusplus
extern "C" {
#endif

/* GNU Compiler */
#if defined(__GNUC__)
/* ARM Compiler */
#elif defined(__CC_ARM)
#pragma anon_unions
/* IAR Compiler */
#elif defined (__ICCARM__)
#endif /* __GNUC__ */

/* Includes ------------------------------------------------------------------*/
#include "usbpd_def.h"
#include <stdint.h>
#include "cmsis_compiler.h"

/** @addtogroup STM32_USBPD_LIBRARY
  * @{
  */

/** @addtogroup USBPD_CORE
  * @{
  */

/** @addtogroup USBPD_CORE_UCSI
  * @{
  */

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
#define UCSI_MAX_DATA_LENGTH               0x10U
#define UCSI_MAX_NUM_ALT_MODE              0x80U
#define UCSI_MIN_TIME_TO_RESPOND_WITH_BUSY 0x0AU
#define UCSI_GET_ERROR_STATUS_DATA_LENGTH  0x10U
#define UCSI_MAX_NUM_PDOS                  0x32U /* Table 6-32 Counter parameters PD 3.1 V1.3 */
#define UCSI_MASK_UCSI_COMMAND             0xFFU /* Mask used to pass UCSI command to         */

/* Exported typedef ----------------------------------------------------------*/
/** @defgroup USBPD_UCSI_exported_TypeDef USBPD_UCSI exported Typedef
  *  @{
  */

/**
  *  @brief  List the USBPD UCSI function status.
  *  list of the returned status value, any negative value is corresponding to an error.
  */
typedef enum
{
  USBPD_UCSI_OK    =  0U,    /*!< Operation terminated successfully.  */
  USBPD_UCSI_ERROR =  1U     /*!< Operation terminated failed.        */
} USBPD_UCSI_Status_t;

/**
  * @brief Advanced trace driver definition
  */
typedef struct
{
#if defined(USBPD_THREADX)
  USBPD_UCSI_Status_t (* Init)(void (*ReceiveCommandCb)(uint8_t, uint8_t *), void *); /*!< Media initialization.    */
#else
  USBPD_UCSI_Status_t (* Init)(void (*ReceiveCommandCb)(uint8_t, uint8_t *));         /*!< Media initialization.    */
#endif /* USBPD_THREADX */
  void (* AlertON)(void);                                                             /*!< Media alert ON           */
  void (* AlertOFF)(void);                                                            /*!< Media alert OFF          */
} USBPD_UCSI_Driver_t;

typedef enum
{
  /* Use for messages between OPM -> PPM*/
  UcsiCommandUnkown                 = 0x00U,
  UcsiCommandPpmReset               = 0x01U,
  UcsiCommandCancel                 = 0x02U,
  UcsiCommandConnectorReset         = 0x03U,
  UcsiCommandAckCcCi                = 0x04U,
  UcsiCommandSetNotificationEnable  = 0x05U,
  UcsiCommandGetCapability          = 0x06U,
  UcsiCommandGetConnectorCapability = 0x07U,
  UcsiCommandSetCCom                = 0x08U, /* Optional */
  UcsiCommandSetUor                 = 0x09U,
  UcsiCommandSetPdm                 = 0x0AU, /* Optional - Obsolete from Rev1.2, Jan20 */
  UcsiCommandSetPdr                 = 0x0BU,
  UcsiCommandGetAlternateModes      = 0x0CU, /* Optional */
  UcsiCommandGetCamSupported        = 0x0DU, /* Optional */
  UcsiCommandGetCurrentCam          = 0x0EU, /* Optional */
  UcsiCommandSetNewCam              = 0x0FU, /* Optional */
  UcsiCommandGetPdos                = 0x10U, /* Optional */
  UcsiCommandGetCableProperty       = 0x11U, /* Optional */
  UcsiCommandGetConnectorStatus     = 0x12U,
  UcsiCommandGetErrorStatus         = 0x13U,
  UcsiCommandSetPowerLevel          = 0x14U, /* Optional */
  UcsiCommandGetPDMessage           = 0x15U, /* Optional */
  UcsiCommandMax                    = (UcsiCommandGetPDMessage + 1U)
} UCSI_COMMAND_t;


typedef enum
{
  UcsiRegDataStructureVersion             = 0x00U, /* PPM -> OPM */
  UcsiRegDataStructureReserved            = 0x02U, /* N/A        */
  UcsiRegDataStructureCci                 = 0x04U, /* PPM -> OPM */
  UcsiRegDataStructureControl             = 0x08U, /* OPM -> PPM */
  UcsiRegDataStructureMessageIn           = 0x10U, /* PPM -> OPM */
  UcsiRegDataStructureMessageOut          = 0x20U, /* OPM -> PPM */
  UcsiRegDataStructureUnkown              = (UcsiRegDataStructureMessageOut + 1U)
} UCSI_REG_t;


typedef union
{
  uint16_t AsUInt16;
  struct
  {
    uint16_t SubMinorVersion : 4U;
    uint16_t MinorVersion    : 4U;
    uint16_t MajorVersion    : 8U;
  };
} UCSI_VERSION_t, *PUCSI_VERSION_t;

typedef union
{
  uint32_t AsUInt32;
  struct
  {
    uint32_t                             : 1U;
    uint32_t ConnectorChangeIndicator    : 7U;
    uint32_t DataLength                  : 8U;
    uint32_t                             : 9U;
    uint32_t NotSupportedIndicator       : 1U;
    uint32_t CancelCompletedIndicator    : 1U;
    uint32_t ResetCompletedIndicator     : 1U;
    uint32_t BusyIndicator               : 1U;
    uint32_t AcknowledgeCommandIndicator : 1U;
    uint32_t ErrorIndicator              : 1U;
    uint32_t CommandCompletedIndicator   : 1U;
  };
} UCSI_CCI_t, *PUCSI_CCI_t;

typedef union
{
  uint64_t AsUInt64;
  struct
  {
    uint64_t Command         : 8U;
    uint64_t DataLength      : 8U;
    uint64_t ConnectorNumber : 7U;
    uint64_t HardReset       : 1U;
    uint64_t                 : 40U;
  };
} UCSI_CONNECTOR_RESET_COMMAND_t, *PUCSI_CONNECTOR_RESET_COMMAND_t;

typedef union
{
  uint64_t AsUInt64;
  struct
  {
    uint64_t Command                     : 8U;
    uint64_t DataLength                  : 8U;
    uint64_t ConnectorChangeAcknowledge  : 1U;
    uint64_t CommandCompletedAcknowledge : 1U;
    uint64_t                             : 46U;
  };
} UCSI_ACK_CC_CI_COMMAND_t, *PUCSI_ACK_CC_CI_COMMAND_t;

typedef union
{
  uint64_t AsUInt64;
  struct
  {
    uint64_t Command      : 8U;
    uint64_t DataLength   : 8U;
    uint64_t AckCciStatus : 32U; /* Based on the structure UCSI_CCI_t */
    uint64_t              : 16U;
  };
} UCSI_ACK_CC_CI_STATUS_COMMAND_t, *PUCSI_ACK_CC_CI_STATUS_COMMAND_t;

typedef union
{
  uint16_t NotificationEnable;
  struct
  {
    uint16_t CommandComplete                     : 1U;
    uint16_t ExternalSupplyChange                : 1U;
    uint16_t PowerOperationModeChange            : 1U;
    uint16_t                                     : 1U;
    uint16_t                                     : 1U;
    uint16_t SupportedProviderCapabilitiesChange : 1U;
    uint16_t NegotiatedPowerLevelChange          : 1U;
    uint16_t PdResetComplete                     : 1U;
    uint16_t SupportedCamChange                  : 1U;
    uint16_t BatteryChargingStatusChange         : 1U;
    uint16_t                                     : 1U;
    uint16_t ConnectorPartnerChange              : 1U;
    uint16_t PowerDirectionChange                : 1U;
    uint16_t                                     : 1U;
    uint16_t ConnectChange                       : 1U;
    uint16_t Error                               : 1U;
  };
} UCSI_SET_NOTIFICATION_ENABLE_t, *PUCSI_SET_NOTIFICATION_ENABLE_t;

typedef union
{
  uint64_t AsUInt64;
  struct
  {
    uint8_t Command;
    uint8_t DataLength;
    UCSI_SET_NOTIFICATION_ENABLE_t Content;
    uint32_t : 32U;
  };
} UCSI_SET_NOTIFICATION_ENABLE_COMMAND_t, *PUCSI_SET_NOTIFICATION_ENABLE_COMMAND_t;

typedef union
{
  uint8_t AsUInt8;
  struct
  {
    uint8_t AcSupply : 1U;
    uint8_t          : 1U;
    uint8_t Other    : 1U;
    uint8_t          : 3U;
    uint8_t UsesVBus : 1U;
    uint8_t          : 1U;
  };
} UCSI_BM_POWER_SOURCE_t, *PUCSI_BM_POWER_SOURCE_t;

typedef union
{
  uint32_t AsUInt32;
  struct
  {
    uint32_t SetCComSupported                     : 1U;
    uint32_t SetPowerLevelSupported               : 1U;
    uint32_t AlternateModeDetailsSupported        : 1U;
    uint32_t AlternateModeOverrideSupported       : 1U;
    uint32_t PdoDetailsSupported                  : 1U;
    uint32_t CableDetailsSupported                : 1U;
    uint32_t ExternalSupplyNotificationSupported  : 1U;
    uint32_t PdResetNotificationSupported         : 1U;
    uint32_t GetPDMessageSupported                : 1U;
    uint32_t                                      : 15U;
  };
} bmOptionalFeatures_t, *PbmOptionalFeatures_t;

typedef __PACKED_STRUCT
{
  __PACKED_UNION
  {
    uint32_t AsUInt32;
    __PACKED_STRUCT
    {
      uint32_t DisabledStateSupport : 1U;
      uint32_t BatteryCharging      : 1U;
      uint32_t UsbPowerDelivery     : 1U;
      uint32_t                      : 3U;
      uint32_t UsbTypeCCurrent      : 1U;
      uint32_t                      : 1U;
      uint32_t bmPowerSource        : 8U;
      uint32_t                      : 16U;
    };
  } bmAttributes;

  uint32_t bNumConnectors     : 7U;
  uint32_t                    : 1U;
  uint32_t OptionalFeatures   : 24U;
  uint32_t bNumAltModes       : 8U;

  uint32_t                    : 8U;
  uint32_t bcdBcVersion       :16U;
  uint32_t bcdPdVersion       :16U;
  uint32_t bcdUsbTypeCVersion :16U;

} UCSI_GET_CAPABILITY_IN_t, *PUCSI_GET_CAPABILITY_IN_t;


typedef union
{
  uint64_t AsUInt64;
  struct
  {
    uint64_t Command         : 8U;
    uint64_t DataLength      : 8U;
    uint64_t ConnectorNumber : 7U;
    uint64_t                 : 41U;
  };
} UCSI_GET_CONNECTOR_CAPABILITY_COMMAND_t, *PUCSI_GET_CONNECTOR_CAPABILITY_COMMAND_t;

typedef struct
{
  union
  {
    uint8_t AsUInt8;
    struct
    {
      uint8_t RpOnly             : 1U;
      uint8_t RdOnly             : 1U;
      uint8_t Drp                : 1U;
      uint8_t AudioAccessoryMode : 1U;
      uint8_t DebugAccessoryMode : 1U;
      uint8_t Usb2               : 1U;
      uint8_t Usb3               : 1U;
      uint8_t AlternateMode      : 1U;
    };
  } OperationMode;

  uint8_t Provider  : 1U;
  uint8_t Consumer  : 1U;
  uint8_t SwapToDFP : 1U;
  uint8_t SwapToUFP : 1U;
  uint8_t SwapToSRC : 1U;
  uint8_t SwapToSNK : 1U;
  uint8_t           : 2U;
} UCSI_GET_CONNECTOR_CAPABILITY_IN_t, *PUCSI_GET_CONNECTOR_CAPABILITY_IN_t;

typedef enum
{
  UcsiCCOperationModeRpOnly = 0x1U,
  UcsiCCOperationModeRdOnly = 0x2U,
  UcsiCCOperationModeDrp    = 0x4U
} UCSI_CC_OPERATION_MODE_t;

typedef union
{
  uint64_t AsUInt64;
  struct
  {
    uint64_t Command          : 8U;
    uint64_t DataLength       : 8U;
    uint64_t ConnectorNumber  : 7U;
    uint64_t UsbOperationMode : 3U;
    uint64_t                  : 38U;
  };
} UCSI_SET_UOM_COMMAND_t, *PUCSI_SET_UOM_COMMAND_t;

typedef enum
{
  UcsiUsbOperationRoleDfp        = 0x1U,
  UcsiUsbOperationRoleUfp        = 0x2U,
  UcsiUsbOperationRoleAcceptSwap = 0x4U
} UCSI_USB_OPERATION_ROLE_t;

typedef union
{
  uint64_t AsUInt64;
  struct
  {
    uint64_t Command          : 8U;
    uint64_t DataLength       : 8U;
    uint64_t ConnectorNumber  : 7U;
    uint64_t UsbOperationRole : 3U;
    uint64_t                  : 38U;
  };
} UCSI_SET_UOR_COMMAND_t, *PUCSI_SET_UOR_COMMAND_t;

typedef enum
{
  UcsiPowerDirectionModeProvider = 0x1U,
  UcsiPowerDirectionModeConsumer = 0x2U,
  UcsiPowerDirectionModeEither   = 0x4U
} UCSI_POWER_DIRECTION_MODE_t;

typedef union
{
  uint64_t AsUInt64;
  struct
  {
    uint64_t Command            : 8U;
    uint64_t DataLength         : 8U;
    uint64_t ConnectorNumber    : 7U;
    uint64_t PowerDirectionMode : 3U;
    uint64_t                    : 38U;
  };
} UCSI_SET_PDM_COMMAND_t, *PUCSI_SET_PDM_COMMAND_t;

typedef enum
{
  UcsiPowerDirectionRoleProvider   = 0x1U,
  UcsiPowerDirectionRoleConsumer   = 0x2U,
  UcsiPowerDirectionRoleAcceptSwap = 0x4U
} UCSI_POWER_DIRECTION_ROLE_t;

typedef union
{
  uint64_t AsUInt64;
  struct
  {
    uint64_t Command            : 8U;
    uint64_t DataLength         : 8U;
    uint64_t ConnectorNumber    : 7U;
    uint64_t PowerDirectionRole : 3U;
    uint64_t                    : 38U;
  };
} UCSI_SET_PDR_COMMAND_t, *PUCSI_SET_PDR_COMMAND_t;

typedef enum
{
  UcsiGetAlternateModesRecipientConnector = 0U,
  UcsiGetAlternateModesRecipientSop       = 1U,
  UcsiGetAlternateModesRecipientSopP      = 2U,
  UcsiGetAlternateModesRecipientSopPP     = 3U
} UCSI_GET_ALTERNATE_MODES_RECIPIENT_t;

typedef struct
{
  uint16_t Recipient              : 3U;
  uint16_t AlternateModeOffset    : 8U;
  uint16_t NumberOfAlternateModes : 3U;
  uint16_t                        : 2U;
} UCSI_GET_ALTERNATE_MODES_COMMAND_STRUCT_t, *PUCSI_GET_ALTERNATE_MODES_COMMAND_STRUCT_t;

typedef union
{
  uint64_t AsUInt64;
  struct
  {
    uint64_t Command                : 8U;
    uint64_t DataLength             : 8U;
    uint64_t Recipient              : 3U;
    uint64_t                        : 5U;
    uint64_t ConnectorNumber        : 7U;
    uint64_t                        : 1U;
    uint64_t AlternateModeOffset    : 8U;
    uint64_t NumberOfAlternateModes : 2U;
    uint64_t                        : 22U;
  };
} UCSI_GET_ALTERNATE_MODES_COMMAND_t, *PUCSI_GET_ALTERNATE_MODES_COMMAND_t;

typedef struct
{
  uint16_t Svid;
  uint32_t Mode;
} UCSI_ALTERNATE_MODE_t, *PUCSI_ALTERNATE_MODE_t;

typedef struct
{
  UCSI_ALTERNATE_MODE_t AlternateModes[2];
} UCSI_GET_ALTERNATE_MODES_IN_t, *PUCSI_GET_ALTERNATE_MODES_IN_t;

typedef union
{
  uint64_t AsUInt64;
  struct
  {
    uint64_t Command         : 8U;
    uint64_t DataLength      : 8U;
    uint64_t ConnectorNumber : 7U;
    uint64_t                 : 41U;
  };
} UCSI_GET_CAM_SUPPORTED_COMMAND_t, *PUCSI_GET_CAM_SUPPORTED_COMMAND_t;

typedef struct
{
  uint8_t bmAlternateModeSupported[16];
} UCSI_GET_CAM_SUPPORTED_IN_t, *PUCSI_GET_CAM_SUPPORTED_IN_t;

typedef union
{
  uint64_t AsUInt64;
  struct
  {
    uint64_t Command         : 8U;
    uint64_t DataLength      : 8U;
    uint64_t ConnectorNumber : 7U;
    uint64_t                 : 41U;
  };
} UCSI_GET_CURRENT_CAM_COMMAND_t, *PUCSI_GET_CURRENT_CAM_COMMAND_t;

typedef struct
{
  uint8_t CurrentAlternateMode;
} UCSI_GET_CURRENT_CAM_IN_t, *PUCSI_GET_CURRENT_CAM_IN_t;

typedef union
{
  uint64_t AsUInt64;
  struct
  {
    uint64_t Command         : 8U;
    uint64_t DataLength      : 8U;
    uint64_t ConnectorNumber : 7U;
    uint64_t EnterOrExit     : 1U;
    uint64_t NewCam          : 8U;
    uint64_t AmSpecific      : 32U;
  };
} UCSI_SET_NEW_CAM_COMMAND_t, *PUCSI_SET_NEW_CAM_COMMAND_t;

typedef enum
{
  UcsiGetPdosTypeSink   = 0U,
  UcsiGetPdosTypeSource = 1U
} UCSI_GET_PDOS_TYPE_t;

typedef enum
{
  UcsiGetPdosCurrentSourceCapabilities    = 0U,
  UcsiGetPdosAdvertisedSourceCapabilities = 1U,
  UcsiGetPdosMaxSourceCapabilities        = 2U
} UCSI_GET_PDOS_SOURCE_CAPABILITIES_TYPE_t;

typedef union
{
  uint64_t AsUInt64;
  struct
  {
    uint64_t Command                                                : 8U;
    uint64_t DataLength                                             : 8U;
    uint64_t ConnectorNumber                                        : 7U;
    uint64_t PartnerPdo                                             : 1U;
    uint64_t PdoOffset                                              : 8U;
    uint64_t NumberOfPdos                                           : 2U;
    UCSI_GET_PDOS_TYPE_t SourceOrSinkPdos                           : 1U;
    UCSI_GET_PDOS_SOURCE_CAPABILITIES_TYPE_t SourceCapabilitiesType : 2U;
    uint64_t                                                        : 27U;
  };
} UCSI_GET_PDOS_COMMAND_t, *PUCSI_GET_PDOS_COMMAND_t;

typedef struct
{
  uint32_t Pdos[4];
} UCSI_GET_PDOS_IN_t, *PUCSI_GET_PDOS_IN_t;

typedef union
{
  uint64_t AsUInt64;
  struct
  {
    uint64_t Command         : 8U;
    uint64_t DataLength      : 8U;
    uint64_t ConnectorNumber : 7U;
    uint64_t                 : 41U;
  };
} UCSI_GET_CABLE_PROPERTY_COMMAND_t, *PUCSI_GET_CABLE_PROPERTY_COMMAND_t;

typedef struct
{
  union
  {
    uint16_t AsUInt16;
    struct
    {
      uint16_t SpeedExponent : 2U;
      uint16_t Mantissa      : 14U;
    };
  } bmSpeedSupported;

  uint8_t bCurrentCapability;
  uint16_t VBusInCable          : 1U;
  uint16_t CableType            : 1U;
  uint16_t Directionality       : 1U;
  USBPD_CableToType PlugEndType : 2U;
  uint16_t ModeSupport          : 1U;
  uint16_t                      : 2U;
  USBPD_CableLatency Latency    : 4U;
  uint16_t                      : 4U;
} UCSI_GET_CABLE_PROPERTY_IN_t, *PUCSI_GET_CABLE_PROPERTY_IN_t;

typedef union
{
  uint64_t AsUInt64;
  struct
  {
    uint64_t Command         : 8U;
    uint64_t DataLength      : 8U;
    uint64_t ConnectorNumber : 7U;
    uint64_t                 : 41U;
  };
} UCSI_GET_CONNECTOR_STATUS_COMMAND_t, *PUCSI_GET_CONNECTOR_STATUS_COMMAND_t;

typedef enum
{
  UcsiPowerOperationModeNoConsumer = 0U,
  UcsiPowerOperationModeDefaultUsb = 1U,
  UcsiPowerOperationModeBc         = 2U,
  UcsiPowerOperationModePd         = 3U,
  UcsiPowerOperationModeTypeC1500  = 4U,
  UcsiPowerOperationModeTypeC3000  = 5U
} UCSI_POWER_OPERATION_MODE_t;

typedef enum
{
  UcsiPowerDirectionConsumer = 0U,
  UcsiPowerDirectionProvider = 1U
} UCSI_POWER_DIRECTION_t;

typedef enum
{
  UcsiConnectorPartnerFlagUsb           = 0x1U,
  UcsiConnectorPartnerFlagAlternateMode = 0x2U
} UCSI_CONNECTOR_PARTNER_FLAGS_t;

typedef enum
{
  UcsiConnectorPartnerUnknown                 = 0U,
  UcsiConnectorPartnerTypeDfp                 = 1U,
  UcsiConnectorPartnerTypeUfp                 = 2U,
  UcsiConnectorPartnerTypePoweredCableNoUfp   = 3U,
  UcsiConnectorPartnerTypePoweredCableWithUfp = 4U,
  UcsiConnectorPartnerTypeDebugAccessory      = 5U,
  UcsiConnectorPartnerTypeAudioAccessory      = 6U
} UCSI_CONNECTOR_PARTNER_TYPE_t;

typedef enum
{
  UcsiBatteryChargingNotCharging     = 0U,
  UcsiBatteryChargingNominal         = 1U,
  UcsiBatteryChargingSlowCharging    = 2U,
  UcsiBatteryChargingTrickleCharging = 3U
} UCSI_BATTERY_CHARGING_STATUS_t;

typedef struct
{
  union
  {
    uint16_t AsUInt16;
    struct
    {
      uint16_t                                     : 1U;
      uint16_t ExternalSupplyChange                : 1U;
      uint16_t PowerOperationModeChange            : 1U;
      uint16_t                                     : 1U;
      uint16_t                                     : 1U;
      uint16_t SupportedProviderCapabilitiesChange : 1U;
      uint16_t NegotiatedPowerLevelChange          : 1U;
      uint16_t PdResetComplete                     : 1U;
      uint16_t SupportedCamChange                  : 1U;
      uint16_t BatteryChargingStatusChange         : 1U;
      uint16_t                                     : 1U;
      uint16_t ConnectorPartnerChange              : 1U;
      uint16_t PowerDirectionChange                : 1U;
      uint16_t                                     : 1U;
      uint16_t ConnectChange                       : 1U;
      uint16_t Error                               : 1U;
    };
  } ConnectorStatusChange;

  UCSI_POWER_OPERATION_MODE_t PowerOperationMode     : 3U;
  uint16_t ConnectStatus                             : 1U;
  UCSI_POWER_DIRECTION_t PowerDirection              : 1U;
  uint16_t ConnectorPartnerFlags                     : 8U;
  UCSI_CONNECTOR_PARTNER_TYPE_t ConnectorPartnerType : 3U;

  uint32_t RequestDataObject;

  union
  {
    struct
    {
      UCSI_BATTERY_CHARGING_STATUS_t BatteryChargingStatus : 2U;
      uint8_t PowerBudgetLimitedReason                     : 4U;
      uint8_t                                              : 2U;
    };

    struct
    {
      uint8_t                          : 2U;
      uint8_t PowerBudgetLowered       : 1U;
      uint8_t ReachingPowerBudgetLimit : 1U;
      uint8_t                          : 1U;
      uint8_t                          : 1U;
      uint8_t                          : 2U;
    } bmPowerBudgetLimitedReason;
  };
} UCSI_GET_CONNECTOR_STATUS_IN_t, *PUCSI_GET_CONNECTOR_STATUS_IN_t;

typedef union
{
  uint64_t AsUInt64;
  struct
  {
    uint64_t Command    : 8U;
    uint64_t DataLength : 8U;
    uint64_t            : 48U;
  };
} UCSI_GET_ERROR_STATUS_COMMAND_t, *PUCSI_GET_ERROR_STATUS_COMMAND_t;

typedef struct
{
  union
  {
    uint16_t AsUInt16;
    struct
    {
      uint16_t UnrecognizedCommandError           : 1U;
      uint16_t NonExistentConnectorNumberError    : 1U;
      uint16_t InvalidCommandParametersError      : 1U;
      uint16_t IncompatibleConnectorPartnerError  : 1U;
      uint16_t CcCommunicationError               : 1U;
      uint16_t CommandFailureDueToDeadBattery     : 1U;
      uint16_t ContractNegotiationFailure         : 1U;
      uint16_t OverCurrent                        : 1U;
      uint16_t Undefined                          : 1U;
      uint16_t PortPartnerRejectedSwap            : 1U;
      uint16_t HardReset                          : 1U;
      uint16_t PPM_PolicyConflict                 : 1U;
      uint16_t SwapRejected                       : 1U;
      uint16_t                                    : 3U;
    };
  } ErrorInformation;

  uint8_t VendorDefined[14];

} UCSI_GET_ERROR_STATUS_IN_t, *PUCSI_GET_ERROR_STATUS_IN_t;

/*
  * Definition of SET POWER LEVEL COMMAND
  */
typedef enum
{
  UcsiTypeCCurrentPPMDefault    = 0U,  /* PPM Defined default */
  UcsiTypeCCurrent3A            = 1U,  /* 3A                  */
  UcsiTypeCCurrent1P5A          = 2U,  /* 1.5A                */
  UcsiTypeCCurrentUSBDefault    = 3U   /* USB Type-C Default  */
} UCSI_TYPEC_CURRENT_t;

typedef union
{
  uint64_t AsUInt64;
  struct
  {
    uint64_t Command                  : 8U;
    uint64_t DataLength               : 8U;
    uint64_t ConnectorNumber          : 7U;  /* 0: means max power negotiable by all the connected ports */
    uint64_t SourceOrSink             : 1U;  /* 1: source, 0: Sink                                       */
    uint64_t USB_PD_MaxPower          : 8U;  /* 0.5W unit - 0x00 means max power determined by PPM,
                                                0xFF disable this command                                */
    UCSI_TYPEC_CURRENT_t TypeCCurrent : 2U;  /* Maximum current valid if ConnectorNumber != 0 else 0     */
    uint64_t                          : 30U;
  };
} UCSI_SET_POWER_LEVEL_COMMAND_t, *PUCSI_SET_POWER_LEVEL_COMMAND_t;

/*
  * Definition of GET PD MESSAGE COMMAND
  */
typedef enum
{
  UcsiRecipientConnector    = 0U,  /* Connector */
  UcsiRecipientSOP          = 1U,  /* SOP       */
  UcsiRecipientSOPPrime     = 2U,  /* SOP'      */
  UcsiRecipientSOPSecond    = 3U   /* SOP''     */
} UCSI_RECIPIENT_t;

typedef enum
{
  UcsiRspMsgTypeGetSnkCapaExt    = 0U, /* Get Sink Capa extended     */
  UcsiRspMsgTypeGetSrcCapaExt    = 1U, /* Get Source Capa extended   */
  UcsiRspMsgTypeGetBatteryCapa   = 2U, /* Get Battery capa           */
  UcsiRspMsgTypeGetBatteryStatus = 3U, /* Get Battery Status         */
  UcsiRspMsgTypeDiscoIdentReq    = 4U  /* Discovery Identity request */
} UCSI_RSP_MSG_TYPE_t;

typedef union
{
  uint64_t AsUInt64;
  struct
  {
    uint64_t Command                : 8U;
    uint64_t DataLength             : 8U;
    uint64_t ConnectorNumber        : 7U;  /* Set to the connector being queried                       */
    UCSI_RECIPIENT_t Recipient      : 3U;  /* port partner or cable plus or idenitifed connector       */
    uint64_t MessageOffset          : 8U;  /* Starting offset (in bytes) of the message to be returned */
    uint64_t NumberOfBytes          : 8U;  /* Number of bytes to  return starting from the offset      */
    UCSI_RSP_MSG_TYPE_t RspMsgType  : 6U;  /* Response message Type                                    */
    uint16_t                        : 16U;
  };
} UCSI_GET_PD_MESSAGE_COMMAND_t, *PUCSI_GET_PD_MESSAGE_COMMAND_t;

typedef union
{
  uint64_t AsUInt64;

  struct
  {
    uint64_t Command         : 8U;
    uint64_t DataLength      : 8U;
    uint64_t CommandSpecific : 48U;
  };

  UCSI_CONNECTOR_RESET_COMMAND_t          ConnectorReset;
  UCSI_ACK_CC_CI_COMMAND_t                AckCcCi;
  UCSI_SET_NOTIFICATION_ENABLE_COMMAND_t  SetNotificationEnable;
  UCSI_GET_CONNECTOR_CAPABILITY_COMMAND_t GetConnectorCapability;
  UCSI_SET_UOM_COMMAND_t                  SetUom;
  UCSI_SET_UOR_COMMAND_t                  SetUor;
  UCSI_SET_PDM_COMMAND_t                  SetPdm;
  UCSI_SET_PDR_COMMAND_t                  SetPdr;
  UCSI_GET_ALTERNATE_MODES_COMMAND_t      GetAlternateModes;
  UCSI_GET_CAM_SUPPORTED_COMMAND_t        GetCamSupported;
  UCSI_GET_CURRENT_CAM_COMMAND_t          GetCurrentCam;
  UCSI_SET_NEW_CAM_COMMAND_t              SetNewCam;
  UCSI_GET_PDOS_COMMAND_t                 GetPdos;
  UCSI_GET_CABLE_PROPERTY_COMMAND_t       GetCableProperty;
  UCSI_GET_CONNECTOR_STATUS_COMMAND_t     GetConnectorStatus;
  UCSI_GET_ERROR_STATUS_COMMAND_t         GetErrorStatus;
  UCSI_ACK_CC_CI_STATUS_COMMAND_t         AckCcCiStatus;
  UCSI_SET_POWER_LEVEL_COMMAND_t          SetPowerLevel;
  UCSI_GET_PD_MESSAGE_COMMAND_t           GetPDMessage;

} UCSI_CONTROL_t, *PUCSI_CONTROL_t;

typedef union
{
  uint8_t AsBuffer[UCSI_MAX_DATA_LENGTH];

  UCSI_GET_CAPABILITY_IN_t           Capability;
  UCSI_GET_CONNECTOR_CAPABILITY_IN_t ConnectorCapability;
  UCSI_GET_ALTERNATE_MODES_IN_t      AlternateModes;
  UCSI_GET_CAM_SUPPORTED_IN_t        CamSupported;
  UCSI_GET_CURRENT_CAM_IN_t          CurrentCam;
  UCSI_GET_PDOS_IN_t                 Pdos;
  UCSI_GET_CABLE_PROPERTY_IN_t       CableProperty;
  UCSI_GET_CONNECTOR_STATUS_IN_t     ConnectorStatus;
  UCSI_GET_ERROR_STATUS_IN_t         ErrorStatus;

} UCSI_MESSAGE_IN_t, *PUCSI_MESSAGE_IN_t;


typedef union
{
  uint8_t AsBuffer[UCSI_MAX_DATA_LENGTH];
} UCSI_MESSAGE_OUT_t, *PUCSI_MESSAGE_OUT_t;

typedef struct
{
  UCSI_VERSION_t                     UcsiVersion;
  uint16_t                           : 16U;
  UCSI_CCI_t                         CCI;
  UCSI_CONTROL_t                     Control;
  UCSI_GET_CAPABILITY_IN_t           Capability;
  UCSI_GET_CONNECTOR_CAPABILITY_IN_t ConnectorCapability;
  UCSI_GET_ALTERNATE_MODES_IN_t      AlternateModes;
  UCSI_GET_CAM_SUPPORTED_IN_t        CamSupported;
  UCSI_GET_CURRENT_CAM_IN_t          CurrentCam;
  UCSI_GET_PDOS_IN_t                 Pdos;
  UCSI_GET_CABLE_PROPERTY_IN_t       CableProperty;
  UCSI_GET_CONNECTOR_STATUS_IN_t     ConnectorStatus;
  UCSI_GET_ERROR_STATUS_IN_t         ErrorStatus;
} UCSI_DATA_BLOCK_t, *PUCSI_DATA_BLOCK_t;

/**
  *  @}
  */

/* External variables --------------------------------------------------------*/
/** @defgroup USBPD_CORE_UCSI_exported_variables USBPD_CORE_UCSI exported variables
  *
  *  @{
  */
/**
  *  @brief This structure is the linked with the IF layer implementation.
  */
extern const USBPD_UCSI_Driver_t USBPD_UcsiDriver;

/**
  *  @}
  */

/* Exported functions --------------------------------------------------------*/
/** @addtogroup USBPD_CORE_UCSI_Exported_Functions
  * @{
  */

#if defined(USBPD_THREADX)
USBPD_StatusTypeDef USBPD_UCSI_Init(void *MemoryPtr);
#else
USBPD_StatusTypeDef USBPD_UCSI_Init(void);
#endif /* USBPD_THREADX */
void                USBPD_UCSI_DeInit();
void                USBPD_UCSI_SendNotification(uint32_t PortNumber);
void                USBPD_UCSI_ClearAlert(uint32_t PortNumber);

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

#endif /* __USBPD_UCSI_H */
