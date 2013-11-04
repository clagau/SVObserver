// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVPLC
// * .File Name       : $Workfile:   SVPLC5APIClass.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 16:02:22  $
// ******************************************************************************

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef SVPLC5APICLASS_INCLUDED
#define SVPLC5APICLASS_INCLUDED

#include <DTL.h>
#include "SVRSLinxABPLCBaseClass.h"


class SV_PLCAPI_DS SVPLC5APIClass 
: public SVRSLinxABPLCBaseClass
{
public:
	SVPLC5APIClass(long* aplErr);
	~SVPLC5APIClass();

	long Open(TCHAR *apConnectionString);
	long InitializeBlocks(SVPLCBlockStruct* apBlock);
};

#endif /* SVPLC5APICLASS_INCLUDED */

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVP_SRC\SVPLC5APIClass.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 16:02:22   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   14 Nov 2008 14:58:42   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Feb 2002 16:56:36   jBrown
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  141
 * SCR Title:  Add support for Allen-Bradley PLC5 via the KT card (Data Highway)
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   Needed to expose the SVPLCAPIClass.
 * 
 * Changed
 * class SVPLC5APIClass 
 * to
 * class SV_PLCAPI_DS SVPLC5APIClass 
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Feb 2002 16:56:20   jBrown
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  141
 * SCR Title:  Add support for Allen-Bradley PLC5 via the KT card (Data Highway)
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   Needed to expose the SVPLCAPIClass.
 * 
 * Changed
 * class SVPLC5APIClass 
 * to
 * class SV_PLCAPI_DS SVPLC5APIClass 
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2001 13:21:44   jBrown
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  141
 * SCR Title:  Add support for Allen-Bradley PLC5 via the KT card (Data Highway)
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   The SVPLC5APIClass class was added.  This derived class represents the PLC5 series PLCs as connected via DataHighway plus.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/