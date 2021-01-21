//******************************************************************************
/// COPYRIGHT (c) by Hilscher GmbH
/// All Rights Reserved 
/// \file EplCn_If_Public.h
//******************************************************************************	

/**************************************************************************************
Copyright (c) Hilscher Gesellschaft fuer Systemautomation mbH. All Rights Reserved.
***************************************************************************************
$Id: EplCn_PacketAPI.h 4596 2020-12-01 15:26:38Z MAzarkan $:

Description:
  Powerlink Controlled Node stack packet API
**************************************************************************************/

#ifndef __EPLCN_PACKETAPI_H
#define __EPLCN_PACKETAPI_H

#include <stdint.h>
#include "Hil_Packet.h"
#include "Hil_Types.h"

/*************************************************************************************/
/* Commands                                                                          */
/*************************************************************************************/
/* Start of packet range. Used to filter commands to be handled by the stack */
typedef enum EPLCN_IF_CMD_Etag
{
  /* NMT state change indications */
  EPLCN_IF_NMT_STATE_CHANGE_IND                             = 0xA200,
  EPLCN_IF_NMT_STATE_CHANGE_RES                             = 0xA201,
  EPLCN_IF_NMT_SET_STATE_REQ                                = 0xA202,
  EPLCN_IF_NMT_SET_STATE_CNF                                = 0xA203,

  /* NMT commands */
  EPLCN_IF_NMT_CMD_ENABLE_RDY_TO_OPERATE_IND                = 0xA210,
  EPLCN_IF_NMT_CMD_ENABLE_RDY_TO_OPERATE_RES                = 0xA211,

  /* Status/Error entries and emergency queue */
  EPLCN_IF_NMT_WRITE_ERROR_ENTRY_REQ                        = 0xA220,
  EPLCN_IF_NMT_WRITE_ERROR_ENTRY_CNF                        = 0xA221,
  EPLCN_IF_NMT_NEW_ERROR_ENTRY_IND                          = 0xA222,
  EPLCN_IF_NMT_NEW_ERROR_ENTRY_RES                          = 0xA223,
  EPLCN_IF_NMT_WRITE_STATUS_ENTRY_REQ                       = 0xA224,
  EPLCN_IF_NMT_WRITE_STATUS_ENTRY_CNF                       = 0xA225,
  EPLCN_IF_NMT_NEW_STATUS_ENTRY_IND                         = 0xA226,
  EPLCN_IF_NMT_NEW_STATUS_ENTRY_RES                         = 0xA227,
  EPLCN_IF_NMT_WRITE_STATIC_ERROR_BIT_FIELD_REQ             = 0xA228,
  EPLCN_IF_NMT_WRITE_STATIC_ERROR_BIT_FIELD_CNF             = 0xA229,

  /* Configuration */
  EPLCN_IF_SET_CONFIG_REQ                                   = 0xA230,
  EPLCN_IF_SET_CONFIG_CNF                                   = 0xA231,
  EPLCN_IF_SET_PDO_SIZE_REQ                                 = 0xA232,
  EPLCN_IF_SET_PDO_SIZE_CNF                                 = 0xA233,
  EPLCN_IF_SET_NODE_ID_REQ                                  = 0xA234,
  EPLCN_IF_SET_NODE_ID_CNF                                  = 0xA235,
  EPLCN_IF_CROSS_TRAFFIC_CONFIGURED_IND                     = 0xA236,
  EPLCN_IF_CROSS_TRAFFIC_CONFIGURED_RES                     = 0xA237,

} EPLCN_IF_CMD_E;


/* Pragma pack */
#ifdef PRAGMA_PACK_ENABLE
  #pragma PRAGMA_PACK_1(EPLCN_PACKET_API)
#endif

