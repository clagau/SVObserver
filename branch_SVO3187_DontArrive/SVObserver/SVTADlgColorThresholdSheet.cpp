//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgColorThresholdSheet
//* .File Name       : $Workfile:   SVTADlgColorThresholdSheet.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:21:00  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "Tools/SVColorTool.h"
#include "SVTADlgColorThresholdSheet.h"
#include "SVROIFigureEditor.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

constexpr const char* thresholdHsiNames[] {_T("Hue Threshold Adjustment"), _T("Saturation Threshold Adjustment"), _T("Intensity Threshold Adjustment")};
constexpr const char* thresholdRgbNames[] {_T("Red Threshold Adjustment"), _T("Green Threshold Adjustment"), _T("Blue Threshold Adjustment")};
#pragma endregion Declarations

SVTADlgColorThresholdSheet::SVTADlgColorThresholdSheet(SvTo::SVColorToolClass* pTool, UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
	, m_pTool{pTool}
{
	AddTabs();
}

SVTADlgColorThresholdSheet::SVTADlgColorThresholdSheet(SvTo::SVColorToolClass* pTool, LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
	, m_pTool{pTool}
{
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
	mSheetOutput.m_pSheet = this;

	AddPage( &mSheetROI );

	for (SvDef::BandEnum Band : SvDef::BandList)
	{
		mSheetBand[Band].m_pSheet = this;
		mSheetBand[Band].setBand(Band);
		mSheetBand[Band].m_psp.pszTitle = bIsHSI ? thresholdHsiNames[Band] : thresholdRgbNames[Band];
		mSheetBand[Band].m_psp.dwFlags |= PSP_USETITLE;
		AddPage(&mSheetBand[Band]);
	}

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
		std::string Text = _T("Tool Adjustment: ");
		Text += m_pTool->GetName();

		SetWindowText( Text.c_str() );
		
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

