//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : 
//* .File Name       : $Workfile:   SVSharpen2FilterClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   12 Aug 2013 16:29:22  $
//******************************************************************************

#ifndef SVSHARPEN2FILTERCLASS_H
#define SVSHARPEN2FILTERCLASS_H

#include "SVFilterClass.h"

class SVSharpen2FilterClass : public SVFilterClass
{
	SV_DECLARE_CLASS( SVSharpen2FilterClass );
public:
	SVSharpen2FilterClass( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVSHARPEN2FILTER );

protected:
	virtual BOOL onRun( BOOL First, SVSmartHandlePointer RInputImageHandle, SVSmartHandlePointer ROutputImageHandle, SVRunStatusClass& RRunStatus );
};

#endif	//	SVSHARPEN2FILTERCLASS_H

