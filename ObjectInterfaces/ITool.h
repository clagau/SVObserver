//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Interface class for SVTool class
//******************************************************************************

#pragma once
#pragma region Includes
#include "SVUtilityLibrary\NameGuidList.h"
#pragma endregion Includes

#pragma region Declarations
//! Declaration is in #include "SVUtilityLibrary\SVGUID.h"
class SVGUID;
#pragma endregion Declarations

namespace SvOi
{
	//this class is a interface. It should only have pure virtual public method and no member variables
	class ITool
	{
	public:
		virtual ~ITool() {}

		virtual bool areAuxExtentsAvailable() const = 0;
		virtual SvUl::NameGuidList getAvailableAuxSourceImages() const = 0;
		virtual SvUl::NameGuidPair getAuxSourceImage() const = 0;
		virtual HRESULT setAuxSourceImage(const SVGUID& rObjectID) = 0;
		virtual void SetToolImage( const SVGUID& rObjectID ) = 0;
		virtual HRESULT UpdateImageWithExtent() = 0;
		virtual bool SetFirstInputImageName( LPCTSTR FirstName) = 0;
		virtual long getToolPosition() const = 0;
	};
} //namespace SvOi
