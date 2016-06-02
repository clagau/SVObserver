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

#pragma region Includes
#include "stdafx.h"
#include "SVTool.h"
#include "ObjectInterfaces/IObjectManager.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVObjectLibrary/SVAnalyzerLevelCreateStruct.h"
#include "SVAnalyzer.h"
#include "SVConditional.h"
#include "SVGlobal.h"
#include "SVInspectionProcess.h"
#include "SVToolImage.h"
#include "SVToolSet.h"
#include "ToolSizeAdjustTask.h"
#pragma endregion Includes

SV_IMPLEMENT_CLASS( SVToolClass, SVToolClassGuid );

SVToolClass::SVToolClass( BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID /*= IDS_CLASSNAME_SVTOOL*/ )
: SVTaskObjectListClass( BCreateDefaultTaskList, POwner, StringResourceID )
, m_pToolConditional(nullptr)
{
	init();
}

void SVToolClass::init()
{
	// Indentify our type in the Output List
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVToolObjectType;

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

	RegisterEmbeddedObject( &m_svUpdateAuxiliaryExtents, SVUpdateAuxiliaryExtentsObjectGuid, IDS_OBJECTNAME_UPDATE_AUXILIARY_EXTENTS_OBJECT, false, SVResetItemTool );
	RegisterEmbeddedObject( &m_svAuxiliarySourceX, SVAuxiliarySourceXObjectGuid, IDS_OBJECTNAME_AUXILIARY_SOURCE_X, false, SVResetItemNone, _T("Extent X") );
	RegisterEmbeddedObject( &m_svAuxiliarySourceY, SVAuxiliarySourceYObjectGuid, IDS_OBJECTNAME_AUXILIARY_SOURCE_Y, false, SVResetItemNone, _T("Extent Y") );
	RegisterEmbeddedObject( &m_svAuxiliarySourceAngle, SVAuxiliarySourceAngleObjectGuid, IDS_OBJECTNAME_AUXILIARY_SOURCE_ANGLE, false, SVResetItemNone, _T("Extent Angle") );
	RegisterEmbeddedObject( &m_svAuxiliarySourceImageName, SVAuxiliarySourceImageNameObjectGuid, IDS_OBJECTNAME_AUXILIARY_SOURCE_IMAGE_NAME, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_svAuxiliaryDrawType, SVAuxiliaryDrawTypeObjectGuid, IDS_OBJECTNAME_AUXILIARY_DRAW_TYPE_NAME, false, SVResetItemNone );
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

	drawToolFlag.SetEnumTypes( IDS_TOOLDRAW_ENUMOBJECT_LIST );
	drawToolFlag.SetDefaultValue( ( const long ) 0, TRUE ); // 0 Should be show tool 'Always'

	m_svUpdateAuxiliaryExtents.SetDefaultValue( FALSE, TRUE );

	m_svAuxiliarySourceX.SetDefaultValue( 0.0, TRUE );
	m_svAuxiliarySourceY.SetDefaultValue( 0.0, TRUE );
	m_svAuxiliarySourceAngle.SetDefaultValue( 0.0, TRUE );
	m_svAuxiliarySourceImageName.SetDefaultValue( "", TRUE );
	m_svAuxiliaryDrawType.SetDefaultValue( "", TRUE );

	pCurrentToolSet = nullptr;
	pPropertyArray = nullptr;
	propertyCount = 0;

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

	for( size_t j = 0;nullptr ==  m_pToolConditional && j < m_friendList.size(); j++ )
	{
		m_pToolConditional = dynamic_cast<SVConditionalClass *>(m_friendList[j].PObject);
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
	
	// Defaults for the Scale Factors should be hidden (but NOT removed at this time, so 
	// don't use hideEmbeddedObject() here).
	extentWidthScaleFactor.ObjectAttributesAllowedRef() &=(~SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES);
	extentHeightScaleFactor.ObjectAttributesAllowedRef() &=(~SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES);

	// Auxiliary Tool Source Extent
	m_svUpdateAuxiliaryExtents.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_SETABLE_ONLINE | SV_REMOTELY_SETABLE;
	
	drawToolFlag.ObjectAttributesAllowedRef() |= SV_PRINTABLE;

	// Tool Comment attributes...
	m_svToolComment.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	m_svToolComment.ObjectAttributesAllowedRef() &= ~SV_VIEWABLE;	// We do not want this to show up in the results picker.

	m_isCreated = bOk;

	return bOk;
}

BOOL SVToolClass::CloseObject()
{
	m_svToolExtent.SetToolImage( nullptr );
	m_svToolExtent.SetSelectedImage( nullptr );

	return SVTaskObjectListClass::CloseObject();
}

BOOL SVToolClass::DisconnectInput( SVInObjectInfoStruct* pInObjectInfo )
{
	BOOL l_Status( SVTaskObjectListClass::DisconnectInput( pInObjectInfo ) && nullptr != pInObjectInfo );

	if( nullptr != pInObjectInfo )
	{
		if( pInObjectInfo->GetInputObjectInfo().PObject == m_svToolExtent.GetToolImage() )
		{
			m_svToolExtent.SetToolImage( nullptr );
		}

		if( pInObjectInfo->GetInputObjectInfo().PObject == m_svToolExtent.GetSelectedImage() )
		{
			m_svToolExtent.SetSelectedImage( nullptr );
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

	if( nullptr != pCurrentToolSet )
	{
		bEnabled = pCurrentToolSet->WasEnabled();
	}

	if (bEnabled)
	{
		bEnabled = IsEnabled();

		if (bEnabled)
		{
			bEnabled = ( false != getConditionalResult() );
		}
	}

	return bEnabled;
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

	p_rMultiLine.m_Passed = ( FALSE != bPassed );
	p_rMultiLine.m_Failed = ( FALSE != bFailed );
	p_rMultiLine.m_Warned = ( FALSE != bWarned );

	return l_Status;
}

BOOL SVToolClass::OnValidate()
{
	BOOL bRetVal = FALSE;
	
	if( inputConditionBoolObjectInfo.IsConnected() &&
		inputConditionBoolObjectInfo.GetInputObjectInfo().PObject )
	{
		bRetVal = SVTaskObjectListClass::OnValidate();
	}
	else
	{
		SvStl::MessageContainer message;
		SVStringArray msgList;
		msgList.push_back(GetName());
		message.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_ConditionalValue_Invalid, msgList, SvStl::SourceFileParams(StdMessageParams) );
		addTaskMessage( message );
	}

	if( !bRetVal )
	{
		SetInvalid();
	}
	return bRetVal;
}

void SVToolClass::UpdateAuxiliaryExtents(long resultDataIndex)
{
	if( GetInspection()->GetEnableAuxiliaryExtent() )
	{
		BOOL l_bUpdateSourceExtents = false;

		bool l_bForceOffsetReset = GetInspection()->m_bForceOffsetUpdate;

		m_svToolExtent.UpdateOffsetData( l_bForceOffsetReset );

		HRESULT hr = m_svUpdateAuxiliaryExtents.GetValue( l_bUpdateSourceExtents );
		if( S_OK == hr && l_bUpdateSourceExtents )
		{
			SVExtentOffsetStruct l_svOffsetData;

			hr = m_svToolExtent.GetSelectedOffsetData( l_svOffsetData );
			if( S_OK != hr || ! l_svOffsetData.m_bIsLinear )
			{
				hr = m_svToolExtent.UpdateOffsetDataToImage( l_svOffsetData, GetAuxSourceImage() );
			}

			SVImageClass* pAuxSourceImage = GetAuxSourceImage();
			if( nullptr != pAuxSourceImage )
			{
				m_svAuxiliarySourceImageName.SetValue( resultDataIndex, pAuxSourceImage->GetCompleteObjectName() );
			}

			CString l_strDrawType;
			m_svToolExtent.GetAuxiliaryDrawTypeString( l_strDrawType );
			m_svAuxiliaryDrawType.SetValue( resultDataIndex, l_strDrawType );
			
			SVExtentPointStruct pt;
			SVImageClass* pImage = m_svToolExtent.GetToolImage();
			if (pImage && pAuxSourceImage)
			{
				pImage->TranslateFromOutputSpaceToImage(pAuxSourceImage, pt, pt);
			}
			m_svAuxiliarySourceX.SetValue( resultDataIndex, pt.m_dPositionX );
			m_svAuxiliarySourceY.SetValue( resultDataIndex, pt.m_dPositionY );

			m_svAuxiliarySourceAngle.SetValue( resultDataIndex, l_svOffsetData.m_dRotationAngle );
		}
	}
}

BOOL SVToolClass::Run( SVRunStatusClass& RRunStatus )
{
	DWORD retVal = 0;
	long lCount;
	BOOL bIsValid = FALSE;

	clearTaskMessages();

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
		UpdateAuxiliaryExtents(RRunStatus.m_lResultDataIndex);
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
		{
			m_pToolConditional->onRun( RRunStatus );
		}

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
	HRESULT hr(S_OK);

	hr = getFirstTaskMessage().getMessage().m_MessageCode;
	if( !bRetVal && SUCCEEDED( hr ) )
	{
		hr = SVMSG_SVO_5075_INCONSISTENTDATA;
	}

	if( SUCCEEDED( hr ) )
	{
		if( SV_NO_ATTRIBUTES != ( extentTop.ObjectAttributesAllowed() & SV_NO_ATTRIBUTES )  )
		{
			bRetVal = ( S_OK == extentTop.CopyLastSetValue( RRunStatus.m_lResultDataIndex ) ) && bRetVal;
		}
		if( SV_NO_ATTRIBUTES != ( extentLeft.ObjectAttributesAllowed() & SV_NO_ATTRIBUTES ) )
		{
			bRetVal = ( S_OK == extentLeft.CopyLastSetValue( RRunStatus.m_lResultDataIndex ) ) && bRetVal;
		}
		if( SV_NO_ATTRIBUTES != ( extentWidth.ObjectAttributesAllowed() & SV_NO_ATTRIBUTES ) )
		{
			bRetVal = ( S_OK == extentWidth.CopyLastSetValue( RRunStatus.m_lResultDataIndex ) ) && bRetVal;
		}
		if( SV_NO_ATTRIBUTES != ( extentHeight.ObjectAttributesAllowed() & SV_NO_ATTRIBUTES ) )
		{
			bRetVal = ( S_OK == extentHeight.CopyLastSetValue( RRunStatus.m_lResultDataIndex ) ) && bRetVal;
		}
		if( SV_NO_ATTRIBUTES != ( extentWidthScaleFactor.ObjectAttributesAllowed() & SV_NO_ATTRIBUTES ) )
		{
			bRetVal = ( S_OK == extentWidthScaleFactor.CopyLastSetValue( RRunStatus.m_lResultDataIndex ) ) && bRetVal;
		}
		if( SV_NO_ATTRIBUTES != ( extentHeightScaleFactor.ObjectAttributesAllowed() & SV_NO_ATTRIBUTES ) )
		{
			bRetVal = ( S_OK == extentHeightScaleFactor.CopyLastSetValue( RRunStatus.m_lResultDataIndex ) ) && bRetVal;
		}

		if( !bRetVal )
		{
			hr = SVMSG_SVO_5076_EXTENTSNOTCOPIED;
		}

		// Friends were running, validation was successfully
		// Check conditional execution
		if ( !getConditionalResult() )
		{
			RRunStatus.SetDisabledByCondition();
		}
	}

	if( !SUCCEEDED ( hr ) )
	{
		SvStl::MessageContainer message;
		message.setMessage( hr, SvOi::Tid_Empty, SvStl::SourceFileParams(StdMessageParams) );
		addTaskMessage( message );
		bRetVal = false;
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
	DWORD_PTR DwResult = SVMR_NOT_PROCESSED;
	SVToolLevelCreateStruct createStruct;

	// Try to process message by yourself...
	DWORD dwPureMessageID = DwMessageID & SVM_PURE_MESSAGE;
	switch( dwPureMessageID )
	{
	case SVMSGID_RESET_ALL_OBJECTS:
		{
			ToolSizeAdjustTask* pToolSizeAdjustTask = nullptr;
			pToolSizeAdjustTask = ToolSizeAdjustTask::GetToolSizeAdjustTask( this );
			if( nullptr != pToolSizeAdjustTask )
			{
				DwResult = pToolSizeAdjustTask->ProcessResetAllObject(DwMessageID,DwMessageValue, DwMessageContext);
			}
			HRESULT l_ResetStatus = ResetObject();
			if( S_OK != l_ResetStatus )
			{
				DwResult |= SVMR_NO_SUCCESS;
			}
			else
			{
				DwResult |= SVMR_SUCCESS;
			}
			break;
		}

	case SVMSGID_CREATE_ALL_OBJECTS:
		{
			if( !IsCreated() && !CreateObject( reinterpret_cast<SVObjectLevelCreateStruct*>(DwMessageValue) ) )
			{
				ASSERT( FALSE );

				SVStringArray msgList;
				msgList.push_back(GetObjectName());
				msgList.push_back(SVString(GetCompleteObjectName()));
				SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
				Msg.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_CreationOf2Failed, msgList, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10209 );

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
			if( S_OK != ConnectObject( reinterpret_cast<SVObjectLevelCreateStruct*>(DwMessageValue) ) )
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
			createStruct.InspectionObjectInfo = GetInspection();

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

			return SVSendMessage( pChildObject, SVM_CONNECT_ALL_OBJECTS, reinterpret_cast<DWORD_PTR>(&createStruct), 0 );
		}
	}

	DWORD_PTR Status = ( SVTaskObjectListClass::processMessage( DwMessageID, DwMessageValue, DwMessageContext ) | DwResult );
	return Status;
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
	if( nullptr != GetInspection() )
	{
		GetInspection()->m_bForceOffsetUpdate = true;
	}

	return m_svToolExtent.UpdateOffsetData( true, p_svToolImage );
}

HRESULT SVToolClass::TranslatePointToSource( SVExtentPointStruct p_svIn, SVExtentPointStruct& p_rsvOut )
{
	return m_svToolExtent.TranslatePointToSource( p_svIn, p_rsvOut );
}

HRESULT SVToolClass::EnableAuxiliaryExtents( bool p_bEnable )
{
	HRESULT l_hr = S_OK;

	DWORD l_dwAttributes = SV_VIEWABLE
							| SV_ARCHIVABLE
							| SV_SELECTABLE_FOR_EQUATION
							| SV_SELECTABLE_FOR_STATISTICS
							| SV_PUBLISHABLE;

	if( p_bEnable )
	{
		m_svAuxiliarySourceX.ObjectAttributesAllowedRef() |= l_dwAttributes;
		m_svAuxiliarySourceY.ObjectAttributesAllowedRef() |= l_dwAttributes;
		m_svAuxiliarySourceAngle.ObjectAttributesAllowedRef() |= l_dwAttributes;
		m_svAuxiliaryDrawType.ObjectAttributesAllowedRef() |= l_dwAttributes;
		m_svAuxiliarySourceImageName.ObjectAttributesAllowedRef() |= l_dwAttributes;

		SetBits( m_svAuxiliarySourceX.ObjectAttributesAllowedRef(), SV_HIDDEN, false );
		SetBits( m_svAuxiliarySourceY.ObjectAttributesAllowedRef(), SV_HIDDEN, false );
		SetBits( m_svAuxiliarySourceAngle.ObjectAttributesAllowedRef(), SV_HIDDEN, false );
		SetBits( m_svAuxiliaryDrawType.ObjectAttributesAllowedRef(), SV_HIDDEN, false );
		SetBits( m_svAuxiliarySourceImageName.ObjectAttributesAllowedRef(), SV_HIDDEN, false );
	}
	else
	{
		m_svAuxiliarySourceX.ObjectAttributesAllowedRef() &= ~l_dwAttributes;
		m_svAuxiliarySourceY.ObjectAttributesAllowedRef() &= ~l_dwAttributes;
		m_svAuxiliarySourceAngle.ObjectAttributesAllowedRef() &= ~l_dwAttributes;
		m_svAuxiliaryDrawType.ObjectAttributesAllowedRef() &= ~l_dwAttributes;
		m_svAuxiliarySourceImageName.ObjectAttributesAllowedRef() &= ~l_dwAttributes;

		m_svAuxiliarySourceX.ObjectAttributesSetRef() &= (UINT)~l_dwAttributes;
		m_svAuxiliarySourceY.ObjectAttributesSetRef() &= (UINT)~l_dwAttributes;
		m_svAuxiliarySourceAngle.ObjectAttributesSetRef() &= (UINT)~l_dwAttributes;
		m_svAuxiliaryDrawType.ObjectAttributesSetRef() &= (UINT)~l_dwAttributes;
		m_svAuxiliarySourceImageName.ObjectAttributesSetRef() &= (UINT)~l_dwAttributes;

		SetBits( m_svAuxiliarySourceX.ObjectAttributesAllowedRef(), SV_HIDDEN, true );
		SetBits( m_svAuxiliarySourceY.ObjectAttributesAllowedRef(), SV_HIDDEN, true );
		SetBits( m_svAuxiliarySourceAngle.ObjectAttributesAllowedRef(), SV_HIDDEN, true );
		SetBits( m_svAuxiliaryDrawType.ObjectAttributesAllowedRef(), SV_HIDDEN, true );
		SetBits( m_svAuxiliarySourceImageName.ObjectAttributesAllowedRef(), SV_HIDDEN, true );
	}
	return l_hr;
}

HRESULT SVToolClass::ResetObject()
{
	HRESULT l_hrOk = S_OK;
		
	if( S_OK != SVTaskObjectListClass::ResetObject() )
	{
		l_hrOk = S_FALSE;
	}

	bool l_bReset = false;
	if ( nullptr != GetInspection() && 
		 nullptr != GetInspection()->GetToolSet() && 
		 ( S_OK == GetInspection()->GetToolSet()->m_bvoResetCounts.GetValue( l_bReset ) ) &&
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
	if ( S_OK == l_hrOk )
	{
		UpdateBottomAndRight();
	}

	// Auxiliary Extents
	double l_dValue = 0;
	
	if( GetInspection()->GetEnableAuxiliaryExtent() )
	{
		m_svUpdateAuxiliaryExtents.ObjectAttributesAllowedRef() |=  SV_VIEWABLE 
			| SV_ARCHIVABLE
			| SV_SELECTABLE_FOR_EQUATION
			| SV_SELECTABLE_FOR_STATISTICS 
			| SV_PUBLISHABLE;
		m_svUpdateAuxiliaryExtents.GetValue( l_dValue );

		SetBits( m_svUpdateAuxiliaryExtents.ObjectAttributesAllowedRef(), SV_HIDDEN, false );
	}
	else
	{
		m_svUpdateAuxiliaryExtents.ObjectAttributesAllowedRef() &= ~( SV_VIEWABLE 
			| SV_ARCHIVABLE
			| SV_SELECTABLE_FOR_EQUATION
			| SV_SELECTABLE_FOR_STATISTICS 
			| SV_PUBLISHABLE);

		m_svUpdateAuxiliaryExtents.ObjectAttributesSetRef() &= ~( SV_VIEWABLE 
			| SV_ARCHIVABLE
			| SV_SELECTABLE_FOR_EQUATION
			| SV_SELECTABLE_FOR_STATISTICS 
			| SV_PUBLISHABLE);

		SetBits( m_svUpdateAuxiliaryExtents.ObjectAttributesAllowedRef(), SV_HIDDEN, true );
	}

	EnableAuxiliaryExtents( l_dValue > 0 );

	return l_hrOk;
}

void SVToolClass::UpdateBottomAndRight()
{
	SVImageExtentClass l_svExtents;

	if ( S_OK == GetImageExtent( l_svExtents ) )
	{
		SVExtentFigureStruct l_svFigure;

		if ( S_OK == l_svExtents.GetFigure( l_svFigure ) )
		{
			if( SV_NO_ATTRIBUTES != extentRight.ObjectAttributesAllowed() )
			{
				long l_lValue = static_cast<long>(l_svFigure.m_svBottomRight.m_dPositionX);

				extentRight.SetValue( 1, l_lValue );
			}

			if( SV_NO_ATTRIBUTES != extentBottom.ObjectAttributesAllowed() )
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
		m_svToolExtent.SetExtentObject( SVExtentPropertyPositionPointX, nullptr );
		m_svToolExtent.SetExtentObject( SVExtentPropertyPositionPointY, nullptr );
		m_svToolExtent.SetExtentObject( SVExtentPropertyWidth, nullptr );
		m_svToolExtent.SetExtentObject( SVExtentPropertyHeight, nullptr );
		m_svToolExtent.SetExtentObject( SVExtentPropertyWidthScaleFactor, nullptr );
		m_svToolExtent.SetExtentObject( SVExtentPropertyHeightScaleFactor, nullptr );
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

	hideEmbeddedObject( extentWidthScaleFactor );
	RemoveEmbeddedObject( &extentWidthScaleFactor );

	hideEmbeddedObject( extentHeightScaleFactor );
	RemoveEmbeddedObject( &extentHeightScaleFactor );
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

	if( S_OK == l_Status )
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

	if( S_OK == IsInputImage( p_Image ) )
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

// Source Image Functions
HRESULT SVToolClass::GetSourceImages( SVImageListClass* p_psvImageList ) const
{
	HRESULT l_hr = S_OK;

	SVImageClass* l_psvImageParent = nullptr;
	const SVToolClass* l_psvTool = this;
	if( m_svToolExtent.GetToolImage() )
	{
		l_psvImageParent = m_svToolExtent.GetToolImage()->GetParentImage();
		if( nullptr != l_psvImageParent )
		{
			 l_psvTool = l_psvImageParent->GetTool(); //dynamic_cast<SVToolClass*>( l_psvImageParent->GetTool() );

			 if( nullptr != l_psvTool && l_psvTool != this )
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

SVImageClass* SVToolClass::GetAuxSourceImage() const
{
	SVImageClass* l_pImage = nullptr;
	if( m_AuxSourceImageObjectInfo.IsConnected() && m_AuxSourceImageObjectInfo.GetInputObjectInfo().PObject )
	{
		l_pImage = static_cast<SVImageClass*>(m_AuxSourceImageObjectInfo.GetInputObjectInfo().PObject);
	}
	return l_pImage;
}

HRESULT SVToolClass::SetAuxSourceImage( SVImageClass* p_psvImage )
{
	HRESULT l_hr = S_FALSE;

	SVImageListClass l_svImageList;

	if( S_OK == GetSourceImages( &l_svImageList ) )
	{
		SVImageClass* l_psvImage = nullptr;

		long l_lCount = l_svImageList.GetSize();

		if( 0 < l_lCount )
		{
			l_psvImage = l_svImageList.GetAt( 0 );

			for( int i = l_lCount - 1; S_OK != l_hr && i > 0; i-- )
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

		if ( nullptr != GetInspection() )
		{
			GetInspection()->m_bForceOffsetUpdate = true;
		}
		UpdateAuxiliaryExtents(1);
		::SVSendMessage( GetInspection(), SVM_RESET_ALL_OBJECTS, 0, 0 );
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

#pragma region ITool methods
bool SVToolClass::areAuxExtentsAvailable() const
{
	bool bRetVal = true;
	// check inspection, not gauge tool, and has image input!
	if (nullptr == GetToolImage() || 
		0 == GetInspection()->GetEnableAuxiliaryExtent())
	{
		bRetVal = false;
	}
	return bRetVal;
}

SvUl::NameGuidList SVToolClass::getAvailableAuxSourceImages() const
{
	SvUl::NameGuidList list;
	SVImageListClass ImageList;
	HRESULT hr = GetSourceImages(&ImageList);
	if (S_OK == hr)
	{
		for (SVImageListClass::const_iterator it = ImageList.begin();it != ImageList.end();++it)
		{
			list.push_back(std::make_pair((*it)->getDisplayedName(), (*it)->GetUniqueObjectID()));
		}
	}
	return list;
}

SvUl::NameGuidPair SVToolClass::getAuxSourceImage() const
{
	SvUl::NameGuidPair result;
	SVImageClass* pImage = GetAuxSourceImage();
	if (pImage)
	{
		result = std::make_pair(pImage->getDisplayedName(), pImage->GetUniqueObjectID());
	}
	return result;
}

HRESULT SVToolClass::setAuxSourceImage(const SVGUID& rObjectID)
{
	HRESULT hr = E_POINTER;
	SVImageClass* pImage = dynamic_cast<SVImageClass *>(SvOi::getObject(rObjectID));
	if (pImage)
	{
		hr = SetAuxSourceImage(pImage);
	}
	return hr;
}
#pragma endregion ITool methods

SVStaticStringValueObjectClass* SVToolClass::GetInputImageNames( )
{
	return nullptr;
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

	return SVSendMessage( pChildObject, SVM_CREATE_ALL_OBJECTS, reinterpret_cast<DWORD_PTR>(&createStruct), 0 );
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
		if( !bAllowMoveY  || !bAllowHeight )
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


