//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOCRGrayCandidateDlg
//* .File Name       : $Workfile:   SVOCRGrayCandidateDlg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:56:04  $
//******************************************************************************

#include "stdafx.h"
#include "SVOCRGrayDialog.h"
#include "SVOCRGrayCandidateDlg.h"
#include "SVOCRGrayAnalyzerResult.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVOCRGrayCandidateDlg dialog


SVOCRGrayCandidateDlg::SVOCRGrayCandidateDlg(): 
	CPropertyPage(SVOCRGrayCandidateDlg::IDD)
{
	//pOCRAnalyzerResult = NULL;

	//{{AFX_DATA_INIT(SVOCRGrayCandidateDlg)
	m_maxCandidates = 0;
	m_candsInVicinity = 0;
	m_candidateThresh = 0.0f;
	m_candidateRelThresh = 0.0f;
	//}}AFX_DATA_INIT
}


void SVOCRGrayCandidateDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVOCRGrayCandidateDlg)
	DDX_Control(pDX, IDC_EDIT_CANDIDATE_THRESH, m_editBuddy2);
	DDX_Control(pDX, IDC_EDIT_CANDIDATE_RELTHRESH, m_editBuddy1);
	DDX_Control(pDX, IDC_SLIDER_CANDIDATE_RELTHRESH, m_sliderRelThreshold);
	DDX_Control(pDX, IDC_SLIDER_CANDIDATE_THRESH, m_sliderThreshold);
	DDX_Control(pDX, IDC_COMBO_SCALE, m_cbScale);
	DDX_Text(pDX, IDC_MAX_CANDIDATES, m_maxCandidates);
	DDX_Text(pDX, IDC_CANDS_IN_VACINITY, m_candsInVicinity);
	DDX_Text(pDX, IDC_EDIT_CANDIDATE_THRESH, m_candidateThresh);
	DDX_Text(pDX, IDC_EDIT_CANDIDATE_RELTHRESH, m_candidateRelThresh);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SVOCRGrayCandidateDlg, CPropertyPage)
	//{{AFX_MSG_MAP(SVOCRGrayCandidateDlg)
	ON_EN_CHANGE(IDC_EDIT_CANDIDATE_RELTHRESH, OnChangeEditCandidateRelthresh)
	ON_EN_CHANGE(IDC_EDIT_CANDIDATE_THRESH, OnChangeEditCandidateThresh)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_CANDIDATE_RELTHRESH, OnCustomdrawSliderCandidateRelthresh)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_CANDIDATE_THRESH, OnCustomdrawSliderCandidateThresh)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVOCRGrayCandidateDlg message handlers

BOOL SVOCRGrayCandidateDlg::OnInitDialog() 
{
	CString	tmp;

	CPropertyPage::OnInitDialog();
	
	SVOCRGrayDialogClass * pParent = (SVOCRGrayDialogClass *)GetParent();
	ASSERT(pParent);
	SVOCRGrayAnalyzeResultClass * pOCRAnalyzerResult = pParent->pOCRAnalyzerResult;
	SVOCRGrayParamStruct *pOCRParameters = &pOCRAnalyzerResult->GetOCRParameters();

	m_cbScale.SetCurSel(pOCRParameters->scale);
	m_maxCandidates = pOCRParameters->maxCandidates;
	m_candsInVicinity = pOCRParameters->candsInVicinity;
	m_candidateThresh = pOCRParameters->candidateThresh;
	m_candidateRelThresh = pOCRParameters->candidateRelThresh;

	m_sliderThreshold.SetRange (0, 100);
	m_sliderThreshold.SetTicFreq (10);
	m_sliderThreshold.SetPos ((int)(m_candidateThresh*100.0+0.5));

	m_sliderRelThreshold.SetRange (0, 100);
	m_sliderRelThreshold.SetTicFreq (10);
	m_sliderRelThreshold.SetPos ((int)(m_candidateRelThresh*100.0+0.5));

	UpdateData (FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
//
//
//
void SVOCRGrayCandidateDlg::UpdateOCRParameters ()
{
    //
    // Make sure the dialog has been 'created' - 14 Apr 1999 - frb.
    //
    if(!GetSafeHwnd())
    {
       return;
    }


	UpdateData (TRUE);

	SVOCRGrayDialogClass * pParent = (SVOCRGrayDialogClass *)GetParent();
	ASSERT(pParent);
	SVOCRGrayAnalyzeResultClass * pOCRAnalyzerResult = pParent->pOCRAnalyzerResult;
	SVOCRGrayParamStruct *pOCRParameters = &pOCRAnalyzerResult->GetOCRParameters();

	pOCRParameters->scale = m_cbScale.GetCurSel();
	pOCRParameters->maxCandidates = m_maxCandidates;
	pOCRParameters->candsInVicinity = m_candsInVicinity;
	pOCRParameters->candidateThresh = m_candidateThresh;
	pOCRParameters->candidateRelThresh = m_candidateRelThresh;

	return;
}

BOOL SVOCRGrayCandidateDlg::OnKillActive() 
{
	// TODO: Add your specialized code here and/or call the base class
	UpdateOCRParameters ();

	return CPropertyPage::OnKillActive();
}

void SVOCRGrayCandidateDlg::OnChangeEditCandidateRelthresh() 
{
	UpdateData();
	if (m_candidateRelThresh < 0.0)
		m_candidateRelThresh = 0.0;
	if (m_candidateRelThresh > 1.0)
		m_candidateRelThresh = 1.0;

	int nPos = (int)(m_candidateRelThresh*100.0+0.5);
	m_sliderRelThreshold.SetPos (nPos);
}

void SVOCRGrayCandidateDlg::OnChangeEditCandidateThresh() 
{
	UpdateData();
	if (m_candidateThresh < 0.0)
		m_candidateThresh = 0.0;
	if (m_candidateThresh > 1.0)
		m_candidateThresh = 1.0;

	int nPos = (int)(m_candidateThresh*100.0+0.5);
	m_sliderThreshold.SetPos (nPos);
}

void SVOCRGrayCandidateDlg::OnCustomdrawSliderCandidateRelthresh(NMHDR* pNMHDR, LRESULT* pResult) 
{
	UpdateData();
	int nPos = m_sliderRelThreshold.GetPos ();
	m_candidateRelThresh = (float)(nPos / 100.0);
	UpdateData (FALSE);
	
	*pResult = 0;
}

void SVOCRGrayCandidateDlg::OnCustomdrawSliderCandidateThresh(NMHDR* pNMHDR, LRESULT* pResult) 
{
	UpdateData();
	int nPos = m_sliderThreshold.GetPos ();
	m_candidateThresh = (float)(nPos / 100.0);
	UpdateData (FALSE);
	
	*pResult = 0;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVOCRGrayCandidateDlg.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 12:56:04   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   22 Apr 2003 12:53:28   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   04 Oct 2001 16:48:42   Joe
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
 *    Rev 1.0   16 May 2000 14:44:48   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  133, 132
 * SCR Title:  Add OCR Gray Scale to SVObserver and Create OCR Gray Scale Font Training
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Upgrade Logical Vision WiT v5.3 to Logical Vision FastOCR v1.2.  Completely changed headers, libraries, and variable name types.  Functionality still the same as prior version.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/