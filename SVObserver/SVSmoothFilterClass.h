//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : 
//* .File Name       : $Workfile:   SVSmoothFilterClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   09 Aug 2013 11:09:04  $
//******************************************************************************

#pragma once

#include "SVFilterClass.h"

class SVSmoothFilterClass : public SVFilterClass
{
	SV_DECLARE_CLASS( SVSmoothFilterClass );
public:
	SVSmoothFilterClass( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVSMOOTHFILTER );

protected:
	virtual BOOL onRun( BOOL First, SVSmartHandlePointer RInputImageHandle, SVSmartHandlePointer ROutputImageHandle, SVRunStatusClass& RRunStatus );
};

