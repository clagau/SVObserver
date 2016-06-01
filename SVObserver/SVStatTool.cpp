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
#include "SVStatusLibrary\MessageManagerResource.h"
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

	CString strName;
	msvVariableName.GetValue(strName);

	if ( strName.IsEmpty() )
	{
		// check for backwards compatibility
		CString strGuid;
		msvVariableGUID_OBSOLETE.GetValue( strGuid );
		if ( !strGuid.IsEmpty() )
		{
			GUID guid = SV_GUID_NULL;
			AfxGetClassIDFromString(strGuid, &guid);
			SVObjectClass* pObject = SVObjectManagerClass::Instance().GetObject(guid);
			if( nullptr != pObject )
			{
				strName = pObject->GetCompleteObjectName();
			}
		}
	}

	SetVariableSelected( strName );

	RestoreFeatureAttributes();

	return m_isCreated;
}

HRESULT SVStatisticsToolClass::ResetObject()
{
	HRESULT Result = SVToolClass::ResetObject();
	
	if ( S_OK == Result )
	{
		CString strName;
		msvVariableName.GetValue(strName);
		SetVariableSelected( strName );

		if( IsEnabled() )
		{
			if( HasVariable() && !Test() )
			{
				Result = -SvOi::Err_25012_StatTool_Test;
			}
			else
			{
				resetValues();
			}
		}
	}

	return Result;
}

CString SVStatisticsToolClass::GetFeatureString()
{
	CString featureStr;
	msvPersistantFeaturesEnabled.GetValue(featureStr);
	return featureStr;
}

CString SVStatisticsToolClass::GetFeatureName( int aIndex )
{
	return msvValue [aIndex ].GetName();
}

/////////////////////////////////////////////////////////////////////////////
//
DWORD SVStatisticsToolClass::EnableFeature (SVStatisticsFeatureEnum aIndex)
{
	msvValue [aIndex].ObjectAttributesAllowedRef() = msvlDefaultAttributes;

	CString featureStr;
	msvPersistantFeaturesEnabled.GetValue(featureStr);

	featureStr.SetAt(aIndex, _T( '1' ) );
	msvPersistantFeaturesEnabled.SetValue(1, featureStr);

	AllocateResult (aIndex);

	return 0;
}

