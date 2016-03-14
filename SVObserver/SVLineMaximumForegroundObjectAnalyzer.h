//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLineMaximumForegroundObjectAnalyzer
//* .File Name       : $Workfile:   SVLineMaximumForegroundObjectAnalyzer.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:09:16  $
//******************************************************************************

#ifndef SVLINEMAXIMUMFOREGROUNDOBJECTANALYZER_H
#define SVLINEMAXIMUMFOREGROUNDOBJECTANALYZER_H

#include "SVDualLineAnalyzer.h"
#include "SVEdge.h"
#include "SVValueObjectImpl.h"

class SVMaximumForegroundObjectLineAnalyzerClass : public SVDualLineAnalyzerClass
{
	SV_DECLARE_CLASS( SVMaximumForegroundObjectLineAnalyzerClass );

	friend class SVSetupDialogManager;

public:
	SVMaximumForegroundObjectLineAnalyzerClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = NULL , int StringResourceID = IDS_CLASSNAME_SVMAXIMUMFOREGROUNDOBJECTLINEANALYZER );

	virtual ~SVMaximumForegroundObjectLineAnalyzerClass();

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
	SVStringValueObjectClass   StrEdgeControlParameters7;
	SVStringValueObjectClass   StrEdgeControlParameters8;

	// *** // ***
	SVEdgeControlClass	edgeAControl;
	SVEdgeControlClass	edgeBControl;

private:
	void init();

};

#endif

