// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMicroLogixAPIClass
// * .File Name       : $Workfile:   SVMicroLogixAPIClass.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 16:01:50  $
// ******************************************************************************

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_SVMICROLOGIXAPICLASS_3B1D4D980399_INCLUDED
#define _INC_SVMICROLOGIXAPICLASS_3B1D4D980399_INCLUDED

#include <DTL.h>
#include "SVRSLinxABPLCBaseClass.h"

class  SV_PLCAPI_DS SVMicroLogixAPIClass : public SVRSLinxABPLCBaseClass
{
public:
	SVMicroLogixAPIClass(long* aplErr);
	~SVMicroLogixAPIClass();

	long Open(TCHAR *apConnectionString);
	long DataTypeSize (SVPLCDataTypeEnum aDataType);
	long InitializeBlocks(SVPLCBlockStruct* apBlock);
};

#endif /* _INC_SVMICROLOGIXAPICLASS_3B1D4D980399_INCLUDED */

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVP_SRC\SVMicroLogixAPIClass.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 16:01:50   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   14 Nov 2008 14:58:40   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2001 15:07:38   jBrown
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  131
 * SCR Title:  Add support for Allen-Bradley Micrologix PLC's via the Ethernet
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   The SVMicroLogixAPIClass class was added.  This derived class represents the MicroLogix series PLCs as connected via ethernet.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/