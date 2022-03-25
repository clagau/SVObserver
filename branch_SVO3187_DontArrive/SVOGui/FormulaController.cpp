//******************************************************************************
//* COPYRIGHT (c) 2013 by Körber Pharma Inspection GmbH. All Rights Reserved
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
FormulaController::FormulaController(uint32_t inspectionID, uint32_t taskObjectID, uint32_t equationID)
	: m_InspectionID {inspectionID}
	, m_TaskObjectID {taskObjectID}
	, m_EquationID {equationID}
	, m_EnableID {SvPb::ToolEnabledEId}
	, m_isConditional {false}
	, m_values {SvOg::BoundValues{ inspectionID, taskObjectID }}
	, m_EquationValues {SvOg::BoundValues{ inspectionID, equationID }}
{
	Init();
}

FormulaController::FormulaController(uint32_t inspectionID, uint32_t taskObjectID, const SvDef::SVObjectTypeInfoStruct& rInfo)
	: m_InspectionID {inspectionID}
	, m_TaskObjectID {taskObjectID}
	, m_EnableID {SvPb::ToolEnabledEId}
	, m_Info {rInfo}
	, m_isConditional {SvPb::SVConditionalObjectType == rInfo.m_SubType}
	, m_values {SvOg::BoundValues { inspectionID, taskObjectID, !m_isConditional}}
	, m_EquationValues {SvOg::BoundValues { inspectionID, SvDef::InvalidObjectId, !m_isConditional}}
{
	Init();
}
#pragma endregion Constructor

#pragma region Destructor
FormulaController::~FormulaController()
{}
#pragma endregion Destructor

#pragma region Public Methods
#pragma region virtual Methods IFormulaController
std::string FormulaController::GetInspectionName() const
{
	std::string inspectionName;
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_getobjectparametersrequest();
	pRequest->set_objectid(m_InspectionID);

	HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
	if (S_OK == hr && responseCmd.has_getobjectparametersresponse())
	{
		inspectionName = responseCmd.getobjectparametersresponse().name();
	}

	return inspectionName;
}

std::string FormulaController::GetEquationText() const
{
	std::string equationText;
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_getequationrequest();
	pRequest->set_objectid(m_EquationID);

	HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
	if (S_OK == hr && responseCmd.has_getequationresponse())
	{
		equationText = responseCmd.getequationresponse().equationtext();
	}
	else
	{
		SvDef::StringVector msgList;
		msgList.push_back(SvUl::Format(_T("%d"), hr));
		SvStl::MessageManager e(SvStl::MsgType::Log);
		e.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_UnknownCommandError, SvStl::SourceFileParams(StdMessageParams));
		assert(false);
	}
	return equationText;
}

std::string FormulaController::GetEquationName() const
{
	std::string name;
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_getobjectparametersrequest();
	pRequest->set_objectid(m_EquationID);

	HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
	if (S_OK == hr && responseCmd.has_getobjectparametersresponse())
	{
		name = responseCmd.getobjectparametersresponse().name();
	}

	return name;
}

HRESULT FormulaController::SetEquationName(const std::string& rNewName)
{
	SvPb::InspectionCmdRequest requestCmd;
	auto* pRequest = requestCmd.mutable_setobjectnamerequest();
	pRequest->set_objectid(m_EquationID);
	pRequest->set_objectname(rNewName.c_str());

	return SvCmd::InspectionCommands(m_InspectionID, requestCmd, nullptr);
}

void FormulaController::BuildSelectableItems()
{
	SvPb::InspectionCmdRequest requestCmd;
	requestCmd.mutable_getobjectselectoritems2request()->set_objectid(m_EquationID);
	SvPb::InspectionCmdResponse responseCmd;
	SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
	if (responseCmd.has_getobjectselectoritemsresponse())
	{
		SvOsl::ObjectTreeGenerator::Instance().insertTreeObjects(responseCmd.getobjectselectoritemsresponse().tree());
	}
}

HRESULT FormulaController::IsOwnerAndEquationEnabled(bool& ownerEnabled, bool& equationEnabled) const
{
	//Only when the controller is of type SvDef::SVConditionalObjectType
	if (m_isConditional)
	{
		ownerEnabled = m_values.Get<bool>(m_EnableID);
		equationEnabled = m_EquationValues.Get<bool>(SvPb::EquationEnabledEId);
	}
	else
	{
		ownerEnabled = true,
			equationEnabled = true;
	}
	return S_OK;
}

void FormulaController::SetOwnerAndEquationEnabled(bool ownerEnabled, bool equationEnabled)
{
	if (m_isConditional)
	{
		m_values.Set<bool>(m_EnableID, ownerEnabled);
		m_EquationValues.Set<bool>(SvPb::EquationEnabledEId, equationEnabled);
		m_values.Commit();
		m_EquationValues.Commit();
	}
}

bool FormulaController::getEditModeFreezeFlag() const
{
	if (m_isConditional)
	{
		return m_values.Get<bool>(SvPb::ConditionalEditFreezeFlagEId);
	}
	return false;
}

