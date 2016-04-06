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
#include "SVValueObjectImpl.h"
#pragma endregion Includes

class SVLinearEdgePositionLineAnalyzerClass : public SVLinearAnalyzerClass
{
	SV_DECLARE_CLASS( SVLinearEdgePositionLineAnalyzerClass );

public:
	SVLinearEdgePositionLineAnalyzerClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVLINEAREDGEPOSITIONLINEANALYZER );
	virtual ~SVLinearEdgePositionLineAnalyzerClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual BOOL CloseObject();

	virtual BOOL OnValidate();

protected:
	virtual BOOL onRun( SVRunStatusClass& RRunStatus );

	SVDPointValueObjectClass dpEdge;	// Point
	SVDoubleValueObjectClass m_svLinearDistance;

private:
	void init();
};


