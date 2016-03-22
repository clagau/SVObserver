//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLinearPixelCountingLineAnalyzerClass
//* .File Name       : $Workfile:   SVLinearLinePixelCountingAnalyzer.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:03:52  $
//******************************************************************************

#pragma once

#include "SVLinearAnalyzerClass.h"
#include "SVValueObject.h"

class SVLinearPixelCountingLineAnalyzerClass : public SVLinearAnalyzerClass
{
	SV_DECLARE_CLASS( SVLinearPixelCountingLineAnalyzerClass );

public:
	SVLinearPixelCountingLineAnalyzerClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = NULL , int StringResourceID = IDS_CLASSNAME_SVLINEARPIXELCOUNTINGLINEANALYZER );
	virtual ~SVLinearPixelCountingLineAnalyzerClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual BOOL CloseObject();
	
	virtual HRESULT GetSelectedEdgeOverlays( SVExtentMultiLineStruct &p_MultiLine );

protected:
	virtual BOOL onRun( SVRunStatusClass& RRunStatus );

	SVLongValueObjectClass	blackPixelCount;
	SVLongValueObjectClass  whitePixelCount;

private:
	void init();

};