/*************************************************************************************/
/* Packet: EPLCN_IF_NMT_STATE_CHANGE_IND                                             */
/*************************************************************************************/
typedef __HIL_PACKED_PRE struct  EPLCN_IF_NMT_STATE_CHANGE_IND_DATA_Ttag
{
  uint8_t   bCurrentState;  //NMT state as defined in the EPSG (see Epl_Common_Defines.h). Status led has to be handled depending on the current state
  uint8_t   fErrorLedIsOn;  //Error led is indicated explicitly here
} __HIL_PACKED_POST EPLCN_IF_NMT_STATE_CHANGE_IND_DATA_T;

typedef __HIL_PACKED_PRE struct  EPLCN_IF_NMT_STATE_CHANGE_IND_Ttag
{
  HIL_PACKET_HEADER_T                   tHead;
  EPLCN_IF_NMT_STATE_CHANGE_IND_DATA_T  tData;
} __HIL_PACKED_POST EPLCN_IF_NMT_STATE_CHANGE_IND_T;

typedef __HIL_PACKED_PRE struct  EPLCN_IF_NMT_STATE_CHANGE_RES_Ttag
{
  HIL_PACKET_HEADER_T                   tHead;
} __HIL_PACKED_POST EPLCN_IF_NMT_STATE_CHANGE_RES_T;

/*************************************************************************************/
/* Packet: EPLCN_IF_NMT_SET_STATE_REQ                                                */
/*************************************************************************************/
typedef __HIL_PACKED_PRE struct  EPLCN_IF_NMT_SET_STATE_REQ_DATA_Ttag
{
  uint8_t   bTargetState;  //Targeted NMT state as defined in the EPSG (see Epl_Common_Defines.h).
} __HIL_PACKED_POST EPLCN_IF_NMT_SET_STATE_REQ_DATA_T;

typedef __HIL_PACKED_PRE struct  EPLCN_IF_NMT_SET_STATE_REQ_Ttag
{
  HIL_PACKET_HEADER_T                   tHead;
  EPLCN_IF_NMT_SET_STATE_REQ_DATA_T     tData;
} __HIL_PACKED_POST EPLCN_IF_NMT_SET_STATE_REQ_T;

typedef __HIL_PACKED_PRE struct  EPLCN_IF_NMT_SET_STATE_CNF_Ttag
{
  HIL_PACKET_HEADER_T                   tHead;
} __HIL_PACKED_POST EPLCN_IF_NMT_SET_STATE_CNF_T;

/*************************************************************************************/
/* Packet: EPLCN_IF_NMT_CMD_ENABLE_RDY_TO_OPERATE_IND                                */
/*************************************************************************************/
typedef __HIL_PACKED_PRE struct  EPLCN_IF_NMT_CMD_ENABLE_RDY_TO_OPERATE_IND_Ttag
{
  HIL_PACKET_HEADER_T                   tHead;
} __HIL_PACKED_POST EPLCN_IF_NMT_CMD_ENABLE_RDY_TO_OPERATE_IND_T;

typedef __HIL_PACKED_PRE struct  EPLCN_IF_NMT_CMD_ENABLE_RDY_TO_OPERATE_RES_Ttag
{
  HIL_PACKET_HEADER_T                   tHead;
} __HIL_PACKED_POST EPLCN_IF_NMT_CMD_ENABLE_RDY_TO_OPERATE_RES_T;

/*************************************************************************************/
/* Packet: EPLCN_IF_NMT_WRITE_ERROR_ENTRY_REQ                                        */
/*************************************************************************************/
typedef __HIL_PACKED_PRE struct EPLCN_IF_NMT_WRITE_ERROR_ENTRY_REQ_DATA_Ttag
{
  /* Error entry type (see EplCn_Defines.h) */
  uint16_t        usEntryType;
  /* Error entry code */
  uint16_t        usErrorCode;
  /* Profile or vendor specific additional error information */
  /* This is specified as UNSIGNED64 in the EPSG. But it is declared as byte field here,
   * in order to simplify encoding/decoding of the information with a neutral endianness */
  uint8_t         abAddInformation[8];
} __HIL_PACKED_POST EPLCN_IF_NMT_WRITE_ERROR_ENTRY_REQ_DATA_T;

