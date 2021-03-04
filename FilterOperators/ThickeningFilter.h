// ******************************************************************************
// * COPYRIGHT (c) 2004 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVThickeningFilter
// * .File Name       : $Workfile:   SVThickeningFilter.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   13 Aug 2013 10:37:30  $
// ******************************************************************************
#pragma once

#pragma region Includes
#include "SVFilterClass.h"
#include "SVValueObjectLibrary/SVBoolValueObjectClass.h"
#include "SVValueObjectLibrary/SVLongValueObjectClass.h"
#pragma endregion Includes

class ThickeningFilter : public SVFilterClass
{
	SV_DECLARE_CLASS
public:

	ThickeningFilter( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVTHICKENINGFILTER );
	virtual ~ThickeningFilter();
	
#pragma region virtual method (IFilter)
	virtual bool shouldResetInspection() const override { return true; }
#pragma region virtual method (IFilter)

protected:
	virtual bool onRun( bool First, SvOi::SVImageBufferHandlePtr RInputImageHandle, SvOi::SVImageBufferHandlePtr ROutputImageHandle, RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

private:
	void init();

	SvVol::SVLongValueObjectClass m_lvoItterations;
	SvVol::SVBoolValueObjectClass m_bvoGrayOn;
};

