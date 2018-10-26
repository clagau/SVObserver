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
#include "InspectionCommands/GetInstanceIDByTypeInfo.h"
#include "InspectionCommands/GetObjectName.h"
#include "InspectionCommands/GetObjectTypeInfo.h"
#include "InspectionCommands/GetPPQObjectName.h"
#include "InspectionCommands/BuildSelectableItems.h"
#include "InspectionCommands/TaskObjectGetEmbeddedValues.h"
#include "InspectionCommands/TaskObjectSetEmbeddedValues.h"
#include "InspectionCommands/SetDefaultInputs.h"
#include "InspectionCommands/SetObjectName.h"
#include "SVObjectLibrary\SVClsIds.h"
#include "ObjectSelectorLibrary\ObjectTreeGenerator.h"
#include "SVCommandLibrary\SVObjectSynchronousCommandTemplate.h"
#include "BoundValue.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVMessage/SVMessage.h"
#include "Definitions/StringTypeDef.h"
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
	, m_EnableID {SVToolEnabledObjectGuid}
	, m_isConditional {false}
	, m_Values {SvOg::BoundValues{rInspectionID, rTaskObjectID}}
	, m_EquationValues {SvOg::BoundValues{rInspectionID, rEquationID}}
{
	Init();
}

FormulaController::FormulaController(const SVGUID& rInspectionID, const SVGUID& rTaskObjectID, const SvDef::SVObjectTypeInfoStruct& rInfo)
	: m_InspectionID {rInspectionID}
	, m_TaskObjectID {rTaskObjectID}
	, m_EnableID {SVToolEnabledObjectGuid}
	, m_Info{rInfo}
	, m_isConditional {SvDef::SVConditionalObjectType == rInfo.SubType}
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

std::string FormulaController::GetPPQName() const
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

std::string FormulaController::GetEquationText() const
{
	std::string equationText;
	SvPb::InspectionCmdMsgs Request, Response;
	SvPb::GetEquationRequest* pEquationRequest = Request.mutable_getequationrequest();
	SvPb::SetGuidInProtoBytes(pEquationRequest->mutable_objectid(), m_EquationID);
	HRESULT hr = SvCmd::InspectionCommandsSynchronous(m_InspectionID, &Request, &Response);
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
	typedef SvCmd::GetObjectName Command;
	typedef std::shared_ptr<Command> CommandPtr;

	CommandPtr commandPtr(new Command(m_EquationID));
	SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
	HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
	if (S_OK == hr)
	{
		name = commandPtr->GetName();
	}
	return name;
}

HRESULT FormulaController::SetEquationName(const std::string& rNewName)
{
	typedef SvCmd::SetObjectName Command;
	typedef std::shared_ptr<Command> CommandPtr;
	CommandPtr commandPtr {new Command(m_EquationID, rNewName.c_str())};
	SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
	return cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
}

void FormulaController::BuildSelectableItems()
{
	SvCmd::SelectorOptions BuildOptions({SvCmd::ObjectSelectorType::globalConstantItems, SvCmd::ObjectSelectorType::ppqItems, SvCmd::ObjectSelectorType::toolsetItems},
										m_InspectionID, SvDef::SV_SELECTABLE_FOR_EQUATION, m_InspectionID, true);
	SvCl::SelectorItemVector SelectorItems;
	SvCmd::BuildSelectableItems(BuildOptions, std::back_inserter(SelectorItems));
	SvOsl::ObjectTreeGenerator::Instance().insertTreeObjects(SelectorItems);
}

HRESULT FormulaController::IsOwnerAndEquationEnabled(bool& ownerEnabled, bool& equationEnabled) const
{
	//Only when the controller is of type SvDef::SVConditionalObjectType
	if (m_isConditional)
	{
		ownerEnabled = m_Values.Get<bool>(m_EnableID);
		equationEnabled = m_EquationValues.Get<bool>(SVEquationEnabledObjectGuid);
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
		m_EquationValues.Set<bool>(SVEquationEnabledObjectGuid, equationEnabled);
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
	HRESULT hr = SvCmd::InspectionCommandsSynchronous(m_InspectionID, &requestMessage, &responseMessage);
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
			hr = SvCmd::InspectionCommandsSynchronous(m_InspectionID, &Request, &Response);
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

HRESULT FormulaController::SetDefaultInputs()
{
	typedef SvCmd::SetDefaultInputs<SvOi::IInspectionProcess> Command;
	typedef std::shared_ptr<Command> CommandPtr;

	CommandPtr commandPtr(new Command(m_InspectionID));
	SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
	HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
	return hr;
}

#pragma endregion virtual Methods IFormulaController

std::string FormulaController::GetOwnerName() const
{
	std::string name;
	typedef SvCmd::GetObjectName Command;
	typedef std::shared_ptr<Command> CommandPtr;

	CommandPtr commandPtr(new Command(m_TaskObjectID));
	SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
	HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
	if (S_OK == hr)
	{
		name = commandPtr->GetName();
	}
	return name;
}
#pragma endregion Public Methods

#pragma region Private Methods
void FormulaController::Init()
{
	typedef SvCmd::GetInstanceIDByTypeInfo Command;
	typedef std::shared_ptr<Command> CommandPtr;
	// check for Math Container...
	if (SvDef::SVMathContainerObjectType == m_Info.ObjectType)
	{
		// Get the Math Container
		CommandPtr commandPtr(new Command(m_TaskObjectID, m_Info));
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		if (S_OK == hr)
		{
			// Get the Equation
			SvDef::SVObjectTypeInfoStruct info(SvDef::SVEquationObjectType, SvDef::SVMathEquationObjectType);
			GUID containerID = commandPtr->GetInstanceID();
			commandPtr = CommandPtr(new Command(containerID, info));
			SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
			HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
			if (S_OK == hr)
			{
				m_EquationID = commandPtr->GetInstanceID();
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
	else if (SvDef::SVNotSetObjectType != m_Info.ObjectType)
	{
		// Get the Equation
		CommandPtr commandPtr(new Command(m_TaskObjectID, m_Info));
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		if (S_OK == hr)
		{
			m_EquationID = commandPtr->GetInstanceID();
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
		typedef SvCmd::GetObjectTypeInfo Command;
		typedef std::shared_ptr<Command> CommandPtr;
		CommandPtr commandPtr {new Command(m_TaskObjectID)};
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(m_InspectionID, commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		if (S_OK == hr)
		{
			if (SvDef::SVToolSetObjectType == commandPtr->GetTypeInfo().ObjectType)
			{
				m_EnableID = SVToolSetEnabledObjectGuid;
			}

		}
		m_EquationValues.SetTaskID(m_EquationID);

		m_Values.Init();
		m_EquationValues.Init();
	}
}
#pragma endregion Private Methods
} //namespace SvOg

