//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Enum list for images
//******************************************************************************

#pragma once

namespace SvDef
{
	enum SVImageTypeEnum // Used by SVImageClass
	{
//--------- SVImageTypeUnknown - only for initialization at construction. 
//---------   Should be set to a different value by first 
//---------	  SVImageClass::Initialize() call.
//---------	  This image type does not create an image buffer pool?
		SVImageTypeUnknown     = 0,

//--------- SVImageTypeFixed - deprecated
//---------   Physical (Parent) buffer.
//---------   Does not set Tool Image within UpdateFromToolInformation().
//		SVImageTypeFixed       = 1,

//--------- SVImageTypeIndependent
//---------   Physical (Parent) buffer.
//---------   Does not set Tool Image within UpdateFromToolInformation().
		SVImageTypeIndependent = 2,

//--------- SVImageTypeDependent - Only used to define the 3 banded images of 
//---------   a Color Tool.
//---------
//---------   Logical buffer (similar to SVImageTypeLogical)
//---------   SVImageTypeLogicalAndPhysical, SVImageTypeLogical and SVImageTypeDependent are the 
//---------   image types that result in a Matrox "child" buffer
//---------   (as opposed to an SVO "child" buffer).
//---------   Dependent image types maintain a child->parent 
//---------   relationship at the SVImageClass level (nullptr != m_ParentImagePtr)
//---------
//---------	  Different between to SVImageTypeLogical is that this type have the same size as the parent and SVImageTypeLogical can be a part of the parent.
//---------   Also this type does not set Tool Image within UpdateFromToolInformation(), but SVImageTypeLogical do.
		SVImageTypeDependent   = 3,

//--------- SVImageTypePhysical - Normal buffer type for most output images. 
//---------   Originally Logical images were also allowed as output images; 
//---------   but problems forced us to simplify the use case logic and make 
//---------   them all physical.  This could be re-visited in the future.
		SVImageTypePhysical    = 4,

//--------- SVImageTypeLogicalAndPhysical (previously SVImageTypeLogical) -
//---------   SHOULD NOT BE USED FOR NEW IMPLEMENTATIONS.
//---------   SVImageTypeLogicalAndPhysical, SVImageTypeLogical and SVImageTypeDependent are the 
//---------   image types that result in a Matrox "child" buffer
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
//		SVImageTypeLogicalAndPhysical = 5, deprecated

//		SVImageTypeVirtual     = 6, - Not used.

//--------- SVImageTypeMain - only used for the MainImage.  Set by 
//---------   SVMainImageClass constructor. 
		SVImageTypeMain        = 7,

//--------- SVImageTypeRGBMain - deprecated
//		SVImageTypeRGBMain     = 8,

//--------- SVImageTypeLogical -
//---------   SVImageTypeLogicalAndPhysical, SVImageTypeLogical and SVImageTypeDependent are the 
//---------   image types that result in a Matrox "child" buffer
//---------   (as opposed to an SVO "child" buffer).
		SVImageTypeLogical = 9
	};

	enum SVImagePropertyEnum
	{
		SVImagePropertyAll = 0x0000001F,
		SVImagePropertyFormat = 0x00000001,
		SVImagePropertyPixelDepth = 0x00000002,
		SVImagePropertyBandNumber = 0x00000004,
		SVImagePropertyBandLink = 0x00000008,
	};
} //namespace SvDef

