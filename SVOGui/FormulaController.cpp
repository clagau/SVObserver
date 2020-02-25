//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader, Markt Schwaben
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : FormulaController
//* .File Name       : $Workfile:   FormulaController.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.5  $
//* .Check In Date   : $Date:   27 Aug 2014 01:22:48  $
//******************************************************************************
//Description:  FormulaController is the class to get/set and
//              validate equation objects inside SVObserver.
//			    From outside SVObserver (in the GUI), use IFormulaController.

#pragma region Includes
#include "Stdafx.h"
//Moved to precompiled header: #include <iterator>
//Moved to precompiled header: #include <string>
#include "FormulaController.h"
#include "InspectionCommands/CommandExternalHelper.h"
#include "ObjectSelectorLibrary\ObjectTreeGenerator.h"
#include "BoundValue.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVMessage/SVMessage.h"
#include "Definitions/StringTypeDef.h"
#include "SVProtoBuf/ConverterHelper.h"
#include "SVUtilityLibrary/StringHelper.h"
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
#pragma region Constructor
FormulaController::FormulaController(const SVGUID& rInspectionID, const SVGUID& rTaskObjectID, const SVGUID& rEquationID)
	: m_InspectionID {rInspectionID}
	, m_TaskObjectID {rTaskObjectID}
	, m_EquationID {rEquationID}
	, m_EnableID { SvPb::ToolEnabledEId }
	, m_isConditional {false}
	, m_Values {SvOg::BoundValues{rInspectionID, rTaskObjectID}}
	, m_EquationValues {SvOg::BoundValues{rInspectionID, rEquationID}}
{
	Init();
}

FormulaController::FormulaController(const SVGUID& rInspectionID, const SVGUID& rTaskObjectID, const SvDef::SVObjectTypeInfoStruct& rInfo)
	: m_InspectionID {rInspectionID}
	, m_TaskObjectID {rTaskObjectID}
	, m_EnableID { SvPb::ToolEnabledEId }
	, m_Info{rInfo}
	, m_isConditional {SvPb::SVConditionalObjectType == rInfo.m_SubType}
	, m_Values {SvOg::BoundValues {rInspectionID, rTaskObjectID, !m_isConditional}}
	, m_EquationValues {SvOg::BoundValues {rInspectionID, GUID_NULL, !m_isConditional}}
{
	Init();
}
#pragma endregion Constructor

#pragma region Destructor
FormulaController::~FormulaController()
{
}
#pragma endregion Destructor

#pragma region Public Methods
#pragma region virtual Methods IFormulaController
std::string FormulaController::GetInspectionName() const
{
	std::string inspectionName;
	SvPb::InspectionCmdMsgs request, response;
	SvPb::GetObjectParametersRequest* pGetObjectNameRequest = request.mutable_getobjectparametersrequest();

	SvPb::SetGuidInProtoBytes(pGetObjectNameRequest->mutable_objectid(), m_InspectionID);
	HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, request, &response);
	if (S_OK == hr && response.has_getobjectparametersresponse())
	{
		inspectionName = response.getobjectparametersresponse().name();
	}

	return inspectionName;
}

std::string FormulaController::GetEquationText() const
{
	std::string equationText;
	SvPb::InspectionCmdMsgs Request, Response;
	SvPb::GetEquationRequest* pEquationRequest = Request.mutable_getequationrequest();
	SvPb::SetGuidInProtoBytes(pEquationRequest->mutable_objectid(), m_EquationID);
	HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, Request, &Response);
	if (S_OK == hr&& Response.has_getequationresponse() )
	{
		equationText = Response.getequationresponse().equationtext();
	}
	else
	{
		SvDef::StringVector msgList;
		msgList.push_back(SvUl::Format(_T("%d"), hr));
		SvStl::MessageMgrStd e(SvStl::MsgType::Log);
		e.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_UnknownCommandError, SvStl::SourceFileParams(StdMessageParams));
		ASSERT(false);
	}
	return equationText;
}

