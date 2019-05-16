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

#pragma region Includes
//Moved to precompiled header: #include <vector>
//Moved to precompiled header: #include <utility>
#pragma endregion Includes

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
	typedef std::vector<std::pair<SVBoardEnum, long long>> SVBoardEnumPair;
	static SVBoardEnumPair m_BoardEnumConvertor;
	
	// Camera Format
	enum SVColorModeEnum
	{
		SVComposite,			// M_COMPOSITE  Composite input.  
		SVExternalChrominance,	// M_EXTERNAL_CHROMINANCE  External chrominance input.  
		SVMono8ViaRgb,			// M_MONO8_VIA_RGB  Monochrome 8-bit through RGB input.  
		SVMonochrome,			// M_MONOCHROME  Monochrome input.  
		SVRGB,					// M_RGB  
	};
	typedef std::vector<std::pair<SVColorModeEnum, long long>> SVColorModeEnumPair;
	static SVColorModeEnumPair m_ColorModeEnumConvertor;

	enum SVScanModeEnum
	{
		SVInterlace, 	// M_INTERLACE  Interlace mode  
		SVLinescan, 	// M_LINESCAN  Line-scan mode  
		SVProgressive, 	// M_PROGRESSIVE  Progressive mode  
	};
	typedef std::vector<std::pair<SVScanModeEnum, long long>> SVScanModeEnumPair;
	static SVScanModeEnumPair m_ScanModeEnumConvertor;
	
	// Frame
	enum SVFrameEnum
	{
		SVProcessFrameCount,	// M_PROCESS_FRAME_COUNT
		SVProcessFrameMissed,		// M_PROCESS_FRAME_MISSED
		SVProcessFrameCorrupted,	// M_PROCESS_FRAME_CORRUPTED
		SVProcessFrameRate,			// M_PROCESS_FRAME_RATE
	};
	typedef std::vector<std::pair<SVFrameEnum, long long>> SVFrameEnumPair;
	static SVFrameEnumPair m_FrameEnumConvertor;
};

