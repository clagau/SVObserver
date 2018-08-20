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

	HRESULT SetValues(const SvOg::BoundValues& rValues, PostAction doAction)
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
			bool bReset {false};
			bool bRunOnce {false};

			switch (doAction)
			{
				case doReset:
				{
					bReset = true;
					break;
				}
				case doRunOnce:
				{
					bRunOnce = true;
					break;
				}
				case doResetRunOnce:
				{
					bReset = true;
					bRunOnce = true;
					break;
				}
				default:
				{
					break;
				}
			}
			if (bReset)
			{
				// Do a reset of the Tool
				hr = ResetObject(rInspectionID, rTaskID);
			}
			if (S_OK == hr && bRunOnce)
			{
				hr = RunOnce(rInspectionID, rTaskID);
			}
		}
		return hr;
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

	HRESULT ResetObject(const GUID& rInspectionID, const GUID& rObjectID)
	{
		m_MessageFailList.clear();
		SvPb::InspectionCmdMsgs requestMessage;
		SvPb::InspectionCmdMsgs responseMessage;

		SvPb::ResetObjectRequest* pRequest = requestMessage.mutable_resetobjectrequest();
		SvPb::SetGuidInProtoBytes(pRequest->mutable_objectid(), rObjectID);
		HRESULT hr = SvCmd::InspectionCommandsSynchronous(rInspectionID, &requestMessage, &responseMessage);
		if (hr == S_OK && responseMessage.has_resetobjectresponse())
		{
			m_MessageFailList = SvCmd::setMessageContainerFromMessagePB(responseMessage.resetobjectresponse().messages());
		}
		return hr;
	}

	HRESULT RunOnce(const GUID& rInspectionID, const GUID& rObjectID)
	{
		// Do a run once of the Tool/Inspection ?
		return SvCmd::RunOnceSynchronous(rInspectionID, rObjectID);
	}

	const SvStl::MessageContainerVector& getFailedMessageList() { return m_MessageFailList; };
#pragma endregion Public Methods

#pragma region Member Variables
private:
	SvStl::MessageContainerVector m_MessageFailList;
#pragma endregion Member Variables
};
} //namespace SvOg
