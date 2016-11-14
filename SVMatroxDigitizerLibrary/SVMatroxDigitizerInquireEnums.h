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
};

