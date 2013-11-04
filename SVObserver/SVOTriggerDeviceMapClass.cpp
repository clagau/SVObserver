//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOTriggerDeviceMapClass
//* .File Name       : $Workfile:   SVOTriggerDeviceMapClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:10:12  $
//******************************************************************************

#include "stdafx.h"
#include "SVOTriggerDeviceMapClass.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVOTriggerDeviceMapClass::SVOTriggerDeviceMapClass()
{
}

SVOTriggerDeviceMapClass::~SVOTriggerDeviceMapClass()
{
}

BOOL SVOTriggerDeviceMapClass::GetDeviceList(CStringList &rDeviceList)
{
	BOOL bOk = TRUE;

	POSITION pos;
	CString csName;
	void *pItem = NULL;

	rDeviceList.RemoveAll();

  for( pos = GetStartPosition(); bOk && pos != NULL; GetNextAssoc( pos, csName, pItem ) )
	{
		bOk = ! csName.IsEmpty();

		if ( bOk )
		{
			rDeviceList.AddTail( csName );
		}

		csName.Empty();
	}

	if ( ! bOk )
	{
		rDeviceList.RemoveAll();
	}

	return bOk;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVOTriggerDeviceMapClass.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 13:10:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   22 Apr 2003 13:27:36   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 13:39:26   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   This is the first revision of this file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/