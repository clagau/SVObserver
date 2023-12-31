//******************************************************************************
//* COPYRIGHT (c) 2013 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : 
//* .File Name       : $Workfile:   SVLaplacianEdge2FilterClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   09 Aug 2013 09:55:10  $
//******************************************************************************
#pragma once
#pragma region Includes
#include "SVFilterClass.h"
#include "ObjectInterfaces/SVImageBufferHandleInterface.h"
#pragma region Includes

class SVLaplacianEdge2FilterClass : public SVFilterClass
{
	SV_DECLARE_CLASS
public:
	SVLaplacianEdge2FilterClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVLAPLACIANEDGE2FILTER );
	virtual ~SVLaplacianEdge2FilterClass();

protected:
	virtual bool onRun( bool First, SvOi::SVImageBufferHandlePtr RInputImageHandle, SvOi::SVImageBufferHandlePtr ROutputImageHandle, SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;
};

