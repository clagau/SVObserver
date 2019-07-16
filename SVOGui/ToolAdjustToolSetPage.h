//*****************************************************************************
/// \copyright (c) 2019,2019 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
/// \file ToolAdjustToolSetPage.h
//*****************************************************************************
/// Tool adjust page for the Tool Set parameters
//*****************************************************************************

#pragma once

#pragma region Includes
#include "ValuesAccessor.h"
#include "DataController.h"
#pragma endregion Includes

namespace SvOg
{
class ToolAdjustToolSetPage : public CPropertyPage
{
public:
	ToolAdjustToolSetPage(const SVGUID& rInspectionID, const SVGUID& rTaskObjectID);
	virtual ~ToolAdjustToolSetPage() = default;

	HRESULT SetInspectionData();

protected:
	virtual BOOL OnInitDialog() override;
	afx_msg void OnBtnObjectPicker();

	DECLARE_MESSAGE_MAP()

private:
	virtual void DoDataExchange(CDataExchange* pDX) override;

	enum { IDD = IDD_TA_TOOL_SET_DIALOG };
	CString     m_InspectedObjectID;

	SVGUID m_InspectionID;
	SVGUID m_TaskObjectID;
	typedef SvOg::DataController<SvOg::ValuesAccessor, SvOg::ValuesAccessor::value_type> Controller;
	Controller m_Values;
};

} //namespace SvOg


