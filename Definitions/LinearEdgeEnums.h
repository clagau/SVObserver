//*****************************************************************************
/// \copyright (c) 2018,2018 by Seidenader Maschinenbau GmbH
/// \file LinearEdgeEnums.h
/// All Rights Reserved
//*****************************************************************************
/// Enums for the Linear Edge Analyzer
//******************************************************************************

#pragma once

#pragma region Includes
#pragma endregion Includes

#pragma region Declarations
#pragma endregion Declarations

namespace SvDef
{
	enum SV_THRESHOLD_SELECTION_ENUM
	{
		SV_USE_SLIDER = 0,
		SV_USE_MAX_MINUS_PERCENT_DIFF = 1,
		SV_USE_MAX_MINUS_OFFSET = 2,
		SV_USE_MIN_PLUS_OFFSET = 3,
	};

	enum SV_EDGECONTROL_POLARISATION_ENUM
	{
		SV_UNDEFINED_POLARISATION = 0x0000,	// 0000 0000 0000 0000
		SV_ANY_POLARISATION = 0x0003,	// 0000 0000 0000 0011
		SV_NEGATIVE_POLARISATION = 0x0002,	// 0000 0000 0000 0010
		SV_POSITIVE_POLARISATION = 0x0001	// 0000 0000 0000 0001
	};// end SV_EDGECONTROL_POLARISATION_ENUM

	  // Add String for SVEnumerateValueObjectClass
	constexpr char* cPolarisationEnums = _T("Any=3,Negative=2,Positive=1");

	enum SV_EDGECONTROL_EDGESELECT_ENUM
	{
		SV_UNDEFINED_EDGE = 0x0000,	// 0000 0000 0000 0000
		SV_FIRST_EDGE = 0x8000,	// 1000 0000 0000 0000
		SV_LAST_EDGE = 0x2000,	// 0010 0000 0000 0000
		SV_THIS_EDGE = 0x1000	// 0001 0000 0000 0000
	};// end SV_EDGECONTROL_EDGESELECT_ENUM

	  // Add String for SVEnumerateValueObjectClass
	constexpr char* cEdgeSelectEnums = _T("First=32768,Last=8192,This=4096");

	enum SV_EDGECONTROL_POSITION_ENUM
	{
		SV_UNDEFINED_POSITION = 0x0000,	// 0000 0000 0000 0000
		SV_START_POSITION = 0x0080,	// 0000 0000 1000 0000
		SV_CENTER_POSITION = 0x0040,	// 0000 0000 0100 0000
		SV_END_POSITION = 0x0020,	// 0000 0000 0010 0000
		SV_OFFSET_POSITION = 0x0010	// 0000 0000 0001 0000
	};// end SV_EDGECONTROL_POSITION_ENUM

	  // Add String for SVEnumerateValueObjectClass
	constexpr char* cPositionEnums = _T("Start=128,Center=64,End=32,Offset=16");

	enum SV_EDGECONTROL_DIRECTION_ENUM
	{
		SV_UNDEFINED_DIRECTION = 0x0000,	// 0000 0000 0000 0000
		SV_TAIL_TO_HEAD_DIRECTION = 0x0200,	// 0000 0010 0000 0000
		SV_HEAD_TO_TAIL_DIRECTION = 0x0100	// 0000 0001 0000 0000
	};// end SV_EDGECONTROL_DIRECTION_ENUM

	  // Add String for SVEnumerateValueObjectClass
	constexpr char* cDirectionEnums = _T("Undefined=0,Tail to Head=512,Head to Tail=256");
} //namespace SvDef
