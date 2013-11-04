//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVStatisticsTool
//* .File Name       : $Workfile:   SVStatTool.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   13 May 2013 12:37:06  $
//******************************************************************************

#include "stdafx.h"
#include "SVStatTool.h"

#include "SVObjectLibrary/SVObjectManagerClass.h"

#include "SVGetObjectDequeByTypeVisitor.h"
#include "SVGlobal.h"
#include "SVInspectionProcess.h"
#include "SVResultDouble.h"
#include "SVToolSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////
// Error Codes used by this program
///////////////////////////////////////////////////////////
enum
{
	ErrorBase = 15000,
	Err_15001 = (ErrorBase+1),
	Err_15002 = (ErrorBase+2),
	Err_15003 = (ErrorBase+3),
	Err_15004 = (ErrorBase+4),
};

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

	outObjectInfo.ObjectTypeInfo.ObjectType = SVToolObjectType;
	outObjectInfo.ObjectTypeInfo.SubType    = SVStatisticsToolObjectType;
	
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
	isCreated = SVToolClass::CreateObject( PCreateStruct );

	for ( int i = 0; i < SV_NUMBER_OF_STAT_FEATURES; i++ )
	{
		msvValue[i].ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	}

	msvOccurenceValue.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	msvPersistantFeaturesEnabled.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	msvVariableGUID_OBSOLETE.ObjectAttributesAllowedRef() = SV_HIDDEN;
	msvVariableName.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;

	CString strName;
	msvVariableName.GetValue(strName);

	if ( strName.IsEmpty() )
	{
		// check for backwards compatibility
		CString strGuid;
		msvVariableGUID_OBSOLETE.GetValue( strGuid );
		if ( !strGuid.IsEmpty() )
		{
			GUID guid = SVInvalidGUID;
			AfxGetClassIDFromString(strGuid, &guid);
			SVObjectClass* pObject = SVObjectManagerClass::Instance().GetObject(guid);
			if( pObject != NULL )
			{
				strName = pObject->GetCompleteObjectName();
			}
		}
	}

	SetVariableSelected( strName );


	return isCreated;
}

HRESULT SVStatisticsToolClass::ResetObject()
{
	HRESULT hrOk = SVToolClass::ResetObject();
	
	if ( hrOk == S_OK )
	{
		CString strName;
		msvVariableName.GetValue(strName);
		SetVariableSelected( strName );
	}

	return hrOk;
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
			msvError.msvlErrorCd = -Err_15001;
			SV_TRAP_ERROR_BRK (msvError, Err_15001);
		}
		
		Add( pResult );
		
		SVObjectTypeInfoStruct info;
		info.ObjectType = SVDoubleValueObjectType;
		info.EmbeddedID = SVValueObjectGuid;
		
		// Get the output of the result
		SVDoubleValueObjectClass* pValue = 
			(SVDoubleValueObjectClass*)SVSendMessage(pResult, 
			SVM_GETFIRST_OBJECT, 
			NULL, 
			(DWORD)&info);
		
		if (!pValue)
		{
			msvError.msvlErrorCd = -Err_15002;
			SV_TRAP_ERROR_BRK (msvError, Err_15002);
		}
		
		// Make it non visible for any selection
		pValue->ObjectAttributesAllowedRef() &= ( ~SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES );
		
		// Ensure this Object's inputs get connected
		::SVSendMessage( pResult, SVM_CONNECT_ALL_INPUTS, NULL, NULL );
		
		// And last - Create (initialize) it
		
		if( ! pResult->IsCreated() )
		{
			// And finally try to create the child object...
			if( ::SVSendMessage( this, SVM_CREATE_CHILD_OBJECT, ( DWORD ) pResult, NULL ) != SVMR_SUCCESS )
			{
				AfxMessageBox("Creation of Statistics Result Failed");
				
				// Remove it from the Blob Analyzer TaskObjectList ( Destruct it )
				GUID objectID = pResult->GetUniqueObjectID();
				if( objectID != SVInvalidGUID )
					Delete( objectID );
				else
					delete pResult;
				
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
			msvError.msvlErrorCd = -Err_15003;
			SV_TRAP_ERROR_BRK (msvError, Err_15003);
		}
		
		
		::SVSendMessage (this, 
			SVM_DESTROY_CHILD_OBJECT,
			(DWORD) pResult,
			SVMFSetDefaultInputs);
		
		pResult = NULL;
		
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

		if( pResult != NULL )
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
							SVM_DISCONNECT_OBJECT_INPUT, reinterpret_cast <DWORD> (&m_inputObjectInfo), NULL );
		}
	}

	if( !p_strName.IsEmpty() )
	{
		// Get the Object
		SVObjectReference refObject;
#ifdef _DEBUG
		SVValueObjectReference refValueObject;
		HRESULT hrGet = SVObjectManagerClass::Instance().GetObjectByDottedName( static_cast< LPCTSTR >( p_strName ), refValueObject );
		refObject = refValueObject;
#endif
		refObject = SVObjectManagerClass::Instance().GetObjectReference( p_strName );
		if( refObject.Object() )
		{
			// Connect to the input
			m_inputObjectInfo.SetObject( GetObjectInfo() );

			//m_inputObjectInfo.InputObjectInfo.UniqueObjectID = selectedVarGuid;
			m_inputObjectInfo.SetInputObject( refObject );
			
			::SVSendMessage(refObject.Object(), SVM_CONNECT_OBJECT_INPUT, reinterpret_cast <DWORD> (&m_inputObjectInfo), NULL );
		}// end if( refObject.Object() )
		msvVariableName.SetValue(1, p_strName);
	}
	else
	{
		//msvVariableGUID_OBSOLETE.SetValue(1, guidStr);
		msvVariableName.SetValue(1,_T(""));

		// Clear the Object Info
		m_inputObjectInfo.SetInputObject( NULL );
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
				if ( hr == S_OK || hr == SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE )
				{
					bRetVal = TRUE;
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
			bRetVal = m_inputObjectInfo.IsConnected() && m_inputObjectInfo.GetInputObjectInfo().PObject != NULL;
		}
		else
		{
			bRetVal = TRUE;
		}
	}

	if( !bRetVal )
		SetInvalid();

	return bRetVal;
}


