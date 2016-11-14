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

#include "SVMatroxLibrary/SVMatroxTypedefs.h"
#include "SVMatroxLibrary/SVMatroxEnumConvertor.h"

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
	// define convertor for from/to SVEnum/Matrox types
	typedef SVMatroxEnumConvertor<SVGrabModeEnum, long> SVGrabModeEnumMap;
	static SVGrabModeEnumMap m_GrabModeEnumConvertor;

	// Grab Wait
	enum SVGrabWaitEnum
	{
		SVGrabEnd,		// M_GRAB_END 
						// Wait for the end of the queued grabs. 
						// This value should not be used when grabbing data with MdigGrabContinuous(). 
		SVGrabFrameEnd, // M_GRAB_FRAME_END 
						// Waits for the end of the current grab. 
	};
	// define convertor for from/to SVEnum/Matrox types
	typedef SVMatroxEnumConvertor<SVGrabWaitEnum, long> SVGrabWaitEnumMap;
	static SVGrabWaitEnumMap m_GrabWaitEnumConvertor;

	// M_GRAB_DIRECTION_X
	// M_GRAB_DIRECTION_Y
	// Sets the horizontal/vertical grab direction.
	enum SVGrabDirectionEnum
	{
		SVGrabDirectionDefault,	//  M_DEFAULT - Same as M_FORWARD . 
		SVForward,				//  M_FORWARD - Grabs from left to right, in the horizontal direction. This is the default value.
		SVReverse				//  M_REVERSE - Flips the grabbed image vertically. 
	};
	// define convertor for from/to SVEnum/Matrox types
	typedef SVMatroxEnumConvertor<SVGrabDirectionEnum, long> SVGrabDirectionEnumMap;
	static SVGrabDirectionEnumMap m_GrabFirectionEnumConvertor;

	// M_GRAB_TRIGGER Sets the grab trigger detection state.
	enum SVGrabTriggerEnum		// M_GRAB_TRIGGER Sets the grab trigger detection state.
	{
		SVGrabTriggerDefault,	// M_DEFAULT  Same as the one specified by the DCF or, if none specified, M_DISABLE. 
		SVGrabTriggerActivate,	// M_ACTIVATE  Starts the grab immediately (that is, issues a software trigger). 
		SVGrabTriggerDisable,	// M_DISABLE  Disables trigger detection. 
		SVGrabTriggerEnable		// M_ENABLE  Enables trigger detection. 
	};
	// define convertor for from/to SVEnum/Matrox types
	typedef SVMatroxEnumConvertor<SVGrabTriggerEnum, long> SVGrabTriggerEnumMap;
	static SVGrabTriggerEnumMap m_GrabTriggerEnumConvertor;

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
	typedef SVMatroxEnumConvertor<SVGrabTriggerModeEnum, long> SVGrabTriggerModeEnumMap;
	static SVGrabTriggerModeEnumMap m_GrabTriggerModeEnumConvertor;
};