typedef __HIL_PACKED_PRE struct EPLCN_IF_NMT_WRITE_ERROR_ENTRY_REQ_Ttag
{
  HIL_PACKET_HEADER_T                       tHead;
  EPLCN_IF_NMT_WRITE_ERROR_ENTRY_REQ_DATA_T tData;
} __HIL_PACKED_POST EPLCN_IF_NMT_WRITE_ERROR_ENTRY_REQ_T;

typedef __HIL_PACKED_PRE struct  EPLCN_IF_NMT_WRITE_ERROR_ENTRY_CNF_Ttag
{
  HIL_PACKET_HEADER_T                       tHead;
} __HIL_PACKED_POST EPLCN_IF_NMT_WRITE_ERROR_ENTRY_CNF_T;

/*************************************************************************************/
/* Packet: EPLCN_IF_NMT_NEW_ERROR_ENTRY_IND                                          */
/*************************************************************************************/
typedef __HIL_PACKED_PRE struct EPLCN_IF_NMT_NEW_ERROR_ENTRY_IND_DATA_Ttag
{
  /* Error entry type (see EplCn_Defines.h) */
  uint16_t        usEntryType;
  /* Error entry code */
  uint16_t        usErrorCode;
  /* Profile or vendor specific additional error information */
  /* This is specified as UNSIGNED64 in the EPSG. But it is declared as byte field here,
   * in order to simplify encoding/decoding of the information with a neutral endianness */
  uint8_t         abAddInformation[8];
} __HIL_PACKED_POST EPLCN_IF_NMT_NEW_ERROR_ENTRY_IND_DATA_T;

typedef __HIL_PACKED_PRE struct EPLCN_IF_NMT_NEW_ERROR_ENTRY_IND_Ttag
{
  HIL_PACKET_HEADER_T                     tHead;
  EPLCN_IF_NMT_NEW_ERROR_ENTRY_IND_DATA_T tData;
} __HIL_PACKED_POST EPLCN_IF_NMT_NEW_ERROR_ENTRY_IND_T;

typedef __HIL_PACKED_PRE struct  EPLCN_IF_NMT_NEW_ERROR_ENTRY_RES_Ttag
{
  HIL_PACKET_HEADER_T                     tHead;
} __HIL_PACKED_POST EPLCN_IF_NMT_NEW_ERROR_ENTRY_RES_T;

/*************************************************************************************/
/* Packet: EPLCN_IF_NMT_WRITE_STATUS_ENTRY_REQ                                       */
/*************************************************************************************/
typedef __HIL_PACKED_PRE struct EPLCN_IF_NMT_WRITE_STATUS_ENTRY_REQ_DATA_Ttag
{
  /* Status Entry Number */
  uint16_t        usStatusEntryNumber;
  /* Error entry type (see EplCn_Defines.h) */
  uint16_t        usEntryType;
  /* Error entry code */
  uint16_t        usErrorCode;
  /* Profile or vendor specific additional error information */
  /* This is specified as UNSIGNED64 in the EPSG. But it is declared as byte field here,
   * in order to simplify encoding/decoding of the information with a neutral endianness */
  uint8_t         abAddInformation[8];
} __HIL_PACKED_POST EPLCN_IF_NMT_WRITE_STATUS_ENTRY_REQ_DATA_T;

typedef __HIL_PACKED_PRE struct EPLCN_IF_NMT_WRITE_STATUS_ENTRY_REQ_Ttag
{
  HIL_PACKET_HEADER_T                         tHead;
  EPLCN_IF_NMT_WRITE_STATUS_ENTRY_REQ_DATA_T  tData;
} __HIL_PACKED_POST EPLCN_IF_NMT_WRITE_STATUS_ENTRY_REQ_T;

typedef __HIL_PACKED_PRE struct  EPLCN_IF_NMT_WRITE_STATUS_ENTRY_CNF_Ttag
{
  HIL_PACKET_HEADER_T                         tHead;
} __HIL_PACKED_POST EPLCN_IF_NMT_WRITE_STATUS_ENTRY_CNF_T;

