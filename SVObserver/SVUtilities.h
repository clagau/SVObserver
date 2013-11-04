//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVUtilitiesClass
//* .File Name       : $Workfile:   SVUtilities.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:57:10  $
//******************************************************************************

#ifndef SVUTILITIES_H
#define SVUTILITIES_H

#include <deque>
#include "SVSecurity/SVSecurityManager.h"

class SVUtilitiesClass  
{
public:
	CMenu *FindSubMenuByName(CMenu *pMenu, CString &szName);
	BOOL LoadMenu (CMenu *pMenu);
	BOOL SetupUtilities (CMenu *pMenu);
	void RunUtility (SVSecurityManager* pAccess, UINT uiUtilityId);
	SVUtilitiesClass();
	virtual ~SVUtilitiesClass();
	BOOL LoadMenuFromINI(CMenu *pMenu);

	BOOL CleanupIni();

	BOOL UpdateIni();
private:

	BOOL ClearMenu (CMenu *pMenu);

};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVUtilities.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 15:57:10   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   16 Dec 2009 13:28:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   02 Mar 2007 06:27:40   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  595
 * SCR Title:  Fix issues with Utility Menu when switching products
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed the utilities to use the ini files instead of the registry
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   21 Jun 2005 13:16:54   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified RunUtility to use SVSecurity create process.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   22 Apr 2003 17:18:36   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 */
