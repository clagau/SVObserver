//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : <SVAnalyzerLuminanceSetup>
//* .File Name       : $Workfile:   SVAnalyzerLuminanceSetup.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:32:20  $
//******************************************************************************

#include "stdafx.h"
#include "SVAnalyzerLuminanceSetup.h"

#include "SVAnalyzerLuminance.h"
#include "SVResult.h"
#include "SVSetupDialogManager.h"

SVLuminanceAnalyzerSetupClass::SVLuminanceAnalyzerSetupClass(CWnd* pParent /*=NULL*/)
: CDialog(SVLuminanceAnalyzerSetupClass::IDD, pParent)
{
	m_pAnalyzer = NULL;

	//{{AFX_DATA_INIT(SVLuminanceAnalyzerSetupClass)
	m_calcStdDev = FALSE;
	//}}AFX_DATA_INIT
}

void SVLuminanceAnalyzerSetupClass::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVLuminanceAnalyzerSetupClass)
	DDX_Check(pDX, IDC_CALC_STDDEV, m_calcStdDev);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SVLuminanceAnalyzerSetupClass, CDialog)
	//{{AFX_MSG_MAP(SVLuminanceAnalyzerSetupClass)
	ON_BN_CLICKED(IDC_RANGE, OnRange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVLuminanceAnalyzerSetupClass message handlers

void SVLuminanceAnalyzerSetupClass::OnOK() 
{
	SetInspectionData();

	CDialog::OnOK();
}

BOOL SVLuminanceAnalyzerSetupClass::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SetTaskObject( m_pAnalyzer );
	
	if (m_pAnalyzer)
	{
		m_pAnalyzer->msvCalcStdDevValue.GetValue( m_calcStdDev );

		UpdateData(FALSE);
	}
	else
	{
		OnCancel();
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void SVLuminanceAnalyzerSetupClass::OnCancel() 
{
	CDialog::OnCancel();
}

void SVLuminanceAnalyzerSetupClass::OnRange() 
{
	SVResultClass* pAnalyzerResult;

    msvError.ClearLastErrorCd ();

    while (1)
    {
        pAnalyzerResult = m_pAnalyzer->GetResultObject();

        if (!pAnalyzerResult)
        {
            msvError.msvlErrorCd = -1064;
            SV_TRAP_ERROR_BRK (msvError, 1064);
        }

		if ( SVSetupDialogManager::Instance().SetupDialog( pAnalyzerResult->GetClassID(), pAnalyzerResult->GetUniqueObjectID(), this ) != S_OK )
        {
            msvError.msvlErrorCd = -1093;
            SV_TRAP_ERROR_BRK (msvError, 1093);
        }

        break;
    }
}

HRESULT SVLuminanceAnalyzerSetupClass::SetInspectionData()
{
	HRESULT l_hrOk = S_OK;

	UpdateData(); // get the value from the dialog

	l_hrOk = AddInputRequest( &( m_pAnalyzer->msvCalcStdDevValue ), m_calcStdDev );

	if( l_hrOk == S_OK )
	{
		l_hrOk = AddInputRequestMarker();
	}
	
	if( l_hrOk == S_OK )
	{
		l_hrOk = RunOnce( m_pAnalyzer->GetTool() );
	}

	return l_hrOk;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVAnalyzerLuminanceSetup.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 09:32:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   02 Jul 2012 16:11:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   08 Aug 2005 15:21:32   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   17 Apr 2003 16:33:36   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   02 Feb 2000 17:42:04   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  95
 * SCR Title:  Standard Deviation on Luminance Histogram
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin.
 * Setup Dialog for the Luminance Analyzer.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
