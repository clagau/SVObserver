//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFileDialog
//* .File Name       : $Workfile:   SVFileDialog.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Sep 2014 13:15:12  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <afxdlgs.h>
#pragma endregion Includes

namespace Seidenader
{
	namespace SVMFCControls
	{
		class CNewWnd : public CWnd
		{
		public:
			CNewWnd();
			virtual ~CNewWnd();

			// ClassWizard generated virtual function overrides
			//{{AFX_VIRTUAL(CNewWnd)
		protected:
			virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
			//}}AFX_VIRTUAL

			// Generated message map functions
		protected:
			//{{AFX_MSG(CNewWnd)
			//}}AFX_MSG
			DECLARE_MESSAGE_MAP()
		};

		class SVFileDialog : public CFileDialog
		{
			DECLARE_DYNAMIC(SVFileDialog)

		public:
			SVFileDialog(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
				bool bFullAccess,
				LPCTSTR lpszDefExt = nullptr,
				LPCTSTR lpszFileName = nullptr,
				DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
				LPCTSTR lpszFilter = nullptr,
				CWnd* pParentWnd = nullptr);
			virtual ~SVFileDialog();

		protected:
			//{{AFX_MSG(SVFileDialog)
			// NOTE - the ClassWizard will add and remove member functions here.
			//}}AFX_MSG
			virtual void OnFolderChange();

			CNewWnd m_oWnd1; // View containing Explorer window to intercept WM_COMMAND
			bool m_bFullAccess;

			DECLARE_MESSAGE_MAP()
		};
	} //SVMFCControls
} //Seidenader

namespace SvMc = Seidenader::SVMFCControls;

