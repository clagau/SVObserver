//******************************************************************************
//* COPYRIGHT (c) 2009 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxDigitizerControlEnums
//* .File Name       : $Workfile:   SVMatroxDigitizerControlEnums.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Oct 2013 10:45:28  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <vector>
//Moved to precompiled header: #include <utility>
#pragma endregion Includes

struct SVMatroxDigitizerControl
{
	// Camera
	enum SVCameraEnum
	{
		SVSourceOffsetX,			// M_SOURCE_OFFSET_X
									// Sets the X-offset of the input signal capture window. 
									// Note that certain source sizes, offsets, and destination buffer sizes can affect a grab. 
									// Refer to MdigGrab() and MdigGrabContinuous() for more information. 
									// Value  Specifies the X-offset, in pixels. 

		SVSourceOffsetY,			// M_SOURCE_OFFSET_Y
									// Sets the Y-offset of the input signal capture window. 
									// Value  Specifies the Y-offset, in pixels. 
									// Note that certain source sizes, offsets, and destination buffer sizes can affect a grab. 
									// Refer to MdigGrab() and MdigGrabContinuous() for more information. 
 
		SVSourceSizeX,				// M_SOURCE_SIZE_X
									// Sets the width of the input signal capture window. 
									// Note that certain source sizes, offsets, and destination buffer sizes can affect a grab. 
									// Refer to MdigGrab() and MdigGrabContinuous() for more information. 
									// Value  Specifies the width, in pixels. 
 
		SVSourceSizeY,				// M_SOURCE_SIZE_Y
									// Sets the height of the input signal capture window. 
									// Note that certain source sizes, offsets, and destination buffer sizes can affect a grab. Refer to MdigGrab() and MdigGrabContinuous() for more information. 
									// Value  Specifies the height, in pixels. 
 	};

	// define convertor for from/to SVEnum/Matrox types
	typedef std::vector<std::pair<SVCameraEnum, long long>> SVCameraEnumPair;

	static SVCameraEnumPair m_CameraEnumConvertor;
};

struct SVMatroxDigitizerFormat
{
	POINT offset;
	SIZE size;
	long bitDepth;
	long numBands;
};

