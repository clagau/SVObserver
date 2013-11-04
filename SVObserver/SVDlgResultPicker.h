//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDlgResultPicker
//* .File Name       : $Workfile:   SVDlgResultPicker.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:18:14  $
//******************************************************************************

#if !defined(AFX_SVDLGRESULTPICKER_H__BF71BAE3_564C_11D3_A8C3_00106F000C74__INCLUDED_)
#define AFX_SVDLGRESULTPICKER_H__BF71BAE3_564C_11D3_A8C3_00106F000C74__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SVOutputInfoListTreeCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// SVDlgResultPicker dialog

class SVDlgResultPicker : public CDialog
{
// Construction
public:
	SVDlgResultPicker(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(SVDlgResultPicker)
	enum { IDD = IDD_RESULT_PICKER };
	SVOutputInfoListTreeCtrlClass m_treeOutputList;
	//}}AFX_DATA

	SVTaskObjectListClass * PTaskObjectList;	
	UINT uAttributesDesired;

protected:
	CString csCaptionTitle;       // Dialog bar text

// Methods
public:
	void SetCaptionTitle( LPCTSTR pszCaptionTitle );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVDlgResultPicker)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(SVDlgResultPicker)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnSelectObjectFilter();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// SVSelectObjectFilterDlg dialog

class SVSelectObjectFilterDlg : public CDialog
{
// Construction
public:
	SVSelectObjectFilterDlg(CWnd* pParent = NULL);   // standard constructor

	SVOutputInfoListTreeCtrlClass* m_pTree;

// Dialog Data
	//{{AFX_DATA(SVSelectObjectFilterDlg)
	enum { IDD = IDD_SELECT_OBJECT_FILTER_DIALOG };
	CListBox    m_lbFilter;
	CString     m_strTextFilter;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVSelectObjectFilterDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	UINT GetSelectedAttributes();

	// Generated message map functions
	//{{AFX_MSG(SVSelectObjectFilterDlg)
	afx_msg void OnSelect();
	afx_msg void OnDeselect();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVDLGRESULTPICKER_H__BF71BAE3_564C_11D3_A8C3_00106F000C74__INCLUDED_)

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVDlgResultPicker.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:18:14   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   25 Aug 2005 13:46:26   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   added text filter capabilities
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   18 Feb 2005 11:01:08   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  463
 * SCR Title:  Add new Linear Measurement Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated the result picker to allow in debug mode to select items by a criteria.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   22 Apr 2003 09:09:16   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 11:27:28   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   Aug 25 1999 17:06:04   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  27
 * SCR Title:  Results Picker
 * Checked in by:  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Corrected caption title code so caption text would be set after dialog window is created.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   Aug 25 1999 16:35:18   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  27
 * SCR Title:  Results Picker
 * Checked in by:  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added method to set Caption Bar text.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   Aug 25 1999 10:29:42   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  27
 * SCR Title:  Results Picker
 * Checked in by:  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Provide a dialog for result picking.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/