//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : 
//* .File Name       : $Workfile:   SVVertEdgeFilterClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   12 Aug 2013 16:31:40  $
//******************************************************************************

#pragma once

#include "SVFilterClass.h"

class SVVertEdgeFilterClass : public SVFilterClass
{
	SV_DECLARE_CLASS( SVVertEdgeFilterClass );
public:
	SVVertEdgeFilterClass( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVVERTEDGEFILTER );

protected:
	virtual BOOL onRun( BOOL First, SVSmartHandlePointer RInputImageHandle, SVSmartHandlePointer ROutputImageHandle, SVRunStatusClass& RRunStatus );
};

