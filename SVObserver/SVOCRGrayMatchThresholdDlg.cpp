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

