//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxDigitizerInquireEnums
//* .File Name       : $Workfile:   SVMatroxDigitizerInquireEnums.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Oct 2013 10:45:28  $
//******************************************************************************

#pragma once

#include "SVMatroxLibrary/SVMatroxTypedefs.h"
#include "SVMatroxLibrary/SVMatroxEnumConvertor.h"

/**
@SVObjectName Matrox Digitizer Inquire Enum

@SVObjectOverview This enum is used to get the paramwters of the Digitizer.

@SVObjectOperations None

*/

struct SVMatroxDigitizerInquire
{
	// Board
	enum SVBoardEnum
	{
		SVChannel,					// M_CHANNEL
		SVChannelNum,				// M_CHANNEL_NUM
	};
	// define convertor for from/to SVEnum/Matrox types
	typedef SVMatroxEnumConvertor<SVBoardEnum, long> SVBoardEnumMap;
	static SVBoardEnumMap m_BoardEnumConvertor;
	
	// Camera Format
	enum SVColorModeEnum
	{
		SVComposite,			// M_COMPOSITE  Composite input.  
		SVExternalChrominance,	// M_EXTERNAL_CHROMINANCE  External chrominance input.  
		SVMono8ViaRgb,			// M_MONO8_VIA_RGB  Monochrome 8-bit through RGB input.  
		SVMonochrome,			// M_MONOCHROME  Monochrome input.  
		SVRGB,					// M_RGB  
	};
	typedef SVMatroxEnumConvertor<SVColorModeEnum, long> SVColorModeEnumMap;
	static SVColorModeEnumMap m_ColorModeEnumConvertor;

	enum SVScanModeEnum
	{
		SVInterlace, 	// M_INTERLACE  Interlace mode  
		SVLinescan, 	// M_LINESCAN  Line-scan mode  
		SVProgressive, 	// M_PROGRESSIVE  Progressive mode  
	};
	typedef SVMatroxEnumConvertor<SVScanModeEnum, long> SVScanModeEnumMap;
	static SVScanModeEnumMap m_ScanModeEnumConvertor;
	
