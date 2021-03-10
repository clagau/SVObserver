// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxEnums
// * .File Name       : $Workfile:   SVMatroxEnums.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 15:13:12  $
// ******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <vector>
//Moved to precompiled header: #include <utility>
#pragma endregion Includes

#include "Definitions\TextDefineSvDef.h"

//// Image processing enums
/**
@SVObjectName Matrox Image Operation Type Enum

@SVObjectOverview This enum is used for the Matrox Image Interface.

@SVObjectOperations None

*/

//	Uses include Resize function
enum InterpolationMode
{
	Default =					0x00000004L, // M_INTERPOLATE
	Bilinear =					0x00000008L, // M_BILINEAR 
	Bicubic =					0x00000010L, // M_BICUBIC
	InterpolationModeAverage =	0x00000020L, // M_AVERAGE
	NearestNeighbor =			0x00000040L  // M_NEAREST_NEIGHBOR 
};

//	Used by Resize function
enum OverscanActive
{
	OverscanInitialize =	0,           // for initialization only
	OverscanEnable =		0x00008000L, // M_OVERSCAN_ENABLE
	OverscanDisable =		0x00080000L  // M_OVERSCAN_DISABLE
};


static std::initializer_list<InterpolationMode> c_allInterpolationModes
{
	InterpolationMode::Default,
	InterpolationMode::Bilinear,
	InterpolationMode::Bicubic,
	InterpolationMode::NearestNeighbor
};

static std::initializer_list<OverscanActive> c_allOverscanStates
{
	OverscanActive::OverscanInitialize,
	OverscanActive::OverscanEnable,
	OverscanActive::OverscanDisable
};

const std::initializer_list<std::pair<std::string, InterpolationMode>> c_allInterpolationModeNameValuePairs
{
	{ SvDef::Interpolate, InterpolationMode::Default },
	{ SvDef::Bicubic, InterpolationMode::Bicubic },
	{ SvDef::Bilinear, InterpolationMode::Bilinear },
	{ SvDef::NearestNeighbor, InterpolationMode::NearestNeighbor },
};


const std::initializer_list<std::pair<std::string, OverscanActive>> c_allOverscanActiveNameValuePairs
{
	{ SvDef::Enabled, OverscanActive::OverscanEnable },
	{ SvDef::Disabled, OverscanActive::OverscanDisable },
};
