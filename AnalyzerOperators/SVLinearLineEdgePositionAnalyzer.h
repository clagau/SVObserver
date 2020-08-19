// ******************************************************************************
// * COPYRIGHT (c) 2005 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVLinearEdgePositionLineAnalyzer
// * .File Name       : $Workfile:   SVLinearLineEdgePositionAnalyzer.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   23 Apr 2013 11:59:18  $
// ******************************************************************************

#pragma once

#pragma region Includes
#include "SVLinearAnalyzerClass.h"
#include "SVValueObjectLibrary/SVDoubleValueObjectClass.h"
#include "SVValueObjectLibrary/SVDPointValueObjectClass.h"
#pragma endregion Includes

namespace SvAo
{

class SVLinearEdgePositionLineAnalyzer : public SVLinearAnalyzerClass
{
	SV_DECLARE_CLASS( SVLinearEdgePositionLineAnalyzer );

public:
	SVLinearEdgePositionLineAnalyzer( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVLINEAREDGEPOSITIONLINEANALYZER );
	virtual ~SVLinearEdgePositionLineAnalyzer();

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;
	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;
	virtual void addOverlayResults(SvPb::Overlay& rOverlay, bool isVertical = false) const override;

protected:
	virtual std::vector<std::string> getParameterNamesForML() const override;

	virtual bool onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

	SvVol::SVDPointValueObjectClass dpEdge;	// Point
	SvVol::SVDoubleValueObjectClass m_svLinearDistance;

private:
	void init();
};

} //namespace SvAo
