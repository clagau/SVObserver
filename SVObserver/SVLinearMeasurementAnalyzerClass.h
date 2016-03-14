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

#ifndef SVLINEARMEASUREMENTANALYZER_H
#define SVLINEARMEASUREMENTANALYZER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SVLinearAnalyzerClass.h"
#include "SVValueObjectImpl.h"

class SVLinearMeasurementAnalyzerClass : public SVLinearAnalyzerClass
{
	SV_DECLARE_CLASS( SVLinearMeasurementAnalyzerClass );

public:
	SVLinearMeasurementAnalyzerClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = NULL , int StringResourceID = IDS_CLASSNAME_SVLINEARMEASUREMENTLINEANALYZER );
	virtual ~SVLinearMeasurementAnalyzerClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual BOOL CloseObject();

	virtual BOOL OnValidate();

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

//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************
#endif	// SVLINELINEARMEASUREMENTANALYZER_H