std::string FormulaController::GetEquationName() const
{
	std::string name;
	SvPb::InspectionCmdMsgs request, response;
	SvPb::GetObjectParametersRequest* pGetObjectNameRequest = request.mutable_getobjectparametersrequest();

	SvPb::SetGuidInProtoBytes(pGetObjectNameRequest->mutable_objectid(), m_EquationID);
	HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, request, &response);
	if (S_OK == hr && response.has_getobjectparametersresponse())
	{
		name = response.getobjectparametersresponse().name();
	}

	return name;
}

HRESULT FormulaController::SetEquationName(const std::string& rNewName)
{
	SvPb::InspectionCmdMsgs request, response;
	SvPb::SetObjectNameRequest* pSetObjectNameRequest = request.mutable_setobjectnamerequest();

	SvPb::SetGuidInProtoBytes(pSetObjectNameRequest->mutable_objectid(), m_EquationID);
	pSetObjectNameRequest->set_objectname(rNewName.c_str());
	return SvCmd::InspectionCommands(m_InspectionID, request, &response);
}

void FormulaController::BuildSelectableItems()
{
	SvPb::InspectionCmdMsgs request, response;
	*request.mutable_getobjectselectoritemsrequest() = SvCmd::createObjectSelectorRequest(
		{SvPb::ObjectSelectorType::globalConstantItems, SvPb::ObjectSelectorType::ppqItems, SvPb::ObjectSelectorType::toolsetItems},
		m_InspectionID, SvPb::selectableForEquation, GUID_NULL, true);
	SvCmd::InspectionCommands(m_InspectionID, request, &response);
	if (response.has_getobjectselectoritemsresponse())
	{
		SvOsl::ObjectTreeGenerator::Instance().insertTreeObjects(response.getobjectselectoritemsresponse().tree());
	}
}

HRESULT FormulaController::IsOwnerAndEquationEnabled(bool& ownerEnabled, bool& equationEnabled) const
{
	//Only when the controller is of type SvDef::SVConditionalObjectType
	if (m_isConditional)
	{
		ownerEnabled = m_Values.Get<bool>(m_EnableID);
		equationEnabled = m_EquationValues.Get<bool>(SvPb::EquationEnabledEId);
	}
	else
	{
		ownerEnabled = true,
			equationEnabled = true;
	}
	return S_OK;
}

HRESULT FormulaController::SetOwnerAndEquationEnabled(bool ownerEnabled, bool equationEnabled)
{
	HRESULT hr = S_OK;
	if (m_isConditional)
	{
		m_Values.Set<bool>(m_EnableID, ownerEnabled);
		m_EquationValues.Set<bool>(SvPb::EquationEnabledEId, equationEnabled);
		m_Values.Commit();
		m_EquationValues.Commit();
	}
	return hr;
}

int FormulaController::ValidateEquation(const std::string& equationString, double& result, bool bSetValue, SvStl::MessageContainerVector& rErrorMessages) const
{
	int retValue = validateSuccessful;
	SvPb::InspectionCmdMsgs requestMessage;
	SvPb::InspectionCmdMsgs responseMessage;
	
	SvPb::ValidateAndSetEquationRequest* pRequest = requestMessage.mutable_validateandsetequationrequest();
	
	SvPb::SetGuidInProtoBytes(pRequest->mutable_objectid(), m_EquationID);
	pRequest->set_equationtext(equationString);
	pRequest->set_bsetvalue(bSetValue);
	HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestMessage, &responseMessage);
	if (S_OK == hr && responseMessage.has_validateandsetequationresponse())
	{
		retValue = responseMessage.validateandsetequationresponse().validatestatus();
		result = responseMessage.validateandsetequationresponse().result();
		rErrorMessages = SvCmd::setMessageContainerFromMessagePB(responseMessage.validateandsetequationresponse().messages());;

		if (validateSuccessful == retValue && bSetValue)
		{
			SvPb::InspectionCmdMsgs Request, Response;
			SvPb::ResetObjectRequest* pResetObjectRequest = Request.mutable_resetobjectrequest();
			SvPb::SetGuidInProtoBytes(pResetObjectRequest->mutable_objectid(), m_TaskObjectID);
			hr = SvCmd::InspectionCommands(m_InspectionID, Request, &Response);
			if (S_OK != hr)
			{
				retValue = resetFailed;
			}
		}
	}
	else
	{
		SvDef::StringVector msgList;
		msgList.push_back(SvUl::Format(_T("%d"), hr));
		SvStl::MessageMgrStd e(SvStl::MsgType::Log);
		e.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_UnknownCommandError, SvStl::SourceFileParams(StdMessageParams));
		ASSERT(false);
	}
	return retValue;
}
#pragma endregion virtual Methods IFormulaController

