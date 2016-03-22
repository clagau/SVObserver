//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxDigitizerInquireEnums
//* .File Name       : $Workfile:   SVMatroxDigitizerInquireEnums.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Oct 2013 10:45:28  $
//******************************************************************************
#include "stdafx.h"
//Moved to precompiled header: #include <boost/config.hpp>
//Moved to precompiled header: #include <boost/assign/list_of.hpp>
//Moved to precompiled header: #include <mil.h>
#include "SVMatroxDigitizerInquireEnums.h"

// Assign mappings for Inquire Board Types
SVMatroxDigitizerInquire::SVBoardEnumMap SVMatroxDigitizerInquire::m_BoardEnumConvertor = boost::assign::map_list_of<>
(SVMatroxDigitizerInquire::SVChannel,				M_CHANNEL)
(SVMatroxDigitizerInquire::SVChannelNum,			M_CHANNEL_NUM);

// Assign mappings for Inquire Color Mode Types
SVMatroxDigitizerInquire::SVColorModeEnumMap SVMatroxDigitizerInquire::m_ColorModeEnumConvertor = boost::assign::map_list_of<>
(SVMatroxDigitizerInquire::SVComposite,				M_COMPOSITE)
(SVMatroxDigitizerInquire::SVExternalChrominance,	M_EXTERNAL_CHROMINANCE)
(SVMatroxDigitizerInquire::SVMono8ViaRgb,			M_MONO8_VIA_RGB)
(SVMatroxDigitizerInquire::SVMonochrome,			M_MONOCHROME)
(SVMatroxDigitizerInquire::SVRGB,					M_RGB);

// Assign mappings for Inquire Scan Mode Types
SVMatroxDigitizerInquire::SVScanModeEnumMap SVMatroxDigitizerInquire::m_ScanModeEnumConvertor = boost::assign::map_list_of<>
(SVMatroxDigitizerInquire::SVInterlace, 	M_INTERLACE)
(SVMatroxDigitizerInquire::SVLinescan, 		M_LINESCAN)
(SVMatroxDigitizerInquire::SVProgressive,	M_PROGRESSIVE);

// Assign mappings for Inquire Frame Types
SVMatroxDigitizerInquire::SVFrameEnumMap SVMatroxDigitizerInquire::m_FrameEnumConvertor = boost::assign::map_list_of<>
(SVMatroxDigitizerInquire::SVProcessFrameCount,		M_PROCESS_FRAME_COUNT)
(SVMatroxDigitizerInquire::SVProcessFrameMissed,	M_PROCESS_FRAME_MISSED)
(SVMatroxDigitizerInquire::SVProcessFrameCorrupted,	M_PROCESS_FRAME_CORRUPTED)
(SVMatroxDigitizerInquire::SVProcessFrameRate,		M_PROCESS_FRAME_RATE);

/*
// Assign mappings for Inquire Grab Types
SVMatroxDigitizerInquire::SVGrabEnumMap SVMatroxDigitizerInquire::m_GrabEnumConvertor = boost::assign::map_list_of<>
(SVMatroxDigitizerInquire::SVGrabPeriod,					M_GRAB_PERIOD)
(SVMatroxDigitizerInquire::SVGrabContinuousEndTrigger,		M_GRAB_CONTINUOUS_END_TRIGGER)
(SVMatroxDigitizerInquire::SVGrabDirectionX,				M_GRAB_DIRECTION_X)
(SVMatroxDigitizerInquire::SVGrabDirectionY,				M_GRAB_DIRECTION_Y)
(SVMatroxDigitizerInquire::SVGrabFieldNum,					M_GRAB_FIELD_NUM)
(SVMatroxDigitizerInquire::SVGrabMode,						M_GRAB_MODE)
(SVMatroxDigitizerInquire::SVGrabScale,						M_GRAB_SCALE)
(SVMatroxDigitizerInquire::SVGrabScaleX,					M_GRAB_SCALE_X)
(SVMatroxDigitizerInquire::SVGrabScaleY,					M_GRAB_SCALE_Y)
(SVMatroxDigitizerInquire::SVGrabStartMode,					M_GRAB_START_MODE)
(SVMatroxDigitizerInquire::SVGrabTimeout,					M_GRAB_TIMEOUT)
(SVMatroxDigitizerInquire::SVGrabWindowRange,				M_GRAB_WINDOW_RANGE)
(SVMatroxDigitizerInquire::SVGrabTrigger,					M_GRAB_TRIGGER)
(SVMatroxDigitizerInquire::SVGrabTriggerFormat,				M_GRAB_TRIGGER_FORMAT)
(SVMatroxDigitizerInquire::SVGrabTriggerMode,				M_GRAB_TRIGGER_MODE)
(SVMatroxDigitizerInquire::SVGrabTriggerSource,				M_GRAB_TRIGGER_SOURCE)
(SVMatroxDigitizerInquire::SVGrabExposure,					M_GRAB_EXPOSURE)
(SVMatroxDigitizerInquire::SVGrabExposureClockSource,		M_GRAB_EXPOSURE_CLOCK_SOURCE)
(SVMatroxDigitizerInquire::SVGrabExposureFormat,			M_GRAB_EXPOSURE_FORMAT)
(SVMatroxDigitizerInquire::SVGrabExposureMode,				M_GRAB_EXPOSURE_MODE)
(SVMatroxDigitizerInquire::SVGrabExposureSource,			M_GRAB_EXPOSURE_SOURCE)
(SVMatroxDigitizerInquire::SVGrabExposureTime,				M_GRAB_EXPOSURE_TIME)
(SVMatroxDigitizerInquire::SVGrabExposureTimeDelay,			M_GRAB_EXPOSURE_TIME_DELAY)
(SVMatroxDigitizerInquire::SVGrabExposureTriggerMode,		M_GRAB_EXPOSURE_TRIGGER_MODE)
(SVMatroxDigitizerInquire::SVGrabExposureClockFrequency,	M_GRAB_EXPOSURE_CLOCK_FREQUENCY);
*/

// Assign mappings for Inquire UserBit Types
SVMatroxDigitizerInquire::SVUserBitEnumMap SVMatroxDigitizerInquire::m_UserBitEnumConvertor = boost::assign::map_list_of< SVMatroxDigitizerInquire::SVUserBitEnum, SVMatroxInt >
(SVMatroxDigitizerInquire::SVUserBitCount,			M_USER_BIT_COUNT)
(SVMatroxDigitizerInquire::SVUserBitCountIn,		M_USER_BIT_COUNT_IN)
(SVMatroxDigitizerInquire::SVUserBitCountOut,		M_USER_BIT_COUNT_OUT)
(SVMatroxDigitizerInquire::SVUserBitInterruptMode,	M_USER_BIT_INTERRUPT_MODE)
(SVMatroxDigitizerInquire::SVUserBitInterruptState,	M_USER_BIT_INTERRUPT_STATE)
(SVMatroxDigitizerInquire::SVUserBitMode,			M_USER_BIT_MODE)
(SVMatroxDigitizerInquire::SVUserBitValue,			M_USER_BIT_VALUE)
(SVMatroxDigitizerInquire::SVUserBitSource,			M_USER_BIT_SOURCE);

