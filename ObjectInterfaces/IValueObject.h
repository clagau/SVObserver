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

			/// Validate the value. If value invalid an exception message will be thrown.
			/// \param rValue [in]
			virtual void ValidateValue( const _variant_t& rValue ) const = 0;

		};

		typedef std::pair<IValueObject*, _variant_t> SetValuePair;
		typedef std::vector<SetValuePair> SetValuePairVector;
	}
}

namespace SvOi = Seidenader::ObjectInterfaces;