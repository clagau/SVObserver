//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPatAnalyzeSetupDlgSheet
//* .File Name       : $Workfile:   SVPatAnalyzeSetupDlgSheet.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:18:52  $
//******************************************************************************

#include "stdafx.h"
#include "SVPatAnalyzeSetupDlgSheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SVPatAnalyzeSetupDlgSheet::SVPatAnalyzeSetupDlgSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
: CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

SVPatAnalyzeSetupDlgSheet::SVPatAnalyzeSetupDlgSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
: CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
}

SVPatAnalyzeSetupDlgSheet::~SVPatAnalyzeSetupDlgSheet()
{
}

BEGIN_MESSAGE_MAP(SVPatAnalyzeSetupDlgSheet, CPropertySheet)
	//{{AFX_MSG_MAP(SVPatAnalyzeSetupDlgSheet)
	ON_WM_SYSCOMMAND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVPatAnalyzeSetupDlgSheet message handlers

BOOL SVPatAnalyzeSetupDlgSheet::OnInitDialog() 
{
	BOOL bResult = CPropertySheet::OnInitDialog();
	
	// Disable and Hide Cancel Button
	HWND hWnd = ::GetDlgItem(m_hWnd, IDCANCEL);
	if (nullptr != hWnd)
	{
		::EnableWindow( hWnd, FALSE );
		::ShowWindow(hWnd, SW_HIDE );
	}
	// Remove Close Button
	ModifyStyle( WS_SYSMENU, 0, SWP_FRAMECHANGED );
	
	return bResult;
}

void SVPatAnalyzeSetupDlgSheet::OnSysCommand(UINT nID, LPARAM lParam) 
{
	switch (nID & 0xFFF0)
	{
		// Ignore the close command
		case SC_CLOSE:
			return;
	}

	CPropertySheet::OnSysCommand(nID, lParam);
}