/*************************************************************************************/
/* Packet: EPLCN_IF_NMT_NEW_STATUS_ENTRY_IND                                         */
/*************************************************************************************/
typedef __HIL_PACKED_PRE struct EPLCN_IF_NMT_NEW_STATUS_ENTRY_IND_DATA_Ttag
{
  /* Status Entry Number */
  uint16_t        usStatusEntryNumber;
  /* Error entry type (see EplCn_Defines.h) */
  uint16_t        usEntryType;
  /* Error entry code */
  uint16_t        usErrorCode;
  /* Profile or vendor specific additional error information */
  /* This is specified as UNSIGNED64 in the EPSG. But it is declared as byte field here,
   * in order to simplify encoding/decoding of the information with a neutral endianness */
  uint8_t         abAddInformation[8];
} __HIL_PACKED_POST EPLCN_IF_NMT_NEW_STATUS_ENTRY_IND_DATA_T;

typedef __HIL_PACKED_PRE struct EPLCN_IF_NMT_NEW_STATUS_ENTRY_IND_Ttag
{
  HIL_PACKET_HEADER_T                       tHead;
  EPLCN_IF_NMT_NEW_STATUS_ENTRY_IND_DATA_T  tData;
} __HIL_PACKED_POST EPLCN_IF_NMT_NEW_STATUS_ENTRY_IND_T;

typedef __HIL_PACKED_PRE struct EPLCN_IF_NMT_NEW_STATUS_ENTRY_RES_Ttag
{
  HIL_PACKET_HEADER_T                       tHead;
} __HIL_PACKED_POST EPLCN_IF_NMT_NEW_STATUS_ENTRY_RES_T;

/*************************************************************************************/
/* Packet: EPLCN_IF_NMT_WRITE_STATIC_ERROR_BIT_FIELD_REQ                             */
/*************************************************************************************/
typedef __HIL_PACKED_PRE struct EPLCN_IF_NMT_WRITE_STATIC_ERROR_BIT_FIELD_REQ_DATA_ENTRY_Ttag
{
  /* Bit number */
  uint8_t                       bBitNumber;
  /* Bit value */
  uint8_t                       fBitValue;
} __HIL_PACKED_POST EPLCN_IF_NMT_WRITE_STATIC_ERROR_BIT_FIELD_REQ_DATA_ENTRY_T;

typedef __HIL_PACKED_PRE struct EPLCN_IF_NMT_WRITE_STATIC_ERROR_BIT_FIELD_REQ_DATA_Ttag
{
  /* Only the first bit entry is defined here. Other entries may follow. The Stack recognizes
   * the number of the entries by checking the value of ulLen in the packet head */
  EPLCN_IF_NMT_WRITE_STATIC_ERROR_BIT_FIELD_REQ_DATA_ENTRY_T  atEntry[1];
} __HIL_PACKED_POST EPLCN_IF_NMT_WRITE_STATIC_ERROR_BIT_FIELD_REQ_DATA_T;

typedef __HIL_PACKED_PRE struct EPLCN_IF_NMT_WRITE_STATIC_ERROR_BIT_FIELD_REQ_Ttag
{
  HIL_PACKET_HEADER_T                                   tHead;
  EPLCN_IF_NMT_WRITE_STATIC_ERROR_BIT_FIELD_REQ_DATA_T  tData;
} __HIL_PACKED_POST EPLCN_IF_NMT_WRITE_STATIC_ERROR_BIT_FIELD_REQ_T;

typedef __HIL_PACKED_PRE struct EPLCN_IF_NMT_WRITE_STATIC_ERROR_BIT_FIELD_CNF_Ttag
{
  HIL_PACKET_HEADER_T                                   tHead;
} __HIL_PACKED_POST EPLCN_IF_NMT_WRITE_STATIC_ERROR_BIT_FIELD_CNF_T;

