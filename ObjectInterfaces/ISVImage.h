//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Interface class for image class
//******************************************************************************

#pragma once
#pragma region Includes
#include "SVImageEnums.h"
#include "IObjectAppClass.h"
#include "IMatroxImageData.h"
#include "SVUtilityLibrary\SVSharedPtr.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace ObjectInterfaces
	{	
		typedef SVSharedPtr< IMatroxImageData > MatroxImageSmartHandlePtr;
		//this class is a interface. It should only have pure virtual public method and new member parameter
		class ISVImage : virtual public IObjectAppClass
		{
		public:
			virtual ~ISVImage() {}

			//************************************
			/// Return the image type.
			/// \returns SVImageTypeEnum
			//************************************
			virtual SVImageTypeEnum GetImageType() const = 0;

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
			/// Get the Owner Object for the Image.
			/// \returns SvOi::IObjectClass*
			//************************************
			virtual SvOi::IObjectClass* getOwner() const = 0;

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
		};
	}
}

namespace SvOi = Seidenader::ObjectInterfaces;