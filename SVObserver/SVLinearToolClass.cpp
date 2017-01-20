// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVLinearToolClass
// * .File Name       : $Workfile:   SVLinearToolClass.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.4  $
// * .Check In Date   : $Date:   15 May 2014 13:12:48  $
// ******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVLinearToolClass.h"
#include "SVAnalyzer.h"
#include "SVInspectionProcess.h"
#include "SVLinearImageOperatorList.h"
#include "SVThresholdClass.h"
#include "SVUserMaskOperatorClass.h"
#include "ToolSizeAdjustTask.h"
#include "ObjectInterfaces/GlobalConst.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SV_IMPLEMENT_CLASS( SVLinearToolClass, SVLinearToolClassGuid );
#pragma endregion Declarations

#pragma region Constructor
SVLinearToolClass::SVLinearToolClass( BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID )
: SVToolClass( BCreateDefaultTaskList, POwner, StringResourceID )
{
	init();
}

SVLinearToolClass::~SVLinearToolClass()
{
	CloseObject();
}
#pragma endregion Constructor

#pragma region Public Methods
BOOL SVLinearToolClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = FALSE;

	bOk = SVToolClass::CreateObject( PCreateStructure );

	BOOL l_bValue = FALSE;

	if( S_OK == m_voUseProfileRotation.GetValue( l_bValue ) )
	{
		if( l_bValue )
		{
			bOk &= S_OK == m_svToolExtent.SetTranslation( SVExtentTranslationProfile );
		}
		else
		{
			bOk &= S_OK == m_svToolExtent.SetTranslation( SVExtentTranslationProfileShift );
		}
	}

	m_svRotationAngle.ObjectAttributesAllowedRef() |= SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE | SV_PRINTABLE; 
	m_svRotationPointX.ObjectAttributesAllowedRef() |= SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE | SV_PRINTABLE;
	m_svRotationPointY.ObjectAttributesAllowedRef() |= SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE | SV_PRINTABLE;
	m_voProfileOrientation.ObjectAttributesAllowedRef() |= SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE | SV_PRINTABLE;
	m_voUseProfileRotation.ObjectAttributesAllowedRef() |= SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE | SV_PRINTABLE;

	m_svSourceImageNames.ObjectAttributesAllowedRef() &=~SV_REMOTELY_SETABLE & ~SV_SETABLE_ONLINE;

	extentLeft.ObjectAttributesAllowedRef() &= ~SV_REMOTELY_SETABLE & ~SV_SETABLE_ONLINE;
	extentTop.ObjectAttributesAllowedRef() &= ~SV_REMOTELY_SETABLE & ~SV_SETABLE_ONLINE;


	if(bOk)
	{
		bOk  = (S_OK == ToolSizeAdjustTask::EnsureInFriendList(this, true, true, true)); 
	}

	m_isCreated = bOk;

	return bOk;
}

BOOL SVLinearToolClass::CloseObject()
{
	BOOL bRetVal =  SVToolClass::CloseObject();

	return bRetVal;
}

