//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLinearMaximumObjectLineAnalyzerClass
//* .File Name       : $Workfile:   SVLinearLineMaximumObjectAnalyzer.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 11:58:48  $
//******************************************************************************

#ifndef SVLINEARLINEMAXIMUMOBJECTANALYZER_H
#define SVLINEARLINEMAXIMUMOBJECTANALYZER_H

#include "SVLinearAnalyzerClass.h"
#include "SVValueObjectImpl.h"

class SVLinearMaximumObjectLineAnalyzerClass : public SVLinearAnalyzerClass
{
	SV_DECLARE_CLASS( SVLinearMaximumObjectLineAnalyzerClass );

public:
	SVLinearMaximumObjectLineAnalyzerClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = NULL , int StringResourceID = IDS_CLASSNAME_SVLINEARMAXIMUMOBJECTLINEANALYZER );
	virtual ~SVLinearMaximumObjectLineAnalyzerClass();

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

#endif

