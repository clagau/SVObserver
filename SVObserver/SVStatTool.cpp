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
#include "ObjectInterfaces\ErrorNumbers.h"
#include "TextDefinesSvO.h"
#include "SVStatusLibrary\MessageManager.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SV_IMPLEMENT_CLASS( SVStatisticsToolClass, SVStatisticsToolClassGuid );


SVStatisticsToolClass::SVStatisticsToolClass( BOOL BCreateDefaultTaskList, 
                                              SVObjectClass* POwner, 
                                              int StringResourceID )
				:SVToolClass( BCreateDefaultTaskList, POwner, StringResourceID )
{
	init();
}

void SVStatisticsToolClass::init(void)
{
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVToolObjectType;
	m_outObjectInfo.ObjectTypeInfo.SubType    = SVStatisticsToolObjectType;
	
	// Register an empty input object
	m_inputObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_inputObjectInfo, _T( "StatisticsToolValue" ) );

	// Remove Embedded Extents
	removeEmbeddedExtents();

	// Set up your type...
	//Register embedded objects.
		RegisterEmbeddedObject(
			&msvValue [SV_STATS_MIN_VALUE], 
			SVStatMinObjectGuid,
			IDS_OBJECTNAME_STATMIN,
			false, SVResetItemNone );
		
		RegisterEmbeddedObject(
			&msvValue [SV_STATS_MAX_VALUE], 
			SVStatMaxObjectGuid,
			IDS_OBJECTNAME_STATMAX,
			false, SVResetItemNone );
		
		RegisterEmbeddedObject(
			&msvValue [SV_STATS_AVERAGEOF_VALUES], 
			SVStatAverageObjectGuid,
			IDS_OBJECTNAME_STATAVERAGE,
			false, SVResetItemNone );
		
		RegisterEmbeddedObject(
			&msvValue [SV_STATS_STANDARD_DEVIATION], 
			SVStatStdDevObjectGuid,
			IDS_OBJECTNAME_STATSTDDEV,
			false, SVResetItemNone );
		
		RegisterEmbeddedObject(
			&msvValue [SV_STATS_VARIANCEIN_VALUES], 
			SVStatVarianceObjectGuid,
			IDS_OBJECTNAME_STATVARIANCE,
			false, SVResetItemNone );
		
		RegisterEmbeddedObject(
			&msvValue [SV_STATS_NUMBEROF_OCCURANCES], 
			SVStatNbrOfOccurancesObjectGuid,
			IDS_OBJECTNAME_STATNBROFOCCURANCES,
			false, SVResetItemNone );
		
		RegisterEmbeddedObject(
			&msvValue [SV_STATS_PERCENTOF_OCCURANCES], 
			SVStatPercentOfOccurancesObjectGuid,
			IDS_OBJECTNAME_STATPERCENTOFOCCURANCES,
			false, SVResetItemNone );
		
		RegisterEmbeddedObject(
			&msvOccurenceValue, 
			SVStatTestValueObjectGuid,
			IDS_OBJECTNAME_STATTESTVALUE,
			false, SVResetItemTool );
		
		RegisterEmbeddedObject(
			&msvVariableGUID_OBSOLETE, 
			SVStatVariableToMonitorObjectGuid,
			IDS_OBJECTNAME_VARIABLEGUID,
			false, SVResetItemTool );
		
		RegisterEmbeddedObject(
			&msvVariableName, 
			SVStatVariableNameToMonitorObjectGuid,
			IDS_OBJECTNAME_STAT_VARIABLE_NAME,
			false, SVResetItemTool );
		
		RegisterEmbeddedObject(
			&msvPersistantFeaturesEnabled, 
			SVStatEnabledFeaturesObjectGuid,
			IDS_OBJECTNAME_ENABLEDFEATURES,
			false, SVResetItemTool );

	// Kludge
	SetObjectDepth (2);	

	// Save default attributes
	msvlDefaultAttributes = msvValue [ SV_STATS_MIN_VALUE ].ObjectAttributesAllowed();

	TCHAR szFeaturesEnabled[SV_NUMBER_OF_STAT_FEATURES + 1];

	// Set Embedded defaults
	for (int i = SV_STATS_MIN_VALUE; i < SV_STATS_TOPOF_LIST; i = (SVStatisticsFeatureEnum) (i + 1))
	{
		msvValue [i].SetDefaultValue(0.0, TRUE);
		msvValue [i].ObjectAttributesAllowedRef() &= ( ~SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES );

		/*----- FEATURE LIST ---------------------------------------------------------*/
		/*----- The list of enabled features is kept in a string because, of the      */
		/*----- "value objects", the string appeared to contain the least overhead. --*/
		szFeaturesEnabled [i] = _T('0');             // Not enabled.
	}
	
	szFeaturesEnabled [SV_STATS_TOPOF_LIST] = _T( '\0' );    // Null termination.
	
	msvPersistantFeaturesEnabled.SetDefaultValue (szFeaturesEnabled,
		                                          TRUE);

	msvAccumulatedSquares = 0.0;
	msvAccumulatedTotal = 0.0;

	// Set default inputs and outputs
	addDefaultInputObjects();

}

