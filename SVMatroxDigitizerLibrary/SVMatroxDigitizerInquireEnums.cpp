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
