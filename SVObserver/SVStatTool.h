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

#pragma region Includes
#include "SVTool.h" 
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

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
	SVStatisticsToolClass( BOOL BCreateDefaultTaskList=FALSE, SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVSTATISTICSTOOL );
	virtual ~SVStatisticsToolClass();

	virtual BOOL CreateObject(SVObjectLevelCreateStruct* PCreateStruct ) override;
	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;
	
	BOOL HasVariable() const;

	SVString GetFeatureString();
	SVString GetFeatureName( int aIndex );

	SVString GetOccurenceTestValue();
	void SetOccurenceTestValue( const SVString& rValue );
	
	SVObjectReference GetVariableSelected() const;
	void SetVariableSelected( const SVString& rName );
	void SetVariableSelected( SVObjectReference refObject );

	DWORD EnableFeature( SVStatisticsFeatureEnum aIndex );
	DWORD DisableFeature( SVStatisticsFeatureEnum aIndex );

	DWORD AllocateResult( SVStatisticsFeatureEnum aFeatureIndex );
	DWORD FreeResult( SVStatisticsFeatureEnum aFeatureIndex );
	SVResultClass* GetResultObject( SVStatisticsFeatureEnum aFeatureIndex );

	void UpdateTaskObjectOutputListAttributes();
	virtual bool DisconnectObjectInput( SVInObjectInfoStruct* pObjectInInfo ) override;

protected:
	double getInputValue();
	double getNumberOfSamples();
	double calculateVariance( double aNumberOfSamples, double aAverageValue );

	void init( void );
	void resetValues();
	virtual bool onRun( SVRunStatusClass& RRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

	void RestoreFeatureAttributes();

	bool Test(SvStl::MessageContainerVector *pErrorMessages=nullptr);

private:
	bool ValidateLocal(SvStl::MessageContainerVector *pErrorMessages=nullptr) const;

protected:
	SVDoubleValueObjectClass    msvValue [SV_NUMBER_OF_STAT_FEATURES];
	SVStringValueObjectClass    msvOccurenceValue;
	SVStringValueObjectClass    msvPersistantFeaturesEnabled;
	SVStringValueObjectClass	msvVariableGUID_OBSOLETE;
	SVStringValueObjectClass    msvVariableName;

	DWORD                       msvlDefaultAttributes; 

	double                      msvAccumulatedTotal;
	double                      msvAccumulatedSquares;

	// Input: 
	SVInObjectInfoStruct        m_inputObjectInfo;
};