	// Frame
	enum SVFrameEnum
	{
		SVProcessFrameCount,	// M_PROCESS_FRAME_COUNT
		SVProcessFrameMissed,		// M_PROCESS_FRAME_MISSED
		SVProcessFrameCorrupted,	// M_PROCESS_FRAME_CORRUPTED
		SVProcessFrameRate,			// M_PROCESS_FRAME_RATE
	};
	typedef SVMatroxEnumConvertor<SVFrameEnum, long> SVFrameEnumMap;
	static SVFrameEnumMap m_FrameEnumConvertor;
/*
	// Grab
	enum SVGrabEnum
	{
		SVGrabPeriod,				// M_GRAB_PERIOD
									// Returns the duration of a frame (as specified in the DCF), in msec. 

		SVGrabContinuousEndTrigger, // M_GRAB_CONTINUOUS_END_TRIGGER
									// Returns whether an automatic trigger is generated after MdigHalt() is issued when performing a triggered continuous grab. 
									// Return values: M_DISABLE, M_ENABLE
									// See MdigControl() with M_GRAB_CONTINUOUS_END_TRIGGER for details.

		SVGrabDirectionX,			// M_GRAB_DIRECTION_X
									// Returns the horizontal grab direction.
									// Return values: M_FORWARD, M_REVERSE
									// See MdigControl() with M_GRAB_DIRECTION_X for details.

		SVGrabDirectionY,			// M_GRAB_DIRECTION_Y
									// Returns the vertical grab direction.
									// Return values: M_FORWARD, M_REVERSE
									// See MdigControl() with M_GRAB_DIRECTION_Y for details.

		SVGrabFieldNum,				// M_GRAB_FIELD_NUM
									// Returns the number of fields grabbed when MdigGrab() is called. 
									// Return values: 1, 2
									// See MdigControl() with M_GRAB_FIELD_NUM for details.

		SVGrabMode,					// M_GRAB_MODE
									// Returns the grab synchronization with the Host.
									// Return values: M_ASYNCHRONOUS, M_ASYNCHRONOUS_QUEUED, M_SYNCHRONOUS
									// See MdigControl() with M_GRAB_MODE for details.
		
		SVGrabScale,				// M_GRAB_SCALE
		
		SVGrabScaleX,				// M_GRAB_SCALE_X

		SVGrabScaleY,				// M_GRAB_SCALE_Y

		SVGrabStartMode,			// M_GRAB_START_MODE

		SVGrabTimeout,				// M_GRAB_TIMEOUT

		SVGrabWindowRange,			// M_GRAB_WINDOW_RANGE
									// Returns the state of limiting the range of the grabbed pixel values. 
									// Return values: M_DISABLE, M_ENABLE
									// See MdigControl() with M_GRAB_WINDOW_RANGE for details.

		SVGrabTrigger,				// M_GRAB_TRIGGER Sets the grab trigger detection state. 
									// M_DEFAULT  Same as the one specified by the DCF or, if none specified, M_DISABLE. 
									// M_ACTIVATE  Starts the grab immediately (that is, issues a software trigger). 
									// To use this setting, the trigger source must be set to software 
									// (use MdigControl() with M_GRAB_TRIGGER_SOURCE set to M_SOFTWARE). 
									// An asynchronous or continuous grab must be in progress. 
									// M_DISABLE  Disables trigger detection. 
									// With a hardware trigger, setting M_GRAB_TRIGGER to M_DISABLE temporarily stops a continuous grab. 
									// M_ENABLE  Enables trigger detection
 
		SVGrabTriggerFormat,		// M_GRAB_TRIGGER_FORMAT
									// Sets whether to enable the TTL, OPTO, or LVDS receivers for triggers, 
									// on systems whose receivers are enabled through software 
									// and where the option of TTL, OPTO, and LVDS are possible...

		SVGrabTriggerMode,			// M_GRAB_TRIGGER_MODE Sets the hardware trigger activation mode. 
									// M_DEFAULT  Same as the one specified by the DCF. 
									// If no trigger mode is specified in the DCF, M_EDGE_RISING is the default. 
									// M_EDGE_FALLING  Specifies to trigger upon a high to low signal variation. 
									// M_EDGE_RISING  Specifies to trigger upon a low to high signal variation. 
									// M_LEVEL_HIGH  Specifies to trigger while a maximum signal level occurs. 
									// M_LEVEL_LOW  Specifies to trigger while a minimum signal level occurs. 
 		
		SVGrabTriggerSource,		// M_GRAB_TRIGGER_SOURCE Sets the source of the grab trigger. 

		SVGrabExposure,				// M_GRAB_EXPOSURE 
									// Sets the state of the specified exposure timer.
									// M_ENABLE: Enables the exposure timer.
									// M_DISABLE: Disables the exposure timer. 
									// M_ACTIVATE: Activates a software trigger for the specified exposure timer. 
									// M_DEFAULT: (M_DISABLE).

		SVGrabExposureClockSource,	// M_GRAB_EXPOSURE_CLOCK_SOURCE 
									// Specifies the clock source that drives the exposure counter for the specified timer.
									// M_AUTOMATIC: The driver selects the best clock source from the 17 internal clocks.
									// M_CLOCK0 to M_CLOCK16: Uses the specified Matrox imaging board's clock source frequency.
									// M_TIMER1 to M_TIMER8: Uses the frequency of a timer, if continuous.
									// Two timer groups exist, 1 to 4 and 5 to 8. A timer of group 1 to 4 
									// cannot be used as a clock source for a timer of group 5 to 8, and vice versa.
									// M_HARDWARE_PORT0 to M_HARDWARE_PORT9: Uses an external clock as the source.
									// M_DEFAULT (M_AUTOMATIC).

		SVGrabExposureFormat,		// M_GRAB_EXPOSURE_FORMAT
									// Sets whether to enable the TTL or RS-422/LVDS transmitters for exposures and user-defined output signals, 
									// on systems whose transmitters are enabled through software and where the option of both TTL and RS-422/LVDS are possible. 
									// Refer to the Technical information appendix in the Installation and Hardware Reference manual for your Matrox imaging board. 
									// Refer to M_USER_OUT_FORMAT to change formats on other Matrox imaging boards. 

		SVGrabExposureMode,			// M_GRAB_EXPOSURE_MODE 
									// Sets the polarity of the active portion of the exposure signal.
									// M_LEVEL_LOW:  Specifies a low signal polarity. 
									// M_LEVEL_HIGH: Specifies a high signal polarity.
									// M_DEFAULT (M_LEVEL_HIGH).
	      
		SVGrabExposureSource,		// M_GRAB_EXPOSURE_SOURCE  
									// Selects the trigger source for the specified exposure timer.
									// M_CONTINUOUS: Runs the specified exposure timer in periodic mode;
									// no actual trigger signal is used.
									// M_HARDWARE_PORT0 to 9: Uses the hardware trigger input signal connected to port 0 to 9.
									// M_TIMER1 to 8: Specifies to use a timer as a trigger. 
									// Two timer groups exist, 1 to 4 and 5 to 8. A timer of group 1 to 4 cannot be used as a 
									// trigger source for a timer of group 5 to 8, and vice versa.
									// M_SOFTWARE: Uses a software trigger. Use MdigControl() with M_GRAB_EXPOSURE + M_TIMERn set to M_ACTIVATE to issue the trigger.
									// M_DEFAULT (M_HARDWARE_PORT0).

		SVGrabExposureTime,			// M_GRAB_EXPOSURE_TIME 
									// Sets the time for the active portion of the exposure signal (that is, the exposure time). 
									// Value: Specifies the time, in nsec. Set this value to 0 to disable the exposure and perform the grab immediately.

		SVGrabExposureTimeDelay,	// M_GRAB_EXPOSURE_TIME_DELAY
									// Sets the delay between the trigger and the start of exposure.
									// Value: Specifies the delay, in nsec. 
	      
		SVGrabExposureTriggerMode,	// M_GRAB_EXPOSURE_TRIGGER_MODE 
									// Sets the trigger activation mode for specified timer.
									// M_EDGE_FALLING: Specifies to trigger upon a high-to-low signal variation.
									// M_EDGE_RISING: Specifies to trigger upon a low-to-high signal variation.
									// M_DEFAULT (M_EDGE_RISING).
		SVGrabExposureClockFrequency,	// M_GRAB_EXPOSURE_CLOCK_FREQUENCY 
										// Returns the frequency (in hertz) of the clock driving the exposure counter.
										// Data type: double
										// Return values: Value  The frequency, in hertz, of the exposure counter clock.  
										// 0  Unable to inquire the value.  
	};
	// define convertor for from/to SVEnum/Matrox types
	typedef SVMatroxEnumConvertor<SVGrabEnum, long> SVGrabEnumMap;
	static SVGrabEnumMap m_GrabEnumConvertor;
*/
/*    
      - Combination constants for M_GRAB_EXPOSURE_XXX inquires are M_TIMER1 
        to M_TIMER8, M_BOARD, and M_CAMERA. If nothing is specified, M_BOARD 
        is implied.
                
      - Combination constants for M_USER_BIT_XXX inquires are user-bit numbers 
        0 to 17.    
*/    

