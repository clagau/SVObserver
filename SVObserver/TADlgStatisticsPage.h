//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : TADlgStatisticsPage
//* .File Name       : $Workfile:   TADlgStatisticsPage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 11:22:48  $
//******************************************************************************

#pragma once

#pragma region Includes
#pragma endregion Includes

namespace SvTo
{
class SVStatTool;
}

class TADlgSheetClass;
class SVToolSet;

class TADlgStatisticsPage : public CPropertyPage
{
public:
	TADlgStatisticsPage(uint32_t inspectionId, uint32_t taskObjectId, TADlgSheetClass* pParent);
	virtual ~TADlgStatisticsPage();

	void UpdateStatisticsParameters();
	void OnOK();
	virtual BOOL OnSetActive() override;
	virtual BOOL OnKillActive() override;

protected:
	void getParameters();
	void initListBox(CListBox* pListBox, TCHAR atcTestChar);

protected:
	//{{AFX_MSG(TADlgStatisticsPage)
	virtual BOOL OnInitDialog() override;
	afx_msg void OnButtonAdd();
	afx_msg void OnButtonRemove();
	afx_msg void OnSetRange();
	afx_msg void OnBtnObjectPicker();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	//{{AFX_VIRTUAL(TADlgStatisticsPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;
	//}}AFX_VIRTUAL

public:
	//{{AFX_DATA(TADlgStatisticsPage)
	enum { IDD = IDD_TA_STATISTICS_PARAMETERS_DIALOG };
	CListBox    m_lbSelectedList;
	CListBox    m_lbAvailableList;
	CString     m_strTestValue = _T("");;
	CString     m_strVariableToMonitor = _T("");;
	//}}AFX_DATA

protected:
	TADlgSheetClass* m_pParent;
	SvTo::SVStatTool* m_pTool;
	CString                 m_strFullNameOfVariable;
};

//{{AFX_INSERT_LOCATION}}
// DevStudio inserts additional declarations immediate in front of the preceding line
////////////////////////////////////////////////////////////////////////////////


