//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFormulaEditorSheet
//* .File Name       : $Workfile:   SVFormulaEditorSheet.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:39:28  $
//******************************************************************************

//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************

#ifndef SVFORMULAEDITORSHEET_H
#define SVFORMULAEDITORSHEET_H

//******************************************************************************
//* INCLUDE(S):
//******************************************************************************

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SVFormulaEditorPage.h"

class SVTaskObjectClass;


//******************************************************************************
//* CLASS(ES) & TYPE(S):
//* STRUCTURE(S) & TYPE(S):
//* UNION(S) & TYPE(S):
//******************************************************************************

class SVFormulaEditorSheetClass : public CPropertySheet
{

//******************************************************************************
// Friend Declaration(s):
//******************************************************************************

//******************************************************************************
// Constructor(s):
//******************************************************************************
// Construction
public:
	SVFormulaEditorSheetClass(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	SVFormulaEditorSheetClass(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

//******************************************************************************
// Destructor(s):
//******************************************************************************
public:
	virtual ~SVFormulaEditorSheetClass();

//******************************************************************************
// Operator(s):
//******************************************************************************
public:
	void SetTaskObject( SVTaskObjectClass* PObject );

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVFormulaEditorSheetClass)
	public:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

protected:
	void init();

	//{{AFX_MSG(SVFormulaEditorSheetClass)
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
    afx_msg void OnOK();

//******************************************************************************
// Data Element(s):
//******************************************************************************
protected:

	SVFormulaEditorPageClass	formulaPage;

};



//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************
#endif	// SVFORMULAEDITORSHEET_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVFormulaEditorSheet.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:39:28   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   22 Apr 2003 09:45:36   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   29 Feb 2000 14:28:34   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  106
 * SCR Title:  Math Tool - List processing
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Removed Close Button on caption.
 * Added Handler for WM_SYSCOMMAND to restrict SC_CLOSE message.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 11:39:20   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   08 Nov 1999 11:00:24   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  16
 * SCR Title:  Integrate Math Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to use IDS_INVALID_FORMULA (string table).
 * Added OnOK handler to prevent exiting dialog with an
 * invalid formula.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   29 Sep 1999 13:12:26   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added OnInitDilaog Method.
 * Disabled and Hid IDCANCEL button.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   Sep 14 1999 15:17:10   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  34
 * SCR Title:  Add Build Reference Tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Provides Standard Sheet for Formula Editor Page.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/