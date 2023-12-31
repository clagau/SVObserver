//******************************************************************************
//* COPYRIGHT (c) 2013 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : 
//* .File Name       : $Workfile:   SVClosingFilterClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   09 Aug 2013 09:52:00  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVFilterClass.h"
#pragma endregion Includes

class SVClosingFilterClass : public SVFilterClass
{
	SV_DECLARE_CLASS
public:
	SVClosingFilterClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVCLOSINGFILTER );
	virtual ~SVClosingFilterClass();

protected:
	virtual bool onRun( bool First, SvOi::SVImageBufferHandlePtr RInputImageHandle, SvOi::SVImageBufferHandlePtr ROutputImageHandle, SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;
};

