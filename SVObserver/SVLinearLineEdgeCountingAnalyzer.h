//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLineEdgeCountingAnalyzer
//* .File Name       : $Workfile:   SVLinearLineEdgeCountingAnalyzer.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 11:59:00  $
//******************************************************************************

#pragma once

#include "SVLinearAnalyzerClass.h"
#include "SVValueObject.h"

class SVLinearEdgeCountingLineAnalyzerClass : public SVLinearAnalyzerClass
{
	SV_DECLARE_CLASS( SVLinearEdgeCountingLineAnalyzerClass );

public:
	SVLinearEdgeCountingLineAnalyzerClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = NULL , int StringResourceID = IDS_CLASSNAME_SVLINEAREDGECOUNTINGLINEANALYZER );
	virtual ~SVLinearEdgeCountingLineAnalyzerClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual BOOL CloseObject();

	virtual BOOL OnValidate();

	virtual HRESULT GetSelectedEdgeOverlays( SVExtentMultiLineStruct &p_MultiLine );

protected:
	virtual BOOL onRun( SVRunStatusClass& RRunStatus );

	SVLongValueObjectClass	m_svEdgeCount;

private:
	void init();

};


