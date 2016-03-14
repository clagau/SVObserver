//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLineMaximumObjectAnalyzer
//* .File Name       : $Workfile:   SVLineMaximumObjectAnalyzer.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:08:06  $
//******************************************************************************

#ifndef SVLINEMAXIMUMOBJECTANALYZER_H
#define SVLINEMAXIMUMOBJECTANALYZER_H

#include "SVDualLineAnalyzer.h"
#include "SVEdge.h"
#include "SVValueObjectImpl.h"

class SVMaximumObjectLineAnalyzerClass : public SVDualLineAnalyzerClass
{
	SV_DECLARE_CLASS( SVMaximumObjectLineAnalyzerClass );

	friend class SVSetupDialogManager;

public:
	SVMaximumObjectLineAnalyzerClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = NULL , int StringResourceID = IDS_CLASSNAME_SVMAXIMUMOBJECTLINEANALYZER );

	virtual ~SVMaximumObjectLineAnalyzerClass();

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
	SVStringValueObjectClass   StrEdgeControlParameters9;
	SVStringValueObjectClass   StrEdgeControlParameters10;

	// *** // ***
	SVEdgeControlClass	edgeAControl;
	SVEdgeControlClass	edgeBControl;

private:
	void init();

};

#endif

