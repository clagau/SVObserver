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

#pragma once

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

	SVExtentPropertyHeightScaleFactor		= 0x00040000, ///< Used by Resize Tool and the Image Tool Height Doubling operation
	
	SVExtentPropertyWidthScaleFactor		= 0x00080000, ///< Used by Resize Tool

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
	                                | SVExtentPropertyOuterRadius
									| SVExtentPropertyHeightScaleFactor
									| SVExtentPropertyWidthScaleFactor,

	SVExtentPropertyDimentionsOutput = SVExtentPropertyOutputWidth
	                                 | SVExtentPropertyOutputHeight,

	SVExtentPropertyRectangle = SVExtentPropertyWidth
	                          | SVExtentPropertyHeight
	                          | SVExtentPropertyDimentionsOutput,

	SVExtentPropertyScaleFactor = SVExtentPropertyWidthScaleFactor
								| SVExtentPropertyHeightScaleFactor,

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
	SVExtentLocationPropertyDisabled =  11,
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

	/// SVExtentTranslationResize - Does a position, height, and width, 
	///  translation that uses the scale factor values.
	///  Uses include the Resize Tool.
	SVExtentTranslationResize		   = 89,

	/// SVExtentDoubleHeight - Does a position, and height translation 
	///  that uses the height scale factor value.
	///  Uses include the Image Tool, Double Height operation.
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

enum SVExtentDirectionsEnum
{
	SVExtentDirectionBoth         = 0,
	SVExtentDirectionHorizontal            = 1,
	SVExtentDirectionVertical           = 2
};

