//*****************************************************************************
/// \copyright (c) 2016 by Körber Pharma Inspection GmbH. All Rights Reserved
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

SVMeasureAnalyzerAdjustmentSheet::SVMeasureAnalyzerAdjustmentSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	m_psh.dwFlags |= PSH_NOAPPLYNOW;
}

SVMeasureAnalyzerAdjustmentSheet::SVMeasureAnalyzerAdjustmentSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	m_psh.dwFlags |= PSH_NOAPPLYNOW;
}

SVMeasureAnalyzerAdjustmentSheet::~SVMeasureAnalyzerAdjustmentSheet()
{
}

BEGIN_MESSAGE_MAP(SVMeasureAnalyzerAdjustmentSheet, CPropertySheet)
	//{{AFX_MSG_MAP(SVMeasureAnalyzerAdjustmentSheet)
	ON_WM_DESTROY()
	ON_WM_SYSCOMMAND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten SVMeasureAnalyzerAdjustmentSheet 


BOOL SVMeasureAnalyzerAdjustmentSheet::OnInitDialog() 
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
    SetWindowPos(nullptr, gPtPosition.x, gPtPosition.y, 0,0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOREDRAW);
	
	return bResult;
}

void SVMeasureAnalyzerAdjustmentSheet::OnDestroy() 
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

void SVMeasureAnalyzerAdjustmentSheet::OnSysCommand(UINT nID, LPARAM lParam) 
{
	switch (nID & 0xFFF0)
	{
		// Ignore the close command
		case SC_CLOSE:
			return;
	}

	CPropertySheet::OnSysCommand(nID, lParam);
}

