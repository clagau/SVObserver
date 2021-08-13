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
#include "InspectionEngine/SVGigeCameraManager.h"
#pragma endregion Includes

class SVGigeCameraManagerDlg : public CDialog
{
public:
	SVGigeCameraStructVector m_CamList;
	SVGigeCameraManagerDlg( CWnd* pParent = nullptr );
	virtual ~SVGigeCameraManagerDlg();

// Dialog Data
	//{{AFX_DATA(SVGigeCameraManagerDlg)
	enum { IDD = IDD_GIGE_CAMERA_MANAGER_DLG };
	//}}AFX_DATA

protected:
	DECLARE_MESSAGE_MAP()

	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual BOOL OnInitDialog() override;
	afx_msg void OnRefresh();

	HRESULT Refresh();

private:
	CListCtrl m_ctlCameraList;
};


