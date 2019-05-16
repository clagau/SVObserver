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
#include "SVProtoBuf/ConverterHelper.h"
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
ObjectSelectorController::ObjectSelectorController(const SVGUID& rInspectionID, const SVGUID& rInstanceID)
	: m_InspectionID(rInspectionID)
	, m_InstanceID(rInstanceID)
{
}

ObjectSelectorController::~ObjectSelectorController()
{
}
#pragma endregion Constructor

bool ObjectSelectorController::Show(std::string& rName, const std::string& rTitle, CWnd* pParent, const SVGUID& rInstanceId, SvPb::SelectorFilter FilterType)
{
	bool result = false;

	SVGUID InstanceGuid = m_InstanceID;
	if (GUID_NULL == InstanceGuid)
	{
		InstanceGuid = GetToolSetGUID();
	}
	SvPb::InspectionCmdMsgs request, response;
	*request.mutable_getobjectselectoritemsrequest() = SvCmd::createObjectSelectorRequest(
		{SvPb::ObjectSelectorType::globalConstantItems, SvPb::ObjectSelectorType::toolsetItems},
		m_InspectionID, SvPb::selectableForEquation, m_InstanceID, false, FilterType);
	SvCmd::InspectionCommandsSynchronous(m_InspectionID, &request, &response);

	SvOsl::ObjectTreeGenerator::Instance().setSelectorType(SvOsl::ObjectTreeGenerator::TypeSingleObject);
	if (response.has_getobjectselectoritemsresponse())
	{
		SvOsl::ObjectTreeGenerator::Instance().insertTreeObjects(response.getobjectselectoritemsresponse().tree());
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
GUID ObjectSelectorController::GetToolSetGUID() const
{
	GUID toolsetGUID = GUID_NULL;
	
	SvPb::InspectionCmdMsgs requestMessage, responseMessage;
	auto* pRequest = requestMessage.mutable_getobjectidrequest()->mutable_info();
	SvPb::SetGuidInProtoBytes(pRequest->mutable_ownerid(), m_InspectionID);
	pRequest->mutable_infostruct()->set_objecttype(SvPb::SVToolSetObjectType);

	HRESULT hr = SvCmd::InspectionCommandsSynchronous(m_InspectionID, &requestMessage, &responseMessage);
	if (S_OK == hr && responseMessage.has_getobjectidresponse())
	{
		toolsetGUID = SvPb::GetGuidFromProtoBytes(responseMessage.getobjectidresponse().objectid());
	}

	return toolsetGUID;
}
#pragma endregion Private Methods
} //namespace SvOg
