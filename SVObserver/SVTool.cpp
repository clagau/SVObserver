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
#include "SVUtilityLibrary/SetBits.h"
#include "SVAnalyzer.h"
#include "SVConditional.h"
#include "ObjectInterfaces/GlobalConst.h"
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
	RegisterEmbeddedObject( &m_Passed, SVPassedObjectGuid, IDS_OBJECTNAME_PASSED, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_Failed, SVFailedObjectGuid, IDS_OBJECTNAME_FAILED, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_Warned, SVWarnedObjectGuid, IDS_OBJECTNAME_WARNED, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_ExplicitFailed, SVExplicitFailedObjectGuid, IDS_OBJECTNAME_EXPLICIT_FAILED, false, SVResetItemNone );

	RegisterEmbeddedObject( &m_PassedCount, SVPassedCountObjectGuid, IDS_OBJECTNAME_PASSED_COUNT, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_FailedCount, SVFailedCountObjectGuid, IDS_OBJECTNAME_FAILED_COUNT, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_WarnedCount, SVWarnedCountObjectGuid, IDS_OBJECTNAME_WARNED_COUNT, false, SVResetItemNone );

	RegisterEmbeddedObject( &m_EnabledCount, SVEnabledCountObjectGuid, IDS_OBJECTNAME_ENABLED_COUNT, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_ProcessedCount, SVProcessedCountObjectGuid, IDS_OBJECTNAME_PROCESSED_COUNT, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_ToolPosition, SVToolPositionGuid, IDS_OBJECTNAME_TOOL_POSITION, false, SVResetItemNone );

	RegisterEmbeddedObject ( &m_ToolTime, SVToolTimeGuid, IDS_OBJECTNAME_TOOLTIME, false, SVResetItemNone );

	/////////////////////////////////////////////////////////////////////////
	// Extents - These are shadows of the extents in the SVImageInfoClass,
	// so that they can be exposed and be used by other objects in the system.
	// They are not updated directly (at this point in time).
	/////////////////////////////////////////////////////////////////////////
	RegisterEmbeddedObject( &m_ExtentLeft, SVExtentRelativeLeftPositionObjectGuid, IDS_OBJECTNAME_EXTENT_LEFT, false, SVResetItemTool, _T("Extent X") );
	RegisterEmbeddedObject( &m_ExtentTop, SVExtentRelativeTopPositionObjectGuid, IDS_OBJECTNAME_EXTENT_TOP, false, SVResetItemTool, _T("Extent Y") );
	RegisterEmbeddedObject( &m_ExtentRight, SVExtentRelativeRightPositionObjectGuid, IDS_OBJECTNAME_EXTENT_RIGHT, false, SVResetItemNone, _T("Extent X") );
	RegisterEmbeddedObject( &m_ExtentBottom, SVExtentRelativeBottomPositionObjectGuid, IDS_OBJECTNAME_EXTENT_BOTTOM, false, SVResetItemNone, _T("Extent Y") );
	RegisterEmbeddedObject( &m_ExtentWidth, SVExtentWidthObjectGuid, IDS_OBJECTNAME_EXTENT_WIDTH, false, SVResetItemTool, _T("Extent Width") );
	RegisterEmbeddedObject( &m_ExtentHeight, SVExtentHeightObjectGuid, IDS_OBJECTNAME_EXTENT_HEIGHT, false, SVResetItemTool, _T("Extent Height") );
	RegisterEmbeddedObject( &m_ExtentHeightScaleFactor, 
							SVExtentHeightScaleFactorObjectGuid, 
							IDS_OBJECTNAME_EXTENT_HEIGHTSF, 
							false, 
							SVResetItemTool,
							_T("Extent Height Scale Factor") );

	RegisterEmbeddedObject( &m_ExtentWidthScaleFactor, 
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
	m_svToolExtent.SetExtentObject( SVExtentPropertyPositionPointX, &m_ExtentLeft );
	m_svToolExtent.SetExtentObject( SVExtentPropertyPositionPointY, &m_ExtentTop );
	m_svToolExtent.SetExtentObject( SVExtentPropertyWidth, &m_ExtentWidth );
	m_svToolExtent.SetExtentObject( SVExtentPropertyHeight, &m_ExtentHeight );
	m_svToolExtent.SetExtentObject( SVExtentPropertyWidthScaleFactor, &m_ExtentWidthScaleFactor );
	m_svToolExtent.SetExtentObject( SVExtentPropertyHeightScaleFactor, &m_ExtentHeightScaleFactor );

	//
	// Set Embedded defaults
	enabled.SetDefaultValue( TRUE, TRUE );
	m_Passed.SetDefaultValue( FALSE, TRUE );			// Default for Passed is FALSE !!!
	m_Failed.SetDefaultValue( TRUE, TRUE );			// Default for Failed is TRUE !!!
	m_Warned.SetDefaultValue( TRUE, TRUE );			// Default for Warned is TRUE !!!
	m_ExplicitFailed.SetDefaultValue( FALSE, TRUE );	// Default for Explicit Failed is FALSE !!!

	m_PassedCount.SetDefaultValue( 0, TRUE );
	m_FailedCount.SetDefaultValue( 0, TRUE );
	m_WarnedCount.SetDefaultValue( 0, TRUE );

	m_EnabledCount.SetDefaultValue( 0, TRUE );
	m_ProcessedCount.SetDefaultValue( 0, TRUE );

	m_ToolPosition.SetDefaultValue( -1L, true );
	m_ToolTime.SetDefaultValue(0,TRUE);

	/////////////////////////////////////////////////////////////////////////
	// Set Default values for Shadowed Extents
	/////////////////////////////////////////////////////////////////////////
	m_ExtentLeft.SetDefaultValue( SvOi::cDefaultWindowToolLeft, TRUE );
	m_ExtentTop.SetDefaultValue( SvOi::cDefaultWindowToolTop, TRUE );
	m_ExtentRight.SetDefaultValue( SvOi::cDefaultWindowToolLeft + SvOi::cDefaultWindowToolWidth, TRUE );
	m_ExtentBottom.SetDefaultValue( SvOi::cDefaultWindowToolTop + SvOi::cDefaultWindowToolHeight, TRUE );
	m_ExtentWidth.SetDefaultValue( SvOi::cDefaultWindowToolWidth, TRUE );
	m_ExtentHeight.SetDefaultValue( SvOi::cDefaultWindowToolHeight, TRUE );
	m_ExtentWidthScaleFactor.SetDefaultValue( SvOi::cDefaultWindowToolWidthScaleFactor, TRUE );
	m_ExtentHeightScaleFactor.SetDefaultValue( SvOi::cDefaultWindowToolHeightScaleFactor, TRUE );

	drawToolFlag.SetEnumTypes( IDS_TOOLDRAW_ENUMOBJECT_LIST );
	drawToolFlag.SetDefaultValue( ( const long ) 0, TRUE ); // 0 Should be show tool 'Always'

	m_svUpdateAuxiliaryExtents.SetDefaultValue( FALSE, TRUE );

	m_svAuxiliarySourceX.SetDefaultValue( 0.0, TRUE );
	m_svAuxiliarySourceY.SetDefaultValue( 0.0, TRUE );
	m_svAuxiliarySourceAngle.SetDefaultValue( 0.0, TRUE );
	m_svAuxiliarySourceImageName.SetDefaultValue( "", TRUE );
	m_svAuxiliaryDrawType.SetDefaultValue( "", TRUE );

	m_pCurrentToolSet = nullptr;

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

			m_pCurrentToolSet = dynamic_cast<SVInspectionProcess*>(GetInspection())->GetToolSet();
		}
	}

	for( size_t j = 0;nullptr ==  m_pToolConditional && j < m_friendList.size(); j++ )
	{
		m_pToolConditional = dynamic_cast<SVConditionalClass *>(m_friendList[j].PObject);
	}

	// Set / Reset Printable Flags
	enabled.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_SETABLE_ONLINE | SV_REMOTELY_SETABLE;
	m_Passed.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_Failed.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_Warned.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_ExplicitFailed.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;

	m_PassedCount.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_FailedCount.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_WarnedCount.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;

	m_EnabledCount.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_ProcessedCount.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_ToolTime.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;

	m_ExtentLeft.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_SETABLE_ONLINE | SV_REMOTELY_SETABLE | SV_EXTENT_OBJECT;
	m_ExtentTop.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_SETABLE_ONLINE | SV_REMOTELY_SETABLE | SV_EXTENT_OBJECT;
	m_ExtentRight.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	m_ExtentBottom.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	m_ExtentWidth.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_REMOTELY_SETABLE | SV_EXTENT_OBJECT | SV_SETABLE_ONLINE;
	m_ExtentHeight.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_REMOTELY_SETABLE | SV_EXTENT_OBJECT | SV_SETABLE_ONLINE;
	
	// Defaults for the Scale Factors should be hidden (but NOT removed at this time, so 
	// don't use hideEmbeddedObject() here).
	m_ExtentWidthScaleFactor.ObjectAttributesAllowedRef() &=(~SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES);
	m_ExtentHeightScaleFactor.ObjectAttributesAllowedRef() &=(~SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES);

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