SVStatisticsToolClass::~SVStatisticsToolClass()
{ 
}

BOOL SVStatisticsToolClass::CreateObject(SVObjectLevelCreateStruct* PCreateStruct )
{
	m_isCreated = SVToolClass::CreateObject( PCreateStruct );

	for ( int i = 0; i < SV_NUMBER_OF_STAT_FEATURES; i++ )
	{
		msvValue[i].ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	}

	msvOccurenceValue.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	msvPersistantFeaturesEnabled.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	msvVariableGUID_OBSOLETE.ObjectAttributesAllowedRef() = SV_HIDDEN;
	msvVariableName.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;

	// Override base class exposure of the drawflag
	// This value will not be exposed for the Statistics Tool.
	drawToolFlag.ObjectAttributesAllowedRef() = SV_HIDDEN;

	// Override base class exposure of the auxillaryextent variables
	// These values will not be exposed for the Statistics Tool.
	m_svUpdateAuxiliaryExtents.ObjectAttributesAllowedRef() = SV_HIDDEN;
	m_svAuxiliarySourceX.ObjectAttributesAllowedRef() = SV_HIDDEN;
	m_svAuxiliarySourceY.ObjectAttributesAllowedRef() = SV_HIDDEN;
	m_svAuxiliarySourceAngle.ObjectAttributesAllowedRef() = SV_HIDDEN;
	m_svAuxiliarySourceImageName.ObjectAttributesAllowedRef() = SV_HIDDEN;
	m_svAuxiliaryDrawType.ObjectAttributesAllowedRef() = SV_HIDDEN;

	SVString Name;
	msvVariableName.GetValue( Name );

	if ( Name.empty() )
	{
		// check for backwards compatibility
		SVString GuidString;
		msvVariableGUID_OBSOLETE.GetValue( GuidString );
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
		msvVariableName.GetValue( Name );
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
	msvPersistantFeaturesEnabled.GetValue( FeatureString );
	return FeatureString;
}

SVString SVStatisticsToolClass::GetFeatureName( int aIndex )
{
	return SVString( msvValue[aIndex].GetName() );
}

/////////////////////////////////////////////////////////////////////////////
//
DWORD SVStatisticsToolClass::EnableFeature (SVStatisticsFeatureEnum aIndex)
{
	msvValue[aIndex].ObjectAttributesAllowedRef() = msvlDefaultAttributes;

	SVString FeatureString;
	msvPersistantFeaturesEnabled.GetValue(FeatureString);

	if( aIndex < static_cast<int> (FeatureString.size()) )
	{
		FeatureString[aIndex] =  '1';
	}
	msvPersistantFeaturesEnabled.SetValue(1, FeatureString);

	AllocateResult (aIndex);

	return 0;
}

////////////////////////////////////////////////////
// This function restores feature attributesAllowed
// that were previously stored in the configuration.
////////////////////////////////////////////////////
void SVStatisticsToolClass::RestoreFeatureAttributes()
{
	SVString FeatureString;
	msvPersistantFeaturesEnabled.GetValue( FeatureString );
	for( int iFeature = SV_STATS_MIN_VALUE ; iFeature < SV_STATS_TOPOF_LIST ; iFeature++ )
	{
		if( '1' == FeatureString[iFeature] )
		{
			msvValue[iFeature].ObjectAttributesAllowedRef() = msvlDefaultAttributes;
		}
		else
		{
			msvValue[iFeature].ObjectAttributesAllowedRef() &= ~SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
//
DWORD SVStatisticsToolClass::DisableFeature (SVStatisticsFeatureEnum aIndex)
{

	msvValue [aIndex].ObjectAttributesAllowedRef() &= (~SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES);

	msvValue [aIndex].ObjectAttributesSetRef() = 0;

	SVString FeatureString;
	msvPersistantFeaturesEnabled.GetValue( FeatureString );
	if( aIndex < static_cast<int>  (FeatureString.size()) )
	{
		FeatureString[aIndex] = '0';
	}
	msvPersistantFeaturesEnabled.SetValue( 1, FeatureString );

	FreeResult (aIndex);

	// Get this object's outputInfo
	SVOutObjectInfoStruct& valueOutObjectInfo = msvValue [aIndex].GetObjectOutputInfo();

	valueOutObjectInfo.DisconnectAllInputs();

	return 0;
}

DWORD SVStatisticsToolClass::AllocateResult (SVStatisticsFeatureEnum aFeatureIndex)
{
	SVClassInfoStruct       resultClassInfo;
	SVObjectTypeInfoStruct  interfaceInfo;

	SVDoubleResultClass*    pResult;
	
	DWORD LastError(0);
	
	while (1)
	{
		// Setup the result
		
		// Declare Input Interface of Result...
		interfaceInfo.EmbeddedID = msvValue [aFeatureIndex].GetEmbeddedID();
		resultClassInfo.m_DesiredInputInterface.Add( interfaceInfo );
		
		resultClassInfo.m_ObjectTypeInfo.ObjectType = SVResultObjectType;
		resultClassInfo.m_ObjectTypeInfo.SubType	= SVResultDoubleObjectType;
		resultClassInfo.m_ClassId = SVDoubleResultClassGuid;
		resultClassInfo.m_ClassName = SvUl_SF::LoadSVString( IDS_OBJECTNAME_RESULT );
		resultClassInfo.m_ClassName += _T(" ") + SVString(msvValue [aFeatureIndex].GetName());
		
		// Construct the result class
		pResult = (SVDoubleResultClass *) resultClassInfo.Construct();
		
		if(!pResult)
		{
			SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16207);
			LastError = -SvOi::Err_16207;
			break;
		}
		
		Add( pResult );
		
		SVObjectTypeInfoStruct info;
		info.ObjectType = SVDoubleValueObjectType;
		info.EmbeddedID = SVValueObjectGuid;
		
		// Get the output of the result
		SVDoubleValueObjectClass* pValue = dynamic_cast<SVDoubleValueObjectClass*>(pResult->getFirstObject(info));
		
		if (!pValue)
		{
			SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16208);
			LastError = -SvOi::Err_16208;
			break;
		}
		
		// Make it non visible for any selection
		pValue->ObjectAttributesAllowedRef() &= ( ~SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES );
		
		// Ensure this Object's inputs get connected
		pResult->ConnectAllInputs();
		
		// And last - Create (initialize) it
		
		if( ! pResult->IsCreated() )
		{
			// And finally try to create the child object...
			if( !CreateChildObject(pResult) )
			{
				SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
				Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_StatTool_ResultFailed, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10200 ); 
				
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
			}
		}
		break;
	}
	
	return LastError;
}

DWORD SVStatisticsToolClass::FreeResult (SVStatisticsFeatureEnum aFeatureIndex)
{
	SVResultClass*    pResult;
	DWORD LastError(0);
	
	while (1)
	{
		pResult = GetResultObject (aFeatureIndex);
		
		if (!pResult)
		{
			SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
			MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16208);
			LastError = -SvOi::Err_16208;
			break;
		}
		
		DestroyChildObject(pResult, SVMFSetDefaultInputs);
		pResult = nullptr;
		break;
	}
	
	return LastError;
}

