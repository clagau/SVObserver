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
#include "InspectionCommands/CommandExternalHelper.h"
#include "ObjectSelectorLibrary/ObjectTreeGenerator.h"
#include "SVProtoBuf/ConverterHelper.h"
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
	SvPb::InspectionCmdMsgs request, response;
	SvPb::GetObjectParametersRequest* pGetObjectNameRequest = request.mutable_getobjectparametersrequest();

	SvPb::SetGuidInProtoBytes(pGetObjectNameRequest->mutable_objectid(), m_InspectionID);
	HRESULT hr = SvCmd::InspectionCommandsSynchronous(m_InspectionID, &request, &response);
	if (S_OK == hr && response.has_getobjectparametersresponse())
	{
		inspectionName = response.getobjectparametersresponse().name();
	}
	return inspectionName;
}

std::string ObjectSelectorController::GetPPQName() const
{
	SvPb::InspectionCmdMsgs request, response;
	SvPb::GetPPQNameRequest* pPPQNameRequest = request.mutable_getppqnamerequest();
	SvPb::SetGuidInProtoBytes(pPPQNameRequest->mutable_inspectionid(), m_InspectionID);
	HRESULT hr = SvCmd::InspectionCommandsSynchronous(m_InspectionID, &request, &response);
	if (S_OK == hr && response.has_getppqnameresponse())
	{
		return response.getppqnameresponse().ppqname();
	}
	return {};
}

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