/*************************************************************************************/
/* Packet: EPLCN_IF_SET_CONFIG_REQ                                                   */
/*************************************************************************************/
/* System flags */
#define MSK_EPLCN_IF_CFG_SYSTEM_FLAGS_APP_CONTROLLED                        0x00000001

/* Stack configuration flags */
#define MSK_EPLCN_IF_CFG_STACK_CFG_FLAGS_NMT_TRIGGERED_BY_APP               0x00000001
#define MSK_EPLCN_IF_CFG_STACK_CFG_FLAGS_DISABLE_HOST_TRIGGERED_PREQ_XCHG   0x00000002  //Only for LFW application
#define MSK_EPLCN_IF_CFG_STACK_CFG_FLAGS_USE_CUSTOM_PDO_OBJ                 0x00000004
#define MSK_EPLCN_IF_CFG_STACK_CFG_FLAGS_DISABLE_PDO_MAP_VERS_CHECK         0x00000008
#define MSK_EPLCN_IF_CFG_STACK_CFG_FLAGS_USE_APP_READY_FOR_PRES_RD_FLAG     0x00000010  //Only for LFW application
#define MSK_EPLCN_IF_CFG_STACK_CFG_FLAGS_ENABLE_PDO_STATUS                  0x00000020  //Only for LFW application
#define MSK_EPLCN_IF_CFG_STACK_CFG_FLAGS_SUPPORT_CROSS_TRAFFIC              0x00000040

/* Custom threshold for loss of frames detection */
#define MSK_EPLCN_IF_CFG_USE_CUSTOM_TH_LOSS_SOC_THRESHOLD                   0x00000001
#define MSK_EPLCN_IF_CFG_USE_CUSTOM_TH_LOSS_PREQ_THRESHOLD                  0x00000002
#define MSK_EPLCN_IF_CFG_USE_CUSTOM_TH_LOSS_SOA_THRESHOLD                   0x00000004
#define MSK_EPLCN_IF_CFG_USE_CUSTOM_TH_COLLISION_THRESHOLD                  0x00000008
#define MSK_EPLCN_IF_CFG_USE_CUSTOM_TH_CRC_ERROR_THRESHOLD                  0x00000010
#define MSK_EPLCN_IF_CFG_USE_CUSTOM_TH_SOC_JITTER_THRESHOLD                 0x00000020