SVResultClass* SVStatisticsToolClass::GetResultObject(SVStatisticsFeatureEnum aFeatureIndex)
{
	SVInputInfoListClass	resultInputList;
	SVOutputInfoListClass	resultOutputList;
	
	SVInObjectInfoStruct*	pResultInputInfo;
	
	SVObjectTypeInfoStruct  info;
	SVDoubleResultClass*    pResult;
	SVObjectClass*          pSVObject;
	
	long                    lDone;
	
	
	lDone = FALSE;
	
	info.ObjectType = SVResultObjectType;
	info.SubType = SVResultDoubleObjectType;
	
	SVGetObjectDequeByTypeVisitor l_Visitor( info );

	SVObjectManagerClass::Instance().VisitElements( l_Visitor, GetUniqueObjectID() );

	SVGetObjectDequeByTypeVisitor::SVObjectPtrDeque::const_iterator l_Iter;

	for( l_Iter = l_Visitor.GetObjects().begin(); l_Iter != l_Visitor.GetObjects().end() && !lDone; ++l_Iter )
	{
		pResult = dynamic_cast< SVDoubleResultClass* >( const_cast< SVObjectClass* >( *l_Iter ) );

		if( nullptr != pResult )
		{
			pResult->GetPrivateInputList( resultInputList );
			
			pResultInputInfo = resultInputList.GetAt( 0 );
			
			pSVObject = pResultInputInfo->GetInputObjectInfo().PObject;
			
			if (&msvValue [aFeatureIndex] == pSVObject)
			{
				lDone = TRUE;
			}
		}
	}
		
	return pResult;
}

