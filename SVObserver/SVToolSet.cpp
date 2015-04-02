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

#include "stdafx.h"
#include "SVToolSet.h"

#include "SVMessage/SVMessage.h"
#include "SVObjectLibrary/SVAnalyzerLevelCreateStruct.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"

#include "SVAnalyzer.h"
#include "SVConditional.h"
#include "SVImageListClass.h"
#include "SVInspectionProcess.h"
#include "SVSVIMStateClass.h"
#include "SVTool.h"

SV_IMPLEMENT_CLASS( SVToolSetClass, SVToolSetClassGuid );

SVToolSetClass::SVToolSetClass( BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID )
			   :SVTaskObjectListClass( BCreateDefaultTaskList, POwner, StringResourceID )
{
	init();
}

void SVToolSetClass::init()
{
	// Identify our output type
	outObjectInfo.ObjectTypeInfo.ObjectType = SVToolSetObjectType;

	// Identify our input type needs

	// SetObjectDepth() already called in SVObjectClass Ctor

	// Register Embedded Objects
	RegisterEmbeddedObject( &enabled, SVToolSetEnabledObjectGuid, IDS_OBJECTNAME_ENABLED, false, SVResetItemNone );
	RegisterEmbeddedObject( &mainImageObject, SVOutputImageObjectGuid, IDS_OBJECTNAME_IMAGE1 );

	RegisterEmbeddedObject( &passed, SVPassedObjectGuid, IDS_OBJECTNAME_PASSED, false, SVResetItemNone );
	RegisterEmbeddedObject( &failed, SVFailedObjectGuid, IDS_OBJECTNAME_FAILED, false, SVResetItemNone );
	RegisterEmbeddedObject( &warned, SVWarnedObjectGuid, IDS_OBJECTNAME_WARNED, false, SVResetItemNone );
	RegisterEmbeddedObject( &explicitFailed, SVExplicitFailedObjectGuid, IDS_OBJECTNAME_EXPLICIT_FAILED, false, SVResetItemNone );

	RegisterEmbeddedObject( &passedCount, SVPassedCountObjectGuid, IDS_OBJECTNAME_PASSED_COUNT, false, SVResetItemNone );
	RegisterEmbeddedObject( &failedCount, SVFailedCountObjectGuid, IDS_OBJECTNAME_FAILED_COUNT, false, SVResetItemNone );
	RegisterEmbeddedObject( &warnedCount, SVWarnedCountObjectGuid, IDS_OBJECTNAME_WARNED_COUNT, false, SVResetItemNone );

	RegisterEmbeddedObject( &enabledCount, SVEnabledCountObjectGuid, IDS_OBJECTNAME_ENABLED_COUNT, false, SVResetItemNone );
	RegisterEmbeddedObject( &processedCount, SVProcessedCountObjectGuid, IDS_OBJECTNAME_PROCESSED_COUNT, false, SVResetItemNone );

	RegisterEmbeddedObject ( &ToolTime, SVToolTimeGuid, IDS_OBJECTNAME_TOOLTIME, false, SVResetItemNone );
	RegisterEmbeddedObject ( &m_svMinToolsetTime, SVMinToolSetTimeGuid, IDS_OBJECTNAME_MINTOOLSETTIME, false, SVResetItemNone );
	RegisterEmbeddedObject ( &m_svMaxToolsetTime, SVMaxToolSetTimeGuid, IDS_OBJECTNAME_MAXTOOLSETTIME, false, SVResetItemNone );

	RegisterEmbeddedObject( &RegressionTestMode, SVRegressionTestModeGuid, IDS_OBJECTNAME_REGRESSIONTESTMODE, false, SVResetItemNone );

	RegisterEmbeddedObject( &drawFlag, SVConditionalToolSetDrawFlagObjectGuid, IDS_OBJECTNAME_DRAWTOOL_FLAG, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_bvoResetCounts, SVResetInspectionCountsGuid, IDS_OBJECTNAME_RESET_COUNTS, false, SVResetItemIP );
	RegisterEmbeddedObject( &m_lvoTriggerCount, SVTriggerCountGuid, IDS_OBJECTNAME_TRIGGER_COUNT, false, SVResetItemNone );

	// Set Embedded defaults
	enabled.SetDefaultValue( TRUE, TRUE );
	passed.SetDefaultValue( FALSE, TRUE );			// Default for Passed is FALSE !!!
	failed.SetDefaultValue( TRUE, TRUE );			// Default for Failed is TRUE !!!
	warned.SetDefaultValue( TRUE, TRUE );			// Default for Warned is TRUE !!!
	explicitFailed.SetDefaultValue( FALSE, TRUE );	// Default for Explicit Failed is FALSE !!!

	passedCount.SetDefaultValue( 0, TRUE );
	failedCount.SetDefaultValue( 0, TRUE );
	warnedCount.SetDefaultValue( 0, TRUE );

	enabledCount.SetDefaultValue( 0, TRUE );
	processedCount.SetDefaultValue( 0, TRUE );

	ToolTime.SetDefaultValue(0, TRUE);
	ToolTime.SetName( "Tool Set Time" );

	m_bResetMinMaxToolsetTime = true;

	m_svMinToolsetTime.SetDefaultValue( 0, TRUE );
	m_svMaxToolsetTime.SetDefaultValue( 0, TRUE );

	RegressionTestMode.SetDefaultValue(FALSE, TRUE);

	drawFlag.SetEnumTypes( IDS_TOOLSETDRAW_ENUMOBJECT_LIST );
	drawFlag.SetDefaultValue( ( const long ) 0, TRUE ); // 0 Should be show 'All Tools'
	m_lvoTriggerCount.SetDefaultValue( 0, TRUE );

	// Set local defaults
	m_StartTime = 0.0;
	m_EndTime = 0.0;
	m_AverageTime = 0.0;
	m_ProcessTime = 0.0;
	setNumber = 0;
	isCreated = false;

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

//******************************************************************************
// Destructor(s):
//******************************************************************************

////////////////////////////////////////////////////////////////////////////////
// .Title       : ~SVToolSetClass
// -----------------------------------------------------------------------------
// .Description : Standard destructor of class SVToolSetClass
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :28.05.1997 RO			First Implementation
//	:30.07.1997 RO			Last Changes, no deletion of toolAdjustmentDialog
////////////////////////////////////////////////////////////////////////////////
SVToolSetClass::~SVToolSetClass()
{
	Destroy();
}

//******************************************************************************
// Operator(s):
//******************************************************************************

////////////////////////////////////////////////////////////////////////////////
// Create Operator
////////////////////////////////////////////////////////////////////////////////

BOOL SVToolSetClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = SVTaskObjectListClass::CreateObject( PCreateStructure );

	// Set / Reset Printable Flags
	enabled.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	passed.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	failed.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	warned.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	explicitFailed.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	passedCount.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	failedCount.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	warnedCount.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	enabledCount.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	processedCount.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	RegressionTestMode.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	drawFlag.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	ToolTime.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_bvoResetCounts.ObjectAttributesAllowedRef() = SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE | SV_EMBEDABLE;
	m_lvoTriggerCount.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;

	m_svMinToolsetTime.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_svMaxToolsetTime.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;

	mainImageObject.ObjectAttributesAllowedRef() |= SV_REMOTELY_SETABLE;

	isCreated = bOk;

	return bOk;
}

