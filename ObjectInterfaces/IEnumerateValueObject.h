//******************************************************************************
/// COPYRIGHT (c) 2015 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
//******************************************************************************
/// Interface class for the SVEnumerateValueObject class
//******************************************************************************
#pragma once

#pragma region Includes
#include "Definitions/NameValueVector.h"
#pragma endregion Includes

namespace SvOi
{
	class IEnumerateValueObject
	{
	public:
		virtual ~IEnumerateValueObject() {}
		virtual const SvDef::NameValueVector& GetEnumVector() const = 0;
	};
} //namespace SvOi