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

namespace SvTo
{
class SVColorToolClass;
}

/////////////////////////////////////////////////////////////////////////////
// SVTADlgColorThresholdSheet

class SVTADlgColorThresholdSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(SVTADlgColorThresholdSheet)

// Construction
public:
	SVTADlgColorThresholdSheet(SvTo::SVColorToolClass* pTool, UINT nIDCaption, CWnd* pParentWnd = nullptr, UINT iSelectPage = 0);
	SVTADlgColorThresholdSheet(SvTo::SVColorToolClass* pTool, LPCTSTR pszCaption, CWnd* pParentWnd = nullptr, UINT iSelectPage = 0);

	virtual ~SVTADlgColorThresholdSheet();

	SvTo::SVColorToolClass* GetTool() { return m_pTool; }

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVTADlgColorThresholdSheet)
	public:
	virtual BOOL OnInitDialog() override;
	//}}AFX_VIRTUAL

// Implementation
	// Generated message map functions

protected:
	//{{AFX_MSG(SVTADlgColorThresholdSheet)
		afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	SVTADlgColorThresholdROI		mSheetROI;	
	SVTADlgColorThresholdAdjustment	mSheetBand[SvDef::BandNumber];
	SVTADlgColorThresholdOutput		mSheetOutput;

	SvTo::SVColorToolClass* m_pTool;

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
