//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//*****************************************************************************
//* .Module Name     : SVTADlgColorThresholdROI
//* .File Name       : $Workfile:   SVTADlgColorThresholdROI.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   26 Jun 2014 18:17:56  $
//******************************************************************************

#if !defined(SVTADLGCOLORTHRESHOLDROI_H)
#define SVTADLGCOLORTHRESHOLDROI_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SVTADlgColorThresholdROI.h : header file
//

#include "SVTADlgColorThresholdBasePage.h"
#include "SVTaskObjectInterfaceClass.h"

/////////////////////////////////////////////////////////////////////////////
// SVTADlgColorThresholdROI dialog

class SVTADlgColorThresholdROI : public SVTADlgColorThresholdBasePage, public SVTaskObjectInterfaceClass
{
	DECLARE_DYNCREATE(SVTADlgColorThresholdROI)

// Construction
public:

	SVTADlgColorThresholdROI();
	~SVTADlgColorThresholdROI();

	virtual HRESULT SetInspectionData();

// Dialog Data
	//{{AFX_DATA(SVTADlgColorThresholdROI)
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
	//{{AFX_VIRTUAL(SVTADlgColorThresholdROI)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(SVTADlgColorThresholdROI)
	virtual BOOL OnInitDialog();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	void UpdateToolFigure();
	void RefreshProperties();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(SVTADLGCOLORTHRESHOLDROI_H)

