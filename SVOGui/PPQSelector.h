//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// PPQVariablesSelector is the function object to get the PPQ Variables for inclusion
/// in the object selector
//******************************************************************************

#pragma once

#pragma region Includes
#include "InspectionCommands\GetPPQSelectorList.h"
#include "SVObjectLibrary\SVObjectSynchronousCommandTemplate.h"
#pragma endregion Includes

namespace SvOg
{
	class PPQSelector
	{
	public:
		SvCl::SelectorItemVectorPtr operator()(const GUID& rInspectionID, UINT Attribute )
		{
			SvCl::SelectorItemVectorPtr SelectorList;

			typedef SvCmd::GetPPQSelectorList Command;
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
} //namespace SvOg
