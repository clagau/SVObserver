//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Interface class for the SVValueObject class
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <comdef.h>
#pragma endregion Includes

namespace Seidenader
{
	namespace ObjectInterfaces
	{
		class IValueObject
		{
		public:
			virtual ~IValueObject() {}
			virtual HRESULT GetValue(_variant_t& rValue) const = 0;
			virtual HRESULT SetValue(const _variant_t& rValue) = 0;
		};
	}
}

namespace SvOi = Seidenader::ObjectInterfaces;