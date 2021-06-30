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
#include "Definitions/GlobalConst.h"
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
	, m_searchAreas{SvPb::SearchArea::globalConstantItems, SvPb::SearchArea::toolsetItems }
{
}

ObjectSelectorController::~ObjectSelectorController()
{
}
#pragma endregion Constructor

bool ObjectSelectorController::Show(std::string& rName, const std::string& rTitle, CWnd* pParent, SvPb::ObjectSelectorType type, SvPb::GetObjectSelectorItemsRequest::FilterCase filter, uint32_t importantObjectForStopAtClosed /*= SvDef::InvalidObjectId*/)
{
	SvPb::InspectionCmdRequest requestCmd;
	*requestCmd.mutable_getobjectselectoritemsrequest() = SvCmd::createObjectSelectorRequest(
		m_searchAreas, m_InspectionID, m_objectAttributes, m_InstanceID, false, type, filter, importantObjectForStopAtClosed);
	return Show(rName, rTitle, pParent, requestCmd);
}

bool ObjectSelectorController::Show(std::string& rName, const std::string& rTitle, CWnd* pParent, SvPb::ObjectSelectorType type, const std::vector<uint32_t>& excludeSameLineageVector, uint32_t importantObjectForStopAtClosed /*= SvDef::InvalidObjectId*/)
{
	SvPb::InspectionCmdRequest requestCmd;
	*requestCmd.mutable_getobjectselectoritemsrequest() = SvCmd::createObjectSelectorRequest(
		m_searchAreas, m_InspectionID, m_objectAttributes, m_InstanceID, false, type, excludeSameLineageVector, importantObjectForStopAtClosed);
	return Show(rName, rTitle, pParent, requestCmd);
}

#pragma region Private Methods
bool ObjectSelectorController::Show(std::string& rName, const std::string& rTitle, CWnd* pParent, const SvPb::InspectionCmdRequest& rRequestCmd)
{
	bool result = false;

	SvPb::InspectionCmdResponse responseCmd;
	SvCmd::InspectionCommands(m_InspectionID, rRequestCmd, &responseCmd);

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
		SVObjectReference objectRef{ SvOsl::ObjectTreeGenerator::Instance().getSingleObjectResult() };

		rName = objectRef.GetObjectNameBeforeObjectType(SvPb::SVInspectionObjectType, true);

		//@TODO[gra][10.10][29.06.2021]: This quick fix needs to be done properly using object reference (Inspection name is only added to Linked Values when preceded with Tool Set!
		if (std::string::npos != rName.find(SvDef::FqnRemoteInput[1]) || std::string::npos != rName.find(SvDef::FqnDioInput[1]))
		{
			rName = objectRef.GetCompleteName();
		}
		result = true;
	}

	return result;
}
#pragma endregion Private Methods
} //namespace SvOg
