//******************************************************************************
/// COPYRIGHT (c) by Hilscher GmbH
/// All Rights Reserved 
/// \file CifxCommonData.h
//******************************************************************************	
#pragma once

#ifdef __cplusplus
  extern "C" {
#endif  /* _cplusplus */

#include "TLR_Types.h"

struct CIFX_PACKETtag;

/*************************************************************************************************
 * @brief This structure is part of all application data.
 * It contains all common data structures independent of the stack or bus.
 */
 /* default timeout for driver api calls */
constexpr TLR_UINT32 c_DriverTimeout_ms = 100;


struct CommonData
{
	CommonData() = default;
	~CommonData() = default;

	CommonData& CommonData::operator=(const CommonData&) = default;
	CommonData(const CommonData&) = default;
	
	/* handle from driver api */
  TLR_HANDLE m_hDriver = nullptr;
    
  /* channel handle from driver api */
  TLR_HANDLE m_hChannel = nullptr;

  /* receive buffers */
  TLR_UINT8* m_pReadBuffer;
  TLR_UINT32 m_readBufferSize;

  /* transmit buffers */
  TLR_UINT8* m_pWriteBuffer;
  TLR_UINT32 m_writeBufferSize;

  TLR_RESULT SendRecvPkt(CIFX_PACKETtag* ptSendPkt, CIFX_PACKETtag* ptRecvPkt);
  TLR_RESULT SendRecvEmptyPkt(TLR_UINT32 ulCmd);

  /* initialize / finalize application */
  TLR_RESULT App_Initialize(TLR_UINT16 DataLength);
  TLR_RESULT App_Finalize();
};


/*************************************************************************************************
 * Below you will find some common functions, 
 * they are implemented by Application.h and can be used by any application.
 */


/*************************************************************************************************
 * Below you will find some common interface functions, 
 * which must be implemented by all applications.
 */

/* configuration */
TLR_RESULT BuildConfigurationReq(CIFX_PACKETtag* ptPacket, TLR_UINT8 NodeId, TLR_UINT16 DataLength);

/*************************************************************************************************/

#ifdef __cplusplus
  }
#endif  /* _cplusplus */
