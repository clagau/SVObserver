//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : 
//* .File Name       : $Workfile:   SVSharpenFilterClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   12 Aug 2013 16:29:44  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVFilterClass.h"
#pragma endregion Includes

class SVSharpenFilterClass : public SVFilterClass
{
	SV_DECLARE_CLASS( SVSharpenFilterClass );
public:
	SVSharpenFilterClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVSHARPENFILTER );
	virtual ~SVSharpenFilterClass();

protected:
	virtual BOOL onRun( BOOL First, SVSmartHandlePointer RInputImageHandle, SVSmartHandlePointer ROutputImageHandle, SVRunStatusClass& RRunStatus );
};

