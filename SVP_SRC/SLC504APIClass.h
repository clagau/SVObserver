// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVSLC504APIClass
// * .File Name       : $Workfile:   SLC504APIClass.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 15:36:44  $
// ******************************************************************************

#ifndef SVSLC504APIClass_H_INCLUDED
#define SVSLC504APIClass_H_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif

#include <DTL.h>
#include "SVRSLinxABPLCBaseClass.h"

struct SVPLCElementStruct;

class SV_PLCAPI_DS SVSLC504APIClass : public SVRSLinxABPLCBaseClass  
{
public:
	SVSLC504APIClass(long* aplErr);
	~SVSLC504APIClass();

// Open () ------------------------------------------------------------------
// apConnectionString is the input parameter which will indicate all 
// connection parameters.  The format of the input string will be;
// "DriverName, StationNumber" where 
// Driver Name indicates the name of the 
//   connection as referenced by RSLinx.  
// Station Number will reference the station.

   long Open (TCHAR *apConnectionString);
   long InitializeBlocks (SVPLCBlockStruct* apBlock);
};

#endif // #ifndef SVSLC504APIClass_H_INCLUDED

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVP_SRC\SLC504APIClass.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 15:36:44   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   14 Nov 2008 14:56:10   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   14 Nov 2001 09:58:32   jBrown
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  131, 141
 * SCR Title:  Add support for Allen-Bradley Micrologix PLC's via the Ethernet
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   The name of the base class from which SVSLC504APIClass was derived was changed from SVSLC500BaseClass to SVABPLCBaseClass.
 * 
 * The following functions were modified in order to support Allen-Bradley MicroLogix and Allen-Bradley PLC5 series PLCs:
 * 
 * SVSLC504APIClass::SVSLC504APIClass (), and
 * SVSLC504APIClass::Open ()
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   12 Sep 2000 13:52:40   Jim
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  14
 * SCR Title:  Stage 2: Expand PLC Support
 * Checked in by:  Jim;  James A. Brown
 * Change Description:  
 *   Extensive changes to support integration.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   Mar 22 2000 17:32:10   mike
 * Initial revision.
*/