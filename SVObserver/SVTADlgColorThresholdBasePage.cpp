//******************************************************************************
//* COPYRIGHT (c) 2006 by SVResearch, Harrisburg
//* All Rights Reserved
//*****************************************************************************
//* .Module Name     : SVTADlgColorThresholdBasePage
//* .File Name       : $Workfile:   SVTADlgColorThresholdBasePage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:19:44  $
//******************************************************************************

#include "stdafx.h"
#include "SVTADlgColorThresholdBasePage.h"
#include "SVOMFCLibrary/SVOMFCLibraryGlobals.h"
#include "SVTADlgColorThresholdSheet.h"
#include "SVOMFCLibrary/stringmunge.h"

IMPLEMENT_DYNCREATE(SVTADlgColorThresholdBasePage, CPropertyPage)

BEGIN_MESSAGE_MAP(SVTADlgColorThresholdBasePage, CPropertyPage)
	//{{AFX_MSG_MAP(SVTADlgColorThresholdBasePage)
	ON_CBN_SELCHANGE(IDC_COMBO_ZOOM, OnSelchangeComboZoom)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

SVTADlgColorThresholdBasePage::SVTADlgColorThresholdBasePage(UINT nIDTemplate) : CPropertyPage(nIDTemplate)
{
	//{{AFX_DATA_INIT(SVTADlgColorThresholdBasePage)
	//}}AFX_DATA_INIT
	m_pSheet = NULL;
	mpTool = NULL;
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
	
	mpTool = m_pSheet->GetTool();
	
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
	int iZoom = m_cbZoom.GetItemData( m_cbZoom.GetCurSel() );
	//CString sText;
	//m_cbZoom.GetWindowText( sText );
	//StringMunge::KeepChars(&sText, _T("0123456789"));
	//double dPercent = atof(sText);
	double dZoom = iZoom / 100.0;
	m_svDlgImage.SetZoom( dZoom );
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVTADlgColorThresholdBasePage.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 15:19:44   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   27 Jan 2011 12:02:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   16 Dec 2009 13:12:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Aug 2006 15:59:50   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  574
 * SCR Title:  Fix Color Tool so it can properly handle images larger than 640x480
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   initial check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