typedef __HIL_PACKED_PRE struct EPLCN_IF_SET_CONFIG_REQ_DATA_Ttag
{
  /** Only the bit MSK_EPLCN_IF_CFG_SYSTEM_FLAGS_APP_CONTROLLED is used */
  uint32_t      ulSystemFlags;
  /** DPM watchdog time in millisecs */
  uint32_t      ulWatchdogTime;

  /** Stack configuration flags (see EPLCN_IF_CFG_STACK_CFG_FLAGS_*) */
  uint32_t      ulStackCfgFlags;

  /** Vendor Id */
  uint32_t      ulVendorId;
  /** Product code */
  uint32_t      ulProductCode;
  /** Revision number */
  uint32_t      ulRevisionNumber;
  /** Serial number (0 will use default serial number from Sec Mem or Flash Device Label if available) */
  uint32_t      ulSerialNumber;
  /** Cycle Length configuration (us) */
  uint32_t      ulCycleLength;
  /** Device type */
  uint32_t      ulDeviceType;

  /* Powerlink device feature flags */
  uint32_t      ulFeatureFlags;

  /** Startup Poll Request data size (range 0 to 1490) */
  uint16_t      usPReqDataSize;
  /** Startup Poll Response data size (range 0 to 1490) */
  uint16_t      usPResDataSize;
  /** PReq Mapping Version */
  uint8_t       bPReqMappingVersion;
  /** PRes Mapping Version */
  uint8_t       bPResMappingVersion;
  /** Max supported PReq data size (range 0 to 1490).
   * This should be equal or bigger than usPReqDataSize */
  uint16_t      usMaxPReqDataSize;
  /** Max supported PRes data size (range 0 to 1490).
   * This should be equal or bigger than usPResDataSize */
  uint16_t      usMaxPResDataSize;

  /** EPL node id (range 1 to 239) */
  uint8_t       bNodeId;
  /** IP Gateway (if set to 0: default address is configured 192.168.100.254) */
  uint32_t      ulGatewayAddress;
  /** EPL node DNS Hostname (if abNodeName[0] is set to 0, the stack configures the Hostname with the format <nodeId>-<vendorId>) */
  uint8_t       abNodeName[32];

  /** configured status entries (range: 0 .. 13) */
  uint8_t       bNumberOfStatusEntries;

  /** if the bit MSK_EPLCN_IF_CFG_USE_CUSTOM_TH_* is set, the value of ulThreshold* is used.
   * Otherwise the thresholds are configured with their default values */
  uint8_t       bUseCustomThreshold;
  /** LossSoC Threshold (disabled if set to 0) */
  uint32_t      ulThresholdLossSoC;
  /** LossPReq Threshold (disabled if set to 0) */
  uint32_t      ulThresholdLossPReq;
  /** LossSoA Threshold  (disabled if set to 0) */
  uint32_t      ulThresholdLossSoA;
  /** Collision Threshold (disabled if set to 0) */
  uint32_t      ulThresholdCollision;
  /** CrcError Threshold (disabled if set to 0) */
  uint32_t      ulThresholdCrcError;

  /** Minimum cycle length supported by the device (us).
   * Internal defined values:
   * -- netX100/netX500: 200us
   * -- netX51/52: 200us
   * If the parameter is set to 0, it will be ignored and the stack will be configured with the internal defined minimum cycle length */
  uint32_t      ulMinCycleLength;

  /** if the bit MSK_EPLCN_IF_CFG_USE_CUSTOM_TH_* is set, the value of ulThreshold* is used.
   * Otherwise the thresholds are configured with their default values */
  uint32_t      ulThresholdSoCJitter;

  /** Reserved (for future purpose). Set to zero */
  uint32_t      aulReserved[8];
} __HIL_PACKED_POST EPLCN_IF_SET_CONFIG_REQ_DATA_T;

typedef __HIL_PACKED_PRE struct EPLCN_IF_SET_CONFIG_REQ_Ttag
{
  HIL_PACKET_HEADER_T             tHead;
  EPLCN_IF_SET_CONFIG_REQ_DATA_T  tData;
} __HIL_PACKED_POST EPLCN_IF_SET_CONFIG_REQ_T;

typedef __HIL_PACKED_PRE struct EPLCN_IF_SET_CONFIG_CNF_Ttag
{
  HIL_PACKET_HEADER_T             tHead;
} __HIL_PACKED_POST EPLCN_IF_SET_CONFIG_CNF_T;

/*************************************************************************************/
/* Packet: EPLCN_IF_SET_PDO_SIZE_REQ                                                 */
/*************************************************************************************/
typedef __HIL_PACKED_PRE struct EPLCN_IF_SET_PDO_SIZE_REQ_DATA_Ttag
{
  /** Poll Request data size (range 0 to 1490) */
  uint16_t      usPReqDataSize;
  /** Poll Response data size (range 0 to 1490) */
  uint16_t      usPResDataSize;
  /** PReq Mapping Version */
  uint8_t       bPReqMappingVersion;
  /** PRes Mapping Version */
  uint8_t       bPResMappingVersion;
} __HIL_PACKED_POST EPLCN_IF_SET_PDO_SIZE_REQ_DATA_T;

typedef __HIL_PACKED_PRE struct EPLCN_IF_SET_PDO_SIZE_REQ_Ttag
{
  HIL_PACKET_HEADER_T               tHead;
  EPLCN_IF_SET_PDO_SIZE_REQ_DATA_T  tData;
} __HIL_PACKED_POST EPLCN_IF_SET_PDO_SIZE_REQ_T;

typedef __HIL_PACKED_PRE struct EPLCN_IF_SET_PDO_SIZE_CNF_Ttag
{
  HIL_PACKET_HEADER_T               tHead;
} __HIL_PACKED_POST EPLCN_IF_SET_PDO_SIZE_CNF_T;

