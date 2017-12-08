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
#include "ObjectInterfaces/NameValueList.h"
#include "SVObjectLibrary/SVObjectSynchronousCommandTemplate.h"
#include "InspectionCommands/TaskObjectGetEmbeddedValues.h"
#include "InspectionCommands/TaskObjectSetEmbeddedValues.h"
#include "InspectionCommands/ValueObjectGetEnums.h"
#include "InspectionCommands/InspectionRunOnce.h"
#include "InspectionCommands/ResetObject.h"
#include "InspectionCommands/GetObjectName.h"
#pragma endregion Includes

namespace SvOg
{
	template<typename Values>
	class ValuesAccessor
	{
	public:
		typedef Values value_type;

		ValuesAccessor() {}
		virtual ~ValuesAccessor() {}

		HRESULT GetValues(Values& rValues) 
		{
			const GUID& inspectionID = rValues.GetInspectionID();
			const GUID& ownerID = rValues.GetOwnerID();
	
			typedef SvCmd::TaskObjectGetEmbeddedValues<Values> Command;
			typedef SVSharedPtr<Command> CommandPtr;

			CommandPtr commandPtr(new Command(ownerID, rValues));
			SVObjectSynchronousCommandTemplate<CommandPtr> cmd(inspectionID, commandPtr);
			HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
			if (S_OK == hr)
			{
				rValues = commandPtr->GetItems();
			}
			return hr;
		}

		HRESULT SetValues(const SvOg::BoundValues& rValues, bool bReset) 
		{
			m_setMessageFailList.clear();
			const GUID& ownerID = rValues.GetOwnerID();
			const GUID& inspectionID = rValues.GetInspectionID();
	
			typedef SvCmd::TaskObjectSetEmbeddedValues<Values> Command;
			typedef SVSharedPtr<Command> CommandPtr;

			CommandPtr commandPtr(new Command(rValues));
			SVObjectSynchronousCommandTemplate<CommandPtr> cmd(inspectionID, commandPtr);
			HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
			m_setMessageFailList = commandPtr->getErrorMessages();

			if (S_OK == hr)
			{
				if (bReset)
				{
					// Do a reset of the Tool
					typedef SVSharedPtr<SvCmd::ResetObject> ResetObjectCommandPtr;
					ResetObjectCommandPtr commandPtr(new SvCmd::ResetObject(ownerID));
					SVObjectSynchronousCommandTemplate<ResetObjectCommandPtr> cmd(inspectionID, commandPtr);

					hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
					if (S_OK != hr)
					{
						m_setMessageFailList = commandPtr->getErrorMessages();
					}
				}
				if (S_OK == hr)
				{
					// Do a run once of the Tool/Inspection ?
					SvCmd::InspectionRunOncePtr commandPtr(new SvCmd::InspectionRunOnce(inspectionID, ownerID));
					SVObjectSynchronousCommandTemplate<SvCmd::InspectionRunOncePtr> cmd(inspectionID, commandPtr);

					hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
				}
			}
			return hr;
		}

		SvOi::NameValueList GetEnums(const GUID& rInspectionID, const GUID& rObjectID) const
		{
			typedef SvCmd::ValueObjectGetEnums Command;
			typedef SVSharedPtr<Command> CommandPtr;

			CommandPtr commandPtr(new Command(rObjectID));
			SVObjectSynchronousCommandTemplate<CommandPtr> cmd(rInspectionID, commandPtr);
			HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);

			if (S_OK == hr)
			{
				return commandPtr->GetEnumList();
			}
			return SvOi::NameValueList();
		}

		std::string GetObjectName(const GUID& rInspectionID, const GUID& rObjectID) const
		{
			typedef SvCmd::GetObjectName Command;
			typedef SVSharedPtr<Command> CommandPtr;

			CommandPtr commandPtr(new Command(rObjectID));
			SVObjectSynchronousCommandTemplate<CommandPtr> cmd(rInspectionID, commandPtr);
			HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);

			if (S_OK == hr)
			{
				return commandPtr->GetName();
			}
			return std::string();
		}

		SvStl::MessageContainerVector getSetFailedMessageList() { return m_setMessageFailList; };

	protected:
		SvStl::MessageContainerVector m_setMessageFailList;
	};
} //namespace SvOg
