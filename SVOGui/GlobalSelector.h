//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// GlobalSelector is the function object to get the list of selectable 
/// Global Constants and Environment variables
//******************************************************************************

#pragma once

#pragma region Includes
#include "ObjectInterfaces\ISelectorItemVector.h"
#include "ObjectInterfaces\IRootObject.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace SVOGui
	{
		class GlobalSelector
		{
		public:
			SvOi::ISelectorItemVectorPtr operator()(UINT attribute)
			{
				return SvOi::getRootChildSelectorList(_T(""), attribute);
			}
		};
	}
}

namespace SvOg = Seidenader::SVOGui;