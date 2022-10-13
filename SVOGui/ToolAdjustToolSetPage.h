//*****************************************************************************
/// \copyright (c) 2019,2019 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
/// \file ToolAdjustToolSetPage.h
//*****************************************************************************
/// Tool adjust page for the Tool Set parameters
//*****************************************************************************

#pragma once

#pragma region Includes
#include "SVOGuiUtility/DataController.h"
#pragma endregion Includes

namespace SvOg
{
class ToolAdjustToolSetPage : public CPropertyPage
{
public:
	ToolAdjustToolSetPage(uint32_t inspectionId, uint32_t taskObjectId);
	virtual ~ToolAdjustToolSetPage() = default;

	HRESULT SetInspectionData();

protected:
	virtual BOOL OnInitDialog() override;
	afx_msg void OnBtnObjectPicker();

	DECLARE_MESSAGE_MAP()

private:
	virtual void DoDataExchange(CDataExchange* pDX) override;

	enum { IDD = IDD_TA_TOOL_SET_DIALOG };
	std::string m_inspectedObjectId_IdString;
	CString     m_inspectedObjectId_NameString;

	uint32_t m_InspectionID;
	uint32_t m_TaskObjectID;
	SvOgu::ValueController m_values;
};

} //namespace SvOg


