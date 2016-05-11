//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Interface class for tool set class
//******************************************************************************

#pragma once
#pragma region Includes
#include "SVUtilityLibrary\SVGUID.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace ObjectInterfaces
	{	
		//this class is a interface. It should only have pure virtual public method and no member variables
		class IToolSet
		{
		public:
			virtual ~IToolSet() {}

			/**********
			 The method determines if it is previous to another in the list.
			 \param rToolID <out>: tool id
			***********/
			virtual bool IsToolPreviousToSelected( const SVGUID& rToolID ) const = 0;
		};
	}
}

namespace SvOi = Seidenader::ObjectInterfaces;