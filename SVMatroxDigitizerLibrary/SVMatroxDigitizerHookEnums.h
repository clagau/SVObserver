//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxDigitizerHookEnums
//* .File Name       : $Workfile:   SVMatroxDigitizerHookEnums.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Oct 2013 10:45:28  $
//******************************************************************************

#pragma once

#include "SVMatroxLibrary/SVMatroxTypedefs.h"
#include "SVMatroxLibrary/SVMatroxEnumConvertor.h"


/**
@SVObjectName Matrox Digitizer Hook Enum

@SVObjectOverview This enum is used to get/set the Hook parameters of the Digitizer.

@SVObjectOperations None

*/

// Hook
struct SVMatroxDigitizerHook
{
	// Hook Type
	enum SVHookTypeEnum
	{
		SVCameraPresent,		// M_CAMERA_PRESENT 
								// Hooks the function to the presence of the camera. Use MdigInquire() with M_CAMERA_PRESENT to inquire if the camera is currently present. 

		SVFieldStart,			// M_FIELD_START	
								// Hooks the function to the event that occurs at the start of each field of the incoming signal. 
								// Note that MIL need not be grabbing the incoming signal for this event to occur. 
								// The event is only dependent on the presence of a camera. 

		SVFieldStartEven,		// M_FIELD_START_EVEN 
								// Hooks the function to the event that occurs at the start of the incoming signal's even fields. 
								// Note that MIL need not be grabbing the incoming signal for this event to occur. 
								// The event is only dependent on the presence of a camera. 

		SVFieldStartOdd,		// M_FIELD_START_ODD 
								// Hooks the function to the event that occurs at the start of the incoming signal's odd fields. 
								// Note that MIL need not be grabbing the incoming signal for this event to occur. 
								// The event is only dependent on the presence of a camera. 

		SVFrameStart,			// M_FRAME_START 
								// Hooks the function to the event that occurs at the start of each frame of the incoming signal. 
								// Note that MIL need not be grabbing the incoming signal for this event to occur. 
								// The event is only dependent on the presence of a camera. 

		SVGrabEnd,				// M_GRAB_END 
								// Hooks the function to the event that occurs at the end of each grab. 
								// When grabbing continuously, the M_GRAB_END event is fired only when the last frame is grabbed. 
								// To be notified at the end of each frame, use the M_GRAB_FRAME_END event. 
								// A function hooked to an M_GRAB_END event is called when all the data of the grab is transferred to the Host. 
								// This means that this hooked function can be called after the M_GRAB_START or M_GRAB_FRAME_START event of the next frame. 

		SVGrabFieldEnd,			// M_GRAB_FIELD_END 
								// Hooks the function to event that occurs at the end of grabbed fields. 
								// Note that, in this case, the grabbed data might not be completely transferred to the Host. 

		SVGrabFieldEndEven,		// M_GRAB_FIELD_END_EVEN 
								// Hooks the function to the event that occurs at the end of grabbed even fields. 
								// Note that, in this case, the grabbed data might not be completely transferred to the Host. 

		SVGrabFieldEndOdd,		// M_GRAB_FIELD_END_ODD 
								// Hooks the function to the event that occurs at the end of grabbed odd fields. 
								// Note that, in this case, the grabbed data might not be completely transferred to the Host. 

		SVGrabFrameEnd,			// M_GRAB_FRAME_END 
								// Hooks the function to the event that occurs at the end of grabbed frames. 
								// Note that, in this case, the grabbed data might not be completely transferred to the Host. 

		SVGrabFrameStart,		// M_GRAB_FRAME_START 
								// Hooks the function to the event that occurs at the start of grabbed frames. 
								// Specifics                                                          
								// M_GRAB_FRAME_START is not supported on Matrox Genesis-LC. 

								// M_GRAB_LINE + n 
								//  Hooks the function to the event that occurs when the specified line (row) is reached, where n is the specified line. For example, M_GRAB_LINE+200 would call the hook-handler function when line number 200 is reached. Note that the data of the specified line number might not be in the buffer. 

								// M_GRAB_LINE_END + n 
								// Hooks the function to the event that occurs when the data of the specified line (row) is in the buffer and ready to be processed, where n is the specified line. For example, M_GRAB_LINE+200 would call the hook-handler function when the data of line number 200 is in the buffer. 

		SVGrabStart,			// M_GRAB_START 
								// Hooks the function to the event that occurs at the start of each grab. 
								// When grabbing continuously, the M_GRAB_START event is fired only when the first frame is grabbed. 
								// To be notified at the start of each frame, use the M_GRAB_FRAME_START event. 

#if SV_DESIRED_MIL_VERSION == 0x0900
		SVUserBitChange,		// M_USER_BIT_CHANGE
								// Hooks the function to the event that occurs when a user-defined input signal changes in accordance with its specified interrupt mode (see M_USER_BIT_INTERRUPT_MODE in MdigControl()). 
								// It is prudent to verify that the appropriate user-defined signal triggered the hook-handler function. 
								// To determine which bit caused the event, call MdigGetHookInfo() from within your hook-handler function. 
#else
		SVIOChange,				// M_IO_CHANGE
								// Hooks the function to the event that occurs when a user-defined input signal changes in accordance with its specified interrupt mode (see M_USER_BIT_INTERRUPT_MODE in MdigControl()). 
								// It is prudent to verify that the appropriate user-defined signal triggered the hook-handler function. 
								// To determine which bit caused the event, call MdigGetHookInfo() from within your hook-handler function. 
#endif

		SVGigeEvent,			// M_GC_EVENT (MIL9)
		SVLineRisingEdgeEvent,	// M_LINE_RISING_EDGE (MIL9)
		SVLineFallingEdgeEvent,	// M_LINE_FALLING_EDGE (MIL9)
		SVLineAnyEdgeEvent,		// M_LINE_ANY_EDGE (MIL9)

								// M_UNHOOK
	};
	// define convertor for from/to SVEnum/Matrox types
	typedef SVMatroxEnumConvertor<SVHookTypeEnum, long> SVHookTypeEnumMap;
	static SVHookTypeEnumMap m_HookTypeEnumConvertor;

	// Hook Info
	enum SVHookInfoEnum
	{
		
		SVUserBit,			// M_USER_BIT 
							// Identifies a user-defined input signal type of event, and returns the number of the user-defined input signal that triggered the event. 
							// This event only occurs if the hook-handler function was hooked using MdigHookFunction() with M_USER_BIT_CHANGE. 
							// If more than one user-defined input signal triggers an interrupt at the same time, the hook-handler function (or chain of hook-handler functions) is called for each user-defined input signal that generated an interrupt. 
							// As a result, any and all user-specified function(s) hooked using MdigHookFunction() with M_USER_BIT_CHANGE will be executed for each interrupt.
	        
		SVUserBitState,		// M_USER_BIT_STATE 
							// Identifies the state (M_ON or M_OFF) of the user-defined input signal that generated the interrupt. 
							// The returned value reflects the state of the user-defined signal at the time of the interrupt. 
							// To retrieve the current state, user MdigInquire() with M_USER_BIT_VALUE.

		SVGigeCameraTimeStamp, // M_GC_CAMERA_TIME_STAMP
		SVGigeEventType, // M_GC_EVENT_TYPE
	};
	// define convertor for from/to SVEnum/Matrox types
	typedef SVMatroxEnumConvertor< SVHookInfoEnum, SVMatroxInt > SVHookInfoEnumMap;
	static SVHookInfoEnumMap m_HookInfoEnumConvertor;
};

