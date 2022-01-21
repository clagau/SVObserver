//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVStatisticsTool
//* .File Name       : $Workfile:   SVStatTool.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.7  $
//* .Check In Date   : $Date:   28 Jan 2015 11:09:06  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVStatTool.h"
#include "Definitions/StringTypeDef.h"
#include "ObjectInterfaces/IObjectManager.h"
#include "Operators/SVResultDouble.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVStatusLibrary/MessageManager.h"
#include "InspectionEngine/RunStatus.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

namespace SvTo
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS( SVStatTool, SvPb::StatisticsToolClassId );


SVStatTool::SVStatTool( SVObjectClass* POwner, int StringResourceID )
				:SVToolClass(ToolExtType::None,POwner, StringResourceID )
				, m_NumberOfValidSamples(0)
{
	init();
}

void SVStatTool::init(void)
{
	m_ObjectTypeInfo.m_ObjectType = SvPb::SVToolObjectType;
	m_ObjectTypeInfo.m_SubType    = SvPb::SVStatisticsToolObjectType;
	
	// Register an empty input object
	registerInputObject( &m_inputValue, _T( "StatisticsToolValue" ), SvPb::ResultInputEId);


	// Set up your type...
	//Register embedded objects.
		RegisterEmbeddedObject(
			&m_Value [SV_STATS_MIN_VALUE], 
			SvPb::StatMinEId,
			IDS_OBJECTNAME_STATMIN,
			false, SvOi::SVResetItemNone );
		
		RegisterEmbeddedObject(
			&m_Value [SV_STATS_MAX_VALUE], 
			SvPb::StatMaxEId,
			IDS_OBJECTNAME_STATMAX,
			false, SvOi::SVResetItemNone );
		
		RegisterEmbeddedObject(
			&m_Value [SV_STATS_AVERAGEOF_VALUES], 
			SvPb::StatAverageEId,
			IDS_OBJECTNAME_STATAVERAGE,
			false, SvOi::SVResetItemNone );
		
		RegisterEmbeddedObject(
			&m_Value [SV_STATS_STANDARD_DEVIATION], 
			SvPb::StatStdDevEId,
			IDS_OBJECTNAME_STATSTDDEV,
			false, SvOi::SVResetItemNone );
		
		RegisterEmbeddedObject(
			&m_Value [SV_STATS_VARIANCEIN_VALUES], 
			SvPb::StatVarianceEId,
			IDS_OBJECTNAME_STATVARIANCE,
			false, SvOi::SVResetItemNone );
		
		RegisterEmbeddedObject(
			&m_Value [SV_STATS_NUMBEROF_OCCURANCES], 
			SvPb::StatNbrOfOccurancesEId,
			IDS_OBJECTNAME_STATNBROFOCCURANCES,
			false, SvOi::SVResetItemNone );
		
		RegisterEmbeddedObject(
			&m_Value [SV_STATS_PERCENTOF_OCCURANCES], 
			SvPb::StatPercentOfOccurancesEId,
			IDS_OBJECTNAME_STATPERCENTOFOCCURANCES,
			false, SvOi::SVResetItemNone );
		
		RegisterEmbeddedObject(
			&m_Value[SV_STATS_NUMBEROF_VALIDSAMPLES],
			SvPb::StatNumberOfValidSamplesEId,
			IDS_OBJECTNAME_STAT_NUMBEROF_VALID_SAMPLES,
			false, SvOi::SVResetItemNone);
		
		RegisterEmbeddedObject(
			&m_OccurenceValue, 
			SvPb::StatTestValueEId,
			IDS_OBJECTNAME_STATTESTVALUE,
			false, SvOi::SVResetItemTool );
		
		RegisterEmbeddedObject(
			&m_VariableName, 
			SvPb::StatVariableNameToMonitorEId,
			IDS_OBJECTNAME_STAT_VARIABLE_NAME,
			false, SvOi::SVResetItemTool );
		
		RegisterEmbeddedObject(
			&m_PersistantFeaturesEnabled, 
			SvPb::StatEnabledFeaturesEId,
			IDS_OBJECTNAME_ENABLEDFEATURES,
			false, SvOi::SVResetItemTool );

	// Save default attributes
	m_Value[SV_STATS_MIN_VALUE].SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute);
	m_DefaultAttributes = m_Value [ SV_STATS_MIN_VALUE ].ObjectAttributesAllowed();

	TCHAR szFeaturesEnabled[SV_NUMBER_OF_STAT_FEATURES + 1];

	// Set Embedded defaults
	for (int i = SV_STATS_MIN_VALUE; i < SV_STATS_TOPOF_LIST; i = (SVStatisticsFeatureEnum) (i + 1))
	{
		m_Value [i].SetDefaultValue(0.0, true);
		m_Value [i].SetObjectAttributesAllowed( SvDef::defaultValueObjectAttributes, SvOi::SetAttributeType::RemoveAttribute );
		m_Value[i].setSaveValueFlag(false);

		/*----- FEATURE LIST ---------------------------------------------------------*/
		/*----- The list of enabled features is kept in a string because, of the      */
		/*----- "value objects", the string appeared to contain the least overhead. --*/
		szFeaturesEnabled [i] = _T('0');             // Not enabled.
	}
	
	szFeaturesEnabled [SV_STATS_TOPOF_LIST] = _T( '\0' );    // Null termination.
	
	m_PersistantFeaturesEnabled.SetDefaultValue (szFeaturesEnabled, true);

	m_AccumulatedSquares = 0.0;
	m_AccumulatedTotal = 0.0;
}

