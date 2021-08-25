// ******************************************************************************
// * COPYRIGHT (c) 2004 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVSkeletonFilter
// * .File Name       : $Workfile:   SVSkeletonFilter.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   13 Aug 2013 10:32:12  $
// ******************************************************************************

#pragma once

#pragma region Includes
#include "SVFilterClass.h"
#pragma endregion Includes

class SkeletonFilter : public SVFilterClass
{
	SV_DECLARE_CLASS

public:
	SkeletonFilter( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVSKELETONFILTER );
	virtual ~SkeletonFilter();
	
protected:
	virtual bool onRun( bool First, SvOi::SVImageBufferHandlePtr RInputImageHandle, SvOi::SVImageBufferHandlePtr ROutputImageHandle, SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;
};

