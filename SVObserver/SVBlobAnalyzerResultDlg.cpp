//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVBlobAnalyzerResultDlg
//* .File Name       : $Workfile:   SVBlobAnalyzerResultDlg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:44:52  $
//******************************************************************************
#include "stdafx.h"
#include "SVBlobAnalyzerResultDlg.h"
#include "SVBlobAnalyzer.h"

/////////////////////////////////////////////////////////////////////////////
// SVBlobAnalyzerResultDlg dialog

SVBlobAnalyzerResultDlg::SVBlobAnalyzerResultDlg(CWnd* pParent /*=NULL*/)
	: CDialog(SVBlobAnalyzerResultDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(SVBlobAnalyzerResultDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void SVBlobAnalyzerResultDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVBlobAnalyzerResultDlg)
	DDX_Control(pDX, IDC_BA_RESULT_LIST, m_ResultList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(SVBlobAnalyzerResultDlg, CDialog)
	//{{AFX_MSG_MAP(SVBlobAnalyzerResultDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVBlobAnalyzerResultDlg message handlers

BOOL SVBlobAnalyzerResultDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_ResultList.SetTabStops(80);
	CString	strItem;

	for (SVBlobFeatureEnum i = SV_AREA; 
	i < SV_TOPOF_LIST; 
	i = (SVBlobFeatureEnum)(i + 1))
	{	
		if (m_pBlobAnalyzer->msvszFeaturesEnabled [i] == _T('1'))
		{
			double* pData = &(m_pBlobAnalyzer->m_vec2dBlobResults[i][0]);
			
			strItem.Format("%s\t%5.1f", m_pBlobAnalyzer->msvValue[i].GetName(),
				pData[m_pBlobAnalyzer->m_nBlobIndex]);
			m_ResultList.AddString(strItem.Left(100));
		}
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVBlobAnalyzerResultDlg.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 09:44:52   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   30 Aug 2010 13:48:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  700
 * SCR Title:  Remove String Buffer Problems
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update code to remove or fix uses of CString GetBuffer and ReleaseBuffer methods.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   20 Oct 2008 10:57:38   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  635
 * SCR Title:  Blob Analyzer Bug Fixes
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Moved Fom SVBlobAnalyzer modules.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
