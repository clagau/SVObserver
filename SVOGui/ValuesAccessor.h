//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Value Accessor class for use by the GuiController/IObjectManagerCommand.
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <guiddef.h>
#include "BoundValue.h"
#include "ObjectInterfaces/NameValueList.h"
#include "SVObjectLibrary/SVObjectSynchronousCommandTemplate.h"
#include "GuiCommands/TaskObjectGetEmbeddedValues.h"
#include "GuiCommands/TaskObjectSetEmbeddedValues.h"
#include "GuiCommands/ValueObjectGetEnums.h"
#include "GuiCommands/InspectionRunOnce.h"
#include "GuiCommands/ResetObject.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace SVOGui
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
		
				typedef GuiCmd::TaskObjectGetEmbeddedValues<Values> Command;
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
		
				typedef GuiCmd::TaskObjectSetEmbeddedValues<Values> Command;
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
						typedef SVSharedPtr<GuiCmd::ResetObject> ResetObjectCommandPtr;
						ResetObjectCommandPtr commandPtr(new GuiCmd::ResetObject(ownerID, true));
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
						GuiCmd::InspectionRunOncePtr commandPtr(new GuiCmd::InspectionRunOnce(inspectionID, ownerID));
						SVObjectSynchronousCommandTemplate<GuiCmd::InspectionRunOncePtr> cmd(inspectionID, commandPtr);

						hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
					}
				}
				return hr;
			}

			SvOi::NameValueList GetEnums(const GUID& rInspectionID, const GUID& rObjectID) const
			{
				typedef GuiCmd::ValueObjectGetEnums Command;
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

			SvStl::MessageContainerVector getSetFailedMessageList() { return m_setMessageFailList; };

		protected:
			SvStl::MessageContainerVector m_setMessageFailList;
		};
	}
}

namespace SvOg = Seidenader::SVOGui;