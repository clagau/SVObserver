//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVActiveXLockStruct
//* .File Name       : $Workfile:   SVActiveXLockStruct.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:26:42  $
//******************************************************************************

#include "stdafx.h"
#include "SVActiveXLockStruct.h"

SVActiveXLockStruct::SVActiveXLockStruct()
: strName( _T( "" ) ), lProcessCount( -1 ), m_ImageHandlePtr()
{
}

SVActiveXLockStruct::SVActiveXLockStruct( const SVActiveXLockStruct &rvalue )
: strName( rvalue.strName ), lProcessCount( rvalue.lProcessCount ), m_ImageHandlePtr( rvalue.m_ImageHandlePtr )
{
}

SVActiveXLockStruct::~SVActiveXLockStruct()
{
	clear();
}

const SVActiveXLockStruct& SVActiveXLockStruct::operator =(const SVActiveXLockStruct &rvalue)
{
	if( this != &rvalue )
	{
		strName = rvalue.strName;
		lProcessCount = rvalue.lProcessCount;
		m_ImageHandlePtr = rvalue.m_ImageHandlePtr;
	}

	return *this;
}

void SVActiveXLockStruct::clear()
{
	strName.Empty();
	lProcessCount = -1;
	m_ImageHandlePtr.clear();
}

BOOL SVActiveXLockStruct::Valid()
{
	if ( strName.IsEmpty() || m_ImageHandlePtr.empty() || m_ImageHandlePtr->empty() )
		return FALSE;
	else
		return TRUE;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVActiveXLockStruct.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 09:26:42   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   10 Feb 2011 14:24:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   27 Jan 2011 10:47:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   24 Jul 2007 10:49:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Update to Matrox Imaging Library 8.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   17 Apr 2003 16:25:38   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   04 Feb 2003 16:05:52   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  309
 * SCR Title:  Implement the ability to lock acquired or processed image from the SIAC.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated all methods to change the way the image lock will be maintained.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   29 Jan 2003 10:22:42   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