BOOL SVStatisticsToolClass::onRun( SVRunStatusClass& RRunStatus )
{
	BOOL l_bOk = SVToolClass::onRun( RRunStatus );

	if( m_inputObjectInfo.IsConnected() && m_inputObjectInfo.GetInputObjectInfo().PObject != NULL )
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

DWORD SVStatisticsToolClass::processMessage( DWORD DwMessageID, DWORD DwMessageValue, DWORD DwMessageContext )
{
	DWORD DwResult = NULL;
	// Try to process message by yourself...
	DWORD dwPureMessageID = DwMessageID & SVM_PURE_MESSAGE;
	switch( dwPureMessageID )
	{
		// is sent in SVIPDoc::Validate() ( old PrepareForRunning() )
	case SVMSGID_RESET_ALL_OBJECTS:
		{
			ResetObject();

			// is this redundant? (OnValidate)
			// call Test()...( Verifys Variable Input !!! )
			if( IsEnabled()  )
			{
				if( HasVariable() && ! Test() )
				{
					ASSERT( FALSE );

					CString tmp,fullObjectName;
					fullObjectName = GetCompleteObjectNameToObjectType( NULL, SVInspectionObjectType );
					tmp.LoadString(IDS_STATTOOL_INVALID_VARIABLE);
					m_errStr.Format(_T( "%s \n %s" ), fullObjectName, tmp );

					DwResult = SVMR_NO_SUCCESS;
				}
				else
				{
					resetValues();

					DwResult = SVMR_SUCCESS;
				}
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
				m_inputObjectInfo.SetInputObject( NULL );

				// Check if variable still exists and is selectable for stats
				if( !HasVariable() )
				{
					// Clear it since the object is gone
					m_inputObjectInfo.SetInputObject( NULL );
					SetVariableSelected( _T("") );	// will this result in a recursive call to this message???
				}
				DwResult = SVMR_SUCCESS;
			}
		}
		break;
	}
	return( SVToolClass::processMessage( DwMessageID, DwMessageValue, DwMessageContext ) | DwResult );
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVStatTool.cpp_v  $
 * 
 *    Rev 1.1   13 May 2013 12:37:06   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 014.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.36   02 May 2013 11:25:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  810
 * SCR Title:  Remove support for Legacy Tools
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated process message method to fix formatting issues and updated the reset object section to only assert when the HRESULT is a failed status.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 15:09:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.35   03 Aug 2012 10:48:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed issues with error number definitions.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.34   30 Jul 2012 13:05:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated the parameter change for the visitor fucntionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.33   27 Jul 2012 09:01:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed include information and updated overlay collection functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.32   12 Jul 2012 15:12:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to add in an additional string identifier for the input and fixed clear functionality assoicated with input structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.31   02 Jul 2012 17:35:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.30   19 Jun 2012 14:19:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Protected Object Information structure attributes.  This change will add accessor methods to get and set attributes.
 * Converted GUID to SVGUID.   This allows the use of standard operators for comparison and methods for conversion.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.29   30 Mar 2011 15:29:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.40 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use STL min/max functions instead of MFC macros.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.28   27 Jan 2011 11:58:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.27   08 Dec 2010 13:35:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.26   28 Oct 2010 14:11:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate definition of HRESULT.  The SVHRESULT definition will be removed in place of the Microsoft defined HRESULT.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.25   01 Jun 2010 15:13:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to improve ability to track performance.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.24   16 Dec 2009 12:54:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.23   20 Jan 2006 15:07:20   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   SV_ALL_ATTRIBUTES was renamed to SV_DEFAULT_ATTRIBUTES
 * added SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES
 * added SV_DEFAULT_IMAGE_OBJECT_ATTRIBUTES
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.22   24 Oct 2005 15:28:50   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added pointer check in create object to prevent crash when connecting to obsolete object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.21   21 Oct 2005 14:04:52   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed un-necessary setting of the IsConnected flag.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.20   21 Sep 2005 11:31:30   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  506
 * SCR Title:  Update Tool Set List Tab to include an indicator when a tool is invalid
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated OnValidate and onRun methods to handle correct tool validation logic.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.19   21 Sep 2005 07:47:20   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects to handle invalid items on the input and output lists.  Methods now verify object validity before use.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.18   31 Aug 2005 11:14:50   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   removed #ifdef _DEBUG for #include "SVInspectionProcess.h"
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.17   30 Aug 2005 14:05:56   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   optimized GetVariableSelected and HasVariable
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.16   10 Aug 2005 13:28:14   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed UpdateTaskObjectOutputListAttributes to call base class
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.15   09 Aug 2005 15:59:24   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   check for old-style configuration data
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.14   03 Aug 2005 14:52:48   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added UpdateTaskObjectOutputListAttributes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.13   29 Jul 2005 13:24:30   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated object initialization to change the parameters for RegisterEmbeddedObject for a SVValueObjectClass.  The two additional parameters will inform the preperation process before toolset execution.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.12   28 Jul 2005 07:55:20   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed ::getInputValue to work with SVObjectReference
 * added more conditions to ::Test
 * cleaned up code
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.11   27 Jul 2005 16:05:56   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   updated StatisticsTool to use SVObjectReference (array value objects) instead of GUIDs
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.10   21 Jun 2005 08:32:08   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   object attributes now use accessor methods
 * value object functions now use HRESULT
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   17 Feb 2005 15:35:24   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed unused function onUpdateFigure( no longer needed.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.8   16 Feb 2005 15:16:26   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   updated header files
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   22 Apr 2003 15:34:26   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   10 Jan 2003 15:55:56   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  305
 * SCR Title:  Implement the ability to perform RunOnce from a SIAC client
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Made changes related to changing the SVDrawObjectClass to work in a consistent manner with SVDrawObjectListClass.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   09 Dec 2002 20:08:10   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  295
 * SCR Title:  Remove Result Data from Configuration Printout
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   CreateObject method was modified to properly update the isCreated flag and to set the printability of the internal value objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   20 Nov 2002 13:29:54   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Removed StatisticsTool Debug
 * Added index to setvalue of value objects.
 * Removed ErrorEvent.PulseEvent from ProcessMessage
 * removed  SVEvent.h
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   10 May 2001 15:55:06   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  197
 * SCR Title:  Fix lock-ups in application when loading configuration from SVFocusNT
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Changed SVObserver state code in:
 * HasVariable
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   09 Jun 2000 10:10:34   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  157
 * SCR Title:  Inspection Hangs after loading and going online
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Wrapped TRACE statement in another conditional compile define.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   03 Mar 2000 13:01:28   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  112
 * SCR Title:  Expose Extents of Images
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to call removeEmbeddedExtents.
 * Revised onUpdateFigure to adjust for changed SVM_GET_EXTENT parameters.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 13:40:12   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   20 Oct 1999 18:11:38   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  39
 * SCR Title:  Integrate Statistics Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised DisableFeatures to notify Objects using this output
 * that it is no longer available.
 * Revised DisableFeatures to reset objectAttributesSet.
 * Revised Handler of Disconnect Message to clear guid
 * of selected varaible if it goes away.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   19 Oct 1999 13:24:18   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  39
 * SCR Title:  Integrate Statistics Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised getNumberOfSamples to add one to enabledCount.
 * (enabledCount gets updated after SVStatToolClass::onRun
 * Revised onRun to update percentage of occurences
 * always if selected and enabled.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   18 Oct 1999 16:02:30   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  39
 * SCR Title:  Integrate Statistics Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to show complete name when displaying error
 * message - IDS_STATTOOL_INVALID_VARIABLE
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   18 Oct 1999 10:20:36   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  39
 * SCR Title:  Integrate Statistics Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised onRun to check if Disabled/Disabled by Condition
 * Revised getNumberOfSamples to use enabledCount
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   08 Oct 1999 10:56:14   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added Call to SVIPDoc::SetDefaultInputs() after sending
 * SVM_DESTROY_CHILD_OBJECT message to refresh
 * dependencies.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   07 Oct 1999 20:04:50   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  39
 * SCR Title:  Integrate Statistics Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised msvOccurencevalue from type double to type
 * String.
 * Added handler for Disconnect of inputs.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   07 Oct 1999 16:16:18   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  39
 * SCR Title:  Integrate Statistics Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added Code to perform the statistical calculations
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   06 Oct 1999 16:35:10   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  39
 * SCR Title:  Integrate Statistics Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised features.
 * Added logic to handle variable to monitor
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   05 Oct 1999 11:13:54   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  39
 * SCR Title:  Integrate Statistics Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin - Statistics Tool
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
