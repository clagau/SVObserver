//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVStatisticsTool
//* .File Name       : $Workfile:   SVStatTool.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 12:40:02  $
//******************************************************************************

#pragma once

#include "SVTool.h"

class SVResultClass;

enum SVStatisticsFeatureEnum
{
    SV_STATS_MIN_VALUE = 0,           // 0
    SV_STATS_MAX_VALUE,               // 1
    SV_STATS_AVERAGEOF_VALUES,        // 2
	SV_STATS_VARIANCEIN_VALUES,       // 3
    SV_STATS_STANDARD_DEVIATION,      // 4
    SV_STATS_NUMBEROF_OCCURANCES,     // 5
    SV_STATS_PERCENTOF_OCCURANCES,    // 6
    SV_STATS_TOPOF_LIST               // 7
};
enum { SV_NUMBER_OF_STAT_FEATURES = SV_STATS_TOPOF_LIST };

////////////////////////////////////////////////////////////////////////////////
// .Title       : Class SVStatisticsToolClass
// -----------------------------------------------------------------------------
// .Description : This class provides the functionality of a Math
//              : processing Tool
// -----------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

class SVStatisticsToolClass : public SVToolClass
{
	SV_DECLARE_CLASS( SVStatisticsToolClass );
public:
	SVStatisticsToolClass( BOOL BCreateDefaultTaskList=FALSE, SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVSTATISTICSTOOL );
	virtual ~SVStatisticsToolClass();

	BOOL CreateObject(SVObjectLevelCreateStruct* PCreateStruct );
	virtual HRESULT ResetObject();
	virtual BOOL    OnValidate();
	
	BOOL HasVariable();
	BOOL Test();

	CString GetFeatureString();
	CString GetFeatureName( int aIndex );

	CString GetOccurenceTestValue();
	void SetOccurenceTestValue( CString value );
	
	SVObjectReference GetVariableSelected();
	void SetVariableSelected( CString strName );
	void SetVariableSelected( SVObjectReference refObject );

	DWORD EnableFeature( SVStatisticsFeatureEnum aIndex );
	DWORD DisableFeature( SVStatisticsFeatureEnum aIndex );

	DWORD AllocateResult( SVStatisticsFeatureEnum aFeatureIndex );
	DWORD FreeResult( SVStatisticsFeatureEnum aFeatureIndex );
	SVResultClass* GetResultObject( SVStatisticsFeatureEnum aFeatureIndex );

	void UpdateTaskObjectOutputListAttributes();

protected:
	double getInputValue();
	double getNumberOfSamples();
	double calculateVariance( double aNumberOfSamples, double aAverageValue );

	virtual void init( void );
	void resetValues();
	virtual BOOL onRun( SVRunStatusClass& RRunStatus );
	virtual DWORD_PTR processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext );

	void RestoreFeatureAttributes();

protected:
	SVDoubleValueObjectClass    msvValue [SV_NUMBER_OF_STAT_FEATURES];
	SVStringValueObjectClass    msvOccurenceValue;
	SVStringValueObjectClass    msvPersistantFeaturesEnabled;
	SVStringValueObjectClass	msvVariableGUID_OBSOLETE;
	SVStringValueObjectClass    msvVariableName;

	DWORD                       msvlDefaultAttributes; 

	CString                     m_errStr;				// for errorEvent

	double                      msvAccumulatedTotal;
	double                      msvAccumulatedSquares;

	// Input: 
	SVInObjectInfoStruct        m_inputObjectInfo;

};

