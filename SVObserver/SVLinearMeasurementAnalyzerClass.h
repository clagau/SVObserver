//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLinearMeasurementAnalyzerClass
//* .File Name       : $Workfile:   SVLinearMeasurementAnalyzerClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:04:12  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVLinearAnalyzerClass.h"
#pragma endregion Includes

class SVLinearMeasurementAnalyzerClass : public SVLinearAnalyzerClass
{
	SV_DECLARE_CLASS( SVLinearMeasurementAnalyzerClass );

public:
	SVLinearMeasurementAnalyzerClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVLINEARMEASUREMENTLINEANALYZER );
	virtual ~SVLinearMeasurementAnalyzerClass();

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;
	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	virtual void addParameterForMonitorList(SvStl::MessageContainerVector& rMessages, std::back_insert_iterator<SvOi::ParametersForML> inserter) const override;

protected:
	virtual bool onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

	SVDPointValueObjectClass mdpEdgeA;	// Point
	SVDPointValueObjectClass mdpEdgeB;	// Point
	SVDPointValueObjectClass mdpCenter;	// Point
	SVDoubleValueObjectClass mdWidth;	// double

	SVDoubleValueObjectClass m_svLinearDistanceA;
	SVDoubleValueObjectClass m_svLinearDistanceB;

private:
	void init();
};


