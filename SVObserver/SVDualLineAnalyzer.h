//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDualLineAnalyzer
//* .File Name       : $Workfile:   SVDualLineAnalyzer.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:23:16  $
//******************************************************************************

#ifndef SVDUALLINEANALYZER_H
#define SVDUALLINEANALYZER_H

#include "SVLineAnalyzer.h"
#include "SVValueObjectImpl.h"

class SVDualLineAnalyzerClass : public SVLineAnalyzerClass
{
	SV_DECLARE_CLASS( SVDualLineAnalyzerClass );

public:
	SVDualLineAnalyzerClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = NULL , int StringResourceID = IDS_CLASSNAME_SVLINEARMEASUREMENTANALYZER );

	virtual ~SVDualLineAnalyzerClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );

	virtual BOOL OnValidate();

	// Special to restrict outputs for the gage tool
	virtual BOOL SetObjectOwner( SVObjectClass* PNewOwner );
	virtual void DisableThresholdOutputs();

	virtual HRESULT GetEdgesOverlay( SVExtentMultiLineStruct &p_rsvMiltiLine );
	virtual HRESULT GetSelectedEdgeOverlays( SVExtentMultiLineStruct &p_MultiLine );

	BOOL CalculateThresholdValues( DWORD& DWUpperThresholdValueA, DWORD& DWLowerThresholdValueA, DWORD& DWUpperThresholdValueB, DWORD& DWLowerThresholdValueB );
	BOOL GetMinMaxThresholdValues( DWORD& DWMinValue, DWORD& DWMaxValue );

protected:

	void SetCalculatedThresholdPrintableFlags();

	SVPointValueObjectClass edgeA;	// Point
	SVPointValueObjectClass edgeB;	// Point

	// New threshold flags and variables
	// Edge A
	SVBoolValueObjectClass	m_voUseLowerThresholdSelectableA;
	SVBoolValueObjectClass	m_voUseLowerThresholdMaxMinusPercentDiffA;
	SVBoolValueObjectClass	m_voUseLowerThresholdMaxMinusOffsetA;
	SVBoolValueObjectClass	m_voUseLowerThresholdMinPlusOffsetA;
	SVDWordValueObjectClass	m_voLowerThresholdMaxMinusPercentDiffA;
	SVDWordValueObjectClass	m_voLowerThresholdMaxMinusOffsetA;
	SVDWordValueObjectClass	m_voLowerThresholdMinPlusOffsetA;

	SVBoolValueObjectClass	m_voUseUpperThresholdSelectableA;
	SVBoolValueObjectClass	m_voUseUpperThresholdMaxMinusPercentDiffA;
	SVBoolValueObjectClass	m_voUseUpperThresholdMaxMinusOffsetA;
	SVBoolValueObjectClass	m_voUseUpperThresholdMinPlusOffsetA;
	SVDWordValueObjectClass	m_voUpperThresholdMaxMinusPercentDiffA;
	SVDWordValueObjectClass	m_voUpperThresholdMaxMinusOffsetA;
	SVDWordValueObjectClass	m_voUpperThresholdMinPlusOffsetA;

	// Edge B
	SVBoolValueObjectClass	m_voUseLowerThresholdSelectableB;
	SVBoolValueObjectClass	m_voUseLowerThresholdMaxMinusPercentDiffB;
	SVBoolValueObjectClass	m_voUseLowerThresholdMaxMinusOffsetB;
	SVBoolValueObjectClass	m_voUseLowerThresholdMinPlusOffsetB;
	SVDWordValueObjectClass	m_voLowerThresholdMaxMinusPercentDiffB;
	SVDWordValueObjectClass	m_voLowerThresholdMaxMinusOffsetB;
	SVDWordValueObjectClass	m_voLowerThresholdMinPlusOffsetB;

	SVBoolValueObjectClass	m_voUseUpperThresholdSelectableB;
	SVBoolValueObjectClass	m_voUseUpperThresholdMaxMinusPercentDiffB;
	SVBoolValueObjectClass	m_voUseUpperThresholdMaxMinusOffsetB;
	SVBoolValueObjectClass	m_voUseUpperThresholdMinPlusOffsetB;
	SVDWordValueObjectClass	m_voUpperThresholdMaxMinusPercentDiffB;
	SVDWordValueObjectClass	m_voUpperThresholdMaxMinusOffsetB;
	SVDWordValueObjectClass	m_voUpperThresholdMinPlusOffsetB;

	// Input: DWORD - Min Projection Threshold
	SVInObjectInfoStruct		inputMinThresholdObjectInfo;

	// Input: DWORD - Max Projection Threshold
	SVInObjectInfoStruct		inputMaxThresholdObjectInfo;

	//Value objects for Edge Parameters
	SVEnumerateValueObjectClass	m_voEdgeADirection;
	SVEnumerateValueObjectClass	m_voEdgeAPolarisation;
	SVEnumerateValueObjectClass	m_voEdgeAEdgeSelect;
	SVDoubleValueObjectClass	m_voEdgeAEdgeSelectThisValue;

	SVBoolValueObjectClass		m_voEdgeAIsFixedEdgeMarker;
	SVEnumerateValueObjectClass	m_voEdgeAPosition;
	SVDoubleValueObjectClass	m_voEdgeAPositionOffsetValue;

	SVDWordValueObjectClass		m_voEdgeALowerThresholdValue;
	SVDWordValueObjectClass		m_voEdgeAUpperThresholdValue;

	SVEnumerateValueObjectClass	m_voEdgeBDirection;
	SVEnumerateValueObjectClass	m_voEdgeBPolarisation;
	SVEnumerateValueObjectClass	m_voEdgeBEdgeSelect;
	SVDoubleValueObjectClass	m_voEdgeBEdgeSelectThisValue;

	SVBoolValueObjectClass		m_voEdgeBIsFixedEdgeMarker;
	SVEnumerateValueObjectClass	m_voEdgeBPosition;
	SVDoubleValueObjectClass	m_voEdgeBPositionOffsetValue;

	SVDWordValueObjectClass		m_voEdgeBLowerThresholdValue;
	SVDWordValueObjectClass		m_voEdgeBUpperThresholdValue;

private:
	void init();

};

#endif

