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
#include "SVObjectLibrary/SVClsids.h"
#include "SVObjectLibrary/SVGetObjectDequeByTypeVisitor.h"
#include "SVObjectLibrary/SVOutputInfoListClass.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVStatusLibrary/SVRunStatus.h"
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

SV_IMPLEMENT_CLASS( SVStatisticsToolClass, SvPb::StatisticsToolClassId );


SVStatisticsToolClass::SVStatisticsToolClass( SVObjectClass* POwner, int StringResourceID )
				:SVToolClass( POwner, StringResourceID )
{
	init();
}

void SVStatisticsToolClass::init(void)
{
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVToolObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType    = SvPb::SVStatisticsToolObjectType;
	
	// Register an empty input object
	m_inputObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_inputObjectInfo, _T( "StatisticsToolValue" ) );

	// Remove Embedded Extents
	removeEmbeddedExtents();

	// Set up your type...
	//Register embedded objects.
		RegisterEmbeddedObject(
			&m_Value [SV_STATS_MIN_VALUE], 
			SVStatMinObjectGuid,
			IDS_OBJECTNAME_STATMIN,
			false, SvOi::SVResetItemNone );
		
		RegisterEmbeddedObject(
			&m_Value [SV_STATS_MAX_VALUE], 
			SVStatMaxObjectGuid,
			IDS_OBJECTNAME_STATMAX,
			false, SvOi::SVResetItemNone );
		
		RegisterEmbeddedObject(
			&m_Value [SV_STATS_AVERAGEOF_VALUES], 
			SVStatAverageObjectGuid,
			IDS_OBJECTNAME_STATAVERAGE,
			false, SvOi::SVResetItemNone );
		
		RegisterEmbeddedObject(
			&m_Value [SV_STATS_STANDARD_DEVIATION], 
			SVStatStdDevObjectGuid,
			IDS_OBJECTNAME_STATSTDDEV,
			false, SvOi::SVResetItemNone );
		
		RegisterEmbeddedObject(
			&m_Value [SV_STATS_VARIANCEIN_VALUES], 
			SVStatVarianceObjectGuid,
			IDS_OBJECTNAME_STATVARIANCE,
			false, SvOi::SVResetItemNone );
		
		RegisterEmbeddedObject(
			&m_Value [SV_STATS_NUMBEROF_OCCURANCES], 
			SVStatNbrOfOccurancesObjectGuid,
			IDS_OBJECTNAME_STATNBROFOCCURANCES,
			false, SvOi::SVResetItemNone );
		
		RegisterEmbeddedObject(
			&m_Value [SV_STATS_PERCENTOF_OCCURANCES], 
			SVStatPercentOfOccurancesObjectGuid,
			IDS_OBJECTNAME_STATPERCENTOFOCCURANCES,
			false, SvOi::SVResetItemNone );
		
		RegisterEmbeddedObject(
			&m_OccurenceValue, 
			SVStatTestValueObjectGuid,
			IDS_OBJECTNAME_STATTESTVALUE,
			false, SvOi::SVResetItemTool );
		
		RegisterEmbeddedObject(
			&m_VariableGUID_OBSOLETE, 
			SVStatVariableToMonitorObjectGuid,
			IDS_OBJECTNAME_VARIABLEGUID,
			false, SvOi::SVResetItemTool );
		
		RegisterEmbeddedObject(
			&m_VariableName, 
			SVStatVariableNameToMonitorObjectGuid,
			IDS_OBJECTNAME_STAT_VARIABLE_NAME,
			false, SvOi::SVResetItemTool );
		
		RegisterEmbeddedObject(
			&m_PersistantFeaturesEnabled, 
			SVStatEnabledFeaturesObjectGuid,
			IDS_OBJECTNAME_ENABLEDFEATURES,
			false, SvOi::SVResetItemTool );

	// Save default attributes
	m_Value[SV_STATS_MIN_VALUE].SetObjectAttributesAllowed(SvPb::printable, SvOi::SetAttributeType::RemoveAttribute);
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

	// Set default inputs and outputs
	addDefaultInputObjects();

}

