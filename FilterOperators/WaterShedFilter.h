// ******************************************************************************
// * COPYRIGHT (c) 2004 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVWaterShedFilter
// * .File Name       : $Workfile:   SVWaterShedFilter.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   13 Aug 2013 10:32:08  $
// ******************************************************************************

#pragma once

#pragma region Includes
#include "SVFilterClass.h"
#include "SVValueObjectLibrary/SVBoolValueObjectClass.h"
#include "SVValueObjectLibrary/SVLongValueObjectClass.h"
#pragma endregion Includes

class WatershedFilter : public SVFilterClass
{
	SV_DECLARE_CLASS( WatershedFilter );

public:
	WatershedFilter( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVWATERSHEDFILTER );
	virtual ~WatershedFilter();
	
	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

#pragma region virtual method (IFilter)
	virtual bool shouldResetInspection() const override { return true; }
#pragma region virtual method (IFilter)

protected:
	virtual bool onRun( bool First, SvOi::SVImageBufferHandlePtr RInputImageHandle, SvOi::SVImageBufferHandlePtr ROutputImageHandle, RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

private:
	void init();
	bool ValidateLocal( SvStl::MessageContainerVector * pErrorMessages ) const;


private:
	SvOl::SVInObjectInfoStruct m_MarkerImageInfo;
	SvVol::SVBoolValueObjectClass m_bvoUseMarker;
	SvVol::SVLongValueObjectClass m_lvoMinVariation;
	SvVol::SVLongValueObjectClass m_lvoControlFlag;
};

