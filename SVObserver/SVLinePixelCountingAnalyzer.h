//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLinePixelCountingAnalyzer
//* .File Name       : $Workfile:   SVLinePixelCountingAnalyzer.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:17:28  $
//******************************************************************************

#ifndef SVLINEPIXELCOUNTINGANALYZER_H
#define SVLINEPIXELCOUNTINGANALYZER_H

#include "SVSingleLineAnalyzer.h"
#include "SVEdge.h"
#include "SVValueObject.h"

class SVPixelCountingLineAnalyzerClass : public SVSingleLineAnalyzerClass
{
	SV_DECLARE_CLASS( SVPixelCountingLineAnalyzerClass );

	friend class SVSetupDialogManager;

public:
	SVPixelCountingLineAnalyzerClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = NULL , int StringResourceID = IDS_CLASSNAME_SVPIXELCOUNTINGLINEANALYZER );

	virtual ~SVPixelCountingLineAnalyzerClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual BOOL CloseObject();
	
	virtual HRESULT GetEdgesOverlay( SVExtentMultiLineStruct &p_MultiLine );
	virtual HRESULT GetSelectedEdgeOverlays( SVExtentMultiLineStruct &p_MultiLine );

protected:
	virtual BOOL onRun( SVRunStatusClass& RRunStatus );

	SVLongValueObjectClass	blackPixelCount;
	SVLongValueObjectClass  whitePixelCount;

	SVVector< DWORD >	pixelHistogramArray;
	
	//
	// A string value to represent all the edgeControl parameters so they can
	// be stored and retrieved from storage.
	//
	SVStringValueObjectClass   StrEdgeControlParameters1;

	// *** // ***
	SVEdgeControlClass	edgeControl;
	BOOL m_bDisplayBWAnalyzeLine;

private:
	void init();

};

#endif

