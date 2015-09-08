//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolSetAdjustmentDialog
//* .File Name       : $Workfile:   SVToolSetAdjustmentDialogSheet.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   14 Jan 2014 12:32:42  $
//******************************************************************************

#include "stdafx.h"
#include "SVToolSetAdjustmentDialogSheet.h"
#include "FormulaController.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(SVToolSetAdjustmentDialogSheetClass, CPropertySheet)

//******************************************************************************
// Message Map Entries
//******************************************************************************
BEGIN_MESSAGE_MAP(SVToolSetAdjustmentDialogSheetClass, CPropertySheet)
	//{{AFX_MSG_MAP(SVToolSetAdjustmentDialogSheetClass)
	ON_WM_DESTROY()
	ON_COMMAND(IDOK,OnOK)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

SVToolSetAdjustmentDialogSheetClass::SVToolSetAdjustmentDialogSheetClass( const GUID& rInspectionID, const GUID& rTaskObjectID, SVConditionalClass& rCondition, UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage )
: CPropertySheet( nIDCaption, pParentWnd, iSelectPage )
, m_InspectionID(rInspectionID)
, m_TaskObjectID(rTaskObjectID)
, m_pToolSet(nullptr)
{
	init( rCondition );
}

SVToolSetAdjustmentDialogSheetClass::SVToolSetAdjustmentDialogSheetClass( const GUID& rInspectionID, const GUID& rTaskObjectID, SVConditionalClass& rCondition, LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage )
: CPropertySheet( pszCaption, pParentWnd, iSelectPage )
, m_InspectionID(rInspectionID)
, m_TaskObjectID(rTaskObjectID)
, m_pToolSet(nullptr)
{
	init( rCondition );
}

SVToolSetAdjustmentDialogSheetClass::~SVToolSetAdjustmentDialogSheetClass()
{
}

void SVToolSetAdjustmentDialogSheetClass::init( SVConditionalClass& rCondition )
{
	m_formulaPage = FormulaEditorPagePtr(new SVFormulaEditorPageClass(m_InspectionID, m_TaskObjectID, 
		new FormulaController(m_InspectionID, m_TaskObjectID, SVObjectTypeInfoStruct(SVEquationObjectType, SVConditionalObjectType), false),
								true, IDS_CONDITIONAL_STRING, IDS_CLASSNAME_SVTOOLSET));

	addPages();
}

void SVToolSetAdjustmentDialogSheetClass::addPages()
{
	AddPage( m_formulaPage.get() );
}

BOOL SVToolSetAdjustmentDialogSheetClass::OnInitDialog() 
{
	BOOL bResult = CPropertySheet::OnInitDialog();

	// Disable and Hide Cancel Button
	HWND hWnd = ::GetDlgItem(m_hWnd, IDCANCEL);
	if (hWnd != nullptr)
	{
		::EnableWindow( hWnd, FALSE );
		::ShowWindow(hWnd, SW_HIDE );
	}

	// Remove Close Button
	ModifyStyle( WS_SYSMENU, 0, SWP_FRAMECHANGED );

	CString l_Temp = _T( "ToolSet Adjustment: " );
	l_Temp += m_formulaPage->GetOwnerName();

	SetWindowText( l_Temp );
		
	return bResult;
}

void SVToolSetAdjustmentDialogSheetClass::OnDestroy() 
{
	CPropertySheet::OnDestroy();
}

void SVToolSetAdjustmentDialogSheetClass::OnOK()
{
	//SEJ99 - refactor...
	// Try to validate the Conditional
	if( m_formulaPage->validateAndSetEquation() )
	{
		m_formulaPage->SetDefaultInputs();
		EndDialog( IDOK );
	}
	else
	{
		// Equation must be valid or disabled
		CString tmp;
		tmp.LoadString(IDS_INVALID_CONDITION_FORMULA);
		AfxMessageBox(tmp);
	}
}

void SVToolSetAdjustmentDialogSheetClass::OnCancel() 
{
	EndDialog( IDCANCEL );
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVToolSetAdjustmentDialogSheet.cpp_v  $
 * 
 *    Rev 1.1   14 Jan 2014 12:32:42   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  877
 * SCR Title:  Add undo-button to formula and conditional pages
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Changed to use FormulaController instead of SVConditionalDialog.
 * Changed parameters from pointers to references.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 15:47:10   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   08 Dec 2010 13:45:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   05 Feb 2009 16:18:30   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  649
 * SCR Title:  Update Equation Dialog to disable the PPQ Variable Edit/Selection Field
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to remove close (X) button
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   09 Aug 2005 08:49:28   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   22 Apr 2003 16:55:56   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   19 Apr 2001 20:00:34   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen E. Steffan
 * Change Description:  
 *   Header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 13:51:44   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   08 Nov 1999 10:57:20   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  16
 * SCR Title:  Integrate Math Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to use IDS_INVALID_FORMULA (string table)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   Oct 28 1999 11:38:14   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  52
 * SCR Title:  Version 3.00 Beta 14 Versioning
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   Oct 28 1999 11:30:34   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  52
 * SCR Title:  Version 3.00 Beta 14 Versioning
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Adjustment to tool/toolset conditional dialog.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   29 Sep 1999 13:14:48   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Disabled and Hid IDCANCEL button.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   24 Sep 1999 19:04:30   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Removed call to RemoveExternalDependencies
 * (Fix to restore inputs via script)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   14 Sep 1999 17:06:26   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  16
 * SCR Title:  Integrate Math Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added equation validation in OnOk method
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   31 Aug 1999 23:39:36   sjones
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
