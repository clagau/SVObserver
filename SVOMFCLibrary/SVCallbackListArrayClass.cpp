// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVCallbackListArrayClass
// * .File Name       : $Workfile:   SVCallbackListArrayClass.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 12:59:40  $
// ******************************************************************************

#include "stdafx.h"
#include "SVCallbackListArrayClass.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVCallbackListArrayClass::SVCallbackListArrayClass()
: SVVector< SVCallbackListClass >()
{
	m_bIsStarted = false;
}

SVCallbackListArrayClass::~SVCallbackListArrayClass()
{
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVOMFCLibrary\SVCallbackListArrayClass.cpp_v  $
 * 
 *    Rev 1.0   25 Apr 2013 12:59:40   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   09 Nov 2010 16:01:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate container objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   08 Jul 2003 13:13:58   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322, 320
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   New files needed to handle callback functionality and parameter functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
