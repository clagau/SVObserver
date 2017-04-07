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
#pragma region Includes
#include "stdafx.h"
#include "SVBlobAnalyzerResultDlg.h"
#include "SVBlobAnalyzer.h"
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

/////////////////////////////////////////////////////////////////////////////
// SVBlobAnalyzerResultDlg dialog

SVBlobAnalyzerResultDlg::SVBlobAnalyzerResultDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(SVBlobAnalyzerResultDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(SVBlobAnalyzerResultDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

SVBlobAnalyzerResultDlg::~SVBlobAnalyzerResultDlg()
{
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

	for( SVBlobFeatureEnum i = SV_AREA; i < SV_TOPOF_LIST; i = (SVBlobFeatureEnum)(i + 1))
	{	
		if (m_pBlobAnalyzer->m_FeaturesEnabled [i] == _T('1'))
		{
			double* pData = &(m_pBlobAnalyzer->m_vec2dBlobResults[i][0]);
			
			SVString Item = SvUl_SF::Format( _T("%s\t%5.1f"), m_pBlobAnalyzer->m_Value[i].GetName(), pData[m_pBlobAnalyzer->m_nBlobIndex]);
			m_ResultList.AddString( SvUl_SF::Left(Item, 100).c_str() );
		}
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