HRESULT SVLinearToolClass::ResetObject()
{
	HRESULT l_hrOk = S_OK;

	long l_lValue = 0;
	BOOL l_bValue = FALSE;

	if( S_OK == m_voUseProfileRotation.GetValue( l_bValue ) )
	{
		long l_DataIndex = m_voUseProfileRotation.GetLastSetIndex();

		if( l_bValue )
		{
			if( m_svToolExtent.GetTranslation() != SVExtentTranslationProfile )
			{
				m_voProfileOrientation.SetValue( l_DataIndex, "Horizontal" );

				l_hrOk = m_svToolExtent.SetTranslation( SVExtentTranslationProfile, l_DataIndex );
			}
		}
		else
		{
			if( m_svToolExtent.GetTranslation() != SVExtentTranslationProfileShift )
			{
				m_svRotationAngle.SetValue( l_DataIndex, 0.0 );

				l_hrOk = m_svToolExtent.SetTranslation( SVExtentTranslationProfileShift, l_DataIndex );
			}
		}

		SVExtentPropertyInfoStruct info;

		if( S_OK == m_svToolExtent.GetExtentPropertyInfo( SVExtentPropertyRotationAngle, info ) )
		{
			info.bHidden = ! l_bValue;

			if( S_OK != m_svToolExtent.SetExtentPropertyInfo( SVExtentPropertyRotationAngle, info ) )
			{
				l_hrOk = S_FALSE;
			}
		}
		else
		{
			l_hrOk = S_FALSE;
		}
	}

	if( S_OK != SVToolClass::ResetObject() )
	{
		l_hrOk = S_FALSE;
	}

	if ( S_OK == l_hrOk )
	{
		SVImageExtentClass l_svExtents;

		if ( S_OK == GetImageExtent( l_svExtents ) )
		{
			SVExtentFigureStruct l_svFigure;

			if ( S_OK == l_svExtents.GetFigure( l_svFigure ) )
			{
				if( extentLeft.ObjectAttributesAllowed() != SV_NO_ATTRIBUTES )
				{
					extentLeft.SetValue( 1, l_svFigure.m_svTopLeft.m_dPositionX );
				}
				
				if( extentTop.ObjectAttributesAllowed() != SV_NO_ATTRIBUTES )
				{
					extentTop.SetValue( 1, l_svFigure.m_svTopLeft.m_dPositionY );
				}
			}
		}
		else
		{
			l_hrOk = S_FALSE;
		}
	}

	UpdateImageWithExtent( 1 );

	return l_hrOk;
}

HRESULT SVLinearToolClass::SetImageExtentToParent( unsigned long p_ulIndex )
{
	HRESULT l_hrOk = S_OK;
	SVImageExtentClass l_NewExtent;

	l_hrOk = m_svToolExtent.UpdateExtentToParentExtents( p_ulIndex, l_NewExtent );

	if( S_OK == l_hrOk )
	{
		l_hrOk = SVToolClass::SetImageExtent( p_ulIndex, l_NewExtent );
	}
	return l_hrOk;
}

SVTaskObjectClass* SVLinearToolClass::GetObjectAtPoint( const SVExtentPointStruct &p_rsvPoint )
{
	SVImageExtentClass l_svExtents;

	SVTaskObjectClass *l_psvObject = nullptr;

	if( S_OK == m_svToolExtent.GetImageExtent( l_svExtents ) &&
	    SVExtentLocationPropertyUnknown != l_svExtents.GetLocationPropertyAt( p_rsvPoint ) )
	{
		l_psvObject = this;
	}

	return l_psvObject;
}

bool SVLinearToolClass::DoesObjectHaveExtents() const
{
	return true;
}

SVString SVLinearToolClass::GetProfileOrientation()
{
	SVString Result;

	m_voProfileOrientation.GetValue( Result );

	return Result;
}

BOOL SVLinearToolClass::GetRotation()
{
	BOOL bVal = false;
	
	m_voUseProfileRotation.GetValue(bVal);

	return bVal;
}

EAutoSize SVLinearToolClass::GetAutoSizeEnabled()
{
	if (GetRotation() == TRUE)
	{
		return EnableNone;
	}
	else
	{
		return EnableSizeAndPosition;
	}
}

BOOL SVLinearToolClass::IsToolRotated()
{
	BOOL bRet = false;
	SVImageExtentClass svImageExtent;
	double dRotationAngle;

	GetImageExtent(svImageExtent);
	svImageExtent.GetExtentProperty(SVExtentPropertyRotationAngle,dRotationAngle);

	if ( dRotationAngle != 0 )
	{
		bRet = true;
	}
	return bRet;
}

SVStaticStringValueObjectClass* SVLinearToolClass::GetInputImageNames()
{
	return &m_svSourceImageNames;
}
#pragma endregion Public Methods

#pragma region Protected Methods
BOOL SVLinearToolClass::onRun( SVRunStatusClass& RRunStatus )
{
	return SVToolClass::onRun( RRunStatus );
}
#pragma endregion Protected Methods

