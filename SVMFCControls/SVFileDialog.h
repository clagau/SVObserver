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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVLibrary\SVFileDialog.h_v  $
 * 
 *    Rev 1.0   18 Sep 2014 13:15:12   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  944
 * SCR Title:  Fix Security for File and Folder Selection Dialog for 64 Bit
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial checkin, moved from SVObserver
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 10:36:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   22 Apr 2003 09:33:16   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/