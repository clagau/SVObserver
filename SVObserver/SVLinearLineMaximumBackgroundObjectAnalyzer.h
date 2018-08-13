//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLinearMaximumBackgroundObjectLineAnalyzerClass
//* .File Name       : $Workfile:   SVLinearLineMaximumBackgroundObjectAnalyzer.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 11:59:40  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVLinearAnalyzerClass.h"
#pragma endregion Includes

class SVLinearMaximumBackgroundObjectLineAnalyzerClass : public SVLinearAnalyzerClass
{
	SV_DECLARE_CLASS( SVLinearMaximumBackgroundObjectLineAnalyzerClass );

public:
	SVLinearMaximumBackgroundObjectLineAnalyzerClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVLINEARMAXIMUMBACKGROUNDOBJECTLINEANALYZER );
	virtual ~SVLinearMaximumBackgroundObjectLineAnalyzerClass();

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;
	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	virtual HRESULT GetSelectedEdgeOverlays( SVExtentMultiLineStruct &p_MultiLine ) override;

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

