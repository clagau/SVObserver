//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// PPQNameSelector is the function object to get the PPQname for inclusion
/// in the object selector
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVUtilityLibrary\SVString.h"
#include "GuiCommands\GetPPQVariablesNames.h"
#include "SVObjectLibrary\SVObjectSynchronousCommandTemplate.h"
#pragma endregion Includes

class PPQVariablesSelector
{
public:
	SVStringArray operator()(const GUID& rInspectionID)
	{
		SVStringArray list;

		typedef GuiCmd::GetPPQVariablesNames Command;
		typedef SVSharedPtr<Command> CommandPtr;

		CommandPtr commandPtr = new Command(rInspectionID);
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(rInspectionID, commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		if (S_OK == hr)
		{
			list = commandPtr->GetResults();
		}
		return list;
	}
};