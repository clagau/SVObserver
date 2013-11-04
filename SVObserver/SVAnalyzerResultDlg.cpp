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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVAnalyzerResultDlg.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 09:32:34   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   17 Sep 2003 12:45:58   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  384
 * SCR Title:  Add new Matrox OCR/OCV analyzer to SVObserver
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Created and integrated a new Matrox based OCR/OCV analyzer to SVObserver.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   17 Apr 2003 16:45:22   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Jul 2000 15:48:14   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  169
 * SCR Title:  Add Match Score Information to OCR Tools
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   OCR Match Score Updates
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/