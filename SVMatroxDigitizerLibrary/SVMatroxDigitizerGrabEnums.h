//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxDigitizerGrabEnums
//* .File Name       : $Workfile:   SVMatroxDigitizerGrabEnums.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:27:42  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <vector>
//Moved to precompiled header: #include <utility>
#include "SVMatroxLibrary/SVMatroxTypedefs.h"
#include "SVMatroxLibrary/SVMatroxEnumConvertor.h"
#pragma endregion Includes

/**
@SVObjectName Matrox Digitizer Grab Enum

@SVObjectOverview This enum is used to get/set the Grab parameters for the Digitizer.

@SVObjectOperations None

*/
struct SVMatroxDigitizerGrab
{
	// M_GRAB_MODE Sets how the grab should be synchronized with the Host when grabbing data with MdigGrab(). 
	// Note that this does not affect MdigGrabContinuous(), which always runs asynchronously, otherwise MdigHalt() could not stop the grab. 
	enum SVGrabModeEnum
	{
		SVGrabAsynchronous,			// M_ASYNCHRONOUS
		SVGrabAsynchronousQueued,	// M_ASYNCHRONOUS_QUEUED
		SVGrabSynchronous,			// M_SYNCHRONOUS  
	};
	// define converter for from/to SVEnum/Matrox types
	typedef std::vector<std::pair<SVGrabModeEnum, long long>> SVGrabModeEnumPair;
	static SVGrabModeEnumPair m_GrabModeEnumConvertor;

	// Grab Wait
	enum SVGrabWaitEnum
	{
		SVGrabEnd,		// M_GRAB_END 
						// Wait for the end of the queued grabs. 
						// This value should not be used when grabbing data with MdigGrabContinuous(). 
		SVGrabFrameEnd, // M_GRAB_FRAME_END 
						// Waits for the end of the current grab. 
	};
	// define converter for from/to SVEnum/Matrox types
	typedef std::vector<std::pair<SVGrabWaitEnum, long long>> SVGrabWaitEnumPair;
	static SVGrabWaitEnumPair m_GrabWaitEnumConvertor;

	// M_GRAB_TRIGGER Sets the grab trigger detection state.
	enum SVGrabTriggerEnum		// M_GRAB_TRIGGER Sets the grab trigger detection state.
	{
		SVGrabTriggerDefault,	// M_DEFAULT  Same as the one specified by the DCF or, if none specified, M_DISABLE. 
		SVGrabTriggerActivate,	// M_ACTIVATE  Starts the grab immediately (that is, issues a software trigger). 
		SVGrabTriggerDisable,	// M_DISABLE  Disables trigger detection. 
		SVGrabTriggerEnable		// M_ENABLE  Enables trigger detection. 
	};
	// define convertor for from/to SVEnum/Matrox types
	typedef std::vector<std::pair<SVGrabTriggerEnum, long long>> SVGrabTriggerEnumPair;
	static SVGrabTriggerEnumPair m_GrabTriggerEnumConvertor;

	// M_GRAB_TRIGGER_MODE Sets the hardware trigger activation mode.
	enum SVGrabTriggerModeEnum
	{
		SVGrabTriggerModeDefault,	// M_DEFAULT  Same as the one specified by the DCF. 
		SVEdgeFalling,				// M_EDGE_FALLING  Specifies to trigger upon a high to low signal variation. 
		SVEdgeRising,				// M_EDGE_RISING  Specifies to trigger upon a low to high signal variation. 
		SVLevelHigh,				// M_LEVEL_HIGH  Specifies to trigger while a maximum signal level occurs. 
		SVLevelLow					// M_LEVEL_LOW  Specifies to trigger while a minimum signal level occurs. 
 	};
	// define convertor for from/to SVEnum/Matrox types
	typedef std::vector<std::pair<SVGrabTriggerModeEnum, long long>> SVGrabTriggerModeEnumPair;
	static SVGrabTriggerModeEnumPair m_GrabTriggerModeEnumConvertor;
};