#pragma region Private Methods
void SVLinearToolClass::init()
{
	// Set up your type...
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVToolObjectType;
	m_outObjectInfo.ObjectTypeInfo.SubType    = SVLinearToolObjectType;

	// Register Embedded Objects
	RegisterEmbeddedObject( &m_svRotationAngle, SVRotationAngleObjectGuid, IDS_OBJECTNAME_ROTATION_ANGLE, false, SVResetItemTool, _T("Extent Angle") );
	RegisterEmbeddedObject( &m_svRotationPointX, SVRotationPointXObjectGuid, IDS_OBJECTNAME_ROTATION_POINT_X, false, SVResetItemTool, _T("Extent X") );
	RegisterEmbeddedObject( &m_svRotationPointY, SVRotationPointYObjectGuid, IDS_OBJECTNAME_ROTATION_POINT_Y, false, SVResetItemTool, _T("Extent Y") );
	RegisterEmbeddedObject( &m_voProfileOrientation, SVProfileOrientationGuid,IDS_OBJECTNAME_LINEARTOOL_ORIENTATION, false, SVResetItemTool );
	RegisterEmbeddedObject( &m_voUseProfileRotation, SVLinearToolUseRotationGuid, IDS_OBJECTNAME_LINEAR_TOOL_USE_ROTATION, false, SVResetItemTool, _T("Extent Angle") );
	RegisterEmbeddedObject( &m_svSourceImageNames, SVSourceImageNamesGuid, IDS_OBJECTNAME_SOURCE_IMAGE_NAMES, false, SVResetItemTool );

	// Set Embedded defaults
	m_svRotationAngle.SetDefaultValue( 0.0, TRUE );
	m_svRotationPointX.SetDefaultValue( SvOi::cDefaultWindowToolLeft, TRUE );
	m_svRotationPointY.SetDefaultValue( ( SvOi::cDefaultWindowToolTop + ( SvOi::cDefaultWindowToolHeight / 2 ) ), TRUE );
	m_voProfileOrientation.SetEnumTypes(g_strOrientationEnums);
	m_voProfileOrientation.SetDefaultValue( "Horizontal", TRUE );
	m_voUseProfileRotation.SetDefaultValue(TRUE,TRUE);

	m_svToolExtent.SetImageType( SVImageTypePhysical );
	m_svToolExtent.SetTranslation( SVExtentTranslationProfile );
	m_svToolExtent.SetExtentObject( SVExtentPropertyPositionPointX, &m_svRotationPointX );
	m_svToolExtent.SetExtentObject( SVExtentPropertyPositionPointY, &m_svRotationPointY );
	m_svToolExtent.SetExtentObject( SVExtentPropertyRotationAngle, &m_svRotationAngle );

	// Populate the available analyzer list
	SVClassInfoStruct analyzerClassInfo;

	// Add the Line Pixel Count Analyzer...
	analyzerClassInfo.m_ObjectTypeInfo.ObjectType = SVAnalyzerObjectType;
	analyzerClassInfo.m_ObjectTypeInfo.SubType	= SVLinearPixelCountingAnalyzerObjectType;
	analyzerClassInfo.m_ClassId = SVLinearPixelCountingLineAnalyzerClassGuid;
	analyzerClassInfo.m_ClassName = SvUl_SF::LoadSVString( IDS_CLASSNAME_SVLINEARPIXELCOUNTINGLINEANALYZER );
	m_availableChildren.Add( analyzerClassInfo );

	// Add the Line Edge Count Analyzer...
	analyzerClassInfo.m_ObjectTypeInfo.ObjectType = SVAnalyzerObjectType;
	analyzerClassInfo.m_ObjectTypeInfo.SubType	= SVLinearEdgeCountingAnalyzerObjectType;
	analyzerClassInfo.m_ClassId = SVLinearEdgeCountingLineAnalyzerClassGuid;
	analyzerClassInfo.m_ClassName = SvUl_SF::LoadSVString( IDS_CLASSNAME_SVLINEAREDGECOUNTINGLINEANALYZER );
	m_availableChildren.Add(analyzerClassInfo);
	
	// Add the Line Edge Position Analyzer...
	analyzerClassInfo.m_ObjectTypeInfo.ObjectType = SVAnalyzerObjectType;
	analyzerClassInfo.m_ObjectTypeInfo.SubType	= SVLinearEdgePositionAnalyzerObjectType;
	analyzerClassInfo.m_ClassId = SVLinearEdgePositionLineAnalyzerClassGuid;
	analyzerClassInfo.m_ClassName = SvUl_SF::LoadSVString( IDS_CLASSNAME_SVLINEAREDGEPOSITIONLINEANALYZER );
	m_availableChildren.Add(analyzerClassInfo);

	// Add the Line Linear Measurement Analyzer...
	analyzerClassInfo.m_ObjectTypeInfo.ObjectType = SVAnalyzerObjectType;
	analyzerClassInfo.m_ObjectTypeInfo.SubType	= SVLinearMeasurementAnalyzerObjectType;
	analyzerClassInfo.m_ClassId = SVLinearMeasurementAnalyzerClassGuid;
	analyzerClassInfo.m_ClassName = SvUl_SF::LoadSVString( IDS_CLASSNAME_SVLINEARMEASUREMENTLINEANALYZER );
	m_availableChildren.Add(analyzerClassInfo);

	// Add the Line Maximum Foreground Object Analyzer...
	analyzerClassInfo.m_ObjectTypeInfo.ObjectType = SVAnalyzerObjectType;
	analyzerClassInfo.m_ObjectTypeInfo.SubType	= SVLinearMaximumForegroundObjectAnalyzerObjectType;
	analyzerClassInfo.m_ClassId = SVLinearMaximumForegroundObjectLineAnalyzerClassGuid;
	analyzerClassInfo.m_ClassName = SvUl_SF::LoadSVString( IDS_CLASSNAME_SVLINEARMAXIMUMFOREGROUNDOBJECTLINEANALYZER );
	m_availableChildren.Add(analyzerClassInfo);

	// Add the Line Maximum Background Object Analyzer...
	analyzerClassInfo.m_ObjectTypeInfo.ObjectType = SVAnalyzerObjectType;
	analyzerClassInfo.m_ObjectTypeInfo.SubType	= SVLinearMaximumBackgroundObjectAnalyzerObjectType;
	analyzerClassInfo.m_ClassId = SVLinearMaximumBackgroundObjectLineAnalyzerClassGuid;
	analyzerClassInfo.m_ClassName = SvUl_SF::LoadSVString( IDS_CLASSNAME_SVLINEARMAXIMUMBACKGROUNDOBJECTLINEANALYZER );
	m_availableChildren.Add(analyzerClassInfo);

	// Add the Line Maximum Object Analyzer...
	analyzerClassInfo.m_ObjectTypeInfo.ObjectType = SVAnalyzerObjectType;
	analyzerClassInfo.m_ObjectTypeInfo.SubType	= SVLinearMaximumObjectAnalyzerObjectType;
	analyzerClassInfo.m_ClassId = SVLinearMaximumObjectLineAnalyzerClassGuid;
	analyzerClassInfo.m_ClassName = SvUl_SF::LoadSVString( IDS_CLASSNAME_SVLINEARMAXIMUMOBJECTLINEANALYZER );
	m_availableChildren.Add(analyzerClassInfo);

	// Build an operator list...

	// ...use In Place image operator list, because we already have an output image!
	SVUnaryImageOperatorListClass* pOperatorList = new SVLinearImageOperatorListClass;

	// Operator list defaults:
	if( pOperatorList )
	{
		// Requires a SVThresholdClass Object
		pOperatorList->Add( new SVThresholdClass( pOperatorList ) );

		// and Requires a SVUsermaskOperatorClass Object
		pOperatorList->Add( new SVUserMaskOperatorClass( pOperatorList ) );

		// Add the UnaruyImageOperatorList to the Tool's List
		Add( pOperatorList );
	}
	ToolSizeAdjustTask::AddToFriendlist(this, true,true,true);

	addDefaultInputObjects();
}

BOOL SVLinearToolClass::IsValid()
{
	BOOL bValid = TRUE;

	ToolSizeAdjustTask* pToolSizeAdjustTask = nullptr;
	pToolSizeAdjustTask = ToolSizeAdjustTask::GetToolSizeAdjustTask(this);
	if(nullptr != pToolSizeAdjustTask)
	{
			bValid =  pToolSizeAdjustTask->OnValidate();
	}

	return SVToolClass::IsValid() & bValid ;
}

#pragma endregion Private Methods

