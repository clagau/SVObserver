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
#include "SVMatroxDigitizerInquireEnums.h"

// Assign mappings for Inquire Board Types
SVMatroxDigitizerInquire::SVBoardEnumPair SVMatroxDigitizerInquire::m_BoardEnumConvertor
{
	{SVMatroxDigitizerInquire::SVChannel,				M_CHANNEL},
	{SVMatroxDigitizerInquire::SVChannelNum,			M_CHANNEL_NUM}
};

// Assign mappings for Inquire Color Mode Types
SVMatroxDigitizerInquire::SVColorModeEnumPair SVMatroxDigitizerInquire::m_ColorModeEnumConvertor
{
	{SVMatroxDigitizerInquire::SVComposite,				M_COMPOSITE},
	{SVMatroxDigitizerInquire::SVExternalChrominance,	M_EXTERNAL_CHROMINANCE},
	{SVMatroxDigitizerInquire::SVMono8ViaRgb,			M_MONO8_VIA_RGB},
	{SVMatroxDigitizerInquire::SVMonochrome,			M_MONOCHROME},
	{SVMatroxDigitizerInquire::SVRGB,					M_RGB}
};

// Assign mappings for Inquire Scan Mode Types
SVMatroxDigitizerInquire::SVScanModeEnumPair SVMatroxDigitizerInquire::m_ScanModeEnumConvertor
{
	{SVMatroxDigitizerInquire::SVInterlace, 	M_INTERLACE},
	{SVMatroxDigitizerInquire::SVLinescan, 		M_LINESCAN},
	{SVMatroxDigitizerInquire::SVProgressive,	M_PROGRESSIVE}
};

// Assign mappings for Inquire Frame Types
SVMatroxDigitizerInquire::SVFrameEnumPair SVMatroxDigitizerInquire::m_FrameEnumConvertor
{
	{SVMatroxDigitizerInquire::SVProcessFrameCount,		M_PROCESS_FRAME_COUNT},
	{SVMatroxDigitizerInquire::SVProcessFrameMissed,	M_PROCESS_FRAME_MISSED},
	{SVMatroxDigitizerInquire::SVProcessFrameCorrupted,	M_PROCESS_FRAME_CORRUPTED},
	{SVMatroxDigitizerInquire::SVProcessFrameRate,		M_PROCESS_FRAME_RATE}
};