bool SVToolClass::DisconnectObjectInput( SVInObjectInfoStruct* pInObjectInfo )
{
	bool Result( SVTaskObjectListClass::DisconnectObjectInput( pInObjectInfo ) && nullptr != pInObjectInfo );

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

	return Result;
}

bool SVToolClass::IsEnabled() const
{
	bool bEnabled = true;
	enabled.GetValue( bEnabled );

	return bEnabled;
}

bool SVToolClass::WasEnabled() const
{
	bool bEnabled = true;

	if( nullptr != m_pCurrentToolSet )
	{
		bEnabled = m_pCurrentToolSet->WasEnabled();
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

	if( m_pCurrentToolSet )
	{
		SVEnumerateValueObjectClass* pSetEnum = m_pCurrentToolSet->GetDrawFlagObject();

		if( pSetEnum )
		{
			pSetEnum->GetValue( p_rMultiLine.m_ToolSetDrawFlag );
		}
	}
	
	drawToolFlag.GetValue( p_rMultiLine.m_ToolDrawFlag );

	BOOL bPassed = TRUE;
	BOOL bFailed = TRUE;
	BOOL bWarned = TRUE;

	m_Passed.GetValue( bPassed );
	m_Failed.GetValue( bFailed );
	m_Warned.GetValue( bWarned );

	p_rMultiLine.m_Passed = ( FALSE != bPassed );
	p_rMultiLine.m_Failed = ( FALSE != bFailed );
	p_rMultiLine.m_Warned = ( FALSE != bWarned );

	return l_Status;
}

void SVToolClass::UpdateAuxiliaryExtents(long resultDataIndex)
{
	if( dynamic_cast<SVInspectionProcess*>(GetInspection())->GetEnableAuxiliaryExtent() )
	{
		BOOL l_bUpdateSourceExtents = false;

		bool l_bForceOffsetReset = dynamic_cast<SVInspectionProcess*>(GetInspection())->m_bForceOffsetUpdate;

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
				m_svAuxiliarySourceImageName.SetValue( resultDataIndex, SVString(pAuxSourceImage->GetCompleteName()) );
			}

			SVString DrawType = m_svToolExtent.GetAuxiliaryDrawTypeString();
			m_svAuxiliaryDrawType.SetValue( resultDataIndex, DrawType );
			
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

bool SVToolClass::Run( SVRunStatusClass& RRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	bool retVal = true;
	long lCount;
	BOOL bIsValid = FALSE;

	clearRunErrorMessages();

	m_ToolTime.Start();

	if( !dynamic_cast<SVInspectionProcess*>(GetInspection())->GetNewDisableMethod() )
	{
		// First Set the old stuff forward for the counts
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
				m_ProcessedCount.GetValue( lCount );
				m_ProcessedCount.SetValue( RRunStatus.m_lResultDataIndex, ++lCount );
			}

			retVal = __super::Run( RRunStatus, &m_RunErrorMessages );

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
				m_EnabledCount.GetValue( lCount );
				m_EnabledCount.SetValue( RRunStatus.m_lResultDataIndex, ++lCount );
			}

			// set our state according to the runStatus
			m_Passed.SetValue( RRunStatus.m_lResultDataIndex, RRunStatus.IsPassed() );
			m_Failed.SetValue( RRunStatus.m_lResultDataIndex, RRunStatus.IsFailed() );
			m_ExplicitFailed.SetValue( RRunStatus.m_lResultDataIndex, RRunStatus.IsFailed() );
			m_Warned.SetValue( RRunStatus.m_lResultDataIndex, RRunStatus.IsWarned() );

			if( RRunStatus.m_UpdateCounters )
			{
				// Set Counts...
				lCount = 0;
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
		retVal = RunWithNewDisable( RRunStatus, &m_RunErrorMessages );
	}// end else

	//
	if( dynamic_cast<SVInspectionProcess*>(GetInspection())->GetEnableAuxiliaryExtent() )
	{
		UpdateAuxiliaryExtents(RRunStatus.m_lResultDataIndex);
	}
	
	m_ToolTime.Stop( RRunStatus.m_lResultDataIndex );

	if (nullptr != pErrorMessages && !m_RunErrorMessages.empty())
	{
		pErrorMessages->insert(pErrorMessages->end(), m_RunErrorMessages.begin(), m_RunErrorMessages.end());
	}

	return retVal;
}// end Run

bool SVToolClass::RunWithNewDisable( SVRunStatusClass& RRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	bool Return = true;
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
				m_ProcessedCount.GetValue( lCount );
				m_ProcessedCount.SetValue( RRunStatus.m_lResultDataIndex, ++lCount );
			}

			Return = SVTaskObjectListClass::Run( RRunStatus, pErrorMessages );

			if( Return )
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
		Return = !( RRunStatus.m_UpdateCounters );
		SetDisabled();
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
			m_EnabledCount.GetValue( lCount );
			m_EnabledCount.SetValue( RRunStatus.m_lResultDataIndex, ++lCount );
		}

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
	else
	{
		// Set Enabled Count...
		m_EnabledCount.GetValue( lCount );
		m_EnabledCount.SetValue( RRunStatus.m_lResultDataIndex, lCount );
	}// end else
	
	// Get Status Color...
	DWORD dwValue = RRunStatus.GetStatusColor();
	m_statusColor.SetValue( RRunStatus.m_lResultDataIndex, dwValue );

	// Get Status...
	dwValue = RRunStatus.GetState();
	m_statusTag.SetValue( RRunStatus.m_lResultDataIndex, dwValue );

	return Return;
}// end RunWithNewDisable

