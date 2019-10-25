//******************************************************************************
/// COPYRIGHT (c) by Hilscher GmbH
/// All Rights Reserved 
/// \file CifxCommonData.cpp
//******************************************************************************	
/*************************************************************************************************
 *
 *   Copyright (c) Hilscher GmbH. All Rights Reserved.
 *
 **************************************************************************************************
 * @file [Application.cpp]
 *
 * @brief This file contains some common functions which might be used by any example application. 
 * 
 * @author Elmar
 *
 */

#pragma region Includes
#include "stdafx.h"
#include "CifxCommonData.h"

#include "cifXErrors.h"
//Arvid: the following are the Powerlink V3 headers (which were not used in the original version of the file)
#include "EplCn_If_Public.h"
#include "Epl_Common_Defines.h"
#include "TLR_Results.h"
#pragma endregion Includes

 /* Identity Information */
#define VENDOR_ID           0x00000044 /* Hilscher Vendor ID */
#define PRODUCT_CODE        0x00000001 /* CIFX */
#define SERIAL_NUMBER       0x00000000 /* Use serial number from SecMem or FDL */
#define REVISION_NUMBER     0x00000000

#define RCX_REGISTER_APP_REQ                    0x00002F10
#define RCX_UNREGISTER_APP_REQ					0x00002F12


/*************************************************************************************************
 * @brief This function sends a packet and waits for the confirmation.
 * Packets which will come up meanwhile will be passed to the standard packet handler.
 * 
 * @param rCommon   Application data.
 * @param ptSendPkt Packet which will be sent.
 * @param ptRecvPkt Packet which the function has received.
 *
 * @return TLR_RESULT Returns TLR_S_OK if no error, otherwise it will return an error code.
 *
 */
TLR_RESULT CommonData::SendRecvPkt(CIFX_PACKET* ptSendPkt, CIFX_PACKET* ptRecvPkt)
{	
  /* fire the packet */
  TLR_RESULT tResult = xChannelPutPacket(m_hChannel, ptSendPkt, c_DriverTimeout_ms);
  if (TLR_S_OK != tResult)
  {
    return tResult;
  }

  /* ok, at this point we have successfully sent a packet */

  /* check for packets to receive */
  /* the ulCnfCmd is always: ulReqCmd | 0x01 */
  TLR_UINT32 ulCnfCmd = ptSendPkt->tHeader.ulCmd | 0x01;  
  
  while(CIFX_NO_ERROR == (tResult = xChannelGetPacket(m_hChannel, sizeof(*ptRecvPkt), ptRecvPkt, c_DriverTimeout_ms)))
  {
    /* check for our packet */
    if(ptRecvPkt->tHeader.ulCmd == ulCnfCmd)
    {
      /* it is our packet, so return its status as result */
      tResult = ptRecvPkt->tHeader.ulState;
      
      /* Note: we also return the packet which we have received (by reference, see signature of function) */

      /* we have received our packet, so we can break here*/
      break;
    }
    else
    {
      /* it is something else, so place it in the application packet handler */
	//AB here HandlePacket() used to be called
	//AB which, however, requires the old (V2) Powerlink-defines and is unnecessary for our communication with the PLC
    }
  }

  return tResult;
}


/*************************************************************************************************
 * @brief This function sends and receives an empty packet.
 * 
 * @param ptApp Pointer to application data.
 * @param ulCmd Command code which will be inserted into packet header.
 *
 * @return TLR_RESULT Returns TLR_S_OK if no error, otherwise it will return an error code.
 *
 */
TLR_RESULT CommonData::SendRecvEmptyPkt(TLR_UINT32 ulCmd)
{
  
  CIFX_PACKET tSendPkt = {{0}};
  CIFX_PACKET tRecvPkt = {{0}};

  /* all register packets are empty packet, */
  /* which means that they contain no data, */
  /* so a cast to a specific type is not necessary */
  TLR_EMPTY_PACKET_T* ptReq = (TLR_EMPTY_PACKET_T*) &tSendPkt;

  /* ulSrc and ulDest are set by GetPacket / SendPacket */
  ptReq->tHead.ulCmd    = ulCmd;
  ptReq->tHead.ulDest   = 0x20;
  ptReq->tHead.ulDestId = 0;
  ptReq->tHead.ulExt    = 0;
  ptReq->tHead.ulId     = 0;
  ptReq->tHead.ulLen    = 0;
  ptReq->tHead.ulRout   = 0;
  ptReq->tHead.ulSrc    = 0;
  ptReq->tHead.ulSrcId  = 0;
  ptReq->tHead.ulSta    = 0;

  return SendRecvPkt(&tSendPkt, &tRecvPkt);
}


