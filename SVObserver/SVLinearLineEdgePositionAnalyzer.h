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
	SVLinearEdgePositionLineAnalyzerClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVLINEAREDGEPOSITIONLINEANALYZER );
	virtual ~SVLinearEdgePositionLineAnalyzerClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure ) override;
	virtual BOOL CloseObject() override;

	virtual BOOL OnValidate() override;

protected:
	virtual BOOL onRun( SVRunStatusClass& RRunStatus ) override;

	SVDPointValueObjectClass dpEdge;	// Point
	SVDoubleValueObjectClass m_svLinearDistance;

private:
	void init();
};


