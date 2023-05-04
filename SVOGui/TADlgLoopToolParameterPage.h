//*****************************************************************************
/// \copyright (c) 2018,2018 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file TADlgLoopToolParameterPage.h
/// All Rights Reserved 
//*****************************************************************************
/// Property page for the tool adjustment sheet for parameter in the LoopTool.
//*****************************************************************************

#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#include "ISVPropertyPageDialog.h"
#include "SVOGuiUtility/DataController.h"
#include "SVOGuiUtility/LinkedValueWidgetHelper.h"
#pragma endregion Includes


namespace SvOg
{
class TADlgLoopToolParameterPage : public CPropertyPage, public ISVPropertyPageDialog
{

protected:
	//{{AFX_DATA(TADlgLoopToolParameterPage)
	enum { IDD = IDD_TA_LOOPTOOL_PARAMETER_DIALOG };
	//}}AFX_DATA
public:
	TADlgLoopToolParameterPage(uint32_t inspectionID, uint32_t taskObjectID);
	virtual ~TADlgLoopToolParameterPage();
public:
	virtual bool QueryAllowExit() override; //from ISVPropertyPageDialog
	DECLARE_MESSAGE_MAP()
	//{{AFX_VIRTUAL(TADlgTableParameterPage)
protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL
protected:
	//{{AFX_MSG(TADlgTableParameterPage)
	virtual BOOL OnInitDialog() override;
	virtual BOOL OnKillActive() override;
	void OnBnClickedButtonLastLoopCount();
	void OnBnClickedButtonBreakCondition();
	void OnKillFocusLastLoopCount();
	void OnKillFocusBreakCondition();
	//}}AFX_MSG
private:
	/// Set the date of the page in the business logic
	/// \returns HRESULT
	HRESULT SetPageData();
	//Reset the inspection.
	void resetInspection();
private:
	CButton m_ButtonLoopsValue;
	CEdit m_EditLoopsValue;
	std::unique_ptr<SvOgu::LinkedValueWidgetHelper> m_LoopsValueWidget;
	CButton m_ButtonBreakCondition;
	CEdit m_EditBreakCondition;
	std::unique_ptr<SvOgu::LinkedValueWidgetHelper> m_BreakConditionWidget;


	long m_MaxLoopCount;
	uint32_t m_InspectionID;
	uint32_t m_TaskObjectID;
	SvOgu::ValueController m_values;
};
} //namespace SvOg
