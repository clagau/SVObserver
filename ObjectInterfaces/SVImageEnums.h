//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Enum list for images
//******************************************************************************

#pragma once

//TODO (MZA:2015/01/09): Add namespace to fit to the code rules. 
//								Not edit yet to avoid to change to much code in this prototype.
//namespace Seidenader
//{
//	namespace ObjectInterfaces
//	{

		enum SVImageTypeEnum // Used by SVImageClass
		{
//--------- SVImageTypeUnknown - only for initialization at construction. 
//---------   Should be set to a different value by first 
//---------	  SVImageClass::Initialize() call.
//---------	  This image type does not create an image buffer pool?
			SVImageTypeUnknown     = 0,

//--------- SVImageTypeFixed
//---------   Physical (Parent) buffer.
//---------   Does not set Tool Image within UpdateFromToolInformation().
			SVImageTypeFixed       = 1,

//--------- SVImageTypeIndependent
//---------   Physical (Parent) buffer.
//---------   Does not set Tool Image within UpdateFromToolInformation().
			SVImageTypeIndependent = 2,

//--------- SVImageTypeDependent - Only used to define the 3 banded images of 
//---------   a Color Tool.
//---------
//---------   Dependent and Virtual image types maintain a child->parent 
//---------   relationship at the SVImageClass level (nullptr != m_ParentImagePtr)
//---------   but are still allocated as physical buffers.
//---------
//---------	  This image type does not create an image buffer pool as part of
//---------	  the SVImageObjectClass.  Instead its buffer pool is maintained 
//---------   as a child image of the SVImageClass parent!
//---------
//---------   Virtual image type can be a child of a Dependent image type?
//---------   The only places where Virtual images and Dependent images are 
//---------   treated differently are: 
//---------     SVImageClass::UpdateFromToolInformation()
//---------     SVImageClass::ValidateAgainstChildrenExtents()
//---------   Does not set Tool Image within UpdateFromToolInformation().
			SVImageTypeDependent   = 3,

//--------- SVImageTypePhysical - Normal buffer type for most output images. 
//---------   Originally Logical images were also allowed as output images; 
//---------   but problems forced us to simplify the use case logic and make 
//---------   them all physical.  This could be re-visited in the future.
			SVImageTypePhysical    = 4,

//--------- SVImageTypeLogicalAndPhysical (previously SVImageTypeLogical) -
//---------   SHOULD NOT BE USED FOR NEW IMPLEMENTATIONS.
//---------   SVImageTypeLogicalAndPhysical and SVImageTypeLogical are the 
//---------   only two image types that result in a Matrox "child" buffer
//---------   (as opposed to an SVO "child" buffer).
//---------   SVImageTypeLogicalAndPhysical also results in allocation of 
//---------   physical buffers.  In general I would consider this a mistake or
//---------   bad design; but they are used (the physical buffers) by the 
//---------   SVLinearImageOperatorListClass and the 
//---------   SVStdImageOperatorListClass. 
//---------   @TODO someday the logical buffer 
//---------   portions should be replaced with the new SVImageTypeLogical
//---------   buffer, and the physical portions should be replaced with the 
//---------   SVImageTypePhysical buffer.  This type should be obsoleted and
//---------   removed.
//---------   Dependent and Virtual image types maintain a child->parent 
//---------   relationship (nullptr != m_ParentImagePtr) but are still allocated 
//---------   as physical buffers?
			SVImageTypeLogicalAndPhysical = 5,

//--------- SVImageTypeVirtual - Not used. 
//---------
//---------   Dependent and Virtual image types maintain a child->parent 
//---------   relationship (nullptr != m_ParentImagePtr) but are still allocated 
//---------   as physical buffers?
//---------
//---------   Dependent and Virtual image types maintain a child->parent 
//---------   relationship at the SVImageClass level (nullptr != m_ParentImagePtr) 
//---------   but are still allocated as physical buffers.
//---------
//---------	  This image type does not create an image buffer pool as part of
//---------	  the SVImageObjectClass.  Instead its buffer pool is maintained 
//---------   as a child image of the SVImageClass parent!
//---------
//---------   Virtual image type can be a child of a Dependent image type?
//---------   The only places where Virtual images and Dependent images are 
//---------   treated differently are: 
//---------     SVImageClass::UpdateFromToolInformation()
//---------     SVImageClass::ValidateAgainstChildrenExtents()
			SVImageTypeVirtual     = 6,

//--------- SVImageTypeMain - only used for the gray scale MainImage.  Set by 
//---------   SVMainImageClass constructor. 
			SVImageTypeMain        = 7,

//--------- SVImageTypeRGBMain - only used for the color MainImage.  Set by 
//---------   SVRGBMainImageClass constructor. 
			SVImageTypeRGBMain     = 8,

//--------- SVImageTypeLogical -
//---------   SVImageTypeLogicalAndPhysical and SVImageTypeLogical are the 
//---------   only two image types that result in a Matrox "child" buffer
//---------   (as opposed to an SVO "child" buffer).
//---------   Dependent and Virtual image types maintain a child->parent 
//---------   relationship (nullptr != m_ParentImagePtr) but are still allocated 
//---------   as physical buffers.
			SVImageTypeLogical = 9
		};

		enum SVImagePropertyEnum
		{
			SVImagePropertyAll = 0x0000001F,
			SVImagePropertyFormat = 0x00000001,
			SVImagePropertyPixelDepth = 0x00000002,
			SVImagePropertyBandNumber = 0x00000004,
			SVImagePropertyBandLink = 0x00000008,
			SVImagePropertyResetOwner = 0x00000010,
		};
		
		enum SVDigitizerParameterEnum
		{
			SVDigitizerParameterUnknown = 0,
			SVDigitizerParameterBrightness = 100,
			SVDigitizerParameterBrightnessMax = 101,
			SVDigitizerParameterBrightnessMin = 102,
			SVDigitizerParameterBrightnessStep = 103,
			SVDigitizerParameterBrightnessRed = 110,
			SVDigitizerParameterBrightnessRedMax = 111,
			SVDigitizerParameterBrightnessRedMin = 112,
			SVDigitizerParameterBrightnessRedStep = 113,
			SVDigitizerParameterBrightnessGreen = 120,
			SVDigitizerParameterBrightnessGreenMax = 121,
			SVDigitizerParameterBrightnessGreenMin = 122,
			SVDigitizerParameterBrightnessGreenStep = 123,
			SVDigitizerParameterBrightnessBlue = 130,
			SVDigitizerParameterBrightnessBlueMax = 131,
			SVDigitizerParameterBrightnessBlueMin = 132,
			SVDigitizerParameterBrightnessBlueStep = 133,
			SVDigitizerParameterContrast = 140,
			SVDigitizerParameterContrastMax = 141,
			SVDigitizerParameterContrastMin = 142,
			SVDigitizerParameterContrastStep = 143,
			SVDigitizerParameterContrastRed = 150,
			SVDigitizerParameterContrastRedMax = 151,
			SVDigitizerParameterContrastRedMin = 152,
			SVDigitizerParameterContrastRedStep = 153,
			SVDigitizerParameterContrastGreen = 160,
			SVDigitizerParameterContrastGreenMax = 161,
			SVDigitizerParameterContrastGreenMin = 162,
			SVDigitizerParameterContrastGreenStep = 163,
			SVDigitizerParameterContrastBlue = 170,
			SVDigitizerParameterContrastBlueMax = 171,
			SVDigitizerParameterContrastBlueMin = 172,
			SVDigitizerParameterContrastBlueStep = 173,
			SVDigitizerParameterLUT = 180,
			SVDigitizerParameterLUTFormat = 181,
			SVDigitizerParameterLUTBandSize = 182,
			SVDigitizerParameterLUTBandCount = 183,
		};

		enum SVDigitizerParameterTypeEnum
		{
			SVDigitizerParameterTypeUnknown = 0,
			SVDigitizerParameterTypeAnyValue = 10,
			SVDigitizerParameterTypeNumericValue = 11,
			SVDigitizerParameterTypeRangedValue = 12,
			SVDigitizerParameterTypeEnum = 20,
			SVDigitizerParameterTypeEnumWithValue = 21,
			SVDigitizerParameterTypeLUT = 30,
		};
//	}  //namespace ObjectInterfaces
//} //namespace Seidenader

