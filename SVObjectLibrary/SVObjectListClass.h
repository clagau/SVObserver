//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectListClass
//* .File Name       : $Workfile:   SVObjectListClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 16:57:24  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVContainerLibrary/SVVector.h"
#include "SVOMFCLibrary/SVTemplate.h"
#include "SVObjectInfoStruct.h"
#pragma endregion Includes

class SVObjectAttributeClass;
class SVObjectClass;

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVObjectListClass
// -----------------------------------------------------------------------------
// .Description : Contains pointer to SVObjectClass items, but doesn't owns this
//				: SVObjectClass items. That means it doesn't construct
//				: or destruct the items! 
////////////////////////////////////////////////////////////////////////////////
class SVObjectListClass : public SVVector< SVObjectReference >
{
public:
	SVObjectListClass();
	virtual ~SVObjectListClass();
};

