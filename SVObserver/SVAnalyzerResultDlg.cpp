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
#include "SVOCVBlobRecord.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SVAnalyzerResultDlg::SVAnalyzerResultDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(SVAnalyzerResultDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(SVAnalyzerResultDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

SVAnalyzerResultDlg::~SVAnalyzerResultDlg()
{
}

void SVAnalyzerResultDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVAnalyzerResultDlg)
	DDX_Control(pDX, IDC_BA_RESULT_LIST, mclbResultList);
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
	CDialog::OnInitDialog();

	mclbResultList.SetTabStops(80);

	if( nullptr != m_pOcvCharaterResults )
	{
		for( auto pResult : *m_pOcvCharaterResults )
		{
			if( nullptr != pResult )
			{
				char cValue;
				double dValue;
				pResult->m_cvoLabelValue.GetValue( cValue );
				pResult->m_dvoMatchScore.GetValue( dValue );
				
				if( dValue  > -1.0f )
				{
					CString	strItem;
					strItem.Format( "%c\t%f", cValue, dValue );
					mclbResultList.AddString( strItem );
				}// end if
			}// end if
		}// end for
	}// end if
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
