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
#include "SVObjectLibrary/SVClsids.h"
#include "ObjectInterfaces/IObjectManager.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVObjectLibrary/SVAnalyzerLevelCreateStruct.h"
#include "SVUtilityLibrary/SetBits.h"
#include "SVAnalyzer.h"
#include "SVConditional.h"
#include "ObjectInterfaces/GlobalConst.h"
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
	m_canResizeToParent = false;
	// Indentify our type in the Output List
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SVToolObjectType;

	// Register Embedded Objects
	RegisterEmbeddedObject( &enabled, SVToolEnabledObjectGuid, IDS_OBJECTNAME_ENABLED, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_Passed, SVPassedObjectGuid, IDS_OBJECTNAME_PASSED, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_Failed, SVFailedObjectGuid, IDS_OBJECTNAME_FAILED, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_Warned, SVWarnedObjectGuid, IDS_OBJECTNAME_WARNED, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_ExplicitFailed, SVExplicitFailedObjectGuid, IDS_OBJECTNAME_EXPLICIT_FAILED, false, SvOi::SVResetItemNone );

	RegisterEmbeddedObject( &m_PassedCount, SVPassedCountObjectGuid, IDS_OBJECTNAME_PASSED_COUNT, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_FailedCount, SVFailedCountObjectGuid, IDS_OBJECTNAME_FAILED_COUNT, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_WarnedCount, SVWarnedCountObjectGuid, IDS_OBJECTNAME_WARNED_COUNT, false, SvOi::SVResetItemNone );

	RegisterEmbeddedObject( &m_EnabledCount, SVEnabledCountObjectGuid, IDS_OBJECTNAME_ENABLED_COUNT, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_ProcessedCount, SVProcessedCountObjectGuid, IDS_OBJECTNAME_PROCESSED_COUNT, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_ToolPosition, SVToolPositionGuid, IDS_OBJECTNAME_TOOL_POSITION, false, SvOi::SVResetItemNone );

	RegisterEmbeddedObject ( &m_ToolTime, SVToolTimeGuid, IDS_OBJECTNAME_TOOLTIME, false, SvOi::SVResetItemNone );

	/////////////////////////////////////////////////////////////////////////
	// Extents - These are shadows of the extents in the SVImageInfoClass,
	// so that they can be exposed and be used by other objects in the system.
	// They are not updated directly (at this point in time).
	/////////////////////////////////////////////////////////////////////////
	//Special type names for extents
	m_ExtentWidth.SetTypeName( _T("Extent Width") );
	m_ExtentHeight.SetTypeName( _T("Extent Height") );
	m_ExtentLeft.SetTypeName( _T("Extent X") );
	m_ExtentTop.SetTypeName( _T("Extent Y") );
	m_ExtentRight.SetTypeName( _T("Extent X") );
	m_ExtentBottom.SetTypeName( _T("Extent Y") );
	m_svAuxiliarySourceX.SetTypeName( _T("Extent X") );
	m_svAuxiliarySourceY.SetTypeName( _T("Extent Y") );
	m_svAuxiliarySourceAngle.SetTypeName( _T("Extent Angle") );
	m_ExtentHeightScaleFactor.SetTypeName( _T("Extent Height Scale Factor") );
	m_ExtentWidthScaleFactor.SetTypeName( _T("Extent Width Scale Factor") );

	RegisterEmbeddedObject( &m_ExtentLeft, SVExtentRelativeLeftPositionObjectGuid, IDS_OBJECTNAME_EXTENT_LEFT, false, SvOi::SVResetItemTool );
	RegisterEmbeddedObject( &m_ExtentTop, SVExtentRelativeTopPositionObjectGuid, IDS_OBJECTNAME_EXTENT_TOP, false, SvOi::SVResetItemTool );
	RegisterEmbeddedObject( &m_ExtentRight, SVExtentRelativeRightPositionObjectGuid, IDS_OBJECTNAME_EXTENT_RIGHT, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_ExtentBottom, SVExtentRelativeBottomPositionObjectGuid, IDS_OBJECTNAME_EXTENT_BOTTOM, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_ExtentWidth, SVExtentWidthObjectGuid, IDS_OBJECTNAME_EXTENT_WIDTH, false, SvOi::SVResetItemTool );
	RegisterEmbeddedObject( &m_ExtentHeight, SVExtentHeightObjectGuid, IDS_OBJECTNAME_EXTENT_HEIGHT, false, SvOi::SVResetItemTool );
	RegisterEmbeddedObject( &m_ExtentHeightScaleFactor, SVExtentHeightScaleFactorObjectGuid, IDS_OBJECTNAME_EXTENT_HEIGHTSF, false, SvOi::SVResetItemTool );
	RegisterEmbeddedObject( &m_ExtentWidthScaleFactor, SVExtentWidthScaleFactorObjectGuid, IDS_OBJECTNAME_EXTENT_WIDTHSF, false, SvOi::SVResetItemTool );
	RegisterEmbeddedObject( &ToolSelectedForOperatorMove, SVToolSelectedForOperatorMoveObjectGuid, IDS_OBJECTNAME_TOOL_SELECTED_FOR_OPERATOR_MOVE, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject( &drawToolFlag, SVConditionalToolDrawFlagObjectGuid, IDS_OBJECTNAME_DRAWTOOL_FLAG, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_svUpdateAuxiliaryExtents, SVUpdateAuxiliaryExtentsObjectGuid, IDS_OBJECTNAME_UPDATE_AUXILIARY_EXTENTS_OBJECT, false, SvOi::SVResetItemTool );
	RegisterEmbeddedObject( &m_svAuxiliarySourceX, SVAuxiliarySourceXObjectGuid, IDS_OBJECTNAME_AUXILIARY_SOURCE_X, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_svAuxiliarySourceY, SVAuxiliarySourceYObjectGuid, IDS_OBJECTNAME_AUXILIARY_SOURCE_Y, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_svAuxiliarySourceAngle, SVAuxiliarySourceAngleObjectGuid, IDS_OBJECTNAME_AUXILIARY_SOURCE_ANGLE, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_svAuxiliarySourceImageName, SVAuxiliarySourceImageNameObjectGuid, IDS_OBJECTNAME_AUXILIARY_SOURCE_IMAGE_NAME, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_svAuxiliaryDrawType, SVAuxiliaryDrawTypeObjectGuid, IDS_OBJECTNAME_AUXILIARY_DRAW_TYPE_NAME, false, SvOi::SVResetItemNone );
	// Tool Comment...
	RegisterEmbeddedObject( &m_ToolComment, SVToolCommentTypeObjectGuid, IDS_OBJECTNAME_TOOL_COMMENT, false, SvOi::SVResetItemNone );
	
	m_svToolExtent.SetTool( this );
	m_svToolExtent.SetImageType( SvOi::SVImageTypeEnum::SVImageTypeLogicalAndPhysical );
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
	enabled.SetDefaultValue( BOOL(true), true );
	m_Passed.SetDefaultValue( BOOL(false), true );			// Default for Passed is FALSE !!!
	m_Failed.SetDefaultValue( BOOL(true), true );			// Default for Failed is TRUE !!!
	m_Warned.SetDefaultValue( BOOL(true), true );			// Default for Warned is TRUE !!!
	m_ExplicitFailed.SetDefaultValue( BOOL(false), true );	// Default for Explicit Failed is FALSE !!!

	m_PassedCount.SetDefaultValue( 0L, true );
	m_FailedCount.SetDefaultValue( 0L, true );
	m_WarnedCount.SetDefaultValue( 0L, true );

	m_EnabledCount.SetDefaultValue( 0L, true );
	m_ProcessedCount.SetDefaultValue( 0L, true );

	m_ToolPosition.SetDefaultValue( -1L, true );
	m_ToolTime.SetDefaultValue( 0LL, true);

	/////////////////////////////////////////////////////////////////////////
	// Set Default values for Shadowed Extents
	/////////////////////////////////////////////////////////////////////////
	m_ExtentLeft.SetDefaultValue( SvOi::cDefaultWindowToolLeft, true );
	m_ExtentTop.SetDefaultValue( SvOi::cDefaultWindowToolTop, true );
	m_ExtentRight.SetDefaultValue( SvOi::cDefaultWindowToolLeft + SvOi::cDefaultWindowToolWidth, true );
	m_ExtentBottom.SetDefaultValue( SvOi::cDefaultWindowToolTop + SvOi::cDefaultWindowToolHeight, true );
	m_ExtentWidth.SetDefaultValue( SvOi::cDefaultWindowToolWidth, true );
	m_ExtentHeight.SetDefaultValue( SvOi::cDefaultWindowToolHeight, true );
	m_ExtentWidthScaleFactor.SetDefaultValue( SvOi::cDefaultWindowToolWidthScaleFactor, true );
	m_ExtentHeightScaleFactor.SetDefaultValue( SvOi::cDefaultWindowToolHeightScaleFactor, true );

	drawToolFlag.SetEnumTypes( IDS_TOOLDRAW_ENUMOBJECT_LIST );
	drawToolFlag.SetDefaultValue( 0L, true );

	m_svUpdateAuxiliaryExtents.SetDefaultValue( BOOL(false), true );

	m_svAuxiliarySourceX.SetDefaultValue( 0.0, true );
	m_svAuxiliarySourceY.SetDefaultValue( 0.0, true );
	m_svAuxiliarySourceAngle.SetDefaultValue( 0.0, true );
	m_svAuxiliarySourceImageName.SetDefaultValue( _T(""), true );
	m_svAuxiliaryDrawType.SetDefaultValue( "", true );

	m_pCurrentToolSet = nullptr;

	// Auxiliary Source Image.
	m_AuxSourceImageObjectInfo.SetInputObjectType( SVImageObjectType );
	m_AuxSourceImageObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_AuxSourceImageObjectInfo, _T( "ToolAuxSourceImage" ) );

	// instantiate the Conditional class
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
		if( GetInspectionInterface() )
		{
			bOk = TRUE;

			SvOi::IInspectionProcess* iIP = GetInspectionInterface();
			m_pCurrentToolSet = dynamic_cast<SvOi::IToolSet*>(iIP->GetToolSetInterface());
		}
	}

	for( size_t j = 0;nullptr ==  m_pToolConditional && j < m_friendList.size(); j++ )
	{
		//conditional must be the first friend because it will be blocked in runFriends if tool
		assert(0 == j);
		m_pToolConditional = dynamic_cast<SVConditionalClass *>(m_friendList[j].m_pObject);
		setSkipFirstFriendFromRun();
	}

	// Set / Reset Printable Flags
	enabled.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE | SvOi::SV_SETABLE_ONLINE | SvOi::SV_REMOTELY_SETABLE, SvOi::SetAttributeType::AddAttribute );
	m_Passed.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	m_Failed.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	m_Warned.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	m_ExplicitFailed.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );

	m_PassedCount.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	m_FailedCount.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	m_WarnedCount.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );

	m_EnabledCount.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	m_ProcessedCount.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	m_ToolTime.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );

	m_ExtentLeft.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE | SvOi::SV_SETABLE_ONLINE | SvOi::SV_REMOTELY_SETABLE | SvOi::SV_EXTENT_OBJECT, SvOi::SetAttributeType::AddAttribute );
	m_ExtentTop.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE | SvOi::SV_SETABLE_ONLINE | SvOi::SV_REMOTELY_SETABLE | SvOi::SV_EXTENT_OBJECT, SvOi::SetAttributeType::AddAttribute );
	m_ExtentRight.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
	m_ExtentBottom.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
	m_ExtentWidth.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE | SvOi::SV_REMOTELY_SETABLE | SvOi::SV_EXTENT_OBJECT | SvOi::SV_SETABLE_ONLINE, SvOi::SetAttributeType::AddAttribute );
	m_ExtentHeight.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE | SvOi::SV_REMOTELY_SETABLE | SvOi::SV_EXTENT_OBJECT | SvOi::SV_SETABLE_ONLINE, SvOi::SetAttributeType::AddAttribute );
	
	// Defaults for the Scale Factors should be hidden (but NOT removed at this time, so 
	// don't use hideEmbeddedObject() here).
	m_ExtentWidthScaleFactor.SetObjectAttributesAllowed( SvOi::SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES, SvOi::SetAttributeType::RemoveAttribute );
	m_ExtentHeightScaleFactor.SetObjectAttributesAllowed( SvOi::SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES, SvOi::SetAttributeType::RemoveAttribute );

	// Auxiliary Tool Source Extent
	m_svUpdateAuxiliaryExtents.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE | SvOi::SV_SETABLE_ONLINE | SvOi::SV_REMOTELY_SETABLE, SvOi::SetAttributeType::AddAttribute );
	
	drawToolFlag.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );

	// Tool Comment attributes...
	m_ToolComment.setStatic( true );
	m_ToolComment.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
	m_ToolComment.SetObjectAttributesAllowed( SvOi::SV_VIEWABLE, SvOi::SetAttributeType::RemoveAttribute );	// We do not want this to show up in the results picker.

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
		if( pInObjectInfo->GetInputObjectInfo().m_pObject == m_svToolExtent.GetToolImage() )
		{
			m_svToolExtent.SetToolImage( nullptr );
		}

		if( pInObjectInfo->GetInputObjectInfo().m_pObject == m_svToolExtent.GetSelectedImage() )
		{
			m_svToolExtent.SetSelectedImage( nullptr );
		}
	}

	return Result;
}

