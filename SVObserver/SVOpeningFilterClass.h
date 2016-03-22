//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : 
//* .File Name       : $Workfile:   SVOpeningFilterClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   12 Aug 2013 16:28:36  $
//******************************************************************************

#pragma once

#include "SVFilterClass.h"

class SVOpeningFilterClass : public SVFilterClass
{
	SV_DECLARE_CLASS( SVOpeningFilterClass );
public:
	SVOpeningFilterClass( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVOPENINGFILTER );
protected:
	virtual BOOL onRun( BOOL First, SVSmartHandlePointer RInputImageHandle, SVSmartHandlePointer ROutputImageHandle, SVRunStatusClass& RRunStatus );
};


