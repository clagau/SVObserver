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
//Moved to precompiled header: #include <string>
#include "FormulaController.h"
#include "InspectionCommands/GetObjectName.h"
#include "InspectionCommands/GetPPQObjectName.h"
#include "InspectionCommands/GetPPQSelectorList.h"
#include "InspectionCommands/GetInstanceIDByTypeInfo.h"
#include "InspectionCommands/TaskObjectGetEmbeddedValues.h"
#include "InspectionCommands/TaskObjectSetEmbeddedValues.h"
#include "InspectionCommands/SetDefaultInputs.h"
#include "SVObjectLibrary\SVClsIds.h"
#include "ObjectSelectorLibrary\ObjectTreeGenerator.h"
#include "SVObjectLibrary\SVObjectSynchronousCommandTemplate.h"
#include "BoundValue.h"
#include "GlobalSelector.h"
#include "PPQSelector.h"
#include "ToolSetItemSelector.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVMessage/SVMessage.h"
#include "Definitions/StringTypeDef.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "InspectionCommands/SetObjectName.h"
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
	FormulaController::FormulaController(const GUID& rInspectionID, const GUID& rTaskObjectID, const SvDef::SVObjectTypeInfoStruct& rInfo, bool bEnabledReadOnly)
	: m_InspectionID(rInspectionID)
	, m_TaskObjectID(rTaskObjectID)
	, m_info(rInfo)
	, m_EquationID(GUID_NULL)
	, m_Values(SvOg::BoundValues(rInspectionID, rTaskObjectID, bEnabledReadOnly))
	{
		init();
	}

	FormulaController::FormulaController(const GUID& rInspectionID, const GUID& rTaskObjectID, const GUID& rEquationID, bool bEnabledReadOnly)
		: m_InspectionID(rInspectionID)
		, m_TaskObjectID(rTaskObjectID)
		, m_info(SvDef::SVObjectTypeInfoStruct())
		, m_EquationID(rEquationID)
		, m_Values(SvOg::BoundValues(rInspectionID, rTaskObjectID, bEnabledReadOnly))
	{
		init();
	}
	#pragma endregion Constructor

	#pragma region Destructor
	FormulaController::~FormulaController()
	{
	}
	#pragma endregion Destructor

	#pragma region Public Methods
	#pragma region virtual Methods IFormulaController
	std::string FormulaController::GetEquationText() const
	{
		std::string equationText;
		SvPB::GetEquationRequest requestMessage;
		SvPB::GetEquationResponse responseMessage;
		SvPB::SetGuidInProtoBytes(requestMessage.mutable_objectid(), m_EquationID);
		HRESULT hr = SvCmd::InspectionCommandsSynchronous(m_InspectionID, &requestMessage, &responseMessage);
		if (S_OK == hr)
		{
			equationText = responseMessage.equationtext();
		}
		else
		{
			SvDef::StringVector msgList;
			msgList.push_back(SvUl::Format(_T("%d"), hr));
			SvStl::MessageMgrStd e( SvStl::LogOnly );
			e.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_UnknownCommandError, SvStl::SourceFileParams(StdMessageParams) );
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
		SvOsl::SelectorOptions BuildOptions( m_InspectionID, SvDef::SV_SELECTABLE_FOR_EQUATION, m_InspectionID, true );
		SvOsl::ObjectTreeGenerator::Instance().BuildSelectableItems<SvOg::GlobalSelector, SvOg::PPQSelector, SvOg::ToolSetItemSelector<>>( BuildOptions );
	}

	HRESULT FormulaController::IsOwnerAndEquationEnabled(bool& ownerEnabled, bool& equationEnabled) const
	{
		//Only when the controller is of type SvDef::SVConditionalObjectType
		if (SvDef::SVConditionalObjectType == m_info.SubType)
		{
			ownerEnabled = m_Values.Get<bool>(SVToolEnabledObjectGuid);
			equationEnabled = m_Values.Get<bool>(SVEquationEnabledObjectGuid);
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
		if (SvDef::SVConditionalObjectType == m_info.SubType)
		{
			m_Values.Set<bool>(SVToolEnabledObjectGuid, ownerEnabled);
			m_Values.Set<bool>(SVEquationEnabledObjectGuid, equationEnabled);
			m_Values.Commit();
		}
		return hr;
	}

	int FormulaController::ValidateEquation( const std::string& equationString, double& result, bool bSetValue, SvStl::MessageContainerVector& rErrorMessages ) const
	{
		int retValue = validateSuccessful;
		SvPB::ValidateAndSetEquationRequest requestEquationMessage;
		SvPB::ValidateAndSetEquationResponse responseEquationMessage;
		
		SvPB::SetGuidInProtoBytes(requestEquationMessage.mutable_objectid(), m_EquationID);
	
		requestEquationMessage.set_equationtext(equationString);
		requestEquationMessage.set_bsetvalue(bSetValue);
		HRESULT hr = SvCmd::InspectionCommandsSynchronous(m_InspectionID, &requestEquationMessage, &responseEquationMessage);
		if (S_OK == hr)
		{
			retValue = responseEquationMessage.validatestatus();
			result = responseEquationMessage.result();
			rErrorMessages = SvCmd::setMessageContainerFromMessagePB(responseEquationMessage.messages());;

			if (validateSuccessful == retValue && bSetValue)
			{
				SvPB::ResetObjectRequest requestResetMessage;
				SvPB::SetGuidInProtoBytes(requestResetMessage.mutable_objectid(), m_TaskObjectID);
				hr = SvCmd::InspectionCommandsSynchronous(m_InspectionID, &requestResetMessage);
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
			SvStl::MessageMgrStd e( SvStl::LogOnly );
			e.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_UnknownCommandError, SvStl::SourceFileParams(StdMessageParams) );
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

	#pragma region Protected Methods
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

	#pragma endregion Protected Methods

	void FormulaController::init()
	{
		//Only when the controller is of type SvDef::SVConditionalObjectType
		if (SvDef::SVConditionalObjectType == m_info.SubType)
		{
			m_Values.Init();
		}

		if (GUID_NULL == m_EquationID)
		{
			typedef SvCmd::GetInstanceIDByTypeInfo Command;
			typedef std::shared_ptr<Command> CommandPtr;
			// check for Math Container...
			if (SvDef::SVMathContainerObjectType == m_info.ObjectType)
			{
				// Get the Math Container
				CommandPtr commandPtr(new Command(m_TaskObjectID, m_info));
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
				}
			}
			else
			{
				// Get the Equation
				CommandPtr commandPtr(new Command(m_TaskObjectID, m_info));
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
					SvStl::MessageMgrStd e( SvStl::LogOnly );
					e.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_UnknownCommandError, SvStl::SourceFileParams(StdMessageParams) );
					ASSERT(false);
				}
			}
		}
	}
} //namespace SvOg

