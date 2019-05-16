//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCommandDataBlock
//* .File Name       : $Workfile:   SVCommandDataBlock.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Apr 2013 18:30:28  $
//******************************************************************************

#pragma once

#include "SVCommandLibraryEnums.h"
#include "SVCommandDataTemplate.h"

typedef SVCommandDataTemplate<SV_COMMAND_DATA_TYPE_BLOCK, std::vector<unsigned char>> SVCommandDataBlock;

