//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVExtentEnums
//* .File Name       : $Workfile:   SVExtentEnums.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   26 May 2014 10:23:38  $
//******************************************************************************

#ifndef _SVTOOLEXTENTENUMS_H
#define _SVTOOLEXTENTENUMS_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum SVExtentPropertyEnum
{
	SVExtentPropertyPositionPointX          = 0x00000001,
	SVExtentPropertyPositionPointY          = 0x00000002,
	SVExtentPropertyPositionPoint           = 0x00000003,
	SVExtentPropertyPositionPointEndOfLineX = 0x00000004,
	SVExtentPropertyPositionPointEndOfLineY = 0x00000008,
	SVExtentPropertyPositionPointEndOfLine  = 0x0000000C,
	SVExtentPropertyRotationAngle           = 0x00000010,
	SVExtentPropertyTranslationOffsetX      = 0x00000040,
	SVExtentPropertyTranslationOffsetY      = 0x00000080,
	SVExtentPropertyTranslationOffset       = 0x000000C0,
	SVExtentPropertyOutputPositionPointX    = 0x00000100,
	SVExtentPropertyOutputPositionPointY    = 0x00000200,
	SVExtentPropertyOutputPositionPoint     = 0x00000300,

	SVExtentPropertyOldRotationAngle        = 0x00000020,
	SVExtentPropertyOldPositionPointX       = 0x00000400,
	SVExtentPropertyOldPositionPointY       = 0x00000800,
	SVExtentPropertyOldPositionPoint        = 0x00000C00,
	SVExtentPropertyOldAbsPositionPointX    = 0x00001000,
	SVExtentPropertyOldAbsPositionPointY    = 0x00002000,
	SVExtentPropertyOldAbsPositionPoint     = 0x00003000,
	SVExtentPropertyOldRotationPointX       = 0x00004000,
	SVExtentPropertyOldRotationPointY       = 0x00008000,
	SVExtentPropertyOldRotationPoint        = 0x0000C000,
	SVExtentPropertyOldTranslationOffsetX   = 0x00010000,
	SVExtentPropertyOldTranslationOffsetY   = 0x00020000,
	SVExtentPropertyOldTranslationOffset    = 0x00030000,

	SVExtentPropertyStartAngle              = 0x01000000,
	SVExtentPropertyEndAngle                = 0x02000000,
	SVExtentPropertyInnerRadius             = 0x04000000,
	SVExtentPropertyOuterRadius             = 0x08000000,
	SVExtentPropertyWidth                   = 0x10000000,
	SVExtentPropertyHeight                  = 0x20000000,
	SVExtentPropertyOutputWidth             = 0x40000000,
	SVExtentPropertyOutputHeight            = 0x80000000,

	SVExtentPropertyPositionsInput = SVExtentPropertyPositionPointX
	                               | SVExtentPropertyPositionPointY
	                               | SVExtentPropertyPositionPointEndOfLineX
	                               | SVExtentPropertyPositionPointEndOfLineY
	                               | SVExtentPropertyTranslationOffsetX
	                               | SVExtentPropertyTranslationOffsetY
	                               | SVExtentPropertyRotationAngle,

	SVExtentPropertyPositionsOutput = SVExtentPropertyOutputPositionPointX
	                                | SVExtentPropertyOutputPositionPointY,

	SVExtentPropertyDimensionsInput = SVExtentPropertyWidth
	                                | SVExtentPropertyHeight
	                                | SVExtentPropertyStartAngle
	                                | SVExtentPropertyEndAngle
	                                | SVExtentPropertyInnerRadius
	                                | SVExtentPropertyOuterRadius,

	SVExtentPropertyDimentionsOutput = SVExtentPropertyOutputWidth
	                                 | SVExtentPropertyOutputHeight,

	SVExtentPropertyRectangle = SVExtentPropertyWidth
	                          | SVExtentPropertyHeight
	                          | SVExtentPropertyDimentionsOutput,

	SVExtentPropertyPoint = SVExtentPropertyRectangle,

	SVExtentPropertyLine = SVExtentPropertyRectangle,

	SVExtentPropertyPie = SVExtentPropertyStartAngle
	                    | SVExtentPropertyEndAngle
	                    | SVExtentPropertyInnerRadius
	                    | SVExtentPropertyOuterRadius
                      | SVExtentPropertyDimentionsOutput,

	SVExtentPropertyOldPositionsAll = SVExtentPropertyOldRotationAngle
	                                | SVExtentPropertyOldPositionPointX
		                              | SVExtentPropertyOldPositionPointY
		                              | SVExtentPropertyOldPositionPoint
		                              | SVExtentPropertyOldAbsPositionPointX
		                              | SVExtentPropertyOldAbsPositionPointY
		                              | SVExtentPropertyOldAbsPositionPoint
		                              | SVExtentPropertyOldRotationPointX
		                              | SVExtentPropertyOldRotationPointY
		                              | SVExtentPropertyOldRotationPoint
		                              | SVExtentPropertyOldTranslationOffsetX
		                              | SVExtentPropertyOldTranslationOffsetY
		                              | SVExtentPropertyOldTranslationOffset,

	SVExtentPropertyPositionsAll = SVExtentPropertyOldPositionsAll
	                             | SVExtentPropertyPositionsInput
	                             | SVExtentPropertyPositionsOutput,

	SVExtentPropertyDimensionsAll = SVExtentPropertyDimensionsInput
	                              | SVExtentPropertyDimentionsOutput,

	SVExtentPropertyAll = SVExtentPropertyPositionsAll
	                    | SVExtentPropertyDimensionsAll,

	SVExtentPropertyNone = 0,
};