bool SVToolClass::IsEnabled() const
{
	BOOL bEnabled( true );
	enabled.GetValue( bEnabled );

	return bEnabled ? true : false;
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
	if( GetInspectionInterface()->GetEnableAuxiliaryExtent() )
	{
		BOOL l_bUpdateSourceExtents = false;

		bool l_bForceOffsetReset = GetInspectionInterface()->IsOffsetUpdateForced();

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
				m_svAuxiliarySourceImageName.SetValue( pAuxSourceImage->GetCompleteName(), resultDataIndex );
			}

			SVString DrawType = m_svToolExtent.GetAuxiliaryDrawTypeString();
			m_svAuxiliaryDrawType.SetValue( DrawType, resultDataIndex );
			
			SVExtentPointStruct pt;
			SVImageClass* pImage = m_svToolExtent.GetToolImage();
			if (pImage && pAuxSourceImage)
			{
				pImage->TranslateFromOutputSpaceToImage(pAuxSourceImage, pt, pt);
			}
			m_svAuxiliarySourceX.SetValue( pt.m_dPositionX, resultDataIndex );
			m_svAuxiliarySourceY.SetValue(  pt.m_dPositionY, resultDataIndex );

			m_svAuxiliarySourceAngle.SetValue( l_svOffsetData.m_dRotationAngle, resultDataIndex );
		}
	}
}

