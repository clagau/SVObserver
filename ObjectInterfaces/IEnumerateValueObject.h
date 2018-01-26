//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Interface class for the SVEnumerateValueObject class
//******************************************************************************
#pragma once

#pragma region Includes
#include "NameValueVector.h"
#pragma endregion Includes

namespace SvOi
{
	class IEnumerateValueObject
	{
	public:
		virtual ~IEnumerateValueObject() {}
		virtual const NameValueVector& GetEnumVector() const = 0;
	};
} //namespace SvOi