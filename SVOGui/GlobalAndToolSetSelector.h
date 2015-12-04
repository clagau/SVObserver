//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// GlobalAndToolSetSelector  is the function used to get the list of selectable
/// items fronm the Globals or the Tool Set for the ObjectSelector
//******************************************************************************
#pragma once

#pragma region Includes
#include <Guiddef.h>
#include "ObjectSelectorLibrary\ObjectTreeGenerator.h"
#include "GlobalSelector.h"
#include "NoSelector.h"
#include "ToolSetItemSelector.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace SVOGui
	{
		/// Note: this method is only used for the RingBuffer tool adjustment Dialog at the moment.
		/// If the requirements change as to what is selectable, perhaps it will be identical to other tools as well.
		static void GlobalAndToolSetSelector(const GUID& rInspectionID, const GUID& rInstanceID)
		{
			SvOsl::ObjectTreeGenerator::Instance().BuildSelectableItems<GlobalSelector, NoSelector, NoSelector, ToolSetItemSelector<>>(rInspectionID, rInstanceID);
		}
	}
}

namespace SvOg = Seidenader::SVOGui;