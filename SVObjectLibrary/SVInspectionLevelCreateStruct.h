//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVInspectionLevelCreateStruct
//* .File Name       : $Workfile:   SVInspectionLevelCreateStruct.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 16:37:48  $
//******************************************************************************

#ifndef SVINSPECTIONLEVELCREATESTRUCT_H
#define SVINSPECTIONLEVELCREATESTRUCT_H

#include "SVObjectLevelCreateStruct.h"

struct SVInspectionLevelCreateStruct : public SVObjectLevelCreateStruct
{
	SVInspectionLevelCreateStruct();
	SVInspectionLevelCreateStruct( SVObjectLevelCreateStruct& ROLCS );

	virtual ~SVInspectionLevelCreateStruct();

	SVObjectInfoStruct InspectionObjectInfo;
};

////////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// DevStudio inserts additional declarations immediate in front of the preceding line
////////////////////////////////////////////////////////////////////////////////

#endif