bool SVToolClass::Run( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	bool retVal = true;
	long lCount;
	BOOL bIsValid( false );

	clearRunErrorMessages();

	m_ToolTime.Start();

	if( !GetInspectionInterface()->IsNewDisableMethodSet() )
	{
		// First Set the old stuff forward for the counts
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

		if( IsEnabled() )
		{
			SVImageClassPtrSet::iterator l_oImageIter;

			for( l_oImageIter = m_ImageObjectSet.begin(); l_oImageIter != m_ImageObjectSet.end(); ++l_oImageIter )
			{
				(*l_oImageIter)->CopyImageTo( rRunStatus.Images );
			}

			if( rRunStatus.m_UpdateCounters )
			{
				// Set Processed Count...
				long lCount = 0;
				m_ProcessedCount.GetValue( lCount );
				m_ProcessedCount.SetValue( ++lCount, rRunStatus.m_lResultDataIndex );
			}

			if (m_pToolConditional)
			{
				m_pToolConditional->Run(rRunStatus, &m_RunErrorMessages);
			}

			if (!m_pToolConditional || getConditionalResult())
			{
				retVal = __super::Run(rRunStatus, &m_RunErrorMessages);

				if (retVal)
				{
					rRunStatus.SetValid();
				}
				else
				{
					rRunStatus.SetInvalid();
					SetInvalid();
				}
			}
			else
			{
				rRunStatus.SetDisabledByCondition();
				//bDisabled = TRUE;
			}// end else
		}
		else
		{
			retVal = !( rRunStatus.m_UpdateCounters );

			rRunStatus.SetDisabled();
			// To set disabled color for any part of this tool ( embeddeds, children, etc. )
			SetDisabled();
		}

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

			// set our state according to the runStatus
			m_Passed.SetValue( BOOL(rRunStatus.IsPassed()), rRunStatus.m_lResultDataIndex );
			m_Failed.SetValue( BOOL(rRunStatus.IsFailed()), rRunStatus.m_lResultDataIndex );
			m_ExplicitFailed.SetValue( BOOL(rRunStatus.IsFailed()), rRunStatus.m_lResultDataIndex );
			m_Warned.SetValue( BOOL(rRunStatus.IsWarned()), rRunStatus.m_lResultDataIndex );

			if( rRunStatus.m_UpdateCounters )
			{
				// Set Counts...
				lCount = 0;
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
		retVal = RunWithNewDisable( rRunStatus, &m_RunErrorMessages );
	}// end else

	//
	if( GetInspectionInterface()->GetEnableAuxiliaryExtent() )
	{
		UpdateAuxiliaryExtents(rRunStatus.m_lResultDataIndex);
	}
	
	m_ToolTime.Stop( rRunStatus.m_lResultDataIndex );

	if (nullptr != pErrorMessages && !m_RunErrorMessages.empty())
	{
		pErrorMessages->insert(pErrorMessages->end(), m_RunErrorMessages.begin(), m_RunErrorMessages.end());
	}

	return retVal;
}// end Run

bool SVToolClass::RunWithNewDisable( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	bool Return = true;
	long lCount = 0;
	
	BOOL bDisabled = FALSE;

	if( IsEnabled() )
	{
		SvOi::IValueObjectPtrSet::iterator ValueIter( m_ValueObjectSet.begin() );
		for( ; m_ValueObjectSet.end() != ValueIter; ++ValueIter )
		{
			(*ValueIter)->CopyLastSetValue( rRunStatus.m_lResultDataIndex );
		}

		SVImageClassPtrSet::iterator ImageIter( m_ImageObjectSet.begin() );
		for( ; m_ImageObjectSet.end() != ImageIter ; ++ImageIter )
		{
			(*ImageIter)->CopyImageTo( rRunStatus.Images );
		}

		if( m_pToolConditional )
		{
			m_pToolConditional->Run( rRunStatus, pErrorMessages );
		}

		if( !m_pToolConditional || getConditionalResult() )
		{
			if( rRunStatus.m_UpdateCounters )
			{
				// Set Processed Count...
				m_ProcessedCount.GetValue( lCount );
				m_ProcessedCount.SetValue( ++lCount, rRunStatus.m_lResultDataIndex );
			}

			Return = SVTaskObjectListClass::Run( rRunStatus, pErrorMessages );

			if( Return )
			{
				rRunStatus.SetValid();
			}
			else
			{
				rRunStatus.SetInvalid();
				SetInvalid();
			}// end else

		}// end if
		else
		{
			rRunStatus.SetDisabledByCondition();
			bDisabled = TRUE;
		}// end else

	}// end if
	else
	{
		Return = !( rRunStatus.m_UpdateCounters );
		SetDisabled();
		rRunStatus.SetDisabled();
		bDisabled = TRUE;
	}// end else

	// if disabled or disabled by condition
	// leave in previous state
	if( !bDisabled && !rRunStatus.IsDisabled() && !rRunStatus.IsDisabledByCondition() )
	{
		if( rRunStatus.m_UpdateCounters )
		{
			// Set Enabled Count...
			m_EnabledCount.GetValue( lCount );
			m_EnabledCount.SetValue( ++lCount, rRunStatus.m_lResultDataIndex );
		}

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
	else
	{
		// Set Enabled Count...
		m_EnabledCount.GetValue( lCount );
		m_EnabledCount.SetValue( lCount, rRunStatus.m_lResultDataIndex );
	}// end else
	
	// Get Status Color...
	DWORD dwValue = rRunStatus.GetStatusColor();
	m_statusColor.SetValue( dwValue, rRunStatus.m_lResultDataIndex );

	// Get Status...
	dwValue = rRunStatus.GetState();
	m_statusTag.SetValue( dwValue, rRunStatus.m_lResultDataIndex );

	return Return;
}// end RunWithNewDisable

bool SVToolClass::onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	bool Result = __super::onRun( rRunStatus, pErrorMessages );

	if( Result )
	{
		if( SvOi::SV_NO_ATTRIBUTES != ( m_ExtentTop.ObjectAttributesAllowed() & SvOi::SV_NO_ATTRIBUTES )  )
		{
			Result = ( S_OK == m_ExtentTop.CopyLastSetValue( rRunStatus.m_lResultDataIndex ) ) && Result;
		}
		if( SvOi::SV_NO_ATTRIBUTES != ( m_ExtentLeft.ObjectAttributesAllowed() & SvOi::SV_NO_ATTRIBUTES ) )
		{
			Result = ( S_OK == m_ExtentLeft.CopyLastSetValue( rRunStatus.m_lResultDataIndex ) ) && Result;
		}
		if( SvOi::SV_NO_ATTRIBUTES != ( m_ExtentWidth.ObjectAttributesAllowed() & SvOi::SV_NO_ATTRIBUTES ) )
		{
			Result = ( S_OK == m_ExtentWidth.CopyLastSetValue( rRunStatus.m_lResultDataIndex ) ) && Result;
		}
		if( SvOi::SV_NO_ATTRIBUTES != ( m_ExtentHeight.ObjectAttributesAllowed() & SvOi::SV_NO_ATTRIBUTES ) )
		{
			Result = ( S_OK == m_ExtentHeight.CopyLastSetValue( rRunStatus.m_lResultDataIndex ) ) && Result;
		}
		if( SvOi::SV_NO_ATTRIBUTES != ( m_ExtentWidthScaleFactor.ObjectAttributesAllowed() & SvOi::SV_NO_ATTRIBUTES ) )
		{
			Result = ( S_OK == m_ExtentWidthScaleFactor.CopyLastSetValue( rRunStatus.m_lResultDataIndex ) ) && Result;
		}
		if( SvOi::SV_NO_ATTRIBUTES != ( m_ExtentHeightScaleFactor.ObjectAttributesAllowed() & SvOi::SV_NO_ATTRIBUTES ) )
		{
			Result = ( S_OK == m_ExtentHeightScaleFactor.CopyLastSetValue( rRunStatus.m_lResultDataIndex ) ) && Result;
		}

		if( !Result )
		{
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_5076_EXTENTSNOTCOPIED, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
		}

		// Friends were running, validation was successfully
		// Check conditional execution
		if ( !getConditionalResult() )
		{
			rRunStatus.SetDisabledByCondition();
		}
	}

	return Result;
}

bool SVToolClass::getConditionalResult() const
{
	BOOL Value( false );
	SVBoolValueObjectClass* pBoolObject;

	if( inputConditionBoolObjectInfo.IsConnected() &&
		inputConditionBoolObjectInfo.GetInputObjectInfo().m_pObject )
	{
		pBoolObject = ( SVBoolValueObjectClass* )inputConditionBoolObjectInfo.GetInputObjectInfo().m_pObject;
		pBoolObject->GetValue( Value );
	}
	return Value ? true : false;
}

bool SVToolClass::getConditionalResult(long Index) const
{
	BOOL Value( false );
	SVBoolValueObjectClass* pBoolObject;

	if( inputConditionBoolObjectInfo.IsConnected() &&
		inputConditionBoolObjectInfo.GetInputObjectInfo().m_pObject )
	{
		pBoolObject = ( SVBoolValueObjectClass* )inputConditionBoolObjectInfo.GetInputObjectInfo().m_pObject;
		pBoolObject->GetValue( Value, Index );
	}
	return Value ? true : false;
}

HRESULT SVToolClass::SetImageExtentProperty( SVExtentPropertyEnum p_eProperty, SvOi::IValueObject* pValueObject )
{
	return m_svToolExtent.SetExtentObject( p_eProperty, pValueObject );
}

HRESULT SVToolClass::GetRootOffsetData( SVExtentOffsetStruct& p_rsvOffsetData )
{
	return m_svToolExtent.GetRootOffsetData( p_rsvOffsetData );
}

HRESULT SVToolClass::UpdateOffsetData( SVImageClass* p_svToolImage )
{
	SvOi::IInspectionProcess* pInspection = GetInspectionInterface();
	if( nullptr != pInspection  )
	{
		pInspection->ForceOffsetUpdate();
	}

	return m_svToolExtent.UpdateOffsetData( true, p_svToolImage );
}

HRESULT SVToolClass::TranslatePointToSource( SVExtentPointStruct p_svIn, SVExtentPointStruct& p_rsvOut )
{
	return m_svToolExtent.TranslatePointToSource( p_svIn, p_rsvOut );
}

HRESULT SVToolClass::EnableAuxiliaryExtents( bool p_bEnable )
{
	const UINT cAttributes = SvOi::SV_VIEWABLE | SvOi::SV_ARCHIVABLE | SvOi::SV_SELECTABLE_FOR_EQUATION | SvOi::SV_SELECTABLE_FOR_STATISTICS | SvOi::SV_PUBLISHABLE;

	if( p_bEnable )
	{
		m_svAuxiliarySourceX.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
		m_svAuxiliarySourceY.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
		m_svAuxiliarySourceAngle.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
		m_svAuxiliaryDrawType.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
		m_svAuxiliarySourceImageName.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );

		m_svAuxiliarySourceX.SetObjectAttributesAllowed( SvOi::SV_HIDDEN, SvOi::SetAttributeType::RemoveAttribute );
		m_svAuxiliarySourceY.SetObjectAttributesAllowed( SvOi::SV_HIDDEN, SvOi::SetAttributeType::RemoveAttribute );
		m_svAuxiliarySourceAngle.SetObjectAttributesAllowed( SvOi::SV_HIDDEN, SvOi::SetAttributeType::RemoveAttribute );
		m_svAuxiliaryDrawType.SetObjectAttributesAllowed( SvOi::SV_HIDDEN, SvOi::SetAttributeType::RemoveAttribute );
		m_svAuxiliarySourceImageName.SetObjectAttributesAllowed( SvOi::SV_HIDDEN, SvOi::SetAttributeType::RemoveAttribute );
	}
	else
	{
		m_svAuxiliarySourceX.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::RemoveAttribute );
		m_svAuxiliarySourceY.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::RemoveAttribute );
		m_svAuxiliarySourceAngle.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::RemoveAttribute );
		m_svAuxiliaryDrawType.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::RemoveAttribute );
		m_svAuxiliarySourceImageName.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::RemoveAttribute );

		m_svAuxiliarySourceX.SetObjectAttributesSet( cAttributes, SvOi::SetAttributeType::RemoveAttribute );;
		m_svAuxiliarySourceY.SetObjectAttributesSet( cAttributes, SvOi::SetAttributeType::RemoveAttribute );
		m_svAuxiliarySourceAngle.SetObjectAttributesSet( cAttributes, SvOi::SetAttributeType::RemoveAttribute );
		m_svAuxiliaryDrawType.SetObjectAttributesSet( cAttributes, SvOi::SetAttributeType::RemoveAttribute );
		m_svAuxiliarySourceImageName.SetObjectAttributesSet( cAttributes, SvOi::SetAttributeType::RemoveAttribute );

		m_svAuxiliarySourceX.SetObjectAttributesAllowed( SvOi::SV_HIDDEN, SvOi::SetAttributeType::AddAttribute );
		m_svAuxiliarySourceY.SetObjectAttributesAllowed( SvOi::SV_HIDDEN, SvOi::SetAttributeType::AddAttribute );
		m_svAuxiliarySourceAngle.SetObjectAttributesAllowed( SvOi::SV_HIDDEN, SvOi::SetAttributeType::AddAttribute );
		m_svAuxiliaryDrawType.SetObjectAttributesAllowed( SvOi::SV_HIDDEN, SvOi::SetAttributeType::AddAttribute );
		m_svAuxiliarySourceImageName.SetObjectAttributesAllowed( SvOi::SV_HIDDEN, SvOi::SetAttributeType::AddAttribute );
	}
	return S_OK;
}

