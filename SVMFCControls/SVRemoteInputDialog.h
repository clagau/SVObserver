//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRemoteInputDialog
//* .File Name       : $Workfile:   SVRemoteInputDialog.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 14:47:36  $
//******************************************************************************

#pragma once

namespace Seidenader
{
	namespace SVMFCControls
	{
		class SVRemoteInputDialog : public CDialog
		{
			// Construction
		public:
			SVRemoteInputDialog(CWnd* pParent = NULL);   // standard constructor

			// Dialog Data
			//{{AFX_DATA(SVRemoteInputDialog)
			enum { IDD = IDD_REMOTE_INPUTS };
			long	m_lRemoteInputCount;
			//}}AFX_DATA


			// Overrides
			// ClassWizard generated virtual function overrides
			//{{AFX_VIRTUAL(SVRemoteInputDialog)
		protected:
			virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
			//}}AFX_VIRTUAL

			// Implementation
		protected:

			// Generated message map functions
			//{{AFX_MSG(SVRemoteInputDialog)
			virtual BOOL OnInitDialog();
			//}}AFX_MSG
			DECLARE_MESSAGE_MAP()
		};
	} //SVMFCControls
} //Seidenader

namespace SvMc = Seidenader::SVMFCControls;

