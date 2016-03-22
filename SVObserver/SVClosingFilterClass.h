//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : 
//* .File Name       : $Workfile:   SVClosingFilterClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   09 Aug 2013 09:52:00  $
//******************************************************************************

#pragma once

#include "SVFilterClass.h"

class SVClosingFilterClass : public SVFilterClass
{
	SV_DECLARE_CLASS( SVClosingFilterClass );
public:
	SVClosingFilterClass( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVCLOSINGFILTER );
protected:
	virtual BOOL onRun( BOOL First, SVSmartHandlePointer RInputImageHandle, SVSmartHandlePointer ROutputImageHandle, SVRunStatusClass& RRunStatus );
};


