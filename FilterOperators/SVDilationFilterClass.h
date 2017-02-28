//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : 
//* .File Name       : $Workfile:   SVDilationFilterClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   09 Aug 2013 09:52:22  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVFilterClass.h"
#pragma endregion Includes

class SVDilationFilterClass : public SVFilterClass
{
	SV_DECLARE_CLASS( SVDilationFilterClass );
public:
	SVDilationFilterClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVDILATIONFILTER );
	virtual ~SVDilationFilterClass();

protected:
	virtual bool onRun( bool First, SVSmartHandlePointer RInputImageHandle, SVSmartHandlePointer ROutputImageHandle, SVRunStatusClass& RRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;
};
