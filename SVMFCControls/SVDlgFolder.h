//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDlgFolder
//* .File Name       : $Workfile:   SVDlgFolder.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Sep 2014 13:15:12  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVFileDialog.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace SVMFCControls
	{
		class SVDlgFolder : public SVFileDialog
		{
			DECLARE_DYNAMIC(SVDlgFolder)

		public:
			SVDlgFolder(bool bFullAccess, LPCTSTR initialDir=nullptr);
			virtual ~SVDlgFolder();
			void InitDlgFolder(LPCTSTR csTextOKButton, LPCTSTR csTextCaptionBar);

			virtual void OnInitDone() override;
			CString GetPathName() const;
			void SetSelectedPath(LPCTSTR path);

			CString m_csTextOKButton;
			CString m_csTextCaptionBar;
			static WNDPROC m_wndProc;

		protected:
			virtual void OnFolderChange() override;

			//{{AFX_MSG(SVDlgFolder)
			//}}AFX_MSG
			DECLARE_MESSAGE_MAP()

			CString m_path;
		};
	} //SVMFCControls
} //Seidenader

namespace SvMc = Seidenader::SVMFCControls;

