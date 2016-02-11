//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// NoSelector is the function object to get the empty selection list
//******************************************************************************

#pragma once

#pragma region Includes
#include "ObjectInterfaces\ISelectorItemVector.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace SVOGui
	{
		class NoSelector
		{
		public:
			SvOi::ISelectorItemVectorPtr operator()(const GUID& rGuid, UINT attribute)
			{
				return SvOi::ISelectorItemVectorPtr();
			}
			SvOi::ISelectorItemVectorPtr operator()(UINT attribute)
			{
				return SvOi::ISelectorItemVectorPtr();
			}
		};
	}
}

namespace SvOg = Seidenader::SVOGui;