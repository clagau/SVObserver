//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCommandDataValue
//* .File Name       : $Workfile:   SVCommandDataValue.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Apr 2013 18:31:36  $
//******************************************************************************

#ifndef SVCOMMANDDATAVALUE_H
#define SVCOMMANDDATAVALUE_H

#include <comdef.h>
#include "SVCommandLibraryEnums.h"
#include "SVCommandDataTemplate.h"

typedef SVCommandDataTemplate< SV_COMMAND_DATA_TYPE_VALUE, _variant_t > SVCommandDataValue;

#endif

