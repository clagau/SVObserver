//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolLevelCreateStruct
//* .File Name       : $Workfile:   SVToolLevelCreateStruct.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 17:25:34  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVInspectionLevelCreateStruct.h"
#pragma endregion Includes

struct SVToolLevelCreateStruct : public SVInspectionLevelCreateStruct
{
	SVToolLevelCreateStruct();
	SVToolLevelCreateStruct( const SVObjectLevelCreateStruct& ROLCS );

	virtual ~SVToolLevelCreateStruct();

	SVObjectInfoStruct ToolObjectInfo;
};

