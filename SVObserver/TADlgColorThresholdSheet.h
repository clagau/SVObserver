//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : TADlgColorThresholdSheet
//* .File Name       : $Workfile:   TADlgColorThresholdSheet.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:21:12  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "TADlgColorThresholdROI.h"
#include "TADlgColorThresholdAdjustment.h"
#include "TADlgColorThresholdOutput.h"
#pragma endregion Includes

namespace SvTo
{
class SVColorToolClass;
}

class SVROIFigureEditor;


class TADlgColorThresholdSheet : public CPropertySheet
{
public:
	TADlgColorThresholdSheet(SvTo::SVColorToolClass* pTool, UINT nIDCaption, CWnd* pParentWnd = nullptr, UINT iSelectPage = 0);
	TADlgColorThresholdSheet(SvTo::SVColorToolClass* pTool, LPCTSTR pszCaption, CWnd* pParentWnd = nullptr, UINT iSelectPage = 0);

	virtual ~TADlgColorThresholdSheet();

	SvTo::SVColorToolClass* GetTool() { return m_pTool; }

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(TADlgColorThresholdSheet)
	public:
	virtual BOOL OnInitDialog() override;
	//}}AFX_VIRTUAL

// Implementation
	// Generated message map functions

protected:
	//{{AFX_MSG(TADlgColorThresholdSheet)
		afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	TADlgColorThresholdROI		mSheetROI;	
	TADlgColorThresholdAdjustment	mSheetBand[SvDef::BandNumber];
	TADlgColorThresholdOutput		mSheetOutput;

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
