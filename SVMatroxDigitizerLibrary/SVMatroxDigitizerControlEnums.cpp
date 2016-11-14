//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxDigitizerControlEnums
//* .File Name       : $Workfile:   SVMatroxDigitizerControlEnums.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Oct 2013 10:45:26  $
//******************************************************************************
#include "stdafx.h"
//Moved to precompiled header: #include <boost/config.hpp>
//Moved to precompiled header: #include <boost/assign/list_of.hpp>
//Moved to precompiled header: #include <mil.h>
#include "SVMatroxDigitizerControlEnums.h"

// Assign mappings for Control Camera Types
SVMatroxDigitizerControl::SVCameraEnumMap SVMatroxDigitizerControl::m_CameraEnumConvertor = boost::assign::map_list_of<>
(SVMatroxDigitizerControl::SVSourceOffsetX,			M_SOURCE_OFFSET_X)
(SVMatroxDigitizerControl::SVSourceOffsetY,			M_SOURCE_OFFSET_Y)
(SVMatroxDigitizerControl::SVSourceSizeX,			M_SOURCE_SIZE_X)
(SVMatroxDigitizerControl::SVSourceSizeY,			M_SOURCE_SIZE_Y);
