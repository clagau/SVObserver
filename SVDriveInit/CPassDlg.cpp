//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : CCPassDlg
//* .File Name       : $Workfile:   CPassDlg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 08:33:08  $
//******************************************************************************

#include "stdafx.h"
#include "SVDriveInit.h"
#include "CPassDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCPassDlg dialog


CCPassDlg::CCPassDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCPassDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCPassDlg)
	m_password = _T("");
	//}}AFX_DATA_INIT
}


void CCPassDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCPassDlg)
	DDX_Text(pDX, IDC_EDIT1, m_password);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCPassDlg, CDialog)
	//{{AFX_MSG_MAP(CCPassDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCPassDlg message handlers

void CCPassDlg::OnOK() 
{
	UpdateData(TRUE);

	CString tmp, title;
	
	GetWindowText( title );

	if( (m_password.CompareNoCase("magic") != 0) && (m_password.CompareNoCase("merlin") != 0) )
		{
		tmp = "Invalid Password";
		MessageBox( tmp, title, MB_ICONERROR );
		}
	else
		{

	// TODO: Add extra validation here
	
	CDialog::OnOK();
		}
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVDriveInit\CPassDlg.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 08:33:08   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   28 Aug 2003 10:58:18   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Initial Check-in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/