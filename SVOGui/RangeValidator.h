//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// RangeValidator is the function object used to Validate the range variable(s)
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVStatusLibrary/MessageTextEnum.h"
#pragma endregion Includes

namespace SvOg
{
	class RangeValidator
	{
	public:
		static void IsFieldValid(SvStl::MessageTextEnum fieldName, const std::string& value);
	};
} //namespace SvOg
