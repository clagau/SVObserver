//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : LinearPixelCountingAnalyzer
//* .File Name       : $Workfile:   LinearPixelCountingAnalyzer.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:03:52  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVLinearAnalyzerClass.h"
#include "SVValueObjectLibrary/SVLongValueObjectClass.h"
#pragma endregion Includes

namespace SvAo
{

class LinearPixelCountingAnalyzer : public SVLinearAnalyzerClass
{
	SV_DECLARE_CLASS( LinearPixelCountingAnalyzer );

public:
	LinearPixelCountingAnalyzer( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_LINEARPIXELCOUNTINGANALYZER );
	virtual ~LinearPixelCountingAnalyzer();

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;
	
	virtual HRESULT GetSelectedEdgeOverlays( SVExtentMultiLineStruct &p_MultiLine ) override;

protected:
	virtual bool onRun( RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

	virtual void addOverlayResultDetails(SvPb::Overlay& rOverlay, bool isVertical = false) const override;

	SvVol::SVLongValueObjectClass	blackPixelCount;
	SvVol::SVLongValueObjectClass  whitePixelCount;

private:
	void init();
};

} //namespace SvAo
