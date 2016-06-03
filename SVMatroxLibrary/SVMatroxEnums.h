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

#include "SVMatroxEnumConvertor.h" //SVMatroxEnumConvertor 
///all enums from this file were moved to SVMatroxSimpleEnums.h"
#include "SVMatroxSimpleEnums.h" 



//// Image processing enums
/**
@SVObjectName Matrox Image Operation Type Enum

@SVObjectOverview This enum is used for the Matrox Image Interface.

@SVObjectOperations None

*/

//	Uses include Resize function
class SVInterpolationModeOptions
{
public:
	enum SVInterpolationModeOptionsEnum
	{
		InterpolationModeInitialize				= 0,			// for initialization only
		InterpolationModeAuto					= 0x00000004L,  // M_INTERPOLATE
		InterpolationModeBilinear				= 0x00000008L,	// M_BILINEAR 
		InterpolationModeBicubic				= 0x00000010L,	// M_BICUBIC
		InterpolationModeAverage				= 0x00000020L,	// M_AVERAGE
		InterpolationModeNearestNeighbor		= 0x00000040L	// M_NEAREST_NEIGHBOR 
	};

	typedef SVMatroxEnumConvertor<SVInterpolationModeOptionsEnum, long> SVInterpolationModeOptionsEnumMap;
	static SVInterpolationModeOptionsEnumMap m_Convertor;
};


//	Uses include Resize function
class SVOverscanOptions
{
public:
	enum SVOverscanOptionsEnum
	{
		OverscanInitialize					= 0,			// for initialization only
		OverscanEnable						= 0x00008000L, // M_OVERSCAN_ENABLE
		OverscanDisable						= 0x00080000L  // M_OVERSCAN_DISABLE
	};

	typedef SVMatroxEnumConvertor<SVOverscanOptionsEnum, long> SVOverscanOptionsEnumMap;
	static SVOverscanOptionsEnumMap m_Convertor;
};


//	Uses include Resize function
class SVPerformanceOptions
{
public:
	enum SVPerformanceOptionsEnum
	{
		PerformanceInitialize				= 0,			// for initialization only
		PerformanceFast						= 0x00002000L, // M_FAST
		PerformancePresice					= 0x00020000L  // M_REGULAR
	};


	typedef SVMatroxEnumConvertor<SVPerformanceOptionsEnum, long> SVPerformanceOptionsEnumMap;
	static SVPerformanceOptionsEnumMap m_Convertor;
};


