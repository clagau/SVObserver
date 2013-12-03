//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxDigitizerControlEnums
//* .File Name       : $Workfile:   SVMatroxDigitizerControlEnums.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Oct 2013 10:45:28  $
//******************************************************************************

#ifndef SVMATROXDIGITIZERCONTROLENUMS_H
#define SVMATROXDIGITIZERCONTROLENUMS_H

#include "SVMatroxLibrary/SVMatroxTypedefs.h"
#include "SVMatroxLibrary/SVMatroxEnumConvertor.h"

/**
@SVObjectName Matrox Digitizer Control Enum

@SVObjectOverview This enum is used to set the parameters of the Digitizer.

@SVObjectOperations None

*/
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
	typedef SVMatroxEnumConvertor<SVCameraEnum, long> SVCameraEnumMap;
	static SVCameraEnumMap m_CameraEnumConvertor;
	
	// User Bits
	enum SVUserBitEnum
	{
		SVUserBitInterruptMode,		// M_USER_BIT_INTERRUPT_MODE 
									// Sets the type of functional-state change upon which to generate an interrupt. 
									// Note that this only applies to user-defined input signals.
									// M_EDGE_RISING: Specifies to generate an interrupt upon a low-to-high signal variation.
									// M_EDGE_FALLING: Specifies to generate an interrupt upon a high-to-low signal variation.
									// M_ANY_EDGE: Specifies to generate an interrupt upon a low-to-high and high-to-low signal variation.
									// M_DEFAULT (M_EDGE_RISING).

		SVUserBitInterruptState,	// M_USER_BIT_INTERRUPT_STATE 
									// Sets whether to generate an interrupt when the functional state of the user-defined signal changes. 
									// Note that this only applies to user-defined input signals.
									// M_ENABLE: Enables the interrupt.
									// M_DISABLE: Disables the interrupt. 
									// M_DEFAULT (M_DISABLE)
         
		SVUserBitMode,				// M_USER_BIT_MODE 
									// Sets the mode of the specified user-defined I/O signal(s). 
									// Note that the 4 I/O signal modes cannot be changed individually. 
									// Changing the mode of one signal will affect all four I/O signals
									// M_INPUT: Specifies that the user-defined signal is for input.
									// M_OUTPUT: Specifies that the user-defined signal is for output.
									// M_DEFAULT (M_INPUT).

		SVUserBitValue,				// M_USER_BIT_VALUE 
									// Sets the state of the current bit on the specified user-defined output signal(s) or I/O signals set to output.
									// M_OFF: Specifies that the current bit is set to off. 
									// M_ON: Specifies that the current bit is set to on.
 
		SVUserBitSource,			// M_USER_BIT_SOURCE 
									// Sets the type of signal to be routed to a user-defined output signal or I/O signal set to output.
									// M_GRAB_EXPOSURE++M_TIMERn: Specifies that an exposure signal will be routed.
									// M_USER_BIT: Specifies that a user-defined output signal will be routed.
									// M_USER_OUT_COMBINE(M_GRAB_EXPOSURE+M_TIMERm, M_AND/M_OR,
									// M_GRAB_EXPOSURE+M_TIMERn): Specifies that a combination of two exposure signals will be routed.
									// M_USER_OUT_COMBINE(M_GRAB_EXPOSURE+M_TIMERn, M_AND/M_OR,
									// M_HARDWARE_PORTx): Specifies that a combination of one exposure and one input signal will be routed.
									// M_USER_OUT_COMBINE(M_GRAB_EXPOSURE+M_TIMERn, M_AND/M_OR,
									// M_USER_BIT): Specifies that a combination of one exposure signal and the user bit will be routed.
									// M_DEFAULT (M_USER_BIT).
	};
	// define convertor for from/to SVEnum/Matrox types
	typedef SVMatroxEnumConvertor< SVUserBitEnum, SVMatroxInt > SVUserBitEnumMap;
	static SVUserBitEnumMap m_UserBitEnumConvertor;
};

struct SVMatroxDigitizerFormat
{
	POINT offset;
	SIZE size;
	long bitDepth;
	long numBands;
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxDigitizerLibrary\SVMatroxDigitizerControlEnums.h_v  $
 * 
 *    Rev 1.1   01 Oct 2013 10:45:28   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 14:27:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   29 Jul 2009 16:12:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branced changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   02 Jun 2009 15:28:58   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/