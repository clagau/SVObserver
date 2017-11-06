//*****************************************************************************
/// \copyright (c) 2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// Function Object to filter by AttributesSet
//******************************************************************************
#pragma once

#pragma region Includes
#include "Definitions/ObjectDefines.h"
#include "ObjectInterfaces/IObjectClass.h"
#include "ObjectInterfaces/IValueObject.h"
#pragma endregion Includes

namespace SvCmd
{
	class AttributesSetFilter
	{
	public:
		bool operator()( const SvOi::IObjectClass* pObject, unsigned int Attribute, int ArrayIndex ) const
		{
			bool Result( false );

			if( SvDef::SV_NO_ATTRIBUTES == Attribute )
			{
				Result = true;
			}
			else
			{
				if( nullptr != pObject )
				{
					const SvOi::IValueObject* pValueObject = dynamic_cast<const SvOi::IValueObject*> (pObject);
					if( nullptr != pValueObject && pValueObject->isArray() )
					{
						Result = ( Attribute == ( pObject->ObjectAttributesSet( ArrayIndex ) & Attribute) );
					}
					else
					{
						Result = ( Attribute == ( pObject->ObjectAttributesSet() & Attribute) );
					}
				}
			}
			return Result;
		}
	};
} //namespace SvCmd

