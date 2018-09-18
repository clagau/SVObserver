//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// \file ObjectSelectorController.cpp
/// All Rights Reserved 
//*****************************************************************************
/// Controller to display the object selector and return the selected result.
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "ObjectSelectorController.h"
#include "InspectionCommands/BuildSelectableItems.h"
#include "InspectionCommands/GetObjectName.h"
#include "InspectionCommands/GetPPQObjectName.h"
#include "InspectionCommands/GetInstanceIDByTypeInfo.h"
#include "ObjectSelectorLibrary/ObjectTreeGenerator.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SvOg
{

#pragma region Constructor
ObjectSelectorController::ObjectSelectorController(const SVGUID& rInspectionID, const SVGUID& rInstanceID)
	: m_InspectionID(rInspectionID)
	, m_InstanceID(rInstanceID)
{
}

ObjectSelectorController::~ObjectSelectorController()
{
}
#pragma endregion Constructor

bool ObjectSelectorController::Show(std::string& rName, const std::string& rTitle, CWnd* pParent, const SVGUID& rInstanceId, SvCmd::SelectorFilterTypeEnum FilterType)
{
	bool result = false;
	std::string InspectionName = GetInspectionName();
	std::string PPQName = GetPPQName();

	SvOsl::ObjectTreeGenerator::Instance().setLocationFilter(SvOsl::ObjectTreeGenerator::FilterInput, InspectionName, std::string(_T("")));
	SvOsl::ObjectTreeGenerator::Instance().setLocationFilter(SvOsl::ObjectTreeGenerator::FilterOutput, InspectionName, std::string(_T("")));
	SvOsl::ObjectTreeGenerator::Instance().setLocationFilter(SvOsl::ObjectTreeGenerator::FilterOutput, PPQName, std::string(_T("")));
	SvOsl::ObjectTreeGenerator::Instance().setSelectorType(SvOsl::ObjectTreeGenerator::TypeSingleObject);

	SVGUID InstanceGuid = m_InstanceID;
	if (GUID_NULL == InstanceGuid)
	{
		InstanceGuid = GetToolSetGUID();
	}
	SvCmd::SelectorOptions BuildOptions {{SvCmd::ObjectSelectorType::globalConstantItems, SvCmd::ObjectSelectorType::toolsetItems},
		m_InspectionID, SvDef::SV_SELECTABLE_FOR_EQUATION, InstanceGuid};
	SvCl::SelectorItemVector SelectorItems;
	SvCmd::BuildSelectableItems(BuildOptions, std::back_inserter(SelectorItems), FilterType);
	SvOsl::ObjectTreeGenerator::Instance().insertTreeObjects(SelectorItems);

	if (!rName.empty())
	{
		SvDef::StringSet nameSet;
		nameSet.insert(rName);
		SvOsl::ObjectTreeGenerator::Instance().setCheckItems(nameSet);
	}

	std::string mainTabTitle = SvUl::LoadStdString(IDS_SELECT_TOOLSET_OUTPUT);
	std::string FilterTab = SvUl::LoadStdString(IDS_FILTER);

	INT_PTR Result = SvOsl::ObjectTreeGenerator::Instance().showDialog(rTitle.c_str(), mainTabTitle.c_str(), FilterTab.c_str(), pParent);

	if (IDOK == Result)
	{
		rName = SvOsl::ObjectTreeGenerator::Instance().getSingleObjectResult().m_Location.c_str();
		result = true;
	}

	return result;
}

#pragma endregion Private Methods
std::string ObjectSelectorController::GetInspectionName() const
{
	std::string inspectionName;
	typedef SvCmd::GetObjectName Command;
	typedef std::shared_ptr<Command> CommandPtr;

	CommandPtr commandPtr(new Command(m_InspectionID));
	SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
	HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
	if (S_OK == hr)
	{
		inspectionName = commandPtr->GetName();
	}
	return inspectionName;
}

std::string ObjectSelectorController::GetPPQName() const
{
	std::string PPQName;
	typedef SvCmd::GetPPQObjectName Command;
	typedef std::shared_ptr<Command> CommandPtr;

	CommandPtr commandPtr(new Command(m_InspectionID));
	SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
	HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
	if (S_OK == hr)
	{
		PPQName = commandPtr->GetName();
	}
	return PPQName;
}

GUID ObjectSelectorController::GetToolSetGUID() const
{
	GUID toolsetGUID = GUID_NULL;

	typedef SvCmd::GetInstanceIDByTypeInfo Command;
	typedef std::shared_ptr<Command> CommandPtr;

	SvDef::SVObjectTypeInfoStruct info(SvDef::SVToolSetObjectType);
	CommandPtr commandPtr = CommandPtr(new Command(m_InspectionID, info));
	SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
	HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
	if (S_OK == hr)
	{
		toolsetGUID = commandPtr->GetInstanceID();
	}

	return toolsetGUID;
}
#pragma endregion Private Methods
} //namespace SvOg
