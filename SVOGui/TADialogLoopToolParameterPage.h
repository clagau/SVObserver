//*****************************************************************************
/// \copyright (c) 2018,2018 by Seidenader Maschinenbau GmbH
/// \file TADialogLoopToolParameterPage.h
/// All Rights Reserved 
//*****************************************************************************
/// Property page for the tool adjustment sheet for parameter in the LoopTool.
//*****************************************************************************

#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#include "ISVPropertyPageDialog.h"
#include "ValuesAccessor.h"
#include "DataController.h"
#include "ObjectSelectorController.h"
#pragma endregion Includes


namespace SvOg
{
class TADialogLoopToolParameterPage : public CPropertyPage, public ISVPropertyPageDialog
{

protected:
	//{{AFX_DATA(TADialogLoopToolParameterPage)
	enum { IDD = IDD_TA_LOOPTOOL_PARAMETER_DIALOG };
	//}}AFX_DATA
public:
	TADialogLoopToolParameterPage(uint32_t inspectionID, uint32_t taskObjectID);
	virtual ~TADialogLoopToolParameterPage();
public:
	virtual bool QueryAllowExit() override; //from ISVPropertyPageDialog
	DECLARE_MESSAGE_MAP()
	//{{AFX_VIRTUAL(TADialogTableParameterPage)
protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL
protected:
	//{{AFX_MSG(TADialogTableParameterPage)
	virtual BOOL OnInitDialog() override;
	virtual BOOL OnKillActive() override;
	void OnBnClickedButtonLastLoopCount();
	void OnBnClickedButtonBreakCondition();
	//}}AFX_MSG
private:
	/// Set the date of the page in the business logic
	/// \returns HRESULT
	HRESULT SetPageData();
	//Reset the inspection.
	void resetInspection();
private:
	ObjectSelectorController m_objectSelector;
	CButton m_ButtonLoopsLink;
	CEdit m_EditLoopsValue;

	CButton m_ButtonBreakConditionLink;
	CEdit m_EditBreakCondition;


	long m_MaxLoopCount;
	uint32_t m_InspectionID;
	uint32_t m_TaskObjectID;
	typedef SvOg::DataController<SvOg::ValuesAccessor, SvOg::ValuesAccessor::value_type> Controller;
	Controller m_Values;
	CBitmap m_downArrowBitmap;
};
} //namespace SvOg
