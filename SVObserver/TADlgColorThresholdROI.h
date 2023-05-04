//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//*****************************************************************************
//* .Module Name     : TADlgColorThresholdROI
//* .File Name       : $Workfile:   TADlgColorThresholdROI.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   26 Jun 2014 18:17:56  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "TADlgColorThresholdBasePage.h"
#pragma endregion Includes

/////////////////////////////////////////////////////////////////////////////
// TADlgColorThresholdROI dialog

class TADlgColorThresholdROI : public TADlgColorThresholdBasePage
{
	DECLARE_DYNCREATE(TADlgColorThresholdROI)

// Construction
public:

	TADlgColorThresholdROI();
	virtual ~TADlgColorThresholdROI();

	void SetInspectionData();

// Dialog Data
	//{{AFX_DATA(TADlgColorThresholdROI)
	enum { IDD = IDD_TA_COLOR_THRESHOLD_ROI };
	CString	mcsROIHeight;
	CString	mcsROIWidth;
	CString	mcsROIY;
	CString	mcsROIX;
	CString	m_strMouseColor;
	CString	m_strMouseCoordinates;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(TADlgColorThresholdROI)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(TADlgColorThresholdROI)
	virtual BOOL OnInitDialog() override;
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	void UpdateToolFigure();
	void RefreshProperties();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
