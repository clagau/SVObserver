//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLinearMaximumObjectLineAnalyzerClass
//* .File Name       : $Workfile:   SVLinearLineMaximumObjectAnalyzer.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 11:58:48  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVLinearAnalyzerClass.h"
#pragma endregion Includes

class SVLinearMaximumObjectLineAnalyzerClass : public SVLinearAnalyzerClass
{
	SV_DECLARE_CLASS( SVLinearMaximumObjectLineAnalyzerClass );

public:
	SVLinearMaximumObjectLineAnalyzerClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVLINEARMAXIMUMOBJECTLINEANALYZER );
	virtual ~SVLinearMaximumObjectLineAnalyzerClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure ) override;
	virtual BOOL CloseObject() override;
	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	virtual HRESULT GetSelectedEdgeOverlays( SVExtentMultiLineStruct &p_MultiLine ) override;

protected:
	virtual bool onRun( SVRunStatusClass& RRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

	SVDPointValueObjectClass mdpEdgeA;	// Point
	SVDPointValueObjectClass mdpEdgeB;	// Point
	SVDPointValueObjectClass mdpCenter;	// Point
	SVDoubleValueObjectClass mdWidth;	// double

	SVDoubleValueObjectClass m_svLinearDistanceA;
	SVDoubleValueObjectClass m_svLinearDistanceB;

private:
	void init();
};


