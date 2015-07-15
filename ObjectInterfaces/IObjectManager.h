///******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader, Markt Schwaben
/// All Rights Reserved
///*****************************************************************************
/// IObjectManager is the interface to get/set the objects
///              By now there are only function to call intern static methods.
///******************************************************************************
#pragma once

#pragma region Includes
#include "SVUtilityLibrary\SVString.h"
#include "SVUtilityLibrary\SVGUID.h"
#include "IObjectClass.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace ObjectInterfaces
	{	
		//************************************
		//! Return a object defined by dotted name.
		//! \param rFullName [in] Name of the object.
		//! \returns IObjectClass*
		//************************************
		IObjectClass* getObjectByDottedName( const SVString& rFullName );

		//************************************
		//! Construct an object by the guid and return the pointer.
		//! \param rClassID [in]
		//! \returns IObjectClass*
		//************************************
		IObjectClass* ConstructObject( const SVGUID& rClassID );
	}
}

namespace SvOi = Seidenader::ObjectInterfaces;