SVString SVStatisticsToolClass::GetOccurenceTestValue()
{
	SVString Value;

	msvOccurenceValue.GetValue( Value );
	return Value;
}

void SVStatisticsToolClass::SetOccurenceTestValue( const SVString& rValue )
{
	msvOccurenceValue.SetValue( 1, rValue );
}

SVObjectReference SVStatisticsToolClass::GetVariableSelected() const
{
	SVString Name;
	msvVariableName.GetValue( Name );

	SVObjectReference refObject;

	if( !Name.empty() )
	{
		// much better! (~50 µs)
		SVValueObjectReference refValueObject;
		HRESULT hr = SVObjectManagerClass::Instance().GetObjectByDottedName( Name.c_str(), refValueObject );
		refObject = refValueObject;
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
		if( m_inputObjectInfo.IsConnected() && m_inputObjectInfo.GetInputObjectInfo().PObject )
		{
			m_inputObjectInfo.GetInputObjectInfo().PObject->DisconnectObjectInput(&m_inputObjectInfo);
		}
	}

	if( !rName.empty() )
	{
		// Get the Object
		SVObjectReference refObject;
		HRESULT hrGet = SVObjectManagerClass::Instance().GetObjectByDottedName( rName.c_str(), refObject );
		if( refObject.Object() )
		{
			// Connect to the input
			m_inputObjectInfo.SetObject( GetObjectInfo() );

			//m_inputObjectInfo.InputObjectInfo.UniqueObjectID = selectedVarGuid;
			m_inputObjectInfo.SetInputObject( refObject );
			
			refObject.Object()->ConnectObjectInput(&m_inputObjectInfo);
		}// end if( refObject.Object() )
		msvVariableName.SetValue( 1, rName );
	}
	else
	{
		//msvVariableGUID_OBSOLETE.SetValue(1, guidStr);
		msvVariableName.SetValue( 1, SVString() );

		// Clear the Object Info
		m_inputObjectInfo.SetInputObject( nullptr );
	}
}

void SVStatisticsToolClass::UpdateTaskObjectOutputListAttributes()
{
	SVToolClass::UpdateTaskObjectOutputListAttributes( m_inputObjectInfo.GetInputObjectInfo().GetObjectReference(), SV_SELECTABLE_FOR_STATISTICS );
}

bool SVStatisticsToolClass::DisconnectObjectInput( SVInObjectInfoStruct* pObjectInInfo )
{
	if( pObjectInInfo && pObjectInInfo->GetInputObjectInfo().UniqueObjectID == m_inputObjectInfo.GetInputObjectInfo().UniqueObjectID )
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
	double value = 0.0;
	if( m_inputObjectInfo.IsConnected() && m_inputObjectInfo.GetInputObjectInfo().PObject )
	{
		SVValueObjectReference refObject = m_inputObjectInfo.GetInputObjectInfo().GetObjectReference();
		ASSERT( refObject.Object() );
		refObject.GetValue( value );
		//( ( SVValueObjectClass* )( m_inputObjectInfo.InputObjectInfo.PObject ) )->GetValue( value );
	}
	return value;
}

