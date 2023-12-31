//******************************************************************************
/// COPYRIGHT (c) 2015 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
//******************************************************************************
/// Interface class for image class
//******************************************************************************

#pragma once
#pragma region Includes
#include "SVImageBufferHandleInterface.h"
#include "SVProtoBuf/SVO-Enum.h"
#include "SVProtoBuf/InspectionCommands.h"
#pragma endregion Includes

namespace SvPb
{
class OverlayDesc;
}

namespace SvOi
{
	//this class is a interface. It should only have pure virtual public method and no member variables
	class ISVImage
	{
	public:
		virtual ~ISVImage() {}

		//************************************
		/// Return the image type.
		/// \returns SvDef::SVImageTypeEnum
		//************************************
		virtual SvPb::SVImageTypeEnum GetImageType() const = 0;

		//************************************
		/// Return the parent image.
		/// \returns ISVImage*
		//************************************
		virtual ISVImage* GetParentImageInterface() const = 0;

		//************************************
		/// Return the data of the image.
		/// \returns SVImageBufferHandlePtr
		//************************************
		virtual SVImageBufferHandlePtr getLastImage(bool newIfNotAvailable = false) const = 0;

		//************************************
		/// Return the display name the image.
		/// \returns std::string
		//************************************
		virtual std::string getDisplayedName() const = 0;

		//************************************
		/// Get the pixel depth of the image.
		/// \returns long
		//************************************
		virtual long getPixelDepth() const = 0;

		//************************************
		/// Save an Image to a File.
		/// \returns HRESULT
		//************************************
		virtual HRESULT Save(const std::string& rFilename) = 0; 

		//************************************
		/// Return the output rectangle
		//************************************
		virtual RECT GetOutputRectangle() const = 0;

		/// Return the Overlay struct depend of this image.
		/// \returns SvPb::OverlayDesc
		virtual SvPb::OverlayDesc getOverlayStruct() const = 0;

		virtual void getExtentProperties(::google::protobuf::RepeatedPtrField< ::SvPb::ExtentParameter >& rExtentProperties) const  = 0;
	};
} //namespace SvOi
