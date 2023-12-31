//******************************************************************************
//* COPYRIGHT (c) 2003 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : LinearEdgeCountingAnalyzer
//* .File Name       : $Workfile:   LinearEdgeCountingAnalyzer.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 11:59:00  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "LinearAnalyzer.h"
#include "SVValueObjectLibrary/SVLongValueObjectClass.h"
#pragma endregion Includes

namespace SvAo
{

class LinearEdgeCountingAnalyzer : public LinearAnalyzer
{
	SV_DECLARE_CLASS

public:
	LinearEdgeCountingAnalyzer( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_LINEAREDGECOUNTINGANALYZER );
	virtual ~LinearEdgeCountingAnalyzer();

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;
	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	virtual HRESULT GetSelectedEdgeOverlays( SVExtentMultiLineStruct &p_MultiLine ) const override;
	virtual void addOverlayResults(SvPb::Overlay& rOverlay, bool isVertical = false) const override;
	
protected:
	virtual bool onRun( SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

	SvVol::SVLongValueObjectClass	m_svEdgeCount;

private:
	void init();
};

} //namespace SvAo