/*************************************************************************************************
* @brief This method builds a configuration request packet.
*
* @param CIFX_PACKET* ptPacket pointer to a CIFX_PACKET structure.
*
* @return TLR_RESULT Returns TLR_S_OK if no error, otherwise it will return an error code.
*
* //Arvid Modifiziert gemäß neuer Version von Herrn Azarkan 170804
*************************************************************************************************/
TLR_RESULT BuildConfigurationReq(CIFX_PACKET* ptPacket, TLR_UINT8 NodeId, TLR_UINT16 DataLength)
{
	uint32_t lRet = CIFX_NO_ERROR;

	EPLCN_IF_SET_CONFIG_REQ_T* ptConfigReq = (EPLCN_IF_SET_CONFIG_REQ_T*)ptPacket;

	ptConfigReq->tHead.ulDest = 0x20;                         /* Destination of packet                    */
	ptConfigReq->tHead.ulSrc = 0x10;                         /* Source of packet, process queue          */// not always necessary
	ptConfigReq->tHead.ulLen = sizeof(ptConfigReq->tData);   /* Length of packet data without header     */
	ptConfigReq->tHead.ulCmd = EPLCN_IF_SET_CONFIG_REQ;
	ptConfigReq->tHead.ulSta = 0;                            /* Status code of operation                 */
	ptConfigReq->tHead.ulExt = 0;                            /* extension                                */

	ptConfigReq->tData.ulSystemFlags = 0;                            /* System Flags                             */
	ptConfigReq->tData.ulWatchdogTime = 1000;                         /* Watchdog time                            */
	ptConfigReq->tData.ulStackCfgFlags = MSK_EPLCN_IF_CFG_STACK_CFG_FLAGS_DISABLE_HOST_TRIGGERED_PREQ_XCHG; /* Stack creates default PDO objects */
	//Arvid: MSK_EPLCN_IF_CFG_STACK_CFG_FLAGS_DISABLE_HOST_TRIGGERED_PREQ_XCHG: Damit wird die Aktualisierung des DualPort-Memorys von netX angestoßen,
	//Arvid: Lesevorgänge mit xChannelIORead werden dann immer bei ankommenden Telegrammen abgeschlossen.
	//Arvid: Synchronisation ist damit möglich, z.B. direkt nach dem abgeschlossenen Lesevorgang, aber auch mit CIFX_NOTIFY_PD0_IN - Callback.*/
																											/* Configuration Flags                      */
	ptConfigReq->tData.ulVendorId = VENDOR_ID;                    /* Hilscher Vendor ID */
	ptConfigReq->tData.ulProductCode = PRODUCT_CODE;                 /* Product code of CifX example application */
	ptConfigReq->tData.ulRevisionNumber = REVISION_NUMBER;              /* Revision number                          */
	ptConfigReq->tData.ulSerialNumber = SERIAL_NUMBER;                /* Serial number                            */
	ptConfigReq->tData.ulCycleLength = 1000;                         /* Cycle Length configuration (ns)          */
	ptConfigReq->tData.ulDeviceType = 0;                            /* No standard profile */
	ptConfigReq->tData.ulFeatureFlags = 0;                            /* Simple configuration. Stack will set the common supported features */
	ptConfigReq->tData.usPReqDataSize = DataLength;                   /* Data length Master->Slave */
	ptConfigReq->tData.usPResDataSize = DataLength;                   /* Data length Slave->Master */
	ptConfigReq->tData.bPReqMappingVersion = 0;                               /* PReq Mapping Version                     */
	ptConfigReq->tData.bPResMappingVersion = 0;                               /* PRes Mapping Version                     */
	ptConfigReq->tData.usMaxPReqDataSize = EPL_C_DLL_ISOCHR_MAX_PAYL;    /* >= ptConfigReq->tData.usPReqDataSize */
	ptConfigReq->tData.usMaxPResDataSize = EPL_C_DLL_ISOCHR_MAX_PAYL;    /* >= ptConfigReq->tData.usPResDataSize */
	ptConfigReq->tData.bNodeId = NodeId;                       /* EPL node id (range 1 to 239)             */
	ptConfigReq->tData.ulGatewayAddress = 0;                            /* Stack configures default value (192.168.100.254) */
	memset(ptConfigReq->tData.abNodeName, 0, 32);                             /* Stack configures the Host Name in form <nodeId>-<vendorId> */
	ptConfigReq->tData.bNumberOfStatusEntries = 0;                            /* configured status entries                */

	ptConfigReq->tData.bUseCustomThreshold = 0;                            /* Use default Thresholds */
	ptConfigReq->tData.ulThresholdCollision = 0;                            /* Collision Threshold (ignored if set to 0)*/
	ptConfigReq->tData.ulThresholdLossSoC = 0;                            /* LossSoC Threshold   (ignored if set to 0)*/
	ptConfigReq->tData.ulThresholdLossSoA = 0;                            /* LossSoA Threshold   (ignored if set to 0)*/
	ptConfigReq->tData.ulThresholdLossPReq = 0;                            /* LossPReq Threshold  (ignored if set to 0)*/
	ptConfigReq->tData.ulThresholdSoCJitter = 0;                            /* SoCJitter Threshold (ignored if set to 0)*/
	ptConfigReq->tData.ulThresholdCrcError = 0;                            /* CrcError Threshold  (ignored if set to 0)*/
	ptConfigReq->tData.ulMinCycleLength = 0;                            /* Use minimum cycle length supported by the hardware */
	ptConfigReq->tData.ulThresholdSoCJitter = 0;                            /* Not evaluated, since we use default thresholds */

																			/* Set the reserved block to 0 to ensure the compatibility with future versions of the stack */
	memset(ptConfigReq->tData.aulReserved, 0, sizeof(ptConfigReq->tData.aulReserved));


	return lRet;
}