////////////////////////////////////////////////////////////////////////////////
// Destroy Operator
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// .Title       : Destroy All Array Member operator of class SVToolSetClass
// -----------------------------------------------------------------------------
// .Description : Destroys all array members
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	:None
// .Return Value
//	:None 
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :28.05.1997 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
void SVToolSetClass::Destroy()
{
	// Delete our friends
	DestroyFriends();

	isCreated = FALSE;

	SVImageListClass destroyImageList;

	DeleteAll();
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : Destroy Specific Array Member operator of class SVToolSetClass
// -----------------------------------------------------------------------------
// .Description : Destroys the array member which is specificated by the given
//              : index argument
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	:int				I
// .Return Value
//	:None 
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :28.05.1997 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
void SVToolSetClass::DestroyAt( int I )
{
	ASSERT( 0 );
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : DeleteAt
// -----------------------------------------------------------------------------
// .Description : Calls the base class 
//				: SVTaskObjectListClass::DeleteAt( Index, Count ) 
//				: and resets the current tool pointer and index.
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	:int				Index
//	:int				Count
// .Return Value
//	:None 
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :17.02.1999 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
/*
void SVToolSetClass::DeleteAt( int Index, int Count )
{
	SVTaskObjectListClass::DeleteAt( Index, Count );

	if( currentIndex >= GetSize() )
		setCurrent( GetSize() - 1 );
	else
		setCurrent( currentIndex );

	if( GetSize() <= 0 )
	{
		currentTool		= NULL;
		currentIndex	= -1;
	}
}
*/


////////////////////////////////////////////////////////////////////////////////
// .Title       : Check the Enabled State
//              : SVToolSetClass
// -----------------------------------------------------------------------------
// .Description : Checks the Enabled state of the SVToolSetClass Object
//              : SVToolClass* argument
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	:SVToolClass*
// .Return Value
//	:None
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :29.06.1999 SEJ			First Implementation
////////////////////////////////////////////////////////////////////////////////
bool SVToolSetClass::IsEnabled() const
{
	bool bEnabled = true;
	enabled.GetValue( bEnabled );
	return bEnabled;
}

bool SVToolSetClass::IsEnabled(long p_lIndex) const
{
	bool bEnabled = true;
	enabled.GetValue( p_lIndex, bEnabled );
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

//******************************************************************************
// Operation(s) Of Writing Access:
//******************************************************************************

////////////////////////////////////////////////////////////////////////////////
// .Title       : Set Current Array Member member function of class
//              : SVToolSetClass
// -----------------------------------------------------------------------------
// .Description : Sets the current array member attribute to the given 
//              : SVToolClass* argument
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	:SVToolClass*
// .Return Value
//	:None
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :28.05.1997 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
/*
BOOL SVToolSetClass::setCurrent( SVToolClass* Tool )
{
	int i = GetIndex( Tool );
	if( i >= 0 )
	{
		currentIndex = i;
		currentTool = Tool;
		return TRUE;
	}
	return FALSE;
}
*/

////////////////////////////////////////////////////////////////////////////////
// .Title       : Set Current Array Member member function of class
//              : SVToolSetClass
// -----------------------------------------------------------------------------
// .Description : Sets the current array member attribute to the given int index
//              : argument
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	:int
// .Return Value
//	:None
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :28.05.1997 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
/*
BOOL SVToolSetClass::setCurrent( int Index )
{
	if( Index >= 0 && Index <= GetUpperBound() )
	{
		currentIndex = Index;
		currentTool = GetToolAt( Index );
		return TRUE;
	}
	return FALSE;
}
*/


////////////////////////////////////////////////////////////////////////////////
// .Title       : Reset Current Array Member member function of class
//              : SVToolSetClass
// -----------------------------------------------------------------------------
// .Description : Sets the current array member attribute to the given int index
//              : argument
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	:int
// .Return Value
//	:None
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :21.07.1998 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
/*
void SVToolSetClass::ResetCurrent()
{
	currentIndex = -1;
	currentTool  = NULL;
}
*/


////////////////////////////////////////////////////////////////////////////////
// .Title       : InsertAt
// -----------------------------------------------------------------------------
// .Description : ...
//              :
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	: 
//  :
// .Output(s)
//	:
//  :
// .Return Value
//	: 
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//	:
// -----------------------------------------------------------------------------
// .Import Variable Reference(s)
//	:
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :29.01.1999 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
void SVToolSetClass::InsertToolAt( int nIndex, SVToolClass* newElement, int nCount )
{
	SVTaskObjectListClass::InsertAt( nIndex, ( SVTaskObjectClass* ) newElement, nCount );
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

	::SVSendMessage( this, SVM_CONNECT_ALL_INPUTS, NULL, NULL );

	// SEJ Aug 24,1999
	// Rebuild ResultList from the toolset level
	m_ResultList.Refresh( this );
}


//******************************************************************************
// Operation(s) Of Reading Access:
//******************************************************************************

////////////////////////////////////////////////////////////////////////////////
// .Title       : Get First Array Member member function of class SVToolSetClass
// -----------------------------------------------------------------------------
// .Description : Gets the first array member
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	:None
// .Return Value
//	:SVToolClass* 
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :28.05.1997 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
/*
SVToolClass* SVToolSetClass::getFirst()
{
	return( setCurrent( 0 ) == TRUE ? currentTool : NULL );
}
*/
////////////////////////////////////////////////////////////////////////////////
// .Title       : Get Last Array Member member function of class SVToolSetClass
// -----------------------------------------------------------------------------
// .Description : Gets the last array member
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	:None
// .Return Value
//	:SVToolClass* 
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :28.05.1997 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
/*
SVToolClass* SVToolSetClass::getLast()
{
	return( setCurrent( GetUpperBound() ) == TRUE ? currentTool : NULL );
}
*/

////////////////////////////////////////////////////////////////////////////////
// .Title       : Get Next Array Member member function of class SVToolSetClass
// -----------------------------------------------------------------------------
// .Description : Gets the next array member regarding the current member 
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	:None
// .Return Value
//	:SVToolClass* 
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :28.05.1997 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
/*
SVToolClass* SVToolSetClass::getNext()
{
	return( setCurrent( currentIndex + 1 ) == TRUE ? currentTool : NULL );
}
*/

////////////////////////////////////////////////////////////////////////////////
// .Title       : Get Previous Array Member member function of class
//              : SVToolSetClass
// -----------------------------------------------------------------------------
// .Description : Gets the previous array member regarding the current member 
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	:None
// .Return Value
//	:SVToolClass* 
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :28.05.1997 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
/*
SVToolClass* SVToolSetClass::getPrevious()
{
	return( setCurrent( currentIndex - 1 ) == TRUE ? currentTool : NULL );
}
*/

////////////////////////////////////////////////////////////////////////////////
// .Title       : Get Current Array Member member function of class
//              : SVToolSetClass
// -----------------------------------------------------------------------------
// .Description : Gets the current array member
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	:None
// .Return Value
//	:SVToolClass* 
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :28.05.1997 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
/*
SVToolClass* SVToolSetClass::getCurrent()
{
	return( currentTool );
}
*/

////////////////////////////////////////////////////////////////////////////////
// .Title       : Get Current Array Member Index member function of class
//              : SVToolSetClass
// -----------------------------------------------------------------------------
// .Description : Gets the index of the current array member
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	:None
// .Return Value
//	:int 
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :28.05.1997 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
/*
int SVToolSetClass::getCurrentIndex()
{
	return( currentIndex );
}
*/

////////////////////////////////////////////////////////////////////////////////
// .Title       : Get Index Of Given Tool member function of class 
//              : SVToolSetClass
// -----------------------------------------------------------------------------
// .Description : Returns the index of the given Tool argument 
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	:SVToolClass*		Tool
// .Return Value
//	:int
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment
//  :27.05.1997 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
int SVToolSetClass::GetIndex( SVToolClass* PTool )
{
	register int i = GetSize() - 1;
	for( ; i >= 0 && GetAt( i ) != PTool; --i );

	return i;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : GetAt
// -----------------------------------------------------------------------------
// .Description : ...
//              :
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	: 
//  :
// .Output(s)
//	:
//  :
// .Return Value
//	: 
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//	:
// -----------------------------------------------------------------------------
// .Import Variable Reference(s)
//	:
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :29.01.1999 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
SVToolClass* SVToolSetClass::GetToolAt( int nIndex ) const
{
	ASSERT( nIndex >= 0 && nIndex < GetSize() );
	if( nIndex >= 0 && nIndex < GetSize() )
		return( ( SVToolClass* )GetAt( nIndex ) ); 
	return NULL;
}


////////////////////////////////////////////////////////////////////////////////
// .Title       : Get Current Result List member function of class
//              : SVToolSetClass
// -----------------------------------------------------------------------------
// .Description : Returns a pointer to the current SVResultListClass object
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	:None
// .Return Value
//	:SVResultListClass* 
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :28.05.1997 RO			First Implementation
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
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	:None
// .Return Value
//	:SVImageClass* 
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :dd.mm.1997 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
SVImageClass* SVToolSetClass::getCurrentImage()
{
	return &mainImageObject;
}

/*
int SVToolSetClass::GetDigitizer()
{
	return digitizer;
}
*/


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
// .History
//	 Date		Author		Comment
//  :22.02.2000 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
SVEnumerateValueObjectClass* SVToolSetClass::GetDrawFlagObject()
{
	return &drawFlag;
}


//******************************************************************************
// Operation(s) Of Representation:
//******************************************************************************

////////////////////////////////////////////////////////////////////////////////
// Drawing Operations(s)
////////////////////////////////////////////////////////////////////////////////


SVConditionalClass* SVToolSetClass::GetToolSetConditional() const
{
	SVConditionalClass* l_pConditional( NULL );

	for( int j = 0; l_pConditional == NULL && j < friendList.size(); j++ )
	{
		l_pConditional = dynamic_cast<SVConditionalClass *>(friendList[j].PObject);
	}// end for

	return l_pConditional; 
}

void SVToolSetClass::GetToolIds( SVToolIdDeque& p_rToolIds ) const
{
	p_rToolIds.clear();

	for( SVTaskObjectPtrVector::const_iterator l_Iter = m_aTaskObjects.begin(); l_Iter != m_aTaskObjects.end(); ++l_Iter )
	{
		SVTaskObjectClass* l_pTask = ( *l_Iter );

		if( l_pTask != NULL )
		{
			p_rToolIds.push_back( l_pTask->GetUniqueObjectID() );
		}
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

#pragma region virtual method (IToolSet)

BOOL SVToolSetClass::OnValidate()
{
	BOOL bRetVal = FALSE;

	//	if( mainImageObject )

	// check that we have a main image and the Equation Class is created
	if( inputConditionBoolObjectInfo.IsConnected() &&
		inputConditionBoolObjectInfo.GetInputObjectInfo().PObject )
	{
		bRetVal = TRUE;
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
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	:None
// .Return Value
//	:BOOL
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment
//  :13.09.1999 SEJ			First Implementation
////////////////////////////////////////////////////////////////////////////////
BOOL SVToolSetClass::onRun( SVRunStatusClass& RRunStatus )
{
	m_lvoTriggerCount.SetValue( RRunStatus.m_lResultDataIndex, RRunStatus.m_lTriggerCount > 0 ? RRunStatus.m_lTriggerCount : 0 );
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
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	:None
// .Return Value
//	:BOOL
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment
//  :27.05.1997 RO			First Implementation
//	:03.07.1999 SEJ/RO		Conditional Equation added.
//  :01.09.1999 RO			Rerouted.
////////////////////////////////////////////////////////////////////////////////
BOOL SVToolSetClass::Run( SVRunStatusClass& RRunStatus )
{
	long lCount;
	BOOL bRetVal = TRUE;
	BOOL bIsValid = FALSE;
	BOOL bDisabled = FALSE;

	if( !GetInspection()->GetNewDisableMethod() )
	{
		SVRunStatusClass ToolRunStatus;

		ToolRunStatus.ClearAll();
		ToolRunStatus.m_lResultDataIndex  = RRunStatus.m_lResultDataIndex;
		ToolRunStatus.Images = RRunStatus.Images;
		ToolRunStatus.m_UpdateCounters = RRunStatus.m_UpdateCounters;

		// First Set ToolSet Invalid, if other things succeed, it will become valid
		// Do the same for the counts
		isObjectValid.GetValue( bIsValid );
		isObjectValid.SetValue( RRunStatus.m_lResultDataIndex, bIsValid );
		passedCount.GetValue( lCount );
		passedCount.SetValue( RRunStatus.m_lResultDataIndex, lCount );
		failedCount.GetValue( lCount );
		failedCount.SetValue( RRunStatus.m_lResultDataIndex, lCount );
		warnedCount.GetValue( lCount );
		warnedCount.SetValue( RRunStatus.m_lResultDataIndex, lCount );
		processedCount.GetValue( lCount );
		processedCount.SetValue( RRunStatus.m_lResultDataIndex, lCount );
		enabledCount.GetValue( lCount );
		enabledCount.SetValue( RRunStatus.m_lResultDataIndex, lCount );

		// If Conditional is disabled equation.Run() returns always TRUE.
		// Otherwise the return value depends on the Conditional equation result!
		if( IsEnabled() )
		{
			++setNumber;

			SVClock::SVTimeStamp l_Timer = SVClock::GetTimeStamp();

			ToolTime.Start();

			if( RRunStatus.m_UpdateCounters )
			{
				// Set Processed Count...
				long lCount = 0;
				processedCount.GetValue( lCount );
				processedCount.SetValue( RRunStatus.m_lResultDataIndex, ++lCount );
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
					enabledCount.GetValue( lCount );
					enabledCount.SetValue( RRunStatus.m_lResultDataIndex, ++lCount );
				}

				// Run your children...
				for( int i = 0; i < GetSize(); i++ )
				{
					if( GetAt( i ) )
					{
						ToolRunStatus.ClearAll();
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
			isObjectValid.SetValue( RRunStatus.m_lResultDataIndex, TRUE );

			// set our state according to the runStatus
			// RRunStatus.SetValid();

			SVClock::SVTimeStamp l_Elapsed = ( SVClock::GetTimeStamp() - l_Timer );
			m_EndTime = SVClock::ConvertTo( SVClock::Seconds, l_Elapsed );
			m_AverageTime = ( m_AverageTime + m_EndTime ) / 2.0;

			if( 0 < setNumber )
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
				ToolTime.Stop( RRunStatus.m_lResultDataIndex );

				__int64 l_i64Time = -1;

				ToolTime.GetValue( l_i64Time );

				__int64 l_i64MinTime = -1;

				m_svMinToolsetTime.GetValue( l_i64MinTime );

				if( m_bResetMinMaxToolsetTime || l_i64MinTime < 0 || l_i64Time < l_i64MinTime )
				{
					m_svMinToolsetTime.SetValue( RRunStatus.m_lResultDataIndex, l_i64Time );
				}
				else
				{
					m_svMinToolsetTime.SetValue( RRunStatus.m_lResultDataIndex, l_i64MinTime );
				}

				__int64 l_i64MaxTime = -1;

				m_svMaxToolsetTime.GetValue( l_i64MaxTime );

				if( m_bResetMinMaxToolsetTime || l_i64MaxTime < 0 || l_i64MaxTime < l_i64Time )
				{
					m_svMaxToolsetTime.SetValue( RRunStatus.m_lResultDataIndex, l_i64Time );
				}
				else
				{
					m_svMaxToolsetTime.SetValue( RRunStatus.m_lResultDataIndex, l_i64MaxTime );
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
			passed.SetValue( RRunStatus.m_lResultDataIndex, RRunStatus.IsPassed() );
			failed.SetValue( RRunStatus.m_lResultDataIndex, RRunStatus.IsFailed() );
			explicitFailed.SetValue( RRunStatus.m_lResultDataIndex, RRunStatus.IsFailed() );
			warned.SetValue( RRunStatus.m_lResultDataIndex, RRunStatus.IsWarned() );

			if( RRunStatus.m_UpdateCounters )
			{
				// Set Counts...
				long lCount = 0;
				if( RRunStatus.IsPassed() )
				{
					passedCount.GetValue( lCount );
					passedCount.SetValue( RRunStatus.m_lResultDataIndex, ++lCount );
				}
				lCount = 0;
				if( RRunStatus.IsFailed() )
				{
					failedCount.GetValue( lCount );
					failedCount.SetValue( RRunStatus.m_lResultDataIndex, ++lCount );
				}
				lCount = 0;
				if( RRunStatus.IsWarned() )
				{
					warnedCount.GetValue( lCount );
					warnedCount.SetValue( RRunStatus.m_lResultDataIndex, ++lCount );
				}
			}
		}

		// Get Status Color...
		DWORD dwValue = RRunStatus.GetStatusColor();
		statusColor.SetValue( RRunStatus.m_lResultDataIndex, dwValue );

		// Get Status...
		dwValue = RRunStatus.GetState();
		statusTag.SetValue( RRunStatus.m_lResultDataIndex, dwValue );
	}// end if
	else
	{
		SVClock::SVTimeStamp l_Timer = SVClock::GetTimeStamp();

		ToolTime.Start();

		bRetVal = RunWithNewDisable( RRunStatus );

		SVClock::SVTimeStamp l_Elapsed = ( SVClock::GetTimeStamp() - l_Timer );
		m_EndTime = SVClock::ConvertTo( SVClock::Seconds, l_Elapsed );
		m_AverageTime = ( m_AverageTime + m_EndTime ) / 2.0;

		if( 0 < setNumber )
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
			ToolTime.Stop( RRunStatus.m_lResultDataIndex );

			__int64 l_i64Time = -1;

			ToolTime.GetValue( l_i64Time );

			__int64 l_i64MinTime = -1;

			m_svMinToolsetTime.GetValue( l_i64MinTime );

			if( m_bResetMinMaxToolsetTime || l_i64MinTime < 0 || l_i64Time < l_i64MinTime )
			{
				m_svMinToolsetTime.SetValue( RRunStatus.m_lResultDataIndex, l_i64Time );
			}
			else
			{
				m_svMinToolsetTime.SetValue( RRunStatus.m_lResultDataIndex, l_i64MinTime );
			}

			__int64 l_i64MaxTime = -1;

			m_svMaxToolsetTime.GetValue( l_i64MaxTime );

			if( m_bResetMinMaxToolsetTime || l_i64MaxTime < 0 || l_i64MaxTime < l_i64Time )
			{
				m_svMaxToolsetTime.SetValue( RRunStatus.m_lResultDataIndex, l_i64Time );
			}
			else
			{
				m_svMaxToolsetTime.SetValue( RRunStatus.m_lResultDataIndex, l_i64MaxTime );
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
	ToolRunStatus.ClearAll();
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
			++setNumber;

			if( RRunStatus.m_UpdateCounters )
			{
				// Set Processed Count...
				processedCount.GetValue( lCount );
				processedCount.SetValue( RRunStatus.m_lResultDataIndex, ++lCount );
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
					enabledCount.GetValue( lCount );
					enabledCount.SetValue( RRunStatus.m_lResultDataIndex, ++lCount );
				}

				// Run your children...
				for( l = 0; l < GetSize(); l++ )
				{
					if( GetAt( l ) )
					{
						ToolRunStatus.ClearAll();
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
					enabledCount.GetValue( lCount );
					enabledCount.SetValue( RRunStatus.m_lResultDataIndex, lCount );
				}

				RRunStatus.SetDisabledByCondition();
				bDisabled = TRUE;
			}// end else

			// Set ToolSet Valid
			isObjectValid.SetValue( RRunStatus.m_lResultDataIndex, TRUE );

			// set our state according to the runStatus
			RRunStatus.SetValid();
		}// end if
		else
		{
			RRunStatus.SetDisabledByCondition();
			bDisabled = TRUE;
		}// end else

	}// end if
	else
	{
		RRunStatus.SetDisabled();
		bDisabled = TRUE;
	}// end else

	// if disabled or disabled by condition
	// leave in previous state
	if( !bDisabled && !RRunStatus.IsDisabled() && !RRunStatus.IsDisabledByCondition() )
	{
		// set our state according to the runStatus
		passed.SetValue( RRunStatus.m_lResultDataIndex, RRunStatus.IsPassed() );
		failed.SetValue( RRunStatus.m_lResultDataIndex, RRunStatus.IsFailed() );
		explicitFailed.SetValue( RRunStatus.m_lResultDataIndex, RRunStatus.IsFailed() );
		warned.SetValue( RRunStatus.m_lResultDataIndex, RRunStatus.IsWarned() );

		if( RRunStatus.m_UpdateCounters )
		{
			// Set Counts...
			if( RRunStatus.IsPassed() )
			{
				passedCount.GetValue( lCount );
				passedCount.SetValue( RRunStatus.m_lResultDataIndex, ++lCount );
				failedCount.GetValue( lCount );
				failedCount.SetValue( RRunStatus.m_lResultDataIndex, lCount );
				warnedCount.GetValue( lCount );
				warnedCount.SetValue( RRunStatus.m_lResultDataIndex, lCount );
			}// end if

			if( RRunStatus.IsFailed() )
			{
				passedCount.GetValue( lCount );
				passedCount.SetValue( RRunStatus.m_lResultDataIndex, lCount );
				failedCount.GetValue( lCount );
				failedCount.SetValue( RRunStatus.m_lResultDataIndex, ++lCount );
				warnedCount.GetValue( lCount );
				warnedCount.SetValue( RRunStatus.m_lResultDataIndex, lCount );
			}// end if

			if( RRunStatus.IsWarned() )
			{
				passedCount.GetValue( lCount );
				passedCount.SetValue( RRunStatus.m_lResultDataIndex, lCount );
				failedCount.GetValue( lCount );
				failedCount.SetValue( RRunStatus.m_lResultDataIndex, lCount );
				warnedCount.GetValue( lCount );
				warnedCount.SetValue( RRunStatus.m_lResultDataIndex, ++lCount );
			}// end if
		}
	}// end if

	// Get Status Color...
	DWORD dwValue = RRunStatus.GetStatusColor();
	statusColor.SetValue( RRunStatus.m_lResultDataIndex, dwValue );

	// Get Status...
	dwValue = RRunStatus.GetState();
	statusTag.SetValue( RRunStatus.m_lResultDataIndex, dwValue );

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
		return TRUE;
	}// end if
	
	return SVTaskObjectListClass::Validate();
}

void SVToolSetClass::SetInvalid()
{
	SVTaskObjectListClass::SetInvalid();

	SVConditionalClass* l_pConditional = GetToolSetConditional();

	if( l_pConditional != NULL )
	{
		l_pConditional->SetInvalid();
	}
}

HRESULT SVToolSetClass::ResetObject()
{
	HRESULT l_hrOk = S_OK;

	if( SVTaskObjectListClass::ResetObject() != S_OK )
	{
		l_hrOk = S_FALSE;
	}

	bool l_bReset = false;

	if( m_bvoResetCounts.GetValue( l_bReset ) == S_OK && l_bReset )
	{
		// Reset Counters...
		passedCount.SetDefaultValue( 0, TRUE );
		failedCount.SetDefaultValue( 0, TRUE );
		warnedCount.SetDefaultValue( 0, TRUE );
		enabledCount.SetDefaultValue( 0, TRUE );
		processedCount.SetDefaultValue( 0, TRUE );
		RegressionTestMode.SetDefaultValue( SVSVIMStateClass::CheckState( SV_STATE_REGRESSION ), TRUE );

		m_bResetMinMaxToolsetTime = true;
	}
	else
	{
		RegressionTestMode.SetDefaultValue( SVSVIMStateClass::CheckState( SV_STATE_REGRESSION ), TRUE );
	}

	return l_hrOk;
}

HRESULT SVToolSetClass::ResetCounts()
{
	HRESULT l_hrOk = S_OK;

	try
	{
		SVInspectionProcess* pIP = GetInspection();

		//add request to inspection process
		if( pIP->AddInputRequest( &m_bvoResetCounts, _T( "true" ) ) )
		{
			//add request to inspection process
			if( ! pIP->AddInputRequestMarker() )
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
	int l_iIndex = m_bvoResetCounts.GetLastSetIndex( );

	if( -1 < l_iIndex )
	{
		l_hr = m_bvoResetCounts.SetValue( l_iIndex, false );
	}
	else
	{
		l_hr = m_bvoResetCounts.SetValue( 1, false );
	}

	return l_hr;
}

DWORD_PTR SVToolSetClass::processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext )
{
	DWORD_PTR DwResult = NULL;

	SVAnalyzerLevelCreateStruct createStruct;

	// Try to process message by yourself...
	DWORD dwPureMessageID = DwMessageID & SVM_PURE_MESSAGE;
	switch( dwPureMessageID )
	{
	case SVMSGID_RESET_ALL_OBJECTS:
		{
			HRESULT l_ResetStatus = ResetObject();
			if( l_ResetStatus != S_OK )
			{
				ASSERT( SUCCEEDED( l_ResetStatus ) );

				DwResult = SVMR_NO_SUCCESS;
			}
			else
			{
				DwResult = SVMR_SUCCESS;
			}
			break;
		}

	case SVMSGID_CREATE_ALL_OBJECTS:
		{
			if( !IsCreated() && !CreateObject( reinterpret_cast<SVObjectLevelCreateStruct*>(DwMessageValue) ) )
			{
				ASSERT( FALSE );

				DwResult = SVMR_NO_SUCCESS;
			}
			else
			{
				DwResult = SVMR_SUCCESS;
			}

			createStruct.OwnerObjectInfo = this;
			createStruct.AnalyzerObjectInfo = GetAnalyzer();
			createStruct.ToolObjectInfo	= GetTool();
			createStruct.InspectionObjectInfo	= GetInspection();

			DwMessageValue = reinterpret_cast<DWORD_PTR>(&createStruct);

			break;
		}

	case SVMSGID_CONNECT_ALL_OBJECTS:
		{
			if( ConnectObject( reinterpret_cast<SVObjectLevelCreateStruct*>(DwMessageValue) ) != S_OK )
			{
				ASSERT( FALSE );

				DwResult = SVMR_NO_SUCCESS;
			}
			else
			{
				DwResult = SVMR_SUCCESS;
			}

			createStruct.OwnerObjectInfo = this;
			createStruct.AnalyzerObjectInfo = GetAnalyzer();
			createStruct.ToolObjectInfo	= GetTool();
			createStruct.InspectionObjectInfo	= GetInspection();

			DwMessageValue = reinterpret_cast<DWORD_PTR>(&createStruct);

			break;
		}

	case SVMSGID_CREATE_CHILD_OBJECT:
		{
			// ...use second message parameter ( DwMessageValue ) as SVObjectClass* of the child object
			// ...returns SVMR_SUCCESS, SVMR_NO_SUCCESS or SVMR_NOT_PROCESSED
			SVObjectClass* pChildObject = reinterpret_cast<SVObjectClass*>(DwMessageValue);
			return CreateChildObject(pChildObject, static_cast<DWORD>(DwMessageContext));
		}
		break;

	case SVMSGID_CONNECT_CHILD_OBJECT:
		{
			// ...use second message parameter ( DwMessageValue ) as SVObjectClass* of the child object
			SVObjectClass* pChildObject = reinterpret_cast<SVObjectClass*>(DwMessageValue);
			if( SV_IS_KIND_OF( pChildObject, SVObjectClass ) )
			{
				SVInspectionLevelCreateStruct createStruct;

				createStruct.OwnerObjectInfo        = this;
				createStruct.InspectionObjectInfo	= GetInspection();

				DWORD_PTR l_Return = SVSendMessage( pChildObject, SVM_CONNECT_ALL_OBJECTS, reinterpret_cast<DWORD_PTR>(&createStruct), NULL );

				return l_Return;
			}
			return SVMR_NOT_PROCESSED;
		}
		break;
	}

	return( SVTaskObjectListClass::processMessage( DwMessageID, DwMessageValue, DwMessageContext ) | DwResult );
}


/*
////////////////////////////////////////////////////////////////////////////////
// .Title       : Edit Current Tool member function of class SVToolSetClass
// -----------------------------------------------------------------------------
// .Description : Calls the current tool Tool Adjustment Dialog
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	:None
// .Return Value
//	:BOOL
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment
//  :27.05.1997 RO			First Implementation
//	:30.07.1997 RO			Last Changes, always new allocation of the 
//                          toolAdjustmentDialog
////////////////////////////////////////////////////////////////////////////////
BOOL SVToolSetClass::Edit()
{
	if( currentTool )
	{	
		// Check if can Edit this Tool
		// Primarily for the Build Reference Tool
		// to check if anyone is using the output image
		if( ! currentTool->IsOkToEdit() )
			return FALSE;

		// SEJ (July 14,1999) - Get UniqueObjectID
		GUID currentToolID = currentTool->GetUniqueObjectID();

		toolAdjustmentDialog = new SVToolAdjustmentDialogSheetClass( this->getCurrent(), "Tool Adjustment", parent );
		toolAdjustmentDialog->DoModal();
		delete( toolAdjustmentDialog );
		toolAdjustmentDialog = NULL;

		// SEJ (July 14,1999) - Set currentTool
		currentTool = ( SVToolClass * )SVObjectManagerClass::Instance().GetObject( currentToolID );

		return TRUE;
	}
	
	return FALSE;
}
*/

///////////////////////////////////////////////////////////////////////////////
//
//
//
// Check for existing tool with same name as in newText.
// If duplicate found - add 'Dup' to name, and retry test.
//
void SVToolSetClass::CheckForExistingName(
    CString& newText,
    SVToolClass* pToolNewName   // the tool that is getting modified name
)
{
    BOOL bDup = FALSE;
    CString newTextUpper;
    BOOL bDone = FALSE;
    int nCountSafety = 10;

    while(!bDone)
    {
        bDup = FALSE;
        newTextUpper = newText;
        newTextUpper.MakeUpper();

		for( SVTaskObjectPtrVector::iterator l_Iter = m_aTaskObjects.begin(); l_Iter != m_aTaskObjects.end(); ++l_Iter )
		{
			SVToolClass* l_pTool = dynamic_cast< SVToolClass* >( *l_Iter );

			if( l_pTool != NULL )
			{
				CString toolName = l_pTool->GetName();
				toolName.MakeUpper();
	        
				if(toolName == newTextUpper)
				{
					//
					// The tool receiving new name itself is exempt from 
					// test of duplicate.
					//
					if(l_pTool != pToolNewName)
					{
						bDup = TRUE;
						break;
					}
				}
			}
        }
        if(bDup)
        {
            newText+= _T("DUP");

            //
            // Allow only 10 iterations
            //
            nCountSafety--;
            if(nCountSafety < 1)
            {
                bDone = TRUE;
            }
        }
        else
        {
            bDone = TRUE;
        }
    }
}

HRESULT SVToolSetClass::onCollectOverlays(SVImageClass *p_Image, SVExtentMultiLineStructCArray &p_MultiLineArray )
{
	// override TaskObjectList implementation
	return S_FALSE;	// no overlays for toolset
}

DWORD_PTR SVToolSetClass::createAllObjectsFromChild( SVObjectClass* pChildObject )
{
	//MZA: 5. Nov 2014: the method call SetDefaultInputs is missing in the other method
	//do we need this method call SetDefaultInputs here?
	// Set defaults, to ensure that no invalid input info exists...
	SetDefaultInputs();

	SVInspectionLevelCreateStruct createStruct;
	createStruct.OwnerObjectInfo        = this;
	createStruct.InspectionObjectInfo	= GetInspection();

	return SVSendMessage( pChildObject, SVM_CREATE_ALL_OBJECTS, reinterpret_cast<DWORD_PTR>(&createStruct), NULL );
}
//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVToolSet.cpp_v  $
 * 
 *    Rev 1.9   07 Jan 2015 17:52:44   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  980
 * SCR Title:  Add Non-Inspection Objects to the Result View
 * Checked in by:  mEichengruen;  Marcus Eichengruen
 * Change Description:  
 *   Changed init method to call m_ResultList.SetToolSet.
 * Cleaned up spacing.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   19 Dec 2014 04:24:36   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  978
 * SCR Title:  Copy and Paste a Tool within an Inspection or Between Different Inspections
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Added method: moveTool
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   10 Dec 2014 12:07:48   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  908
 * SCR Title:  Remove option for Operator Move (SVO 101)
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Removed ToolSelectedForOperatorMove embedded object and member functions.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   09 Dec 2014 10:22:38   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  908
 * SCR Title:  Remove option for Operator Move (SVO 101)
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Removed functions that support Operator Move.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   12 Jun 2014 16:20:48   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  906
 * SCR Title:  SVObserver Tool Grouping
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Removed include for SVToolSetTabView.h
 * Removed duplicate include for SVInspectionProcess.h
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   15 May 2014 14:50:08   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Revised processMessage to use DWORD_PTR
 * Revised SVSendMessage to use DWORD_PTR
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   01 Feb 2014 12:22:16   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SVSendmessage and processmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   13 May 2013 12:37:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 014.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
      Rev 3.72   07 May 2013 13:19:58   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  830
   SCR Title:  Consolidate all Time Stamp and Clock Functionality
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to use new SVClock conversions functions, which will clarify timing tolerance checks.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.71   02 May 2013 11:25:22   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  810
   SCR Title:  Remove support for Legacy Tools
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Updated process message method to fix formatting issues and updated the reset object section to only assert when the HRESULT is a failed status.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
 *    Rev 1.1   08 May 2013 16:44:08   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
      Rev 3.70   29 Apr 2013 14:33:20   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  830
   SCR Title:  Consolidate all Time Stamp and Clock Functionality
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to use the new SVClock::SVTimeStamp type for all time stamp variables.  Update to use new SVClock::GetTimeStamp() function to get the time stamp value in milliseconds.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
 *    Rev 1.0   23 Apr 2013 15:46:44   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.69   25 Mar 2013 12:58:18   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  796
   SCR Title:  Add a License Manager that will handle the Matrox Licenses
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     removed the copying of the errorlist from the ChildRunStatus
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.68   27 Feb 2013 11:27:50   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  796
   SCR Title:  Add a License Manager that will handle the Matrox Licenses
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Run Method - copied the ToolRunStatus errorlist to the main RunStatus
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.67   18 Feb 2013 13:03:32   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  812
   SCR Title:  Add New Remote Command Functionality
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated the attributes of all main images to be remotely setable.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.66   04 Feb 2013 13:53:54   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  808
   SCR Title:  Current Toolset Draw setting does not draw tool overlay on image in Run Mode
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Modified SetSelectedToolForOperatorMove function so it will not change the referenced guid variable unless the function finds a tool.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.65   08 Jan 2013 12:30:20   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  805
   SCR Title:  Fix Value Object Indexing Problem when Shortening PPQ
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Added new virtual method to classes to fix adjusting Object Depth of Value Object and Last Set Index.  Also, updated code with new methodology.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.64   01 Aug 2012 13:06:52   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Removed all overlay collection functionality that uses data at a particular index.  The current overlay collection functionality collects the data in the inspection thread, so that all data is consistant based on the last executed inspection.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.63   12 Jul 2012 15:12:44   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to add in an additional string identifier for the input and fixed clear functionality assoicated with input structure.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.62   09 Jul 2012 14:21:00   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to change the include for SVIM State.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.61   02 Jul 2012 17:50:40   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to promote new display functionality.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.60   19 Jun 2012 14:25:44   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  744
   SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Protected Object Information structure attributes.  This change will add accessor methods to get and set attributes.
   Converted GUID to SVGUID.   This allows the use of standard operators for comparison and methods for conversion.
   Optimized the application state functionality.  The application state class was altered to simplify its usage and elements were added to object manager to separate tracking states.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.59   01 Mar 2012 15:25:16   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  754
   SCR Title:  Fix regression test mode flag when exiting regression mode
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     fixed issue with Regression Test Mode flag
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.58   28 Sep 2011 13:16:10   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  730
   SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Updated source code to fix issues with camera image interface and methodology.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.57   16 Sep 2011 16:23:12   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  730
   SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Updated objects with new device and resource management functionality.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.56   11 Apr 2011 18:55:38   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  712
   SCR Title:  Fix issues with black images when using command interface (SIAC)
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated child run status objects with update counters flag
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.55   08 Apr 2011 13:33:06   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  712
   SCR Title:  Fix issues with black images when using command interface (SIAC)
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated inspection functionality to use the update counters flag.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.54   08 Dec 2010 13:45:16   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to include changes in notification functionality using the Observer Design Pattern.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.53   09 Nov 2010 16:30:32   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  704
   SCR Title:  Upgrade SVObserver Version for 5.33 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to remove duplicate container objects.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.52   05 Nov 2010 14:58:10   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to remove the un-necessary IP View Control from the application.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.51   04 Nov 2010 14:15:28   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to fix issues with image processing and display image processing classes and associated includes.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.50   01 Jun 2010 15:16:44   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  693
   SCR Title:  Fix Performance Issue with Inspection Process
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to improve ability to track performance.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.49   01 Jun 2010 11:10:08   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  693
   SCR Title:  Fix Performance Issue with Inspection Process
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated code to remove redundent methodologies and fix missing or incorrect calling functionality.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.48   05 Mar 2010 11:01:18   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  686
   SCR Title:  Update Laptop version to load all valid configurations
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Added new element to run status for critical errors.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.47   16 Dec 2009 13:20:18   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  677
   SCR Title:  Fix problem in camera notify thread
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Fix issues with includes and comments.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.46   20 Oct 2008 11:02:48   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  635
   SCR Title:  Blob Analyzer Bug Fixes
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Added IsEnabled method  that uses a dataIndex.
   Added WasEnabled method  that uses a dataIndex.
   Added getConditionalResult method that uses a dataIndex.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.45   21 Jun 2007 15:03:06   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  598
   SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
   Checked in by:  jSpila;  Joseph Spila
   Change Description:  
     These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.44   20 Mar 2006 13:57:12   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  547
   SCR Title:  Fix Image overlay data
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     Fixed overlays issue
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.43   02 Nov 2005 07:33:20   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  512
   SCR Title:  Add Published Result Images functionality
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     changed RunStatus.m_lResultImageIndex to .Images
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.42   14 Oct 2005 12:03:04   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  486
   SCR Title:  Add Trigger Count to Tool Set
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Fixed Bug in CreateObject with ObjectAttributesAllowed on TriggerCount
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.41   16 Aug 2005 12:23:56   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  499
   SCR Title:  Add Maximum/Minimum Tool Set Time
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     Added new member variable to control the Min/Max tool set time reset functionality.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.40   10 Aug 2005 12:44:40   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  500
   SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated methods to improve performance and reduce system delay.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.39   09 Aug 2005 08:49:28   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  500
   SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated objects and dialogs to use the ActiveX Interface to update inspection data.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.38   03 Aug 2005 13:10:32   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  486
   SCR Title:  Add Trigger Count to Tool Set
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Fixed bug in reset counts logic and cleaned up code.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.37   03 Aug 2005 10:07:50   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  486
   SCR Title:  Add Trigger Count to Tool Set
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Forced Trigger count Value Object to never be less than zero in the SVToolSetClass::onRun
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.36   02 Aug 2005 13:56:30   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  455
   SCR Title:  New Security for SVObserver
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Added OKToEdit() to make menu security more consistant.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.35   02 Aug 2005 09:50:00   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  500
   SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Added new method to handle resetting counts.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.34   29 Jul 2005 13:32:40   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  500
   SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated object initialization to change the parameters for RegisterEmbeddedObject for a SVValueObjectClass.  The two additional parameters will inform the preperation process before toolset execution.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.33   26 Jul 2005 14:04:36   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  485
   SCR Title:  Add a way to Reset Counts remotely
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Added Reset Flag struct to replace flag in IPDoc.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.32   26 Jul 2005 13:38:02   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  450
   SCR Title:  Add asynchronous functionality to the archive tool
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     updated to use SVIPDoc SVResetStruct
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.31   14 Jul 2005 12:41:58   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  464
   SCR Title:  Add array indexing for value objects
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     updated for resultList member variable name change (m_pToolSet)
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.30   29 Jun 2005 16:04:40   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  492
   SCR Title:  Upgrade SVObserver to version 4.60
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     added min and max toolset time objects
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.29   28 Jun 2005 14:44:54   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  486
   SCR Title:  Add Trigger Count to Tool Set
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Removed extra attributes on m_lvoTriggerCount
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.28   21 Jun 2005 14:16:36   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  455
   SCR Title:  New Security for SVObserver
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Added SVSecurity for user access.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.27   21 Jun 2005 13:11:20   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  464
   SCR Title:  Add array indexing for value objects
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     object attributes use accessor functions
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.26   19 May 2005 07:28:46   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  486
   SCR Title:  Add Trigger Count to Tool Set
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Add trigger count value object to tool set.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.25   18 May 2005 15:04:04   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  485
   SCR Title:  Add a way to Reset Counts remotely
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Added bool value object to the SVToolSetClass for resetting the counts in the tool set.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.24   16 Mar 2005 11:00:18   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  456
   SCR Title:  Update Image and Tool Objects to use the new Extent Classes
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     added comment for future change
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.23   18 Feb 2005 11:38:22   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  262
   SCR Title:  Improve performance when loading a configuration in SVObserver
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Changed method of tracking values objects from full object manager rebuild to a value object registration methodology.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.22   17 Feb 2005 13:54:36   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  456
   SCR Title:  Update Image and Tool Objects to use the new Extent Classes
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Changes to support new Extents and Reset Methodology
   Added ResetObject(
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.21   16 Feb 2005 15:23:44   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  440
   SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     updated header files
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.20   24 Sep 2003 15:30:08   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  320
   SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     removed setvalid - was overwriting previous status
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.19   27 Aug 2003 10:29:20   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  320
   SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     removed SV_PRINTABLE flag from ToolTime
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.18   07 Aug 2003 12:20:52   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  351
   SCR Title:  Fix Bug in SVObserver I0001125 Rename last tool in toolset
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     did a check to make sure the selected tool is actaully vaild.  If not, it will put a message up saying to load a vaild configuration.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.17   08 Jul 2003 10:57:48   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  373
   SCR Title:  Add a new disable method that doesn't copy forward all result values
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Fixed small problem with counts (pass, fail, etc) for the new disable method.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.16   17 Jun 2003 15:13:00   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  373
   SCR Title:  Add a new disable method that doesn't copy forward all result values
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Updated the new disable method code. Also, made Tool.Enabled accessible through the ActiveX.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.15   02 Jun 2003 15:06:56   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  352
   SCR Title:  Update SVObserver to Version 4.30 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Checked in some preliminary beta code for the new methode of disabling tools and toolsets.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.14   22 Apr 2003 16:55:48   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.13   27 Mar 2003 14:51:04   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  328
   SCR Title:  Upgrade SVObserver to MIL 7.1
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     Changed timer use from MappTimer to SVTimerInterfaceClass
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.12   31 Jan 2003 09:29:32   joe
   Project:  SVObserver
   Change Request (SCR) nbr:  301
   SCR Title:  Implement Result Image Circle Buffer
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated RebuildInputImages and processMessage methods to use the new image circle buffer methodology.  Added SetImageDepth method to use the new image circle buffer methodology.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.11   09 Dec 2002 20:14:30   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  295
   SCR Title:  Remove Result Data from Configuration Printout
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     CreateObject method was modified to properly update the isCreated flag and to set the printability of the internal value objects.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.10   20 Nov 2002 14:41:08   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  226
   SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Removed "SVPPQ.h", "SVSystem.h"
   Changes to CreateObject, Destroy, DestroyAt
   Added RebuildInputImages
   Copy Forward Counts in Run
   ValueObject Data Indexes
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.9   12 Nov 2001 10:14:54   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  238
   SCR Title:  Security Issue: Some users without the privlegy can execute "Move Tools".
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Added user priviledge check of is move operation is allowed in SetSelectedToolForOperatorMove.
   
   /////////////////////////////////////////////////////////////////////////////////////
*/
