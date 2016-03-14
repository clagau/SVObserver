//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSingleLineAnalyzer
//* .File Name       : $Workfile:   SVSingleLineAnalyzer.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:07:06  $
//******************************************************************************

//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************

#ifndef SVSINGLELINEANALYZER_H
#define SVSINGLELINEANALYZER_H

//******************************************************************************
//* INCLUDE(S):
//******************************************************************************

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SVLineAnalyzer.h"
#include "SVValueObjectImpl.h"


//******************************************************************************
//* CLASS(ES) & TYPE(S):
//* STRUCTURE(S) & TYPE(S):
//* UNION(S) & TYPE(S):
//******************************************************************************

class SVSingleLineAnalyzerClass : public SVLineAnalyzerClass
{
	SV_DECLARE_CLASS( SVSingleLineAnalyzerClass );

//******************************************************************************
// Constructor(s):
//******************************************************************************
public:
	SVSingleLineAnalyzerClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = NULL , int StringResourceID = IDS_CLASSNAME_SVEDGECOUNTINGLINEANALYZER );

//******************************************************************************
// Destructor(s):
//******************************************************************************
public:
	virtual ~SVSingleLineAnalyzerClass();

//******************************************************************************
// Operator(s):
//******************************************************************************
public:
	BOOL CreateObject(SVObjectLevelCreateStruct *PCreateStructure);

	// Special to restrict outputs for the gage tool
	virtual BOOL SetObjectOwner( SVObjectClass* PNewOwner );
	virtual void DisableThresholdOutputs();

private:
	void init();

//******************************************************************************
// Operation(s) Of Reading Access:
//******************************************************************************
public:
	BOOL CalculateThresholdValues( DWORD& DWUpperThresholdValue, DWORD& DWLowerThresholdValue );
	BOOL GetMinMaxThresholdValues( DWORD& DWMinValue, DWORD& DWMaxValue );

protected:

	void SetCalculatedThresholdPrintableFlags();

//******************************************************************************
// Operation(s) Of Process:
//******************************************************************************
public:
	virtual BOOL OnValidate();

//******************************************************************************
// Data Element(s):
//******************************************************************************
protected:	
	// New threshold flags and variables
	SVBoolValueObjectClass		m_voUseLowerThresholdSelectable;
	SVBoolValueObjectClass		m_voUseLowerThresholdMaxMinusPercentDiff;
	SVBoolValueObjectClass		m_voUseLowerThresholdMaxMinusOffset;
	SVBoolValueObjectClass		m_voUseLowerThresholdMinPlusOffset;
	SVDWordValueObjectClass		m_voLowerThresholdMaxMinusPercentDiff;
	SVDWordValueObjectClass		m_voLowerThresholdMaxMinusOffset;
	SVDWordValueObjectClass		m_voLowerThresholdMinPlusOffset;

	SVBoolValueObjectClass		m_voUseUpperThresholdSelectable;
	SVBoolValueObjectClass		m_voUseUpperThresholdMaxMinusPercentDiff;
	SVBoolValueObjectClass		m_voUseUpperThresholdMaxMinusOffset;
	SVBoolValueObjectClass		m_voUseUpperThresholdMinPlusOffset;
	SVDWordValueObjectClass		m_voUpperThresholdMaxMinusPercentDiff;
	SVDWordValueObjectClass		m_voUpperThresholdMaxMinusOffset;
	SVDWordValueObjectClass		m_voUpperThresholdMinPlusOffset;

	// Input: DWORD - Min Projection Threshold
	SVInObjectInfoStruct		inputMinThresholdObjectInfo;

	// Input: DWORD - Max Projection Threshold
	SVInObjectInfoStruct		inputMaxThresholdObjectInfo;

	//value objects for the edge parameters
	SVEnumerateValueObjectClass	m_voEdgeADirection;
	SVEnumerateValueObjectClass	m_voEdgeAPolarisation;
	SVEnumerateValueObjectClass	m_voEdgeAEdgeSelect;
	SVDoubleValueObjectClass	m_voEdgeAEdgeSelectThisValue;

	SVBoolValueObjectClass		m_voEdgeAIsFixedEdgeMarker;
	SVEnumerateValueObjectClass	m_voEdgeAPosition;
	SVDoubleValueObjectClass	m_voEdgeAPositionOffsetValue;

	SVDWordValueObjectClass		m_voEdgeALowerThresholdValue;
	SVDWordValueObjectClass		m_voEdgeAUpperThresholdValue;
};

//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************
#endif	// SVSINGLELINEANALYZER_H

