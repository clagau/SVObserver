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

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <deque>
#pragma endregion Includes


enum SVIMProductEnum
{
	SVIM_PRODUCT_INVALID_TYPE = 0,
	//SVIM_TEST_BOARD       = 1005, // not used
	SVIM_PRODUCT_SVPC       = 1006,
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
	SVIM_PRODUCT_X2_GD8A_PLC = 1033,
	SVIM_PRODUCT_TYPE_UNKNOWN = 0xFFFF,
};

typedef std::deque<SVIMProductEnum> SVIMProductEnumList;
