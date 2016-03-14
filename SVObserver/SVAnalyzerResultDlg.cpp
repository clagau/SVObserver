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

#include "stdafx.h"
#include "SVAnalyzerResultDlg.h"
#include "SVOCRBlobRecord.h"
#include "SVOCVBlobRecord.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVAnalyzerResultDlg dialog


SVAnalyzerResultDlg::SVAnalyzerResultDlg(CWnd* pParent /*=NULL*/)
	: CDialog(SVAnalyzerResultDlg::IDD, pParent)
{
	svocrbraResults = NULL;

	//{{AFX_DATA_INIT(SVAnalyzerResultDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
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

	char cValue;
	double dValue;
	
	mclbResultList.SetTabStops(80);

	if ( svocrbraResults )
	{
		CString	strItem;
		for (int i = 0; i < svocrbraResults->GetSize(); i++)
		{
			if ( svocrbraResults->GetAt(i) )
			{
				BOOL bFound = FALSE;

				for (int j = 0; !bFound && j < mclbResultList.GetCount(); j++)
				{
					CString csTemp;

					mclbResultList.GetText(j, csTemp);

					bFound = svocrbraResults->GetAt(i)->csLabel[0] == csTemp[0];
				}

				if (!bFound)
				{
					strItem.Format("%s\t%f", svocrbraResults->GetAt(i)->csLabel, svocrbraResults->GetAt(i)->fScore);
					mclbResultList.AddString(strItem);
				}
			}
		}
	}

	if( psvocvResultArray && psvocvResultArray->GetSize() )
	{
		CString	strItem;
		for( int i = 0; i < psvocvResultArray->GetSize(); i++ )
		{
			if( psvocvResultArray->GetAt(i) )
			{
				psvocvResultArray->GetAt(i)->m_cvoLabelValue.GetValue( cValue );
				psvocvResultArray->GetAt(i)->m_dvoMatchScore.GetValue( dValue );
				
				if( dValue  > -1.0f )
				{
					strItem.Format( "%c\t%f", cValue, dValue );
					mclbResultList.AddString( strItem );
				}// end if

			}// end if

		}// end for

	}// end if
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

