//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolSetAdjustmentDialogSheet
//* .File Name       : $Workfile:   SVToolSetAdjustmentDialogSheet.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   17 Jul 2014 20:51:52  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVUtilityLibrary/SVSharedPtr.h"
#include "SVFormulaEditorPage.h"
#include "SVConditional.h" //SEJ99 - this needs to go
#pragma endregion Includes

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

public:
	SVToolSetAdjustmentDialogSheetClass( const GUID& rInspectionID, const GUID& rTaskObjectID, SVConditionalClass& rCondition, UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0 );
	SVToolSetAdjustmentDialogSheetClass( const GUID& rInspectionID, const GUID& rTaskObjectID, SVConditionalClass& rCondition, LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0 );
	virtual ~SVToolSetAdjustmentDialogSheetClass();

protected:
	virtual void init( SVConditionalClass& rCondition );
	void addPages();

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

private:
	GUID m_InspectionID;
	GUID m_TaskObjectID;

	typedef SVSharedPtr<SVFormulaEditorPageClass> FormulaEditorPagePtr;
	FormulaEditorPagePtr m_formulaPage;
	SVToolSetClass* m_pToolSet; //SEJ99 - this needs to go
};

////////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// DevStudio inserts additional declarations immediate in front of the preceding line
////////////////////////////////////////////////////////////////////////////////

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVToolSetAdjustmentDialogSheet.h_v  $
 * 
 *    Rev 1.2   17 Jul 2014 20:51:52   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Removed namespaces and code review changes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   14 Jan 2014 12:33:20   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  877
 * SCR Title:  Add undo-button to formula and conditional pages
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Changed to use SVFormulaEditorPage instead of SVConditionalDialog.
 * Changed parameters from pointers to references.
 * Changed member variable names to use m_ prefix.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
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