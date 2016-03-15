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

#include "SVImageLibrary/SVDrawContext.h"

#include "SVAnalyzer.h"
#include "SVInspectionProcess.h"
#include "SVLine.h"
#include "SVLinearImageOperatorList.h"
#include "SVThresholdClass.h"
#include "SVUserMaskOperatorClass.h"
#include "ToolSizeAdjustTask.h"
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

	if( m_voUseProfileRotation.GetValue( l_bValue ) == S_OK )
	{
		if( l_bValue )
		{
			bOk &= m_svToolExtent.SetTranslation( SVExtentTranslationProfile ) == S_OK;
		}
		else
		{
			bOk &= m_svToolExtent.SetTranslation( SVExtentTranslationProfileShift ) == S_OK;
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
		bOk  = (S_OK == ToolSizeAdjustTask::EnsureInFriendList(this,true,true,true)); 
	}



	isCreated = bOk;

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

	if( m_voUseProfileRotation.GetValue( l_bValue ) == S_OK )
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

		if( m_svToolExtent.GetExtentPropertyInfo( SVExtentPropertyRotationAngle, info ) == S_OK )
		{
			info.bHidden = ! l_bValue;

			if( m_svToolExtent.SetExtentPropertyInfo( SVExtentPropertyRotationAngle, info ) != S_OK )
			{
				l_hrOk = S_FALSE;
			}
		}
		else
		{
			l_hrOk = S_FALSE;
		}
	}

	if( SVToolClass::ResetObject() != S_OK )
	{
		l_hrOk = S_FALSE;
	}

	if ( l_hrOk == S_OK )
	{
		SVImageExtentClass l_svExtents;

		if ( GetImageExtent( l_svExtents ) == S_OK )
		{
			SVExtentFigureStruct l_svFigure;

			if ( l_svExtents.GetFigure( l_svFigure ) == S_OK )
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

	if( l_hrOk == S_OK )
	{
		l_hrOk = SVToolClass::SetImageExtent( p_ulIndex, l_NewExtent );
	}
	return l_hrOk;
}

SVTaskObjectClass* SVLinearToolClass::GetObjectAtPoint( const SVExtentPointStruct &p_rsvPoint )
{
	SVImageExtentClass l_svExtents;

	SVTaskObjectClass *l_psvObject = NULL;

	if( m_svToolExtent.GetImageExtent( l_svExtents ) == S_OK &&
	    l_svExtents.GetLocationPropertyAt( p_rsvPoint ) != SVExtentLocationPropertyUnknown )
	{
		l_psvObject = this;
	}

	return l_psvObject;
}

bool SVLinearToolClass::DoesObjectHaveExtents() const
{
	return true;
}

CString SVLinearToolClass::GetProfileOrientation()
{
	CString sRet;

	m_voProfileOrientation.GetValue(sRet);

	return sRet;
}

BOOL SVLinearToolClass::GetRotation()
{
	BOOL bVal = FALSE;
	
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
	BOOL bRet = FALSE;
	SVImageExtentClass svImageExtent;
	double dRotationAngle;

	GetImageExtent(svImageExtent);
	svImageExtent.GetExtentProperty(SVExtentPropertyRotationAngle,dRotationAngle);

	if ( dRotationAngle != 0 )
	{
		bRet = TRUE;
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

DWORD_PTR SVLinearToolClass::processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext )
{
	DWORD_PTR DwResult = NULL;

	// Try to process message by yourself...
	DWORD dwPureMessageID = DwMessageID & SVM_PURE_MESSAGE;

	return( SVToolClass::processMessage( DwMessageID, DwMessageValue, DwMessageContext ) | DwResult );
}
#pragma endregion Protected Methods

#pragma region Private Methods
void SVLinearToolClass::init()
{
	// Set up your type...
	outObjectInfo.ObjectTypeInfo.ObjectType = SVToolObjectType;
	outObjectInfo.ObjectTypeInfo.SubType    = SVLinearToolObjectType;

	// Register Embedded Objects
	RegisterEmbeddedObject( &m_svRotationAngle, SVRotationAngleObjectGuid, IDS_OBJECTNAME_ROTATION_ANGLE, false, SVResetItemTool, _T("Extent Angle") );
	RegisterEmbeddedObject( &m_svRotationPointX, SVRotationPointXObjectGuid, IDS_OBJECTNAME_ROTATION_POINT_X, false, SVResetItemTool, _T("Extent X") );
	RegisterEmbeddedObject( &m_svRotationPointY, SVRotationPointYObjectGuid, IDS_OBJECTNAME_ROTATION_POINT_Y, false, SVResetItemTool, _T("Extent Y") );
	RegisterEmbeddedObject( &m_voProfileOrientation, SVProfileOrientationGuid,IDS_OBJECTNAME_LINEARTOOL_ORIENTATION, false, SVResetItemTool );
	RegisterEmbeddedObject( &m_voUseProfileRotation, SVLinearToolUseRotationGuid, IDS_OBJECTNAME_LINEAR_TOOL_USE_ROTATION, false, SVResetItemTool, _T("Extent Angle") );
	RegisterEmbeddedObject( &m_svSourceImageNames, SVSourceImageNamesGuid, IDS_OBJECTNAME_SOURCE_IMAGE_NAMES, false, SVResetItemTool );

	// Set Embedded defaults
	m_svRotationAngle.SetDefaultValue( 0.0, TRUE );
	m_svRotationPointX.SetDefaultValue( SV_DEFAULT_WINDOWTOOL_LEFT, TRUE );
	m_svRotationPointY.SetDefaultValue( ( SV_DEFAULT_WINDOWTOOL_TOP + ( SV_DEFAULT_WINDOWTOOL_HEIGHT / 2 ) ), TRUE );
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
	analyzerClassInfo.ObjectTypeInfo.ObjectType = SVAnalyzerObjectType;
	analyzerClassInfo.ObjectTypeInfo.SubType	= SVLinearPixelCountingAnalyzerObjectType;
	analyzerClassInfo.ClassId = SVLinearPixelCountingLineAnalyzerClassGuid;
	analyzerClassInfo.ClassName.LoadString( IDS_CLASSNAME_SVLINEARPIXELCOUNTINGLINEANALYZER );
	availableChildren.Add( analyzerClassInfo );

	// Add the Line Edge Count Analyzer...
	analyzerClassInfo.ObjectTypeInfo.ObjectType = SVAnalyzerObjectType;
	analyzerClassInfo.ObjectTypeInfo.SubType	= SVLinearEdgeCountingAnalyzerObjectType;
	analyzerClassInfo.ClassId = SVLinearEdgeCountingLineAnalyzerClassGuid;
	analyzerClassInfo.ClassName.LoadString( IDS_CLASSNAME_SVLINEAREDGECOUNTINGLINEANALYZER );
	availableChildren.Add(analyzerClassInfo);
	
	// Add the Line Edge Position Analyzer...
	analyzerClassInfo.ObjectTypeInfo.ObjectType = SVAnalyzerObjectType;
	analyzerClassInfo.ObjectTypeInfo.SubType	= SVLinearEdgePositionAnalyzerObjectType;
	analyzerClassInfo.ClassId = SVLinearEdgePositionLineAnalyzerClassGuid;
	analyzerClassInfo.ClassName.LoadString( IDS_CLASSNAME_SVLINEAREDGEPOSITIONLINEANALYZER );
	availableChildren.Add(analyzerClassInfo);

	// Add the Line Linear Measurement Analyzer...
	analyzerClassInfo.ObjectTypeInfo.ObjectType = SVAnalyzerObjectType;
	analyzerClassInfo.ObjectTypeInfo.SubType	= SVLinearMeasurementAnalyzerObjectType;
	analyzerClassInfo.ClassId = SVLinearMeasurementAnalyzerClassGuid;
	analyzerClassInfo.ClassName.LoadString( IDS_CLASSNAME_SVLINEARMEASUREMENTLINEANALYZER );
	availableChildren.Add(analyzerClassInfo);

	// Add the Line Maximum Foreground Object Analyzer...
	analyzerClassInfo.ObjectTypeInfo.ObjectType = SVAnalyzerObjectType;
	analyzerClassInfo.ObjectTypeInfo.SubType	= SVLinearMaximumForegroundObjectAnalyzerObjectType;
	analyzerClassInfo.ClassId = SVLinearMaximumForegroundObjectLineAnalyzerClassGuid;
	analyzerClassInfo.ClassName.LoadString( IDS_CLASSNAME_SVLINEARMAXIMUMFOREGROUNDOBJECTLINEANALYZER );
	availableChildren.Add(analyzerClassInfo);

	// Add the Line Maximum Background Object Analyzer...
	analyzerClassInfo.ObjectTypeInfo.ObjectType = SVAnalyzerObjectType;
	analyzerClassInfo.ObjectTypeInfo.SubType	= SVLinearMaximumBackgroundObjectAnalyzerObjectType;
	analyzerClassInfo.ClassId = SVLinearMaximumBackgroundObjectLineAnalyzerClassGuid;
	analyzerClassInfo.ClassName.LoadString( IDS_CLASSNAME_SVLINEARMAXIMUMBACKGROUNDOBJECTLINEANALYZER );
	availableChildren.Add(analyzerClassInfo);

	// Add the Line Maximum Object Analyzer...
	analyzerClassInfo.ObjectTypeInfo.ObjectType = SVAnalyzerObjectType;
	analyzerClassInfo.ObjectTypeInfo.SubType	= SVLinearMaximumObjectAnalyzerObjectType;
	analyzerClassInfo.ClassId = SVLinearMaximumObjectLineAnalyzerClassGuid;
	analyzerClassInfo.ClassName.LoadString( IDS_CLASSNAME_SVLINEARMAXIMUMOBJECTLINEANALYZER );
	availableChildren.Add(analyzerClassInfo);

	// Build an operator list...

	// ...use In Place image operator list, because we already have an output image! SEJ_24Mar2000
//	SVUnaryImageOperatorListClass* pOperatorList = new SVInPlaceImageOperatorListClass;
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

