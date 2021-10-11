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
#include "TextDefineSvDef.h"
namespace SvDef
{

enum SVExtentShapeEnum
{
	SVExtentShapeUnknown		= 0,
	SVExtentShapePoint			= 100,
	SVExtentShapeArrow			= 200,
	SVExtentShapeLine			= 300,
	SVExtentShapeLineHorizontal = 301,
	SVExtentShapeLineVertical	= 302,
	SVExtentShapeRectangle		= 400,
	SVExtentShapePie			= 500,
	SVExtentShapeCircle			= 600,
	SVExtentShapeDoubleCircle	= 700,
	SVExtentShapeHexagon		= 800,
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

enum ToolSizeAdjustEnum
{ 
	TSPositionX = 0, 
	TSPositionY, 
	TSWidth, 
	TSHeight, 
	TSValuesCount 
};

enum ToolSizeModes 
{ 
	TSNone = 0, 
	TSAutoFit, 
	TSFormula, 
	TSModeCount 
};

static const ToolSizeAdjustEnum   AllToolSizeAdjustEnum[] =
{
	SvDef::TSPositionX,SvDef::TSPositionY,SvDef::TSWidth,SvDef::TSHeight
};

inline const std::string  ToString(SvDef::ToolSizeAdjustEnum e)
{
	const std::map<SvDef::ToolSizeAdjustEnum, const char*> MyEnumStrings {
		{ SvDef::TSPositionX, "TSPositionX" },
		{ SvDef::TSPositionY, "TSPositionY" },
		{ SvDef::TSWidth, "TSWidth" },
		{ SvDef::TSHeight, "TSHeight" },

	};
	auto   it = MyEnumStrings.find(e);
	return it == MyEnumStrings.end() ? "Out of range" : it->second;
}

inline const std::string  ToString(SvDef::ToolSizeModes e)
{
	const std::map<SvDef::ToolSizeModes, const char*> MyEnumStrings {
		{ SvDef::TSNone, SizeAdjustTextNone },
		{ SvDef::TSAutoFit, SizeAdjustTextAutoFitSize },
		{ SvDef::TSFormula, SizeAdjustTextFormula },
	};
	auto   it = MyEnumStrings.find(e);
	return it == MyEnumStrings.end() ? "Out of range" : it->second;
}
} //namespace SvDef
