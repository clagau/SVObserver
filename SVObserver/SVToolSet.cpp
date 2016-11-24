//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolSet.cpp
//* .File Name       : $Workfile:   SVToolSet.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.9  $
//* .Check In Date   : $Date:   07 Jan 2015 17:52:44  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVToolSet.h"

#include "SVMessage/SVMessage.h"
#include "SVObjectLibrary/SVInspectionLevelCreateStruct.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"

#include "SVAnalyzer.h"
#include "SVConditional.h"
#include "SVOCore/SVImageListClass.h"
#include "SVInspectionProcess.h"
#include "SVSVIMStateClass.h"
#include "SVTool.h"
#include "SVTimerLibrary/SVClock.h"
#include "SVColorTool.h"
#pragma endregion Includes

SV_IMPLEMENT_CLASS( SVToolSetClass, SVToolSetClassGuid );

SVToolSetClass::SVToolSetClass( BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID )
			   :SVTaskObjectListClass( BCreateDefaultTaskList, POwner, StringResourceID )
{
	init();
}

void SVToolSetClass::init()
{
	// Identify our output type
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVToolSetObjectType;

	// Identify our input type needs

	// SetObjectDepth() already called in SVObjectClass Ctor

	// Register Embedded Objects
	RegisterEmbeddedObject( &m_Enabled, SVToolSetEnabledObjectGuid, IDS_OBJECTNAME_ENABLED, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_MainImageObject, SVOutputImageObjectGuid, IDS_OBJECTNAME_IMAGE1 );

	RegisterEmbeddedObject( &m_Passed, SVPassedObjectGuid, IDS_OBJECTNAME_PASSED, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_Failed, SVFailedObjectGuid, IDS_OBJECTNAME_FAILED, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_Warned, SVWarnedObjectGuid, IDS_OBJECTNAME_WARNED, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_ExplicitFailed, SVExplicitFailedObjectGuid, IDS_OBJECTNAME_EXPLICIT_FAILED, false, SVResetItemNone );

	RegisterEmbeddedObject( &m_PassedCount, SVPassedCountObjectGuid, IDS_OBJECTNAME_PASSED_COUNT, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_FailedCount, SVFailedCountObjectGuid, IDS_OBJECTNAME_FAILED_COUNT, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_WarnedCount, SVWarnedCountObjectGuid, IDS_OBJECTNAME_WARNED_COUNT, false, SVResetItemNone );

	RegisterEmbeddedObject( &m_EnabledCount, SVEnabledCountObjectGuid, IDS_OBJECTNAME_ENABLED_COUNT, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_ProcessedCount, SVProcessedCountObjectGuid, IDS_OBJECTNAME_PROCESSED_COUNT, false, SVResetItemNone );

	RegisterEmbeddedObject ( &m_ToolTime, SVToolTimeGuid, IDS_OBJECTNAME_TOOLTIME, false, SVResetItemNone );
	RegisterEmbeddedObject ( &m_MinToolsetTime, SVMinToolSetTimeGuid, IDS_OBJECTNAME_MINTOOLSETTIME, false, SVResetItemNone );
	RegisterEmbeddedObject ( &m_MaxToolsetTime, SVMaxToolSetTimeGuid, IDS_OBJECTNAME_MAXTOOLSETTIME, false, SVResetItemNone );

	RegisterEmbeddedObject( &m_RegressionTestMode, SVRegressionTestModeGuid, IDS_OBJECTNAME_REGRESSIONTESTMODE, false, SVResetItemNone );

	RegisterEmbeddedObject( &m_DrawFlag, SVConditionalToolSetDrawFlagObjectGuid, IDS_OBJECTNAME_DRAWTOOL_FLAG, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_ResetCounts, SVResetInspectionCountsGuid, IDS_OBJECTNAME_RESET_COUNTS, false, SVResetItemIP );
	RegisterEmbeddedObject( &m_TriggerCount, SVTriggerCountGuid, IDS_OBJECTNAME_TRIGGER_COUNT, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_latestCompletionPPQIndex, SVLatestCompletionPPQIndexGuid, IDS_LATEST_PPQ_INDEX_AT_COMPLETION, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_TriggerDelta, SVTriggerDeltaGuid, IDS_TRIGGER_DELTA, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_LastTriggerToPPQCompletion, SVLastTriggerToPPQCompletionGuid, IDS_TRIGGER_TO_COMPLETION_TIME, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_LastTriggerToStart, SVLastTriggerToStartGuid, IDS_TRIGGER_TO_START_TIME, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_Width, SVExtentWidthObjectGuid, IDS_OBJECTNAME_EXTENT_WIDTH, false, SVResetItemTool );
	RegisterEmbeddedObject( &m_Height, SVExtentHeightObjectGuid, IDS_OBJECTNAME_EXTENT_HEIGHT, false, SVResetItemTool );
	

	// Set Embedded defaults
	m_Enabled.SetDefaultValue( true, true );
	m_Passed.SetDefaultValue( false, true );			// Default for Passed is FALSE !!!
	m_Failed.SetDefaultValue( true, true );			// Default for Failed is TRUE !!!
	m_Warned.SetDefaultValue( true, true );			// Default for Warned is TRUE !!!
	m_ExplicitFailed.SetDefaultValue( false, true );	// Default for Explicit Failed is FALSE !!!

	m_PassedCount.SetDefaultValue( 0, true );
	m_FailedCount.SetDefaultValue( 0, true );
	m_WarnedCount.SetDefaultValue( 0, true );

	m_EnabledCount.SetDefaultValue( 0, true );
	m_ProcessedCount.SetDefaultValue( 0, true );

	m_ToolTime.SetDefaultValue(0, true);
	m_ToolTime.SetName( "Tool Set Time" );

	m_bResetMinMaxToolsetTime = true;

	m_MinToolsetTime.SetDefaultValue( 0, true );
	m_MaxToolsetTime.SetDefaultValue( 0, true );

	m_RegressionTestMode.SetDefaultValue(false, true);

	m_DrawFlag.SetEnumTypes( IDS_TOOLSETDRAW_ENUMOBJECT_LIST );
	m_DrawFlag.SetDefaultValue( ( const long ) 0, true ); // 0 Should be show 'All Tools'
	m_TriggerCount.SetDefaultValue( 0, true );
	m_latestCompletionPPQIndex.SetDefaultValue( 0, true );
	m_TriggerDelta.SetDefaultValue( 0, true );
	m_LastTriggerToPPQCompletion.SetDefaultValue( 0, true );
	m_LastTriggerToStart.SetDefaultValue( 0, true );

	m_Width.SetDefaultValue( 0.0, true );
	m_Height.SetDefaultValue( 0.0, true );

	// Set local defaults
	m_StartTime = 0.0;
	m_EndTime = 0.0;
	m_AverageTime = 0.0;
	m_ProcessTime = 0.0;
	m_SetNumber = 0;
	m_isCreated = false;

	SVConditionalClass* l_pConditional = new SVConditionalClass( this );
	AddFriend( l_pConditional->GetUniqueObjectID() );

	// Identify our input type needs
	inputConditionBoolObjectInfo.SetInputObjectType( SVConditionalResultObjectGuid, SVBoolValueObjectType );
	inputConditionBoolObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputConditionBoolObjectInfo, _T( "ToolSetConditionalValue" ) );

	// Set default inputs and outputs
	// Note:: Call the Derived Class (this) here
	addDefaultInputObjects();
	m_ResultList.SetToolSet(this);
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : ~SVToolSetClass
// -----------------------------------------------------------------------------
// .Description : Standard destructor of class SVToolSetClass
////////////////////////////////////////////////////////////////////////////////
SVToolSetClass::~SVToolSetClass()
{
	Destroy();
}

