//******************************************************************************
//* COPYRIGHT (c) 2008 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVASABtcpClass
//* .File Name       : $Workfile:   SVASABtcpClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 15:45:30  $
//******************************************************************************

#ifndef SVASABTCPCLASS_H
#define SVASABTCPCLASS_H

#if _MSC_VER > 1000
#pragma once
#endif

#include "SVABPLCBaseClass.h"
#include "SVASABtcpPLCControl.h"

class SV_PLCAPI_DS SVASABtcpClass : public SVABPLCBaseClass  
{
public:
	SVASABtcpClass(long* aplErr);
	virtual ~SVASABtcpClass();

	// Open () ------------------------------------------------------------------
	// aConnectionString is the input parameter which will indicate 
	// the tcp address of the plc.
	long Open (TCHAR *aConnectionString);

	// The IP address of the PLC + additional parameters see ASAB control doc for NodeAddress.
	virtual long SetupPLC( LPCTSTR p_strNodeAddress, long p_lConnectTimeOut );
	long Close ();

	long CheckStatus (SVPLCBlockStruct* apBlock);

	long DataTypeSize (SVPLCDataTypeEnum aDataType);
	long SendB (SVPLCBlockStruct* apBlock);
	long SendInt16 (SVPLCBlockStruct* apBlock);
	long SendInt32 (SVPLCBlockStruct* apBlock);
	long SendStr (SVPLCBlockStruct* apBlock);
	long SendSPFloat (SVPLCBlockStruct* apBlock);
	long GetB (SVPLCBlockStruct* apBlock);
	long GetInt16 (SVPLCBlockStruct* apBlock);
	long GetInt32 (SVPLCBlockStruct* apBlock);
	long GetStr (SVPLCBlockStruct* apBlock);
	long GetSPFloat (SVPLCBlockStruct* apBlock);

	long CopyStringToSendBuffer (SVPLCBlockStruct* apBlock);
	long TestCommunications();

	long   InitializeBlocks (SVPLCBlockStruct* apBlock);
	long   DeinitializeBlocks (SVPLCBlockStruct* apBlock);

private:
	SVASABtcpPLCControl m_ASABtcpControl;
};

#endif // #ifndef SVASABTCPCLASS_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVP_SRC\SVASABtcpClass.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 15:45:30   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/