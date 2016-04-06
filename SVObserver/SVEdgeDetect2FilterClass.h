//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : 
//* .File Name       : $Workfile:   SVEdgeDetect2FilterClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   12 Aug 2013 16:27:04  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVFilterClass.h"
#pragma endregion Includes

class SVEdgeDetect2FilterClass : public SVFilterClass
{
	SV_DECLARE_CLASS( SVEdgeDetect2FilterClass );
public:
	SVEdgeDetect2FilterClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVEDGEDETECT2FILTER );
	virtual ~SVEdgeDetect2FilterClass();

protected:
	virtual BOOL onRun( BOOL First, SVSmartHandlePointer RInputImageHandle, SVSmartHandlePointer ROutputImageHandle, SVRunStatusClass& RRunStatus );
};

