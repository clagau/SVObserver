// ******************************************************************************
// * COPYRIGHT (c) 2005 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : LinearEdgePositionAnalyzer
// * .File Name       : $Workfile:   LinearEdgePositionAnalyzer.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   23 Apr 2013 11:59:18  $
// ******************************************************************************

#pragma once

#pragma region Includes
#include "LinearAnalyzer.h"
#include "SVValueObjectLibrary/SVDoubleValueObjectClass.h"
#include "SVValueObjectLibrary/SVDPointValueObjectClass.h"
#pragma endregion Includes

namespace SvAo
{

class LinearEdgePositionAnalyzer : public LinearAnalyzer
{
	SV_DECLARE_CLASS

public:
	LinearEdgePositionAnalyzer( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_LINEAREDGEPOSITIONANALYZER );
	virtual ~LinearEdgePositionAnalyzer();

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;
	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;
	virtual void addOverlayResults(SvPb::Overlay& rOverlay, bool isVertical = false) const override;

protected:
	virtual std::vector<std::string> getParameterNamesForML() const override;

	virtual bool onRun( SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

	SvVol::SVDPointValueObjectClass dpEdge;	// Point
	SvVol::SVDoubleValueObjectClass m_svLinearDistance;

private:
	void init();
};

} //namespace SvAo
