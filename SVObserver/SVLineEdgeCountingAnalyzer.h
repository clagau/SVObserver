//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLineEdgeCountingAnalyzer
//* .File Name       : $Workfile:   SVLineEdgeCountingAnalyzer.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:04:52  $
//******************************************************************************

#ifndef SVLINEEDGECOUNTINGANALYZER_H
#define SVLINEEDGECOUNTINGANALYZER_H

#include "SVSingleLineAnalyzer.h"
#include "SVEdge.h"
#include "SVValueObject.h"

class SVEdgeCountingLineAnalyzerClass : public SVSingleLineAnalyzerClass
{
	SV_DECLARE_CLASS( SVEdgeCountingLineAnalyzerClass );

	friend class SVSetupDialogManager;

public:
	SVEdgeCountingLineAnalyzerClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = NULL , int StringResourceID = IDS_CLASSNAME_SVEDGECOUNTINGLINEANALYZER );

	virtual ~SVEdgeCountingLineAnalyzerClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual BOOL CloseObject();

	virtual BOOL OnValidate();

	virtual HRESULT GetEdgesOverlay( SVExtentMultiLineStruct &p_rsvMiltiLine );
	virtual HRESULT GetSelectedEdgeOverlays( SVExtentMultiLineStruct &p_MultiLine );

protected:
	virtual BOOL onRun( SVRunStatusClass& RRunStatus );

	SVLongValueObjectClass	edgeCount;

    //
	// A string value to represent all the edgeControl parameters so they can
	// be stored and retrieved from storage.
	//
	SVStringValueObjectClass   StrEdgeControlParameters2;

	// *** // ***
	SVEdgeControlClass	edgeControl;

private:
	void init();

};

#endif

