//******************************************************************************
//* COPYRIGHT (c) 2006 by SVResearch, Harrisburg
//* All Rights Reserved
//*****************************************************************************
//* .Module Name     : SVTADlgColorThresholdBasePage
//* .File Name       : $Workfile:   SVTADlgColorThresholdBasePage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   02 Oct 2013 08:17:48  $
//******************************************************************************

#include "stdafx.h"
#include "SVTADlgColorThresholdBasePage.h"
#include "SVLibrary/StringHelper.h"
#include "SVTADlgColorThresholdSheet.h"
#include "SVLibrary/Stringmunge.h"

IMPLEMENT_DYNCREATE(SVTADlgColorThresholdBasePage, CPropertyPage)

BEGIN_MESSAGE_MAP(SVTADlgColorThresholdBasePage, CPropertyPage)
	//{{AFX_MSG_MAP(SVTADlgColorThresholdBasePage)
	ON_CBN_SELCHANGE(IDC_COMBO_ZOOM, OnSelchangeComboZoom)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

SVTADlgColorThresholdBasePage::SVTADlgColorThresholdBasePage(UINT nIDTemplate) 
: CPropertyPage(nIDTemplate)
, m_pSheet(nullptr)
, m_pTool(nullptr)
, m_pThreshold(nullptr)
, m_pExtentLeft(nullptr)
, m_pExtentTop(nullptr)
, m_pExtentWidth(nullptr)
, m_pExtentHeight(nullptr)
{
	//{{AFX_DATA_INIT(SVTADlgColorThresholdBasePage)
	//}}AFX_DATA_INIT
	
}

SVTADlgColorThresholdBasePage::~SVTADlgColorThresholdBasePage()
{
}

void SVTADlgColorThresholdBasePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVTADlgColorThresholdBasePage)
	DDX_Control(pDX, IDC_COMBO_ZOOM, m_cbZoom);
	//}}AFX_DATA_MAP
	DDX_CBIndex(pDX, IDC_COMBO_ZOOM, m_pSheet->m_iZoom);
	DDX_Control(pDX, IDC_COLOR_THRESHOLD_IMAGE, m_svDlgImage);
}


/////////////////////////////////////////////////////////////////////////////
// SVTADlgColorThresholdBasePage message handlers

BOOL SVTADlgColorThresholdBasePage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	ASSERT( m_pSheet );

	m_svDlgImage.SetFigureEditor( m_pSheet->m_pFigureEditor );
	
	m_pTool = m_pSheet->GetTool();
	
	m_cbZoom.SetItemData( m_cbZoom.AddString(_T("Fit to Window")), 0 );
	m_cbZoom.SetItemData( m_cbZoom.AddString(AsString(100)+_T("%")), 100 );
	m_cbZoom.SetItemData( m_cbZoom.AddString(AsString(200)+_T("%")), 200 );
	m_cbZoom.SetItemData( m_cbZoom.AddString(AsString(400)+_T("%")), 400 );
	m_cbZoom.SetCurSel(0);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL SVTADlgColorThresholdBasePage::OnKillActive() 
{
	UpdateData(TRUE);

	m_pSheet->m_ptOrigin = m_svDlgImage.GetViewportOrigin();
	
	return CPropertyPage::OnKillActive();
}

BOOL SVTADlgColorThresholdBasePage::OnSetActive() 
{
	UpdateData(FALSE);

	double dZoom = m_cbZoom.GetItemData( m_pSheet->m_iZoom ) / 100.0;
	m_svDlgImage.SetZoom( dZoom );
	m_svDlgImage.SetViewportOrigin( m_pSheet->m_ptOrigin, true );
	
	return CPropertyPage::OnSetActive();
}

void SVTADlgColorThresholdBasePage::OnSelchangeComboZoom() 
{
	UpdateData();
	int iZoom = static_cast<int>(m_cbZoom.GetItemData( m_cbZoom.GetCurSel() ));

	double dZoom = iZoom / 100.0;
	m_svDlgImage.SetZoom( dZoom );
}

