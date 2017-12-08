//*****************************************************************************
/// \copyright (c) 2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// Function Object to filter by AttributesAllowed
//******************************************************************************
#pragma once

#pragma region Includes
#include "Definitions/ObjectDefines.h"
#include "ObjectInterfaces/IObjectClass.h"
#pragma endregion Includes

namespace SvCmd
{
	class AttributesAllowedFilter
	{
	public:
		AttributesAllowedFilter(const AttributesAllowedFilter&) = delete;
		AttributesAllowedFilter& operator=(const AttributesAllowedFilter&) = delete;

		AttributesAllowedFilter() {};

		virtual ~AttributesAllowedFilter() {};

		bool operator()( const SvOi::IObjectClass* pObject, unsigned int Attribute, int ArrayIndex ) const
		{
			bool Result(false);

			if( SvDef::SV_NO_ATTRIBUTES == Attribute )
			{
				Result = true;
			}
			else
			{
				if( nullptr != pObject )
				{
					Result = ( Attribute == ( pObject->ObjectAttributesAllowed() & Attribute ) );
				}
			}
			return Result;
		}
	};
} //namespace SvCmd