bool SVToolClass::onRun( SVRunStatusClass& RRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	bool bRetVal = __super::onRun( RRunStatus, pErrorMessages );

	if( bRetVal )
	{
		if( SV_NO_ATTRIBUTES != ( m_ExtentTop.ObjectAttributesAllowed() & SV_NO_ATTRIBUTES )  )
		{
			bRetVal = ( S_OK == m_ExtentTop.CopyLastSetValue( RRunStatus.m_lResultDataIndex ) ) && bRetVal;
		}
		if( SV_NO_ATTRIBUTES != ( m_ExtentLeft.ObjectAttributesAllowed() & SV_NO_ATTRIBUTES ) )
		{
			bRetVal = ( S_OK == m_ExtentLeft.CopyLastSetValue( RRunStatus.m_lResultDataIndex ) ) && bRetVal;
		}
		if( SV_NO_ATTRIBUTES != ( m_ExtentWidth.ObjectAttributesAllowed() & SV_NO_ATTRIBUTES ) )
		{
			bRetVal = ( S_OK == m_ExtentWidth.CopyLastSetValue( RRunStatus.m_lResultDataIndex ) ) && bRetVal;
		}
		if( SV_NO_ATTRIBUTES != ( m_ExtentHeight.ObjectAttributesAllowed() & SV_NO_ATTRIBUTES ) )
		{
			bRetVal = ( S_OK == m_ExtentHeight.CopyLastSetValue( RRunStatus.m_lResultDataIndex ) ) && bRetVal;
		}
		if( SV_NO_ATTRIBUTES != ( m_ExtentWidthScaleFactor.ObjectAttributesAllowed() & SV_NO_ATTRIBUTES ) )
		{
			bRetVal = ( S_OK == m_ExtentWidthScaleFactor.CopyLastSetValue( RRunStatus.m_lResultDataIndex ) ) && bRetVal;
		}
		if( SV_NO_ATTRIBUTES != ( m_ExtentHeightScaleFactor.ObjectAttributesAllowed() & SV_NO_ATTRIBUTES ) )
		{
			bRetVal = ( S_OK == m_ExtentHeightScaleFactor.CopyLastSetValue( RRunStatus.m_lResultDataIndex ) ) && bRetVal;
		}

		if( !bRetVal )
		{
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_5076_EXTENTSNOTCOPIED, SvOi::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
		}

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
	SVInspectionProcess* pInspection = dynamic_cast<SVInspectionProcess*>(GetInspection());
	if( nullptr != pInspection  )
	{
		pInspection->m_bForceOffsetUpdate = true;
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

		SvUl::SetBits( m_svAuxiliarySourceX.ObjectAttributesAllowedRef(), SV_HIDDEN, false );
		SvUl::SetBits( m_svAuxiliarySourceY.ObjectAttributesAllowedRef(), SV_HIDDEN, false );
		SvUl::SetBits( m_svAuxiliarySourceAngle.ObjectAttributesAllowedRef(), SV_HIDDEN, false );
		SvUl::SetBits( m_svAuxiliaryDrawType.ObjectAttributesAllowedRef(), SV_HIDDEN, false );
		SvUl::SetBits( m_svAuxiliarySourceImageName.ObjectAttributesAllowedRef(), SV_HIDDEN, false );
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

		SvUl::SetBits( m_svAuxiliarySourceX.ObjectAttributesAllowedRef(), SV_HIDDEN, true );
		SvUl::SetBits( m_svAuxiliarySourceY.ObjectAttributesAllowedRef(), SV_HIDDEN, true );
		SvUl::SetBits( m_svAuxiliarySourceAngle.ObjectAttributesAllowedRef(), SV_HIDDEN, true );
		SvUl::SetBits( m_svAuxiliaryDrawType.ObjectAttributesAllowedRef(), SV_HIDDEN, true );
		SvUl::SetBits( m_svAuxiliarySourceImageName.ObjectAttributesAllowedRef(), SV_HIDDEN, true );
	}
	return l_hr;
}

bool SVToolClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = __super::ResetObject(pErrorMessages) && ValidateLocal(pErrorMessages);

	bool l_bReset = false;
	SVInspectionProcess* pInspection = dynamic_cast<SVInspectionProcess*>(GetInspection());
	if ( nullptr != pInspection && 
		 nullptr != pInspection->GetToolSet() && 
		 ( S_OK == pInspection->GetToolSet()->getResetCounts( l_bReset ) ) &&
		 l_bReset )
	{
		// Reset Counter...
		m_PassedCount.SetDefaultValue( 0, TRUE );
		m_FailedCount.SetDefaultValue( 0, TRUE );
		m_WarnedCount.SetDefaultValue( 0, TRUE );
		m_EnabledCount.SetDefaultValue( 0, TRUE );
		m_ProcessedCount.SetDefaultValue( 0, TRUE );
	}

	///UpdateBottomAndRight is called again when imageExtents are changed by ToolsizeAdjust
	if ( Result )
	{
		UpdateBottomAndRight();
	}

	// Auxiliary Extents
	double l_dValue = 0;
	
	if( dynamic_cast<SVInspectionProcess*>(GetInspection())->GetEnableAuxiliaryExtent() )
	{
		m_svUpdateAuxiliaryExtents.ObjectAttributesAllowedRef() |=  SV_VIEWABLE 
			| SV_ARCHIVABLE
			| SV_SELECTABLE_FOR_EQUATION
			| SV_SELECTABLE_FOR_STATISTICS 
			| SV_PUBLISHABLE;
		m_svUpdateAuxiliaryExtents.GetValue( l_dValue );

		SvUl::SetBits( m_svUpdateAuxiliaryExtents.ObjectAttributesAllowedRef(), SV_HIDDEN, false );
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

		SvUl::SetBits( m_svUpdateAuxiliaryExtents.ObjectAttributesAllowedRef(), SV_HIDDEN, true );
	}

	EnableAuxiliaryExtents( l_dValue > 0 );

	return Result;
}