/*************************************************************************************/
/* Packet: EPLCN_IF_SET_NODE_ID_REQ                                                  */
/*************************************************************************************/
typedef __HIL_PACKED_PRE struct EPLCN_IF_SET_NODE_ID_REQ_DATA_Ttag
{
  /** NodeId. The new value will be activated while in NMT state EPL_NMT_GS_RESET_CONFIGURATION */
  uint8_t   bNodeId;
} __HIL_PACKED_POST EPLCN_IF_SET_NODE_ID_REQ_DATA_T;

typedef __HIL_PACKED_PRE struct EPLCN_IF_SET_NODE_ID_REQ_Ttag
{
  HIL_PACKET_HEADER_T               tHead;
  EPLCN_IF_SET_NODE_ID_REQ_DATA_T   tData;
} __HIL_PACKED_POST EPLCN_IF_SET_NODE_ID_REQ_T;

typedef __HIL_PACKED_PRE struct EPLCN_IF_SET_NODE_ID_CNF_Ttag
{
  HIL_PACKET_HEADER_T               tHead;
} __HIL_PACKED_POST EPLCN_IF_SET_NODE_ID_CNF_T;

/*************************************************************************************/
/* Packet: EPLCN_IF_CROSS_TRAFFIC_CONFIGURED_IND                                     */
/*************************************************************************************/
#define EPLCN_IF_CROSS_TRAFFIC_CFG_OFFSET_INVALID     0xFFFF
#define EPLCN_IF_CROSS_TRAFFIC_MAX_SUPPORTED_NODES    8
typedef __HIL_PACKED_PRE struct EPLCN_IF_CROSS_TRAFFIC_CFG_DATA_Ttag
{
  uint8_t   bNodeId;
  uint8_t   abReserved[3];
  uint16_t  usDataSize;
  uint16_t  usDpmOffset; /* if the node PRes could not be mapped, the offset will be set to EPLCN_IF_CROSS_TRAFFIC_CFG_OFFSET_INVALID.
                          * However, in this case the value of usDataSize will still showing the real PRes length*/
} __HIL_PACKED_POST EPLCN_IF_CROSS_TRAFFIC_CFG_DATA_T;

typedef __HIL_PACKED_PRE struct EPLCN_IF_CROSS_TRAFFIC_CONFIGURED_IND_DATA_Ttag
{
  EPLCN_IF_CROSS_TRAFFIC_CFG_DATA_T atConfig[EPLCN_IF_CROSS_TRAFFIC_MAX_SUPPORTED_NODES];
} __HIL_PACKED_POST EPLCN_IF_CROSS_TRAFFIC_CONFIGURED_IND_DATA_T;

typedef __HIL_PACKED_PRE struct EPLCN_IF_CROSS_TRAFFIC_CONFIGURED_IND_Ttag
{
  HIL_PACKET_HEADER_T                           tHead;
  EPLCN_IF_CROSS_TRAFFIC_CONFIGURED_IND_DATA_T  tData;
} __HIL_PACKED_POST EPLCN_IF_CROSS_TRAFFIC_CONFIGURED_IND_T;

typedef __HIL_PACKED_PRE struct EPLCN_IF_CROSS_TRAFFIC_CONFIGURED_RES_Ttag
{
  HIL_PACKET_HEADER_T               tHead;
} __HIL_PACKED_POST EPLCN_IF_CROSS_TRAFFIC_CONFIGURED_RES_T;

/*************************************************************************************/
/* Pragma unpack */
#ifdef PRAGMA_PACK_ENABLE
  #pragma PRAGMA_UNPACK_1(EPLCN_PACKET_API)
#endif

#endif /* __EPLCN_PACKETAPIC_H */
/*************************************************************************************/
/*-----------------------------------------------------------------------------------*/
/*------------------- EOF -----------------------------------------------------------*/
/*-----------------------------------------------------------------------------------*/
/*************************************************************************************/