SVStatTool::~SVStatTool()
{ 
}

bool SVStatTool::CreateObject(const SVObjectLevelCreateStruct& rCreateStructure )
{
	m_isCreated = SVToolClass::CreateObject(rCreateStructure);

	m_OccurenceValue.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute );
	m_PersistantFeaturesEnabled.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute );
	m_VariableName.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute );

	// These values will not be exposed for the this Tool.
	constexpr UINT cAttribute {SvDef::selectableAttributes | SvPb::audittrail};
	m_drawToolFlag.SetObjectAttributesAllowed(cAttribute, SvOi::SetAttributeType::RemoveAttribute);

	std::string Name;
	m_VariableName.GetValue( Name );

	SetVariableSelected( Name );

	RestoreFeatureAttributes();

	return m_isCreated;
}

bool SVStatTool::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = __super::ResetObject(pErrorMessages);
	
	m_inputValue.validateInput();

	if (Result)
	{
		std::string Name;
		m_VariableName.GetValue( Name );
		SetVariableSelected( Name );

		if( IsEnabled() )
		{
			if( HasVariable() && !Test(pErrorMessages) )
			{
				Result = false;
			}
			else
			{
				resetValues();
			}
		}
	}

	return Result;
}

std::string SVStatTool::GetFeatureString()
{
	std::string FeatureString;
	m_PersistantFeaturesEnabled.GetValue( FeatureString );
	return FeatureString;
}

std::string SVStatTool::GetFeatureName( int aIndex )
{
	return std::string( m_Value[aIndex].GetName() );
}

/////////////////////////////////////////////////////////////////////////////
//
void SVStatTool::EnableFeature (SVStatisticsFeatureEnum aIndex)
{
	m_Value[aIndex].SetObjectAttributesAllowed( m_DefaultAttributes, SvOi::SetAttributeType::OverwriteAttribute );

	std::string FeatureString;
	m_PersistantFeaturesEnabled.GetValue(FeatureString);

	if( aIndex < static_cast<int> (FeatureString.size()) )
	{
		FeatureString[aIndex] =  '1';
	}
	m_PersistantFeaturesEnabled.SetValue(FeatureString);

	// ATTENTION: In error case this method throw an exception (const SvStl::MessageContainer&)
	AllocateResult (aIndex);
}