double SVStatisticsToolClass::getNumberOfSamples()
{
	double count;
	enabledCount.GetValue( count );

	return count + 1;
}

// Reset Statistics values
void SVStatisticsToolClass::resetValues()
{
	msvValue[SV_STATS_MIN_VALUE].SetValue(1, 0);
	msvValue[SV_STATS_MAX_VALUE].SetValue(1, 0);
	msvValue[SV_STATS_AVERAGEOF_VALUES].SetValue(1, 0);
	msvValue[SV_STATS_STANDARD_DEVIATION].SetValue(1, 0);
	msvValue[SV_STATS_VARIANCEIN_VALUES].SetValue(1, 0);
	msvValue[SV_STATS_NUMBEROF_OCCURANCES].SetValue(1, 0);
	msvValue[SV_STATS_PERCENTOF_OCCURANCES].SetValue(1, 0);
	msvAccumulatedSquares = 0.0;
	msvAccumulatedTotal = 0.0;
}

double SVStatisticsToolClass::calculateVariance( double aNumberOfSamples, double aAverageValue )
{
	double averageTimesSamples = aAverageValue * aNumberOfSamples; 
	double averageTimesSamplesSquared = (averageTimesSamples * averageTimesSamples);

	double value = ( 1.0 / ( aNumberOfSamples * ( aNumberOfSamples - 1.0 ) ) ) *
	               ( aNumberOfSamples * msvAccumulatedSquares - ( averageTimesSamplesSquared) );

	return value;
}

BOOL SVStatisticsToolClass::HasVariable() const
{
	BOOL bRetVal = FALSE;
	SVObjectReference refObject = GetVariableSelected();

	if( refObject.Object() )
	{
		// Double-check if variable still exists
		// verify that the object is really valid
		if ( refObject.Object() )
		{
			// Special Check for BlobAnalyzer/StatTool Features
			// which don't really go away they just change attributes
			if ( refObject.ObjectAttributesAllowed() & SV_SELECTABLE_FOR_STATISTICS )
				bRetVal = TRUE;
		}
	}

	return bRetVal;
}

