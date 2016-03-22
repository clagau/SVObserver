//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : 
//* .File Name       : $Workfile:   SVEdgeDetectFilterClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   09 Aug 2013 09:53:14  $
//******************************************************************************

#pragma once

#include "SVFilterClass.h"

class SVEdgeDetectFilterClass : public SVFilterClass
{
	SV_DECLARE_CLASS( SVEdgeDetectFilterClass );
public:
	SVEdgeDetectFilterClass( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVEDGEDETECTFILTER );

protected:
	virtual BOOL onRun( BOOL First, SVSmartHandlePointer RInputImageHandle, SVSmartHandlePointer ROutputImageHandle, SVRunStatusClass& RRunStatus );
};

