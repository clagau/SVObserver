//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Value Accessor class for use by the DataController/IObjectManagerCommand.
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <guiddef.h>
#include "BoundValue.h"
#include "ObjectInterfaces/NameValueVector.h"
#include "SVCommandLibrary/SVObjectSynchronousCommandTemplate.h"
#include "InspectionCommands/TaskObjectGetEmbeddedValues.h"
#include "InspectionCommands/TaskObjectSetEmbeddedValues.h"
#include "InspectionCommands/ValueObjectGetEnums.h"
#include "InspectionCommands/GetObjectName.h"
#include "InspectionCommands/CommandFunctionHelper.h"
#include "SVMessage/SVMessage.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

namespace SvOg
{
template<typename Values>
class ValuesAccessor
{
public:
	typedef Values value_type;

#pragma region Constructor
public:
	ValuesAccessor() {}
	virtual ~ValuesAccessor() {}

	ValuesAccessor& operator=(const ValuesAccessor&) = delete;
#pragma endregion Constructor

#pragma region Public Methods
public:
	HRESULT GetValues(Values& rValues)
	{
		const GUID& rInspectionID = rValues.GetInspectionID();
		const GUID& rTaskID = rValues.GetTaskID();
		rValues.clear();

		typedef SvCmd::TaskObjectGetEmbeddedValues<Values> Command;
		typedef std::shared_ptr<Command> CommandPtr;

		CommandPtr commandPtr(new Command(rInspectionID, rTaskID));
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(rInspectionID, commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		if (S_OK == hr)
		{
			rValues = commandPtr->GetItems();
		}
		return hr;
	}

	void SetValues(const SvOg::BoundValues& rValues, PostAction doAction)
	{
		m_MessageFailList.clear();
		const GUID& rTaskID = rValues.GetTaskID();
		const GUID& rInspectionID = rValues.GetInspectionID();

		typedef SvCmd::TaskObjectSetEmbeddedValues<Values> Command;
		typedef std::shared_ptr<Command> CommandPtr;

		CommandPtr commandPtr(new Command(rTaskID, rValues));
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(rInspectionID, commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		m_MessageFailList = commandPtr->getErrorMessages();

		if (S_OK == hr)
		{
			bool bReset {doAction & PostAction::doReset};
			bool bRunOnce {doAction & PostAction::doRunOnce};

			if (bReset)
			{
				// Do a reset of the Tool
				ResetObject(rInspectionID, rTaskID);
			}
			if (S_OK == hr && bRunOnce)
			{
				hr = RunOnce(rInspectionID);
				if(S_OK != hr)
				{
					SvStl::MessageContainer message(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_Error_CannotRunOnce, SvStl::SourceFileParams(StdMessageParams), 0, rTaskID);
					m_MessageFailList.push_back(message);
				}
			}
		}
		if (S_OK != hr && 0 == m_MessageFailList.size())
		{
			SvDef::StringVector msgList;
			msgList.push_back(SvUl::Format(_T("%d"),hr));
			SvStl::MessageContainer message(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_UnknownCommitError, msgList, SvStl::SourceFileParams(StdMessageParams), 0, rTaskID);
			m_MessageFailList.push_back(message);
		}
		if (0 < m_MessageFailList.size())
		{
			throw m_MessageFailList;
		}
	}

	SvOi::NameValueVector GetEnums(const GUID& rInspectionID, const GUID& rObjectID) const
	{
		typedef SvCmd::ValueObjectGetEnums Command;
		typedef std::shared_ptr<Command> CommandPtr;

		CommandPtr commandPtr(new Command(rObjectID));
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(rInspectionID, commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);

		if (S_OK == hr)
		{
			return commandPtr->GetEnumList();
		}
		return SvOi::NameValueVector();
	}

	std::string GetObjectName(const GUID& rInspectionID, const GUID& rObjectID) const
	{
		typedef SvCmd::GetObjectName Command;
		typedef std::shared_ptr<Command> CommandPtr;

		CommandPtr commandPtr(new Command(rObjectID));
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(rInspectionID, commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);

		if (S_OK == hr)
		{
			return commandPtr->GetName();
		}
		return std::string();
	}

	void ResetObject(const GUID& rInspectionID, const GUID& rObjectID)
	{
		m_MessageFailList.clear();
		SvPb::InspectionCmdMsgs requestMessage;
		SvPb::InspectionCmdMsgs responseMessage;

		SvPb::ResetObjectRequest* pRequest = requestMessage.mutable_resetobjectrequest();
		SvPb::SetGuidInProtoBytes(pRequest->mutable_objectid(), rObjectID);
		HRESULT hr = SvCmd::InspectionCommandsSynchronous(rInspectionID, &requestMessage, &responseMessage);
		if (responseMessage.has_resetobjectresponse())
		{
			m_MessageFailList = SvCmd::setMessageContainerFromMessagePB(responseMessage.resetobjectresponse().messages());
		}
		if (S_OK != hr && 0 < m_MessageFailList.size())
		{
			SvStl::MessageContainer message(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorInReset, SvStl::SourceFileParams(StdMessageParams), 0, rObjectID);
			m_MessageFailList.push_back(message);
		}
		if (0 < m_MessageFailList.size())
		{
			throw m_MessageFailList;
		}
	}

	HRESULT RunOnce(const GUID& rInspectionID)
	{
		// Do a run once of the whole Inspection
		return SvCmd::RunOnceSynchronous(rInspectionID);
	}

	const SvStl::MessageContainerVector& getFailedMessageList() { return m_MessageFailList; };
#pragma endregion Public Methods

#pragma region Member Variables
private:
	SvStl::MessageContainerVector m_MessageFailList;
#pragma endregion Member Variables
};
} //namespace SvOg