SVStatisticsToolClass::~SVStatisticsToolClass()
{ 
}

bool SVStatisticsToolClass::CreateObject(const SVObjectLevelCreateStruct& rCreateStructure )
{
	m_isCreated = SVToolClass::CreateObject(rCreateStructure);

	m_OccurenceValue.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::RemoveAttribute );
	m_PersistantFeaturesEnabled.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::RemoveAttribute );
	m_VariableName.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::RemoveAttribute );

	// These values will not be exposed for the this Tool.
	constexpr UINT cAttribute {SvDef::selectableAttributes | SvPb::printable};
	m_drawToolFlag.SetObjectAttributesAllowed(cAttribute, SvOi::SetAttributeType::RemoveAttribute);
	m_VariableGUID_OBSOLETE.SetObjectAttributesAllowed(cAttribute, SvOi::SetAttributeType::RemoveAttribute);

	std::string Name;
	m_VariableName.GetValue( Name );

	if ( Name.empty() )
	{
		// check for backwards compatibility
		std::string GuidString;
		m_VariableGUID_OBSOLETE.GetValue( GuidString );
		if ( !GuidString.empty() )
		{
			SVGUID guid{GuidString};
			IObjectClass* pObject = SvOi::getObject(guid);
			if( nullptr != pObject )
			{
				Name = pObject->GetCompleteName();
			}
		}
	}

	SetVariableSelected( Name );

	RestoreFeatureAttributes();

	return m_isCreated;
}

bool SVStatisticsToolClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = __super::ResetObject(pErrorMessages);
	
	SvOl::ValidateInput(m_inputObjectInfo);

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

std::string SVStatisticsToolClass::GetFeatureString()
{
	std::string FeatureString;
	m_PersistantFeaturesEnabled.GetValue( FeatureString );
	return FeatureString;
}

std::string SVStatisticsToolClass::GetFeatureName( int aIndex )
{
	return std::string( m_Value[aIndex].GetName() );
}

