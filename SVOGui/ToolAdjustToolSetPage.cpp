//*****************************************************************************
/// \copyright (c) 2019,2019 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
/// \file ToolAdjustToolSetPage.cpp
//*****************************************************************************
/// Tool adjust page for the Tool Set parameters
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "ToolAdjustToolSetPage.h"
#include "InspectionCommands/CommandExternalHelper.h"
#include "SVObjectLibrary/SVObjectReference.h"
#include "ObjectSelectorLibrary/ObjectTreeGenerator.h"
#include "Definitions/ObjectNames.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

namespace SvOg
{

BEGIN_MESSAGE_MAP(ToolAdjustToolSetPage, CPropertyPage)
	//{{AFX_MSG_MAP(SVTADlgStatisticsPage)
	ON_BN_CLICKED(IDC_BTN_OBJECT_PICKER, OnBtnObjectPicker)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

ToolAdjustToolSetPage::ToolAdjustToolSetPage(uint32_t inspectionId, uint32_t taskObjectId)
	: CPropertyPage(ToolAdjustToolSetPage::IDD)
	, m_values {SvOg::BoundValues {inspectionId, taskObjectId}}
	, m_InspectionID {inspectionId}
	, m_TaskObjectID {taskObjectId}
{
}

HRESULT ToolAdjustToolSetPage::SetInspectionData()
{
	HRESULT Result {E_FAIL};

	UpdateData(true); // get data from dialog

	//We need to set the main LinkedValue object
	auto data = m_values.Get<LinkedValueData>(SvPb::InspectedObjectIDEId);
	data.m_indirectDotName = std::string{m_InspectedObjectID};
	data.m_type = SvPb::IndirectValue;
	m_values.Set<LinkedValueData>(SvPb::InspectedObjectIDEId, data);
	m_values.Commit();

	return Result;
}

void ToolAdjustToolSetPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(SVTADlgStatisticsPage)
	DDX_Text(pDX, IDC_INSPECTED_OBECT_ID, m_InspectedObjectID);
	//}}AFX_DATA_MAP
}

BOOL ToolAdjustToolSetPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	m_values.Init();

	//We need to get the linked object Id to get the dotted name
	const auto& data = m_values.Get<LinkedValueData>(SvPb::InspectedObjectIDEId);
	m_InspectedObjectID = data.m_indirectDotName.c_str();

	UpdateData(false);

	return true;
	// return TRUE unless you set the focus to a control
}

void ToolAdjustToolSetPage::OnBtnObjectPicker()
{
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	*requestCmd.mutable_getobjectselectoritemsrequest() = SvCmd::createObjectSelectorRequest(
		{ SvPb::SearchArea::toolsetItems }, m_InspectionID, SvPb::viewable, m_TaskObjectID, false, SvPb::allValueObjects, SvPb::GetObjectSelectorItemsRequest::kAttributesAllowed, m_TaskObjectID);
	SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);

	SvOsl::ObjectTreeGenerator::Instance().setSelectorType(SvOsl::ObjectTreeGenerator::SelectorTypeEnum::TypeSingleObject);
	if (responseCmd.has_getobjectselectoritemsresponse())
	{
		SvOsl::ObjectTreeGenerator::Instance().insertTreeObjects(responseCmd.getobjectselectoritemsresponse().tree());
	}

	SvDef::StringSet Items;
	Items.insert(std::string(m_InspectedObjectID));
	SvOsl::ObjectTreeGenerator::Instance().setCheckItems(Items);

	std::string ToolsetOutput = SvUl::LoadStdString(IDS_SELECT_TOOLSET_OUTPUT);
	std::string Title = SvUl::Format( _T("%s - %s"), ToolsetOutput.c_str(), SvDef::c_InspectedObjectID);
	std::string Filter = SvUl::LoadStdString( IDS_FILTER );

	INT_PTR Result = SvOsl::ObjectTreeGenerator::Instance().showDialog( Title.c_str(), ToolsetOutput.c_str(), Filter.c_str(), this );

	if( IDOK == Result )
	{
		SVObjectReference objectRef {SvOsl::ObjectTreeGenerator::Instance().getSingleObjectResult()};

		m_InspectedObjectID = objectRef.GetObjectNameToObjectType(SvPb::SVToolSetObjectType).c_str();

		UpdateData(false);

		SetInspectionData();
	}
}

} //namespace SvOg