bool SVToolClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = __super::ResetObject(pErrorMessages) && ValidateLocal(pErrorMessages);

	bool l_bReset = false;
	SvOi::IInspectionProcess* pInspection = GetInspectionInterface();
	if ( nullptr != pInspection)
	{
		SvOi::IToolSet* pToolSet = dynamic_cast<SvOi::IToolSet*>(pInspection->GetToolSetInterface());
		if (nullptr != pToolSet)
		{
			if(S_OK == pToolSet->getResetCounts(l_bReset) && l_bReset )
				{
					// Reset Counter...
					m_PassedCount.SetDefaultValue(0, TRUE);
					m_FailedCount.SetDefaultValue(0, TRUE);
					m_WarnedCount.SetDefaultValue(0, TRUE);
					m_EnabledCount.SetDefaultValue(0, TRUE);
					m_ProcessedCount.SetDefaultValue(0, TRUE);
				}
		}
	}
	///UpdateBottomAndRight is called again when imageExtents are changed by ToolsizeAdjust
	if ( Result )
	{
		UpdateBottomAndRight();
	}

	// Auxiliary Extents
	BOOL bValue( false );
	const UINT cAttributes = SvOi::SV_VIEWABLE | SvOi::SV_ARCHIVABLE | SvOi::SV_SELECTABLE_FOR_EQUATION | SvOi::SV_SELECTABLE_FOR_STATISTICS | SvOi::SV_PUBLISHABLE;
	
	if( GetInspectionInterface()->GetEnableAuxiliaryExtent() )
	{
		m_svUpdateAuxiliaryExtents.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
		m_svUpdateAuxiliaryExtents.SetObjectAttributesAllowed( SvOi::SV_HIDDEN, SvOi::SetAttributeType::RemoveAttribute );
		m_svUpdateAuxiliaryExtents.GetValue( bValue );
	}
	else
	{
		m_svUpdateAuxiliaryExtents.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::RemoveAttribute );
		m_svUpdateAuxiliaryExtents.SetObjectAttributesAllowed( SvOi::SV_HIDDEN, SvOi::SetAttributeType::AddAttribute );
		m_svUpdateAuxiliaryExtents.SetObjectAttributesSet( cAttributes, SvOi::SetAttributeType::RemoveAttribute );
	}

	EnableAuxiliaryExtents( bValue ? true : false );

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
			if( SvOi::SV_NO_ATTRIBUTES != m_ExtentRight.ObjectAttributesAllowed() )
			{
				long l_lValue = static_cast<long>(l_svFigure.m_svBottomRight.m_dPositionX);

				m_ExtentRight.SetValue( l_lValue, 1 );
			}

			if( SvOi::SV_NO_ATTRIBUTES != m_ExtentBottom.ObjectAttributesAllowed() )
			{
				long l_lValue = static_cast<long>(l_svFigure.m_svBottomRight.m_dPositionY);

				m_ExtentBottom.SetValue( l_lValue, 1 );
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

SvOi::SVImageTypeEnum SVToolClass::GetImageType()
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

HRESULT SVToolClass::CollectOverlays( SVImageClass *p_Image, SVExtentMultiLineStructVector &p_MultiLineArray )
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
	SvOi::ITaskObject* pToolSet = GetInspectionInterface()->GetToolSetInterface();
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
		SVObjectReference ObjectRef = vecObjects.at(i);
		
		// Clear the existing bit
		ObjectRef.SetObjectAttributesSet( uAttributes, SvOi::SetAttributeType::RemoveAttribute );
		
		// Compare and set the attribute set if a match.
		if ( ObjectRef == refTarget )
		{
			if ( ObjectRef.ObjectAttributesAllowed() & uAttributes )
			{
				ObjectRef.SetObjectAttributesSet( uAttributes, SvOi::SetAttributeType::AddAttribute);
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
	if( m_AuxSourceImageObjectInfo.IsConnected() && m_AuxSourceImageObjectInfo.GetInputObjectInfo().m_pObject )
	{
		l_pImage = static_cast<SVImageClass*>(m_AuxSourceImageObjectInfo.GetInputObjectInfo().m_pObject);
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

		SvOi::IInspectionProcess* pInspection = GetInspectionInterface();
		if ( nullptr != pInspection )
		{
			pInspection->ForceOffsetUpdate();
		}
		UpdateAuxiliaryExtents(1);
		dynamic_cast<SVObjectClass*>(GetInspectionInterface())->resetAllObjects();
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
		0 == GetInspectionInterface()->GetEnableAuxiliaryExtent())
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

SVStringValueObjectClass* SVToolClass::GetInputImageNames( )
{
	return nullptr;
}


bool SVToolClass::SetFirstInputImageName( LPCTSTR FirstName )
{
	SVStringValueObjectClass* lstrVO = GetInputImageNames();

	if( lstrVO )
	{
		lstrVO->SetValue( FirstName, 0 );
		return true;
	}
	return false;
}



SVObjectClass* SVToolClass::GetToolComment()
{
	return &m_ToolComment;
}

bool SVToolClass::createAllObjectsFromChild( SVObjectClass& rChildObject )
{
	SVToolLevelCreateStruct createStruct;
	createStruct.OwnerObjectInfo        = this;
	createStruct.ToolObjectInfo			= this;
	createStruct.InspectionObjectInfo = dynamic_cast<SVObjectClass*>(GetInspectionInterface());

	return rChildObject.createAllObjects(createStruct);
}

void SVToolClass::connectChildObject( SVTaskObjectClass& rChildObject )
{
	SVToolLevelCreateStruct createStruct;
	createStruct.OwnerObjectInfo        = this;
	createStruct.ToolObjectInfo			= this;
	createStruct.InspectionObjectInfo	= dynamic_cast<SVObjectClass*>(GetInspectionInterface());

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
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ConditionalValue_Invalid, msgList, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
		return false;
	}
}
