//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSaveToolSetImageDialog
//* .File Name       : $Workfile:   SVSaveToolSetImageDialog.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:02:22  $
//******************************************************************************

//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************

#ifndef SVSAVETOOLSETIMAGEDIALOG_H
#define SVSAVETOOLSETIMAGEDIALOG_H

//******************************************************************************
//* INCLUDE(S):
//******************************************************************************

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SVDlgImage.h"

class SVToolSetClass;

//******************************************************************************
//* CLASS(ES) & TYPE(S):
//* STRUCTURE(S) & TYPE(S):
//* UNION(S) & TYPE(S):
//******************************************************************************

/////////////////////////////////////////////////////////////////////////////
// SVSaveToolSetImageDialogClass dialog
class SVSaveToolSetImageDialogClass : public CDialog
{
//******************************************************************************
// Constructor(s):
//******************************************************************************
public:
	SVSaveToolSetImageDialogClass(CWnd* pParent = NULL);   // standard constructor

//******************************************************************************
// Operator(s):
//******************************************************************************
public:
	//{{AFX_VIRTUAL(SVSaveToolSetImageDialogClass)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	// Generated message map functions
	//{{AFX_MSG(SVSaveToolSetImageDialogClass)
	afx_msg void OnSaveButton();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeAvailableImagesCombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


//******************************************************************************
// Data Element(s):
//******************************************************************************
public:
	SVToolSetClass*		PToolSet;
	//{{AFX_DATA(SVSaveToolSetImageDialogClass)
	enum { IDD = IDD_SELECT_TOOLSET_IMAGE_TO_SAVE_DIALOG };
	SVDlgImageClass	currentSelectedImageCtrl;
	CComboBox	availableImagesComboCtrl;
	//}}AFX_DATA

};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************
#endif	// SVSAVETOOLSETIMAGEDIALOG_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVSaveToolSetImageDialog.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 15:02:22   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   22 Apr 2003 15:11:24   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   30 Jan 2003 16:29:58   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated OnSaveButton method to use the new image circle buffer methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   23 Mar 2000 16:43:22   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  118
 * SCR Title:  Integration of LUT Operator
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to include SVDlgImage.h
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 13:37:32   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   Sep 23 1999 09:00:02   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  35
 * SCR Title:  PPQ mode to wait for input data
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   New Provides Dialog to save any image from ToolSet..
 * Replaces old Save Image in IPDoc::OnFileSaveImage().
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/