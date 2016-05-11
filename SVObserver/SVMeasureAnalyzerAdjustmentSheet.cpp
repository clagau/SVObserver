//*****************************************************************************
/// \copyright (c) 2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// Adjustment Sheet for SVMeasureAnalyzer
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVMeasureAnalyzerAdjustmentSheet.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
// Saved location of dialog.
//
static CPoint gPtPosition(0,0);

IMPLEMENT_DYNAMIC(SVMeasureAnalyzerAdjustmentSheetClass, CPropertySheet)

SVMeasureAnalyzerAdjustmentSheetClass::SVMeasureAnalyzerAdjustmentSheetClass(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	m_psh.dwFlags |= PSH_NOAPPLYNOW;
}

SVMeasureAnalyzerAdjustmentSheetClass::SVMeasureAnalyzerAdjustmentSheetClass(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	m_psh.dwFlags |= PSH_NOAPPLYNOW;
}

SVMeasureAnalyzerAdjustmentSheetClass::~SVMeasureAnalyzerAdjustmentSheetClass()
{
}

BEGIN_MESSAGE_MAP(SVMeasureAnalyzerAdjustmentSheetClass, CPropertySheet)
	//{{AFX_MSG_MAP(SVMeasureAnalyzerAdjustmentSheetClass)
	ON_WM_DESTROY()
	ON_WM_SYSCOMMAND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten SVMeasureAnalyzerAdjustmentSheetClass 


BOOL SVMeasureAnalyzerAdjustmentSheetClass::OnInitDialog() 
{
	BOOL bResult = CPropertySheet::OnInitDialog();
	
	// TODO: Add your specialized code here
    //
	// Disable and Hide Cancel Button
	HWND hWnd = ::GetDlgItem(m_hWnd, IDCANCEL);
	if (nullptr != hWnd)
	{
		::EnableWindow( hWnd, FALSE );
		::ShowWindow(hWnd, SW_HIDE );
	}

	// Remove Close Button
	ModifyStyle( WS_SYSMENU, 0, SWP_FRAMECHANGED );

    // Position this dialog at saved (or initial) location.
    //
    BOOL bResult2 = SetWindowPos(
        nullptr,          // Z order not used
        gPtPosition.x,
        gPtPosition.y,
        0,0,           // Width and Height Not Used
        SWP_NOSIZE | SWP_NOZORDER | SWP_NOREDRAW 
    );
	
	return bResult;
}

void SVMeasureAnalyzerAdjustmentSheetClass::OnDestroy() 
{
	CPropertySheet::OnDestroy();

    //
    // Save the current location of the property sheet.
    //
    CRect rectWindow;
    GetWindowRect(&rectWindow);
    gPtPosition.x = rectWindow.left;
    gPtPosition.y = rectWindow.top;
    	
	// TODO: Add your message handler code here
	
}

void SVMeasureAnalyzerAdjustmentSheetClass::OnSysCommand(UINT nID, LPARAM lParam) 
{
	switch (nID & 0xFFF0)
	{
		// Ignore the close command
		case SC_CLOSE:
			return;
	}

	CPropertySheet::OnSysCommand(nID, lParam);
}

