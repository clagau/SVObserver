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
#include "ISVImage.h"
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

		//************************************
		//! Return a object defined by it's Object ID.
		//! \param rObjectId [in] Instance GUID of the object.
		//! \returns IObjectClass*
		//************************************
		IObjectClass* getObject( const SVGUID& rObjectID );

		//************************************
		//! Traverses the Object DataStore, starting at a certain Object.
		//! \param ObjectVisitor [in] The visitor.
		//! \param SVGUID [in] starting Object.
		//! \returns HRESULT
		//************************************
		template<typename ObjectVisitor>
		HRESULT visitElements(ObjectVisitor& rVisitor, const SVGUID& rStartingObjectID = GUID_NULL);

		IObjectClass* FindObject(const SVGUID& rParentID, const SVObjectTypeInfoStruct& rInfo);
		ISVImage* FindImageObject(const SVGUID& rParentID, const SVObjectTypeInfoStruct& rInfo);
	}
}

namespace SvOi = Seidenader::ObjectInterfaces;