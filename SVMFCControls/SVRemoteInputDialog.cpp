//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRemoteInputDialog
//* .File Name       : $Workfile:   SVRemoteInputDialog.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 14:47:22  $
//******************************************************************************

#include "stdafx.h"
#include "SVRemoteInputDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace Seidenader
{
	namespace SVMFCControls
	{
		SVRemoteInputDialog::SVRemoteInputDialog(CWnd* pParent /*=NULL*/)
			: CDialog(SVRemoteInputDialog::IDD, pParent)
		{
			//{{AFX_DATA_INIT(SVRemoteInputDialog)
			m_lRemoteInputCount = 0;
			//}}AFX_DATA_INIT
		}


		void SVRemoteInputDialog::DoDataExchange(CDataExchange* pDX)
		{
			CDialog::DoDataExchange(pDX);
			//{{AFX_DATA_MAP(SVRemoteInputDialog)
			DDX_Text(pDX, IDC_NUMBER, m_lRemoteInputCount);
			DDV_MinMaxLong(pDX, m_lRemoteInputCount, 0, 128);
			//}}AFX_DATA_MAP
		}


		BEGIN_MESSAGE_MAP(SVRemoteInputDialog, CDialog)
			//{{AFX_MSG_MAP(SVRemoteInputDialog)
			//}}AFX_MSG_MAP
		END_MESSAGE_MAP()

		/////////////////////////////////////////////////////////////////////////////
		// SVRemoteInputDialog message handlers

		BOOL SVRemoteInputDialog::OnInitDialog() 
		{
			CDialog::OnInitDialog();

			return TRUE;  // return TRUE unless you set the focus to a control
			// EXCEPTION: OCX Property Pages should return FALSE
		}// end OnInitDialog
	} //SVMFCControls
} //Seidenader

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVRemoteInputDialog.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 14:47:22   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   22 Apr 2003 14:44:08   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   23 Jan 2003 15:46:24   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  307
 * SCR Title:  Implement the ability to remotely change Tool parameters on the SVIM
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 13:52:44   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  272
 * SCR Title:  Outputs do not always get set correctly.
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   This is the first revision of this file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/