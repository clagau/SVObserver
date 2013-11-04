//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageListClass
//* .File Name       : $Workfile:   SVImageListClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:53:08  $
//******************************************************************************

#include "stdafx.h"
#include "SVImageListClass.h"

SVImageListClass::SVImageListClass()
{
}

SVImageListClass::~SVImageListClass()
{
	RemoveAll();
}

BOOL SVImageListClass::RemoveImage( SVImageClass* PImage )
{
	AfxMessageBox( "BOOL SVImageListClass::RemoveImage( SVImageClass* PImage );\n\nDo not use this function!\nOut of use!" );
	return FALSE;
}

SVImageClass * SVImageListClass::GetAtByTool(int nIndex)   // 25 Jun 1999 - frb.
{
    SVImageClass * pImage = GetAt(nIndex);
    
	ASSERT(pImage);

    //
	// So we don't try to use this image that has been passed to a tool.
	//
	SetAt(nIndex,NULL);

#ifdef _DEBUG
    SVImageClass * pImageTest = GetAt(nIndex);
	ASSERT(pImageTest == NULL);
#endif //_DEBUG

	return pImage;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVImageListClass.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:53:08   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   09 Nov 2010 16:21:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate container objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   27 Oct 2010 09:32:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix include issues and comment issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   22 Apr 2003 10:19:12   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   28 Jan 2003 15:04:08   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Moved existing source code to new source files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

