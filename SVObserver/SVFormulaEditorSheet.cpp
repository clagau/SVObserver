//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFormulaEditorSheet
//* .File Name       : $Workfile:   SVFormulaEditorSheet.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:39:18  $
//******************************************************************************

//******************************************************************************
//* INCLUDE(S):
//******************************************************************************

////////////////////////////////////////////////////////////////////////////////
// General Include File(s)
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

////////////////////////////////////////////////////////////////////////////////
// Other Necessary Include File(s) - Module Link(s)
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Prototyping
////////////////////////////////////////////////////////////////////////////////

#include "SVFormulaEditorSheet.h"

//******************************************************************************
//* DEFINITIONS OF MODULE-LOCAL VARIABLES:
//******************************************************************************

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//******************************************************************************
//* CLASS METHOD IMPLEMENTATION(S):
//******************************************************************************

//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
//* Class Name : SVFormulaEditorSheetClass
//* Note(s)    : Property Sheet
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/


SVFormulaEditorSheetClass::SVFormulaEditorSheetClass(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	init();
}

SVFormulaEditorSheetClass::SVFormulaEditorSheetClass(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	init();
}

SVFormulaEditorSheetClass::~SVFormulaEditorSheetClass()
{
}

void SVFormulaEditorSheetClass::init()
{
	m_psh.dwFlags |= PSH_NOAPPLYNOW;
	AddPage( &formulaPage );
}

void SVFormulaEditorSheetClass::SetTaskObject( SVTaskObjectClass* PObject )
{
	formulaPage.SetTaskObject( PObject );
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
BOOL SVFormulaEditorSheetClass::OnInitDialog() 
{
	BOOL bResult = CPropertySheet::OnInitDialog();

	// Disable and Hide Cancel Button
	HWND hWnd = ::GetDlgItem(m_hWnd, IDCANCEL);
	if (hWnd != NULL)
	{
		::EnableWindow( hWnd, FALSE );
		::ShowWindow(hWnd, SW_HIDE );
	}

	// Remove Close Button
	ModifyStyle( WS_SYSMENU, 0, SWP_FRAMECHANGED );

	return bResult;
}


BEGIN_MESSAGE_MAP(SVFormulaEditorSheetClass, CPropertySheet)
	//{{AFX_MSG_MAP(SVFormulaEditorSheetClass)
		ON_COMMAND(IDOK,OnOK)
	ON_WM_SYSCOMMAND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVFormulaEditorSheetClass message handlers

void SVFormulaEditorSheetClass::OnOK()
{
	// Try to validate the Equations
	int cnt = GetPageCount();

	for( int i = 0;i < cnt; i++ )
	{
		CPropertyPage* pPage = GetPage(i);
		if( pPage && pPage->GetSafeHwnd() ) 
		{
			if( SV_IS_KIND_OF( pPage, SVFormulaEditorPageClass ) )
			{
				if( !( ( SVFormulaEditorPageClass* )pPage)->ValidateEquation() )
				{
					// Equation must be valid or disabled
					CString tmp;
					tmp.LoadString(IDS_INVALID_FORMULA);
					AfxMessageBox(tmp);
					return;
				}
			}
		}
	}

	EndDialog( IDOK );
}

void SVFormulaEditorSheetClass::OnSysCommand(UINT nID, LPARAM lParam) 
{
	switch (nID & 0xFFF0)
	{
		// Ignore the close command
		case SC_CLOSE:
			return;
	}

	CPropertySheet::OnSysCommand(nID, lParam);
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVFormulaEditorSheet.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:39:18   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   09 Apr 2009 14:58:34   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  657
 * SCR Title:  Remove Apply button from the Tool Adjust Dialogs
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   removed the apply button
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   22 Apr 2003 09:45:34   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   29 Feb 2000 14:28:38   sjones
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