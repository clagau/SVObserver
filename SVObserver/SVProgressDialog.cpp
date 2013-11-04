//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVProgressDialog
//* .File Name       : $Workfile:   SVProgressDialog.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 14:37:36  $
//******************************************************************************

#include "stdafx.h"
#include "SVProgressDialog.h"
#include "SVUserMessage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SVProgressDialog::SVProgressDialog(LPCTSTR title, CWnd* pParent /*=NULL*/)
: CDialog(SVProgressDialog::IDD, pParent)
, m_Title(title)
, m_TotalSize(0)
, m_AmountProcessed(0)
, m_pTask(NULL)
{
	//{{AFX_DATA_INIT(SVProgressDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void SVProgressDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVProgressDialog)
	DDX_Control(pDX, IDC_STATIC1, m_staticTextCtrl);
	DDX_Control(pDX, IDC_PROGRESS1, m_progressCtrl);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(SVProgressDialog, CDialog)
	//{{AFX_MSG_MAP(SVProgressDialog)
	//}}AFX_MSG_MAP
	ON_MESSAGE( SV_UPDATE_PROGRESS, OnUpdateProgress )
	ON_MESSAGE( SV_UPDATE_PROGRESS_TEXT, OnUpdateText )
	ON_MESSAGE( SV_END_PROGRESS_DIALOG, OnEndProgressDialog )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVProgressDialog message handlers

LRESULT SVProgressDialog::OnUpdateProgress( WPARAM wParam, LPARAM lParam )
{
	int cnt = static_cast<int>(wParam);
	int total = static_cast<int>(lParam);

	// to handle initial range setup
	if (total > m_TotalSize)
	{
		m_TotalSize = total;
	}
	// To handle recursive routines
	if (total && total != m_TotalSize)
	{
		m_AmountProcessed = (m_TotalSize - total) + cnt;
	}
	else
	{
		m_AmountProcessed = cnt;
	}

	if (m_AmountProcessed && m_TotalSize)
	{
		double percent = (static_cast<double>(m_AmountProcessed) / static_cast<double>(m_TotalSize)) * 100.0;

		m_progressCtrl.SetPos(static_cast<int>(percent));
	}
	return true;
}

LRESULT SVProgressDialog::OnUpdateText(WPARAM wParam, LPARAM lParam)
{
	m_Text = reinterpret_cast<LPCTSTR>(lParam);
	m_staticTextCtrl.SetWindowText(m_Text);

	return true;
}

LRESULT SVProgressDialog::OnEndProgressDialog(WPARAM wParam, LPARAM lParam)
{
	m_progressCtrl.SetPos(100);
	EndDialog(IDOK);

	return true;
}

BOOL SVProgressDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	if (!m_Title.IsEmpty())
	{
		SetWindowText(m_Title);
	}
	if (!m_Text.IsEmpty())
	{
		m_staticTextCtrl.SetWindowText(m_Text);
	}
	m_progressCtrl.SetStep(1);
	m_progressCtrl.SetPos(0);

	if (m_pTask)
	{
		m_pTask->Start();
	}
	return true;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void SVProgressDialog::OnOK()
{
}

void SVProgressDialog::OnCancel()
{
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVProgressDialog.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 14:37:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   04 Sep 2012 15:50:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use new progress dialog functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   22 Apr 2003 14:19:40   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   02 Dec 1999 11:03:44   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  64
 * SCR Title:  Progress Dialog
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added handlers for OnOK and OnCancel.
 * Removed dead code.
 * Revised to eliminate warnings.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 13:29:02   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   16 Nov 1999 11:33:34   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added support to set Caption.
 * Revised to handle recursive progress.
 * Added Static Text Control.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   11 Nov 1999 14:01:28   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to set percentage to 100 when complete.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   03 Nov 1999 18:13:50   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Check in, generic Progress Bar Dialog
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
