//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// PPQVariablesSelector is the function object to get the PPQ Variables for inclusion
/// in the object selector
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVUtilityLibrary\SVString.h"
#include "GuiCommands\GetPPQVariablesNames.h"
#include "SVObjectLibrary\SVObjectSynchronousCommandTemplate.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace SVOGui
	{
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
	}
}

namespace SvOg = Seidenader::SVOGui;