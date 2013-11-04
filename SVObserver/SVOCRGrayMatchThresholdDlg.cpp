//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOCRGrayMatchThresholdDlg
//* .File Name       : $Workfile:   SVOCRGrayMatchThresholdDlg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:59:26  $
//******************************************************************************

#include "stdafx.h"
#include "SVOCRGrayDialog.h"
#include "SVOCRGrayMatchThresholdDlg.h"
#include "SVOCRGrayAnalyzerResult.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVOCRGrayMatchThresholdDlg property page

IMPLEMENT_DYNCREATE(SVOCRGrayMatchThresholdDlg, CPropertyPage)

SVOCRGrayMatchThresholdDlg::SVOCRGrayMatchThresholdDlg() : CPropertyPage(SVOCRGrayMatchThresholdDlg::IDD)
{
	//pOCRAnalyzerResult = NULL;

	//{{AFX_DATA_INIT(SVOCRGrayMatchThresholdDlg)
	m_acceptThresh = 0.0f;
	m_rejectThresh = 0.0f;
	m_relThresh = 0.0f;
	//}}AFX_DATA_INIT
}

SVOCRGrayMatchThresholdDlg::~SVOCRGrayMatchThresholdDlg()
{
}

void SVOCRGrayMatchThresholdDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVOCRGrayMatchThresholdDlg)
	DDX_Control(pDX, IDC_SLIDER_ACCEPT_THRESHOLD, m_SliderAcceptThreshold);
	DDX_Control(pDX, IDC_SLIDER_RELATIVE_THRESHOLD, m_SliderRelativeThreshold);
	DDX_Control(pDX, IDC_SLIDER_REJECT_THRESHOLD, m_SliderRejectThreshold);
	DDX_Text(pDX, IDC_EDIT_ACCEPT_THRESHOLD, m_acceptThresh);
	DDX_Text(pDX, IDC_EDIT_REJECT_THRESHOLD, m_rejectThresh);
	DDX_Text(pDX, IDC_EDIT_RELATIVE_THRESHOLD, m_relThresh);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SVOCRGrayMatchThresholdDlg, CPropertyPage)
	//{{AFX_MSG_MAP(SVOCRGrayMatchThresholdDlg)
	ON_EN_CHANGE(IDC_EDIT_ACCEPT_THRESHOLD, OnChangeEditAcceptThreshold)
	ON_EN_CHANGE(IDC_EDIT_REJECT_THRESHOLD, OnChangeEditRejectThreshold)
	ON_EN_CHANGE(IDC_EDIT_RELATIVE_THRESHOLD, OnChangeEditRelativeThreshold)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_ACCEPT_THRESHOLD, OnCustomdrawSliderAcceptThreshold)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_REJECT_THRESHOLD, OnCustomdrawSliderRejectThreshold)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_RELATIVE_THRESHOLD, OnCustomdrawSliderRelativeThreshold)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVOCRGrayMatchThresholdDlg message handlers

BOOL SVOCRGrayMatchThresholdDlg::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

    SVOCRGrayDialogClass * pParent = (SVOCRGrayDialogClass *)GetParent();
	ASSERT(pParent);
	SVOCRGrayAnalyzeResultClass * pOCRAnalyzerResult = pParent->pOCRAnalyzerResult;
	SVOCRGrayParamStruct *pOCRParameters = &pOCRAnalyzerResult->GetOCRParameters();
	
	m_acceptThresh = pOCRParameters->acceptThresh;
	m_rejectThresh = pOCRParameters->rejectThresh;
	m_relThresh    = pOCRParameters->relThresh;

	m_SliderAcceptThreshold.SetRange (0, 100);
	m_SliderAcceptThreshold.SetTicFreq (10);
	m_SliderAcceptThreshold.SetLineSize(1);
	m_SliderAcceptThreshold.SetPos ((int)(m_acceptThresh*100.0+0.5));

	m_SliderRejectThreshold.SetRange (0, 100);
	m_SliderRejectThreshold.SetTicFreq (10);
	m_SliderRejectThreshold.SetLineSize(1);
	m_SliderRejectThreshold.SetPos ((int)(m_rejectThresh*100.0+0.5));

	m_SliderRelativeThreshold.SetRange (0, 100);
	m_SliderRelativeThreshold.SetTicFreq (10);
	m_SliderRelativeThreshold.SetLineSize(1);
	m_SliderRelativeThreshold.SetPos ((int)(m_relThresh*100.0+0.5));

	UpdateData (FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void SVOCRGrayMatchThresholdDlg::UpdateOCRParameters ()
{

	int	err;

	err = 0;
    //
    // Make sure the dialog has been 'created' - 14 Apr 1999 - frb.
    //
    if(!GetSafeHwnd())
    {
       return;
    }

	while (1)
	{
	    SVOCRGrayDialogClass * pParent = (SVOCRGrayDialogClass *)GetParent();
		ASSERT(pParent);
		SVOCRGrayAnalyzeResultClass * pOCRAnalyzerResult = pParent->pOCRAnalyzerResult;
		SVOCRGrayParamStruct *pOCRParameters = &pOCRAnalyzerResult->GetOCRParameters();
	
		UpdateData (TRUE);
		
		pOCRParameters->acceptThresh = m_acceptThresh;
		pOCRParameters->rejectThresh = m_rejectThresh;
		pOCRParameters->relThresh    = m_relThresh;
		break;
	}

	return;
}

BOOL SVOCRGrayMatchThresholdDlg::OnSetActive() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CPropertyPage::OnSetActive();
}

