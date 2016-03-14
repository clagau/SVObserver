//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SV1394CameraManagerDlg
//* .File Name       : $Workfile:   SV1394CameraManagerDlg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:18:14  $
//******************************************************************************

#ifndef SV1394CAMERAMANAGERDLG_H
#define SV1394CAMERAMANAGERDLG_H

#include "SV1394CameraManager.h"
#include "SVCameraListBox.h"

class SV1394CameraManagerDlg : public CDialog
{
public:
	SV1394CameraStructSet m_CamList;
	SV1394CameraStructSet m_OriginalCamList;
	SV1394CameraManagerDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(SV1394CameraManagerDlg)
	enum { IDD = IDD_1394_CAMERA_MANAGER_DLG };
	CButton	m_CUpButton;
	CButton	m_CDownButton;
	SVCameraListBox	m_lbCameras;
	CString	m_sSelectedCamea;
	//}}AFX_DATA

	CBitmap	m_CUpArrowBmp;
	CBitmap m_CDownArrowBmp;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SV1394CameraManagerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(SV1394CameraManagerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnRefresh();
	virtual void OnOK();
	afx_msg void OnMoveDown();
	afx_msg void OnMoveUp();
	afx_msg void OnCancel();
	//}}AFX_MSG
	HRESULT Refresh();
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif

