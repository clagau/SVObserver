//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTool
//* .File Name       : $Workfile:   SVTool.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.9  $
//* .Check In Date   : $Date:   17 Dec 2014 10:48:04  $
//******************************************************************************

#include "stdafx.h"
#include "SVTool.h"
#include "SVImageLibrary/SVDrawContext.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVObjectLibrary/SVAnalyzerLevelCreateStruct.h"
#include "SVAnalyzer.h"
#include "SVConditional.h"
#include "SVGlobal.h"
#include "SVInspectionProcess.h"
#include "SVToolImage.h"
#include "SVToolSet.h"

SV_IMPLEMENT_CLASS( SVToolClass, SVToolClassGuid );

SVToolClass::SVToolClass( BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID /*= IDS_CLASSNAME_SVTOOL*/ )
: SVTaskObjectListClass( BCreateDefaultTaskList, POwner, StringResourceID )
{
	m_pToolConditional = NULL;

	init();
}

void SVToolClass::init()
{
	// Indentify our type in the Output List
	outObjectInfo.ObjectTypeInfo.ObjectType = SVToolObjectType;

	// Register Embedded Objects
	RegisterEmbeddedObject( &enabled, SVToolEnabledObjectGuid, IDS_OBJECTNAME_ENABLED, false, SVResetItemNone );
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

	/////////////////////////////////////////////////////////////////////////
	// Extents - These are shadows of the extents in the SVImageInfoClass,
	// so that they can be exposed and be used by other objects in the system.
	// They are not updated directly (at this point in time).
	/////////////////////////////////////////////////////////////////////////
	RegisterEmbeddedObject( &extentLeft, SVExtentRelativeLeftPositionObjectGuid, IDS_OBJECTNAME_EXTENT_LEFT, false, SVResetItemTool, _T("Extent X") );
	RegisterEmbeddedObject( &extentTop, SVExtentRelativeTopPositionObjectGuid, IDS_OBJECTNAME_EXTENT_TOP, false, SVResetItemTool, _T("Extent Y") );
	RegisterEmbeddedObject( &extentRight, SVExtentRelativeRightPositionObjectGuid, IDS_OBJECTNAME_EXTENT_RIGHT, false, SVResetItemNone, _T("Extent X") );
	RegisterEmbeddedObject( &extentBottom, SVExtentRelativeBottomPositionObjectGuid, IDS_OBJECTNAME_EXTENT_BOTTOM, false, SVResetItemNone, _T("Extent Y") );
	RegisterEmbeddedObject( &extentWidth, SVExtentWidthObjectGuid, IDS_OBJECTNAME_EXTENT_WIDTH, false, SVResetItemTool, _T("Extent Width") );
	RegisterEmbeddedObject( &extentHeight, SVExtentHeightObjectGuid, IDS_OBJECTNAME_EXTENT_HEIGHT, false, SVResetItemTool, _T("Extent Height") );
	RegisterEmbeddedObject( &extentHeightScaleFactor, 
							SVExtentHeightScaleFactorObjectGuid, 
							IDS_OBJECTNAME_EXTENT_HEIGHTSF, 
							false, 
							SVResetItemTool,
							_T("Extent Height Scale Factor") );

	RegisterEmbeddedObject( &extentWidthScaleFactor, 
							SVExtentWidthScaleFactorObjectGuid, 
							IDS_OBJECTNAME_EXTENT_WIDTHSF, 

							// the only place that this is ever set to true is for the color HSI 
							// conversion value (Color Tool) and it is probably not necessary there.
							false,   
							SVResetItemTool,
							_T("Extent Width Scale Factor") );

	RegisterEmbeddedObject( &ToolSelectedForOperatorMove, SVToolSelectedForOperatorMoveObjectGuid, IDS_OBJECTNAME_TOOL_SELECTED_FOR_OPERATOR_MOVE, false, SVResetItemNone);

	RegisterEmbeddedObject( &drawToolFlag, SVConditionalToolDrawFlagObjectGuid, IDS_OBJECTNAME_DRAWTOOL_FLAG, false, SVResetItemNone );

	RegisterEmbeddedObject( &m_svUpdateAuxilliaryExtents, SVUpdateAuxilliaryExtentsObjectGuid, IDS_OBJECTNAME_UPDATE_AUXILIARY_EXTENTS_OBJECT, false, SVResetItemTool );
	RegisterEmbeddedObject( &m_svAuxilliarySourceX, SVAuxilliarySourceXObjectGuid, IDS_OBJECTNAME_AUXILIARY_SOURCE_X, false, SVResetItemNone, _T("Extent X") );
	RegisterEmbeddedObject( &m_svAuxilliarySourceY, SVAuxilliarySourceYObjectGuid, IDS_OBJECTNAME_AUXILIARY_SOURCE_Y, false, SVResetItemNone, _T("Extent Y") );
	RegisterEmbeddedObject( &m_svAuxilliarySourceAngle, SVAuxilliarySourceAngleObjectGuid, IDS_OBJECTNAME_AUXILIARY_SOURCE_ANGLE, false, SVResetItemNone, _T("Extent Angle") );
	RegisterEmbeddedObject( &m_svAuxilliarySourceImageName, SVAuxilliarySourceImageNameObjectGuid, IDS_OBJECTNAME_AUXILIARY_SOURCE_IMAGE_NAME, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_svAuxilliaryDrawType, SVAuxilliaryDrawTypeObjectGuid, IDS_OBJECTNAME_AUXILIARY_DRAW_TYPE_NAME, false, SVResetItemNone );
	// Tool Comment...
	RegisterEmbeddedObject( &m_svToolComment, SVToolCommentTypeObjectGuid, IDS_OBJECTNAME_TOOL_COMMENT, false, SVResetItemNone );
	
	m_svToolExtent.SetTool( this );
	m_svToolExtent.SetImageType( SVImageTypeLogicalAndPhysical );
	m_svToolExtent.SetTranslation( SVExtentTranslationShift );
	m_svToolExtent.SetAlwaysUpdate( false );
	m_svToolExtent.SetExtentObject( SVExtentPropertyPositionPointX, &extentLeft );
	m_svToolExtent.SetExtentObject( SVExtentPropertyPositionPointY, &extentTop );
	m_svToolExtent.SetExtentObject( SVExtentPropertyWidth, &extentWidth );
	m_svToolExtent.SetExtentObject( SVExtentPropertyHeight, &extentHeight );
	m_svToolExtent.SetExtentObject( SVExtentPropertyWidthScaleFactor, &extentWidthScaleFactor );
	m_svToolExtent.SetExtentObject( SVExtentPropertyHeightScaleFactor, &extentHeightScaleFactor );

	//
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

	ToolTime.SetDefaultValue(0,TRUE);

	/////////////////////////////////////////////////////////////////////////
	// Set Default values for Shadowed Extents
	/////////////////////////////////////////////////////////////////////////
	extentLeft.SetDefaultValue( SV_DEFAULT_WINDOWTOOL_LEFT, TRUE );
	extentTop.SetDefaultValue( SV_DEFAULT_WINDOWTOOL_TOP, TRUE );
	extentRight.SetDefaultValue( SV_DEFAULT_WINDOWTOOL_LEFT + SV_DEFAULT_WINDOWTOOL_WIDTH, TRUE );
	extentBottom.SetDefaultValue( SV_DEFAULT_WINDOWTOOL_TOP + SV_DEFAULT_WINDOWTOOL_HEIGHT, TRUE );
	extentWidth.SetDefaultValue( SV_DEFAULT_WINDOWTOOL_WIDTH, TRUE );
	extentHeight.SetDefaultValue( SV_DEFAULT_WINDOWTOOL_HEIGHT, TRUE );
	extentWidthScaleFactor.SetDefaultValue( SV_DEFAULT_WINDOWTOOL_WIDTHSCALEFACTOR, TRUE );
	extentHeightScaleFactor.SetDefaultValue( SV_DEFAULT_WINDOWTOOL_HEIGHTSCALEFACTOR, TRUE );

//	ToolSelectedForOperatorMove.SetDefaultValue( 0, FALSE );

	drawToolFlag.SetEnumTypes( IDS_TOOLDRAW_ENUMOBJECT_LIST );
	drawToolFlag.SetDefaultValue( ( const long ) 0, TRUE ); // 0 Should be show tool 'Always'

	m_svUpdateAuxilliaryExtents.SetDefaultValue( FALSE, TRUE );

	m_svAuxilliarySourceX.SetDefaultValue( 0.0, TRUE );
	m_svAuxilliarySourceY.SetDefaultValue( 0.0, TRUE );
	m_svAuxilliarySourceAngle.SetDefaultValue( 0.0, TRUE );
	m_svAuxilliarySourceImageName.SetDefaultValue( "", TRUE );
	m_svAuxilliaryDrawType.SetDefaultValue( "", TRUE );

	pCurrentToolSet					= NULL;
	pPropertyArray					= NULL;
	propertyCount					= 0;

	// Auxiliary Source Image.
	m_AuxSourceImageObjectInfo.SetInputObjectType( SVImageObjectType );
	m_AuxSourceImageObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_AuxSourceImageObjectInfo, _T( "ToolAuxSourceImage" ) );

	// instaniate the Conditional class
	SVConditionalClass* pCondition = new SVConditionalClass( this );
	AddFriend( pCondition->GetUniqueObjectID() );

	// Identify our input type needs
	inputConditionBoolObjectInfo.SetInputObjectType( SVConditionalResultObjectGuid, SVBoolValueObjectType );
	inputConditionBoolObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputConditionBoolObjectInfo, _T( "ToolConditionalValue" ) );

	// 
	addDefaultInputObjects();
}

