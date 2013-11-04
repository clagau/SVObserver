//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDlgImagePicker
//* .File Name       : $Workfile:   SVDlgImagePicker.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:17:58  $
//******************************************************************************

#ifndef DLGIMAGEPICKER_H_
#define DLGIMAGEPICKER_H_
#pragma once
//#if 0
#include "SVOutputInfoListTreeCtrl.h"
#include "SVImageListClass.h"

/////////////////////////////////////////////////////////////////////////////
// SVDlgImagePicker dialog

class SVToolSetClass;

class SVDlgImagePicker : public CDialog
{
// Construction
public:
	SVDlgImagePicker(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(SVDlgImagePicker)
	enum { IDD = IDD_RESULT_PICKER };
	SVOutputInfoListTreeCtrlClass m_treeImageList;
	//}}AFX_DATA

	SVImageListClass m_ImageList;	
	UINT             m_uAttributesDesired;
	SVToolSetClass*  m_pToolSet;

protected:
	CString csCaptionTitle;       // Dialog bar text

// Methods
public:
	void SetCaptionTitle( LPCTSTR pszCaptionTitle );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVDlgImagePicker)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(SVDlgImagePicker)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnSelectObjectFilter();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
//#endif//#if 0
#endif // #ifndef DLGIMAGEPICKER_H_

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVDlgImagePicker.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:17:58   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   01 Nov 2005 14:46:36   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  512
 * SCR Title:  Add Published Result Images functionality
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first iteration - allow user to select image like result picker
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/