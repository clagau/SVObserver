//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgColorThresholdSheet
//* .File Name       : $Workfile:   SVTADlgColorThresholdSheet.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:21:12  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVTADlgColorThresholdROI.h"
#include "SVTADlgColorThresholdAdjustment.h"
#include "SVTADlgColorThresholdOutput.h"
#include "SVROIFigureEditor.h"
#pragma endregion Includes

class SVColorToolClass;

/////////////////////////////////////////////////////////////////////////////
// SVTADlgColorThresholdSheet

class SVTADlgColorThresholdSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(SVTADlgColorThresholdSheet)

// Construction
public:
	SVTADlgColorThresholdSheet(SVColorToolClass* aPTool, UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	SVTADlgColorThresholdSheet(SVColorToolClass* aPTool, LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

	virtual ~SVTADlgColorThresholdSheet();

	SVColorToolClass* GetTool() { return m_pTool; }

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVTADlgColorThresholdSheet)
	public:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
	// Generated message map functions

protected:
	//{{AFX_MSG(SVTADlgColorThresholdSheet)
		afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	SVTADlgColorThresholdROI		mSheetROI;	
	SVTADlgColorThresholdAdjustment	mSheetBand0;	
	SVTADlgColorThresholdAdjustment	mSheetBand1;	
	SVTADlgColorThresholdAdjustment	mSheetBand2;	
	SVTADlgColorThresholdOutput		mSheetOutput;

	SVColorToolClass* m_pTool;

public:
	// Shared dialog data
	int		m_iZoom;
	CPoint  m_ptOrigin;

	CRect m_ImageRect;
	CRect m_rectROI;

	SVROIFigureEditor* m_pFigureEditor;

private:
	void AddTabs();
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