BOOL SVStatisticsToolClass::onRun( SVRunStatusClass& RRunStatus )
{
	BOOL l_bOk = SVToolClass::onRun( RRunStatus );

	if( ValidateLocal(&m_RunErrorMessages) )
	{
		if( !RRunStatus.IsDisabled() && !RRunStatus.IsDisabledByCondition() )
		{
			// check which result to calculate
			SVString FeatureString;
			msvPersistantFeaturesEnabled.GetValue( FeatureString );

			// Calculate Average and Number of Occurences always
			// Since they are use in other calculations
			double averageValue;
			double value,count,numberOfSamples;
			
			double dInputValue = getInputValue();
			
			numberOfSamples = getNumberOfSamples();

			// Calculate Average (Mean)
			value = dInputValue;
			msvAccumulatedTotal += value;
			if( numberOfSamples )
				averageValue = msvAccumulatedTotal / numberOfSamples;
			else
				averageValue = msvAccumulatedTotal;

			// Accumulate the Squares of the value
			msvAccumulatedSquares += (value * value);

			// Update the Average value
			msvValue[SV_STATS_AVERAGEOF_VALUES].SetValue( RRunStatus.m_lResultDataIndex, averageValue );

			// increment number of matched occurences
			SVString occurenceValueStr;
			msvOccurenceValue.GetValue( occurenceValueStr );
			if( !occurenceValueStr.empty() )
			{
				value = atof( occurenceValueStr.c_str() );
				if( value == dInputValue )
				{
					msvValue[SV_STATS_NUMBEROF_OCCURANCES].GetValue( count );
					count++;
					msvValue[SV_STATS_NUMBEROF_OCCURANCES].SetValue( RRunStatus.m_lResultDataIndex, count );
				}
			}

			// Set Embedded defaults
			for (int i = SV_STATS_MIN_VALUE; i < SV_STATS_TOPOF_LIST; i = (SVStatisticsFeatureEnum) (i + 1))
			{
				if( FeatureString[i] == '1' )
				{
					switch (i)
					{
						case SV_STATS_MIN_VALUE:
							msvValue[SV_STATS_MIN_VALUE].GetValue( value );
							
							if( numberOfSamples > 1 )
								value = std::min( value, dInputValue );
							else
								value = dInputValue;
							
							msvValue[SV_STATS_MIN_VALUE].SetValue( RRunStatus.m_lResultDataIndex, value );
							break;

						case SV_STATS_MAX_VALUE:
							msvValue[SV_STATS_MAX_VALUE].GetValue( value );

							if( numberOfSamples > 1 )
								value = std::max( value, dInputValue );
							else
								value = dInputValue;
							
							msvValue[SV_STATS_MAX_VALUE].SetValue( RRunStatus.m_lResultDataIndex, value );
							break;

						case SV_STATS_VARIANCEIN_VALUES:
							// Variance is a cumulative measure of the
							// Squares of the difference of all the data values
							// from the Mean
							// using Sample Variance not Population Variance
							if( numberOfSamples > 1 )
							{
								value = calculateVariance( numberOfSamples, averageValue );

								msvValue[SV_STATS_VARIANCEIN_VALUES].SetValue( RRunStatus.m_lResultDataIndex, value );
							}
							break;

						case SV_STATS_STANDARD_DEVIATION:
							// if variance is already calculated
							if( numberOfSamples > 1 )
							{
								if( FeatureString[SV_STATS_VARIANCEIN_VALUES] == '1' )
								{
									msvValue[SV_STATS_VARIANCEIN_VALUES].GetValue( value );
								}
								else
								{
									value = calculateVariance( numberOfSamples, averageValue );
								}
								// Standard Deviation is the positive square root of the Variance
								if( value )
									value = fabs( sqrt( value ) );

								msvValue[SV_STATS_STANDARD_DEVIATION].SetValue( RRunStatus.m_lResultDataIndex, value );
							}
							break;

						case SV_STATS_PERCENTOF_OCCURANCES:
							if( !occurenceValueStr.empty() && numberOfSamples )
							{
								msvValue[SV_STATS_NUMBEROF_OCCURANCES].GetValue( count );
								
								// Calculate percentile
								value = (count / numberOfSamples ) * 100.0;

								msvValue[SV_STATS_PERCENTOF_OCCURANCES].SetValue( RRunStatus.m_lResultDataIndex, value );
							}
							break;
					}
				}
			}

			RRunStatus.SetPassed();
		}
	}

	if( ! l_bOk )
	{
		SetInvalid();
		RRunStatus.SetInvalid();
	}

	return l_bOk;
}

bool SVStatisticsToolClass::Test(SvStl::MessageContainerVector *pErrorMessages)
{
	if( !ValidateLocal(pErrorMessages) )
	{
		return false;
	}

	// verify that the object is really valid
	SVObjectClass* pObject = SVObjectManagerClass::Instance().GetObject( m_inputObjectInfo.GetInputObjectInfo().UniqueObjectID );
	if ( pObject )
	{
		SVValueObjectReference refValueObject = m_inputObjectInfo.GetInputObjectInfo().GetObjectReference();
		if ( refValueObject.Object() )
		{
			double dValue;
			HRESULT hr = refValueObject.GetValue( dValue );
			if ( S_OK == hr || SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE == hr )
			{
				return true;
			}
			else
			{
				if (nullptr != pErrorMessages)
				{
					SVString CompleteName = refValueObject.Object()->GetCompleteObjectNameToObjectType( nullptr, SVInspectionObjectType );
					SVStringVector msgList;
					msgList.push_back( CompleteName );
					SvStl::MessageContainer message;
					message.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_StatToolInvalidVariable, msgList, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10201, GetUniqueObjectID());
					pErrorMessages->push_back( message );
				}
				return false;
			}
		}
	}

	//Test failed, if reach this point.
	if (nullptr != pErrorMessages)
	{
		SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_StatToolTestFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
		pErrorMessages->push_back(Msg);
	}

	return false;
}

bool SVStatisticsToolClass::ValidateLocal(SvStl::MessageContainerVector *pErrorMessages) const
{
	if( HasVariable() )
	{
		if ( !m_inputObjectInfo.IsConnected() || nullptr == m_inputObjectInfo.GetInputObjectInfo().PObject )
		{
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
			return false;
		}
	}

	return true;
}