void FormulaController::setEditModeFreezeFlag(bool flag)
{
	if (m_isConditional)
	{
		m_values.Set<bool>(SvPb::ConditionalEditFreezeFlagEId, flag);
		m_values.Commit();
	}
}


// cppcheck-suppress unusedFunction
bool  FormulaController::ResetOwner()const
{
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pResetObjectRequest = requestCmd.mutable_resetobjectrequest();
	pResetObjectRequest->set_objectid(m_TaskObjectID);
	auto hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, nullptr);
	return (hr == S_OK);

}
int FormulaController::ValidateEquation_NoReset(const std::string& equationString, double& result, bool bSetValue, SvStl::MessageContainerVector& rErrorMessages) const
{
	int retValue = validateSuccessful;
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_validateandsetequationrequest();
	pRequest->set_objectid(m_EquationID);
	pRequest->set_equationtext(equationString);
	pRequest->set_bsetvalue(bSetValue);

	HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
	if (S_OK == hr && responseCmd.has_validateandsetequationresponse())
	{
		retValue = responseCmd.validateandsetequationresponse().validatestatus();
		result = responseCmd.validateandsetequationresponse().result();
		rErrorMessages = SvPb::convertProtobufToMessageVector(responseCmd.validateandsetequationresponse().messages());;
		
	}
	else
	{
		SvDef::StringVector msgList;
		msgList.push_back(SvUl::Format(_T("%d"), hr));
		SvStl::MessageManager e(SvStl::MsgType::Log);
		e.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_UnknownCommandError, SvStl::SourceFileParams(StdMessageParams));
		assert(false);
	}
	return retValue;
}

int FormulaController::ValidateEquation(const std::string& equationString, double& result, bool bSetValue, SvStl::MessageContainerVector& rErrorMessages) const
{
	int retValue = ValidateEquation_NoReset(equationString, result, bSetValue, rErrorMessages);
	if (validateSuccessful == retValue && bSetValue)
	{
		SvPb::InspectionCmdRequest requestCmd;
		auto* pResetObjectRequest = requestCmd.mutable_resetobjectrequest();
		pResetObjectRequest->set_objectid(m_TaskObjectID);
		auto hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, nullptr);
		if (S_OK != hr)
		{
			retValue = resetFailed;
		}
	}

	return retValue;
}
#pragma endregion virtual Methods IFormulaController

#pragma endregion Public Methods

#pragma region Private Methods
void FormulaController::Init()
{
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_getobjectidrequest()->mutable_info();

	// check for Math Container...
	if (SvPb::SVMathContainerObjectType == m_Info.m_ObjectType)
	{
		pRequest->set_ownerid(m_TaskObjectID);
		SvCmd::setTypeInfos(m_Info, *pRequest->mutable_infostruct());
		HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
		if (S_OK == hr && responseCmd.has_getobjectidresponse())
		{
			pRequest->set_ownerid(responseCmd.getobjectidresponse().objectid());
			// Get the Equation
			SvDef::SVObjectTypeInfoStruct info(SvPb::SVEquationObjectType, SvPb::SVMathEquationObjectType);
			SvCmd::setTypeInfos(info, *pRequest->mutable_infostruct());

			hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
			if (S_OK == hr && responseCmd.has_getobjectidresponse())
			{
				m_EquationID = responseCmd.getobjectidresponse().objectid();
			}
			else
			{
				SvDef::StringVector msgList;
				msgList.push_back(SvUl::Format(_T("%d"), hr));
				SvStl::MessageManager e(SvStl::MsgType::Log);
				e.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_UnknownCommandError, SvStl::SourceFileParams(StdMessageParams));
				assert(false);
			}
		}
	}
	else if (SvPb::SVNotSetObjectType != m_Info.m_ObjectType)
	{
		pRequest->set_ownerid(m_TaskObjectID);
		SvCmd::setTypeInfos(m_Info, *pRequest->mutable_infostruct());

		HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
		if (S_OK == hr && responseCmd.has_getobjectidresponse())
		{
			m_EquationID = responseCmd.getobjectidresponse().objectid();
		}
		else
		{
			SvDef::StringVector msgList;
			msgList.push_back(SvUl::Format(_T("%d"), hr));
			SvStl::MessageManager e(SvStl::MsgType::Log);
			e.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_UnknownCommandError, SvStl::SourceFileParams(StdMessageParams));
			assert(false);
		}
	}
	if (m_isConditional)
	{
		requestCmd.Clear();
		responseCmd.Clear();
		auto* pGetObjParameterRequest = requestCmd.mutable_getobjectparametersrequest();
		pGetObjParameterRequest->set_objectid(m_TaskObjectID);

		HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
		if (S_OK == hr && responseCmd.has_getobjectparametersresponse())
		{
			if (SvPb::SVToolSetObjectType == responseCmd.getobjectparametersresponse().typeinfo().objecttype())
			{
				m_EnableID = SvPb::ToolSetEnabledEId;
			}
		}
		m_EquationValues.SetTaskID(m_EquationID);

		m_values.Init();
		m_EquationValues.Init();
	}
}
#pragma endregion Private Methods
} //namespace SvOg

