//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAnalyzerResultDlg
//* .File Name       : $Workfile:   SVAnalyzerResultDlg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:32:34  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVAnalyzerResultDlg.h"
#include "SVOResource/resource.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SVAnalyzerResultDlg::SVAnalyzerResultDlg(const SvDef::StringVector& rAnalyzerResults, UINT IDD, CWnd* pParent /*=nullptr*/)
	: CDialog(IDD, pParent)
	, m_rAnalyzerResults{rAnalyzerResults}
{
}

SVAnalyzerResultDlg::~SVAnalyzerResultDlg()
{
}

void SVAnalyzerResultDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVAnalyzerResultDlg)
	DDX_Control(pDX, IDC_BA_RESULT_LIST, m_ResultList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(SVAnalyzerResultDlg, CDialog)
	//{{AFX_MSG_MAP(SVAnalyzerResultDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVAnalyzerResultDlg message handlers

BOOL SVAnalyzerResultDlg::OnInitDialog() 
{
	constexpr int cMaxCharacters = 100;

	CDialog::OnInitDialog();

	m_ResultList.SetTabStops(80);

	for (const auto& rItem : m_rAnalyzerResults)
	{
		m_ResultList.AddString(SvUl::Left(rItem, cMaxCharacters).c_str());
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
