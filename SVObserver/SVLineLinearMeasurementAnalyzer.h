//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLineLinearMeasurementAnalyzer
//* .File Name       : $Workfile:   SVLineLinearMeasurementAnalyzer.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:08:36  $
//******************************************************************************

#ifndef SVLINELINEARMEASUREMENTANALYZER_H
#define SVLINELINEARMEASUREMENTANALYZER_H

#include "SVDualLineAnalyzer.h"
#include "SVEdge.h"
#include "SVValueObjectImpl.h"

class SVLinearMeasurementLineAnalyzerClass : public SVDualLineAnalyzerClass
{
	SV_DECLARE_CLASS( SVLinearMeasurementLineAnalyzerClass );

	friend class SVSetupDialogManager;

public:
	SVLinearMeasurementLineAnalyzerClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = NULL , int StringResourceID = IDS_CLASSNAME_SVLINEARMEASUREMENTANALYZER );

	virtual ~SVLinearMeasurementLineAnalyzerClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual BOOL CloseObject();

	virtual BOOL OnValidate();

protected:
	virtual BOOL onRun( SVRunStatusClass& RRunStatus );

	SVPointValueObjectClass center;	// Point
	SVLongValueObjectClass  width;	// Long

	SVDPointValueObjectClass mdpEdgeA;	// Point
	SVDPointValueObjectClass mdpEdgeB;	// Point
	SVDPointValueObjectClass mdpCenter;	// Point
	SVDoubleValueObjectClass mdWidth;	// double

    //
	// A string values to represent all the edgeControl parameters so they can
	// be stored and retrieved from storage.
	//
	SVStringValueObjectClass   StrEdgeControlParameters3;
	SVStringValueObjectClass   StrEdgeControlParameters4;

	// *** // ***
	SVEdgeControlClass	edgeAControl;
	SVEdgeControlClass	edgeBControl;

private:
	void init();

};

#endif