//This parameter define how many decimal places the property have in the display
const unsigned long g_SVExtentPropertyNoDecimalPlaces = SVExtentPropertyPositionPoint|SVExtentPropertyPositionPointEndOfLine|SVExtentPropertyTranslationOffset|
														SVExtentPropertyOldPositionPoint|SVExtentPropertyOldAbsPositionPoint|SVExtentPropertyOldTranslationOffset|
														SVExtentPropertyOutputPositionPoint|SVExtentPropertyOldRotationPoint|SVExtentPropertyWidth|SVExtentPropertyHeight|
														SVExtentPropertyOutputWidth|SVExtentPropertyOutputHeight;
const unsigned long g_SVExtentProperty2DecimalPlaces = SVExtentPropertyRotationAngle|SVExtentPropertyOldRotationAngle|SVExtentPropertyStartAngle|
														SVExtentPropertyEndAngle|SVExtentPropertyInnerRadius|SVExtentPropertyOuterRadius;
	

enum SVExtentShapeEnum
{
	SVExtentShapeUnknown   = 0,
	SVExtentShapePoint     = 100,
	SVExtentShapeArrow     = 200,
	SVExtentShapeLine      = 300,
	SVExtentShapeLineHorizontal = 301,
	SVExtentShapeLineVertical = 302,
	SVExtentShapeRectangle = 400,
	SVExtentShapePie       = 500,
	SVExtentShapeCircle    = 600,
	SVExtentShapeDoubleCircle = 700,
	SVExtentShapeHexagon   = 800,
};

enum SVExtentLocationPropertyEnum
{
	SVExtentLocationPropertyUnknown     = 0,
	SVExtentLocationPropertyTopLeft     = 1,
	SVExtentLocationPropertyTopRight    = 2,
	SVExtentLocationPropertyBottomRight = 3,
	SVExtentLocationPropertyBottomLeft  = 4,
	SVExtentLocationPropertyLeft        = 5,
	SVExtentLocationPropertyRight       = 6,
	SVExtentLocationPropertyTop         = 7,
	SVExtentLocationPropertyBottom      = 8,
	SVExtentLocationPropertyCenter      = 9,
	SVExtentLocationPropertyRotate      = 10,
	SVExtentLocationPropertySize,
};

enum SVExtentShapePointEnum
{
	SVExtentShapePointUnknown      = 0,
	SVExtentShapePointTopLeft      = 1,
	SVExtentShapePointTopRight     = 2,
	SVExtentShapePointBottomRight  = 3,
	SVExtentShapePointBottomLeft   = 4,
	SVExtentShapePointCenterLeft   = 5,
	SVExtentShapePointCenterRight  = 6,
	SVExtentShapePointTopCenter    = 7,
	SVExtentShapePointBottomCenter = 8,
	SVExtentShapePointSize,
};

enum SVExtentTranslationEnum
{
	SVExtentTranslationUnknown         = 0,
	SVExtentTranslationNone            = 10,
	SVExtentTranslationShift           = 20,
	SVExtentTranslationRotate          = 30,
	SVExtentTranslationFlippedRotate   = 31,
	SVExtentTranslationProfile         = 40,
	SVExtentTranslationProfileShift    = 41,
	SVExtentTranslationTransformShift  = 50,
	SVExtentTranslationTransformRotate = 51,
	SVExtentTranslationPolarUnwrap     = 60,
	SVExtentTranslationBuildReference  = 70,
	SVExtentTranslationLine            = 80,
	SVExtentTranslationDoubleHeight    = 90,
	SVExtentTranslationFlipVertical	   = 91,
	SVExtentTranslationFlipHorizontal  = 92,
	SVExtentTranslationFigureShift	   = 93,
	SVExtentTranslationLinear          = 100,
	SVExtentTranslationCylindricalWarpH = 110,
	SVExtentTranslationCylindricalWarpV = 111,
	SVExtentTranslationHorizontalPerspective = 120,
	SVExtentTranslationVerticalPerspective = 121,
};

#endif // _SVTOOLEXTENTENUMS_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVImageLibrary\SVExtentEnums.h_v  $
 * 
 *    Rev 1.1   26 May 2014 10:23:38   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  894
 * SCR Title:  Enhancements to Adjust Tool Position Dialog
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   add constants g_SVExtentPropertyNoDecimalPlaces and g_SVExtentProperty2DecimalPlaces
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 10:39:40   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   13 Apr 2011 14:47:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated system to fix color processing issues with color configurations.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   17 Jun 2005 13:37:56   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  487
 * SCR Title:  Add new blob feature for source extents of blob box center
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   new translation type SVExtentTranslationFigureShift
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   20 May 2005 13:30:08   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  487
 * SCR Title:  Add new blob feature for source extents of blob box center
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added new translation types
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   18 Mar 2005 09:03:52   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  463
 * SCR Title:  Add new Linear Measurement Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated Line data to include draw angle to remove rounding issues with drawing straight lines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   09 Mar 2005 14:01:08   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  478
 * SCR Title:  Add a new Perspective Warp Tool
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changes to support the new Perspective Warp Tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   09 Mar 2005 11:52:12   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  477
 * SCR Title:  Add Cylindrical warp Tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added CylindricalWarp
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Feb 2005 14:02:46   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   First Iteration of File
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/