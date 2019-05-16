//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Interface class for tool set class
//******************************************************************************

#pragma once

#pragma region Includes
#include "IObjectClass.h"
#include "ISVImage.h"
#include "IValueObject.h"
#pragma endregion Includes

#pragma region Declarations
//! Declaration is in #include "SVUtilityLibrary\SVGUID.h"
class SVGUID;
#pragma endregion Declarations


namespace SvOi
{
	//this class is a interface. It should only have pure virtual public method and no member variables
	class IToolSet
	{
	public:
		virtual ~IToolSet() {}

		/**********
			The method determines if it is previous to another in the list.
			\param rToolID <in>: tool id
		***********/
		virtual bool IsToolPreviousToSelected( const SVGUID& rToolID ) const = 0;

		/**********
			The method retrieves the band 0 of a color tool else nullptr
			\return Pointer to the Band0 image object
		***********/
		virtual IObjectClass* getBand0Image() const = 0;

		virtual bool WasEnabled() const = 0; 

		virtual HRESULT getResetCounts(bool& rResetCounts) const = 0;

		virtual ISVImage* getCurrentImageInterface() = 0;

		virtual IValueObject* GetDrawFlagObject() = 0;
	};
} //namespace SvOi
