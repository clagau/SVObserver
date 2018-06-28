//*****************************************************************************
/// \copyright (c) 2018,2018 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// \file BuildSelectableItems.h
//*****************************************************************************
/// Command to build the object selectable items to display
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <memory>
#include "SelectorOptions.h"
#include "GetSelectorList.h"
#include "GetPPQSelectorList.h"
#include "SVContainerLibrary/SelectorItem.h"
#include "ObjectInterfaces/IRootObject.h"
#include "SVCommandLibrary/SVObjectSynchronousCommandTemplate.h"
#pragma endregion Includes

namespace SvCmd
{
	//Note the default filter uses the object attribute if in rOptions if it is 0 then all objects are listed
	void inline BuildSelectableItems(const SelectorOptions& rOptions, SvCl::SelectorItemInserter inserter, SelectorFilterTypeEnum FilterType = SelectorFilterTypeEnum::AttributesAllowedFilterType)
	{
		for(const auto& rSelectorType : rOptions.m_ItemTypes)
		{
			switch(rSelectorType)
			{
				case ObjectSelectorType::toolsetItems:
				{
					typedef std::shared_ptr<GetSelectorList> CommandPtr;
				
					const GUID& rGuid = (GUID_NULL != rOptions.m_rInstanceID ) ? rOptions.m_rInstanceID : rOptions.m_rInspectionID;
					CommandPtr commandPtr = std::make_shared<GetSelectorList>(inserter, rGuid, FilterType, rOptions.m_attributesFilter, rOptions.m_wholeArray);
					SVObjectSynchronousCommandTemplate<CommandPtr> cmd(rOptions.m_rInspectionID, commandPtr);
					cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
					break;
				}
				case ObjectSelectorType::globalConstantItems:
				{
					SvOi::getRootChildSelectorList(inserter, _T(""), rOptions.m_attributesFilter);
					break;
				}
				case ObjectSelectorType::ppqItems:
				{
					typedef std::shared_ptr<GetPPQSelectorList> CommandPtr;

					CommandPtr commandPtr = std::make_shared<GetPPQSelectorList>(inserter, rOptions.m_rInspectionID, rOptions.m_attributesFilter);
					SVObjectSynchronousCommandTemplate<CommandPtr> cmd(rOptions.m_rInspectionID, commandPtr);
					cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
					break;
				}
			}
		}
	};
} //namespace SvCmd