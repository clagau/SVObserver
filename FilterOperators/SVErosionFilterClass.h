//******************************************************************************
//* COPYRIGHT (c) 2013 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : 
//* .File Name       : $Workfile:   SVErosionFilterClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   12 Aug 2013 16:27:28  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVFilterClass.h"
#pragma endregion Includes

class SVErosionFilterClass : public SVFilterClass
{
	SV_DECLARE_CLASS
public:
	SVErosionFilterClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVEROSIONFILTER );
	virtual ~SVErosionFilterClass();

protected:
	virtual bool onRun( bool First, SvOi::SVImageBufferHandlePtr RInputImageHandle, SvOi::SVImageBufferHandlePtr ROutputImageHandle, SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;
};

