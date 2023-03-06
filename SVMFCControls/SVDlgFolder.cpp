//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDlgFolder
//* .File Name       : $Workfile:   SVDlgFolder.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Sep 2014 13:15:12  $
//******************************************************************************

#include "stdafx.h"
//Moved to precompiled header: #include <Dlgs.h>
//Moved to precompiled header: #include <afxdlgs.h>
#include "SVDlgFolder.h"

namespace SvMc
{
		WNDPROC SVDlgFolder::m_wndProc = nullptr;

	SVDlgFolder::SVDlgFolder(bool bFullAccess, LPCTSTR initialDir, CWnd* pParentWnd )
		: SVFileDialog(true, bFullAccess, nullptr, _T("*..*"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,nullptr,pParentWnd)      // Show folders only.
	{
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
					///Note here we must use static_cast as the CWnd* value is a temporary and could then cast to nullptr!
					// Figure out the dialog object from the hwnd.
					if (SVFileDialog* pDlg = static_cast<SVFileDialog*> (CWnd::FromHandle(hwnd)))
					{
						// Get the directory that has been selected.
						// This will eliminate any files that might have been selected.
						TCHAR path[MAX_PATH];
						GetCurrentDirectory(MAX_PATH, path);
						//Note GetDlgItem uses a temporary CWnd* and dynamic_cast cannot be used
						(static_cast<SVDlgFolder*>(pDlg->GetDlgItem(0)))->SetSelectedPath(path);
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
} //namespace SvMc
