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
#include "SVObjectLibrary/InputObject.h"
#pragma endregion Includes


namespace SvOp
{
class SVResult;
}

namespace SvTo
{

enum SVStatisticsFeatureEnum
{
    SV_STATS_MIN_VALUE = 0,           // 0
    SV_STATS_MAX_VALUE,               // 1
    SV_STATS_AVERAGEOF_VALUES,        // 2
	SV_STATS_VARIANCEIN_VALUES,       // 3
    SV_STATS_STANDARD_DEVIATION,      // 4
    SV_STATS_NUMBEROF_OCCURANCES,     // 5
    SV_STATS_PERCENTOF_OCCURANCES,    // 6
	SV_STATS_NUMBEROF_VALIDSAMPLES,   // 7
    SV_STATS_TOPOF_LIST               // 8
};
enum { SV_NUMBER_OF_STAT_FEATURES = SV_STATS_TOPOF_LIST };

////////////////////////////////////////////////////////////////////////////////
// .Title       : Class SVStatTool
// -----------------------------------------------------------------------------
// .Description : This class provides the functionality of a Math
//              : processing Tool
// -----------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

class SVStatTool : public SVToolClass
{
	SV_DECLARE_CLASS
public:
	SVStatTool( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVSTATISTICSTOOL );
	virtual ~SVStatTool();

	virtual bool CreateObject(const SVObjectLevelCreateStruct& rCreateStructure ) override;
	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;
	
	bool HasVariable() const;

	std::string GetFeatureString();
	std::string GetFeatureName( int aIndex );

	std::string GetOccurenceTestValue();
	void SetOccurenceTestValue( const std::string& rValue );
	
	SVObjectReference GetVariableSelected() const;
	void SetVariableSelected( const std::string& rName );

	/// Enabled a feature. ATTENTION: In error case this method throw an exception (const SvStl::MessageContainer&)
	/// \param aIndex [in] The index of the feature.
	void EnableFeature( SVStatisticsFeatureEnum aIndex );
	DWORD DisableFeature( SVStatisticsFeatureEnum aIndex );

	/// Allocate a result. ATTENTION: In error case this method throw an exception (const SvStl::MessageContainer&)
	/// \param aFeatureIndex [in] The Index of the feature with should allocated.
	void AllocateResult( SVStatisticsFeatureEnum aFeatureIndex );
	DWORD FreeResult( SVStatisticsFeatureEnum aFeatureIndex );
	SvOp::SVResult* GetResult( SVStatisticsFeatureEnum aFeatureIndex );

	virtual bool areAuxExtentsAvailable() const override { return false; };
	virtual std::vector<std::string> getToolAdjustNameList() const override;

protected:
	/// Get the current input value and consider also the return value, i.e. whether the valus is valid.
	/// This function does not curretly distinguish between the error types, since it is not needed! This shall be implemented if really needed.
	/// \param aValue [in/out] The current value to be returned. It is valid only if the function is successfull, i.e. returns true. 
	/// If the function fails, this value is set to 0.0 for convenience.
	/// \returns true if the value is successfully retrieved. 
	bool getInputValue(double& aValue);
	double getNumberOfSamples();
	double calculateVariance( double aNumberOfSamples, double aAverageValue );

	void init( void );
	void resetValues();
	virtual bool onRun( RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;
	virtual bool useOverlayColorTool() const override { return false; };

	void RestoreFeatureAttributes();

	bool Test(SvStl::MessageContainerVector *pErrorMessages=nullptr);

private:
	bool ValidateLocal(SvStl::MessageContainerVector *pErrorMessages=nullptr) const;

protected:
	SvVol::SVDoubleValueObjectClass m_Value[SV_NUMBER_OF_STAT_FEATURES];
	SvVol::SVStringValueObjectClass m_OccurenceValue;
	SvVol::SVStringValueObjectClass m_PersistantFeaturesEnabled;
	SvVol::SVStringValueObjectClass m_VariableName;

	UINT                        m_DefaultAttributes; 

	double                      m_AccumulatedTotal;
	double                      m_AccumulatedSquares;
	long						m_NumberOfValidSamples;
	// Input: 
	SvOl::InputObject m_inputValue;
};

} //namespace SvTo
