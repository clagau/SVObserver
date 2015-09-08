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
		typedef SVSharedPtr< IMatroxImageData > MatroxImageSmartHandlePointer;
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
			/// \returns MatroxImageSmartHandlePointer
			//************************************
			virtual MatroxImageSmartHandlePointer getImageData() = 0;

			//************************************
			/// Return the data of the parent image.
			/// \returns MatroxImageSmartHandlePointer
			//************************************
			virtual MatroxImageSmartHandlePointer getParentImageData() = 0;
		};
	}
}

namespace SvOi = Seidenader::ObjectInterfaces;