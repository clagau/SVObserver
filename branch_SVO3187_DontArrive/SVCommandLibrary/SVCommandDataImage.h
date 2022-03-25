//******************************************************************************
//* COPYRIGHT (c) 2010 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCommandDataImage
//* .File Name       : $Workfile:   SVCommandDataImage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Apr 2013 18:30:32  $
//******************************************************************************

#pragma once

#include "SVCommandLibraryEnums.h"
#include "SVCommandDataTemplate.h"

typedef SVCommandDataTemplate<SV_COMMAND_DATA_TYPE_IMAGE, std::vector<unsigned char>> SVCommandDataImage;

