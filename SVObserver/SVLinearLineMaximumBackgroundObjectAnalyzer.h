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

#include "SVLinearAnalyzerClass.h"
#include "SVValueObjectImpl.h"

class SVLinearMaximumBackgroundObjectLineAnalyzerClass : public SVLinearAnalyzerClass
{
	SV_DECLARE_CLASS( SVLinearMaximumBackgroundObjectLineAnalyzerClass );

public:
	SVLinearMaximumBackgroundObjectLineAnalyzerClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = NULL , int StringResourceID = IDS_CLASSNAME_SVLINEARMAXIMUMBACKGROUNDOBJECTLINEANALYZER );
	virtual ~SVLinearMaximumBackgroundObjectLineAnalyzerClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual BOOL CloseObject();

	virtual BOOL OnValidate();

	virtual HRESULT GetSelectedEdgeOverlays( SVExtentMultiLineStruct &p_MultiLine );

protected:
	virtual BOOL onRun( SVRunStatusClass& RRunStatus );

	SVDPointValueObjectClass mdpEdgeA;	// Point
	SVDPointValueObjectClass mdpEdgeB;	// Point
	SVDPointValueObjectClass mdpCenter;	// Point
	SVDoubleValueObjectClass mdWidth;	// double

	SVDoubleValueObjectClass m_svLinearDistanceA;
	SVDoubleValueObjectClass m_svLinearDistanceB;

private:
	void init();

};