////////////////////////////////////////////////////////////////////////////////
// Create Operator
////////////////////////////////////////////////////////////////////////////////
BOOL SVToolSetClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = SVTaskObjectListClass::CreateObject( PCreateStructure );

	// Set / Reset Printable Flags
	m_Enabled.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	m_Passed.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_Failed.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_Warned.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_ExplicitFailed.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_PassedCount.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_FailedCount.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_WarnedCount.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_EnabledCount.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_ProcessedCount.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_RegressionTestMode.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	m_DrawFlag.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	m_ToolTime.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_ResetCounts.ObjectAttributesAllowedRef() = SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE | SV_EMBEDABLE;
	m_TriggerCount.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_latestCompletionPPQIndex.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_TriggerDelta.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_LastTriggerToPPQCompletion.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_LastTriggerToStart.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;

	m_MinToolsetTime.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_MaxToolsetTime.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_Width.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_Height.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;

	m_MainImageObject.ObjectAttributesAllowedRef() |= SV_REMOTELY_SETABLE;

	m_isCreated = bOk;

	return bOk;
}

////////////////////////////////////////////////////////////////////////////////
// Destroy Operator
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// .Title       : Destroy All Array Member operator of class SVToolSetClass
// -----------------------------------------------------------------------------
// .Description : Destroys all array members
////////////////////////////////////////////////////////////////////////////////
void SVToolSetClass::Destroy()
{
	// Delete our friends
	DestroyFriends();

	m_isCreated = false;

	DeleteAll();
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : Check the Enabled State
//              : SVToolSetClass
// -----------------------------------------------------------------------------
// .Description : Checks the Enabled state of the SVToolSetClass Object
//              : SVToolClass* argument
////////////////////////////////////////////////////////////////////////////////
bool SVToolSetClass::IsEnabled() const
{
	bool bEnabled = true;
	m_Enabled.GetValue( bEnabled );
	return bEnabled;
}

bool SVToolSetClass::IsEnabled(long p_lIndex) const
{
	bool bEnabled = true;
	m_Enabled.GetValue( p_lIndex, bEnabled );
	return bEnabled;
}

bool SVToolSetClass::WasEnabled() const
{
	bool bEnabled = IsEnabled();

	if (bEnabled)
	{
		bEnabled = getConditionalResult();
	}

	return bEnabled;
}

void SVToolSetClass::moveTool( int NewIndex, SVToolClass* pTool )
{
	int Index = GetIndex( pTool );
	if( -1 != Index && NewIndex != Index )
	{
		//Remove first because inserting causes list to change!
		m_aTaskObjects.RemoveAt(Index);
		m_aTaskObjects.InsertAt( NewIndex, ( SVTaskObjectClass* ) pTool );
		m_LastListUpdateTimestamp = SVClock::GetTimeStamp();
	}
}

////////////////////////////////////////////////////////////////////////////////
// Called by CreateObject
////////////////////////////////////////////////////////////////////////////////
void SVToolSetClass::SetDefaultInputs()
{
	// Connects all not valid ( Invalid ) input objects to default...

	// Try to get all inputs and outputs...
	GetAllInputObjects();
	ConnectAllInputs();

	// Rebuild ResultList from the toolset level
	m_ResultList.Refresh( this );
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : Get Index Of Given Tool member function of class 
//              : SVToolSetClass
// -----------------------------------------------------------------------------
// .Description : Returns the index of the given Tool argument 
////////////////////////////////////////////////////////////////////////////////
int SVToolSetClass::GetIndex( SVToolClass* PTool )
{
	register int i = GetSize() - 1;
	for( ; i >= 0 && GetAt( i ) != PTool; --i );

	return i;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : Get Current Result List member function of class
//              : SVToolSetClass
// -----------------------------------------------------------------------------
// .Description : Returns a pointer to the current SVResultListClass object
////////////////////////////////////////////////////////////////////////////////
SVResultListClass* SVToolSetClass::GetResultList()
{
	return &m_ResultList;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : Get Current Image member function of class
//              : SVToolSetClass
// -----------------------------------------------------------------------------
// .Description : Returns a pointer to the current SVImageClass object
////////////////////////////////////////////////////////////////////////////////
SVImageClass* SVToolSetClass::getCurrentImage()
{
	return &m_MainImageObject;
}

bool SVToolSetClass::getConditionalResult(long p_lIndex) const
{
	bool value = false;
	SVBoolValueObjectClass* pBoolObject;

	if( inputConditionBoolObjectInfo.IsConnected() &&
		inputConditionBoolObjectInfo.GetInputObjectInfo().PObject )
	{
		pBoolObject = ( SVBoolValueObjectClass* )inputConditionBoolObjectInfo.GetInputObjectInfo().PObject;
		pBoolObject->GetValue( p_lIndex, value );
	}
	return value;
}

bool SVToolSetClass::getConditionalResult() const
{
	bool value = false;
	SVBoolValueObjectClass* pBoolObject;

	if( inputConditionBoolObjectInfo.IsConnected() &&
		inputConditionBoolObjectInfo.GetInputObjectInfo().PObject )
	{
		pBoolObject = ( SVBoolValueObjectClass* )inputConditionBoolObjectInfo.GetInputObjectInfo().PObject;
		pBoolObject->GetValue( value );
	}
	return value;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : GetDrawFlagObject
// -----------------------------------------------------------------------------
// .Description : Returns pointer to enumeration value object which is used as 
//				: conditional tool set draw flag.
////////////////////////////////////////////////////////////////////////////////
SVEnumerateValueObjectClass* SVToolSetClass::GetDrawFlagObject()
{
	return &m_DrawFlag;
}

SVConditionalClass* SVToolSetClass::GetToolSetConditional() const
{
	SVConditionalClass* l_pConditional( nullptr );

	for( size_t j = 0; nullptr == l_pConditional && j < m_friendList.size(); j++ )
	{
		l_pConditional = dynamic_cast<SVConditionalClass *>(m_friendList[j].PObject);
	}// end for

	return l_pConditional; 
}

void SVToolSetClass::GetToolIds( SVToolIdDeque& p_rToolIds ) const
{
	p_rToolIds.clear();

	for( SVTaskObjectPtrVector::const_iterator l_Iter = m_aTaskObjects.begin(); l_Iter != m_aTaskObjects.end(); ++l_Iter )
	{
		SVTaskObjectClass* l_pTask = ( *l_Iter );

		if( nullptr != l_pTask )
		{
			p_rToolIds.push_back( l_pTask->GetUniqueObjectID() );
		}
	}
}

HRESULT SVToolSetClass::getResetCounts( bool& rResetCounts )
{
	return m_ResetCounts.GetValue( rResetCounts );
}

void SVToolSetClass::goingOffline()
{
	for( SVTaskObjectPtrVector::const_iterator l_Iter = m_aTaskObjects.begin(); l_Iter != m_aTaskObjects.end(); ++l_Iter )
	{
		(*l_Iter)->goingOffline();
	}
}

#pragma region virtual method (IToolSet)
bool SVToolSetClass::IsToolPreviousToSelected( const SVGUID& p_rToolID ) const
{
	bool l_Status = false;
	SVToolSetClass::SVToolIdDeque l_ToolIds;
	GetToolIds( l_ToolIds );

	for( SVToolSetClass::SVToolIdDeque::const_iterator l_Iter = l_ToolIds.begin(); !l_Status && l_Iter != l_ToolIds.end(); ++l_Iter )
	{
		l_Status = ( p_rToolID == ( *l_Iter ) );
	}

	return l_Status;
}

SvOi::IObjectClass* SVToolSetClass::getBand0Image() const
{
	SvOi::IObjectClass* pResult( nullptr );

	for( SVTaskObjectPtrVector::const_iterator Iter( m_aTaskObjects.begin() ); Iter != m_aTaskObjects.end(); ++Iter )
	{
		SVColorToolClass* pColorTool = dynamic_cast<SVColorToolClass*>( *Iter );
		if (nullptr != pColorTool)
		{
			pResult = dynamic_cast<SvOi::IObjectClass*> (pColorTool->getBand0Image());
			return pResult;
		}
	}
	return pResult;
}
#pragma endregion virtual method (IToolSet)

BOOL SVToolSetClass::OnValidate()
{
	BOOL bRetVal = false;

	//	if( mainImageObject )

	// check that we have a main image and the Equation Class is created
	if( inputConditionBoolObjectInfo.IsConnected() &&
		inputConditionBoolObjectInfo.GetInputObjectInfo().PObject )
	{
		bRetVal = true;
		bRetVal = SVTaskObjectListClass::OnValidate() && bRetVal;
	}

	if( !bRetVal )
	{
		SetInvalid();
	}

	return bRetVal;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : onRun member function of class SVToolSetClass
// -----------------------------------------------------------------------------
// .Description : runs this toolset
////////////////////////////////////////////////////////////////////////////////
BOOL SVToolSetClass::onRun( SVRunStatusClass& RRunStatus )
{
	m_TriggerCount.SetValue( RRunStatus.m_lResultDataIndex, RRunStatus.m_lTriggerCount > 0 ? RRunStatus.m_lTriggerCount : 0 );

	m_latestCompletionPPQIndex.SetValue( RRunStatus.m_lResultDataIndex, RRunStatus.m_WorkloadInfoRsc.m_PPQIndexAtCompletion);

	//before calculating the trigger delta: make sure both trigger times are valid:
	bool bothTriggersValid = (RRunStatus.m_CurrentTriggerTime * RRunStatus.m_PreviousTriggerTime > 0.1);
	double TriggerDelta_us = bothTriggersValid ? (SVClock::c_MicrosecondsPerMillisecond * (RRunStatus.m_CurrentTriggerTime - RRunStatus.m_PreviousTriggerTime)) : 0.0;
	m_TriggerDelta.SetValue( RRunStatus.m_lResultDataIndex, TriggerDelta_us);

	m_LastTriggerToPPQCompletion.SetValue( RRunStatus.m_lResultDataIndex, RRunStatus.m_WorkloadInfoRsc.TriggerToCompletionInMilliseconds() * SVClock::c_MicrosecondsPerMillisecond);
	m_LastTriggerToStart.SetValue( RRunStatus.m_lResultDataIndex, RRunStatus.m_WorkloadInfoRsc.TriggerToStartInMilliseconds() * SVClock::c_MicrosecondsPerMillisecond);

	BOOL bRetVal = SVTaskObjectListClass::onRun( RRunStatus );
	if( bRetVal )
	{
		// Friends were running, validation was successfully
		// Check conditional execution
		if ( !getConditionalResult( RRunStatus.m_lResultDataIndex ) )
		{
			RRunStatus.SetDisabledByCondition();
		}
	}
	return bRetVal;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : Run
// -----------------------------------------------------------------------------
// .Description : To execute the Run process for all Tools in this Tool Set.
//				: This Function returns TRUE, if the Tool Set were running.
//				: Otherwise it returns FALSE, that means: if the Tool Set should
//				: not run, because the Tool Set Condition failed!
////////////////////////////////////////////////////////////////////////////////
BOOL SVToolSetClass::Run( SVRunStatusClass& RRunStatus )
{
	long lCount;
	BOOL bRetVal = TRUE;
	BOOL bIsValid = FALSE;
	BOOL bDisabled = FALSE;

	if( !dynamic_cast<SVInspectionProcess*>(GetInspection())->GetNewDisableMethod() )
	{
		SVRunStatusClass ToolRunStatus;

		ToolRunStatus.m_lResultDataIndex  = RRunStatus.m_lResultDataIndex;
		ToolRunStatus.Images = RRunStatus.Images;
		ToolRunStatus.m_UpdateCounters = RRunStatus.m_UpdateCounters;

		// First Set ToolSet Invalid, if other things succeed, it will become valid
		// Do the same for the counts
		m_isObjectValid.GetValue( bIsValid );
		m_isObjectValid.SetValue( RRunStatus.m_lResultDataIndex, bIsValid );
		m_PassedCount.GetValue( lCount );
		m_PassedCount.SetValue( RRunStatus.m_lResultDataIndex, lCount );
		m_FailedCount.GetValue( lCount );
		m_FailedCount.SetValue( RRunStatus.m_lResultDataIndex, lCount );
		m_WarnedCount.GetValue( lCount );
		m_WarnedCount.SetValue( RRunStatus.m_lResultDataIndex, lCount );
		m_ProcessedCount.GetValue( lCount );
		m_ProcessedCount.SetValue( RRunStatus.m_lResultDataIndex, lCount );
		m_EnabledCount.GetValue( lCount );
		m_EnabledCount.SetValue( RRunStatus.m_lResultDataIndex, lCount );

		// If Conditional is disabled equation.Run() returns always TRUE.
		// Otherwise the return value depends on the Conditional equation result!
		if( IsEnabled() )
		{
			++m_SetNumber;

			SVClock::SVTimeStamp l_Timer = SVClock::GetTimeStamp();

			m_ToolTime.Start();

			if( RRunStatus.m_UpdateCounters )
			{
				// Set Processed Count...
				long lCount = 0;
				m_ProcessedCount.GetValue( lCount );
				m_ProcessedCount.SetValue( RRunStatus.m_lResultDataIndex, ++lCount );
			}

			// Run yourself...
			bRetVal = onRun( RRunStatus );

			// if disabled or disabled by condition
			// leave in previous state
			if( !RRunStatus.IsDisabled() && !RRunStatus.IsDisabledByCondition() )
			{
				if( RRunStatus.m_UpdateCounters )
				{
					// Set Enabled Count...
					long lCount = 0;
					m_EnabledCount.GetValue( lCount );
					m_EnabledCount.SetValue( RRunStatus.m_lResultDataIndex, ++lCount );
				}

				// Run your children...
				for( int i = 0; i < GetSize(); i++ )
				{
					if( GetAt( i ) )
					{
						ToolRunStatus.ResetRunStateAndToolSetTimes();
						ToolRunStatus.m_UpdateCounters = RRunStatus.m_UpdateCounters;

						bRetVal = GetAt( i )->Run( ToolRunStatus ) && bRetVal;

						// Update the ToolSet Run Status
						if( ToolRunStatus.IsWarned() )
							RRunStatus.SetWarned();
						if( ToolRunStatus.IsFailed() )
							RRunStatus.SetFailed();
						if( ToolRunStatus.IsPassed() )
							RRunStatus.SetPassed();
						if( ToolRunStatus.IsCriticalFailure() )
							RRunStatus.SetCriticalFailure();
					}
				}
			}

			// Set ToolSet Valid
			m_isObjectValid.SetValue( RRunStatus.m_lResultDataIndex, true );

			// set our state according to the runStatus
			// RRunStatus.SetValid();

			SVClock::SVTimeStamp l_Elapsed = ( SVClock::GetTimeStamp() - l_Timer );
			m_EndTime = SVClock::ConvertTo( SVClock::Seconds, l_Elapsed );
			m_AverageTime = ( m_AverageTime + m_EndTime ) / 2.0;

			if( 0 < m_SetNumber )
			{
				RRunStatus.m_ToolSetEndTime = m_EndTime;	// put units in seconds
				RRunStatus.m_ToolSetAvgTime = m_AverageTime;
			}
			else
			{
				RRunStatus.m_ToolSetEndTime = -1.0;
				RRunStatus.m_ToolSetAvgTime = -1.0;
			}

			if( RRunStatus.m_UpdateCounters )
			{
				m_ToolTime.Stop( RRunStatus.m_lResultDataIndex );

				__int64 l_i64Time = -1;

				m_ToolTime.GetValue( l_i64Time );

				__int64 l_i64MinTime = -1;

				m_MinToolsetTime.GetValue( l_i64MinTime );

				if( m_bResetMinMaxToolsetTime || l_i64MinTime < 0 || l_i64Time < l_i64MinTime )
				{
					m_MinToolsetTime.SetValue( RRunStatus.m_lResultDataIndex, l_i64Time );
				}
				else
				{
					m_MinToolsetTime.SetValue( RRunStatus.m_lResultDataIndex, l_i64MinTime );
				}

				__int64 l_i64MaxTime = -1;

				m_MaxToolsetTime.GetValue( l_i64MaxTime );

				if( m_bResetMinMaxToolsetTime || l_i64MaxTime < 0 || l_i64MaxTime < l_i64Time )
				{
					m_MaxToolsetTime.SetValue( RRunStatus.m_lResultDataIndex, l_i64Time );
				}
				else
				{
					m_MaxToolsetTime.SetValue( RRunStatus.m_lResultDataIndex, l_i64MaxTime );
				}

				if( m_bResetMinMaxToolsetTime )
				{
					m_bResetMinMaxToolsetTime = false;
				}
			}
		}

		// if disabled or disabled by condition
		// leave in previous state
		if( !RRunStatus.IsDisabled() && !RRunStatus.IsDisabledByCondition() )
		{
			// set our state according to the runStatus
			m_Passed.SetValue( RRunStatus.m_lResultDataIndex, RRunStatus.IsPassed() );
			m_Failed.SetValue( RRunStatus.m_lResultDataIndex, RRunStatus.IsFailed() );
			m_ExplicitFailed.SetValue( RRunStatus.m_lResultDataIndex, RRunStatus.IsFailed() );
			m_Warned.SetValue( RRunStatus.m_lResultDataIndex, RRunStatus.IsWarned() );

			if( RRunStatus.m_UpdateCounters )
			{
				// Set Counts...
				long lCount = 0;
				if( RRunStatus.IsPassed() )
				{
					m_PassedCount.GetValue( lCount );
					m_PassedCount.SetValue( RRunStatus.m_lResultDataIndex, ++lCount );
				}
				lCount = 0;
				if( RRunStatus.IsFailed() )
				{
					m_FailedCount.GetValue( lCount );
					m_FailedCount.SetValue( RRunStatus.m_lResultDataIndex, ++lCount );
				}
				lCount = 0;
				if( RRunStatus.IsWarned() )
				{
					m_WarnedCount.GetValue( lCount );
					m_WarnedCount.SetValue( RRunStatus.m_lResultDataIndex, ++lCount );
				}
			}
		}

		// Get Status Color...
		DWORD dwValue = RRunStatus.GetStatusColor();
		m_statusColor.SetValue( RRunStatus.m_lResultDataIndex, dwValue );

		// Get Status...
		dwValue = RRunStatus.GetState();
		m_statusTag.SetValue( RRunStatus.m_lResultDataIndex, dwValue );
	}// end if
	else
	{
		SVClock::SVTimeStamp l_Timer = SVClock::GetTimeStamp();

		m_ToolTime.Start();

		bRetVal = RunWithNewDisable( RRunStatus );

		SVClock::SVTimeStamp l_Elapsed = ( SVClock::GetTimeStamp() - l_Timer );
		m_EndTime = SVClock::ConvertTo( SVClock::Seconds, l_Elapsed );
		m_AverageTime = ( m_AverageTime + m_EndTime ) / 2.0;

		if( 0 < m_SetNumber )
		{
			RRunStatus.m_ToolSetEndTime = m_EndTime;
			RRunStatus.m_ToolSetAvgTime = m_AverageTime;
		}
		else
		{
			RRunStatus.m_ToolSetEndTime = -1.0;
			RRunStatus.m_ToolSetAvgTime = -1.0;
		}

		if( RRunStatus.m_UpdateCounters )
		{
			m_ToolTime.Stop( RRunStatus.m_lResultDataIndex );

			__int64 l_i64Time = -1;

			m_ToolTime.GetValue( l_i64Time );

			__int64 l_i64MinTime = -1;

			m_MinToolsetTime.GetValue( l_i64MinTime );

			if( m_bResetMinMaxToolsetTime || l_i64MinTime < 0 || l_i64Time < l_i64MinTime )
			{
				m_MinToolsetTime.SetValue( RRunStatus.m_lResultDataIndex, l_i64Time );
			}
			else
			{
				m_MinToolsetTime.SetValue( RRunStatus.m_lResultDataIndex, l_i64MinTime );
			}

			__int64 l_i64MaxTime = -1;

			m_MaxToolsetTime.GetValue( l_i64MaxTime );

			if( m_bResetMinMaxToolsetTime || l_i64MaxTime < 0 || l_i64MaxTime < l_i64Time )
			{
				m_MaxToolsetTime.SetValue( RRunStatus.m_lResultDataIndex, l_i64Time );
			}
			else
			{
				m_MaxToolsetTime.SetValue( RRunStatus.m_lResultDataIndex, l_i64MaxTime );
			}

			if( m_bResetMinMaxToolsetTime )
			{
				m_bResetMinMaxToolsetTime = false;
			}
		}
	}// end else

	return bRetVal;
}// end Run

BOOL SVToolSetClass::RunWithNewDisable( SVRunStatusClass& RRunStatus )
{
	long lCount;
	long l;
	BOOL bRetVal = TRUE;
	BOOL bIsValid = FALSE;
	BOOL bDisabled = FALSE;

	SVRunStatusClass ToolRunStatus;

	ToolRunStatus.m_lResultDataIndex  = RRunStatus.m_lResultDataIndex;
	ToolRunStatus.Images = RRunStatus.Images;
	ToolRunStatus.m_UpdateCounters = RRunStatus.m_UpdateCounters;

	// If Conditional is disabled equation.Run() returns always TRUE.
	// Otherwise the return value depends on the Conditional equation result!
	if( IsEnabled() )
	{
		SVConditionalClass* l_pConditional( GetToolSetConditional() );

		if( l_pConditional )
			l_pConditional->Run( RRunStatus );

		if( !l_pConditional || getConditionalResult( RRunStatus.m_lResultDataIndex ) )
		{
			++m_SetNumber;

			if( RRunStatus.m_UpdateCounters )
			{
				// Set Processed Count...
				m_ProcessedCount.GetValue( lCount );
				m_ProcessedCount.SetValue( RRunStatus.m_lResultDataIndex, ++lCount );
			}

			// Run yourself...
			bRetVal = onRun( RRunStatus );

			// if disabled or disabled by condition
			// leave in previous state
			if( !RRunStatus.IsDisabled() && !RRunStatus.IsDisabledByCondition() )
			{
				if( RRunStatus.m_UpdateCounters )
				{
					// Set Enabled Count...
					m_EnabledCount.GetValue( lCount );
					m_EnabledCount.SetValue( RRunStatus.m_lResultDataIndex, ++lCount );
				}

				// Run your children...
				for( l = 0; l < GetSize(); l++ )
				{
					if( GetAt( l ) )
					{
						ToolRunStatus.ResetRunStateAndToolSetTimes();
						bRetVal = GetAt( l )->Run( ToolRunStatus ) && bRetVal;

						// Update the ToolSet Run Status
						if( ToolRunStatus.IsWarned() )
							RRunStatus.SetWarned();
						if( ToolRunStatus.IsFailed() )
							RRunStatus.SetFailed();
						if( ToolRunStatus.IsPassed() )
							RRunStatus.SetPassed();
						if( ToolRunStatus.IsCriticalFailure() )
							RRunStatus.SetCriticalFailure();
					}// end if

				}// end for
				
			}// end if
			else
			{
				if( RRunStatus.m_UpdateCounters )
				{
					// Set Enabled Count...
					m_EnabledCount.GetValue( lCount );
					m_EnabledCount.SetValue( RRunStatus.m_lResultDataIndex, lCount );
				}

				RRunStatus.SetDisabledByCondition();
				bDisabled = TRUE;
			}// end else

			// Set ToolSet Valid
			m_isObjectValid.SetValue( RRunStatus.m_lResultDataIndex, true );

			// set our state according to the runStatus
			RRunStatus.SetValid();
		}// end if
		else
		{
			RRunStatus.SetDisabledByCondition();
			bDisabled = true;
		}// end else

	}// end if
	else
	{
		RRunStatus.SetDisabled();
		bDisabled = true;
	}// end else

	// if disabled or disabled by condition
	// leave in previous state
	if( !bDisabled && !RRunStatus.IsDisabled() && !RRunStatus.IsDisabledByCondition() )
	{
		// set our state according to the runStatus
		m_Passed.SetValue( RRunStatus.m_lResultDataIndex, RRunStatus.IsPassed() );
		m_Failed.SetValue( RRunStatus.m_lResultDataIndex, RRunStatus.IsFailed() );
		m_ExplicitFailed.SetValue( RRunStatus.m_lResultDataIndex, RRunStatus.IsFailed() );
		m_Warned.SetValue( RRunStatus.m_lResultDataIndex, RRunStatus.IsWarned() );

		if( RRunStatus.m_UpdateCounters )
		{
			// Set Counts...
			if( RRunStatus.IsPassed() )
			{
				m_PassedCount.GetValue( lCount );
				m_PassedCount.SetValue( RRunStatus.m_lResultDataIndex, ++lCount );
				m_FailedCount.GetValue( lCount );
				m_FailedCount.SetValue( RRunStatus.m_lResultDataIndex, lCount );
				m_WarnedCount.GetValue( lCount );
				m_WarnedCount.SetValue( RRunStatus.m_lResultDataIndex, lCount );
			}// end if

			if( RRunStatus.IsFailed() )
			{
				m_PassedCount.GetValue( lCount );
				m_PassedCount.SetValue( RRunStatus.m_lResultDataIndex, lCount );
				m_FailedCount.GetValue( lCount );
				m_FailedCount.SetValue( RRunStatus.m_lResultDataIndex, ++lCount );
				m_WarnedCount.GetValue( lCount );
				m_WarnedCount.SetValue( RRunStatus.m_lResultDataIndex, lCount );
			}// end if

			if( RRunStatus.IsWarned() )
			{
				m_PassedCount.GetValue( lCount );
				m_PassedCount.SetValue( RRunStatus.m_lResultDataIndex, lCount );
				m_FailedCount.GetValue( lCount );
				m_FailedCount.SetValue( RRunStatus.m_lResultDataIndex, lCount );
				m_WarnedCount.GetValue( lCount );
				m_WarnedCount.SetValue( RRunStatus.m_lResultDataIndex, ++lCount );
			}// end if
		}
	}// end if

	// Get Status Color...
	DWORD dwValue = RRunStatus.GetStatusColor();
	m_statusColor.SetValue( RRunStatus.m_lResultDataIndex, dwValue );

	// Get Status...
	dwValue = RRunStatus.GetState();
	m_statusTag.SetValue( RRunStatus.m_lResultDataIndex, dwValue );

	return bRetVal;
}// end RunWithNewDisable

////////////////////////////////////////////////////////////////////////////////
// .Title       : Validate
// -----------------------------------------------------------------------------
// .Description : 
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	:None
// .Return Value
//	:BOOL
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment
//  :30.07.1998 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
BOOL SVToolSetClass::Validate()
{
	if( !IsEnabled() )
	{
		// SetDisabled();
		return true;
	}// end if
	
	return __super::Validate();
}

void SVToolSetClass::SetInvalid()
{
	__super::SetInvalid();

	SVConditionalClass* l_pConditional = GetToolSetConditional();

	if( nullptr != l_pConditional )
	{
		l_pConditional->SetInvalid();
	}
}

bool SVToolSetClass::resetAllObjects( bool shouldNotifyFriends, bool silentReset )
{
	bool Result = ( S_OK == ResetObject() );
	ASSERT( Result );

	return( __super::resetAllObjects( shouldNotifyFriends, silentReset ) && Result );
}

HRESULT SVToolSetClass::ResetObject()
{
	HRESULT l_hrOk = S_OK;

	if( S_OK != __super::ResetObject() )
	{
		l_hrOk = S_FALSE;
	}

	bool l_bReset = false;

	if( S_OK == m_ResetCounts.GetValue( l_bReset ) && l_bReset )
	{
		// Reset Counters...
		m_PassedCount.SetDefaultValue( 0, true );
		m_FailedCount.SetDefaultValue( 0, true );
		m_WarnedCount.SetDefaultValue( 0, true );
		m_EnabledCount.SetDefaultValue( 0, true );
		m_ProcessedCount.SetDefaultValue( 0, true );
		m_RegressionTestMode.SetDefaultValue( SVSVIMStateClass::CheckState( SV_STATE_REGRESSION ), true );

		m_bResetMinMaxToolsetTime = true;
	}
	else
	{
		m_RegressionTestMode.SetDefaultValue( SVSVIMStateClass::CheckState( SV_STATE_REGRESSION ), true );
	}

	SVImageExtentClass ImageExtent( m_MainImageObject.GetImageExtents() );
	double Width(0.0);
	double Height(0.0);
	if( S_OK == ImageExtent.GetExtentProperty( SVExtentPropertyWidth, Width ) &&
		S_OK == ImageExtent.GetExtentProperty( SVExtentPropertyHeight, Height) )
	{
		m_Width.SetDefaultValue( Width, true );
		m_Height.SetDefaultValue( Height, true );
	}

	return l_hrOk;
}

HRESULT SVToolSetClass::ResetCounts()
{
	HRESULT l_hrOk = S_OK;

	try
	{
		SVInspectionProcess* pInspection = dynamic_cast<SVInspectionProcess*>(GetInspection());

		//add request to inspection process
		if( nullptr != pInspection && pInspection->AddInputRequest( &m_ResetCounts, _T( "true" ) ) )
		{
			//add request to inspection process
			if( ! pInspection->AddInputRequestMarker() )
			{
				l_hrOk = S_FALSE;
			}
		}
		else
		{
			l_hrOk = S_FALSE;
		}
	}
	catch( ... )
	{
		l_hrOk = S_FALSE;
	}

	return l_hrOk;
}

HRESULT SVToolSetClass::ClearResetCounts()
{
	HRESULT l_hr;
	int l_iIndex = m_ResetCounts.GetLastSetIndex( );

	if( -1 < l_iIndex )
	{
		l_hr = m_ResetCounts.SetValue( l_iIndex, false );
	}
	else
	{
		l_hr = m_ResetCounts.SetValue( 1, false );
	}

	return l_hr;
}

HRESULT SVToolSetClass::onCollectOverlays(SVImageClass *p_Image, SVExtentMultiLineStructCArray &p_MultiLineArray )
{
	// override TaskObjectList implementation
	return S_FALSE;	// no overlays for toolset
}

bool SVToolSetClass::createAllObjectsFromChild( SVObjectClass& rChildObject )
{
	//MZA: 5. Nov 2014: the method call SetDefaultInputs is missing in the other method
	//do we need this method call SetDefaultInputs here?
	// Set defaults, to ensure that no invalid input info exists...
	SetDefaultInputs();

	SVInspectionLevelCreateStruct createStruct;
	createStruct.OwnerObjectInfo        = this;
	createStruct.InspectionObjectInfo	= GetInspection();

	return rChildObject.createAllObjects(createStruct);
}

void SVToolSetClass::connectChildObject( SVTaskObjectClass& rChildObject )
{
	SVInspectionLevelCreateStruct createStruct;
	createStruct.OwnerObjectInfo        = this;
	createStruct.InspectionObjectInfo	= GetInspection();

	rChildObject.ConnectObject(createStruct);
}

