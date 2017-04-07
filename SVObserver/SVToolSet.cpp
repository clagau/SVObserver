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
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SVToolSetObjectType;

	// Identify our input type needs

	// SetObjectDepth() already called in SVObjectClass Ctor

	// Register Embedded Objects
	RegisterEmbeddedObject( &m_Enabled, SVToolSetEnabledObjectGuid, IDS_OBJECTNAME_ENABLED, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_MainImageObject, SVOutputImageObjectGuid, IDS_OBJECTNAME_IMAGE1 );

	RegisterEmbeddedObject( &m_Passed, SVPassedObjectGuid, IDS_OBJECTNAME_PASSED, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_Failed, SVFailedObjectGuid, IDS_OBJECTNAME_FAILED, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_Warned, SVWarnedObjectGuid, IDS_OBJECTNAME_WARNED, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_ExplicitFailed, SVExplicitFailedObjectGuid, IDS_OBJECTNAME_EXPLICIT_FAILED, false, SvOi::SVResetItemNone );

	RegisterEmbeddedObject( &m_PassedCount, SVPassedCountObjectGuid, IDS_OBJECTNAME_PASSED_COUNT, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_FailedCount, SVFailedCountObjectGuid, IDS_OBJECTNAME_FAILED_COUNT, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_WarnedCount, SVWarnedCountObjectGuid, IDS_OBJECTNAME_WARNED_COUNT, false, SvOi::SVResetItemNone );

	RegisterEmbeddedObject( &m_EnabledCount, SVEnabledCountObjectGuid, IDS_OBJECTNAME_ENABLED_COUNT, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_ProcessedCount, SVProcessedCountObjectGuid, IDS_OBJECTNAME_PROCESSED_COUNT, false, SvOi::SVResetItemNone );

	RegisterEmbeddedObject ( &m_ToolTime, SVToolTimeGuid, IDS_OBJECTNAME_TOOLTIME, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject ( &m_MinToolsetTime, SVMinToolSetTimeGuid, IDS_OBJECTNAME_MINTOOLSETTIME, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject ( &m_MaxToolsetTime, SVMaxToolSetTimeGuid, IDS_OBJECTNAME_MAXTOOLSETTIME, false, SvOi::SVResetItemNone );

	RegisterEmbeddedObject( &m_RegressionTestMode, SVRegressionTestModeGuid, IDS_OBJECTNAME_REGRESSIONTESTMODE, false, SvOi::SVResetItemNone );

	RegisterEmbeddedObject( &m_DrawFlag, SVConditionalToolSetDrawFlagObjectGuid, IDS_OBJECTNAME_DRAWTOOL_FLAG, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_ResetCounts, SVResetInspectionCountsGuid, IDS_OBJECTNAME_RESET_COUNTS, false, SvOi::SVResetItemIP );
	RegisterEmbeddedObject( &m_TriggerCount, SVTriggerCountGuid, IDS_OBJECTNAME_TRIGGER_COUNT, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_latestCompletionPPQIndex, SVLatestCompletionPPQIndexGuid, IDS_LATEST_PPQ_INDEX_AT_COMPLETION, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_TriggerDelta, SVTriggerDeltaGuid, IDS_TRIGGER_DELTA, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_LastTriggerToPPQCompletion, SVLastTriggerToPPQCompletionGuid, IDS_TRIGGER_TO_COMPLETION_TIME, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_LastTriggerToStart, SVLastTriggerToStartGuid, IDS_TRIGGER_TO_START_TIME, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_Width, SVExtentWidthObjectGuid, IDS_OBJECTNAME_EXTENT_WIDTH, false, SvOi::SVResetItemTool );
	RegisterEmbeddedObject( &m_Height, SVExtentHeightObjectGuid, IDS_OBJECTNAME_EXTENT_HEIGHT, false, SvOi::SVResetItemTool );
	

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
	m_Enabled.SetObjectAttributesAllowed( SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
	m_Passed.SetObjectAttributesAllowed( SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	m_Failed.SetObjectAttributesAllowed( SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	m_Warned.SetObjectAttributesAllowed( SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	m_ExplicitFailed.SetObjectAttributesAllowed( SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	m_PassedCount.SetObjectAttributesAllowed( SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	m_FailedCount.SetObjectAttributesAllowed( SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	m_WarnedCount.SetObjectAttributesAllowed( SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	m_EnabledCount.SetObjectAttributesAllowed( SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	m_ProcessedCount.SetObjectAttributesAllowed( SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	m_RegressionTestMode.SetObjectAttributesAllowed( SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
	m_DrawFlag.SetObjectAttributesAllowed( SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
	m_ToolTime.SetObjectAttributesAllowed( SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	m_ResetCounts.SetObjectAttributesAllowed( SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE | SV_EMBEDABLE, SvOi::SetAttributeType::OverwriteAttribute );
	m_TriggerCount.SetObjectAttributesAllowed( SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	m_latestCompletionPPQIndex.SetObjectAttributesAllowed( SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	m_TriggerDelta.SetObjectAttributesAllowed( SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	m_LastTriggerToPPQCompletion.SetObjectAttributesAllowed( SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	m_LastTriggerToStart.SetObjectAttributesAllowed( SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );

	m_MinToolsetTime.SetObjectAttributesAllowed( SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	m_MaxToolsetTime.SetObjectAttributesAllowed( SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	m_Width.SetObjectAttributesAllowed( SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	m_Height.SetObjectAttributesAllowed( SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );

	m_MainImageObject.SetObjectAttributesAllowed( SV_REMOTELY_SETABLE, SvOi::SetAttributeType::AddAttribute );

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
	BOOL bEnabled( true );
	m_Enabled.GetValue( bEnabled );
	return bEnabled ? true : false;
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

void SVToolSetClass::updateToolPosition()
{
	for (long i = 0; i < GetSize(); i++)
	{
		SVToolClass* pTool = dynamic_cast<SVToolClass*> (GetAt(i));
		if (nullptr != pTool)
		{
			//! For object values use one based index
			pTool->setToolPosition(i + 1);
		}
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

bool SVToolSetClass::getConditionalResult(long Index) const
{
	BOOL Value( false );

	if( inputConditionBoolObjectInfo.IsConnected() && inputConditionBoolObjectInfo.GetInputObjectInfo().m_pObject )
	{
		SVBoolValueObjectClass* pBoolObject = ( SVBoolValueObjectClass* )inputConditionBoolObjectInfo.GetInputObjectInfo().m_pObject;
		pBoolObject->GetValue( Value, Index  );
	}
	return Value ? true : false;
}

bool SVToolSetClass::getConditionalResult() const
{
	BOOL Value( false );

	if( inputConditionBoolObjectInfo.IsConnected() && inputConditionBoolObjectInfo.GetInputObjectInfo().m_pObject )
	{
		SVBoolValueObjectClass* pBoolObject = ( SVBoolValueObjectClass* )inputConditionBoolObjectInfo.GetInputObjectInfo().m_pObject;
		pBoolObject->GetValue( Value );
	}
	return Value ? true : false;
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
		l_pConditional = dynamic_cast<SVConditionalClass *>(m_friendList[j].m_pObject);
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
	BOOL Value( false );
	HRESULT Result = m_ResetCounts.GetValue( Value );
	rResetCounts = Value ? true : false;
	return Result;
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

////////////////////////////////////////////////////////////////////////////////
// .Title       : onRun member function of class SVToolSetClass
// -----------------------------------------------------------------------------
// .Description : runs this toolset
////////////////////////////////////////////////////////////////////////////////
bool SVToolSetClass::onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	m_TriggerCount.SetValue( rRunStatus.m_lTriggerCount > 0L ? rRunStatus.m_lTriggerCount : 0L, rRunStatus.m_lResultDataIndex );

	m_latestCompletionPPQIndex.SetValue( rRunStatus.m_WorkloadInfoRsc.m_PPQIndexAtCompletion, rRunStatus.m_lResultDataIndex );

	//before calculating the trigger delta: make sure both trigger times are valid:
	bool bothTriggersValid = (rRunStatus.m_CurrentTriggerTime * rRunStatus.m_PreviousTriggerTime > 0.1);
	__int64 Value = bothTriggersValid ? static_cast<__int64>(SVClock::c_MicrosecondsPerMillisecond * (rRunStatus.m_CurrentTriggerTime - rRunStatus.m_PreviousTriggerTime)) : 0LL;
	m_TriggerDelta.SetValue( Value, rRunStatus.m_lResultDataIndex );
	Value = static_cast<__int64> (rRunStatus.m_WorkloadInfoRsc.TriggerToCompletionInMilliseconds() * SVClock::c_MicrosecondsPerMillisecond);
	m_LastTriggerToPPQCompletion.SetValue( Value, rRunStatus.m_lResultDataIndex );
	Value = static_cast<__int64> (rRunStatus.m_WorkloadInfoRsc.TriggerToStartInMilliseconds() * SVClock::c_MicrosecondsPerMillisecond);
	m_LastTriggerToStart.SetValue( Value, rRunStatus.m_lResultDataIndex );

	bool Result = __super::onRun( rRunStatus, pErrorMessages );
	if( Result )
	{
		// Friends were running, validation was successfully
		// Check conditional execution
		if ( !getConditionalResult( rRunStatus.m_lResultDataIndex ) )
		{
			rRunStatus.SetDisabledByCondition();
		}
	}
	return Result;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : Run
// -----------------------------------------------------------------------------
// .Description : To execute the Run process for all Tools in this Tool Set.
//				: This Function returns TRUE, if the Tool Set were running.
//				: Otherwise it returns FALSE, that means: if the Tool Set should
//				: not run, because the Tool Set Condition failed!
////////////////////////////////////////////////////////////////////////////////
bool SVToolSetClass::Run( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	long lCount;
	bool bRetVal = true;
	BOOL bIsValid = FALSE;
	BOOL bDisabled = FALSE;
	clearRunErrorMessages();

	if( !dynamic_cast<SVInspectionProcess*>(GetInspection())->GetNewDisableMethod() )
	{
		SVRunStatusClass ToolRunStatus;

		ToolRunStatus.m_lResultDataIndex  = rRunStatus.m_lResultDataIndex;
		ToolRunStatus.Images = rRunStatus.Images;
		ToolRunStatus.m_UpdateCounters = rRunStatus.m_UpdateCounters;

		// First Set ToolSet Invalid, if other things succeed, it will become valid
		// Do the same for the counts
		m_isObjectValid.GetValue( bIsValid );
		m_isObjectValid.SetValue( bIsValid, rRunStatus.m_lResultDataIndex );
		m_PassedCount.GetValue( lCount );
		m_PassedCount.SetValue( lCount, rRunStatus.m_lResultDataIndex );
		m_FailedCount.GetValue( lCount );
		m_FailedCount.SetValue( lCount, rRunStatus.m_lResultDataIndex );
		m_WarnedCount.GetValue( lCount );
		m_WarnedCount.SetValue( lCount, rRunStatus.m_lResultDataIndex );
		m_ProcessedCount.GetValue( lCount );
		m_ProcessedCount.SetValue( lCount, rRunStatus.m_lResultDataIndex );
		m_EnabledCount.GetValue( lCount );
		m_EnabledCount.SetValue( lCount, rRunStatus.m_lResultDataIndex );

		// If Conditional is disabled equation.Run() returns always TRUE.
		// Otherwise the return value depends on the Conditional equation result!
		if( IsEnabled() )
		{
			++m_SetNumber;

			SVClock::SVTimeStamp l_Timer = SVClock::GetTimeStamp();

			m_ToolTime.Start();

			if( rRunStatus.m_UpdateCounters )
			{
				// Set Processed Count...
				long lCount = 0;
				m_ProcessedCount.GetValue( lCount );
				m_ProcessedCount.SetValue( ++lCount, rRunStatus.m_lResultDataIndex );
			}

			// Run yourself...
			bRetVal = onRun( rRunStatus, &m_RunErrorMessages );

			// if disabled or disabled by condition
			// leave in previous state
			if( !rRunStatus.IsDisabled() && !rRunStatus.IsDisabledByCondition() )
			{
				if( rRunStatus.m_UpdateCounters )
				{
					// Set Enabled Count...
					long lCount = 0;
					m_EnabledCount.GetValue( lCount );
					m_EnabledCount.SetValue( ++lCount, rRunStatus.m_lResultDataIndex );
				}

				// Run your children...
				for( int i = 0; i < GetSize(); i++ )
				{
					if( GetAt( i ) )
					{
						ToolRunStatus.ResetRunStateAndToolSetTimes();
						ToolRunStatus.m_UpdateCounters = rRunStatus.m_UpdateCounters;

						bRetVal = GetAt( i )->Run( ToolRunStatus, &m_RunErrorMessages ) && bRetVal;

						// Update the ToolSet Run Status
						if( ToolRunStatus.IsWarned() )
							rRunStatus.SetWarned();
						if( ToolRunStatus.IsFailed() )
							rRunStatus.SetFailed();
						if( ToolRunStatus.IsPassed() )
							rRunStatus.SetPassed();
						if( ToolRunStatus.IsCriticalFailure() )
							rRunStatus.SetCriticalFailure();
					}
				}
			}

			// Set ToolSet Valid
			m_isObjectValid.SetValue( BOOL(true), rRunStatus.m_lResultDataIndex );

			// set our state according to the runStatus
			// rRunStatus.SetValid();

			SVClock::SVTimeStamp l_Elapsed = ( SVClock::GetTimeStamp() - l_Timer );
			m_EndTime = SVClock::ConvertTo( SVClock::Seconds, l_Elapsed );
			m_AverageTime = ( m_AverageTime + m_EndTime ) / 2.0;

			if( 0 < m_SetNumber )
			{
				rRunStatus.m_ToolSetEndTime = m_EndTime;	// put units in seconds
				rRunStatus.m_ToolSetAvgTime = m_AverageTime;
			}
			else
			{
				rRunStatus.m_ToolSetEndTime = -1.0;
				rRunStatus.m_ToolSetAvgTime = -1.0;
			}

			if( rRunStatus.m_UpdateCounters )
			{
				m_ToolTime.Stop( rRunStatus.m_lResultDataIndex );

				__int64 l_i64Time = -1;

				m_ToolTime.GetValue( l_i64Time );

				__int64 l_i64MinTime = -1;

				m_MinToolsetTime.GetValue( l_i64MinTime );

				if( m_bResetMinMaxToolsetTime || l_i64MinTime < 0 || l_i64Time < l_i64MinTime )
				{
					m_MinToolsetTime.SetValue( l_i64Time, rRunStatus.m_lResultDataIndex );
				}
				else
				{
					m_MinToolsetTime.SetValue( l_i64MinTime, rRunStatus.m_lResultDataIndex );
				}

				__int64 l_i64MaxTime = -1;

				m_MaxToolsetTime.GetValue( l_i64MaxTime );

				if( m_bResetMinMaxToolsetTime || l_i64MaxTime < 0 || l_i64MaxTime < l_i64Time )
				{
					m_MaxToolsetTime.SetValue( l_i64Time, rRunStatus.m_lResultDataIndex );
				}
				else
				{
					m_MaxToolsetTime.SetValue( l_i64MaxTime, rRunStatus.m_lResultDataIndex );
				}

				if( m_bResetMinMaxToolsetTime )
				{
					m_bResetMinMaxToolsetTime = false;
				}
			}
		}

		// if disabled or disabled by condition
		// leave in previous state
		if( !rRunStatus.IsDisabled() && !rRunStatus.IsDisabledByCondition() )
		{
			// set our state according to the runStatus
			m_Passed.SetValue( BOOL(rRunStatus.IsPassed()), rRunStatus.m_lResultDataIndex );
			m_Failed.SetValue( BOOL(rRunStatus.IsFailed()), rRunStatus.m_lResultDataIndex );
			m_ExplicitFailed.SetValue( BOOL(rRunStatus.IsFailed()), rRunStatus.m_lResultDataIndex );
			m_Warned.SetValue( BOOL(rRunStatus.IsWarned()), rRunStatus.m_lResultDataIndex );

			if( rRunStatus.m_UpdateCounters )
			{
				// Set Counts...
				long lCount = 0;
				if( rRunStatus.IsPassed() )
				{
					m_PassedCount.GetValue( lCount );
					m_PassedCount.SetValue( ++lCount, rRunStatus.m_lResultDataIndex );
				}
				lCount = 0;
				if( rRunStatus.IsFailed() )
				{
					m_FailedCount.GetValue( lCount );
					m_FailedCount.SetValue( ++lCount, rRunStatus.m_lResultDataIndex );
				}
				lCount = 0;
				if( rRunStatus.IsWarned() )
				{
					m_WarnedCount.GetValue( lCount );
					m_WarnedCount.SetValue( ++lCount, rRunStatus.m_lResultDataIndex );
				}
			}
		}

		// Get Status Color...
		DWORD dwValue = rRunStatus.GetStatusColor();
		m_statusColor.SetValue( dwValue, rRunStatus.m_lResultDataIndex );

		// Get Status...
		dwValue = rRunStatus.GetState();
		m_statusTag.SetValue( dwValue, rRunStatus.m_lResultDataIndex );
	}// end if
	else
	{
		SVClock::SVTimeStamp l_Timer = SVClock::GetTimeStamp();

		m_ToolTime.Start();

		bRetVal = RunWithNewDisable( rRunStatus, &m_RunErrorMessages );

		SVClock::SVTimeStamp l_Elapsed = ( SVClock::GetTimeStamp() - l_Timer );
		m_EndTime = SVClock::ConvertTo( SVClock::Seconds, l_Elapsed );
		m_AverageTime = ( m_AverageTime + m_EndTime ) / 2.0;

		if( 0 < m_SetNumber )
		{
			rRunStatus.m_ToolSetEndTime = m_EndTime;
			rRunStatus.m_ToolSetAvgTime = m_AverageTime;
		}
		else
		{
			rRunStatus.m_ToolSetEndTime = -1.0;
			rRunStatus.m_ToolSetAvgTime = -1.0;
		}

		if( rRunStatus.m_UpdateCounters )
		{
			m_ToolTime.Stop( rRunStatus.m_lResultDataIndex );

			__int64 l_i64Time = -1;

			m_ToolTime.GetValue( l_i64Time );

			__int64 l_i64MinTime = -1;

			m_MinToolsetTime.GetValue( l_i64MinTime );

			if( m_bResetMinMaxToolsetTime || l_i64MinTime < 0 || l_i64Time < l_i64MinTime )
			{
				m_MinToolsetTime.SetValue( l_i64Time, rRunStatus.m_lResultDataIndex );
			}
			else
			{
				m_MinToolsetTime.SetValue( l_i64MinTime, rRunStatus.m_lResultDataIndex );
			}

			__int64 l_i64MaxTime = -1;

			m_MaxToolsetTime.GetValue( l_i64MaxTime );

			if( m_bResetMinMaxToolsetTime || l_i64MaxTime < 0 || l_i64MaxTime < l_i64Time )
			{
				m_MaxToolsetTime.SetValue( l_i64Time, rRunStatus.m_lResultDataIndex );
			}
			else
			{
				m_MaxToolsetTime.SetValue( l_i64MaxTime, rRunStatus.m_lResultDataIndex );
			}

			if( m_bResetMinMaxToolsetTime )
			{
				m_bResetMinMaxToolsetTime = false;
			}
		}
	}// end else

	if (nullptr != pErrorMessages && !m_RunErrorMessages.empty())
	{
		pErrorMessages->insert(pErrorMessages->end(), m_RunErrorMessages.begin(), m_RunErrorMessages.end());
	}

	return bRetVal;
}// end Run

bool SVToolSetClass::RunWithNewDisable( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	long lCount;
	long l;
	bool bRetVal = true;
	BOOL bIsValid = FALSE;
	BOOL bDisabled = FALSE;

	SVRunStatusClass ToolRunStatus;

	ToolRunStatus.m_lResultDataIndex  = rRunStatus.m_lResultDataIndex;
	ToolRunStatus.Images = rRunStatus.Images;
	ToolRunStatus.m_UpdateCounters = rRunStatus.m_UpdateCounters;

	// If Conditional is disabled equation.Run() returns always TRUE.
	// Otherwise the return value depends on the Conditional equation result!
	if( IsEnabled() )
	{
		SVConditionalClass* l_pConditional( GetToolSetConditional() );

		if( l_pConditional )
			l_pConditional->Run( rRunStatus );

		if( !l_pConditional || getConditionalResult( rRunStatus.m_lResultDataIndex ) )
		{
			++m_SetNumber;

			if( rRunStatus.m_UpdateCounters )
			{
				// Set Processed Count...
				m_ProcessedCount.GetValue( lCount );
				m_ProcessedCount.SetValue( ++lCount, rRunStatus.m_lResultDataIndex );
			}

			// Run yourself...
			bRetVal = onRun( rRunStatus, pErrorMessages );

			// if disabled or disabled by condition
			// leave in previous state
			if( !rRunStatus.IsDisabled() && !rRunStatus.IsDisabledByCondition() )
			{
				if( rRunStatus.m_UpdateCounters )
				{
					// Set Enabled Count...
					m_EnabledCount.GetValue( lCount );
					m_EnabledCount.SetValue( ++lCount, rRunStatus.m_lResultDataIndex );
				}

				// Run your children...
				for( l = 0; l < GetSize(); l++ )
				{
					if( GetAt( l ) )
					{
						ToolRunStatus.ResetRunStateAndToolSetTimes();
						bRetVal = GetAt( l )->Run( ToolRunStatus, pErrorMessages ) && bRetVal;

						// Update the ToolSet Run Status
						if( ToolRunStatus.IsWarned() )
							rRunStatus.SetWarned();
						if( ToolRunStatus.IsFailed() )
							rRunStatus.SetFailed();
						if( ToolRunStatus.IsPassed() )
							rRunStatus.SetPassed();
						if( ToolRunStatus.IsCriticalFailure() )
							rRunStatus.SetCriticalFailure();
					}// end if

				}// end for
				
			}// end if
			else
			{
				if( rRunStatus.m_UpdateCounters )
				{
					// Set Enabled Count...
					m_EnabledCount.GetValue( lCount );
					m_EnabledCount.SetValue( lCount, rRunStatus.m_lResultDataIndex );
				}

				rRunStatus.SetDisabledByCondition();
				bDisabled = TRUE;
			}// end else

			// Set ToolSet Valid
			m_isObjectValid.SetValue( BOOL(true), rRunStatus.m_lResultDataIndex );

			// set our state according to the runStatus
			rRunStatus.SetValid();
		}// end if
		else
		{
			rRunStatus.SetDisabledByCondition();
			bDisabled = true;
		}// end else

	}// end if
	else
	{
		rRunStatus.SetDisabled();
		bDisabled = true;
	}// end else

	// if disabled or disabled by condition
	// leave in previous state
	if( !bDisabled && !rRunStatus.IsDisabled() && !rRunStatus.IsDisabledByCondition() )
	{
		// set our state according to the runStatus
		m_Passed.SetValue( BOOL(rRunStatus.IsPassed()), rRunStatus.m_lResultDataIndex );
		m_Failed.SetValue( BOOL(rRunStatus.IsFailed()), rRunStatus.m_lResultDataIndex );
		m_ExplicitFailed.SetValue( BOOL(rRunStatus.IsFailed()), rRunStatus.m_lResultDataIndex );
		m_Warned.SetValue( BOOL(rRunStatus.IsWarned()), rRunStatus.m_lResultDataIndex );

		if( rRunStatus.m_UpdateCounters )
		{
			// Set Counts...
			if( rRunStatus.IsPassed() )
			{
				m_PassedCount.GetValue( lCount );
				m_PassedCount.SetValue( ++lCount, rRunStatus.m_lResultDataIndex );
				m_FailedCount.GetValue( lCount );
				m_FailedCount.SetValue( lCount, rRunStatus.m_lResultDataIndex );
				m_WarnedCount.GetValue( lCount );
				m_WarnedCount.SetValue( lCount, rRunStatus.m_lResultDataIndex );
			}// end if

			if( rRunStatus.IsFailed() )
			{
				m_PassedCount.GetValue( lCount );
				m_PassedCount.SetValue( lCount, rRunStatus.m_lResultDataIndex );
				m_FailedCount.GetValue( lCount );
				m_FailedCount.SetValue( ++lCount, rRunStatus.m_lResultDataIndex );
				m_WarnedCount.GetValue( lCount );
				m_WarnedCount.SetValue( lCount, rRunStatus.m_lResultDataIndex );
			}// end if

			if( rRunStatus.IsWarned() )
			{
				m_PassedCount.GetValue( lCount );
				m_PassedCount.SetValue( lCount, rRunStatus.m_lResultDataIndex );
				m_FailedCount.GetValue( lCount );
				m_FailedCount.SetValue( lCount, rRunStatus.m_lResultDataIndex );
				m_WarnedCount.GetValue( lCount );
				m_WarnedCount.SetValue( ++lCount, rRunStatus.m_lResultDataIndex );
			}// end if
		}
	}// end if

	// Get Status Color...
	DWORD dwValue = rRunStatus.GetStatusColor();
	m_statusColor.SetValue( dwValue, rRunStatus.m_lResultDataIndex );

	// Get Status...
	dwValue = rRunStatus.GetState();
	m_statusTag.SetValue( dwValue, rRunStatus.m_lResultDataIndex );

	return bRetVal;
}// end RunWithNewDisable

void SVToolSetClass::SetInvalid()
{
	__super::SetInvalid();

	SVConditionalClass* l_pConditional = GetToolSetConditional();

	if( nullptr != l_pConditional )
	{
		l_pConditional->SetInvalid();
	}
}

bool SVToolSetClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = __super::ResetObject(pErrorMessages) && ValidateLocal(pErrorMessages);
	BOOL bReset( false );

	if( S_OK == m_ResetCounts.GetValue( bReset ) && bReset )
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
	
	updateToolPosition();

	return Result;
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
		l_hr = m_ResetCounts.SetValue( BOOL(false), l_iIndex );
	}
	else
	{
		l_hr = m_ResetCounts.SetValue( BOOL(false), 1 );
	}

	return l_hr;
}

HRESULT SVToolSetClass::onCollectOverlays(SVImageClass *p_Image, SVExtentMultiLineStructVector &p_MultiLineArray )
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

bool SVToolSetClass::ValidateLocal(SvStl::MessageContainerVector *pErrorMessages) const
{
	if( inputConditionBoolObjectInfo.IsConnected() && inputConditionBoolObjectInfo.GetInputObjectInfo().m_pObject )
	{
		return true;
	}
	else
	{
		if (nullptr != pErrorMessages)
		{
			SVStringVector msgList;
			msgList.push_back(GetName());
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_ConditionalValue_Invalid, msgList, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
		return false;
	}
}
