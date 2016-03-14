//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObserverEnums
//* .File Name       : $Workfile:   SVObserverEnums.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   10 Jun 2013 16:43:58  $
//******************************************************************************

#ifndef SVOBSERVERENUMS_H
#define SVOBSERVERENUMS_H

#include <deque>

enum
{
	SVDH_MIL_DIGITIZER    = 0x0000FFFF,
	SVDH_SAPERA_DIGITIZER = 0xFFFF0000
};

// Mask descriptor...
enum
{
	SV_NOT_SET = 0x00000000,
	SV_MIL     = 0x00000001,
	SV_SAPERA  = 0x00000002,
	SV_HOST    = 0x00000004
};

enum SVIMProductEnum
{
	SVIM_PRODUCT_INVALID_TYPE = 0,
	SVIM_PRODUCT_FULL       = 1000,
	SVIM_PRODUCT_05         = 1001,
	SVIM_PRODUCT_RGB_MONO   = 1002,
	SVIM_PRODUCT_RGB_COLOR  = 1003,
	SVIM_PRODUCT_D3         = 1004,	// equivalent to the old SVIM_1394_BOARD
	//SVIM_TEST_BOARD       = 1005, // not used
	SVIM_PRODUCT_SVPC       = 1006,
	SVIM_PRODUCT_DIGITAL    = 1007,
	SVIM_PRODUCT_D1         = 1008,
	SVIM_PRODUCT_D2         = 1009,
	SVIM_PRODUCT_D1_COLOR   = 1010,
	SVIM_PRODUCT_D2_COLOR   = 1011,
	SVIM_PRODUCT_D3_COLOR   = 1012,
	SVIM_PRODUCT_D1_HUB     = 1013,
	SVIM_PRODUCT_D2_HUB     = 1014,
	SVIM_PRODUCT_D3_HUB     = 1015,
	SVIM_PRODUCT_X1         = 1016,
	SVIM_PRODUCT_X1_COLOR   = 1017,
	SVIM_PRODUCT_X1_HUB     = 1018,
	SVIM_PRODUCT_X3         = 1019,
	SVIM_PRODUCT_X3_COLOR   = 1020,
	SVIM_PRODUCT_X2			= 1021,
	SVIM_PRODUCT_X2_COLOR	= 1022,
	SVIM_PRODUCT_X2_GD2A	= 1023, // Kontron GIGE Matrox Dongle
	SVIM_PRODUCT_X2_GD2A_COLOR	= 1024, // Kontron GIGE Matrox Dongle Color
	SVIM_PRODUCT_X2_GD4A	= 1025,
	SVIM_PRODUCT_X2_GD4A_COLOR	= 1026,
	SVIM_PRODUCT_X2_GD8A	= 1027,
	SVIM_PRODUCT_X2_GD8A_COLOR	= 1028,
	SVIM_PRODUCT_X2_GD1A	= 1029,
	SVIM_PRODUCT_X2_GD1A_COLOR	= 1030,
	SVIM_PRODUCT_X2_GD8A_NONIO	= 1031,
	SVIM_PRODUCT_X2_GD8A_NONIO_COLOR = 1032,
	SVIM_PRODUCT_TYPE_UNKNOWN = 0xFFFF,
};

typedef std::deque<SVIMProductEnum> SVIMProductEnumList;

#endif

