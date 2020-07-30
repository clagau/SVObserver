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
#include "InspectionCommands/CommandExternalHelper.h"
#include "ObjectSelectorLibrary/ObjectTreeGenerator.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVObjectLibrary/SVObjectReference.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SvOg
{

#pragma region Constructor
ObjectSelectorController::ObjectSelectorController(uint32_t inspectionID, uint32_t instanceID, SvPb::ObjectAttributes objectAttributes)
	: m_InspectionID(inspectionID)
	, m_InstanceID(instanceID)
	, m_objectAttributes(objectAttributes)
	, m_ItemTypes{SvPb::ObjectSelectorType::globalConstantItems, SvPb::ObjectSelectorType::toolsetItems }
{
}

ObjectSelectorController::~ObjectSelectorController()
{
}
#pragma endregion Constructor

bool ObjectSelectorController::Show(std::string& rName, const std::string& rTitle, CWnd* pParent, SvPb::SelectorFilter FilterType)
{
	bool result = false;

	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	*requestCmd.mutable_getobjectselectoritemsrequest() = SvCmd::createObjectSelectorRequest(
		m_ItemTypes,
		m_InspectionID, m_objectAttributes, m_InstanceID, false, FilterType);
	SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);

	SvOsl::ObjectTreeGenerator::Instance().setSelectorType(SvOsl::ObjectTreeGenerator::TypeSingleObject);
	if (responseCmd.has_getobjectselectoritemsresponse())
	{
		SvOsl::ObjectTreeGenerator::Instance().insertTreeObjects(responseCmd.getobjectselectoritemsresponse().tree());
	}

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
		SVObjectReference objectRef{SvOsl::ObjectTreeGenerator::Instance().getSingleObjectResult()};

		rName = objectRef.GetObjectNameBeforeObjectType(SvPb::SVInspectionObjectType, true);
		result = true;
	}

	return result;
}

#pragma endregion Private Methods
#pragma endregion Private Methods
} //namespace SvOg