////////////////////////////////////////////////////
// This function restores feature attributesAllowed
// that were previously stored in the configuration.
////////////////////////////////////////////////////
void SVStatisticsToolClass::RestoreFeatureAttributes()
{
	CString featureStr;
	msvPersistantFeaturesEnabled.GetValue(featureStr);
	for( int iFeature = SV_STATS_MIN_VALUE ; iFeature < SV_STATS_TOPOF_LIST ; iFeature++ )
	{
		if (_T('1') == featureStr.GetAt(iFeature))
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

	CString featureStr;
	msvPersistantFeaturesEnabled.GetValue(featureStr);
	featureStr.SetAt( aIndex, _T( '0' ) );
	msvPersistantFeaturesEnabled.SetValue(1, featureStr);

	FreeResult (aIndex);

	// Get this object's outputInfo
	SVOutObjectInfoStruct& valueOutObjectInfo = msvValue [aIndex].GetObjectOutputInfo();

	valueOutObjectInfo.DisconnectAllInputs();

	return 0;
}

DWORD SVStatisticsToolClass::AllocateResult (SVStatisticsFeatureEnum aFeatureIndex)
{
	SVClassInfoStruct       resultClassInfo;
	CString                 strTitle;
	SVObjectTypeInfoStruct  interfaceInfo;

	SVDoubleResultClass*    pResult;
	
	msvError.ClearLastErrorCd ();
	
	while (1)
	{
		// Setup the result
		
		// Declare Input Interface of Result...
		interfaceInfo.EmbeddedID = msvValue [aFeatureIndex].GetEmbeddedID();
		resultClassInfo.DesiredInputInterface.Add( interfaceInfo );
		
		resultClassInfo.ObjectTypeInfo.ObjectType = SVResultObjectType;
		resultClassInfo.ObjectTypeInfo.SubType	= SVResultDoubleObjectType;
		resultClassInfo.ClassId = SVDoubleResultClassGuid;
		resultClassInfo.ClassName.LoadString( IDS_OBJECTNAME_RESULT );
		strTitle = msvValue [aFeatureIndex].GetName(); //.LoadString( IDS_CLASSNAME_RESULT_DOUBLE );
		resultClassInfo.ClassName += SV_TSTR_SPACE + strTitle;
		
		// Construct the result class
		pResult = (SVDoubleResultClass *) resultClassInfo.Construct();
		
		if(!pResult)
		{
			msvError.msvlErrorCd = -SvOi::Err_15001;
			SV_TRAP_ERROR_BRK (msvError, SvOi::Err_15001);
		}
		
		Add( pResult );
		
		SVObjectTypeInfoStruct info;
		info.ObjectType = SVDoubleValueObjectType;
		info.EmbeddedID = SVValueObjectGuid;
		
		// Get the output of the result
		SVDoubleValueObjectClass* pValue = 
			reinterpret_cast<SVDoubleValueObjectClass*>(SVSendMessage(pResult, 
			SVM_GETFIRST_OBJECT, 
			0, 
			reinterpret_cast<DWORD_PTR>(&info)));
		
		if (!pValue)
		{
			msvError.msvlErrorCd = -SvOi::Err_15002;
			SV_TRAP_ERROR_BRK (msvError, SvOi::Err_15002);
		}
		
		// Make it non visible for any selection
		pValue->ObjectAttributesAllowedRef() &= ( ~SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES );
		
		// Ensure this Object's inputs get connected
		::SVSendMessage( pResult, SVM_CONNECT_ALL_INPUTS, 0, 0 );
		
		// And last - Create (initialize) it
		
		if( ! pResult->IsCreated() )
		{
			// And finally try to create the child object...
			if( ::SVSendMessage( this, SVM_CREATE_CHILD_OBJECT, reinterpret_cast<DWORD_PTR>(pResult), 0 ) != SVMR_SUCCESS )
			{
				SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
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
	
	return msvError.GetLastErrorCd ();
}

DWORD SVStatisticsToolClass::FreeResult (SVStatisticsFeatureEnum aFeatureIndex)
{
	SVResultClass*    pResult;
	
	msvError.ClearLastErrorCd ();
	
	
	while (1)
	{
		pResult = GetResultObject (aFeatureIndex);
		
		if (!pResult)
		{
			msvError.msvlErrorCd = -SvOi::Err_15003;
			SV_TRAP_ERROR_BRK (msvError, SvOi::Err_15003);
		}
		
		
		::SVSendMessage (this, 
			SVM_DESTROY_CHILD_OBJECT,
			reinterpret_cast<DWORD_PTR>(pResult),
			SVMFSetDefaultInputs);
		
		pResult = nullptr;
		
		break;
	}
	
	return msvError.GetLastErrorCd ();
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
	
	
	msvError.ClearLastErrorCd ();
	
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

CString SVStatisticsToolClass::GetOccurenceTestValue()
{
	CString value;

	msvOccurenceValue.GetValue( value );
	return value;
}

void SVStatisticsToolClass::SetOccurenceTestValue( CString value )
{
	msvOccurenceValue.SetValue( 1, value );
}

SVObjectReference SVStatisticsToolClass::GetVariableSelected()
{
	CString strName;
	msvVariableName.GetValue( strName );

	SVObjectReference refObject;

	if( ! strName.IsEmpty() )
	{
		// much better! (~50 µs)
		SVValueObjectReference refValueObject;
		HRESULT hr = SVObjectManagerClass::Instance().GetObjectByDottedName( static_cast< LPCTSTR >( strName ), refValueObject );
		refObject = refValueObject;
	}
			
	return refObject;
}

void SVStatisticsToolClass::SetVariableSelected( SVObjectReference p_refObject )
{
	CString strName = p_refObject.GetCompleteObjectName();
	SetVariableSelected( strName );
}

void SVStatisticsToolClass::SetVariableSelected( CString p_strName )
{
	if( HasVariable() )
	{
		if( m_inputObjectInfo.IsConnected() && m_inputObjectInfo.GetInputObjectInfo().PObject )
		{
			::SVSendMessage(m_inputObjectInfo.GetInputObjectInfo().PObject,
							SVM_DISCONNECT_OBJECT_INPUT, reinterpret_cast <DWORD_PTR> (&m_inputObjectInfo), 0 );
		}
	}

	if( !p_strName.IsEmpty() )
	{
		// Get the Object
		SVObjectReference refObject;
		HRESULT hrGet = SVObjectManagerClass::Instance().GetObjectByDottedName( static_cast< LPCTSTR >( p_strName ), refObject );
		if( refObject.Object() )
		{
			// Connect to the input
			m_inputObjectInfo.SetObject( GetObjectInfo() );

			//m_inputObjectInfo.InputObjectInfo.UniqueObjectID = selectedVarGuid;
			m_inputObjectInfo.SetInputObject( refObject );
			
			::SVSendMessage(refObject.Object(), SVM_CONNECT_OBJECT_INPUT, reinterpret_cast <DWORD_PTR> (&m_inputObjectInfo), 0 );
		}// end if( refObject.Object() )
		msvVariableName.SetValue(1, p_strName);
	}
	else
	{
		//msvVariableGUID_OBSOLETE.SetValue(1, guidStr);
		msvVariableName.SetValue(1,_T(""));

		// Clear the Object Info
		m_inputObjectInfo.SetInputObject( nullptr );
	}
}

void SVStatisticsToolClass::UpdateTaskObjectOutputListAttributes()
{
	SVToolClass::UpdateTaskObjectOutputListAttributes( m_inputObjectInfo.GetInputObjectInfo().GetObjectReference(), SV_SELECTABLE_FOR_STATISTICS );
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

BOOL SVStatisticsToolClass::HasVariable()
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

BOOL SVStatisticsToolClass::Test()
{
	BOOL bRetVal = FALSE;

	if( m_inputObjectInfo.IsConnected() && m_inputObjectInfo.GetInputObjectInfo().PObject )
	{
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
					bRetVal = TRUE;
				}
				else
				{
					CString fullObjectName = refValueObject.Object()->GetCompleteObjectNameToObjectType( nullptr, SVInspectionObjectType );
					SVStringArray msgList;
					msgList.push_back(SVString(fullObjectName));
					m_errContainer.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_StatToolInvalidVariable, msgList, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10201);
				}
			}
		}
	}

	return bRetVal;
}

BOOL SVStatisticsToolClass::OnValidate()
{
	BOOL bRetVal = SVToolClass::OnValidate();

	if( bRetVal )
	{
		if( HasVariable() )
		{
			bRetVal = m_inputObjectInfo.IsConnected() && nullptr != m_inputObjectInfo.GetInputObjectInfo().PObject;
		}
		else
		{
			bRetVal = true;
		}
	}

	if( !bRetVal )
		SetInvalid();

	return bRetVal;
}


BOOL SVStatisticsToolClass::onRun( SVRunStatusClass& RRunStatus )
{
	BOOL l_bOk = SVToolClass::onRun( RRunStatus );

	if( m_inputObjectInfo.IsConnected() && nullptr != m_inputObjectInfo.GetInputObjectInfo().PObject )
	{
		if( !RRunStatus.IsDisabled() && !RRunStatus.IsDisabledByCondition() )
		{
			// check which result to calculate
			CString featureStr;
			msvPersistantFeaturesEnabled.GetValue(featureStr);

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
			CString occurenceValueStr;
			msvOccurenceValue.GetValue( occurenceValueStr );
			if( !occurenceValueStr.IsEmpty() )
			{
				value = atof( occurenceValueStr );
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
				if( featureStr[i] == _T( '1' ) )
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
								if( featureStr[SV_STATS_VARIANCEIN_VALUES] == _T( '1' ) )
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
							if( !occurenceValueStr.IsEmpty() && numberOfSamples )
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

DWORD_PTR SVStatisticsToolClass::processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext )
{
	DWORD_PTR DwResult = SVMR_NOT_PROCESSED;
	// Try to process message by yourself...
	DWORD dwPureMessageID = DwMessageID & SVM_PURE_MESSAGE;
	switch( dwPureMessageID )
	{
		// is sent in SVIPDoc::Validate()
	case SVMSGID_RESET_ALL_OBJECTS:
		{
			HRESULT ResetStatus = ResetObject();
			if( S_OK != ResetStatus )
			{
				BOOL SilentReset = static_cast<BOOL> (DwMessageValue);

				if( !SilentReset && 0 != m_errContainer.getMessage().m_MessageCode )
				{
					SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
					Msg.setMessage( m_errContainer.getMessage() ); 
				}
				DwResult = SVMR_NO_SUCCESS;
			}
			else
			{
				DwResult = SVMR_SUCCESS;
			}
		}
		break;

	case SVMSGID_DISCONNECT_OBJECT_INPUT:
		{
			// ...use second message parameter ( DwMessageValue ) as pointer to InObjectInfo ( SVInObjectInfoStruct* )
			// ...returns SVMR_SUCCESS, SVMR_NO_SUCCESS or SVMR_NOT_PROCESSED
			SVInObjectInfoStruct* pInObjectInfo = ( SVInObjectInfoStruct* ) DwMessageValue;
			if( pInObjectInfo && pInObjectInfo->GetInputObjectInfo().UniqueObjectID == m_inputObjectInfo.GetInputObjectInfo().UniqueObjectID )
			{
				m_inputObjectInfo.SetInputObject( nullptr );

				// Check if variable still exists and is selectable for stats
				if( !HasVariable() )
				{
					// Clear it since the object is gone
					m_inputObjectInfo.SetInputObject( nullptr );
					SetVariableSelected( _T("") );	// will this result in a recursive call to this message???
				}
				DwResult = SVMR_SUCCESS;
			}
		}
		break;
	}
	return( SVToolClass::processMessage( DwMessageID, DwMessageValue, DwMessageContext ) | DwResult );
}

