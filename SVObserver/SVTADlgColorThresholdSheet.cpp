//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgColorThresholdSheet
//* .File Name       : $Workfile:   SVTADlgColorThresholdSheet.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:21:00  $
//******************************************************************************

#include "stdafx.h"
#include "SVColorTool.h"
#include "SVTADlgColorThresholdSheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVTADlgColorThresholdSheet

IMPLEMENT_DYNAMIC(SVTADlgColorThresholdSheet, CPropertySheet)

SVTADlgColorThresholdSheet::SVTADlgColorThresholdSheet(SVColorToolClass* pPTool, UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	m_pTool = pPTool;

	AddTabs();
}

SVTADlgColorThresholdSheet::SVTADlgColorThresholdSheet(SVColorToolClass* pPTool, LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	m_pTool = pPTool;
	m_pFigureEditor = new SVROIFigureEditor;
	m_pFigureEditor->SetOverlayColor( RGB(255,255,255), RGB(0,0,0) );

	AddTabs();
}

SVTADlgColorThresholdSheet::~SVTADlgColorThresholdSheet()
{
	delete m_pFigureEditor;
}


BEGIN_MESSAGE_MAP(SVTADlgColorThresholdSheet, CPropertySheet)
	//{{AFX_MSG_MAP(SVTADlgColorThresholdSheet)
		ON_WM_SYSCOMMAND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVTADlgColorThresholdSheet message handlers

void SVTADlgColorThresholdSheet::AddTabs()
{
	BOOL bIsHSI = FALSE;

	// init shared data
	m_iZoom = 0;	// fit to window
	m_ptOrigin = CPoint(0,0);

	if (m_pTool)
	{
		m_pTool->GetConvertToHSIVariable()->GetValue( bIsHSI );
	}

	mSheetROI.m_pSheet = this;
	mSheetBand0.m_pSheet = this;
	mSheetBand1.m_pSheet = this;
	mSheetBand2.m_pSheet = this;
	mSheetOutput.m_pSheet = this;

	mSheetBand0.m_psp.dwFlags |= PSP_USETITLE;
	mSheetBand1.m_psp.dwFlags |= PSP_USETITLE;
	mSheetBand2.m_psp.dwFlags |= PSP_USETITLE;

	if ( bIsHSI )
	{
		mSheetBand0.m_psp.pszTitle = _T("Hue Threshold Adjustment");
		mSheetBand1.m_psp.pszTitle = _T("Saturation Threshold Adjustment");
		mSheetBand2.m_psp.pszTitle = _T("Intensity Threshold Adjustment");
	}
	else
	{
		mSheetBand0.m_psp.pszTitle = _T("Red Threshold Adjustment");
		mSheetBand1.m_psp.pszTitle = _T("Green Threshold Adjustment");
		mSheetBand2.m_psp.pszTitle = _T("Blue Threshold Adjustment");
	}

	mSheetBand0.mBandNumber = 0;
	mSheetBand1.mBandNumber = 1;
	mSheetBand2.mBandNumber = 2;

	AddPage( &mSheetROI );	
	AddPage( &mSheetBand0 );	
	AddPage( &mSheetBand1 );	
	AddPage( &mSheetBand2 );	
	AddPage( &mSheetOutput );	
}

BOOL SVTADlgColorThresholdSheet::OnInitDialog() 
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

	if( m_pTool )
	{
		CString l_Temp = _T( "Tool Adjustment: " );
		l_Temp += m_pTool->GetName();

		SetWindowText( l_Temp );
		
		return bResult;
	}

	SendMessage( WM_CLOSE );

	return FALSE;
}

void SVTADlgColorThresholdSheet::OnSysCommand(UINT nID, LPARAM lParam) 
{
	switch (nID & 0xFFF0)
	{
		// Ignore the close command
		case SC_CLOSE:
			return;
	}

	CPropertySheet::OnSysCommand(nID, lParam);
}

