// ******************************************************************************
// * COPYRIGHT (c) 2005 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVLinearEdgePositionLineAnalyzerClass
// * .File Name       : $Workfile:   SVLinearLineEdgePositionAnalyzer.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   23 Apr 2013 11:59:18  $
// ******************************************************************************

#pragma once

#pragma region Includes
#include "SVLinearAnalyzerClass.h"
#pragma endregion Includes

class SVLinearEdgePositionLineAnalyzerClass : public SVLinearAnalyzerClass
{
	SV_DECLARE_CLASS( SVLinearEdgePositionLineAnalyzerClass );

public:
	SVLinearEdgePositionLineAnalyzerClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVLINEAREDGEPOSITIONLINEANALYZER );
	virtual ~SVLinearEdgePositionLineAnalyzerClass();

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;
	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	virtual void addParameterForMonitorList(SvStl::MessageContainerVector& rMessages, std::back_insert_iterator<SvOi::ParametersForML> inserter) const override;

protected:
	virtual bool onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

	SVDPointValueObjectClass dpEdge;	// Point
	SVDoubleValueObjectClass m_svLinearDistance;

private:
	void init();
};


