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

#pragma region Includes
#include "SVFilterClass.h"
#pragma endregion Includes

class SVEdgeDetectFilterClass : public SVFilterClass
{
	SV_DECLARE_CLASS( SVEdgeDetectFilterClass );
public:
	SVEdgeDetectFilterClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVEDGEDETECTFILTER );
	virtual ~SVEdgeDetectFilterClass();

protected:
	virtual bool onRun( bool First, SVSmartHandlePointer RInputImageHandle, SVSmartHandlePointer ROutputImageHandle, SVRunStatusClass& RRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;
};

