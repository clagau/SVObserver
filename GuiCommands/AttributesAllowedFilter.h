//*****************************************************************************
/// \copyright (c) 2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// Function Object to filter by AttributesAllowed
//******************************************************************************
#pragma once

#pragma region Includes
#include "ObjectInterfaces/ObjectDefines.h"
#include "ObjectInterfaces/IObjectClass.h"
#pragma endregion Includes

namespace SvCmd
{
	class AttributesAllowedFilter
	{
	public:
		bool operator()( const SvOi::IObjectClass* pObject, unsigned int Attribute, int ArrayIndex ) const
		{
			bool Result(false);

			if( SvOi::SV_NO_ATTRIBUTES == Attribute )
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
