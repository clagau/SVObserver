//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : 
//* .File Name       : $Workfile:   SVLaplacianEdgeFilterClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   09 Aug 2013 09:55:38  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVFilterClass.h"
#pragma endregion Includes

class SVLaplacianEdgeFilterClass : public SVFilterClass
{
	SV_DECLARE_CLASS( SVLaplacianEdgeFilterClass );
public:
	SVLaplacianEdgeFilterClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVLAPLACIANEDGEFILTER );
	virtual ~SVLaplacianEdgeFilterClass();

protected:
	virtual bool onRun( bool First, SVSmartHandlePointer RInputImageHandle, SVSmartHandlePointer ROutputImageHandle, SVRunStatusClass& RRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;
};


