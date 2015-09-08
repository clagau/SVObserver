//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Value Accessor class for use by the GuiController/IObjectManagerCommand.
//******************************************************************************
#pragma once
#pragma region Includes
#include <guiddef.h>
#include "SVObjectLibrary\SVObjectManagerClass.h"
#include "SVObjectLibrary/SVObjectSynchronousCommandTemplate.h"
#include "GuiCommands/TaskObjectGetEmbeddedValues.h"
#include "GuiCommands/TaskObjectSetEmbeddedValues.h"
#include "SVCommandInspectionRunOnce.h"
#include "SVTaskObjectValueInterface.h"
#include "SVValueObjectClass.h" // only while SVValueObject resides in the SVObserver project
#pragma endregion Includes

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
		
		typedef GuiCmd::TaskObjectGetEmbeddedValues<Values, SVValueObjectClass> Command; // only while SVValueObject resides in the SVObserver project
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

	HRESULT SetValues(const SvoGui::BoundValues& rValues) 
	{
		const GUID& ownerID = rValues.GetOwnerID();
		const GUID& inspectionID = rValues.GetInspectionID();
		
		typedef GuiCmd::TaskObjectSetEmbeddedValues<Values, SVValueObjectClass> Command;
		typedef SVSharedPtr<Command> CommandPtr;

		CommandPtr commandPtr(new Command(rValues));
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(inspectionID, commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);

		if (S_OK == hr)
		{
			// Do a run once of the Tool/Inspection ?
			SVCommandInspectionRunOncePtr commandPtr(new SVCommandInspectionRunOnce(inspectionID, ownerID));
			SVObjectSynchronousCommandTemplate<SVCommandInspectionRunOncePtr> cmd(inspectionID, commandPtr);

			hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT); // why 2 minutes ?
		}
		return hr;
	}
};