BOOL SVOCRGrayMatchThresholdDlg::OnKillActive() 
{
	UpdateOCRParameters();

	return CPropertyPage::OnKillActive();
}

void SVOCRGrayMatchThresholdDlg::OnChangeEditAcceptThreshold() 
{
	UpdateData();
	if (m_acceptThresh < 0.0)
		m_acceptThresh = 0.0;
	if (m_acceptThresh > 1.0)
		m_acceptThresh = 1.0;

	int nPos = (int)(m_acceptThresh*100.0+0.5);
	m_SliderAcceptThreshold.SetPos (nPos);
}

void SVOCRGrayMatchThresholdDlg::OnChangeEditRejectThreshold() 
{
	UpdateData();
	if (m_rejectThresh < 0.0)
		m_rejectThresh = 0.0;
	if (m_rejectThresh > 1.0)
		m_rejectThresh = 1.0;

	int nPos = (int)(m_rejectThresh*100.0+0.5);
	m_SliderRejectThreshold.SetPos (nPos);
}

void SVOCRGrayMatchThresholdDlg::OnChangeEditRelativeThreshold() 
{
	UpdateData();
	if (m_relThresh < 0.0)
		m_relThresh = 0.0;
	if (m_relThresh > 1.0)
		m_relThresh = 1.0;

	int nPos = (int)(m_relThresh*100.0+0.5);
	m_SliderRelativeThreshold.SetPos (nPos);
}

void SVOCRGrayMatchThresholdDlg::OnCustomdrawSliderAcceptThreshold(NMHDR* pNMHDR, LRESULT* pResult) 
{
	UpdateData();
	int nPos = m_SliderAcceptThreshold.GetPos();
	m_acceptThresh = (float)(nPos / 100.0);
	UpdateData(FALSE);
	
	*pResult = 0;
}

void SVOCRGrayMatchThresholdDlg::OnCustomdrawSliderRejectThreshold(NMHDR* pNMHDR, LRESULT* pResult) 
{
	UpdateData();
	int nPos = m_SliderRejectThreshold.GetPos();
	m_rejectThresh = (float)(nPos / 100.0);
	UpdateData(FALSE);
	
	*pResult = 0;
}

void SVOCRGrayMatchThresholdDlg::OnCustomdrawSliderRelativeThreshold(NMHDR* pNMHDR, LRESULT* pResult) 
{
	UpdateData();
	int nPos = m_SliderRelativeThreshold.GetPos();
	m_relThresh = (float)(nPos / 100.0);
	UpdateData(FALSE);
	
	*pResult = 0;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVOCRGrayMatchThresholdDlg.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 12:59:26   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   22 Apr 2003 12:53:16   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   04 Oct 2001 16:52:46   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  229
 * SCR Title:  Upgrade SVObserver to Version 3.34 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Modified source code to remove compiler warning message.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   19 May 2000 10:12:00   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  146
 * SCR Title:  Fix OCR Gray Scale Analyzer Configuration Dialog
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   This change was in response to SCR146.  These updates fix most of the problems with the Gray Scale OCR Dialogs.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 May 2000 14:44:58   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  133, 132
 * SCR Title:  Add OCR Gray Scale to SVObserver and Create OCR Gray Scale Font Training
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Upgrade Logical Vision WiT v5.3 to Logical Vision FastOCR v1.2.  Completely changed headers, libraries, and variable name types.  Functionality still the same as prior version.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/