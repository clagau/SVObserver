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
#include "GuiCommands\GetPPQSelectorList.h"
#include "SVObjectLibrary\SVObjectSynchronousCommandTemplate.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace SVOGui
	{
		class PPQSelector
		{
		public:
			SvOi::ISelectorItemVectorPtr operator()(const GUID& rInspectionID, UINT Attribute )
			{
				SvOi::ISelectorItemVectorPtr SelectorList;

				typedef GuiCmd::GetPPQSelectorList Command;
				typedef SVSharedPtr<Command> CommandPtr;

				CommandPtr commandPtr = new Command(rInspectionID, Attribute);
				SVObjectSynchronousCommandTemplate<CommandPtr> cmd(rInspectionID, commandPtr);
				HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
				if (S_OK == hr)
				{
					SelectorList = commandPtr->GetResults();
				}
				return SelectorList;
			}
		};
	}
}

namespace SvOg = Seidenader::SVOGui;