void SVToolClass::UpdateBottomAndRight()
{
	SVImageExtentClass l_svExtents;

	if ( S_OK == GetImageExtent( l_svExtents ) )
	{
		SVExtentFigureStruct l_svFigure;

		if ( S_OK == l_svExtents.GetFigure( l_svFigure ) )
		{
			if( SV_NO_ATTRIBUTES != m_ExtentRight.ObjectAttributesAllowed() )
			{
				long l_lValue = static_cast<long>(l_svFigure.m_svBottomRight.m_dPositionX);

				m_ExtentRight.SetValue( 1, l_lValue );
			}

			if( SV_NO_ATTRIBUTES != m_ExtentBottom.ObjectAttributesAllowed() )
			{
				long l_lValue = static_cast<long>(l_svFigure.m_svBottomRight.m_dPositionY);

				m_ExtentBottom.SetValue( 1, l_lValue );
			}
		}
	}
}

const SVImageClass* SVToolClass::GetToolImage() const
{
	return m_svToolExtent.GetToolImage();
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

bool SVToolClass::SetDefaultFormulas(SvStl::MessageContainerVector *pErrorMessages)
{
	return true;
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

	hideEmbeddedObject( m_ExtentLeft );	 // Make it Un-Selectable for anything
	RemoveEmbeddedObject( &m_ExtentLeft ); // Remove it from the Embedded List so it is not scripted

	hideEmbeddedObject( m_ExtentTop );
	RemoveEmbeddedObject( &m_ExtentTop );

	hideEmbeddedObject( m_ExtentRight );
	RemoveEmbeddedObject( &m_ExtentRight );

	hideEmbeddedObject( m_ExtentBottom );
	RemoveEmbeddedObject( &m_ExtentBottom );

	hideEmbeddedObject( m_ExtentWidth );
	RemoveEmbeddedObject( &m_ExtentWidth );

	hideEmbeddedObject( m_ExtentHeight );
	RemoveEmbeddedObject( &m_ExtentHeight );

	hideEmbeddedObject( m_ExtentWidthScaleFactor );
	RemoveEmbeddedObject( &m_ExtentWidthScaleFactor );

	hideEmbeddedObject( m_ExtentHeightScaleFactor );
	RemoveEmbeddedObject( &m_ExtentHeightScaleFactor );
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
	SVToolSetClass* pToolSet = dynamic_cast<SVInspectionProcess*>(GetInspection())->GetToolSet();
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
			 l_psvTool = dynamic_cast<SVToolClass*>( l_psvImageParent->GetTool() );

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

		::KeepPrevError( l_hr, ConnectToObject( &m_AuxSourceImageObjectInfo, l_psvImage ) );

		m_svToolExtent.SetSelectedImage( GetAuxSourceImage() );

		SVInspectionProcess* pInspection = dynamic_cast<SVInspectionProcess*>(GetInspection());
		if ( nullptr != pInspection )
		{
			pInspection->m_bForceOffsetUpdate = true;
		}
		UpdateAuxiliaryExtents(1);
		GetInspection()->resetAllObjects();
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

const SVImageInfoClass* SVToolClass::getFirstImageInfo() const
{
	const SVImageInfoClass* pRetVal = nullptr;
	SVObjectTypeInfoStruct objectInfo(SVImageObjectType);
	SVImageClass* pImage = dynamic_cast<SVImageClass*>(getFirstObject(objectInfo, false)); 
	if (nullptr != pImage)
	{
		pRetVal = &pImage->GetImageInfo();
	}

	return pRetVal;
}

#pragma region ITool methods
bool SVToolClass::areAuxExtentsAvailable() const
{
	bool bRetVal = true;
	// check inspection, and has image input!
	if (nullptr == GetToolImage() || 
		0 == dynamic_cast<SVInspectionProcess*>(GetInspection())->GetEnableAuxiliaryExtent())
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
	SVImageClass* pImage = dynamic_cast<SVImageClass*> (SvOi::getObject(rObjectID));
	if( nullptr != pImage )
	{
		hr = SetAuxSourceImage( pImage );
	}
	return hr;
}

void SVToolClass::SetToolImage( const SVGUID& rObjectID )
{
	SVImageClass* pImage = dynamic_cast<SVImageClass*> (SvOi::getObject(rObjectID));
	m_svToolExtent.SetToolImage( pImage );
}

long SVToolClass::getToolPosition() const
{
	long Result( -1 );
	m_ToolPosition.GetValue( Result ); 
	return Result; 
};
#pragma endregion ITool methods

SVStaticStringValueObjectClass* SVToolClass::GetInputImageNames( )
{
	return nullptr;
}


bool SVToolClass::SetFirstInputImageName( LPCTSTR FirstName )
{
	SVStaticStringValueObjectClass* lstrVO = GetInputImageNames();

	if( lstrVO )
	{
		lstrVO->SetValue( 0/*Static value, this parameter will not used*/, FirstName);
		return true;
	}
	return false;
}



SVValueObjectClass* SVToolClass::GetToolComment()
{
	return &m_svToolComment;
}

bool SVToolClass::createAllObjectsFromChild( SVObjectClass& rChildObject )
{
	SVToolLevelCreateStruct createStruct;
	createStruct.OwnerObjectInfo        = this;
	createStruct.ToolObjectInfo			= this;
	createStruct.InspectionObjectInfo	= GetInspection();

	return rChildObject.createAllObjects(createStruct);
}

void SVToolClass::connectChildObject( SVTaskObjectClass& rChildObject )
{
	SVToolLevelCreateStruct createStruct;
	createStruct.OwnerObjectInfo        = this;
	createStruct.ToolObjectInfo			= this;
	createStruct.InspectionObjectInfo	= GetInspection();

	rChildObject.ConnectObject(createStruct);
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

bool SVToolClass::ValidateLocal(SvStl::MessageContainerVector *pErrorMessages) const
{
	if( inputConditionBoolObjectInfo.IsConnected() &&
		inputConditionBoolObjectInfo.GetInputObjectInfo().PObject )
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
