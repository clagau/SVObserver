// ******************************************************************************
// * COPYRIGHT (c) 2006 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVCameraPropDlgClass.h
// * .File Name       : $Workfile:   SVCameraPropDlgClass.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 11:08:28  $
// ******************************************************************************

#ifndef SVCAMERAPROPDLGCLASS_H
#define SVCAMERAPROPDLGCLASS_H

#include "SVMFCControls\SVListCtrl.h"

class SVDigitizerLoadLibraryClass;

class SVCameraPropDlgClass : public CDialog
{
public:
	SVCameraPropDlgClass(CWnd* pParent = NULL);   // standard constructor

	//{{AFX_DATA(SVCameraPropDlgClass)
	enum { IDD = IDD_CAMERA_PROP };
	SvMc::SVListCtrl	m_svListCtrl;
	//}}AFX_DATA

	SVDigitizerLoadLibraryClass *m_psvDigitizers;
	unsigned long m_ulHandle;

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVCameraPropDlgClass)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	// Generated message map functions
	//{{AFX_MSG(SVCameraPropDlgClass)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif

