//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxDigitizerHookEnums
//* .File Name       : $Workfile:   SVMatroxDigitizerHookEnums.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Oct 2013 10:45:28  $
//******************************************************************************
#include "stdafx.h"
//Moved to precompiled header: #include <boost/config.hpp>
#include "SVMatroxDigitizerHookEnums.h"
#include "SVMatroxLibrary/SVMatroxImagingLibrary.h" // for SV_CURRENT_MIL_VERSION define

// Assign mappings for Hook Types
SVMatroxDigitizerHook::SVHookTypeEnumPair SVMatroxDigitizerHook::m_HookTypeEnumConvertor
{
	{SVMatroxDigitizerHook::SVCameraPresent,		M_CAMERA_PRESENT},
	{SVMatroxDigitizerHook::SVFieldStart,			M_FIELD_START},
	{SVMatroxDigitizerHook::SVFieldStartEven,		M_FIELD_START_EVEN},
	{SVMatroxDigitizerHook::SVFieldStartOdd,		M_FIELD_START_ODD},
	{SVMatroxDigitizerHook::SVFrameStart,			M_FRAME_START},
	{SVMatroxDigitizerHook::SVGrabEnd,				M_GRAB_END},
	{SVMatroxDigitizerHook::SVGrabFieldEnd,			M_GRAB_FIELD_END},
	{SVMatroxDigitizerHook::SVGrabFieldEndEven,		M_GRAB_FIELD_END_EVEN},
	{SVMatroxDigitizerHook::SVGrabFieldEndOdd,		M_GRAB_FIELD_END_ODD},
	{SVMatroxDigitizerHook::SVGrabFrameEnd,			M_GRAB_FRAME_END},
	{SVMatroxDigitizerHook::SVGrabFrameStart,		M_GRAB_FRAME_START},
	{SVMatroxDigitizerHook::SVGrabStart,			M_GRAB_START},
	{SVMatroxDigitizerHook::SVIOChange,				M_IO_CHANGE},
	{SVMatroxDigitizerHook::SVGigeEvent,			M_GC_EVENT},
	{SVMatroxDigitizerHook::SVLineRisingEdgeEvent,	M_LINE_RISING_EDGE},
	{SVMatroxDigitizerHook::SVLineFallingEdgeEvent,	M_LINE_FALLING_EDGE},
	{SVMatroxDigitizerHook::SVLineAnyEdgeEvent,		M_LINE_ANY_EDGE}
};

// Assign mappings for Hook Info Types
SVMatroxDigitizerHook::SVHookInfoEnumPair SVMatroxDigitizerHook::m_HookInfoEnumConvertor
{
	{SVMatroxDigitizerHook::SVUserBit,			M_USER_BIT},
	{SVMatroxDigitizerHook::SVUserBitState,		M_USER_BIT_STATE},
	{SVMatroxDigitizerHook::SVGigeCameraTimeStamp,	M_GC_CAMERA_TIME_STAMP},
	{SVMatroxDigitizerHook::SVGigeEventType,		M_GC_EVENT_TYPE}
};

