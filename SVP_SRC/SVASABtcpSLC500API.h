//******************************************************************************
//* COPYRIGHT (c) 2008 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVASABtcpSLC500API
//* .File Name       : $Workfile:   SVASABtcpSLC500API.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 15:47:42  $
//******************************************************************************

#ifndef SVASABTCPSLC500API_H
#define SVASABTCPSLC500API_H

#include "plc_interface.h"
#include "svasabtcpclass.h"

class SV_PLCAPI_DS SVASABtcpSLC500API :
	public SVASABtcpClass
{
public:
	SVASABtcpSLC500API(long* aplErr );
	virtual ~SVASABtcpSLC500API(void);
};

#endif //#ifndef SVASABTCPSLC500API_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVP_SRC\SVASABtcpSLC500API.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 15:47:42   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/