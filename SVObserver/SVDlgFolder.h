//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDlgFolder
//* .File Name       : $Workfile:   SVDlgFolder.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:15:12  $
//******************************************************************************

#ifndef __DLGFOLDER_H__
#define __DLGFOLDER_H__

#include "SVFileDialog.h"

/////////////////////////////////////////////////////////////////////////////
//
// CDlgFolder dialog
//
//
class CDlgFolder : public CSVFileDialog
{
	DECLARE_DYNAMIC(CDlgFolder)

public:
	CDlgFolder();
   void InitDlgFolder(
      CString* pcsPathDestination,   // The destination
      LPCTSTR csTextOKButton,
      LPCTSTR csTextCaptionBar
   );

   
public:
	static WNDPROC m_wndProc;
	virtual void OnInitDone( );

// Attributes
	CString* m_pPath;
   CString m_csTextOKButton;
   CString m_csTextCaptionBar;

protected:
   void RebuildTheTree();

	//{{AFX_MSG(CDlgFolder)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif //__DLGFOLDER_H__

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVDlgFolder.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:15:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   22 Apr 2003 09:03:00   rschock
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
 *    Rev 1.0   Oct 18 1999 11:14:22   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  15
 * SCR Title:  Integrate archive tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Introduce code to select folder for storing images to be archived.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/