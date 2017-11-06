//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Interface class for image class
//******************************************************************************

#pragma once
#pragma region Includes
#include "Definitions/SVImageEnums.h"
#include "IMatroxImageData.h"
#include "SVUtilityLibrary\SVSharedPtr.h"
#include "SVUtilityLibrary\SVString.h"
#pragma endregion Includes

namespace SvOi
{
	typedef SVSharedPtr< IMatroxImageData > MatroxImageSmartHandlePtr;
	//this class is a interface. It should only have pure virtual public method and no member variables
	class ISVImage
	{
	public:
		virtual ~ISVImage() {}

		//************************************
		/// Return the image type.
		/// \returns SvDef::SVImageTypeEnum
		//************************************
		virtual SvDef::SVImageTypeEnum GetImageType() const = 0;

		//************************************
		/// Return the parent image.
		/// \returns ISVImage*
		//************************************
		virtual ISVImage* GetParentImageInterface() const = 0;

		//************************************
		/// Return the data of the image.
		/// \returns MatroxImageSmartHandlePtr
		//************************************
		virtual MatroxImageSmartHandlePtr getImageData() = 0;

		//************************************
		/// Return the data of the parent image.
		/// \returns MatroxImageSmartHandlePtr
		//************************************
		virtual MatroxImageSmartHandlePtr getParentImageData() = 0;

		//************************************
		/// Return the display name the image.
		/// \returns SVString
		//************************************
		virtual SVString getDisplayedName() const = 0;

		//************************************
		/// Get the Number of bands in the image.
		/// \returns long
		//************************************
		virtual long getBands() const = 0;

		//************************************
		/// Get the pixel depth of the image.
		/// \returns long
		//************************************
		virtual long getPixelDepth() const = 0;

		//************************************
		/// Save an Image to a File.
		/// \returns HRESULT
		//************************************
		virtual HRESULT Save(const SVString& rFilename) = 0; 

		//************************************
		/// Return the output rectangle
		//************************************
		virtual RECT GetOutputRectangle() const = 0;
	};
} //namespace SvOi
