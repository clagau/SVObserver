//******************************************************************************
//* COPYRIGHT (c) 2010 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCommandLibraryEnums
//* .File Name       : $Workfile:   SVCommandLibraryEnums.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Apr 2013 18:31:40  $
//******************************************************************************

#pragma once

enum SVCommandType
{
	SV_COMMAND_TYPE_UNKNOWN = 0x00000000,
};

enum SVCommandDataType
{
	SV_COMMAND_DATA_TYPE_UNKNOWN   = 0x00000000,
	SV_COMMAND_DATA_TYPE_READ_ONLY = 0x80000000,
	SV_COMMAND_DATA_TYPE_ITEM_MASK = 0x0000FFFF,
	SV_COMMAND_DATA_TYPE_CONTAINER = 0x00000001,
	SV_COMMAND_DATA_TYPE_VALUE     = 0x00000002,
	SV_COMMAND_DATA_TYPE_BLOCK     = 0x00000004,
	SV_COMMAND_DATA_TYPE_IMAGE     = 0x00000008,
};


