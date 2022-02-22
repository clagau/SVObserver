//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : LinearMeasurementAnalyzer
//* .File Name       : $Workfile:   LinearMeasurementAnalyzer.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:04:12  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "LinearAnalyzer.h"
#include "SVValueObjectLibrary/SVDoubleValueObjectClass.h"
#include "SVValueObjectLibrary/SVDPointValueObjectClass.h"
#pragma endregion Includes

namespace SvAo
{

class LinearMeasurementAnalyzer : public LinearAnalyzer
{
	SV_DECLARE_CLASS

public:
	LinearMeasurementAnalyzer( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVLINEARMEASUREMENTLINEANALYZER );
	virtual ~LinearMeasurementAnalyzer();

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;
	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;
	virtual void addOverlayResults(SvPb::Overlay& rOverlay, bool isVertical = false) const override;

protected:
	virtual bool onRun(SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;
	
	virtual std::vector<std::string> getParameterNamesForML() const override;

	SvVol::SVDPointValueObjectClass mdpEdgeA;	// Point
	SvVol::SVDPointValueObjectClass mdpEdgeB;	// Point
	SvVol::SVDPointValueObjectClass mdpCenter;	// Point
	SvVol::SVDoubleValueObjectClass mdWidth;	// double

	SvVol::SVDoubleValueObjectClass m_svLinearDistanceA;
	SvVol::SVDoubleValueObjectClass m_svLinearDistanceB;

private:
	void init();


};

} //namespace SvAo
