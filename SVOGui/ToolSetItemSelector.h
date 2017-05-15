//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// ToolSetItemSelector is the function object to get the ToolSet Items for inclusion
/// in the object selector
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <boost\bind.hpp>
#include "ObjectInterfaces\ISelectorItemVector.h"
#include "SVUtilityLibrary\SVSharedPtr.h"
#include "SVUtilityLibrary\SVString.h"
#include "GuiCommands\GetSelectorList.h"
#include "ObjectSelectorLibrary\SelectorOptions.h"
#include "SVObjectLibrary\SVObjectSynchronousCommandTemplate.h"
#pragma endregion Includes

namespace SvOg
{
	//Note the default filter uses the object attribute if in rOptions if it is 0 then all objects are listed
	template<SvCmd::SelectorFilterTypeEnum FilterType=SvCmd::AttributesAllowedFilterType>
	class ToolSetItemSelector
	{
	public:
		SvOi::ISelectorItemVectorPtr operator()(const SvOsl::SelectorOptions& rOptions)
		{
			SvOi::ISelectorItemVectorPtr SelectorList;

			typedef SvCmd::GetSelectorList<SvOi::ISelectorItemVectorPtr> Command;
			typedef SVSharedPtr<Command> CommandPtr;
		
			const SVGUID& rGuid = (GUID_NULL != rOptions.getInstanceID() ) ? rOptions.getInstanceID() : rOptions.getInspectionID();
			CommandPtr commandPtr(new Command(rGuid, FilterType, rOptions.getAttributesFilter(), rOptions.getWholeArray()));
			SVObjectSynchronousCommandTemplate<CommandPtr> cmd(rOptions.getInspectionID(), commandPtr);
			HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
			if (S_OK == hr)
			{
				SelectorList = commandPtr->GetResults();
			}
			return SelectorList;
		}
	};
} //namespace SvOg