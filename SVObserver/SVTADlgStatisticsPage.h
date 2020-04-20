//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolAdjustmentDialogStatisticsPageClass
//* .File Name       : $Workfile:   SVTADlgStatisticsPage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 11:22:48  $
//******************************************************************************

#pragma once

#pragma region Includes
#pragma endregion Includes

namespace SvTo
{
class SVStatisticsToolClass;
}

class SVToolAdjustmentDialogSheetClass;
class SVToolSetClass;

class SVToolAdjustmentDialogStatisticsPageClass : public CPropertyPage
{
public:
	SVToolAdjustmentDialogStatisticsPageClass(uint32_t inspectionId, uint32_t taskObjectId, SVToolAdjustmentDialogSheetClass* pParent);
	virtual ~SVToolAdjustmentDialogStatisticsPageClass();

	void UpdateStatisticsParameters();
	void OnOK();
	virtual BOOL OnSetActive() override;
	virtual BOOL OnKillActive() override;

protected:
	void getParameters();
	void initListBox(CListBox* pListBox, TCHAR atcTestChar);

protected:
	//{{AFX_MSG(SVToolAdjustmentDialogStatisticsPageClass)
	virtual BOOL OnInitDialog() override;
	afx_msg void OnButtonAdd();
	afx_msg void OnButtonRemove();
	afx_msg void OnSetRange();
	afx_msg void OnPublishButton();
	afx_msg void OnBtnObjectPicker();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	//{{AFX_VIRTUAL(SVToolAdjustmentDialogStatisticsPageClass)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;
	//}}AFX_VIRTUAL

public:
	//{{AFX_DATA(SVToolAdjustmentDialogStatisticsPageClass)
	enum { IDD = IDD_TA_STATISTICS_PARAMETERS_DIALOG };
	CListBox    m_lbSelectedList;
	CListBox    m_lbAvailableList;
	CString     m_strTestValue;
	CString     m_strVariableToMonitor;
	//}}AFX_DATA

protected:
	SVToolAdjustmentDialogSheetClass* m_pParent;
	SvTo::SVStatisticsToolClass* m_pTool;
	CString                 m_strFullNameOfVariable;
};

//{{AFX_INSERT_LOCATION}}
// DevStudio inserts additional declarations immediate in front of the preceding line
////////////////////////////////////////////////////////////////////////////////


