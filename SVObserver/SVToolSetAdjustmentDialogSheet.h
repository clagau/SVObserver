//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolSetAdjustmentDialogSheet
//* .File Name       : $Workfile:   SVToolSetAdjustmentDialogSheet.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:47:24  $
//******************************************************************************

//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************

#ifndef SVTOOLSETADJUSTMENTDIALOGSHEET_H
#define SVTOOLSETADJUSTMENTDIALOGSHEET_H


//******************************************************************************
//* INCLUDE(S):
//******************************************************************************

//{{AFX_INCLUDES()
//}}AFX_INCLUDES

#include "SVConditionalDialog.h"


//******************************************************************************
//* FUNCTION DECLARATION(S):
//******************************************************************************


////////////////////////////////////////////////////////////////////////////////
// .Title       : Class SVToolSetAdjustmentDialogSheetClass
// -----------------------------------------------------------------------------
// .Description :  
//              :
//              :
// -----------------------------------------------------------------------------
// .Export
//	 Public Method				Description
//  :
//  :
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :dd.mm.yyyy				First Implementation
//	:
////////////////////////////////////////////////////////////////////////////////
class SVToolSetAdjustmentDialogSheetClass : public CPropertySheet
{

//******************************************************************************
// MFC Declarations:
//******************************************************************************
	DECLARE_DYNAMIC(SVToolSetAdjustmentDialogSheetClass)

//******************************************************************************
// Class Wizard Generated Message Map Entries:
//******************************************************************************
	// Generated message map functions
protected:
	//{{AFX_MSG(SVToolSetAdjustmentDialogSheetClass)
	afx_msg void OnDestroy();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

//******************************************************************************
// Friend Declaration(s):
//******************************************************************************

//******************************************************************************
// Constructor(s):
//******************************************************************************
public:
// Standard constructor
	SVToolSetAdjustmentDialogSheetClass( SVConditionalClass* PCondition, UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	SVToolSetAdjustmentDialogSheetClass( SVConditionalClass* PCondition, LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

//******************************************************************************
// Destructor(s):
//******************************************************************************
public:
// Standard destructor
	virtual ~SVToolSetAdjustmentDialogSheetClass();

//******************************************************************************
// Operator(s):
//******************************************************************************
protected:
	virtual void init( SVConditionalClass* PCondition );

//******************************************************************************
// Operation(s) Of Writing Access And Data Exchange:
//******************************************************************************
protected:
	void addPages();

protected:
    afx_msg void OnOK();
    afx_msg void OnCancel();

//******************************************************************************
// Class Wizard Generated Virtual Function(s):
//******************************************************************************
	//{{AFX_VIRTUAL(SVToolSetAdjustmentDialogSheetClass)
	public:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

//******************************************************************************
// Class Wizard Generated Dialog Data:
//******************************************************************************
	//{{AFX_DATA(<ClassName>)
	//}}AFX_DATA

//******************************************************************************
// Data Element(s):
//******************************************************************************
public:
	SVConditionalDialogClass	ConditionalDlg;

private:
	SVConditionalClass* pCondition;
	SVToolSetClass* pToolSet;
};

////////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// DevStudio inserts additional declarations immediate in front of the preceding line
////////////////////////////////////////////////////////////////////////////////


//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************
#endif	//	SVTOOLSETADJUSTMENTDIALOGSHEET_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVToolSetAdjustmentDialogSheet.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 15:47:24   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   09 Aug 2005 08:49:26   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   22 Apr 2003 16:55:58   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 13:50:58   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   31 Aug 1999 23:39:34   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  16
 * SCR Title:  Integrate Math Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial CheckIn.
 * New Class for ToolSet Adjustment Property sheet.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/