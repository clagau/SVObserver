//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Interface class for the SVEnumerateValueObject class
//******************************************************************************
#pragma once

#pragma region Includes
#include "NameValueList.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace ObjectInterfaces
	{
		class IEnumerateValueObject
		{
		public:
			virtual ~IEnumerateValueObject() {}
			virtual NameValueList GetEnumList() const = 0;
		};
	}
}