	// User Bits (I/O)
	enum SVUserBitEnum
	{
		SVUserBitCount,				// M_USER_BIT_COUNT 
									// Returns the total number of user-defined signals.
	      
		SVUserBitCountIn,			// M_USER_BIT_COUNT_IN 
									// Returns the number of user-defined signals that can be used for input.
	      
		SVUserBitCountOut,			// M_USER_BIT_COUNT_OUT 
									// Returns the number of user-defined signals that can be used for output.
	      
		SVUserBitInterruptMode,		// M_USER_BIT_INTERRUPT_MODE 
									// Returns the type of functional-state change upon which to generate an interrupt. 
									// Note that this only applies to user-defined input signals.
	        
		SVUserBitInterruptState,	// M_USER_BIT_INTERRUPT_STATE 
									// Returns whether to generate an interrupt when the functional state for the user-defined signal changes. 
									// Note that this only applies to user-defined input signals.
	              
		SVUserBitMode,				// M_USER_BIT_MODE 
									// Returns the current mode of the specified I/O user-defined signal(s). 
	      
		SVUserBitValue,				// M_USER_BIT_VALUE 
									// Returns the current state of the specified user-defined signal(s). 
	      
		SVUserBitSource,			// M_USER_BIT_SOURCE 
									// Sets the type of signal to be routed to a user-defined output signal or I/O signal set to output.
	};
	// define convertor for from/to SVEnum/Matrox types
	typedef SVMatroxEnumConvertor< SVUserBitEnum, SVMatroxInt > SVUserBitEnumMap;
	static SVUserBitEnumMap m_UserBitEnumConvertor;
};