////////////////////////////////////////////////////
// This function restores feature attributesAllowed
// that were previously stored in the configuration.
////////////////////////////////////////////////////
void SVStatTool::RestoreFeatureAttributes()
{
	std::string FeatureString;
	m_PersistantFeaturesEnabled.GetValue( FeatureString );
	// Ensure that old configurations saved originally with less features are "upgraded" to the number of features known by this program.
	// If this tool version known less features than contained in the configuration (saved with newer SVO), the string remains unchanged, but only known features are used.
	if (FeatureString.size() < SV_STATS_TOPOF_LIST)
	{
		FeatureString.resize(SV_STATS_TOPOF_LIST, '0');
	}
	m_PersistantFeaturesEnabled.SetValue(FeatureString);

	for( int iFeature = SV_STATS_MIN_VALUE ; iFeature < SV_STATS_TOPOF_LIST ; iFeature++ )
	{
		if ('1' == FeatureString[iFeature])
		{
			m_Value[iFeature].SetObjectAttributesAllowed(m_DefaultAttributes, SvOi::SetAttributeType::OverwriteAttribute);
		}
		else
		{
			m_Value[iFeature].SetObjectAttributesAllowed(SvDef::defaultValueObjectAttributes, SvOi::SetAttributeType::RemoveAttribute);
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
//
DWORD SVStatTool::DisableFeature (SVStatisticsFeatureEnum aIndex)
{

	m_Value[aIndex].SetObjectAttributesAllowed( SvDef::defaultValueObjectAttributes, SvOi::SetAttributeType::RemoveAttribute );

	m_Value[aIndex].SetObjectAttributesSet( 0, SvOi::SetAttributeType::OverwriteAttribute );

	std::string FeatureString;
	m_PersistantFeaturesEnabled.GetValue( FeatureString );
	if( aIndex < static_cast<int>  (FeatureString.size()) )
	{
		FeatureString[aIndex] = '0';
	}
	m_PersistantFeaturesEnabled.SetValue( FeatureString);

	FreeResult (aIndex);

	m_Value[aIndex].disconnectAllInputs();

	return 0;
}

void SVStatTool::AllocateResult (SVStatisticsFeatureEnum aFeatureIndex)
{
	SvIe::SVClassInfoStruct       resultClassInfo;
	SvDef::SVObjectTypeInfoStruct  interfaceInfo;

		// Declare Input Interface of Result...
		interfaceInfo.m_EmbeddedID = m_Value [aFeatureIndex].GetEmbeddedID();
		resultClassInfo.m_DesiredInputVector.push_back( interfaceInfo );

		resultClassInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::SVResultObjectType;
		resultClassInfo.m_ObjectTypeInfo.m_SubType	= SvPb::SVResultDoubleObjectType;
		resultClassInfo.m_ClassId = SvPb::DoubleResultClassId;
		resultClassInfo.m_ClassName = SvUl::LoadStdString( IDS_OBJECTNAME_RESULT );
		resultClassInfo.m_ClassName += _T(" ") + std::string(m_Value [aFeatureIndex].GetName());

		// Construct the result class
		SvOp::SVDoubleResult* pResult = dynamic_cast<SvOp::SVDoubleResult*> (resultClassInfo.Construct());

		if( nullptr == pResult)
		{
			SvStl::MessageContainer Exception( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16207, getObjectId() );
			throw Exception;
		}

		Add( pResult );

		SvDef::SVObjectTypeInfoStruct info;
		info.m_ObjectType = SvPb::SVValueObjectType;
		info.m_SubType = SvPb::SVDoubleValueObjectType;
		info.m_EmbeddedID = SvPb::ValueEId;

		// Get the output of the result
		SvVol::SVDoubleValueObjectClass* pValue = dynamic_cast<SvVol::SVDoubleValueObjectClass*>(pResult->getFirstObject(info));

		if(nullptr == pValue)
		{
			SvStl::MessageContainer Exception( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16208, getObjectId() );
			throw Exception;
		}

		// Make it non visible for any selection
		pValue->SetObjectAttributesAllowed( SvDef::defaultValueObjectAttributes, SvOi::SetAttributeType::RemoveAttribute );

		// Ensure this Object's inputs get connected
		pResult->connectAllInputs();

		// And last - Create (initialize) it

		if( ! pResult->IsCreated() )
		{
			// And finally try to create the child object...
			if( !CreateChildObject(pResult) )
			{
				// Remove it from the Blob Analyzer TaskObjectList ( Destruct it )
				uint32_t objectID = pResult->getObjectId();
				if (SvDef::InvalidObjectId != objectID)
				{
					Delete( objectID );
				}
				else
				{
					delete pResult;
				}

				SvStl::MessageContainer Exception( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_StatTool_ResultFailed, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10200, getObjectId() );
				throw Exception;
			}
		}
}

DWORD SVStatTool::FreeResult (SVStatisticsFeatureEnum aFeatureIndex)
{
	DWORD LastError(0);
	
	SvOp::SVResult* pResult = GetResult(aFeatureIndex);
		
	if (nullptr != pResult)
	{
		DestroyChildObject(pResult, SvDef::SVMFSetDefaultInputs);
		pResult = nullptr;
	}
	else
	{
		SvStl::MessageManager MesMan(SvStl::MsgType::Log );
		MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16208);
		LastError = static_cast<DWORD> (-SvStl::Err_16208);
	}
		
	return LastError;
}

SvOp::SVResult* SVStatTool::GetResult(SVStatisticsFeatureEnum aFeatureIndex)
{
	SvOp::SVDoubleResult* pResult{nullptr};

	std::vector<SvOi::IObjectClass*> list;
	fillObjectList(std::back_inserter(list), { SvPb::SVResultObjectType, SvPb::SVResultDoubleObjectType });
	for (const auto pObject : list)
	{
		pResult = dynamic_cast<SvOp::SVDoubleResult*>(pObject);

		if( nullptr != pResult )
		{
			const auto& resultInputList = pResult->GetPrivateInputList( );

			SVObjectClass* pSVObject = resultInputList[0]->GetInputObjectInfo().getObject();
			
			if (&m_Value [aFeatureIndex] == pSVObject)
			{
				break;
			}
		}
	}
		
	return pResult;
}

std::vector<std::string> SVStatTool::getToolAdjustNameList() const
{
	constexpr std::array<LPCTSTR, 3> cToolAdjustNameList
	{
		_T("Parameters"),
		_T("Conditional"),
		_T("Comment"),
	};
	return { cToolAdjustNameList.begin(), cToolAdjustNameList.end() };
}


std::string SVStatTool::GetOccurenceTestValue()
{
	std::string Value;

	m_OccurenceValue.GetValue( Value );
	return Value;
}

void SVStatTool::SetOccurenceTestValue( const std::string& rValue )
{
	m_OccurenceValue.SetValue( rValue);
}

SVObjectReference SVStatTool::GetVariableSelected() const
{
	std::string Name;
	m_VariableName.GetValue( Name );

	SVObjectReference refObject;

	if( !Name.empty() )
	{
		SVObjectManagerClass::Instance().GetObjectByDottedName( Name, refObject );
	}
			
	return refObject;
}

void SVStatTool::SetVariableSelected( const std::string& rName )
{
	if( !rName.empty() )
	{
		// Get the Object
		SVObjectReference refObject;
		/*HRESULT hrGet = */SVObjectManagerClass::Instance().GetObjectByDottedName( rName.c_str(), refObject );
		// Connect to the input
		m_inputValue.SetInputObject( refObject );
		m_VariableName.SetValue(rName);
	}
	else
	{
		m_VariableName.SetValue(std::string());

		// Clear the Object Info
		m_inputValue.SetInputObject( nullptr );
	}
}

bool SVStatTool::getInputValue(double& aValue)
{
	bool Ret{ false };
	aValue = 0.0;
	if (m_inputValue.IsConnected() && nullptr != m_inputValue.GetInputObjectInfo().getObject())
	{
		const SVObjectReference& rObjectRef = m_inputValue.GetInputObjectInfo().GetObjectReference();
		if (S_OK == m_inputValue.GetInputObjectInfo().getObject()->getValue(aValue, rObjectRef.getValidArrayIndex()))
		{
			Ret = true;
		}
		else
		{
			Ret = false;
		}
	}
	else
	{
		Ret = false;
	}

	return Ret;
}

double SVStatTool::getNumberOfSamples()
{
	long count;
	m_EnabledCount.GetValue( count );

	return static_cast<double> (count + 1);
}

// Reset Statistics values
void SVStatTool::resetValues()
{
	m_Value[SV_STATS_MIN_VALUE].SetValue(0.0);
	m_Value[SV_STATS_MAX_VALUE].SetValue(0.0);
	m_Value[SV_STATS_AVERAGEOF_VALUES].SetValue(0.0);
	m_Value[SV_STATS_STANDARD_DEVIATION].SetValue(0.0);
	m_Value[SV_STATS_VARIANCEIN_VALUES].SetValue(0.0);
	m_Value[SV_STATS_NUMBEROF_OCCURANCES].SetValue(0.0);
	m_Value[SV_STATS_PERCENTOF_OCCURANCES].SetValue(0.0);
	m_Value[SV_STATS_NUMBEROF_VALIDSAMPLES].SetValue(0.0);
	m_NumberOfValidSamples = 0;
	m_AccumulatedSquares = 0.0;
	m_AccumulatedTotal = 0.0;
}

double SVStatTool::calculateVariance( double aNumberOfSamples, double aAverageValue )
{
	double averageTimesSamples = aAverageValue * aNumberOfSamples; 
	double averageTimesSamplesSquared = (averageTimesSamples * averageTimesSamples);

	double value = ( 1.0 / ( aNumberOfSamples * ( aNumberOfSamples - 1.0 ) ) ) *
	               ( aNumberOfSamples * m_AccumulatedSquares - ( averageTimesSamplesSquared) );

	return value;
}

bool SVStatTool::HasVariable() const
{
	bool bRetVal = false;
	SVObjectReference refObject = GetVariableSelected();

	if( nullptr != refObject.getObject() )
	{
		// Special Check for BlobAnalyzer/StatTool Features
		// which don't really go away they just change attributes
		if ( refObject.ObjectAttributesAllowed() & SvPb::selectableForStatistics )
		{
			bRetVal = true;
		}
	}

	return bRetVal;
}

bool SVStatTool::onRun(SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = __super::onRun(rRunStatus, pErrorMessages) && ValidateLocal(pErrorMessages);

	if (Result)
	{
		// check which result to calculate
		std::string FeatureString;
		m_PersistantFeaturesEnabled.GetValue(FeatureString);

		// Calculate Average and Number of Occurences always
		// Since they are use in other calculations
		double averageValue;
		double value, count, numberOfSamples;

		numberOfSamples = getNumberOfSamples();
		if (numberOfSamples <= 1)
		{
			resetValues();
		}

		double dInputValue{ 0.0 };
		if (false == getInputValue(dInputValue))
		{
			// just skip the value if there is an error - e.g. invalid value
			rRunStatus.SetPassed();
			return Result;
		}
		
		m_NumberOfValidSamples += 1;
		m_Value[SV_STATS_NUMBEROF_VALIDSAMPLES].SetValue(static_cast<double>(m_NumberOfValidSamples));

		//Compute all statistics based of m_NumberOfValidValues, since numberOfSamples may contain invalid values

		// Calculate Average (Mean)
		value = dInputValue;
		m_AccumulatedTotal += value;
		if (0 < m_NumberOfValidSamples)
		{
			averageValue = m_AccumulatedTotal / m_NumberOfValidSamples;
		}
		else
		{
			averageValue = m_AccumulatedTotal;
		}

		// Accumulate the Squares of the value
		m_AccumulatedSquares += (value * value);

		// Update the Average value
		m_Value[SV_STATS_AVERAGEOF_VALUES].SetValue(averageValue);

		// increment number of matched occurences
		std::string occurenceValueStr;
		m_OccurenceValue.GetValue(occurenceValueStr);
		if (!occurenceValueStr.empty())
		{
			value = atof(occurenceValueStr.c_str());
			if (value == dInputValue)
			{
				m_Value[SV_STATS_NUMBEROF_OCCURANCES].GetValue(count);
				count++;
				m_Value[SV_STATS_NUMBEROF_OCCURANCES].SetValue(count);
			}
		}

		// Set Embedded defaults
		for (int i = SV_STATS_MIN_VALUE; i < SV_STATS_TOPOF_LIST; i = (SVStatisticsFeatureEnum)(i + 1))
		{
			if (FeatureString[i] == '1')
			{
				switch (i)
				{
				case SV_STATS_MIN_VALUE:
					m_Value[SV_STATS_MIN_VALUE].GetValue(value);

					if (m_NumberOfValidSamples > 1)
					{
						value = std::min(value, dInputValue);
					}
					else
					{
						value = dInputValue;
					}

					m_Value[SV_STATS_MIN_VALUE].SetValue(value);
					break;

				case SV_STATS_MAX_VALUE:
					m_Value[SV_STATS_MAX_VALUE].GetValue(value);

					if (m_NumberOfValidSamples > 1)
					{
						value = std::max(value, dInputValue);
					}
					else
					{
						value = dInputValue;
					}

					m_Value[SV_STATS_MAX_VALUE].SetValue(value);
					break;

				case SV_STATS_VARIANCEIN_VALUES:
					// Variance is a cumulative measure of the
					// Squares of the difference of all the data values
					// from the Mean
					// using Sample Variance not Population Variance
					if (m_NumberOfValidSamples > 1)
					{
						value = calculateVariance(m_NumberOfValidSamples, averageValue);

						m_Value[SV_STATS_VARIANCEIN_VALUES].SetValue(value);
					}
					break;

				case SV_STATS_STANDARD_DEVIATION:
					// if variance is already calculated
					if (m_NumberOfValidSamples > 1)
					{
						if (FeatureString[SV_STATS_VARIANCEIN_VALUES] == '1')
						{
							m_Value[SV_STATS_VARIANCEIN_VALUES].GetValue(value);
						}
						else
						{
							value = calculateVariance(m_NumberOfValidSamples, averageValue);
						}
						// Standard Deviation is the positive square root of the Variance
						if (value)
							value = fabs(sqrt(value));

						m_Value[SV_STATS_STANDARD_DEVIATION].SetValue(value);
					}
					break;

				case SV_STATS_PERCENTOF_OCCURANCES:
					if (!occurenceValueStr.empty() && m_NumberOfValidSamples)
					{
						m_Value[SV_STATS_NUMBEROF_OCCURANCES].GetValue(count);

						// Calculate percentile
						value = (count / m_NumberOfValidSamples) * 100.0;

						m_Value[SV_STATS_PERCENTOF_OCCURANCES].SetValue(value);
					}
					break;
				}
			}
		}

		rRunStatus.SetPassed();
	}

	if( ! Result )
	{
		rRunStatus.SetInvalid();
	}

	return Result;
}

bool SVStatTool::Test(SvStl::MessageContainerVector *pErrorMessages)
{
	if( !ValidateLocal(pErrorMessages) )
	{
		return false;
	}

	// verify that the object is really valid
	SVObjectClass* pObject = SVObjectManagerClass::Instance().GetObject(m_inputValue.GetInputObjectInfo().getObjectId() );
	if ( nullptr != pObject )
	{
		SVObjectReference ObjectRef = m_inputValue.GetInputObjectInfo().GetObjectReference();
		if( nullptr != ObjectRef.getObject() )
		{
			double Value;
			HRESULT hr = ObjectRef.getObject()->getValue( Value, ObjectRef.getValidArrayIndex() );
			//Ignore the failures at this point (reset), do not set the tool as invalid
			if ( S_OK == hr || SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE == hr || E_BOUNDS == hr)
			{
				return true;
			}
			else
			{
				if (nullptr != pErrorMessages)
				{
					std::string CompleteName = ObjectRef.getObject()->GetObjectNameToObjectType(SvPb::SVInspectionObjectType);
					SvDef::StringVector msgList;
					msgList.push_back( CompleteName );
					SvStl::MessageContainer message;
					message.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_StatToolInvalidVariable, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10201, getObjectId());
					pErrorMessages->push_back( message );
				}
				return false;
			}
		}
	}

	//Test failed, if reach this point.
	if (nullptr != pErrorMessages)
	{
		SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_StatToolTestFailed, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId() );
		pErrorMessages->push_back(Msg);
	}

	return false;
}

bool SVStatTool::ValidateLocal(SvStl::MessageContainerVector *pErrorMessages) const
{
	if( HasVariable() )
	{
		if ( !m_inputValue.IsConnected() || nullptr == m_inputValue.GetInputObjectInfo().getObject() )
		{
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId() );
				pErrorMessages->push_back(Msg);
			}
			return false;
		}
	}

	return true;
}

} //namespace SvTo
