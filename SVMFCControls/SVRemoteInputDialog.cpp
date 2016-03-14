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

