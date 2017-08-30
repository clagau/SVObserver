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
#include "SVOCore/SVTool.h" 
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
	SVStatisticsToolClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVSTATISTICSTOOL );
	virtual ~SVStatisticsToolClass();

	virtual bool CreateObject(SVObjectLevelCreateStruct* pCreateStructure ) override;
	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;
	
	bool HasVariable() const;

	SVString GetFeatureString();
	SVString GetFeatureName( int aIndex );

	SVString GetOccurenceTestValue();
	void SetOccurenceTestValue( const SVString& rValue );
	
	SVObjectReference GetVariableSelected() const;
	void SetVariableSelected( const SVString& rName );
	void SetVariableSelected( SVObjectReference refObject );

	/// Enabled a feature. ATTENTION: In error case this method throw an exception (const SvStl::MessageContainer&)
	/// \param aIndex [in] The index of the feature.
	void EnableFeature( SVStatisticsFeatureEnum aIndex );
	DWORD DisableFeature( SVStatisticsFeatureEnum aIndex );

	/// Allocate a result. ATTENTION: In error case this method throw an exception (const SvStl::MessageContainer&)
	/// \param aFeatureIndex [in] The Index of the feature with should allocated.
	void AllocateResult( SVStatisticsFeatureEnum aFeatureIndex );
	DWORD FreeResult( SVStatisticsFeatureEnum aFeatureIndex );
	SVResultClass* GetResult( SVStatisticsFeatureEnum aFeatureIndex );

	void UpdateTaskObjectOutputListAttributes();
	virtual bool DisconnectObjectInput( SVInObjectInfoStruct* pObjectInInfo ) override;

protected:
	double getInputValue();
	double getNumberOfSamples();
	double calculateVariance( double aNumberOfSamples, double aAverageValue );

	void init( void );
	void resetValues();
	virtual bool onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

	void RestoreFeatureAttributes();

	bool Test(SvStl::MessageContainerVector *pErrorMessages=nullptr);

private:
	bool ValidateLocal(SvStl::MessageContainerVector *pErrorMessages=nullptr) const;

protected:
	SVDoubleValueObjectClass    m_Value[SV_NUMBER_OF_STAT_FEATURES];
	SVStringValueObjectClass    m_OccurenceValue;
	SVStringValueObjectClass    m_PersistantFeaturesEnabled;
	SVStringValueObjectClass	m_VariableGUID_OBSOLETE;
	SVStringValueObjectClass    m_VariableName;

	UINT                        m_DefaultAttributes; 

	double                      m_AccumulatedTotal;
	double                      m_AccumulatedSquares;

	// Input: 
	SVInObjectInfoStruct        m_inputObjectInfo;
};

