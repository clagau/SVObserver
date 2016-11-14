//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxDigitizerGRabEnums
//* .File Name       : $Workfile:   SVMatroxDigitizerGrabEnums.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:27:36  $
//******************************************************************************
#include "stdafx.h"
//Moved to precompiled header: #include #include <boost/config.hpp>
//Moved to precompiled header: #include #include <boost/assign/list_of.hpp>
//Moved to precompiled header: #include #include <mil.h>
#include "SVMatroxDigitizerGrabEnums.h"

// Assign mappings for Grab Mode Types
SVMatroxDigitizerGrab::SVGrabModeEnumMap SVMatroxDigitizerGrab::m_GrabModeEnumConvertor = boost::assign::map_list_of<>
(SVMatroxDigitizerGrab::SVGrabAsynchronous,			M_ASYNCHRONOUS)
(SVMatroxDigitizerGrab::SVGrabAsynchronousQueued,	M_ASYNCHRONOUS_QUEUED)
(SVMatroxDigitizerGrab::SVGrabSynchronous,			M_SYNCHRONOUS);

// Assign mappings for Grab Wait Types
SVMatroxDigitizerGrab::SVGrabWaitEnumMap SVMatroxDigitizerGrab::m_GrabWaitEnumConvertor = boost::assign::map_list_of<>
(SVMatroxDigitizerGrab::SVGrabEnd,		M_GRAB_END)
(SVMatroxDigitizerGrab::SVGrabFrameEnd,	M_GRAB_FRAME_END);

// Assign mappings for Grab Trigger Types
SVMatroxDigitizerGrab::SVGrabTriggerEnumMap SVMatroxDigitizerGrab::m_GrabTriggerEnumConvertor = boost::assign::map_list_of<>
(SVMatroxDigitizerGrab::SVGrabTriggerActivate,	M_ACTIVATE)
(SVMatroxDigitizerGrab::SVGrabTriggerDisable,	M_DISABLE)
(SVMatroxDigitizerGrab::SVGrabTriggerEnable,	M_ENABLE);

// Assign mappings for Grab Trigger Mode Types
SVMatroxDigitizerGrab::SVGrabTriggerModeEnumMap SVMatroxDigitizerGrab::m_GrabTriggerModeEnumConvertor = boost::assign::map_list_of<>
(SVMatroxDigitizerGrab::SVGrabTriggerModeDefault,	M_DEFAULT)
(SVMatroxDigitizerGrab::SVEdgeFalling,				M_EDGE_FALLING)
(SVMatroxDigitizerGrab::SVEdgeRising,				M_EDGE_RISING)
(SVMatroxDigitizerGrab::SVLevelHigh,				M_LEVEL_HIGH)
(SVMatroxDigitizerGrab::SVLevelLow,					M_LEVEL_LOW);
