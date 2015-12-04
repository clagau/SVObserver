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
#include "GuiCommands\GetPPQObjectName.h"
#include "SVObjectLibrary\SVObjectSynchronousCommandTemplate.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace SVOGui
	{
		class PPQNameSelector
		{
		public:
			SVString operator()(const GUID& rInspectionID)
			{
				SVString PPQName;
				typedef GuiCmd::GetPPQObjectName Command;
				typedef SVSharedPtr<Command> CommandPtr;

				CommandPtr commandPtr(new Command(rInspectionID));
				SVObjectSynchronousCommandTemplate<CommandPtr> cmd(rInspectionID, commandPtr);
				HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
				if (S_OK == hr)
				{
					PPQName = commandPtr->GetName();
				}
				return PPQName;
			}
		};
	}
}

namespace SvOg = Seidenader::SVOGui;