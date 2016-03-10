//*****************************************************************************
/// \copyright (c) 2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// Function Object to filter by AttributesSet
//******************************************************************************
#pragma once

#pragma region Includes
#include "ObjectInterfaces/ObjectDefines.h"
#include "ObjectInterfaces/IObjectClass.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace GuiCommand
	{
		class AttributesSetFilter
		{
		public:
			bool operator()( const SvOi::IObjectClass* pObject, unsigned int Attribute, int ArrayIndex ) const
			{
				bool Result( false );

				if( SV_NO_ATTRIBUTES == Attribute )
				{
					Result = true;
				}
				else
				{
					if( nullptr != pObject )
					{
						if( pObject->IsArray() )
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

	}
}

namespace GuiCmd = Seidenader::GuiCommand;