SVToolClass::~SVToolClass()
{
}

BOOL SVToolClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = FALSE;

	if( SVTaskObjectListClass::CreateObject( PCreateStructure ) )
	{
		if( GetInspection() )
		{
			bOk = TRUE;

			pCurrentToolSet = GetInspection()->GetToolSet();
		}
	}

	for( size_t j = 0;nullptr ==  m_pToolConditional && j < friendList.size(); j++ )
	{
		m_pToolConditional = dynamic_cast<SVConditionalClass *>(friendList[j].PObject);
	}

	// Set / Reset Printable Flags
	enabled.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_SETABLE_ONLINE | SV_REMOTELY_SETABLE;
	passed.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	failed.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	warned.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	explicitFailed.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;

	passedCount.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	failedCount.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	warnedCount.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;

	enabledCount.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	processedCount.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	ToolTime.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;

	extentLeft.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_SETABLE_ONLINE | SV_REMOTELY_SETABLE | SV_EXTENT_OBJECT;
	extentTop.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_SETABLE_ONLINE | SV_REMOTELY_SETABLE | SV_EXTENT_OBJECT;
	extentRight.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	extentBottom.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	extentWidth.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_REMOTELY_SETABLE | SV_EXTENT_OBJECT | SV_SETABLE_ONLINE;
	extentHeight.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_REMOTELY_SETABLE | SV_EXTENT_OBJECT | SV_SETABLE_ONLINE;

	// Auxilliary Tool Source Extent
	m_svUpdateAuxilliaryExtents.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_SETABLE_ONLINE | SV_REMOTELY_SETABLE ;
	
//	ToolSelectedForOperatorMove.ObjectAttributesAllowedRef() = SV_NO_ATTRIBUTES; //Clear Attributes ... No longer used.
	drawToolFlag.ObjectAttributesAllowedRef() |= SV_PRINTABLE;

	// Tool Comment Atributes...
	m_svToolComment.ObjectAttributesAllowedRef() |= SV_PRINTABLE ;
	m_svToolComment.ObjectAttributesAllowedRef() &= ~SV_VIEWABLE ;	// We do not want this to show up in the results picker.

	isCreated = bOk;

	return bOk;
}

BOOL SVToolClass::CloseObject()
{
	m_svToolExtent.SetToolImage( NULL );
	m_svToolExtent.SetSelectedImage( NULL );

	return SVTaskObjectListClass::CloseObject();
}

BOOL SVToolClass::DisconnectInput( SVInObjectInfoStruct* pInObjectInfo )
{
	BOOL l_Status( SVTaskObjectListClass::DisconnectInput( pInObjectInfo ) && pInObjectInfo != NULL );

	if( pInObjectInfo != NULL )
	{
		if( pInObjectInfo->GetInputObjectInfo().PObject == m_svToolExtent.GetToolImage() )
		{
			m_svToolExtent.SetToolImage( NULL );
		}

		if( pInObjectInfo->GetInputObjectInfo().PObject == m_svToolExtent.GetSelectedImage() )
		{
			m_svToolExtent.SetSelectedImage( NULL );
		}
	}

	return l_Status;
}

bool SVToolClass::IsEnabled() const
{
	bool bEnabled = true;
	enabled.GetValue( bEnabled );

	return bEnabled;
}

bool SVToolClass::IsEnabled(long p_lIndex) const
{
	bool bEnabled = true;
	enabled.GetValue( p_lIndex, bEnabled );
	return bEnabled;
}

bool SVToolClass::WasEnabled() const
{
	bool bEnabled = true;

	if( pCurrentToolSet != NULL )
	{
		bEnabled = pCurrentToolSet->WasEnabled();
	}

	if (bEnabled)
	{
		bEnabled = IsEnabled();

		if (bEnabled)
		{
			bEnabled = ( getConditionalResult() != FALSE );
		}
	}

	return bEnabled;
}

BOOL SVToolClass::isFreeMoveable()
{
	return FALSE;
}

HRESULT SVToolClass::GetDrawInfo( SVExtentMultiLineStruct& p_rMultiLine )
{
	HRESULT l_Status = S_OK;

	if( pCurrentToolSet )
	{
		SVEnumerateValueObjectClass* pSetEnum = pCurrentToolSet->GetDrawFlagObject();

		if( pSetEnum )
		{
			pSetEnum->GetValue( p_rMultiLine.m_ToolSetDrawFlag );
		}
	}
	
	drawToolFlag.GetValue( p_rMultiLine.m_ToolDrawFlag );

	BOOL bPassed = TRUE;
	BOOL bFailed = TRUE;
	BOOL bWarned = TRUE;

	passed.GetValue( bPassed );
	failed.GetValue( bFailed );
	warned.GetValue( bWarned );

	p_rMultiLine.m_Passed = ( bPassed != FALSE );
	p_rMultiLine.m_Failed = ( bFailed != FALSE );
	p_rMultiLine.m_Warned = ( bWarned != FALSE );

	return l_Status;
}

SVToolPropertyEntryStruct* SVToolClass::GetSpecialPropertyList( int& RCount )
{
	RCount = propertyCount;
	return pPropertyArray;
}