/*************************************************************************************************
* @brief This function initializes the application.
* It allocates memory for the application data and returns a pointer to it.
* App_Finalize() must be called in order to achieve a friendly shutdown and to release memory again.
*
* @param pptApp    Pointer to Pointer to application data.
* @param hDriver   Driver Handle.
* @param hChannel  Channel Handle.
* @param ulTimeout Standard timeout for packet operations.
*
* @return TLR_RESULT Returns TLR_S_OK if no error, otherwise it will return an error code.
*
*/
TLR_RESULT CommonData::App_Initialize(TLR_UINT16 ulMaxRtdSize)
{
	/* allocate memory for read / write buffers */

	m_readBufferSize = ulMaxRtdSize * sizeof(TLR_UINT8);
	m_writeBufferSize = ulMaxRtdSize * sizeof(TLR_UINT8);

	m_pReadBuffer = new TLR_UINT8[m_readBufferSize];
	m_pWriteBuffer = new TLR_UINT8[m_writeBufferSize];

	/* initialize the read and write buffer with zero */
	memset(m_pReadBuffer, 0, m_readBufferSize);
	memset(m_pWriteBuffer, 0, m_writeBufferSize);

	/* register the application at the stack */
	TLR_RESULT tResult = SendRecvEmptyPkt(RCX_REGISTER_APP_REQ);
	if (TLR_S_OK != tResult)
	{
		return tResult;
	}

	return tResult;
}



/*************************************************************************************************
* @brief This method finalizes the EPL application.
* It returns handles, frees memory, aso.
*
* @return TLR_RESULT Returns TLR_S_OK if no error, otherwise it will return an error code.
*
*/
TLR_RESULT CommonData::App_Finalize()
{
	/* unregister on EcatEsm task (indications for status) */
	TLR_RESULT tResult = SendRecvEmptyPkt(RCX_UNREGISTER_APP_REQ);
	if (TLR_S_OK != tResult)
	{
		return tResult;
	}

	// delete buffers
	delete []m_pReadBuffer;
	delete []m_pWriteBuffer;

	return tResult;
}
