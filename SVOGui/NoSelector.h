//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// NoSelector is the function object to get the empty selection list
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVContainerLibrary/SelectorItem.h"
#pragma endregion Includes

namespace SvOg
{
	class NoSelector
	{
	public:
		SvCl::SelectorItemVectorPtr operator()(const GUID& rGuid, UINT attribute)
		{
			return SvCl::SelectorItemVectorPtr();
		}
		SvCl::SelectorItemVectorPtr operator()(UINT attribute)
		{
			return SvCl::SelectorItemVectorPtr();
		}
	};
} //namespace SvOg