/////////////////////////////////////////////////////////////////////////////
//
void SVStatisticsToolClass::EnableFeature (SVStatisticsFeatureEnum aIndex)
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
void SVStatisticsToolClass::RestoreFeatureAttributes()
{
	std::string FeatureString;
	m_PersistantFeaturesEnabled.GetValue( FeatureString );
	for( int iFeature = SV_STATS_MIN_VALUE ; iFeature < SV_STATS_TOPOF_LIST ; iFeature++ )
	{
		if( '1' == FeatureString[iFeature] )
		{
			m_Value[iFeature].SetObjectAttributesAllowed( m_DefaultAttributes, SvOi::SetAttributeType::OverwriteAttribute );
		}
		else
		{
			m_Value[iFeature].SetObjectAttributesAllowed( SvDef::defaultValueObjectAttributes, SvOi::SetAttributeType::RemoveAttribute );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
//
DWORD SVStatisticsToolClass::DisableFeature (SVStatisticsFeatureEnum aIndex)
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

	// Get this object's outputInfo
	SVOutObjectInfoStruct& valueOutObjectInfo = m_Value [aIndex].GetObjectOutputInfo();

	valueOutObjectInfo.DisconnectAllInputs();

	return 0;
}

void SVStatisticsToolClass::AllocateResult (SVStatisticsFeatureEnum aFeatureIndex)
{
	SvIe::SVClassInfoStruct       resultClassInfo;
	SvDef::SVObjectTypeInfoStruct  interfaceInfo;

		// Declare Input Interface of Result...
		interfaceInfo.EmbeddedID = m_Value [aFeatureIndex].GetEmbeddedID();
		resultClassInfo.m_DesiredInputVector.push_back( interfaceInfo );

		resultClassInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVResultObjectType;
		resultClassInfo.m_ObjectTypeInfo.SubType	= SvPb::SVResultDoubleObjectType;
		resultClassInfo.m_ClassId = SvPb::DoubleResultClassId;
		resultClassInfo.m_ClassName = SvUl::LoadStdString( IDS_OBJECTNAME_RESULT );
		resultClassInfo.m_ClassName += _T(" ") + std::string(m_Value [aFeatureIndex].GetName());

		// Construct the result class
		SvOp::SVDoubleResultClass* pResult = dynamic_cast<SvOp::SVDoubleResultClass*> (resultClassInfo.Construct());

		if( nullptr == pResult)
		{
			SvStl::MessageContainer Exception( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16207, GetUniqueObjectID() );
			throw Exception;
		}

		Add( pResult );

		SvDef::SVObjectTypeInfoStruct info;
		info.ObjectType = SvPb::SVValueObjectType;
		info.SubType = SvPb::SVDoubleValueObjectType;
		info.EmbeddedID = SVValueObjectGuid;

		// Get the output of the result
		SvVol::SVDoubleValueObjectClass* pValue = dynamic_cast<SvVol::SVDoubleValueObjectClass*>(pResult->getFirstObject(info));

		if(nullptr == pValue)
		{
			SvStl::MessageContainer Exception( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16208, GetUniqueObjectID() );
			throw Exception;
		}

		// Make it non visible for any selection
		pValue->SetObjectAttributesAllowed( SvDef::defaultValueObjectAttributes, SvOi::SetAttributeType::RemoveAttribute );

		// Ensure this Object's inputs get connected
		pResult->ConnectAllInputs();

		// And last - Create (initialize) it

		if( ! pResult->IsCreated() )
		{
			// And finally try to create the child object...
			if( !CreateChildObject(pResult) )
			{
				// Remove it from the Blob Analyzer TaskObjectList ( Destruct it )
				GUID objectID = pResult->GetUniqueObjectID();
				if( GUID_NULL != objectID )
				{
					Delete( objectID );
				}
				else
				{
					delete pResult;
				}

				SvStl::MessageContainer Exception( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_StatTool_ResultFailed, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10200, GetUniqueObjectID() );
				throw Exception;
			}
		}
}

DWORD SVStatisticsToolClass::FreeResult (SVStatisticsFeatureEnum aFeatureIndex)
{
	DWORD LastError(0);
	
	SvOp::SVResultClass* pResult = GetResult(aFeatureIndex);
		
	if (nullptr != pResult)
	{
		DestroyChildObject(pResult, SvDef::SVMFSetDefaultInputs);
		pResult = nullptr;
	}
	else
	{
		SvStl::MessageMgrStd MesMan(SvStl::MsgType::Log );
		MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16208);
		LastError = -SvStl::Err_16208;
	}
		
	return LastError;
}

SvOp::SVResultClass* SVStatisticsToolClass::GetResult(SVStatisticsFeatureEnum aFeatureIndex)
{
	SvOl::SVInputInfoListClass	resultInputList;
	SVOutputInfoListClass	resultOutputList;
	
	SvOl::SVInObjectInfoStruct*	pResultInputInfo;
	
	SvDef::SVObjectTypeInfoStruct  info;
	SvOp::SVDoubleResultClass* pResult{nullptr};
	SVObjectClass*          pSVObject;
	
	bool                    bDone = false;
	
	info.ObjectType = SvPb::SVResultObjectType;
	info.SubType = SvPb::SVResultDoubleObjectType;
	
	SVGetObjectDequeByTypeVisitor l_Visitor( info );

	SVObjectManagerClass::Instance().VisitElements( l_Visitor, GetUniqueObjectID() );

	SVGetObjectDequeByTypeVisitor::SVObjectPtrDeque::const_iterator l_Iter;

	for( l_Iter = l_Visitor.GetObjects().begin(); l_Iter != l_Visitor.GetObjects().end() && !bDone; ++l_Iter )
	{
		pResult = dynamic_cast<SvOp::SVDoubleResultClass*>(const_cast< SVObjectClass* > (*l_Iter));

		if( nullptr != pResult )
		{
			pResult->GetPrivateInputList( resultInputList );
			
			pResultInputInfo = resultInputList[0];
			
			pSVObject = pResultInputInfo->GetInputObjectInfo().getObject();
			
			if (&m_Value [aFeatureIndex] == pSVObject)
			{
				bDone = true;
			}
		}
	}
		
	return pResult;
}

std::string SVStatisticsToolClass::GetOccurenceTestValue()
{
	std::string Value;

	m_OccurenceValue.GetValue( Value );
	return Value;
}

void SVStatisticsToolClass::SetOccurenceTestValue( const std::string& rValue )
{
	m_OccurenceValue.SetValue( rValue);
}

SVObjectReference SVStatisticsToolClass::GetVariableSelected() const
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

void SVStatisticsToolClass::SetVariableSelected( const std::string& rName )
{
	if( HasVariable() )
	{
		if( m_inputObjectInfo.IsConnected() && m_inputObjectInfo.GetInputObjectInfo().getObject() )
		{
			m_inputObjectInfo.GetInputObjectInfo().getObject()->DisconnectObjectInput(&m_inputObjectInfo);
		}
	}

	if( !rName.empty() )
	{
		// Get the Object
		SVObjectReference refObject;
		/*HRESULT hrGet = */SVObjectManagerClass::Instance().GetObjectByDottedName( rName.c_str(), refObject );
		if( refObject.getObject() )
		{
			// Connect to the input
			m_inputObjectInfo.SetObject( GetObjectInfo() );

			//m_inputObjectInfo.InputObjectInfo.UniqueObjectID = selectedVarGuid;
			m_inputObjectInfo.SetInputObject( refObject );
			
			refObject.getObject()->ConnectObjectInput(&m_inputObjectInfo);
		}// end if( refObject.Object() )
		m_VariableName.SetValue(rName);
	}
	else
	{
		//msvVariableGUID_OBSOLETE.SetValue( guidStr );
		m_VariableName.SetValue(std::string());

		// Clear the Object Info
		m_inputObjectInfo.SetInputObject( nullptr );
	}
}

bool SVStatisticsToolClass::DisconnectObjectInput(SvOl::SVInObjectInfoStruct* pObjectInInfo )
{
	if( pObjectInInfo && pObjectInInfo->GetInputObjectInfo().getUniqueObjectID() == m_inputObjectInfo.GetInputObjectInfo().getUniqueObjectID() )
	{
		m_inputObjectInfo.SetInputObject( nullptr );

		// Check if variable still exists and is selectable for stats
		if( !HasVariable() )
		{
			// Clear it since the object is gone
			m_inputObjectInfo.SetInputObject( nullptr );
			SetVariableSelected( _T("") );	// will this result in a recursive call to this message???
		}
	}
	return __super::DisconnectObjectInput(pObjectInInfo);
}

double SVStatisticsToolClass::getInputValue()
{
	double Result( 0.0 );
	if(m_inputObjectInfo.IsConnected() && nullptr != m_inputObjectInfo.GetInputObjectInfo().getObject())
	{
		const SVObjectReference& rObjectRef = m_inputObjectInfo.GetInputObjectInfo().GetObjectReference();
		m_inputObjectInfo.GetInputObjectInfo().getObject()->getValue( Result, rObjectRef.getValidArrayIndex() );
	}
	return Result;
}

double SVStatisticsToolClass::getNumberOfSamples()
{
	long count;
	m_EnabledCount.GetValue( count );

	return static_cast<double> (count + 1);
}

// Reset Statistics values
void SVStatisticsToolClass::resetValues()
{
	m_Value[SV_STATS_MIN_VALUE].SetValue(0.0);
	m_Value[SV_STATS_MAX_VALUE].SetValue(0.0);
	m_Value[SV_STATS_AVERAGEOF_VALUES].SetValue(0.0);
	m_Value[SV_STATS_STANDARD_DEVIATION].SetValue(0.0);
	m_Value[SV_STATS_VARIANCEIN_VALUES].SetValue(0.0);
	m_Value[SV_STATS_NUMBEROF_OCCURANCES].SetValue(0.0);
	m_Value[SV_STATS_PERCENTOF_OCCURANCES].SetValue(0.0);
	m_AccumulatedSquares = 0.0;
	m_AccumulatedTotal = 0.0;
}

double SVStatisticsToolClass::calculateVariance( double aNumberOfSamples, double aAverageValue )
{
	double averageTimesSamples = aAverageValue * aNumberOfSamples; 
	double averageTimesSamplesSquared = (averageTimesSamples * averageTimesSamples);

	double value = ( 1.0 / ( aNumberOfSamples * ( aNumberOfSamples - 1.0 ) ) ) *
	               ( aNumberOfSamples * m_AccumulatedSquares - ( averageTimesSamplesSquared) );

	return value;
}

bool SVStatisticsToolClass::HasVariable() const
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

bool SVStatisticsToolClass::onRun(SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages)
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

		double dInputValue = getInputValue();

		numberOfSamples = getNumberOfSamples();
		if (numberOfSamples <= 1)
		{
			resetValues();
		}
	
		// Calculate Average (Mean)
		value = dInputValue;
		m_AccumulatedTotal += value;
		if (0 < numberOfSamples)
		{
			averageValue = m_AccumulatedTotal / numberOfSamples;
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

					if (numberOfSamples > 1)
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

					if (numberOfSamples > 1)
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
					if (numberOfSamples > 1)
					{
						value = calculateVariance(numberOfSamples, averageValue);

						m_Value[SV_STATS_VARIANCEIN_VALUES].SetValue(value);
					}
					break;

				case SV_STATS_STANDARD_DEVIATION:
					// if variance is already calculated
					if (numberOfSamples > 1)
					{
						if (FeatureString[SV_STATS_VARIANCEIN_VALUES] == '1')
						{
							m_Value[SV_STATS_VARIANCEIN_VALUES].GetValue(value);
						}
						else
						{
							value = calculateVariance(numberOfSamples, averageValue);
						}
						// Standard Deviation is the positive square root of the Variance
						if (value)
							value = fabs(sqrt(value));

						m_Value[SV_STATS_STANDARD_DEVIATION].SetValue(value);
					}
					break;

				case SV_STATS_PERCENTOF_OCCURANCES:
					if (!occurenceValueStr.empty() && numberOfSamples)
					{
						m_Value[SV_STATS_NUMBEROF_OCCURANCES].GetValue(count);

						// Calculate percentile
						value = (count / numberOfSamples) * 100.0;

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

bool SVStatisticsToolClass::Test(SvStl::MessageContainerVector *pErrorMessages)
{
	if( !ValidateLocal(pErrorMessages) )
	{
		return false;
	}

	// verify that the object is really valid
	SVObjectClass* pObject = SVObjectManagerClass::Instance().GetObject( m_inputObjectInfo.GetInputObjectInfo().getUniqueObjectID() );
	if ( nullptr != pObject )
	{
		SVObjectReference ObjectRef = m_inputObjectInfo.GetInputObjectInfo().GetObjectReference();
		if( nullptr != ObjectRef.getObject() )
		{
			double Value;
			HRESULT hr = ObjectRef.getObject()->getValue( Value, ObjectRef.getValidArrayIndex() );
			if ( S_OK == hr || SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE == hr )
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
					message.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_StatToolInvalidVariable, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10201, GetUniqueObjectID());
					pErrorMessages->push_back( message );
				}
				return false;
			}
		}
	}

	//Test failed, if reach this point.
	if (nullptr != pErrorMessages)
	{
		SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_StatToolTestFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
		pErrorMessages->push_back(Msg);
	}

	return false;
}

bool SVStatisticsToolClass::ValidateLocal(SvStl::MessageContainerVector *pErrorMessages) const
{
	if( HasVariable() )
	{
		if ( !m_inputObjectInfo.IsConnected() || nullptr == m_inputObjectInfo.GetInputObjectInfo().getObject() )
		{
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
			return false;
		}
	}

	return true;
}

} //namespace SvTo
