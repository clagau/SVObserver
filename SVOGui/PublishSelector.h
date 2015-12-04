//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// PublishSelector is the function used to get the list of selectable
/// items that are publishable for the ObjectSelector
//******************************************************************************
#pragma once

#pragma region Includes
#include <Guiddef.h>
#include "ObjectSelectorLibrary\ObjectTreeGenerator.h"
#include "NoSelector.h"
#include "ToolSetItemSelector.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace SVOGui
	{
		static void PublishSelector(const GUID& rInspectionID, const GUID& rInstanceID)
		{
			SvOsl::ObjectTreeGenerator::Instance().BuildSelectableItems<NoSelector, NoSelector, NoSelector, ToolSetItemSelector<>>(rInspectionID, rInstanceID);
		}
	}
}

namespace SvOg = Seidenader::SVOGui;
