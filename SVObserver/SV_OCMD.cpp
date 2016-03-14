//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOCMD
//* .File Name       : $Workfile:   SV_OCMD.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:18:50  $
//******************************************************************************

#include "stdafx.h"
#include "SVOCRDialog.h"
#include "SV_OCMD.h"
#include "SVOCRAnalyzerResult.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVOCRCharMatchDlg property page

IMPLEMENT_DYNCREATE(SVOCRCharMatchDlg, CPropertyPage)

SVOCRCharMatchDlg::SVOCRCharMatchDlg() : CPropertyPage(SVOCRCharMatchDlg::IDD)
{
	//pOCRAnalyzerResult = NULL;

	//{{AFX_DATA_INIT(SVOCRCharMatchDlg)
	m_thresholdEdit = 0;
	m_ignoreNoMatch = FALSE;
	//}}AFX_DATA_INIT
}

SVOCRCharMatchDlg::~SVOCRCharMatchDlg()
{
}

void SVOCRCharMatchDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVOCRCharMatchDlg)
	DDX_Control(pDX, IDC_NOMATCHCHAR_EDIT, m_noMatchCharLabel);
	DDX_Control(pDX, IDC_THRESHOLD, m_thresholdCntrl);
	DDX_Control(pDX, IDC_SLIDER1, m_thresholdSlider);
	DDX_Text(pDX, IDC_THRESHOLD, m_thresholdEdit);
	DDX_Check(pDX, IDC_IGNORENOMATCH_BUTTON, m_ignoreNoMatch);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SVOCRCharMatchDlg, CPropertyPage)
	//{{AFX_MSG_MAP(SVOCRCharMatchDlg)
	ON_WM_HSCROLL()
	ON_EN_CHANGE(IDC_THRESHOLD, OnChangeThreshold)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVOCRCharMatchDlg message handlers

BOOL SVOCRCharMatchDlg::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
    SVOCRDialogClass * pParent = (SVOCRDialogClass *)GetParent();
	ASSERT(pParent);
	SVOCRAnalyzeResultClass * pOCRAnalyzerResult = pParent->pOCRAnalyzerResult;
	SVOCRParamStruct *pOCRParameters = &pOCRAnalyzerResult->GetOCRParameters();
	
	m_thresholdEdit = (int)(pOCRParameters->fthreshold * 100.0 + .5);

	m_thresholdSlider.SetRange (0, 100);
	m_thresholdSlider.SetPos (m_thresholdEdit);
	m_thresholdSlider.SetTicFreq (10);
	m_thresholdCntrl.SetLimitText (3);

	m_noMatchCharLabel.SetWindowText ((LPCTSTR)pOCRParameters->STRnoMatchLabel);

	if (pOCRParameters->ignoreNoMatch == 0)
		m_ignoreNoMatch = 0;
	else
		m_ignoreNoMatch = 1;


	UpdateData (FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void SVOCRCharMatchDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	CPropertyPage::OnHScroll(nSBCode, nPos, pScrollBar);

	m_thresholdEdit = m_thresholdSlider.GetPos ();
	UpdateData (FALSE);

//	*pResult = 0;

}


void SVOCRCharMatchDlg::OnChangeThreshold() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPropertyPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	TCHAR		tmp [100];
	int			value;

	m_thresholdCntrl.GetWindowText (tmp, sizeof(tmp));

	value = atoi (tmp);

	m_thresholdSlider.SetPos (value);
}

/*---------
void SVOCRCharMatchDlg::OnOK() 
{
	// TODO: Add your specialized code here and/or call the base class

	UpdateOCRParameters ();

	CPropertyPage::OnOK();
}
-------*/

BOOL SVOCRCharMatchDlg::UpdateOCRParameters ()
{
	BOOL l_bRet = TRUE;
	int	err;

	err = 0;
    //
    // Make sure the dialog has been 'created' - 14 Apr 1999 - frb.
    //
    if(!GetSafeHwnd())
    {
       return TRUE;
    }

	while (1)
	{
		SVOCRDialogClass * pParent = (SVOCRDialogClass *)GetParent();
		ASSERT(pParent);
		SVOCRAnalyzeResultClass * pOCRAnalyzerResult = pParent->pOCRAnalyzerResult;
		SVOCRParamStruct *POCRParameters = &pOCRAnalyzerResult->GetOCRParameters();

		UpdateData (TRUE);
		
		POCRParameters->fthreshold = ((float) m_thresholdEdit) / (float) 100.0;
		m_noMatchCharLabel.GetWindowText ( POCRParameters->STRnoMatchLabel );

		if (m_ignoreNoMatch == 0)

			POCRParameters->ignoreNoMatch = 0;
		else
			POCRParameters->ignoreNoMatch = 1;

		break;
	}

	return l_bRet;
}

BOOL SVOCRCharMatchDlg::OnSetActive() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CPropertyPage::OnSetActive();
}

BOOL SVOCRCharMatchDlg::OnKillActive() 
{
	// TODO: Add your specialized code here and/or call the base class
	UpdateOCRParameters();

	return CPropertyPage::OnKillActive();
}

