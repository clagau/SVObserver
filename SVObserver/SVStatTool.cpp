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

#include "SVObjectLibrary/SVObjectManagerClass.h"

#include "SVObjectLibrary\SVGetObjectDequeByTypeVisitor.h"
#include "SVGlobal.h"
#include "SVInspectionProcess.h"
#include "SVResultDouble.h"
#include "SVToolSet.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "TextDefinesSvO.h"
#include "SVStatusLibrary\MessageManager.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SV_IMPLEMENT_CLASS( SVStatisticsToolClass, SVStatisticsToolClassGuid );


SVStatisticsToolClass::SVStatisticsToolClass( SVObjectClass* POwner, int StringResourceID )
				:SVToolClass( POwner, StringResourceID )
{
	init();
}

void SVStatisticsToolClass::init(void)
{
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SVToolObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType    = SVStatisticsToolObjectType;
	
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

	// Kludge
	SetObjectDepth (2);	

	// Save default attributes
	m_DefaultAttributes = m_Value [ SV_STATS_MIN_VALUE ].ObjectAttributesAllowed();

	TCHAR szFeaturesEnabled[SV_NUMBER_OF_STAT_FEATURES + 1];

	// Set Embedded defaults
	for (int i = SV_STATS_MIN_VALUE; i < SV_STATS_TOPOF_LIST; i = (SVStatisticsFeatureEnum) (i + 1))
	{
		m_Value [i].SetDefaultValue(0.0, true);
		m_Value [i].SetObjectAttributesAllowed( SvDef::SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES, SvOi::SetAttributeType::RemoveAttribute );
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

	for ( int i = 0; i < SV_NUMBER_OF_STAT_FEATURES; i++ )
	{
		m_Value[i].SetObjectAttributesAllowed( SvDef::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	}

	m_OccurenceValue.SetObjectAttributesAllowed( SvDef::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	m_PersistantFeaturesEnabled.SetObjectAttributesAllowed( SvDef::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	m_VariableGUID_OBSOLETE.SetObjectAttributesAllowed( SvDef::SV_HIDDEN, SvOi::SetAttributeType::OverwriteAttribute );
	m_VariableName.SetObjectAttributesAllowed( SvDef::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );

	// Override base class exposure of the drawflag
	// This value will not be exposed for the Statistics Tool.
	drawToolFlag.SetObjectAttributesAllowed( SvDef::SV_HIDDEN, SvOi::SetAttributeType::OverwriteAttribute );

	// Override base class exposure of the auxillaryextent variables
	// These values will not be exposed for the Statistics Tool.
	m_svUpdateAuxiliaryExtents.SetObjectAttributesAllowed( SvDef::SV_HIDDEN, SvOi::SetAttributeType::OverwriteAttribute );
	m_svAuxiliarySourceX.SetObjectAttributesAllowed( SvDef::SV_HIDDEN, SvOi::SetAttributeType::OverwriteAttribute );
	m_svAuxiliarySourceY.SetObjectAttributesAllowed( SvDef::SV_HIDDEN, SvOi::SetAttributeType::OverwriteAttribute );
	m_svAuxiliarySourceAngle.SetObjectAttributesAllowed( SvDef::SV_HIDDEN, SvOi::SetAttributeType::OverwriteAttribute );
	m_svAuxiliarySourceImageName.SetObjectAttributesAllowed( SvDef::SV_HIDDEN, SvOi::SetAttributeType::OverwriteAttribute );
	m_svAuxiliaryDrawType.SetObjectAttributesAllowed( SvDef::SV_HIDDEN, SvOi::SetAttributeType::OverwriteAttribute );

	SVString Name;
	m_VariableName.GetValue( Name );

	if ( Name.empty() )
	{
		// check for backwards compatibility
		SVString GuidString;
		m_VariableGUID_OBSOLETE.GetValue( GuidString );
		if ( !GuidString.empty() )
		{
			GUID guid( SV_GUID_NULL );
			AfxGetClassIDFromString(GuidString.c_str(), &guid);
			SVObjectClass* pObject = SVObjectManagerClass::Instance().GetObject(guid);
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
	
	if ( Result )
	{
		SVString Name;
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

SVString SVStatisticsToolClass::GetFeatureString()
{
	SVString FeatureString;
	m_PersistantFeaturesEnabled.GetValue( FeatureString );
	return FeatureString;
}

SVString SVStatisticsToolClass::GetFeatureName( int aIndex )
{
	return SVString( m_Value[aIndex].GetName() );
}

/////////////////////////////////////////////////////////////////////////////
//
void SVStatisticsToolClass::EnableFeature (SVStatisticsFeatureEnum aIndex)
{
	m_Value[aIndex].SetObjectAttributesAllowed( m_DefaultAttributes, SvOi::SetAttributeType::OverwriteAttribute );

	SVString FeatureString;
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
	SVString FeatureString;
	m_PersistantFeaturesEnabled.GetValue( FeatureString );
	for( int iFeature = SV_STATS_MIN_VALUE ; iFeature < SV_STATS_TOPOF_LIST ; iFeature++ )
	{
		if( '1' == FeatureString[iFeature] )
		{
			m_Value[iFeature].SetObjectAttributesAllowed( m_DefaultAttributes, SvOi::SetAttributeType::OverwriteAttribute );
		}
		else
		{
			m_Value[iFeature].SetObjectAttributesAllowed( SvDef::SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES, SvOi::SetAttributeType::RemoveAttribute );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
//
DWORD SVStatisticsToolClass::DisableFeature (SVStatisticsFeatureEnum aIndex)
{

	m_Value[aIndex].SetObjectAttributesAllowed( SvDef::SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES, SvOi::SetAttributeType::RemoveAttribute );

	m_Value[aIndex].SetObjectAttributesSet( 0, SvOi::SetAttributeType::OverwriteAttribute );

	SVString FeatureString;
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
	SVClassInfoStruct       resultClassInfo;
	SVObjectTypeInfoStruct  interfaceInfo;

		// Declare Input Interface of Result...
		interfaceInfo.EmbeddedID = m_Value [aFeatureIndex].GetEmbeddedID();
		resultClassInfo.m_DesiredInputInterface.Add( interfaceInfo );

		resultClassInfo.m_ObjectTypeInfo.ObjectType = SVResultObjectType;
		resultClassInfo.m_ObjectTypeInfo.SubType	= SVResultDoubleObjectType;
		resultClassInfo.m_ClassId = SVDoubleResultClassGuid;
		resultClassInfo.m_ClassName = SvUl_SF::LoadSVString( IDS_OBJECTNAME_RESULT );
		resultClassInfo.m_ClassName += _T(" ") + SVString(m_Value [aFeatureIndex].GetName());

		// Construct the result class
		SVDoubleResultClass* pResult = dynamic_cast<SVDoubleResultClass *>(resultClassInfo.Construct());

		if( nullptr == pResult)
		{
			SvStl::MessageContainer Exception( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16207, GetUniqueObjectID() );
			throw Exception;
		}

		Add( pResult );

		SVObjectTypeInfoStruct info;
		info.ObjectType = SVValueObjectType;
		info.SubType = SVDoubleValueObjectType;
		info.EmbeddedID = SVValueObjectGuid;

		// Get the output of the result
		SVDoubleValueObjectClass* pValue = dynamic_cast<SVDoubleValueObjectClass*>(pResult->getFirstObject(info));

		if(nullptr == pValue)
		{
			SvStl::MessageContainer Exception( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16208, GetUniqueObjectID() );
			throw Exception;
		}

		// Make it non visible for any selection
		pValue->SetObjectAttributesAllowed( SvDef::SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES, SvOi::SetAttributeType::RemoveAttribute );

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
				if( SV_GUID_NULL != objectID )
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
	SVResultClass*    pResult;
	DWORD LastError(0);
	
	while (1)
	{
		pResult = GetResult(aFeatureIndex);
		
		if (!pResult)
		{
			SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16208);
			LastError = -SvStl::Err_16208;
			break;
		}
		
		DestroyChildObject(pResult, SvDef::SVMFSetDefaultInputs);
		pResult = nullptr;
		break;
	}
	
	return LastError;
}

SVResultClass* SVStatisticsToolClass::GetResult(SVStatisticsFeatureEnum aFeatureIndex)
{
	SVInputInfoListClass	resultInputList;
	SVOutputInfoListClass	resultOutputList;
	
	SVInObjectInfoStruct*	pResultInputInfo;
	
	SVObjectTypeInfoStruct  info;
	SVDoubleResultClass*    pResult;
	SVObjectClass*          pSVObject;
	
	bool                    bDone = false;
	
	info.ObjectType = SVResultObjectType;
	info.SubType = SVResultDoubleObjectType;
	
	SVGetObjectDequeByTypeVisitor l_Visitor( info );

	SVObjectManagerClass::Instance().VisitElements( l_Visitor, GetUniqueObjectID() );

	SVGetObjectDequeByTypeVisitor::SVObjectPtrDeque::const_iterator l_Iter;

	for( l_Iter = l_Visitor.GetObjects().begin(); l_Iter != l_Visitor.GetObjects().end() && !bDone; ++l_Iter )
	{
		pResult = dynamic_cast< SVDoubleResultClass* >( const_cast< SVObjectClass* >( *l_Iter ) );

		if( nullptr != pResult )
		{
			pResult->GetPrivateInputList( resultInputList );
			
			pResultInputInfo = resultInputList.GetAt( 0 );
			
			pSVObject = pResultInputInfo->GetInputObjectInfo().m_pObject;
			
			if (&m_Value [aFeatureIndex] == pSVObject)
			{
				bDone = true;
			}
		}
	}
		
	return pResult;
}

SVString SVStatisticsToolClass::GetOccurenceTestValue()
{
	SVString Value;

	m_OccurenceValue.GetValue( Value );
	return Value;
}

void SVStatisticsToolClass::SetOccurenceTestValue( const SVString& rValue )
{
	m_OccurenceValue.SetValue( rValue);
}

SVObjectReference SVStatisticsToolClass::GetVariableSelected() const
{
	SVString Name;
	m_VariableName.GetValue( Name );

	SVObjectReference refObject;

	if( !Name.empty() )
	{
		SVObjectManagerClass::Instance().GetObjectByDottedName( Name, refObject );
	}
			
	return refObject;
}

void SVStatisticsToolClass::SetVariableSelected( SVObjectReference p_refObject )
{
	SetVariableSelected( p_refObject.GetCompleteName().c_str() );
}

void SVStatisticsToolClass::SetVariableSelected( const SVString& rName )
{
	if( HasVariable() )
	{
		if( m_inputObjectInfo.IsConnected() && m_inputObjectInfo.GetInputObjectInfo().m_pObject )
		{
			m_inputObjectInfo.GetInputObjectInfo().m_pObject->DisconnectObjectInput(&m_inputObjectInfo);
		}
	}

	if( !rName.empty() )
	{
		// Get the Object
		SVObjectReference refObject;
		HRESULT hrGet = SVObjectManagerClass::Instance().GetObjectByDottedName( rName.c_str(), refObject );
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
		m_VariableName.SetValue(SVString());

		// Clear the Object Info
		m_inputObjectInfo.SetInputObject( nullptr );
	}
}

void SVStatisticsToolClass::UpdateTaskObjectOutputListAttributes()
{
	SVToolClass::UpdateTaskObjectOutputListAttributes( m_inputObjectInfo.GetInputObjectInfo().GetObjectReference(), SvDef::SV_SELECTABLE_FOR_STATISTICS );
}

bool SVStatisticsToolClass::DisconnectObjectInput( SVInObjectInfoStruct* pObjectInInfo )
{
	if( pObjectInInfo && pObjectInInfo->GetInputObjectInfo().m_UniqueObjectID == m_inputObjectInfo.GetInputObjectInfo().m_UniqueObjectID )
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
	if(m_inputObjectInfo.IsConnected() && nullptr != m_inputObjectInfo.GetInputObjectInfo().m_pObject)
	{
		const SVObjectReference& rObjectRef = m_inputObjectInfo.GetInputObjectInfo().GetObjectReference();
		m_inputObjectInfo.GetInputObjectInfo().m_pObject->getValue( Result, rObjectRef.getValidArrayIndex() );
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

	if( refObject.getObject() )
	{
		// Double-check if variable still exists
		// verify that the object is really valid
		if ( refObject.getObject() )
		{
			// Special Check for BlobAnalyzer/StatTool Features
			// which don't really go away they just change attributes
			if ( refObject.ObjectAttributesAllowed() & SvDef::SV_SELECTABLE_FOR_STATISTICS )
			{
				bRetVal = true;
			}
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
		SVString FeatureString;
		m_PersistantFeaturesEnabled.GetValue(FeatureString);

		// Calculate Average and Number of Occurences always
		// Since they are use in other calculations
		double averageValue;
		double value, count, numberOfSamples;

		double dInputValue = getInputValue();

		numberOfSamples = getNumberOfSamples();

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
		SVString occurenceValueStr;
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
		SetInvalid();
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
	SVObjectClass* pObject = SVObjectManagerClass::Instance().GetObject( m_inputObjectInfo.GetInputObjectInfo().m_UniqueObjectID );
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
					SVString CompleteName = ObjectRef.getObject()->GetCompleteObjectNameToObjectType( nullptr, SVInspectionObjectType );
					SVStringVector msgList;
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
		if ( !m_inputObjectInfo.IsConnected() || nullptr == m_inputObjectInfo.GetInputObjectInfo().m_pObject )
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
