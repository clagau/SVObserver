//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLineEdgePositionAnalyzer
//* .File Name       : $Workfile:   SVLineEdgePositionAnalyzer.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:08:16  $
//******************************************************************************

#ifndef SVLINEEDGEPOSITIONANALYZER_H
#define SVLINEEDGEPOSITIONANALYZER_H

#include "SVEdge.h"
#include "SVSingleLineAnalyzer.h"
#include "SVValueObjectImpl.h"


class SVEdgePositionLineAnalyzerClass : public SVSingleLineAnalyzerClass
{
	SV_DECLARE_CLASS( SVEdgePositionLineAnalyzerClass );

	friend class SVSetupDialogManager;

public:
	SVEdgePositionLineAnalyzerClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = NULL , int StringResourceID = IDS_CLASSNAME_SVEDGEPOSITIONLINEANALYZER );

	virtual ~SVEdgePositionLineAnalyzerClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual BOOL CloseObject();

	virtual BOOL OnValidate();

protected:
	virtual BOOL onRun( SVRunStatusClass& RRunStatus );

	SVPointValueObjectClass edge;	// Point
	SVDPointValueObjectClass dpEdge;	// Point

	//
	// A string value to represent all the edgeControl parameters so they can
	// be stored and retrieved from storage.
	//
	SVStringValueObjectClass   StrEdgeControlParameters1;

	// *** // ***
	SVEdgeControlClass	edgeControl;

private:
	void init();

};

//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************
#endif	// SVLINEEDGEPOSITIONANALYZER_H

