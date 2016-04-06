//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeCameraManagerDlg
//* .File Name       : $Workfile:   SVGigeCameraManagerDlg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   05 Jul 2013 09:11:06  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVGigeCameraManager.h"
#include "SVOGui\SVGigeCameraListCtrl.h"
#pragma endregion Includes

class SVGigeCameraManagerDlg : public CDialog
{
public:
	SVGigeCameraStructSet m_CamList;
	SVGigeCameraStructSet m_OriginalIniCameraList;
	SVGigeCameraManagerDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~SVGigeCameraManagerDlg();

// Dialog Data
	//{{AFX_DATA(SVGigeCameraManagerDlg)
	enum { IDD = IDD_GIGE_CAMERA_MANAGER_DLG };
	CButton	m_CUpButton;
	CButton	m_CDownButton;
	//}}AFX_DATA

	CBitmap	m_CUpArrowBmp;
	CBitmap m_CDownArrowBmp;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVGigeCameraManagerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(SVGigeCameraManagerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnRefresh();
	virtual void OnOK();
	afx_msg void OnMoveDown();
	afx_msg void OnMoveUp();
	afx_msg void OnCancel();
	//}}AFX_MSG
	HRESULT Refresh();
	DECLARE_MESSAGE_MAP()

private:
	void UpdateListCtrl();

public:
	SvOg::SVGigeCameraListCtrl m_ctlCameraList;
};


