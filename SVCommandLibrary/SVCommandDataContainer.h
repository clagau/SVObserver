//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCommandDataContainer
//* .File Name       : $Workfile:   SVCommandDataContainer.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Apr 2013 18:30:28  $
//******************************************************************************

#ifndef SVCOMMANDDATACONTAINER_H
#define SVCOMMANDDATACONTAINER_H

#include "SVCommandDataHolder.h"
#include "SVCommandDataTemplate.h"
#include "SVCommandLibraryEnums.h"

typedef SVCommandDataTemplate< SV_COMMAND_DATA_TYPE_CONTAINER, SVCommandDataHolder > SVCommandDataContainer;

#endif

