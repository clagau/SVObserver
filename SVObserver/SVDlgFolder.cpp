//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDlgFolder
//* .File Name       : $Workfile:   SVDlgFolder.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Oct 2013 12:48:28  $
//******************************************************************************

#include "stdafx.h"
#include "SVDlgFolder.h"

/////////////////////////////////////////////////////////////////////////////
//
// CDlgFolder
//
//
IMPLEMENT_DYNAMIC(CDlgFolder, CSVFileDialog)

WNDPROC CDlgFolder::m_wndProc = NULL;

CDlgFolder::CDlgFolder() : 
   //CFileDialog(TRUE, NULL, _T("*..*"))  // Show folders only.
   CSVFileDialog(TRUE,NULL,_T("*.*"))       // Show all files
{
	//m_pPath = pPath;
}


BEGIN_MESSAGE_MAP(CDlgFolder, CSVFileDialog)
	//{{AFX_MSG_MAP(CDlgFolder)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CDlgFolder::InitDlgFolder( CString* pcsPathDestination,   // The destination
                                LPCTSTR csTextOKButton,
                                LPCTSTR csTextCaptionBar )
{
   m_pPath = pcsPathDestination;
   m_csTextOKButton = csTextOKButton;
   m_csTextCaptionBar = csTextCaptionBar;
}

/////////////////////////////////////////////////////////////////////////////
//
// 
// Call this function when user navigate into CFileDialog.
// 
//
LRESULT CALLBACK WindowProcNew(
   HWND hwnd,
   UINT message, 
   WPARAM wParam, 
   LPARAM lParam
)
{
	
	if (message ==  WM_COMMAND)
   {
		if (HIWORD(wParam) == BN_CLICKED)
      {
			if (LOWORD(wParam) == IDOK)
			{
            //
            // Figure out the dialog object from the hwnd.
            //
				if (CSVFileDialog* pDlg = (CSVFileDialog*)CWnd::FromHandle(hwnd))
				{
               //
               // Get the directory that is has been selected.
               // This will eliminate the any files that might have been
               // selected too.
               //
					TCHAR path[MAX_PATH];
					GetCurrentDirectory(MAX_PATH, path);
					*((CDlgFolder*)pDlg->GetDlgItem(0))->m_pPath = CString(path);
					pDlg->EndDialog(IDOK);
					return NULL;
				}
			}
      }
   }
	return CallWindowProc(
      CDlgFolder::m_wndProc, hwnd, message, wParam, lParam);
}

/////////////////////////////////////////////////////////////////////////////
// 
// For update the wiew of CFileDialog
// 
void CDlgFolder::OnInitDone()
{
   //
   // Hide unused controls.
   //
	HideControl(edt1);
	HideControl(stc3);
	HideControl(cmb1);
	HideControl(stc2);

	CWnd* pFD = GetParent();

   //
   // Reposition the CANCEL and OK buttons.
   //
	CRect rectCancel; 
   pFD->GetDlgItem(IDCANCEL)->GetWindowRect(rectCancel);
	pFD->ScreenToClient(rectCancel);

	CRect rectOK; 
   pFD->GetDlgItem(IDOK)->GetWindowRect(rectOK);
	pFD->ScreenToClient(rectOK);
	pFD->GetDlgItem(IDOK)->SetWindowPos(
      0,
      rectCancel.left - rectOK.Width() - 4,
      rectCancel.top, 
      0,0, 
      SWP_NOZORDER | SWP_NOSIZE
   );

	CRect rectList2; 
   pFD->GetDlgItem(lst1)->GetWindowRect(rectList2);
	pFD->ScreenToClient(rectList2);
	pFD->GetDlgItem(lst1)->SetWindowPos(
      0,0,0,
      rectList2.Width(), 
      abs(rectList2.top - (rectCancel.top - 4)), 
      SWP_NOMOVE | SWP_NOZORDER
   );

   //
   // Change the text on the OK button..
   // LPCSTR "COPY"  not Unicode
   //
	//SetControlText(IDOK, _T("COPY"));
   SetControlText(IDOK,(LPCTSTR)m_csTextOKButton);

   //
   // Set the title in the caption bar..
   //
	//pFD->SetWindowText(_T("Choose Folder/Drive to Copy File To"));
	pFD->SetWindowText((LPCTSTR)m_csTextCaptionBar);

	m_wndProc = (WNDPROC)SetWindowLong(
      pFD->m_hWnd,
#ifdef _WIN64
	  GWLP_WNDPROC,
#else
      GWL_WNDPROC,
#endif
      (long)WindowProcNew
   );
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVDlgFolder.cpp_v  $
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