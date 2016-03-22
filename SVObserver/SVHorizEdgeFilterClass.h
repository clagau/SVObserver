//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : 
//* .File Name       : $Workfile:   SVHorizEdgeFilterClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   09 Aug 2013 09:54:46  $
//******************************************************************************

#pragma once

#include "SVFilterClass.h"

class SVHorizEdgeFilterClass : public SVFilterClass
{
	SV_DECLARE_CLASS( SVHorizEdgeFilterClass );
public:
	SVHorizEdgeFilterClass( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVHORIZEDGEFILTER );

protected:
	virtual BOOL onRun( BOOL First, SVSmartHandlePointer RInputImageHandle, SVSmartHandlePointer ROutputImageHandle, SVRunStatusClass& RRunStatus );
};


