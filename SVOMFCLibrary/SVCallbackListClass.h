// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVCallbackListClass
// * .File Name       : $Workfile:   SVCallbackListClass.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 13:00:28  $
// ******************************************************************************

#ifndef SVCALLBACKLISTCLASS_H
#define SVCALLBACKLISTCLASS_H

#include "SVContainerLibrary/SVList.h"
#include "SVCallbackStruct.h"

class SVCallbackListClass : public SVList< SVCallbackStruct >
{
public:
	SVCallbackListClass();
	virtual ~SVCallbackListClass();

	CString m_csName;

	bool m_bIsStarted;
};

#endif

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVOMFCLibrary\SVCallbackListClass.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 13:00:28   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   09 Nov 2010 16:01:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate container objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   08 Jul 2003 13:13:26   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322, 320
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   New files needed to handle callback functionality and parameter functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
