//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPQVariableTreeCtrl
//* .File Name       : $Workfile:   SVPQVariableTreeCtrl.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 14:34:28  $
//******************************************************************************

#if !defined(AFX_SVPQVariableTreeCtrl_H__A4C49D6B_24DA_11d4_A8F2_00106F030D16__INCLUDED_)
#define AFX_SVPQVariableTreeCtrl_H__A4C49D6B_24DA_11d4_A8F2_00106F030D16__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SVMFCControls/SVTreeCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// SVPQVariableTreeCtrlClass window

class SVPQVariableTreeCtrlClass : public SvMc::SVTreeCtrlClass
{
	DECLARE_DYNCREATE(SVPQVariableTreeCtrlClass)

// Construction
public:
	SVPQVariableTreeCtrlClass();

// Attributes
	DWORD uRequiredAttributes;

protected:

// Operations
public:
	BOOL UpdateAttributes();

protected:
	
	void WalkTheTree( HTREEITEM hItem );
	void ProcessItemAttributes( HTREEITEM hItem );

protected:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVPQVariableTreeCtrlClass)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~SVPQVariableTreeCtrlClass();

	// Generated message map functions
protected:
	//{{AFX_MSG(SVPQVariableTreeCtrlClass)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVPQVariableTreeCtrl_H__A4C49D6B_24DA_11d4_A8F2_00106F030D16__INCLUDED_)

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVPQVariableTreeCtrl.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 14:34:28   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   22 Apr 2003 14:07:46   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   20 Nov 2002 11:01:32   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  272
 * SCR Title:  Outputs do not always get set correctly.
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed to accommodate new architecture
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   17 May 2000 17:40:54   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  141
 * SCR Title:  View DDE Inputs/Outputs on Results View
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added UpdateAttrinbutes method.
 * Added WalkTheTree method.
 * Added ProcessItemAttributes method.
 * Added uRequiredAttributes variable.
 * Added SVPQDataLinkInfoListClass* variable.
 * Revised PopulateTree method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   11 May 2000 09:33:32   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  141
 * SCR Title:  View DDE Inputs/Outputs on Results View
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial checkin.
 * Implements a tree control to display/select PPQ data items
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/