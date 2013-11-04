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

#ifndef SVMATROXDIGITIZERGRABENUMS_H
#define SVMATROXDIGITIZERGRABENUMS_H

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

	// Grab
	// Note: this section isn't mapped correctly yet...
	enum SVGrabEnum
	{
		SVGrabContinuousEndTrigger, // M_GRAB_CONTINUOUS_END_TRIGGER
									// Sets whether to automatically generate a trigger after MdigHalt() is issued when performing a triggered continuous grab. 
									// M_DEFAULT  Specifies the default value. 
									// M_ENABLE is the default for a software trigger grab. 
									// Generates the trigger automatically. 
									// With a software triggered continuous grab operation, MdigHalt() will generate a software trigger that invokes the last grab. 
									// With a hardware triggered continuous grab operation, MdigHalt() will invoke a last grab and not wait indefinitely for a hardware trigger. 
									// M_DISABLE is the default for a hardware trigger grab. 
									// Does not generate the trigger automatically. 
									// With a software triggered continuous grab operation, MdigHalt() will wait indefinitely until a software trigger is issued on a separate thread. 
									// With a hardware triggered continuous grab operation, MdigHalt() will wait indefinitely until a hardware trigger is generated before invoking the last grab. 
 
		SVGrabFieldNum,				// M_GRAB_FIELD_NUM
									// Sets the number of fields to grab when grabbing data with MdigGrab(). 
									// This control type should only be used for interlaced video. 
									// When using progressive cameras, set this control type to 1
									// M_DEFAULT  Specifies the default value. 
									// For interlaced cameras, the default is 2. 
									// For progressive cameras, the default is 1. 
									// 1  Specifies to grab one field. The grabbed field is written to sequential rows of the grab buffer. When set to 1, each field is treated like a frame and the following digitizer hook events are aligned with the field: M_GRAB_FRAME_START, M_GRAB_END, and M_GRAB_FRAME_END. To achieve 60 fps in NTSC or 50 fps in PAL, the control type M_GRAB_START_MODE must be set to M_FIELD_START. 
									// 2  Specifies to grab two fields. 
 
		SVGrabScale,				// M_GRAB_SCALE

		SVGrabScaleX,				// M_GRAB_SCALE_X

		SVGrabScaleY,				// M_GRAB_SCALE_Y

		SVGrabStartMode,			// M_GRAB_START_MODE
									// Sets the grab start mode. 
									// M_DEFAULT  Specifies the default mode. 
									// M_FIELD_START  Specifies that the grab start mode starts on any field. 
									// M_FIELD_START_EVEN  Specifies that the grab start mode starts on even. 
									// M_FIELD_START_ODD  Specifies that the grab start mode starts on odd. 
									// Specifies M_FIELD_START_ODD as the default
 
		SVGrabWindowRange,			// M_GRAB_WINDOW_RANGE
		
		SVGrabTriggerFormat,		// M_GRAB_TRIGGER_FORMAT
									// Sets whether to enable the TTL, OPTO, or LVDS receivers for triggers, 
									// on systems whose receivers are enabled through software 
									// and where the option of TTL, OPTO, and LVDS are possible. 
									// Refer to the Technical information appendix in the 
									// Installation and Hardware Reference manual for your Matrox imaging board. 
									// Refer to M_USER_IN_FORMAT to change formats on other Matrox imaging boards. 

									// M_DEFAULT  Same as the one specified by the DCF. 
									// M_DISABLE  Disables the receivers for the specified triggers. For a more conventional technique of disabling a trigger, use MdigControl() with M_GRAB_TRIGGER set to M_DISABLE. 
									// M_ENABLE  Enables the receivers for the specified triggers. For a more conventional technique of enabling a trigger, use MdigControl() with M_GRAB_TRIGGER set to M_ENABLE. 
									// M_LVDS  Enables the LVDS receivers for the specified for triggers. 
									// M_OPTO  Enables the OPTO transmitters/receivers for the specified for triggers. 
									// M_TTL  Enables the TTL transmitters/receivers for the specified for triggers. 
 
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

/*                         
      - Combination constants for M_GRAB_EXPOSURE_XXX controls are: 
        M_TIMER1 to M_TIMER8, M_BOARD, and M_CAMERA. If nothing is specified, 
        M_BOARD is implied.
                
      - Combination constants for M_USER_BIT_XXX controls are user-bit numbers
        0 to 17.
*/    

		SVGrabExposureClockFrequency,	// M_GRAB_EXPOSURE_CLOCK_FREQUENCY 
										// Specifies the input clock frequency of the specified timer, in Hz. 
										// Is valid only if M_GRAB_EXPOSURE_CLOCK_SOURCE is set to M_HARDWARE_PORT0 to 9.
										// Value: Specifies the input clock frequency, in Hz.

		SVGrabExposureArm,			// M_GRAB_EXPOSURE_ARM 
									// Sets the state of the specified exposure timer arm mechanism.
									// M_ENABLE: Enables the exposure timer arm mechanism.
									// M_DISABLE: Disables the exposure timer arm mechanism. 
									// M_ACTIVATE: Activates a software arm for the specified exposure timer. 
									// M_DEFAULT (M_DISABLE).

		SVGrabExposureArmSource,	// M_GRAB_EXPOSURE_ARM_SOURCE
									// Selects the arm source for the specified exposure timer.
									// M_HARDWARE_PORT0 to 9: Uses the hardware arm input signal connected to port 0 to 9
									// M_TIMER1 to 8: Specifies to use a timer as an arm signal. 
									// Two timer groups exist, 1 to 4 and 5 to 8. A timer of group 1 to 4 cannot be 
									// used as an arm signal for a timer of group 5 to 8, and vice versa.
									// M_SOFTWARE: Uses a software arm. 
									// In this mode, the driver automatically arms the timers when a grab is queued. 
									// To force an arm use MdigControl() with M_GRAB_EXPOSURE_ARM + M_TIMERn set to M_ACTIVATE.
									// M_DEFAULT (M_HARDWARE_PORT0).
               
		SVGrabExposureArmMode,		// M_GRAB_EXPOSURE_ARM_MODE 
									// Sets the trigger arm activation mode for the specified timer.
									// M_EDGE_RISING: Specifies to arm timer upon a low-to-high signal
									// M_EDGE_FALLING: Specifies to arm timer upon a high-to-low signal
									// M_DEFAULT (M_EDGE_RISING)
	};
	// define convertor for from/to SVEnum/Matrox types
	typedef SVMatroxEnumConvertor<SVGrabEnum, long> SVGrabEnumMap;
	static SVGrabEnumMap m_GrabEnumConvertor;
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxDigitizerLibrary\SVMatroxDigitizerGrabEnums.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 14:27:42   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   29 Jul 2009 16:12:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branced changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   02 Jun 2009 15:30:48   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/