#pragma endregion Public Methods

#pragma region Private Methods
void FormulaController::Init()
{
	SvPb::InspectionCmdMsgs requestMessage, responseMessage;
	auto* pRequest = requestMessage.mutable_getobjectidrequest()->mutable_info();

	// check for Math Container...
	if (SvPb::SVMathContainerObjectType == m_Info.m_ObjectType)
	{
		SvPb::SetGuidInProtoBytes(pRequest->mutable_ownerid(), m_TaskObjectID);
		SvCmd::setTypeInfos(m_Info, *pRequest->mutable_infostruct());
		HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestMessage, &responseMessage);
		if (S_OK == hr && responseMessage.has_getobjectidresponse())
		{
			GUID containerID = SvPb::GetGuidFromProtoBytes(responseMessage.getobjectidresponse().objectid());
			SvPb::SetGuidInProtoBytes(pRequest->mutable_ownerid(), containerID);
			// Get the Equation
			SvDef::SVObjectTypeInfoStruct info(SvPb::SVEquationObjectType, SvPb::SVMathEquationObjectType);
			SvCmd::setTypeInfos(info, *pRequest->mutable_infostruct());

			hr = SvCmd::InspectionCommands(m_InspectionID, requestMessage, &responseMessage);
			if (S_OK == hr && responseMessage.has_getobjectidresponse())
			{
				m_EquationID = SvPb::GetGuidFromProtoBytes(responseMessage.getobjectidresponse().objectid());
			}
			else
			{
				SvDef::StringVector msgList;
				msgList.push_back(SvUl::Format(_T("%d"), hr));
				SvStl::MessageMgrStd e(SvStl::MsgType::Log);
				e.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_UnknownCommandError, SvStl::SourceFileParams(StdMessageParams));
				assert(false);
			}
		}
	}
	else if (SvPb::SVNotSetObjectType != m_Info.m_ObjectType)
	{
		SvPb::SetGuidInProtoBytes(pRequest->mutable_ownerid(), m_TaskObjectID);
		SvCmd::setTypeInfos(m_Info, *pRequest->mutable_infostruct());

		HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestMessage, &responseMessage);
		if (S_OK == hr && responseMessage.has_getobjectidresponse())
		{
			m_EquationID = SvPb::GetGuidFromProtoBytes(responseMessage.getobjectidresponse().objectid());
		}
		else
		{
			SvDef::StringVector msgList;
			msgList.push_back(SvUl::Format(_T("%d"), hr));
			SvStl::MessageMgrStd e(SvStl::MsgType::Log);
			e.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_UnknownCommandError, SvStl::SourceFileParams(StdMessageParams));
			assert(false);
		}
	}
	if (m_isConditional)
	{
		SvPb::InspectionCmdMsgs request, response;
		SvPb::GetObjectParametersRequest* pGetObjectNameRequest = request.mutable_getobjectparametersrequest();

		SvPb::SetGuidInProtoBytes(pGetObjectNameRequest->mutable_objectid(), m_TaskObjectID);
		HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, request, &response);
		if (S_OK == hr && response.has_getobjectparametersresponse())
		{
			if (SvPb::SVToolSetObjectType == response.getobjectparametersresponse().typeinfo().objecttype())
			{
				m_EnableID = SvPb::ToolSetEnabledEId;
			}
		}
		m_EquationValues.SetTaskID(m_EquationID);

		m_Values.Init();
		m_EquationValues.Init();
	}
}
#pragma endregion Private Methods
} //namespace SvOg

