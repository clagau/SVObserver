//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDlgFolder
//* .File Name       : $Workfile:   SVDlgFolder.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Sep 2014 13:15:12  $
//******************************************************************************

#include "stdafx.h"
#include <Dlgs.h>
#include <afxdlgs.h>
#include "SVDlgFolder.h"

namespace Seidenader
{
	namespace SVMFCControls
	{
		IMPLEMENT_DYNAMIC(SVDlgFolder, SVFileDialog)

			WNDPROC SVDlgFolder::m_wndProc = nullptr;

		SVDlgFolder::SVDlgFolder(bool bFullAccess, LPCTSTR initialDir) 
			: SVFileDialog(true, bFullAccess, nullptr, _T("*..*"))      // Show folders only.
		{
			/* vista style only
			m_bPickFoldersMode = true; // Show folders only.
			*/
			m_ofn.lpstrInitialDir = initialDir;
		}

		SVDlgFolder::~SVDlgFolder()
		{
		}

		BEGIN_MESSAGE_MAP(SVDlgFolder, SVFileDialog)
			//{{AFX_MSG_MAP(SVDlgFolder)
			//}}AFX_MSG_MAP
		END_MESSAGE_MAP()

		void SVDlgFolder::InitDlgFolder(LPCTSTR csTextOKButton, LPCTSTR csTextCaptionBar)
		{
			m_csTextOKButton = csTextOKButton;
			m_csTextCaptionBar = csTextCaptionBar;
		}

		void SVDlgFolder::OnFolderChange()
		{
			SVFileDialog::OnFolderChange();
		}

		LRESULT CALLBACK WindowProcNew(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			if (WM_COMMAND == message)
			{
				if (BN_CLICKED == HIWORD(wParam))
				{
					if (IDOK == LOWORD(wParam))
					{
						// Figure out the dialog object from the hwnd.
						if (SVFileDialog* pDlg = reinterpret_cast<SVFileDialog *>(CWnd::FromHandle(hwnd)))
						{
							// Get the directory that has been selected.
							// This will eliminate any files that might have been selected.
							TCHAR path[MAX_PATH];
							GetCurrentDirectory(MAX_PATH, path);
							(dynamic_cast<SVDlgFolder*>(pDlg->GetDlgItem(0)))->SetSelectedPath(path);
							pDlg->EndDialog(IDOK);
							return false;
						}
					}
				}
			}
			return CallWindowProc(SVDlgFolder::m_wndProc, hwnd, message, wParam, lParam);
		}

		CString SVDlgFolder::GetPathName() const
		{
			return m_path;
		}

		void SVDlgFolder::SetSelectedPath(LPCTSTR path)
		{
			m_path = path;
		}

		/////////////////////////////////////////////////////////////////////////////
		// For updating the View of CFileDialog
		void SVDlgFolder::OnInitDone()
		{
			// Hide unused controls.
			HideControl(edt1);
			HideControl(stc3);
			HideControl(cmb1);
			HideControl(stc2);
			HideControl(cmb13);

			CWnd* pFD = GetParent();
			if (pFD)
			{
				// Reposition the CANCEL and OK buttons.
				CRect rectCancel; 
				pFD->GetDlgItem(IDCANCEL)->GetWindowRect(rectCancel);
				pFD->ScreenToClient(rectCancel);

				CRect rectOK; 
				pFD->GetDlgItem(IDOK)->GetWindowRect(rectOK);
				pFD->ScreenToClient(rectOK);
				pFD->GetDlgItem(IDOK)->SetWindowPos(0, rectCancel.left - rectOK.Width() - 4, rectCancel.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

				CRect rectList2; 
				pFD->GetDlgItem(lst1)->GetWindowRect(rectList2);
				pFD->ScreenToClient(rectList2);
				pFD->GetDlgItem(lst1)->SetWindowPos(0, 0, 0, rectList2.Width(), abs(rectList2.top - (rectCancel.top - 4)), SWP_NOMOVE | SWP_NOZORDER);

				if (!m_csTextOKButton.IsEmpty())
				{
					// Change the text on the OK button..
					SetControlText(IDOK, static_cast<LPCTSTR>(m_csTextOKButton));
				}

				if (!m_csTextCaptionBar.IsEmpty())
				{
					// Set the title in the caption bar..
					pFD->SetWindowText(static_cast<LPCTSTR>(m_csTextCaptionBar));
				}

				m_wndProc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(pFD->m_hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WindowProcNew)));
			}
		}
	} //SVMFCControls
} //Seidenader

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVLibrary\SVDlgFolder.cpp_v  $
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
 *    Rev 1.3   25 Jul 2014 15:09:18   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  922
 * SCR Title:  Fix issues with the Folder Select Dialog
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed the constructor to only show Folders
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   21 May 2014 17:55:24   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Revised to correct issue with truncated pointer for wndProc in 64bit
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Oct 2013 12:48:28   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 10:15:04   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   22 Apr 2003 09:02:58   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   19 Nov 2002 11:52:48   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   comments were taken out
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   12 Jun 2001 13:35:04   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  200
 * SCR Title:  Update code to implement SVIM COM Server
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Change all occurences of CFileDialog to CSVFileDialog.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 11:27:24   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   Oct 18 1999 11:14:24   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  15
 * SCR Title:  Integrate archive tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Introduce code to select folder for storing images to be archived.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/