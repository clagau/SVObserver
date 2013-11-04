//******************************************************************************
//* COPYRIGHT (c) 2008 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVASABtcpMicroLogixAPI
//* .File Name       : $Workfile:   SVASABtcpMicroLogixAPI.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 15:46:04  $
//******************************************************************************

#ifndef SVASABTCPMICROLOGIXAPICLASS_H
#define SVASABTCPMICROLOGIXAPICLASS_H

#include "plc_interface.h"
#include "SVASABtcpClass.h"

class SV_PLCAPI_DS SVASABtcpMicroLogixAPI :
	public SVASABtcpClass
{
public:
	SVASABtcpMicroLogixAPI( long* aplErr );
	virtual long SetupPLC( LPCTSTR p_strNodeAddress, long p_lConnectTimeOut );
	virtual ~SVASABtcpMicroLogixAPI(void);
};

#endif //#ifndef SVASABTCPMICROLOGIXAPICLASS_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVP_SRC\SVASABtcpMicroLogixAPI.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 15:46:04   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/