BOOL SVToolClass::OnValidate()
{
	BOOL bRetVal = FALSE;
	
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

BOOL SVToolClass::Run( SVRunStatusClass& RRunStatus )
{
	DWORD retVal = 0;
	long lCount;
	BOOL bIsValid = FALSE;

	ToolTime.Start();

	if( !GetInspection()->GetNewDisableMethod() )
	{
		// First Set the old stuff forward for the counts
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

		if( IsEnabled() )
		{
			SVImageClassPtrSet::iterator l_oImageIter;

			for( l_oImageIter = m_svImageObjectSet.begin(); l_oImageIter != m_svImageObjectSet.end(); ++l_oImageIter )
			{
				(*l_oImageIter)->CopyImageTo( RRunStatus.Images );
			}

			if( RRunStatus.m_UpdateCounters )
			{
				// Set Processed Count...
				long lCount = 0;
				processedCount.GetValue( lCount );
				processedCount.SetValue( RRunStatus.m_lResultDataIndex, ++lCount );
			}

			retVal = SVTaskObjectListClass::Run( RRunStatus );

			if( retVal )
			{
				RRunStatus.SetValid();
			}
			else
			{
				RRunStatus.SetInvalid();
				SetInvalid();
			}
		}
		else
		{
			retVal = !( RRunStatus.m_UpdateCounters );

			RRunStatus.SetDisabled();
			// To set disabled color for any part of this tool ( embeddeds, children, etc. )
			SetDisabled();
		}

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

			// set our state according to the runStatus
			passed.SetValue( RRunStatus.m_lResultDataIndex, RRunStatus.IsPassed() );
			failed.SetValue( RRunStatus.m_lResultDataIndex, RRunStatus.IsFailed() );
			explicitFailed.SetValue( RRunStatus.m_lResultDataIndex, RRunStatus.IsFailed() );
			warned.SetValue( RRunStatus.m_lResultDataIndex, RRunStatus.IsWarned() );

			if( RRunStatus.m_UpdateCounters )
			{
				// Set Counts...
				lCount = 0;
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
		retVal = RunWithNewDisable( RRunStatus );
	}// end else


	//
	if( GetInspection()->GetEnableAuxiliaryExtent() )
	{
		BOOL l_bUpdateSourceExtents = FALSE;

		bool l_bForceOffsetReset = GetInspection()->m_bForceOffsetUpdate;

		m_svToolExtent.UpdateOffsetData( l_bForceOffsetReset );

		if( m_svUpdateAuxilliaryExtents.GetValue( l_bUpdateSourceExtents ) == S_OK && l_bUpdateSourceExtents )
		{
			SVExtentOffsetStruct l_svOffsetData;

			if( m_svToolExtent.GetSelectedOffsetData( l_svOffsetData ) != S_OK || ! l_svOffsetData.m_bIsLinear )
			{
				if( m_svToolExtent.UpdateOffsetDataToImage( l_svOffsetData, GetAuxSourceImage() ) == S_OK )
				{
					if( GetAuxSourceImage() != NULL )
					{
						l_svOffsetData.m_csImageName = GetAuxSourceImage()->GetCompleteObjectName();
					}
				}
			}

			CString l_strDrawType;
			m_svToolExtent.GetAuxilliaryDrawTypeString( l_strDrawType );
			m_svAuxilliaryDrawType.SetValue( RRunStatus.m_lResultDataIndex, l_strDrawType );
			m_svAuxilliarySourceImageName.SetValue( RRunStatus.m_lResultDataIndex, l_svOffsetData.m_csImageName );
			m_svAuxilliarySourceX.SetValue( RRunStatus.m_lResultDataIndex, l_svOffsetData.m_svOffset.m_dPositionX );
			m_svAuxilliarySourceY.SetValue( RRunStatus.m_lResultDataIndex, l_svOffsetData.m_svOffset.m_dPositionY );
			m_svAuxilliarySourceAngle.SetValue( RRunStatus.m_lResultDataIndex, l_svOffsetData.m_dRotationAngle );
		}
	}

	ToolTime.Stop( RRunStatus.m_lResultDataIndex );

	return retVal;
}// end Run

BOOL SVToolClass::RunWithNewDisable( SVRunStatusClass& RRunStatus )
{
	DWORD dwRet = 0;
	long lCount = 0;
	
	BOOL bDisabled = FALSE;

	if( IsEnabled() )
	{
		SVValueObjectClassPtrSet::iterator l_oValueIter;

		for( l_oValueIter = m_svValueObjectSet.begin(); l_oValueIter != m_svValueObjectSet.end(); ++l_oValueIter )
		{
			(*l_oValueIter)->CopyLastSetValue( RRunStatus.m_lResultDataIndex );
		}

		SVImageClassPtrSet::iterator l_oImageIter;

		for( l_oImageIter = m_svImageObjectSet.begin(); l_oImageIter != m_svImageObjectSet.end(); ++l_oImageIter )
		{
			(*l_oImageIter)->CopyImageTo( RRunStatus.Images );
		}

		if( m_pToolConditional )
			m_pToolConditional->onRun( RRunStatus );

		if( !m_pToolConditional || getConditionalResult() )
		{
			if( RRunStatus.m_UpdateCounters )
			{
				// Set Processed Count...
				processedCount.GetValue( lCount );
				processedCount.SetValue( RRunStatus.m_lResultDataIndex, ++lCount );
			}

			dwRet = SVTaskObjectListClass::Run( RRunStatus );

			if( dwRet )
			{
				RRunStatus.SetValid();
			}
			else
			{
				RRunStatus.SetInvalid();
				SetInvalid();
			}// end else

		}// end if
		else
		{
			RRunStatus.SetDisabledByCondition();
			bDisabled = TRUE;
		}// end else

	}// end if
	else
	{
		dwRet = !( RRunStatus.m_UpdateCounters );

		RRunStatus.SetDisabled();
		bDisabled = TRUE;
	}// end else

	// if disabled or disabled by condition
	// leave in previous state
	if( !bDisabled && !RRunStatus.IsDisabled() && !RRunStatus.IsDisabledByCondition() )
	{
		if( RRunStatus.m_UpdateCounters )
		{
			// Set Enabled Count...
			enabledCount.GetValue( lCount );
			enabledCount.SetValue( RRunStatus.m_lResultDataIndex, ++lCount );
		}

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
	else
	{
		// Set Enabled Count...
		enabledCount.GetValue( lCount );
		enabledCount.SetValue( RRunStatus.m_lResultDataIndex, lCount );
	}// end else
	
	// Get Status Color...
	DWORD dwValue = RRunStatus.GetStatusColor();
	statusColor.SetValue( RRunStatus.m_lResultDataIndex, dwValue );

	// Get Status...
	dwValue = RRunStatus.GetState();
	statusTag.SetValue( RRunStatus.m_lResultDataIndex, dwValue );

	return dwRet;
}// end RunWithNewDisable

BOOL SVToolClass::Validate()
{
	if( !IsEnabled() )
	{
		SetDisabled();
		//return TRUE;
	}// end if
	
	return SVTaskObjectListClass::Validate();
}

BOOL SVToolClass::onRun( SVRunStatusClass& RRunStatus )
{
	BOOL bRetVal = SVTaskObjectListClass::onRun( RRunStatus );
	if( bRetVal )
	{
		if( (extentTop.ObjectAttributesAllowed() & SV_NO_ATTRIBUTES) != SV_NO_ATTRIBUTES )
			bRetVal = ( extentTop.CopyLastSetValue( RRunStatus.m_lResultDataIndex ) == S_OK ) && bRetVal;
		if( (extentLeft.ObjectAttributesAllowed() & SV_NO_ATTRIBUTES) != SV_NO_ATTRIBUTES )
			bRetVal = ( extentLeft.CopyLastSetValue( RRunStatus.m_lResultDataIndex ) == S_OK ) && bRetVal;
		if( (extentWidth.ObjectAttributesAllowed() & SV_NO_ATTRIBUTES) != SV_NO_ATTRIBUTES )
			bRetVal = ( extentWidth.CopyLastSetValue( RRunStatus.m_lResultDataIndex ) == S_OK ) && bRetVal;
		if( (extentHeight.ObjectAttributesAllowed() & SV_NO_ATTRIBUTES) != SV_NO_ATTRIBUTES )
			bRetVal = ( extentHeight.CopyLastSetValue( RRunStatus.m_lResultDataIndex ) == S_OK ) && bRetVal;

		// Friends were running, validation was successfully
		// Check conditional execution
		if ( !getConditionalResult() )
		{
			RRunStatus.SetDisabledByCondition();
		}
	}

	return bRetVal;
}

bool SVToolClass::getConditionalResult() const
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

bool SVToolClass::getConditionalResult(long p_lIndex) const
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

DWORD_PTR SVToolClass::processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext )
{
	DWORD_PTR DwResult = NULL;

	SVToolLevelCreateStruct createStruct;

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
			if( !IsCreated() && !CreateObject( ( SVObjectLevelCreateStruct* ) DwMessageValue ) )
			{
				ASSERT( FALSE );

				CString sMsg;
				sMsg.Format("Creation of %s \"%s\" failed.", GetObjectName(), GetCompleteObjectName());
				ASSERT(FALSE);  // this makes it easier to get to this point in the code in the debugger
				AfxMessageBox(sMsg);

				DwResult = SVMR_NO_SUCCESS;
			}
			else
			{
				DwResult = SVMR_SUCCESS;
			}

			createStruct.OwnerObjectInfo = this;
			createStruct.ToolObjectInfo	= this;
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
			createStruct.ToolObjectInfo	= this;
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
	case SVMSGID_CONNECT_CHILD_OBJECT:
		{
			// ...use second message parameter ( DwMessageValue ) as SVObjectClass* of the child object
			SVObjectClass* pChildObject = reinterpret_cast<SVObjectClass*>(DwMessageValue);

			createStruct.OwnerObjectInfo        = this;
			createStruct.ToolObjectInfo			= this;
			createStruct.InspectionObjectInfo	= GetInspection();

			return SVSendMessage( pChildObject, SVM_CONNECT_ALL_OBJECTS, reinterpret_cast<DWORD_PTR>(&createStruct), NULL );
		}
	}

	DWORD_PTR l_Status = ( SVTaskObjectListClass::processMessage( DwMessageID, DwMessageValue, DwMessageContext ) | DwResult );

	return l_Status;
}

HRESULT SVToolClass::GetImageExtentProperty( SVExtentPropertyEnum p_eProperty, SVValueObjectClass *&p_rpsvValue )
{
	return m_svToolExtent.GetExtentObject( p_eProperty, p_rpsvValue );
}

HRESULT SVToolClass::SetImageExtentProperty( SVExtentPropertyEnum p_eProperty, SVValueObjectClass *p_psvValue )
{
	return m_svToolExtent.SetExtentObject( p_eProperty, p_psvValue );
}

HRESULT SVToolClass::GetRootOffsetData( SVExtentOffsetStruct& p_rsvOffsetData )
{
	return m_svToolExtent.GetRootOffsetData( p_rsvOffsetData );
}

HRESULT SVToolClass::UpdateOffsetData( SVImageClass* p_svToolImage )
{
	if( GetInspection() != NULL )
	{
		GetInspection()->m_bForceOffsetUpdate = true;
	}

	return m_svToolExtent.UpdateOffsetData( true, p_svToolImage );
}

HRESULT SVToolClass::TranslatePointToSource( SVExtentPointStruct p_svIn, SVExtentPointStruct& p_rsvOut )
{
	return m_svToolExtent.TranslatePointToSource( p_svIn, p_rsvOut );
}

HRESULT SVToolClass::EnableAuxilliaryExtents( bool p_bEnable )
{
	HRESULT l_hr = S_OK;

	DWORD l_dwAttributes = SV_VIEWABLE
							| SV_ARCHIVABLE
							| SV_SELECTABLE_FOR_EQUATION
							| SV_SELECTABLE_FOR_STATISTICS
							| SV_PUBLISHABLE;

	if( p_bEnable )
	{
		m_svAuxilliarySourceX.ObjectAttributesAllowedRef() |= l_dwAttributes ;
		m_svAuxilliarySourceY.ObjectAttributesAllowedRef() |= l_dwAttributes ;
		m_svAuxilliarySourceAngle.ObjectAttributesAllowedRef() |= l_dwAttributes ;
		m_svAuxilliaryDrawType.ObjectAttributesAllowedRef() |= l_dwAttributes ;
		m_svAuxilliarySourceImageName.ObjectAttributesAllowedRef() |= l_dwAttributes;

		SetBits( m_svAuxilliarySourceX.ObjectAttributesAllowedRef(), SV_HIDDEN, false );
		SetBits( m_svAuxilliarySourceY.ObjectAttributesAllowedRef(), SV_HIDDEN, false );
		SetBits( m_svAuxilliarySourceAngle.ObjectAttributesAllowedRef(), SV_HIDDEN, false );
		SetBits( m_svAuxilliaryDrawType.ObjectAttributesAllowedRef(), SV_HIDDEN, false );
		SetBits( m_svAuxilliarySourceImageName.ObjectAttributesAllowedRef(), SV_HIDDEN, false );
	}
	else
	{
		m_svAuxilliarySourceX.ObjectAttributesAllowedRef() &= ~l_dwAttributes ;
		m_svAuxilliarySourceY.ObjectAttributesAllowedRef() &= ~l_dwAttributes ;
		m_svAuxilliarySourceAngle.ObjectAttributesAllowedRef() &= ~l_dwAttributes ;
		m_svAuxilliaryDrawType.ObjectAttributesAllowedRef() &= ~l_dwAttributes ;
		m_svAuxilliarySourceImageName.ObjectAttributesAllowedRef() &= ~l_dwAttributes;

		m_svAuxilliarySourceX.ObjectAttributesSetRef() &= (UINT)~l_dwAttributes ;
		m_svAuxilliarySourceY.ObjectAttributesSetRef() &= (UINT)~l_dwAttributes ;
		m_svAuxilliarySourceAngle.ObjectAttributesSetRef() &= (UINT)~l_dwAttributes ;
		m_svAuxilliaryDrawType.ObjectAttributesSetRef() &= (UINT)~l_dwAttributes ;
		m_svAuxilliarySourceImageName.ObjectAttributesSetRef() &= (UINT)~l_dwAttributes;

		SetBits( m_svAuxilliarySourceX.ObjectAttributesAllowedRef(), SV_HIDDEN, true );
		SetBits( m_svAuxilliarySourceY.ObjectAttributesAllowedRef(), SV_HIDDEN, true );
		SetBits( m_svAuxilliarySourceAngle.ObjectAttributesAllowedRef(), SV_HIDDEN, true );
		SetBits( m_svAuxilliaryDrawType.ObjectAttributesAllowedRef(), SV_HIDDEN, true );
		SetBits( m_svAuxilliarySourceImageName.ObjectAttributesAllowedRef(), SV_HIDDEN, true );
	}
	return l_hr;
}


HRESULT SVToolClass::ResetObject()
{
	HRESULT l_hrOk = S_OK;
		
	if( SVTaskObjectListClass::ResetObject() != S_OK )
	{
		l_hrOk = S_FALSE;
	}

	bool l_bReset = false;
	if ( GetInspection() != NULL && 
		 GetInspection()->GetToolSet() != NULL && 
		 (GetInspection()->GetToolSet()->m_bvoResetCounts.GetValue( l_bReset ) == S_OK ) &&
		 l_bReset )
	{
		// Reset Counter...
		passedCount.SetDefaultValue( 0, TRUE );
		failedCount.SetDefaultValue( 0, TRUE );
		warnedCount.SetDefaultValue( 0, TRUE );
		enabledCount.SetDefaultValue( 0, TRUE );
		processedCount.SetDefaultValue( 0, TRUE );
	}

	
	///UpdateBottomAndRight is called again when imageExtents are changed by ToolsizeAdjust
	if ( l_hrOk == S_OK )
	{
		UpdateBottomAndRight();
	}

	// Auxilliary Extents
	double l_dValue = 0;
	
	if( GetInspection()->GetEnableAuxiliaryExtent() )
	{
		m_svUpdateAuxilliaryExtents.ObjectAttributesAllowedRef() |=  SV_VIEWABLE 
			| SV_ARCHIVABLE
			| SV_SELECTABLE_FOR_EQUATION
			| SV_SELECTABLE_FOR_STATISTICS 
			| SV_PUBLISHABLE;
		m_svUpdateAuxilliaryExtents.GetValue( l_dValue );

		SetBits( m_svUpdateAuxilliaryExtents.ObjectAttributesAllowedRef(), SV_HIDDEN, false );
	}
	else
	{
		m_svUpdateAuxilliaryExtents.ObjectAttributesAllowedRef() &= ~( SV_VIEWABLE 
			| SV_ARCHIVABLE
			| SV_SELECTABLE_FOR_EQUATION
			| SV_SELECTABLE_FOR_STATISTICS 
			| SV_PUBLISHABLE);

		m_svUpdateAuxilliaryExtents.ObjectAttributesSetRef() &= ~( SV_VIEWABLE 
			| SV_ARCHIVABLE
			| SV_SELECTABLE_FOR_EQUATION
			| SV_SELECTABLE_FOR_STATISTICS 
			| SV_PUBLISHABLE);

		SetBits( m_svUpdateAuxilliaryExtents.ObjectAttributesAllowedRef(), SV_HIDDEN, true );
	}

	EnableAuxilliaryExtents( l_dValue > 0 );

	return l_hrOk;
}

void SVToolClass::UpdateBottomAndRight()
{
	SVImageExtentClass l_svExtents;

	if ( GetImageExtent( l_svExtents ) == S_OK )
	{
		SVExtentFigureStruct l_svFigure;

		if ( l_svExtents.GetFigure( l_svFigure ) == S_OK )
		{
			if( extentRight.ObjectAttributesAllowed() != SV_NO_ATTRIBUTES )
			{
				long l_lValue = static_cast<long>(l_svFigure.m_svBottomRight.m_dPositionX);

				extentRight.SetValue( 1, l_lValue );
			}

			if( extentBottom.ObjectAttributesAllowed() != SV_NO_ATTRIBUTES )
			{
				long l_lValue = static_cast<long>(l_svFigure.m_svBottomRight.m_dPositionY);

				extentBottom.SetValue( 1, l_lValue );
			}
		}
	}
}

const SVImageClass* SVToolClass::GetToolImage() const
{
	return m_svToolExtent.GetToolImage();
}

void SVToolClass::SetToolImage( SVImageClass* p_pExtentImage )
{
	m_svToolExtent.SetToolImage( p_pExtentImage );
}

void SVToolClass::SetAlwaysUpdate( bool p_bAlwaysUpdate )
{
	m_svToolExtent.SetAlwaysUpdate( p_bAlwaysUpdate );
}

HRESULT SVToolClass::UpdateOffsetDataToImage( SVExtentOffsetStruct& p_rsvOffsetData, SVImageClass* p_svToolImage ) 
{
	return m_svToolExtent.UpdateOffsetDataToImage( p_rsvOffsetData, p_svToolImage );
}
 
HRESULT SVToolClass::UpdateImageWithExtent( unsigned long p_Index )
{
	HRESULT l_Status = S_OK;

	l_Status = m_svToolExtent.UpdateImageWithExtent( p_Index, SVToolExtentClass::SVToolExtent );

	return l_Status;
}

HRESULT SVToolClass::GetParentExtent( SVImageExtentClass& p_rParent ) const
{
	HRESULT l_hr = S_OK;
	m_svToolExtent.GetParentExtent( p_rParent );
	return l_hr;
}


BOOL SVToolClass::PrepareForRunning()
{
	return TRUE;
}


BOOL SVToolClass::IsOkToEdit()
{
	return TRUE;
}

EAutoSize SVToolClass::GetAutoSizeEnabled()
{
	return (EnableSizeAndPosition);
}



BOOL SVToolClass::SetDefaultFormulas()
{
	return TRUE;
}

SVImageTypeEnum SVToolClass::GetImageType()
{
	return m_svToolExtent.GetImageType();
}

HRESULT SVToolClass::GetImageExtent( SVImageExtentClass &p_rsvImageExtent )
{
	HRESULT l_hrOk = S_OK;

	l_hrOk = m_svToolExtent.GetImageExtent( p_rsvImageExtent );

	return l_hrOk;
}

HRESULT SVToolClass::GetImageExtent( unsigned long p_ulIndex, SVImageExtentClass &p_rsvImageExtent )
{
	HRESULT l_hrOk = S_FALSE;

	l_hrOk = m_svToolExtent.GetImageExtent( p_ulIndex, p_rsvImageExtent );

	return l_hrOk;
}

HRESULT SVToolClass::SetImageExtent( unsigned long p_ulIndex, SVImageExtentClass p_svImageExtent )
{
	HRESULT l_hrOk = S_FALSE;

	l_hrOk = m_svToolExtent.SetImageExtent( p_ulIndex, p_svImageExtent );

	return l_hrOk;
}

void SVToolClass::removeEmbeddedExtents( bool p_DisconnectExtents )
{
	if( p_DisconnectExtents )
	{
		m_svToolExtent.SetExtentObject( SVExtentPropertyPositionPointX, NULL );
		m_svToolExtent.SetExtentObject( SVExtentPropertyPositionPointY, NULL );
		m_svToolExtent.SetExtentObject( SVExtentPropertyWidth, NULL );
		m_svToolExtent.SetExtentObject( SVExtentPropertyHeight, NULL );
	}

	hideEmbeddedObject( extentLeft );	 // Make it Un-Selectable for anything
	RemoveEmbeddedObject( &extentLeft ); // Remove it from the Embedded List so it is not scripted

	hideEmbeddedObject( extentTop );
	RemoveEmbeddedObject( &extentTop );

	hideEmbeddedObject( extentRight );
	RemoveEmbeddedObject( &extentRight );

	hideEmbeddedObject( extentBottom );
	RemoveEmbeddedObject( &extentBottom );

	hideEmbeddedObject( extentWidth );
	RemoveEmbeddedObject( &extentWidth );

	hideEmbeddedObject( extentHeight );
	RemoveEmbeddedObject( &extentHeight );
}


// this function will check the existence of the drive
// szPath paramter should be in the form of c:\xxxxxx\xxxx\xx\xxx etc
BOOL SVToolClass::ValidateDrive(LPCTSTR szFilePath, CString& szDrv /* = NULL*/)
{
	TCHAR szDrive[_MAX_DRIVE], szDir[_MAX_DIR], szFName[_MAX_FNAME], szExt[_MAX_EXT];

	//Get the drive text
	_tsplitpath (szFilePath, szDrive, szDir, szFName, szExt);

	if(szDrv)szDrv = szDrive;

	if( _access( szDrive, 0 ))return FALSE;
	return TRUE;
}

HRESULT SVToolClass::GetFilteredImageExtentPropertyList( SVExtentPropertyListType& p_rPropertyList )
{
	HRESULT hr = S_OK;

	p_rPropertyList.clear();
	m_svToolExtent.GetFilteredImageExtentPropertyList( p_rPropertyList );

	// objects - on ResetObject - set toolextent flag
	return hr;
}

HRESULT SVToolClass::GetPropertyInfo( SVExtentPropertyEnum p_eProperty, SVExtentPropertyInfoStruct& p_rInfo ) const
{
	return m_svToolExtent.GetExtentPropertyInfo( p_eProperty, p_rInfo );
}



HRESULT SVToolClass::SetExtentPropertyInfo( SVExtentPropertyEnum p_eProperty, const SVExtentPropertyInfoStruct& p_rInfo )
{
	return m_svToolExtent.SetExtentPropertyInfo( p_eProperty, p_rInfo);
}



HRESULT SVToolClass::UpdateOverlayIDs( SVExtentMultiLineStruct& p_rMultiLine )
{
	HRESULT l_Status = SVTaskObjectListClass::UpdateOverlayIDs( p_rMultiLine );

	if( l_Status == S_OK )
	{
		if( p_rMultiLine.m_ToolID.empty() )
		{
			p_rMultiLine.m_ToolID = GetUniqueObjectID();
		}
	}

	return l_Status;
}

HRESULT SVToolClass::CollectOverlays( SVImageClass *p_Image, SVExtentMultiLineStructCArray &p_MultiLineArray )
{
	HRESULT l_Status = S_OK;

	if( IsInputImage( p_Image ) == S_OK )
	{
		l_Status = SVTaskObjectListClass::CollectOverlays( p_Image, p_MultiLineArray );
	}

	return l_Status;
}

void SVToolClass::UpdateTaskObjectOutputListAttributes( SVObjectReference refTarget, UINT uAttributes )
{
	SVToolSetClass* pToolSet = GetInspection()->GetToolSet();
	ASSERT( pToolSet );
	SVOutputInfoListClass l_ToolSetOutputList;
	SVObjectReferenceVector vecObjects;
	
	pToolSet->GetOutputList( l_ToolSetOutputList );
	
	l_ToolSetOutputList.GetObjectReferenceList( vecObjects );

	//
	// Iterate the toolset output list and reset the attribute
	//
	int nCount = static_cast<int>(vecObjects.size());
	for (int i = 0; i < nCount; i++)
	{
		SVObjectReference ref = vecObjects.at(i);
		
		// Clear the existing bit
		ref.ObjectAttributesSetRef() &= ~uAttributes;
		
		// Compare and set the attribute set if a match.
		if ( ref == refTarget )
		{
			if ( ref.ObjectAttributesAllowed() & uAttributes )
			{
				ref.ObjectAttributesSetRef() |= uAttributes;
			}
		}
	}
}

// Auxiliary Source Image Functions
HRESULT SVToolClass::GetSourceImages( SVImageListClass* p_psvImageList )
{
	HRESULT l_hr = S_OK;


	SVImageClass* l_psvImageParent = NULL;
	SVToolClass* l_psvTool = this;
	if( m_svToolExtent.GetToolImage() )
	{
		l_psvImageParent = m_svToolExtent.GetToolImage()->GetParentImage();
		if( l_psvImageParent != NULL )
		{
			 l_psvTool = l_psvImageParent->GetTool(); //dynamic_cast<SVToolClass*>( l_psvImageParent->GetTool() );

			 if( l_psvTool != NULL && l_psvTool != this )
			 {
				 l_psvTool->GetSourceImages( p_psvImageList );
			 }

			 p_psvImageList->Add( l_psvImageParent );
		}
	}
	else
	{
		l_hr = -77001;
	}

	return l_hr;
}

SVImageClass* SVToolClass::GetAuxSourceImage()
{
	SVImageClass* l_pImage = NULL;
	if( m_AuxSourceImageObjectInfo.IsConnected() && m_AuxSourceImageObjectInfo.GetInputObjectInfo().PObject )
		l_pImage = static_cast<SVImageClass*>(m_AuxSourceImageObjectInfo.GetInputObjectInfo().PObject) ;
	return l_pImage;
}

HRESULT SVToolClass::SetAuxSourceImage( SVImageClass* p_psvImage )
{
	HRESULT l_hr = S_FALSE;

	SVImageListClass l_svImageList;

	if( GetSourceImages( &l_svImageList ) == S_OK )
	{
		SVImageClass* l_psvImage = NULL;

		long l_lCount = l_svImageList.GetSize();

		if( 0 < l_lCount )
		{
			l_psvImage = l_svImageList.GetAt( 0 );

			for( int i = l_lCount - 1; l_hr != S_OK && i > 0 ; i-- )
			{
				if( l_svImageList.GetAt( i ) == p_psvImage )
				{
					l_psvImage = l_svImageList.GetAt( i );

					l_hr = S_OK;
				}
			}
		}

		::KeepPrevError( l_hr, ConnectToImage( &m_AuxSourceImageObjectInfo, l_psvImage ) );

		m_svToolExtent.SetSelectedImage( GetAuxSourceImage() );

		if ( GetInspection() != NULL )
		{
			GetInspection()->m_bForceOffsetUpdate = true;
		}
	}

	return l_hr;
}

HRESULT SVToolClass::IsAuxInputImage( const SVInObjectInfoStruct* p_psvInfo )
{
	HRESULT l_hrOk = S_FALSE;

	if( p_psvInfo == &m_AuxSourceImageObjectInfo )
	{
		l_hrOk = S_OK;
	}

	return l_hrOk;
}

HRESULT SVToolClass::CollectInputImageNames( SVRunStatusClass& RRunStatus )
{
	return S_FALSE;
}

HRESULT SVToolClass::GetInputImageNames( SVStringValueObjectClass*& p_pSourceNames )
{
	p_pSourceNames = NULL;
	return S_FALSE;
}

SVValueObjectClass* SVToolClass::GetToolComment()
{
	return &m_svToolComment;
}

DWORD_PTR SVToolClass::createAllObjectsFromChild( SVObjectClass* pChildObject )
{
	SVToolLevelCreateStruct createStruct;
	createStruct.OwnerObjectInfo        = this;
	createStruct.ToolObjectInfo			= this;
	createStruct.InspectionObjectInfo	= GetInspection();

	return SVSendMessage( pChildObject, SVM_CREATE_ALL_OBJECTS, reinterpret_cast<DWORD_PTR>(&createStruct), NULL );
}


bool SVToolClass::IsAllowedLocation(const SVExtentLocationPropertyEnum Location , SVExtentDirectionsEnum Direction ) const 
{
	SVExtentPropertyInfoStruct info;
	m_svToolExtent.GetExtentPropertyInfo(SVExtentPropertyHeight,info );
	bool bAllowHeight = !(info.bFormula || info.bSetByReset);

	m_svToolExtent.GetExtentPropertyInfo(SVExtentPropertyWidth,info );
	bool bAllowWidth = !(info.bFormula || info.bSetByReset);

	m_svToolExtent.GetExtentPropertyInfo(SVExtentPropertyPositionPointX,info );
	bool bAllowMoveX = !(info.bFormula || info.bSetByReset);

	m_svToolExtent.GetExtentPropertyInfo(SVExtentPropertyPositionPointY,info );
	bool bAllowMoveY = !(info.bFormula || info.bSetByReset);

	bool ret(true);

	switch(Location)
	{

	case  SVExtentLocationPropertyTopLeft:
		if( !bAllowMoveX || !bAllowMoveY  || !bAllowHeight || !bAllowWidth )
		{
			ret = false;
		}
		break;

	case  SVExtentLocationPropertyTopRight:
		if(  !bAllowMoveY  || !bAllowHeight || !bAllowWidth )
		{
			ret = false;
		}
		break;
	case  SVExtentLocationPropertyBottomRight:
		if(  !bAllowHeight || !bAllowWidth )
		{
			ret = false;
		}
		break;

	case  SVExtentLocationPropertyBottomLeft:
		if( !bAllowMoveX ||  !bAllowHeight || !bAllowWidth )
		{
			ret = false;
		}
		break;

	case  SVExtentLocationPropertyLeft:
		if( !bAllowMoveX ||  !bAllowWidth )
		{
			ret = false;
		}
		break;

	case  SVExtentLocationPropertyRight:
		if( !bAllowWidth )
		{
			ret = false;
		}
		break;	

	case  SVExtentLocationPropertyTop:
		if(  !bAllowMoveY  || !bAllowHeight  )
		{
			ret = false;;
		}
		break;

	case  SVExtentLocationPropertyBottom:
		if( !bAllowHeight )
		{
			ret = false;;
		}
		break;
	case	SVExtentLocationPropertyCenter:
		
		if( Direction == SVExtentDirectionBoth &&( !bAllowMoveX || !bAllowMoveY ) )
		{
			ret = false;
		}
		else if (Direction == SVExtentDirectionHorizontal && !bAllowMoveX )
		{
			ret = false;;

		}
		else   if (Direction == SVExtentDirectionVertical && !bAllowMoveY)
		{

			ret = false;;
		}

		break;

	}
	return ret;
}


//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVTool.cpp_v  $
 * 
 *    Rev 1.9   17 Dec 2014 10:48:04   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  908
 * SCR Title:  Remove option for Operator Move (SVO 101)
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Cleared attributes on ToolSelectedForOperatorMove Value Object because it is obsolete.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   15 May 2014 13:10:54   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed processMessage signature to use DWORD_PTR.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   05 Feb 2014 11:49:36   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   ProcessMessage changed DWORD to LONG_PTR.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   01 Feb 2014 12:18:38   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SVSendmessage and processmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   11 Nov 2013 07:23:56   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  872
 * SCR Title:  Add Archive Tool Headers to Archive File
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed tool comment from SVStringvalueobjectclass to SVStaticstringvalueobjectclass.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   02 Oct 2013 08:24:40   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   15 May 2013 19:46:46   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 016.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
      Rev 3.93   14 May 2013 10:44:10   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  796
   SCR Title:  Add a License Manager that will handle the Matrox Licenses
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Modified Validate function to continue validation even if the tool is disabled.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
 *    Rev 1.2   13 May 2013 12:37:10   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 014.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
      Rev 3.92   02 May 2013 11:25:22   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  810
   SCR Title:  Remove support for Legacy Tools
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Updated process message method to fix formatting issues and updated the reset object section to only assert when the HRESULT is a failed status.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
 *    Rev 1.1   07 May 2013 08:21:16   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 011.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
      Rev 3.91   19 Apr 2013 09:09:18   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  827
   SCR Title:  Add button to the adjust tool size/position to set size to parent image.
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Added GetParentExtent.
   Added AllowResizeToParent trait.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
 *    Rev 1.0   23 Apr 2013 15:32:28   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.90   10 Jan 2013 15:15:22   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  805
   SCR Title:  Fix Value Object Indexing Problem when Shortening PPQ
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Merged 6.01 branch code into main development code base.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.89   04 Oct 2012 11:00:18   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  602
   SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Changed from post incrementor to pre incrementor.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.88.1.0   08 Jan 2013 12:30:22   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  805
   SCR Title:  Fix Value Object Indexing Problem when Shortening PPQ
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Added new virtual method to classes to fix adjusting Object Depth of Value Object and Last Set Index.  Also, updated code with new methodology.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.88   01 Aug 2012 13:06:50   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Removed all overlay collection functionality that uses data at a particular index.  The current overlay collection functionality collects the data in the inspection thread, so that all data is consistant based on the last executed inspection.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.87   25 Jul 2012 14:54:54   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Removed dead code.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.86   18 Jul 2012 14:27:38   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Removed obsolete methods assiciated with overlay drawling and fixed problem with copying data to byte buffer have wrong format.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.85   16 Jul 2012 13:03:06   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated overlay collection functionality and removed duplicate fucntionality by consolidating it to the task object class.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.84   12 Jul 2012 15:12:44   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to add in an additional string identifier for the input and fixed clear functionality assoicated with input structure.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.83   05 Jul 2012 14:27:44   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  764
   SCR Title:  Add tool comments to all tools in SVObserver
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Added Tool Comment support to SVTool.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.82   02 Jul 2012 17:48:10   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to promote new display functionality.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.81   19 Jun 2012 14:23:20   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  744
   SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Protected Object Information structure attributes.  This change will add accessor methods to get and set attributes.
   Converted GUID to SVGUID.   This allows the use of standard operators for comparison and methods for conversion.
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.80   12 Oct 2011 18:18:30   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  730
   SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Fixed return code variable when running with new disable.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.79   20 Apr 2011 11:49:20   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  712
   SCR Title:  Fix issues with black images when using command interface (SIAC)
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated code to handle disabled tools correctly.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.78   08 Apr 2011 13:33:04   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  712
   SCR Title:  Fix issues with black images when using command interface (SIAC)
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated inspection functionality to use the update counters flag.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.77   30 Mar 2011 16:28:30   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  712
   SCR Title:  Fix issues with black images when using command interface (SIAC)
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated code to fix performance problems with the extent changes.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.76   21 Mar 2011 12:19:04   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  712
   SCR Title:  Fix issues with black images when using command interface (SIAC)
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated system to fix performance issues with extent checks and updates.  Added additional methods to determine if object has extents and moved extent updated to  associated children.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.75   16 Mar 2011 13:59:10   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  712
   SCR Title:  Fix issues with black images when using command interface (SIAC)
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated extent functionality and renamed extent methods to correct extent issues and add clarity to the logical flow.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.74   11 Mar 2011 14:33:50   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  712
   SCR Title:  Fix issues with black images when using command interface (SIAC)
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to fix extent and connection issues between image classes and tool classes.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.73   01 Mar 2011 10:39:18   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  712
   SCR Title:  Fix issues with black images when using command interface (SIAC)
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source to fix issues with not assigning input image to extents.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.72   25 Feb 2011 12:25:44   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  712
   SCR Title:  Fix issues with black images when using command interface (SIAC)
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated classes needed to consolidate SVImageClass and SVImageObjectClass updating calls to one method name.  Fixed updating problems with child buffers.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.71   27 Jan 2011 12:02:58   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  712
   SCR Title:  Fix issues with black images when using command interface (SIAC)
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to fix issues with Matrox Buffer Management.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.70   08 Dec 2010 13:45:10   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to include changes in notification functionality using the Observer Design Pattern.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.69   04 Nov 2010 14:15:32   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to fix issues with image processing and display image processing classes and associated includes.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.68   01 Jun 2010 15:16:42   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  693
   SCR Title:  Fix Performance Issue with Inspection Process
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to improve ability to track performance.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.67   01 Jun 2010 11:10:06   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  693
   SCR Title:  Fix Performance Issue with Inspection Process
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated code to remove redundent methodologies and fix missing or incorrect calling functionality.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.66   16 Dec 2009 13:20:08   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  677
   SCR Title:  Fix problem in camera notify thread
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Fix issues with includes and comments.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.65   30 Jul 2009 13:09:10   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  665
   SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Merged branched changes into current code branch with appropriate updates.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.64   20 Oct 2008 11:02:46   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  635
   SCR Title:  Blob Analyzer Bug Fixes
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Added IsEnabled method  that uses a dataIndex.
   Added WasEnabled method  that uses a dataIndex.
   Added getConditionalResult method that uses a dataIndex.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.63   12 Sep 2008 12:06:00   bwalter
   Project:  SVObserver
   Change Request (SCR) nbr:  617
   SCR Title:  Use DirectX instead of Matrox to display images
   Checked in by:  bWalter;  Ben Walter
   Change Description:  
     Fixed problem with retVal and bRetVal in Run function
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.62   10 Jun 2008 12:18:34   JSpila
   Project:  SVObserver
   Change Request (SCR) nbr:  621
   SCR Title:  Fix Bug in the SVImageClass
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Upated source code to clear parent image information from the embedded image objects and the tool extent object.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.61   09 Apr 2008 08:49:48   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  624
   SCR Title:  Fix problems with Color Tool when it gets deleted
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Replaced Get Parent operator with Get Parent Image operator and Set Parent operator with Set Parent Image operator when accessing the parent information of the Image Class.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.60   07 Mar 2008 12:57:28   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  616
   SCR Title:  Add Server-side Configuration Data Def Lists to the SVObserver program
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Added functions to support source image names
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.59   21 Jun 2007 15:03:04   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  598
   SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
   Checked in by:  jSpila;  Joseph Spila
   Change Description:  
     These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.58.3.0   14 Jul 2009 15:19:08   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  665
   SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.58.2.0   07 Apr 2008 08:56:56   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  621
   SCR Title:  Fix Bug in the SVImageClass
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Updated methods to use the new GetParentImage method instead of the GetParent method to remove confusion with CWnd GetParent method.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.58   21 Jun 2006 13:51:08   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  566
   SCR Title:  Fix problem with Image Mask
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated code to move find image input from tool class to task object.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.57   25 Jan 2006 14:59:50   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  534
   SCR Title:  Add Image Select for Auxiliary Extent Source
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     set the Auxilliary Extent SV_HIDDEN flag
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.56   25 Jan 2006 10:42:08   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  534
   SCR Title:  Add Image Select for Auxiliary Extent Source
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Fixed issue with Color Tool pointing to itself.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.55   16 Jan 2006 17:31:24   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  534
   SCR Title:  Add Image Select for Auxiliary Extent Source
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Moved removed embedded object to create object method for load of configuration to happen properly.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.54   06 Dec 2005 15:34:56   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  523
   SCR Title:  Fix Bug, Auxiliary Extents show up in Published Results when disabled
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Modified ResetObject and EnableAuxiliaryExtents to add or remove publishable attribute.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.53   02 Nov 2005 07:33:18   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  512
   SCR Title:  Add Published Result Images functionality
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     changed RunStatus.m_lResultImageIndex to .Images
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.52   24 Oct 2005 09:46:16   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  510
   SCR Title:  Add source image extents to all image using tools
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Changes to support adding a flag to the Inspection that will enable Auxiliary Extents.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.51   10 Oct 2005 13:07:58   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  510
   SCR Title:  Add source image extents to all image using tools
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Fixed Spelling error for Auxiliary Extent IDs
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.50   01 Sep 2005 12:37:00   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  510
   SCR Title:  Add source image extents to all image using tools
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Removed UpdateSourceExtents return code from consideration for validity of operator during a Reset Object Call.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.49   01 Sep 2005 12:04:56   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  510
   SCR Title:  Add source image extents to all image using tools
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     New functions to support Auxilliary Extents.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.48   10 Aug 2005 13:28:48   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  464
   SCR Title:  Add array indexing for value objects
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     added UpdateTaskObjectOutputListAttributes
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.47   10 Aug 2005 12:44:40   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  500
   SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated methods to improve performance and reduce system delay.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.46   09 Aug 2005 08:46:12   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  500
   SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated objects and dialogs to use the ActiveX Interface to update inspection data.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.45   03 Aug 2005 13:10:30   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  486
   SCR Title:  Add Trigger Count to Tool Set
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Fixed bug in reset counts logic and cleaned up code.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.44   29 Jul 2005 13:32:38   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  500
   SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated object initialization to change the parameters for RegisterEmbeddedObject for a SVValueObjectClass.  The two additional parameters will inform the preperation process before toolset execution.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.43   26 Jul 2005 14:04:38   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  485
   SCR Title:  Add a way to Reset Counts remotely
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Added Reset Flag struct to replace flag in IPDoc.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.42   26 Jul 2005 13:38:02   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  450
   SCR Title:  Add asynchronous functionality to the archive tool
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     updated to use SVIPDoc SVResetStruct
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.41   22 Jun 2005 10:34:44   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  464
   SCR Title:  Add array indexing for value objects
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     object attributes use accessor functions
   value objects use SVHRESULT
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.40   17 Jun 2005 13:36:40   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  487
   SCR Title:  Add new blob feature for source extents of blob box center
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Call to GetTitlePoint to place the tool name
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.39   09 Mar 2005 12:27:22   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  477
   SCR Title:  Add Cylindrical warp Tool
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     cleaned up formatting
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.38   22 Feb 2005 10:50:52   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  456
   SCR Title:  Update Image and Tool Objects to use the new Extent Classes
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Fixed load image tool problems.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.37   18 Feb 2005 11:38:24   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  262
   SCR Title:  Improve performance when loading a configuration in SVObserver
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Changed method of tracking values objects from full object manager rebuild to a value object registration methodology.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.36   18 Feb 2005 07:05:52   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  456
   SCR Title:  Update Image and Tool Objects to use the new Extent Classes
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Modifications to support new extents.  Also changes made to support new creat / reset object methodolory
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.35   16 Feb 2005 15:21:24   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  440
   SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     updated header files
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.34   12 Feb 2004 16:52:08   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  374
   SCR Title:  Add check before changing tool parameters when processing input list
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Removed unused function that were originally designed for handling SetToolParameterList functionality.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.33   16 Dec 2003 09:41:28   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  373
   SCR Title:  Add a new disable method that doesn't copy forward all result values
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Fixed bug in new disable method for non disabled tools.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.32   20 Nov 2003 11:52:08   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  385
   SCR Title:  Expose additional parameters for SIAC
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     changed attributes for the value objects to be setable online
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.31   07 Aug 2003 12:03:10   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  380
   SCR Title:  Add Image Mask option
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     modified the OnDraw method for the new mask
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.30   08 Jul 2003 10:57:48   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  373
   SCR Title:  Add a new disable method that doesn't copy forward all result values
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Fixed small problem with counts (pass, fail, etc) for the new disable method.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.29   08 Jul 2003 10:42:56   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  374
   SCR Title:  Add check before changing tool parameters when processing input list
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Added new code the processing of InputRequests (from SetToolParameterList) to verify that extents have changed or the change is ignored.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.28   17 Jun 2003 15:12:58   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  373
   SCR Title:  Add a new disable method that doesn't copy forward all result values
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Updated the new disable method code. Also, made Tool.Enabled accessible through the ActiveX.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.27   02 Jun 2003 15:06:54   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  352
   SCR Title:  Update SVObserver to Version 4.30 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Checked in some preliminary beta code for the new methode of disabling tools and toolsets.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.26   22 May 2003 16:23:36   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  342
   SCR Title:  Configuration report printout changes after going online.
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Fixed some bugs in the configuration print.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.25   07 May 2003 13:12:00   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  271
   SCR Title:  Adding timer features for Tool Execution times.
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     changed the Stop to use the RRunStatus.m_lResultDataIndex 
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.24   22 Apr 2003 16:37:20   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.23   10 Apr 2003 10:13:40   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  271
   SCR Title:  Adding timer features for Tool Execution times.
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     added ToolTime member varaible as a SVTimerValueObject, which will keep track to tool times
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.22   21 Mar 2003 12:33:06   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  330
   SCR Title:  Add SetImageList to ActiveX Interface
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     added member variable m_bFileSetup
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.21   05 Mar 2003 12:13:16   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  307
   SCR Title:  Implement the ability to remotely change Tool parameters on the SVIM
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Restructured and recoded the SetToolParameter code. It now uses a standard struct and each SVToolClass based class is responsible for moving itself based on the values passed in through the struct.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.20   31 Jan 2003 08:54:34   joe
   Project:  SVObserver
   Change Request (SCR) nbr:  301
   SCR Title:  Implement Result Image Circle Buffer
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated CreateObject and processMessage methods to use the new image circle buffer methodology.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.19   10 Jan 2003 16:13:24   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  305
   SCR Title:  Implement the ability to perform RunOnce from a SIAC client
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Added new function VerifyImageForOverlay to determine which object has overlays on which image.
   Added method to return the overlay figure from this object.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.18   03 Jan 2003 15:35:14   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  307
   SCR Title:  Implement the ability to remotely change Tool parameters on the SVIM
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     added methods for  - Set (Left, Top, Width, Height)Extents
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.17   09 Dec 2002 20:14:28   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  295
   SCR Title:  Remove Result Data from Configuration Printout
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     CreateObject method was modified to properly update the isCreated flag and to set the printability of the internal value objects.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.16   20 Nov 2002 13:54:42   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  226
   SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Counts Copy Forward
   Value Object Indexes
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.15   02 Oct 2001 15:07:04   ed
   Project:  SVObserver
   Change Request (SCR) nbr:  241
   SCR Title:  Color SVIM:  Load image tool output not available in window tool as input.
   Checked in by:  Ed;  Ed Chobanoff
   Change Description:  
     Modified functions, SVToolClass::onDraw.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.14   14 Aug 2001 10:41:46   ed
   Project:  SVObserver
   Change Request (SCR) nbr:  220
   SCR Title:  Fix invalid path message when adding a Load Image tool
   Checked in by:  Ed;  Ed Chobanoff
   Change Description:  
     Added ValidateDrive function to class.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.13   10 May 2001 15:57:26   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  197
   SCR Title:  Fix lock-ups in application when loading configuration from SVFocusNT
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Changed SVObserver state code in:
   ~SVTool
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.12   26 Apr 2000 14:59:58   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  129
   SCR Title:  Angular Profile Tool
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Added GetTransform method override.
   Revised onDraw Method to call GetTransform.
   Added message handler for 
   SVMSGID_SET_SHADOWED_ABSEXTENTS.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.11   23 Mar 2000 17:36:54   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  118
   SCR Title:  Integration of LUT Operator
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Introduced SetDefaultFormulas(). Default implementation
   return TRUE.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.10   23 Mar 2000 14:03:44   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  117
   SCR Title:  Bug Fixes for Integration of conditional Toolset/Tool Drawing.
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised to call SetDisabled(), if disabled.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.9   03 Mar 2000 13:13:24   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  112
   SCR Title:  Expose Extents of Images
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised to use SVImageInfoClass methods to get image extents.
   
   Added SVDoubleValueObjects for exposed extents.
   Added handler for SVMSGID_SET_SHADOWED_EXTENT.
   Added method setShadowedExtents.
   Removed SetExtent, SetAbsoluteExtent, GetExtent, and
   GetAbsoluteExtent methods.
   Added removeEmbeddedExtents method to provide a way for tools wishing to not expose extents to do so.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.8   Mar 01 2000 13:02:36   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  113
   SCR Title:  Integrate new Polar Unwrap Tool
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
      Revised to make checkDrawAllowed(...) public.
    Revised onDraw(...) to don't route, if not necessary.
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.7   25 Feb 2000 16:13:44   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  105
   SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised onDraw to call pImage->GetTransform() to get contextual transformation.
   Revised onDraw to call PDrawContext->Transform instead of
   SVCalculateRotation.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.6   Feb 24 2000 12:15:18   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  111
   SCR Title:  Integrate Conditional Tool Set/Tool Drawing
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Introduced new conditional tool drawing, using enumerated draw flag.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.5   21 Feb 2000 15:02:54   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  105
   SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Removed MoveObjectsBase method.
   Revised onDraw to use SVDrawContext* and relative extents.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.4   16 Feb 2000 17:05:16   mike
   Project:  SVObserver
   Change Request (SCR) nbr:  105
   SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Modified Draw functions to use the DrawContext structure
   and removed unused methods.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.3   08 Feb 2000 10:59:32   mike
   Project:  SVObserver
   Change Request (SCR) nbr:  96
   SCR Title:  Scroll/Zoom images displayed in the inspection document.
   Checked in by:  Mike;  Mike McCarl
   Change Description:  
     Modified the onDraw function to use the current scaling
   factor (lZoomFactor) when drawing an object.
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.2   Feb 01 2000 09:41:16   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  90
   SCR Title:  Save  Operator Tool Move Priviledge in Configuration
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Added code to accomodate the operator tool move feature.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.1   Nov 30 1999 14:39:02   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  65
   SCR Title:  Remove obsolute class SVToolSetViewClass
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Removed reference to SVToolSetViewClass.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.0   23 Nov 1999 13:46:04   mike
   Project:  SVObserver
   Change Request (SCR) nbr:  61
   SCR Title:  Update PVCS versioning to version 3.0.
   Checked in by:  Mike;  Mike McCarl
   Change Description:  
     
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.17   09 Nov 1999 17:47:24   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  34
   SCR Title:  Add Build Reference Tool
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Added IsOkToEdit method (primarily for Build Ref Tool)
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.16   Oct 13 1999 15:14:30   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  21
   SCR Title:  Port Gage Tool code from program version 2.42.
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Roberts bug fixes for Beta 10.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.15   Sep 24 1999 09:09:12   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  31
   SCR Title:  Integrate file manager/tracker.
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Completed File Tracker.
   Added Passed-Failed-Warned-Enabled-Processed-Counter
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.14   Sep 23 1999 09:06:18   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  35
   SCR Title:  PPQ mode to wait for input data
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Added missing explicitFailed ( used for Reset Timer ).
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.13   Sep 20 1999 09:57:28   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  35
   SCR Title:  PPQ mode to wait for input data
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Bugfux: Default Value of Failed or Warned is always TRUE.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.12   13 Sep 1999 19:32:16   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  16
   SCR Title:  Integrate Math Tool
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised to update color in onRun method.
   ( to pickup disabled and disabled by conditional ).
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.11   13 Sep 1999 18:03:44   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  16
   SCR Title:  Integrate Math Tool
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised for Conditional execution
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.10   11 Sep 1999 19:39:54   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  16
   SCR Title:  Integrate Math Tool
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Added Enable Method for conditional dialog
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.9   Sep 10 1999 18:29:12   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  33
   SCR Title:  Port Profile Tool from v2.42
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Restore Run function and other code removed in error.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.8   Sep 10 1999 18:03:48   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  33
   SCR Title:  Port Profile Tool from v2.42
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Removed Obsolete Functions.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.6   02 Sep 1999 08:55:06   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Removed Tool Color (color now lives in SVObjectClass).
   Revised Run logic.
   Revised onDraw for color.
   Removed override of SetObjectDepth.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.5   30 Aug 1999 20:07:34   robert
   Project:  SVObserver
   Change Request (SCR) nbr:  21
   SCR Title:  Port Gage Tool code from program version 2.42.
   Checked in by:  Robert;  Robert Massinger
   Change Description:  
     Introduced Input Interface Handling.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.4   30 Aug 1999 19:05:08   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised Run method to use SVRunStatusClass.
   Revised to use SVConditionalClass.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.3   Aug 25 1999 22:50:24   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  1
   SCR Title:  Integrate a common validity check for all tool base class inputs and outputs.
   Checked in by:  Steve Jones
   Change Description:  
     Revised Validate method to be OnValidate method.
    ( OnValidate - local scope validation).
   Added Validate method (Routes to all owned objects)
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.2   24 Aug 1999 20:36:42   robert
   Project:  SVObserver
   Change Request (SCR) nbr:  21
   SCR Title:  Port Gage Tool code from program version 2.42.
   Checked in by:  Robert Massinger
   Change Description:  
     Deleted old Gage Tool, Profile Tool, Reference Tool, Rotate Tool.
   Made changings to support new onUpdate() and onDraw() right.
   
   
   /////////////////////////////////////////////////////////////////////////////////////
*/
